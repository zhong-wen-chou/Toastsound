#include "editwindow.h"
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include "song.h"
#include <QScrollArea>
#include <QSlider>
#include <QtConcurrent/QtConcurrentRun>
#include <QThread>
#include <QGroupBox>
#include <QStackedWidget>
#include <QSpinBox>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <Qpainter>

const int NOTE_WIDTH = 20;
const int TRACK_SPACING = 20;
const int BASE_Y = 20;
const QColor WHITE_KEY_COLOR = Qt::white;
const QColor BLACK_KEY_COLOR = Qt::black;

//class Note;

// NoteCanvas 实现
NoteCanvas::NoteCanvas(QWidget *parent) : QWidget(parent) {
    edittimer.start();

    refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, QOverload<>::of(&NoteCanvas::update));
    refreshTimer->start(16);  // 大约 60FPS
}
/*
void NoteCanvas::setNotes(const QVector<std::tuple<int, int>>& notes) {
    m_notes = notes;
    update();
}*/

void NoteCanvas::addNote(int keyIndex, qint64 startTime) {
    NoteBlocks.append({keyIndex, startTime, -1, true});
    //lastNotePosition = position + NOTE_WIDTH + 5;
    //update();
    //updateCanvasSize();
}

void NoteCanvas::releaseNote(int keyIndex, qint64 endTime) {
    for (NoteBlock &note : NoteBlocks) {
        if (note.pitch == keyIndex && note.active) {
            note.endTime = endTime;
            note.active = false;
            break;
        }
    }
}

qint64 NoteCanvas::currentTime() const {
    return edittimer.elapsed();
}


void NoteCanvas::clearNotes() {
    NoteBlocks.clear();
    lastNotePosition = 0;
    update();
}

int NoteCanvas::getLastNotePosition() const {
    return lastNotePosition;
}
/*
void NoteCanvas::updateCanvasSize() {
    if (m_notes.isEmpty()) {
        setMinimumSize(0, 800);
        return;
    }

    // 计算最大X坐标
    int maxX = 0;
    for (const auto &[keyIndex, x] : m_notes) {
        maxX = qMax(maxX, x + NOTE_WIDTH);
    }

    // 设置画布大小
    setMinimumSize(maxX + 100, 800);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}*/

void NoteCanvas::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制背景
    painter.fillRect(rect(), QColor("#f8f8f8"));
    qint64 currentTime = edittimer.elapsed();
    // 绘制音符
    for (const auto &note : NoteBlocks) {
        qint64 start = note.startTime;
        qint64 end = note.active ? currentTime : note.endTime;
        if (end < currentTime - viewDuration)
            continue;
        int keyIndex = note.pitch;
        int x_start;
        int x_end;
        if(currentTime<5000.0){
            x_start = (currentTime/5000.0)*0.618*width() - (currentTime - start) * 0.3;
            x_end = (currentTime/5000.0)*0.618*width() - (currentTime - end) * 0.3;
        }
        else{
            x_start = 0.618*width() - (currentTime - start) * 0.3;
            x_end = 0.618*width() - (currentTime - end) * 0.3;
        }
        //int x = std::get<1>(note);
        int y = getNoteY(keyIndex);

        //QColor noteColor = (keyIndex < 28) ? QColor("#2196F3") : QColor("#888888");

        int height = 40;
        //int width = (keyIndex < 28) ? 20 : 16;
        int width=qMax(1, x_end - x_start);
        painter.setBrush(note.active ? QColor("#00BCD4") : QColor("#2196F3"));
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(x_start, y, width, height, 4, 4);
    }
    // 当前时间线
    //painter.setPen(QColor("#FF5252"));

    if(currentTime<5000.0){
        QPen pen(Qt::red);       // 线条颜色
        pen.setWidth(4);           // 设置线条宽度为 4 像素
        painter.setPen(pen);       // 将画笔设置给 painter
        painter.drawLine((currentTime/5000.0)*0.618*width(), 0, (currentTime/5000.0)*0.618*width(), height());
    }
    else {
        QPen pen(Qt::black);       // 线条颜色
        pen.setWidth(4);           // 设置线条宽度为 4 像素
        painter.setPen(pen);       // 将画笔设置给 painter
        painter.drawLine(0.618*width(), 0, 0.618*width(), height());
    }
}

