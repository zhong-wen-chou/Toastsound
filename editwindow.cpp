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


const int NOTE_WIDTH = 20;
const int TRACK_SPACING = 20;
const int BASE_Y = 20;
const QColor WHITE_KEY_COLOR = Qt::white;
const QColor BLACK_KEY_COLOR = Qt::black;

class Note; // 前向声明

// NoteCanvas 实现
NoteCanvas::NoteCanvas(QWidget *parent) : QWidget(parent) {}

void NoteCanvas::setNotes(const QVector<std::tuple<int, int>>& notes) {
    m_notes = notes;
    update();
}

void NoteCanvas::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制背景
    painter.fillRect(rect(), QColor("#f8f8f8"));

    // 绘制音符
    for (const auto &note : m_notes) {
        int keyIndex = std::get<0>(note);
        int x = std::get<1>(note);
        int y = getNoteY(keyIndex);

        QColor noteColor = (keyIndex < 28) ?
                               QColor("#2196F3") : QColor("#888888"); // 蓝/灰

        int height = 20;
        int width = (keyIndex < 28) ? 20 : 16; // 白键宽，黑键窄

        painter.setBrush(noteColor);
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(x, y, width, height, 4, 4);


        //QString noteName = PianoKeys::Keytoname(keyIndex);
        //painter.setPen(Qt::black);
        //painter.drawText(x + 3, y + 15, noteName);
    }
}

int NoteCanvas::getNoteY(int keyIndex) const {
    // 简化的坐标计算：按音符索引排列
    return 30 + (keyIndex % 36) * 20; // 每行20像素间距
}

EditWindow::EditWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("编辑界面");
    setMinimumSize(1500, 800);
    score = new Score(); // 初始化 Score 对象
    currentTrackIndex = 0;
    notePosition = 0;
    isopenme = false; // 新增：节拍器开关状态
    selfbpm = 120; // 新增：节拍器节拍
    initvolumn = 70; // 新增：节拍器音量

    createWidgets();
    setupLayout();
    connectSignals();

    if (score && score->gettracksnum() == 0) {
        score->addTrack(Track()); // 假设 Track 有默认构造函数
        updateTrackList();
    }
}

EditWindow::~EditWindow()
{
    delete pianoKeys;
    delete trackList;
    delete score;
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

    // 新增：节拍器控制
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

    // 中间瀑布流画布
    canvas = new NoteCanvas(this);
    canvas->setStyleSheet("#canvas { background: #f8f8f8; }");

    // 让画布能够接收绘制事件
    canvas->setAttribute(Qt::WA_OpaquePaintEvent);

    // 底部钢琴键
    pianoKeys = new PianoKeys(this);
    pianoKeys->setKeyPressCallback([this](int keyIndex) {
        onPianoKeyPressed(keyIndex);
        updateNoteVisual(keyIndex, true); // 按下时更新视觉
    });

    // 连接按键释放信号（处理松开时的视觉恢复）
    //connect(pianoKeys, &QWidget::keyReleaseEvent, this, &EditWindow::onPianoKeyReleased);

    // 新增底部按钮
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
    //topLayout->addWidget(canvas, 1);

    // 右侧画布区域（使用 scrollArea 确保音符不会被遮挡）
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidget(canvas);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    topLayout->addWidget(scrollArea, 1);  // 占据剩余空间

    // 底部按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(20, 10, 20, 20);
    buttonLayout->addWidget(exitButton);
    buttonLayout->addWidget(playButton);
    buttonLayout->addWidget(saveButton);

    mainLayout->addLayout(topLayout, 1);
    mainLayout->addWidget(pianoKeys);  // 将钢琴键添加到主布局
    mainLayout->addLayout(buttonLayout);

    QWidget *centralWidget = new QWidget();
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    // 连接滑块信号到显示更新
    connect(tempoSlider, &QSlider::valueChanged, [tempoLabel, this](int value) {
        tempoLabel->setText(QString("%1 BPM").arg(value));
        selfbpm = value;
    });

    connect(volumeSlider, &QSlider::valueChanged, [volumeLabel, this](int value) {
        volumeLabel->setText(QString("%1%").arg(value));
        initvolumn = value;
    });

    // 连接节拍器按钮
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

    // 音轨管理信号连接
    connect(addTrackButton, &QPushButton::clicked, this, &EditWindow::addTrack);
    connect(removeTrackButton, &QPushButton::clicked, this, &EditWindow::removeCurrentTrack);
    connect(trackList, &QListWidget::currentRowChanged, this, &EditWindow::switchTrack);
}

