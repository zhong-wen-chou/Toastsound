#include "loadwindow.h"
#include "mainwindow.h" // 用于返回主界面
#include "song.h"
#include <QFileDialog>
#include <QtConcurrent>

LoadWindow::LoadWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("读取界面");
    setMinimumSize(1500, 600); // 与编曲界面保持一致的宽度
    createWidgets();
    setupLayout();
    connectSignals();
}

LoadWindow::~LoadWindow()
{
    delete pianoKeys;
}

void LoadWindow::createWidgets()
{
    // 左侧按钮列
    /*
    leftButtonLayout = new QVBoxLayout();
    returnButton = new QPushButton("返回", this);
    returnButton->setMinimumSize(150, 50);
    returnButton->setStyleSheet("background-color: #FF4444; color: white; border-radius: 8px;");

    selectAudioButton = new QPushButton("选择播放音频", this);
    selectAudioButton->setMinimumSize(150, 50);
    selectAudioButton->setStyleSheet("background-color: #165DFF; color: white; border-radius: 8px;");

    leftButtonLayout->addWidget(returnButton);
    leftButtonLayout->addWidget(selectAudioButton);
    leftButtonLayout->setSpacing(20);
    leftButtonLayout->setContentsMargins(20, 20, 0, 0);
    */

    // 右侧钢琴键组件
    pianoKeys = new PianoKeys(this);

    // 底部按钮栏
    bottomButtonLayout = new QHBoxLayout();

    returnButton = new QPushButton("返回", this);
    returnButton->setMinimumSize(150, 50);
    returnButton->setStyleSheet("background-color: #FF4444; color: white; border-radius: 8px;");

    selectAudioButton = new QPushButton("选择播放音频", this);
    selectAudioButton->setMinimumSize(150, 50);
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

    bottomButtonLayout->addWidget(returnButton);
    bottomButtonLayout->addWidget(selectAudioButton);
    bottomButtonLayout->addWidget(playButton);
    bottomButtonLayout->addWidget(stopButton);
    bottomButtonLayout->addWidget(volumeButton);
    bottomButtonLayout->addWidget(volumeSlider);
    bottomButtonLayout->setSpacing(15);
    bottomButtonLayout->setContentsMargins(20, 20, 20, 20);
}

void LoadWindow::setupLayout()
{
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // 左侧按钮区域
    QWidget *leftWidget = new QWidget();
    leftWidget->setLayout(leftButtonLayout);
    mainLayout->addWidget(leftWidget, 1); // 左侧占1份宽度

    // 右侧钢琴键区域
    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->addWidget(pianoKeys, 1);
    rightLayout->addLayout(bottomButtonLayout);

    QWidget *rightWidget = new QWidget();
    rightWidget->setLayout(rightLayout);
    mainLayout->addWidget(rightWidget, 4); // 右侧占4份宽度

    setCentralWidget(centralWidget);
}

void LoadWindow::connectSignals()
{
    connect(returnButton, &QPushButton::clicked, this, &LoadWindow::onReturnToMain);
    connect(volumeButton, &QPushButton::clicked, this, &LoadWindow::showVolumeControl);
    connect(selectAudioButton, &QPushButton::clicked, this, &LoadWindow::selectAudio);
    connect(playButton, &QPushButton::clicked, this, &LoadWindow::beginplay);
    connect(stopButton, &QPushButton::clicked, this, &LoadWindow::stopplay);

    // 音量滑块联动（可扩展为实际音量控制）
    connect(volumeSlider, &QSlider::valueChanged, [this](int value) {
        qDebug() << "当前音量：" << value << "%";
    });
}

void LoadWindow::onReturnToMain()
{
    // 返回主界面
    hide();
    MainWindow *mainWindow = qobject_cast<MainWindow*>(parent());
    if (mainWindow) {
        mainWindow->show();
    }
}

void LoadWindow::showVolumeControl()
{
    // 切换音量滑块可见状态
    isVolumeVisible = !isVolumeVisible;
    volumeSlider->setVisible(isVolumeVisible);

    // 调整按钮文本（可选）
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

    // 如果用户选了文件
    if (!fileName.isEmpty()) {
        qDebug() << "准备加载文件：" << fileName;

        try {
            score.load(fileName.toStdString()); // 你的加载函数
            qDebug() << "文件加载成功";
            //score.play();
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