int NoteCanvas::getNoteY(int keyIndex) const {
    return height()-(30 + (keyIndex % 36) * 20);
}

EditWindow::EditWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("编辑界面");
    setMinimumSize(1500, 800);
    score = new Score();
    currentTrackIndex = 0;
    isopenme = false;
    selfbpm = 120;
    initvolumn = 70;

    createWidgets();
    setupLayout();
    connectSignals();
    /*
    if (score && score->gettracksnum() == 0) {
        score->addTrack(Track());
        // 为第一个音轨创建画布
        NoteCanvas* canvas = new NoteCanvas();
        canvases.append(canvas);
        stackedWidget->addWidget(canvas);
        updateTrackList();
    }
*/
}

EditWindow::~EditWindow()
{
    delete pianoKeys;
    delete trackList;
    delete score;
    // 清理所有画布
    qDeleteAll(canvases);
}

void EditWindow::createWidgets()
{
    // 左侧音轨管理面板
    QWidget* trackPanel = new QWidget(this);
    trackPanel->setFixedWidth(200);
    trackPanel->setStyleSheet("background-color: #f0f0f0;");

    // 音轨列表
    trackList = new QListWidget(trackPanel);
    trackList->setStyleSheet("font-size: 12px;");

    // 音轨操作按钮
    addTrackButton = new QPushButton("添加音轨", trackPanel);
    removeTrackButton = new QPushButton("删除音轨", trackPanel);
    addTrackButton->setStyleSheet("background-color: #4CAF50; color: white;");
    removeTrackButton->setStyleSheet("background-color: #FF4444; color: white;");

    // 音轨控制面板布局
    QVBoxLayout* trackLayout = new QVBoxLayout(trackPanel);
    trackLayout->addWidget(new QLabel("音轨管理", trackPanel));
    trackLayout->addWidget(trackList);
    trackLayout->addWidget(addTrackButton);
    trackLayout->addWidget(removeTrackButton);

    // 节拍器控制
    QGroupBox *tempoGroup = new QGroupBox("节奏", this);
    QHBoxLayout *tempoLayout = new QHBoxLayout(tempoGroup);
    QLabel *tempoLabel = new QLabel("120 BPM", this);
    QSlider *tempoSlider = new QSlider(Qt::Horizontal, this);
    tempoSlider->setRange(40, 200);
    tempoSlider->setValue(120);
    tempoLayout->addWidget(new QLabel("慢", this));
    tempoLayout->addWidget(tempoSlider);
    tempoLayout->addWidget(new QLabel("快", this));
    tempoLayout->addWidget(tempoLabel);
    trackLayout->addWidget(tempoGroup);

    QGroupBox *volumeGroup = new QGroupBox("音量", this);
    QHBoxLayout *volumeLayout = new QHBoxLayout(volumeGroup);
    QLabel *volumeLabel = new QLabel("70%", this);
    QSlider *volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(70);
    volumeLayout->addWidget(new QLabel("静音", this));
    volumeLayout->addWidget(volumeSlider);
    volumeLayout->addWidget(new QLabel("最大", this));
    volumeLayout->addWidget(volumeLabel);
    trackLayout->addWidget(volumeGroup);

    QPushButton *metronomeButton = new QPushButton("节拍器", this);
    trackLayout->addWidget(metronomeButton);

    // 使用堆栈窗口管理多个画布
    stackedWidget = new QStackedWidget(this);

    // 底部钢琴键
    pianoKeys = new PianoKeys(this);
    pianoKeys->setKeyPressCallback([this](int keyIndex) {
        onPianoKeyPressed(keyIndex);
        updateNoteVisual(keyIndex, true);
    });
    pianoKeys->setKeyReleaseCallback([this](int keyIndex,int midinum) {
        onPianoKeyReleased(keyIndex,midinum);
    });

    // 底部按钮
    exitButton = new QPushButton("退出主界面", this);
    playButton = new QPushButton("开始录制", this);
    saveButton = new QPushButton("保存乐谱", this);
    exitButton->setStyleSheet("background-color: #FF4444; color: white;");
    playButton->setStyleSheet("background-color: #4CAF50; color: white;");
    saveButton->setStyleSheet("background-color: #2196F3; color: white;");

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout();
    QHBoxLayout *topLayout = new QHBoxLayout();

    topLayout->addWidget(trackPanel);

    // 将堆栈窗口放入滚动区域
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidget(stackedWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    topLayout->addWidget(scrollArea, 1);

    // 底部按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(20, 10, 20, 20);
    buttonLayout->addWidget(exitButton);
    buttonLayout->addWidget(playButton);
    buttonLayout->addWidget(saveButton);

    mainLayout->addLayout(topLayout, 1);
    mainLayout->addWidget(pianoKeys);
    mainLayout->addLayout(buttonLayout);

    QWidget *centralWidget = new QWidget();
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    // 连接滑块信号
    connect(tempoSlider, &QSlider::valueChanged, [tempoLabel, this](int value) {
        tempoLabel->setText(QString("%1 BPM").arg(value));
        selfbpm = value;
    });

    connect(volumeSlider, &QSlider::valueChanged, [volumeLabel, this](int value) {
        volumeLabel->setText(QString("%1%").arg(value));
        initvolumn = value;
    });

    connect(metronomeButton, &QPushButton::clicked, this, &EditWindow::startmebutton_clicked);
}

void EditWindow::setupLayout()
{
    centralWidget()->layout()->setContentsMargins(10, 10, 10, 10);
}

void EditWindow::connectSignals()
{
    connect(exitButton, &QPushButton::clicked, this, &EditWindow::exitToMain);
    connect(playButton, &QPushButton::clicked, this, &EditWindow::startPlayback);
    connect(saveButton, &QPushButton::clicked, this, &EditWindow::saveScore);

    connect(addTrackButton, &QPushButton::clicked, this, &EditWindow::addTrack);
    connect(removeTrackButton, &QPushButton::clicked, this, &EditWindow::removeCurrentTrack);
    connect(trackList, &QListWidget::currentRowChanged, this, &EditWindow::switchTrack);
}


void EditWindow::onPianoKeyPressed(int keyIndex)
{
    if (!score || currentTrackIndex >= canvases.size()) return;

    // 获取当前音轨的画布
    NoteCanvas* currentCanvas = canvases[currentTrackIndex];
    /*
    if(currentCanvas->ispause){
        currentCanvas->ispause=false;
        currentCanvas->edittimer.restart();
        currentCanvas->clearNotes();
    }*/
    // 获取当前画布的音符位置
    //int position = currentCanvas->getLastNotePosition();

    // 添加音符到当前画布
    currentCanvas->addNote(keyIndex, currentCanvas->currentTime());
}


void EditWindow::onPianoKeyReleased(int keyIndex,int midinum)
{
    NoteCanvas* currentCanvas = canvases[currentTrackIndex];
    currentCanvas->releaseNote(keyIndex, currentCanvas->currentTime());
    /*
        if(currentCanvas->currentTime()<5000.0){
        currentCanvas->ispause=true;
        currentCanvas->tmptime=currentCanvas->currentTime();
    }*/
    updateNoteVisual(keyIndex, false);
}

void EditWindow::updateNoteVisual(int keyIndex, bool isPressed)
{
    if (keyIndex < 28) {
        QPushButton *key = pianoKeys->whiteKeys[keyIndex];
        key->setEnabled(!isPressed);
    } else {
        int blackIndex = keyIndex - 28;
        if (blackIndex >= 0 && blackIndex < pianoKeys->blackKeys.size()) {
            QPushButton *key = pianoKeys->blackKeys[blackIndex];
            key->setEnabled(!isPressed);
        }
    }
}

void EditWindow::exitToMain()
{
    if (parentWidget()) {
        parentWidget()->show();
    }
    this->close();
    //以防万一，恢复通道0音色
    std::vector<unsigned char> programChange;
    programChange.push_back(0xC0 | 0); // Program Change
    programChange.push_back(0);
    midiOut.sendMessage(&programChange);
}

void EditWindow::startPlayback()
{
    if (!isRecording) {
        // 开始录制
        isRecording = true;
        playButton->setText("结束录制");
        playButton->setStyleSheet("background-color: #FF4444; color: white;");

        qDebug() << "开始录制";
        isstart = true;
        keyLogs.clear();
        qDebug() << "计时器已启动";
        timer.restart();
    } else {
        // 已经是录制状态，调用停止录制
        stopRecording();
    }
}

void EditWindow::stopRecording()
{
    if (isRecording) {
        // 结束录制
        isRecording = false;
        playButton->setText("开始录制");
        playButton->setStyleSheet("background-color: #4CAF50; color: white;");

        qDebug() << "结束录制";
        isstart = false;

        // 保存录制的数据
        QVector<std::tuple<std::string, qint64, qint64>> sortedLogs = keyLogs;
        std::sort(sortedLogs.begin(), sortedLogs.end(), [](const auto& a, const auto& b) {
            return std::get<1>(a) < std::get<1>(b); // 按开始时间排序
        });


        double quarterNoteMs = 60000.0 / initbpm;
        qint64 lastEndTime = 0;
        Track& a=score->gettrackbyn(currentTrackIndex);

        for (const auto& log : sortedLogs) {
            const std::string& pitch = std::get<0>(log);
            qint64 start = std::get<2>(log);
            qint64 end = std::get<1>(log);

            // 如果两个音符之间有空隙，加休止符
            if (start > lastEndTime) {
                double restDuration = (start - lastEndTime) / quarterNoteMs;
                if (restDuration >= 0.05) { // 可忽略极短休止
                    a.addNote(new Rest(restDuration));
                }
            }

            // 写音符
            double noteDuration = (end - start) / quarterNoteMs;
            a.addNote(new Note(MidiNote::noteNameToMidi(pitch),noteDuration));
            lastEndTime = end;
        }
    }
}

void EditWindow::saveScore()
{
    qDebug() << "保存乐谱到文件";
    QString filePath = QFileDialog::getSaveFileName(this, "保存乐谱", "", "MIDI Files (*.txt)");
    if (!filePath.isEmpty()) {
        score->save(filePath.toStdString());
        QMessageBox::information(this, "保存成功", "乐谱已成功保存。");
    }
}

void EditWindow::updateTrackList()
{
    trackList->clear();
    if (!score) return;

    for (int i = 0; i < score->gettracksnum(); ++i) {
        // 创建自定义的widget作为列表项Add commentMore actions
        QWidget *itemWidget = new QWidget();
        QHBoxLayout *layout = new QHBoxLayout(itemWidget);
        layout->setContentsMargins(5, 2, 5, 2);

        QLabel *label = new QLabel(QString("音轨 %1").arg(i + 1));
        QPushButton *settingsButton = new QPushButton("编辑");
        settingsButton->setFixedSize(50, 20);
        settingsButton->setStyleSheet("font-size: 10px;");

        // 存储音轨索引作为按钮属性
        settingsButton->setProperty("trackIndex", i);

        layout->addWidget(label);
        layout->addWidget(settingsButton);
        layout->addStretch();

        // 创建列表项
        QListWidgetItem *item = new QListWidgetItem();
        item->setSizeHint(itemWidget->sizeHint());

        trackList->addItem(item);
        trackList->setItemWidget(item, itemWidget);

        // 连接按钮信号
        connect(settingsButton, &QPushButton::clicked, [this, i]() {
            this->showTrackSettings(i);
        });
    }

    if (score->gettracksnum() > 0) {
        trackList->setCurrentRow(currentTrackIndex);
    }
}

void EditWindow::addTrack()
{
    if (!score) return;

    score->addTrack(Track());

    // 为新音轨创建画布
    NoteCanvas* newCanvas = new NoteCanvas();
    canvases.append(newCanvas);
    stackedWidget->addWidget(newCanvas);

    updateTrackList();

    // 切换到新添加的音轨
    currentTrackIndex = score->gettracksnum() - 1;
    switchTrack(currentTrackIndex);
}

void EditWindow::removeCurrentTrack()
{
    /*
    if (!score || score->gettracksnum() <= 1) {
        score->cleartrack(currentTrackIndex);
        stackedWidget->setCurrentIndex(currentTrackIndex);
        return;
    }*/

    // 移除当前音轨的画布
    if(canvases.empty())
        return;
    NoteCanvas* canvasToRemove = canvases.takeAt(currentTrackIndex);
    stackedWidget->removeWidget(canvasToRemove);
    delete canvasToRemove;

    // 从Score中移除音轨
    score->removetrack(currentTrackIndex);

    // 切换到第一条音轨
    currentTrackIndex = 0;
    updateTrackList();
    switchTrack(currentTrackIndex);
}

void EditWindow::switchTrack(int index)
{
    if (!score || index < 0 || index >= score->gettracksnum() || index >= canvases.size())
        return;

    currentTrackIndex = index;

    // 切换到当前音轨的画布
    stackedWidget->setCurrentIndex(index);
    //发送消息更改0通道音色
    score->gettrackbyn(index).sendprogrammessage();
    //如果没有结束录制就自动结束录制
    if(isRecording){
        stopRecording();
    }
}

void EditWindow::startmetronome(const Note& menote)
{
    while (isopenme) {
        menote.constplay(midiOut, selfbpm, 9);
        qDebug() << "tick";
        QThread::msleep(60000 / selfbpm);
    }
}

void EditWindow::startmebutton_clicked()
{
    isopenme = !isopenme;
    if (isopenme) {
        Note menote(77, 0.25);
        QtConcurrent::run([this, menote]() {
            this->startmetronome(menote);
        });
    }
}

void EditWindow::showTrackSettings(int trackIndex) {
    if (!score || trackIndex < 0 || trackIndex >= score->gettracksnum()) return;

    // 创建设置对话框
    QDialog dialog(this);
    dialog.setWindowTitle(QString("音轨 %1 设置").arg(trackIndex + 1));

    QFormLayout layout(&dialog);

    // 通道选择 (0-15)
    QSpinBox *channelSpinBox = new QSpinBox(&dialog);
    channelSpinBox->setRange(0, 15);
    channelSpinBox->setValue(score->gettrackbyn(trackIndex).getchannel());

    // 音色选择 (0-127)
    QSpinBox *programSpinBox = new QSpinBox(&dialog);
    programSpinBox->setRange(0, 127);
    programSpinBox->setValue(score->gettrackbyn(trackIndex).getprogram());

    QLabel *programNameLabel = new QLabel(&dialog);
    programNameLabel->setText(programToName.value(programSpinBox->value(), "自定义音色"));

    // 连接音色值变化信号
    connect(programSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int value) {
        programNameLabel->setText(programToName.value(value, "自定义音色"));
    });


    layout.addRow("通道 (0-15):", channelSpinBox);
    layout.addRow("音色 (0-127):", programSpinBox);
    layout.addRow("音色名称:", programNameLabel);

    // 确认和取消按钮
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    layout.addRow(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        // 保存设置
        score->gettrackbyn(trackIndex).setchannel(channelSpinBox->value());
        score->gettrackbyn(trackIndex).setprogram(programSpinBox->value());
        score->gettrackbyn(trackIndex).sendprogrammessage();
    }
}
