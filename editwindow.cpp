#include "editwindow.h"
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

EditWindow::EditWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("编辑界面");
    setMinimumSize(1200, 600);
    createWidgets();
    setupLayout();

}

EditWindow::~EditWindow()
{
    delete pianoKeys;
    delete trackList;
}

void EditWindow::createWidgets()
{
    // 左侧音轨列表（C3-C6，共28个白键）
    trackList = new QListWidget(this);
    trackList->setFixedWidth(150);
    trackList->setStyleSheet("border-right: 1px solid #ddd; font-size: 14px;");

    // 填充音名（C3-B6，对应白键索引0-27）
    QStringList octaveNames = {"C3", "D3", "E3", "F3", "G3", "A3", "B3",
                               "C4", "D4", "E4", "F4", "G4", "A4", "B4",
                               "C5", "D5", "E5", "F5", "G5", "A5", "B5",
                               "C6", "D6", "E6", "F6", "G6", "A6", "B6"};
    trackList->addItems(octaveNames);

    // 中间瀑布流画布
    QWidget *canvas = new QWidget(this);
    canvas->setObjectName("canvas");
    canvas->setStyleSheet("#canvas { background: #ffffff; }");

    // 底部钢琴键（复用PianoKeys）
    pianoKeys = new PianoKeys(this);
    pianoKeys->setKeyPressCallback([this](int keyIndex) {
        onPianoKeyPressed(keyIndex); // 按键时添加音符
    });

    // 主布局
    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addWidget(trackList);
    mainLayout->addWidget(canvas, 1);

    QVBoxLayout *canvasLayout = new QVBoxLayout(canvas);
    canvasLayout->addStretch();
    canvasLayout->addWidget(pianoKeys);

    QWidget *centralWidget = new QWidget();
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);
}

void EditWindow::setupLayout()
{
    // 画布垂直居中
    centralWidget()->layout()->setContentsMargins(20, 20, 20, 20);
}



void EditWindow::onPianoKeyPressed(int keyIndex)
{
    // 过滤黑键（假设黑键索引 >= 28，仅处理白键0-27）
    if (keyIndex >= pianoKeys->whiteKeys.size()) return;

    // 添加音符到列表（键索引，当前水平位置）
    notes.append({keyIndex, notePosition});
    notePosition += NOTE_WIDTH + 10; // 音符间隔10像素
    update(); // 触发重绘
}

void EditWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制左侧音轨分割线
    painter.setPen(QPen(QColor("#ddd"), 1));
    painter.drawLine(trackList->width(), 0, trackList->width(), height());

    // 绘制音符
    for (const auto &[keyIndex, x] : notes) {
        int y = getTrackY(keyIndex); // 获取音轨Y坐标
        painter.fillRect(x, y, NOTE_WIDTH, TRACK_SPACING * 2, QColor("#4CAF50")); // 绿色音符
        painter.setPen(Qt::black);

        QString keySequence = QKeySequence(pianoKeys->keyMap.key(keyIndex)).toString();
        std::string noteNameStd = pianoKeys->keytoname.value(keySequence); // 获取 std::string
        QString noteName = QString::fromStdString(noteNameStd); // 转换为 QString
        painter.drawText(x + 2, y + 15, noteName); // 正确传递 QString
    }
}

int EditWindow::getTrackY(int keyIndex)
{
    // 白键索引0-27对应C3-B6，共4个八度28个键
    int trackIndex = keyIndex; // 直接用键索引作为音轨索引
    return BASE_Y + trackIndex * TRACK_SPACING; // 每个音轨间隔40像素
}
