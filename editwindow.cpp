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

const int NOTE_WIDTH = 20;
const int TRACK_SPACING = 20;
const int BASE_Y = 20;
const QColor WHITE_KEY_COLOR = Qt::white;
const QColor BLACK_KEY_COLOR = Qt::black;

//class Note;

// NoteCanvas 实现
NoteCanvas::NoteCanvas(QWidget *parent) : QWidget(parent) {}

void NoteCanvas::setNotes(const QVector<std::tuple<int, int>>& notes) {
    m_notes = notes;
    update();
}

void NoteCanvas::addNote(int keyIndex, int position) {
    m_notes.append({keyIndex, position});
    lastNotePosition = position + NOTE_WIDTH + 5;
    update();
    updateCanvasSize();
}

void NoteCanvas::clearNotes() {
    m_notes.clear();
    lastNotePosition = 0;
    update();
}

int NoteCanvas::getLastNotePosition() const {
    return lastNotePosition;
}

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
                               QColor("#2196F3") : QColor("#888888");

        int height = 20;
        int width = (keyIndex < 28) ? 20 : 16;

        painter.setBrush(noteColor);
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(x, y, width, height, 4, 4);
    }
}

int NoteCanvas::getNoteY(int keyIndex) const {
    return 30 + (keyIndex % 36) * 20;
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
    pianoKeys->setKeyPressCallback([this](int keyIndex,int midinum) {
        onPianoKeyPressed(keyIndex,midinum);
        updateNoteVisual(keyIndex, true);
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

void EditWindow::onPianoKeyPressed(int keyIndex,int midinum)
{
    if (!score || currentTrackIndex >= canvases.size()) return;

    // 创建新的音符
    Note* newNote = new Note(midinum, 1.0, 90);
    score->gettrackbyn(currentTrackIndex).addNote(newNote);

    // 获取当前音轨的画布
    NoteCanvas* currentCanvas = canvases[currentTrackIndex];

    // 获取当前画布的音符位置
    int position = currentCanvas->getLastNotePosition();

    // 添加音符到当前画布
    currentCanvas->addNote(keyIndex, position);
}

void EditWindow::onPianoKeyReleased(int keyIndex)
{
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
}

void EditWindow::startPlayback()
{
    qDebug() << "开始录制";
    isstart = true;
    keyLogs.clear();
    qDebug() << "计时器已启动";
    timer.restart();
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
        trackList->addItem(QString("音轨 %1").arg(i + 1));
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
