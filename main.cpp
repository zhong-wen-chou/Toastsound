#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QStyle>
#include<QLabel>
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 创建主窗口
    QMainWindow window;
    window.setWindowTitle("大吐司");
    window.setMinimumSize(800, 600);

    // 创建中央部件和布局
    QWidget *centralWidget = new QWidget(&window);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);
    layout->setContentsMargins(40, 40, 40, 40);

    // 创建标题标签
    QLabel *titleLabel = new QLabel("音乐编曲", centralWidget);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 32px; font-weight: bold; color: #165DFF;");
    layout->addWidget(titleLabel);

    QLabel *subtitleLabel = new QLabel("Created by 大吐司", centralWidget);
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setStyleSheet("font-size: 16px; color: #555; margin-bottom: 40px;");
    layout->addWidget(subtitleLabel);

    // 创建谱曲按钮
    QPushButton *composeButton = new QPushButton("谱曲", centralWidget);
    composeButton->setObjectName("composeButton");
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
    layout->addWidget(composeButton);

    // 创建读取按钮
    QPushButton *loadButton = new QPushButton("读取", centralWidget);
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
    layout->addWidget(loadButton);

    // 创建帮助按钮
    QPushButton *helpButton = new QPushButton("帮助", centralWidget);
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
    layout->addWidget(helpButton);

    // 添加底部信息
    QLabel *versionLabel = new QLabel("© 大吐司 | 版本 1.0.0？maybe", centralWidget);
    versionLabel->setAlignment(Qt::AlignCenter);
    versionLabel->setStyleSheet("color: #888; margin-top: 40px;");
    layout->addWidget(versionLabel);

    // 设置中央部件
    window.setCentralWidget(centralWidget);


    // 连接按钮点击事件
    QObject::connect(composeButton, &QPushButton::clicked, [&]() {
        qDebug()<<"谱曲按钮被点击";

    });

    QObject::connect(loadButton, &QPushButton::clicked, [&]() {
        // 处理读取按钮点击事件
        qDebug() << "读取按钮被点击";
        // 这里可以添加打开文件对话框的代码

    });

    QObject::connect(helpButton, &QPushButton::clicked, [&]() {
        // 处理帮助按钮点击事件
        qDebug() << "帮助按钮被点击";
        // 这里可以添加显示帮助文档的代码
    });

    // 显示窗口
    window.show();

    return app.exec();
}
