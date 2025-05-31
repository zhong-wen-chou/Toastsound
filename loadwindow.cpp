#include "loadwindow.h"
#include "mainwindow.h" // 用于返回主界面
#include "song.h"
#include <QFileDialog>
#include <QtConcurrent/QtConcurrentRun>
#include <QGroupBox>
#include <QScrollArea>
#include <QPainter>

const int NOTE_WIDTH = 20;
const int TRACK_SPACING = 20;
const int BASE_Y = 20;
const QColor WHITE_KEY_COLOR = Qt::white;
const QColor BLACK_KEY_COLOR = Qt::black;
// old_Notecanvas 实现
old_Notecanvas::old_Notecanvas(QWidget *parent) : QWidget(parent) {}

void old_Notecanvas::setNotes(const QVector<std::tuple<int, int>>& notes) {
    m_notes = notes;
    update();
}

void old_Notecanvas::addNote(int keyIndex, int position) {
    m_notes.append({keyIndex, position});
    lastNotePosition = position + NOTE_WIDTH + 5;
    update();
    updateCanvasSize();
}

void old_Notecanvas::clearNotes() {
    m_notes.clear();
    lastNotePosition = 0;
    update();
}

int old_Notecanvas::getLastNotePosition() const {
    return lastNotePosition;
}

void old_Notecanvas::updateCanvasSize() {
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

void old_Notecanvas::paintEvent(QPaintEvent *event) {
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

int old_Notecanvas::getNoteY(int keyIndex) const {
    return 30 + (keyIndex % 36) * 20;
}

LoadWindow::LoadWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("读取界面");
    setMinimumSize(1500, 800); // 增加高度以适应新布局
    createWidgets();
    setupLayout();
    connectSignals();

    // 添加初始音轨
    addTrack();
}

LoadWindow::~LoadWindow()
{
    qDeleteAll(canvases);
}

void LoadWindow::createWidgets()
{
    // 音轨列表
    trackList = new QListWidget(this);

    // 音轨操作按钮
    addTrackButton = new QPushButton("添加音轨", this);
    removeTrackButton = new QPushButton("删除音轨", this);
    addTrackButton->setMinimumSize(120, 40);
    removeTrackButton->setMinimumSize(120, 40);
    addTrackButton->setStyleSheet("background-color: #4CAF50; color: white; border-radius: 8px;");
    removeTrackButton->setStyleSheet("background-color: #FF4444; color: white; border-radius: 8px;");

    // 使用堆栈窗口管理多个画布
    stackedWidget = new QStackedWidget(this);

    // 底部按钮
    returnButton = new QPushButton("返回", this);
    returnButton->setMinimumSize(120, 50);
    returnButton->setStyleSheet("background-color: #FF4444; color: white; border-radius: 8px;");

    selectAudioButton = new QPushButton("选择播放音频", this);
    selectAudioButton->setMinimumSize(120, 50);
    selectAudioButton->setStyleSheet("background-color: #165DFF; color: white; border-radius: 8px;");

    playButton = new QPushButton("开始播放", this);
    playButton->setMinimumSize(120, 50);
    playButton->setStyleSheet("background-color: #4CAF50; color: white; border-radius: 8px;");

    stopButton = new QPushButton("中止", this);
    stopButton->setMinimumSize(120, 50);
    stopButton->setStyleSheet("background-color: #FF4444; color: white; border-radius: 8px;");

    volumeButton = new QPushButton("音量", this);
    volumeButton->setMinimumSize(120, 50);
    volumeButton->setStyleSheet("background-color: #7B61FF; color: white; border-radius: 8px;");

    // 音量滑块（初始隐藏）
    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(70);
    volumeSlider->setStyleSheet("margin: 0 20px;");
    volumeSlider->hide();
}

void LoadWindow::setupLayout()
{
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // 左侧音轨管理面板
    QWidget* trackPanel = new QWidget();
    trackPanel->setFixedWidth(200);
    trackPanel->setStyleSheet("background-color: #f0f0f0; padding: 10px;");

    QVBoxLayout* trackPanelLayout = new QVBoxLayout(trackPanel);
    trackPanelLayout->addWidget(new QLabel("音轨管理", trackPanel));
    trackPanelLayout->addWidget(trackList);
    trackPanelLayout->addWidget(addTrackButton);
    trackPanelLayout->addWidget(removeTrackButton);
    trackPanelLayout->addStretch();

    // 右侧音符显示区域
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidget(stackedWidget);
    scrollArea->setWidgetResizable(true);

    // 底部按钮栏
    QHBoxLayout *bottomButtonLayout = new QHBoxLayout();
    bottomButtonLayout->addWidget(returnButton);
    bottomButtonLayout->addWidget(selectAudioButton);
    bottomButtonLayout->addWidget(playButton);
    bottomButtonLayout->addWidget(stopButton);
    bottomButtonLayout->addWidget(volumeButton);
    bottomButtonLayout->addWidget(volumeSlider);
    bottomButtonLayout->setSpacing(15);
    bottomButtonLayout->setContentsMargins(20, 10, 20, 20);

    // 主布局
    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->addWidget(scrollArea, 1);
    rightLayout->addLayout(bottomButtonLayout);

    QWidget *rightWidget = new QWidget();
    rightWidget->setLayout(rightLayout);

    mainLayout->addWidget(trackPanel);
    mainLayout->addWidget(rightWidget, 1);

    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);
}

