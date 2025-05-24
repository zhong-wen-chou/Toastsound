#include "scoreeditor.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

ScoreEditor::ScoreEditor(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("钢琴键盘");
    setMinimumSize(1200, 300);

    // 创建中央部件和布局
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // 添加钢琴键组件
    pianoKeys = new PianoKeys(centralWidget);
    mainLayout->addWidget(pianoKeys, 1);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 20, 0, 20);
    buttonLayout->setSpacing(15);

    QPushButton *playButton = new QPushButton("播放", this);
    QPushButton *pauseButton = new QPushButton("暂停", this);
    QPushButton *saveButton = new QPushButton("保存", this);
    exitButton = new QPushButton("退出", this);
    exitButton->setStyleSheet("background-color: #FF4444; color: white;");

    buttonLayout->addWidget(playButton);
    buttonLayout->addWidget(pauseButton);
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(exitButton);

    mainLayout->addLayout(buttonLayout);


    setCentralWidget(centralWidget);

    // 连接退出按钮
    connect(exitButton, &QPushButton::clicked, this, &ScoreEditor::exitRequested);
}



ScoreEditor::~ScoreEditor() {}
