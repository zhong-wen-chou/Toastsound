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

void old_Notecanvas::setNotes(const QVector<std::tuple<int, int, int, QString>>& notes) {
    m_notes = notes;

    // 更新最后一个位置
    if (!m_notes.isEmpty()) {
        const auto& lastNote = m_notes.last();
        lastNotePosition = std::get<1>(lastNote) + std::get<2>(lastNote) + 5;
    } else {
        lastNotePosition = 0;
    }

    update();
    updateCanvasSize();
}

void old_Notecanvas::addNote(int keyIndex, int position, int width) {
    // 添加音高文本
    QString pitchText = QString::number(keyIndex);

    // 存储音符信息：音高、位置、宽度、文本
    m_notes.append({keyIndex, position, width, pitchText});
    lastNotePosition = position + width + 5;
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
    for (const auto &[keyIndex, x, width, text] : m_notes) {
        maxX = qMax(maxX, x + width);
    }

    // 设置画布大小
    setMinimumSize(maxX + 100, 800);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void old_Notecanvas::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    // 绘制背景
    painter.fillRect(rect(), QColor("#f8f8f8"));

    // 绘制音符
    for (const auto &note : m_notes) {
        int keyIndex = std::get<0>(note);
        int x = std::get<1>(note);
        int width = std::get<2>(note);
        QString text = std::get<3>(note);
        int y = getNoteY(keyIndex);

        QColor noteColor = (keyIndex < 28) ?
                               QColor("#2196F3") : QColor("#888888");

        int height = 20;

        // 绘制音符矩形
        painter.setBrush(noteColor);
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(x, y, width, height, 4, 4);

        // 绘制音高文本
        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 8));

        // 确保文本在矩形内居中
        QRect textRect(x, y, width, height);
        painter.drawText(textRect, Qt::AlignCenter, text);
    }
}

int old_Notecanvas::getNoteY(int keyIndex) const {
    // 将MIDI音符编号映射到合理的高度范围
    // MIDI音符范围0-127，映射到画布高度0-800
    const int MIN_PITCH = 0;
    const int MAX_PITCH = 127;
    const int CANVAS_HEIGHT = 800;

    // 反转映射，使低音在底部，高音在顶部
    int normalizedPitch = MAX_PITCH - keyIndex;

    // 计算Y位置，保留顶部和底部边距
    return 50 + (normalizedPitch * (CANVAS_HEIGHT - 100)) / (MAX_PITCH - MIN_PITCH);
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
            qDebug() << "音轨数量：" << score.gettracksnum();

            // 清空现有画布
            while (!canvases.isEmpty()) {
                old_Notecanvas* canvas = canvases.takeLast();
                stackedWidget->removeWidget(canvas);
                delete canvas;
            }
            trackList->clear();

            // 定义音符显示参数
            const int PIXELS_PER_QUARTER_NOTE = 50;
            const int MIN_NOTE_WIDTH = 10;
            const int NOTE_SPACING = 15;

            // 为每个音轨创建画布
            for (int i = 0; i < score.gettracksnum(); i++) {
                // 创建新画布
                old_Notecanvas* newCanvas = new old_Notecanvas();
                canvases.append(newCanvas);
                stackedWidget->addWidget(newCanvas);

                // 更新音轨列表
                trackList->addItem(QString("音轨 %1").arg(trackList->count() + 1));

                // 获取当前音轨
                Track& track = score.gettrackbyn(i);
                qDebug() << "音轨" << i << "包含" << track.getnotesnum() << "个音符";

                // 当前时间位置（像素）
                int currentPosition = 0;

                // 遍历音轨中的所有音符
                for (int u = 0; u < track.getnotesnum(); u++) {
                    MidiNote* pnote = track.getnotesbyn(u);

                    // 获取音符的音高和时长
                    int pitch = pnote->getpitch();
                    double duration = pnote->getduration();

                    // 计算音符的宽度（基于时长）
                    int noteWidth = duration * PIXELS_PER_QUARTER_NOTE;
                    if (noteWidth < MIN_NOTE_WIDTH) {
                        noteWidth = MIN_NOTE_WIDTH;
                    }

                    qDebug() << "音符" << u << ": pitch=" << pitch
                             << ", duration=" << duration
                             << ", width=" << noteWidth
                             << ", position=" << currentPosition;

                    // 添加音符到画布
                    newCanvas->addNote(pitch, currentPosition, noteWidth);

                    // 更新当前时间位置
                    currentPosition += noteWidth + NOTE_SPACING;
                }

                qDebug() << "音轨" << i << "总宽度:" << currentPosition;

                // 强制刷新画布
                newCanvas->update();
                newCanvas->updateCanvasSize();
            }

            // 切换到第一个音轨
            if (trackList->count() > 0) {
                trackList->setCurrentRow(0);
                currentTrackIndex = 0;
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
