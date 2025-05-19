// scoreeditor.cpp
#include "scoreeditor.h"

ScoreEditor::ScoreEditor(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("编曲界面");
    setMinimumSize(800, 600);

    // 创建中央部件和主布局
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // 创建数字谱编辑器
    scoreTextEdit = new QTextEdit(this);
    scoreTextEdit->setPlaceholderText("请在此输入数字乐谱...");
    scoreTextEdit->setFont(QFont("Courier New", 12));
    scoreTextEdit->setMinimumHeight(400);
    mainLayout->addWidget(scoreTextEdit);

    // 创建按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(20);

    // 创建控制按钮
    playButton = new QPushButton("播放", this);
    pauseButton = new QPushButton("暂停", this);
    saveButton = new QPushButton("保存", this);
    exitButton = new QPushButton("退出", this);

    // 设置按钮样式
    QList<QPushButton*> buttons = {playButton, pauseButton, saveButton, exitButton};
    for (auto button : buttons) {
        button->setMinimumSize(120, 40);
        button->setStyleSheet(
            "QPushButton {"
            "    background-color: #165DFF;"
            "    color: white;"
            "    border-radius: 8px;"
            "    font-size: 14px;"
            "}"
            "QPushButton:hover {"
            "    background-color: #0E42B3;"
            "}"
            );
        buttonLayout->addWidget(button);
    }

    mainLayout->addLayout(buttonLayout);
    setCentralWidget(centralWidget);

    // 连接退出按钮信号到槽
    connect(exitButton, &QPushButton::clicked, this, &ScoreEditor::onExitClicked);
}

ScoreEditor::~ScoreEditor()
{
    // 自动释放子控件
}

void ScoreEditor::onExitClicked()
{
    emit exitRequested(); // 发出退出请求信号
}
