#include "mainwindow.h"
#include "loadwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("ToustSound");
    setWindowIcon(QIcon(":/pictures/toust_icon.png")); // 设置图标
    setMinimumSize(800, 600);

    createWidgets(); // 创建控件
    setupLayout();   // 设置布局
    connectSignals(); // 连接信号
}

MainWindow::~MainWindow()
{
    delete scoreEditor;
}

// 创建控件
void MainWindow::createWidgets()
{
    titleLabel = new QLabel("ToustSound", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 32px; font-weight: bold; color: #165DFF;");

    subtitleLabel = new QLabel("Created by Big Toust", this);
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet("font-size: 16px; color: #555; margin-bottom: 40px;");

    composeButton = new QPushButton("谱曲", this);
    composeButton->setMinimumSize(30, 40);
    composeButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #165DFF;"
        "    color: white;"
        "    border-radius: 12px;"
        "    font-size: 18px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #0E42B3;"
        "}"
        );

    loadButton = new QPushButton("读取", this);
    loadButton->setMinimumSize(30, 40);
    loadButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #7B61FF;"
        "    color: white;"
        "    border-radius: 12px;"
        "    font-size: 18px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #5A47CC;"
        "}"
        );

    helpButton = new QPushButton("帮助", this);
    helpButton->setMinimumSize(30, 40);
    helpButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #FF7D00;"
        "    color: white;"
        "    border-radius: 12px;"
        "    font-size: 18px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #CC6400;"
        "}"
        );

    versionLabel = new QLabel("© 大吐司 | 版本 1.0.0？maybe", this);
    versionLabel->setAlignment(Qt::AlignCenter);
    versionLabel->setStyleSheet("color: #888; margin-top: 40px;");
}

// 设置布局和背景图
void MainWindow::setupLayout()
{
    // 创建中央部件
    QWidget *centralWidget = new QWidget(this);

    // 设置背景图样式
    centralWidget->setStyleSheet(
        "background-image: url(:/pictures/mainbackgroud.png);"
        "background-repeat: no-repeat;"
        "background-position: center;"
        "background-origin: content;"
        );

    // 创建垂直布局
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);
    layout->setContentsMargins(40, 40, 40, 40);

    // 添加控件
    layout->addWidget(titleLabel);
    layout->addWidget(subtitleLabel);
    layout->addWidget(composeButton);
    layout->addWidget(loadButton);
    layout->addWidget(helpButton);
    layout->addWidget(versionLabel);

    setCentralWidget(centralWidget);
}

// 连接信号
void MainWindow::connectSignals()
{
    connect(composeButton, &QPushButton::clicked, this, &MainWindow::onComposeButtonClicked);
    connect(loadButton, &QPushButton::clicked, this, &MainWindow::onLoadButtonClicked);
    connect(helpButton, &QPushButton::clicked, []() {
        qDebug() << "帮助按钮被点击";
    });
}

// 跳转到谱曲界面
void MainWindow::onComposeButtonClicked()
{
    qDebug() << "跳转到编曲界面";
    hide();

    if (!scoreEditor) {
        scoreEditor = new ScoreEditor(this);
        connect(scoreEditor, &ScoreEditor::exitRequested, this, [this]() {
            scoreEditor->hide();
            show();
        });
    }

    scoreEditor->show();
}

// 跳转到读取界面
void MainWindow::onLoadButtonClicked()
{
    qDebug() << "跳转到读取界面";
    hide();

    if (!loadWindow) {
        loadWindow = new LoadWindow(this);
        connect(loadWindow, &LoadWindow::destroyed, this, &MainWindow::show);
    }

    loadWindow->show();
}