void EditWindow::onPianoKeyPressed(int keyIndex)
{
    if (!score) return;

    // 创建新的音符
    Note* newNote = new Note(keyIndex, 1.0, 90);
    // 将音符添加到当前音轨
    score->gettrackbyn(currentTrackIndex).addNote(newNote);

    notes.append({keyIndex, notePosition});
    notePosition += NOTE_WIDTH + 5; // 减小间隔为5像素

    canvas->setNotes(notes);
    updateCanvasSize(); // 确保调用此函数
    if (scrollArea) {
        scrollArea->ensureVisible(notePosition, 0, 50, 0);
    }
}

void EditWindow::onPianoKeyReleased(int keyIndex)
{
    updateNoteVisual(keyIndex, false); // 松开时恢复视觉
}

void EditWindow::updateNoteVisual(int keyIndex, bool isPressed)
{
    if (keyIndex < 28) { // 白键
        QPushButton *key = pianoKeys->whiteKeys[keyIndex];
        key->setEnabled(!isPressed);
    } else { // 黑键
        int blackIndex = keyIndex - 28;
        if (blackIndex >= 0 && blackIndex < pianoKeys->blackKeys.size()) {
            QPushButton *key = pianoKeys->blackKeys[blackIndex];
            key->setEnabled(!isPressed);
        }
    }
}


void EditWindow::updateCanvasSize() {
    if (!canvas || notes.isEmpty()) return;

    // 计算最大X坐标
    int maxX = 0;
    for (const auto &[keyIndex, x] : notes) {
        maxX = qMax(maxX, x + NOTE_WIDTH);
    }

    // 设置画布大小（宽度+边距，高度足够显示所有音符）
    canvas->setMinimumSize(maxX + 100, 800);
    canvas->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 确保滚动区域更新
    if (scrollArea) {
        scrollArea->updateGeometry();
    }
}


void EditWindow::exitToMain()
{
    if (parentWidget()) {
        parentWidget()->show(); // 显示父窗口
    }
    this->close(); // 关闭当前窗口
}

void EditWindow::startPlayback()
{
    qDebug() << "开始录制";
    // 播放逻辑
    isstart=true;
    keyLogs.clear(); // 清除旧记录
    qDebug() << "计时器已启动";
    timer.restart();
}

void EditWindow::saveScore()
{
    qDebug() << "保存乐谱到文件";
    // 保存逻辑
    QString filePath = QFileDialog::getSaveFileName(this, "保存乐谱", "", "MIDI Files (*.mid)");
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
        trackList->addItem(QString("音轨 %1").arg(i + 1));
    }

    // 保持当前选中状态
    if (score->gettracksnum() > 0) {
        trackList->setCurrentRow(currentTrackIndex);
    }
}

void EditWindow::addTrack()
{
    if (!score) return;

    score->addTrack(Track()); // 添加新音轨
    updateTrackList();

    // 切换到新添加的音轨
    currentTrackIndex = score->gettracksnum() - 1;
    switchTrack(currentTrackIndex);
}

void EditWindow::removeCurrentTrack()
{
    if (!score || score->gettracksnum() <= 1) return; // 至少保留一条音轨

    score->gettrackbyn(currentTrackIndex).clear(); // 清空当前音轨
    //score->gettracks().erase(score->gettracks().begin() + currentTrackIndex); // 删除音轨
    score->removetrack(currentTrackIndex);
    // 切换到第一条音轨
    currentTrackIndex = 0;
    updateTrackList();
    switchTrack(currentTrackIndex);
}

void EditWindow::switchTrack(int index)
{
    if (!score || index < 0 || index >= score->gettracksnum()) return;

    currentTrackIndex = index;

    // 从 Score 中加载当前音轨的音符
    notes.clear();
    Track& currentTrack = score->gettrackbyn(currentTrackIndex);
    for (int i = 0; i < currentTrack.getnotesnum(); ++i) {
        MidiNote* note = currentTrack.getnotesbyn(i);
        notes.append({note->getpitch(), notePosition});
        notePosition += NOTE_WIDTH + 5;
    }

    // 更新 notePosition 为最后一个音符的位置
    if (!notes.isEmpty()) {
        notePosition = std::get<1>(notes.last()) + NOTE_WIDTH + 5;
    } else {
        notePosition = 0;
    }

    canvas->setNotes(notes);
    updateCanvasSize();
}

void EditWindow::startmetronome(const Note& menote)
{
    while (isopenme) {
        // 可以在这里播放节拍声（Beep、QSound、或 pianoKeys->playClickSound()）
        menote.constplay(midiOut, selfbpm, 9); // 使用调整后的音量
        qDebug() << "tick";
        QThread::msleep(60000 / selfbpm); // 按当前 BPM 延时
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