void LoadWindow::connectSignals()
{
    connect(returnButton, &QPushButton::clicked, this, &LoadWindow::onReturnToMain);
    connect(volumeButton, &QPushButton::clicked, this, &LoadWindow::showVolumeControl);
    connect(selectAudioButton, &QPushButton::clicked, this, &LoadWindow::selectAudio);
    connect(playButton, &QPushButton::clicked, this, &LoadWindow::beginplay);
    connect(stopButton, &QPushButton::clicked, this, &LoadWindow::stopplay);
    connect(addTrackButton, &QPushButton::clicked, this, &LoadWindow::addTrack);
    connect(removeTrackButton, &QPushButton::clicked, this, &LoadWindow::removeCurrentTrack);
    connect(trackList, &QListWidget::currentRowChanged, this, &LoadWindow::switchTrack);

    // 音量滑块
    connect(volumeSlider, &QSlider::valueChanged, [this](int value) {
        qDebug() << "当前音量：" << value << "%";
    });
}

void LoadWindow::onReturnToMain()
{
    hide();
    MainWindow *mainWindow = qobject_cast<MainWindow*>(parent());
    if (mainWindow) {
        mainWindow->show();
    }
}

void LoadWindow::showVolumeControl()
{
    isVolumeVisible = !isVolumeVisible;
    volumeSlider->setVisible(isVolumeVisible);
    volumeButton->setText(isVolumeVisible ? "隐藏音量" : "音量");
}

void LoadWindow::selectAudio()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "选择要加载的文件",
        "",
        "MIDI Files (*.mid *.txt *.mscore);;All Files (*)"
        );

    if (!fileName.isEmpty()) {
        qDebug() << "准备加载文件：" << fileName;

        try {
            score.load(fileName.toStdString());
            qDebug() << "文件加载成功";

            // 清空现有画布
            while (!canvases.isEmpty()) {
                old_Notecanvas* canvas = canvases.takeLast();
                stackedWidget->removeWidget(canvas);
                delete canvas;
            }

            // 为每个音轨创建画布
            for (int i = 0; i < score.gettracksnum(); i++) {
                // 创建新画布
                old_Notecanvas* newCanvas = new old_Notecanvas();
                canvases.append(newCanvas);
                stackedWidget->addWidget(newCanvas);

                // 更新音轨列表
                trackList->addItem(QString("音轨 %1").arg(trackList->count() + 1));
                trackList->setCurrentRow(trackList->count() - 1);
                currentTrackIndex = trackList->count() - 1;
                //对该音轨进行绘制
                Track& temp=score.gettrackbyn(i);
                for(int u=0;u<temp.getnotesnum();u++){
                    MidiNote* pnote=temp.getnotesbyn(u);
                    //根据该音符的音高pitch时长duration在画布对应位置画出色块
                    //待完成
                }

            }

        } catch (const std::exception& e) {
            qDebug() << "异常：" << e.what();
        }
    } else {
        qDebug() << "用户取消了文件选择";
    }
}

void LoadWindow::beginplay(){
    QtConcurrent::run([this]() {
        score.play();
    });
}

void LoadWindow::stopplay(){
    score.stop();
}

void LoadWindow::addTrack()
{
    score.addTrack(Track());

    // 创建新画布
    old_Notecanvas* newCanvas = new old_Notecanvas();
    canvases.append(newCanvas);
    stackedWidget->addWidget(newCanvas);

    // 更新音轨列表
    trackList->addItem(QString("音轨 %1").arg(trackList->count() + 1));
    trackList->setCurrentRow(trackList->count() - 1);
    currentTrackIndex = trackList->count() - 1;
}

void LoadWindow::removeCurrentTrack()
{
    if (trackList->count() <= 1) return; // 至少保留一个音轨

    // 从堆栈中移除画布
    old_Notecanvas* canvasToRemove = canvases.takeAt(currentTrackIndex);
    stackedWidget->removeWidget(canvasToRemove);
    delete canvasToRemove;

    // 从score中移除音轨
    score.removetrack(currentTrackIndex);

    // 更新音轨列表
    delete trackList->takeItem(currentTrackIndex);

    // 调整当前音轨索引
    if (currentTrackIndex >= trackList->count()) {
        currentTrackIndex = trackList->count() - 1;
    }
    trackList->setCurrentRow(currentTrackIndex);
}

void LoadWindow::switchTrack(int index)
{
    if (index >= 0 && index < stackedWidget->count()) {
        currentTrackIndex = index;
        stackedWidget->setCurrentIndex(index);
    }
}
