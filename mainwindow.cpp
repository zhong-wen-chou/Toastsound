#include "mainwindow.h"
#include "loadwindow.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("大吐司");
    setMinimumSize(800, 600);
    createWidgets(); // 创建控件
    setupLayout();   // 设置布局
    connectSignals();// 连接信号
}

MainWindow::~MainWindow()
{
    delete scoreEditor;
}

void MainWindow::createWidgets()
{
    // 创建标题标签
    QLabel *titleLabel = new QLabel("音乐编曲", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 32px; font-weight: bold; color: #165DFF;");

    // 创建副标题标签
    QLabel *subtitleLabel = new QLabel("Created by 大吐司", this);
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet("font-size: 16px; color: #555; margin-bottom: 40px;");

    // 创建谱曲按钮
    composeButton = new QPushButton("谱曲", this);
    composeButton->setMinimumSize(200, 60);
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

    // 创建读取按钮和帮助按钮
    loadButton = new QPushButton("读取", this);
    loadButton->setMinimumSize(200, 60);
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
    helpButton->setMinimumSize(200, 60);
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

    // 创建底部标签
    QLabel *versionLabel = new QLabel("© 大吐司 | 版本 1.0.0？maybe", this);
    versionLabel->setAlignment(Qt::AlignCenter);
    versionLabel->setStyleSheet("color: #888; margin-top: 40px;");


}

void MainWindow::setupLayout()
{
    // 创建中央部件和垂直布局
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);
    layout->setContentsMargins(40, 40, 40, 40);

    // 按顺序添加控件到布局
    layout->addWidget(titleLabel);
    layout->addWidget(subtitleLabel);
    layout->addWidget(composeButton);
    layout->addWidget(loadButton);
    layout->addWidget(helpButton);
    //layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
    layout->addWidget(versionLabel);
    setCentralWidget(centralWidget);
}

void MainWindow::connectSignals()
{
    connect(composeButton, &QPushButton::clicked, this, &MainWindow::onComposeButtonClicked);


    connect(loadButton, &QPushButton::clicked, this, &MainWindow::onLoadButtonClicked);


    connect(helpButton, &QPushButton::clicked, []() {
        qDebug() << "帮助按钮被点击";
    });
}

void MainWindow::onComposeButtonClicked()
{
    qDebug() << "跳转到编曲界面";
    hide(); // 隐藏主窗口

    if (!scoreEditor) {
        scoreEditor = new ScoreEditor(this); // 父对象为主窗口
        connect(scoreEditor, &ScoreEditor::exitRequested, this, [this]() {
            scoreEditor->hide();
            show(); // 返回主窗口
        });
    }

    scoreEditor->show();
}

void MainWindow::onLoadButtonClicked()
{
    qDebug() << "跳转到读取界面";
    hide();

    if (!loadWindow) {
        loadWindow = new LoadWindow(this);
        connect(loadWindow, &LoadWindow::destroyed, this, &MainWindow::show); // 返回时显示主窗口
    }

    loadWindow->show();
}
