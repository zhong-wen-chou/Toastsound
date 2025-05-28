#include "editwindow.h"
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

EditWindow::EditWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("编辑界面");
    setMinimumSize(1500, 800);
    createWidgets();
    setupLayout();
    connectSignals();


}

EditWindow::~EditWindow()
{
    delete pianoKeys;
    delete trackList;
}

void EditWindow::createWidgets()
{
    // 左侧音轨列表（C3-C6）
    trackList = new QListWidget(this);
    trackList->setFixedWidth(150);
    trackList->setStyleSheet("border-right: 1px solid #ddd; font-size: 12px;"); // 缩小字体
    trackList->addItems({
        "C3", "C#3", "D3", "D#3", "E3", "F3", "F#3", "G3", "G#3", "A3", "A#3", "B3",
        "C4", "C#4", "D4", "D#4", "E4", "F4", "F#4", "G4", "G#4", "A4", "A#4", "B4",
        "C5", "C#5", "D5", "D#5", "E5", "F5", "F#5", "G5", "G#5", "A5", "A#5", "B5",
        "C6", "C#6", "D6", "D#6", "E6", "F6", "F#6", "G6", "G#6", "A6", "A#6", "B6"
    }); // 包含黑键音名

    // 中间瀑布流画布
    QWidget *canvas = new QWidget(this);
    canvas->setStyleSheet("#canvas { background: #f8f8f8; }");

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
    playButton = new QPushButton("开始播放", this);
    saveButton = new QPushButton("保存乐谱", this);
    exitButton->setStyleSheet("background-color: #FF4444; color: white;");
    playButton->setStyleSheet("background-color: #4CAF50; color: white;");
    saveButton->setStyleSheet("background-color: #2196F3; color: white;");

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout();
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(trackList);
    topLayout->addWidget(canvas, 1);

    QVBoxLayout *canvasLayout = new QVBoxLayout(canvas);
    canvasLayout->addStretch();
    canvasLayout->addWidget(pianoKeys);

    // 底部按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(20, 10, 20, 20);
    buttonLayout->addWidget(exitButton);
    buttonLayout->addWidget(playButton);
    buttonLayout->addWidget(saveButton);

    mainLayout->addLayout(topLayout, 1);
    mainLayout->addLayout(buttonLayout);

    QWidget *centralWidget = new QWidget();
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);
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
}

void EditWindow::onPianoKeyPressed(int keyIndex)
{
    //if (keyIndex >= 28) return;

    notes.append({keyIndex, notePosition});
    notePosition += NOTE_WIDTH + 5; // 减小间隔为5像素
    update(); // 重绘瀑布流
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

void EditWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制左侧分割线
    painter.drawLine(trackList->width(), 0, trackList->width(), height());

    // 绘制音符
    for (const auto &[keyIndex, x] : notes) {
        int y = getTrackY(keyIndex);
        QColor noteColor = (keyIndex < 28) ? WHITE_KEY_COLOR : BLACK_KEY_COLOR; // 白键/黑键颜色区分

        // 绘制音符矩形（黑键音符宽度略窄，视觉上位于两白键中间）
        int noteWidth = (keyIndex < 28) ? NOTE_WIDTH : NOTE_WIDTH * 0.8; // 黑键音符宽度为白键的80%
        painter.fillRect(x, y, noteWidth, TRACK_SPACING * 1.5, noteColor);

        // 绘制音名（黑键显示带#的音名）
        QKeySequence keySequence = pianoKeys->keyMap.key(keyIndex);
        std::string noteNameStd = pianoKeys->keytoname.value(keySequence);
        QString noteName = QString::fromStdString(noteNameStd);
        painter.drawText(x + 2, y + 10, noteName);
    }
}

int EditWindow::getTrackY(int keyIndex)
{
    // 4个八度共28个白键，每个音轨间隔20像素
    //return BASE_Y + keyIndex * TRACK_SPACING;

    int octaveIndex = keyIndex / 12; // 0=C3八度，1=C4八度，2=C5八度，3=C6八度
    int keyInOctave = keyIndex % 12; // 0-11（0-6为白键，7-11为黑键）

    // 白键垂直位置：每个白键间隔20像素
    // 黑键垂直位置：位于两个白键之间，偏移10像素
    int whiteKeyY = BASE_Y + (keyInOctave % 7) * TRACK_SPACING; // 白键在八度内的位置（0-6）
    int blackKeyY = whiteKeyY + TRACK_SPACING / 2; // 黑键位于两白键中间

    return octaveIndex * (TRACK_SPACING * 7) + (keyInOctave < 7 ? whiteKeyY : blackKeyY);
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
    qDebug() << "开始播放乐谱";
    // 播放逻辑
}

void EditWindow::saveScore()
{
    qDebug() << "保存乐谱到文件";
    // 保存逻辑
}
