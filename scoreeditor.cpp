#include "scoreeditor.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include<QGroupBox>
#include<QLabel>
#include<QSlider>
ScoreEditor::ScoreEditor(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("钢琴键盘");
    setMinimumSize(1500, 400);

    // 创建中央部件
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget); // 修改为主水平布局

    // === 左侧控制面板 ===
    QVBoxLayout *controlPanelLayout = new QVBoxLayout();
    controlPanelLayout->setSpacing(20);
    controlPanelLayout->setContentsMargins(10, 10, 10, 10);

    // 节奏控制
    QGroupBox *tempoGroup = new QGroupBox("节奏", this);
    QHBoxLayout *tempoLayout = new QHBoxLayout(tempoGroup);
    QLabel *tempoLabel = new QLabel("120 BPM", this);
    QSlider *tempoSlider = new QSlider(Qt::Horizontal, this);
    tempoSlider->setRange(40, 200);
    tempoSlider->setValue(120);
    tempoLayout->addWidget(new QLabel("慢", this));
    tempoLayout->addWidget(tempoSlider);
    tempoLayout->addWidget(new QLabel("快", this));
    tempoLayout->addWidget(tempoLabel);
    controlPanelLayout->addWidget(tempoGroup);

    // 音量控制
    QGroupBox *volumeGroup = new QGroupBox("音量", this);
    QHBoxLayout *volumeLayout = new QHBoxLayout(volumeGroup);
    QLabel *volumeLabel = new QLabel("70%", this);
    QSlider *volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(70);
    volumeLayout->addWidget(new QLabel("静音", this));
    volumeLayout->addWidget(volumeSlider);
    volumeLayout->addWidget(new QLabel("最大", this));
    volumeLayout->addWidget(volumeLabel);
    controlPanelLayout->addWidget(volumeGroup);

    controlPanelLayout->addStretch(); // 填充剩余空间
    mainLayout->addLayout(controlPanelLayout, 1); // 左侧面板占1份

    // === 右侧原有内容 ===
    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->setContentsMargins(0, 0, 0, 0);

    // 添加钢琴键组件
    pianoKeys = new PianoKeys(centralWidget);
    rightLayout->addWidget(pianoKeys, 1);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 20, 0, 20);
    buttonLayout->setSpacing(15);

    QPushButton *metronomeButton = new QPushButton("节拍器", this);
    QPushButton *pauseButton = new QPushButton("开始录制", this);
    QPushButton *saveButton = new QPushButton("保存曲谱", this);
    exitButton = new QPushButton("退出", this);
    exitButton->setStyleSheet("background-color: #FF4444; color: white;");

    buttonLayout->addWidget(metronomeButton);
    buttonLayout->addWidget(pauseButton);
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(exitButton);

    rightLayout->addLayout(buttonLayout);
    mainLayout->addLayout(rightLayout, 4); // 右侧区域占4份

    setCentralWidget(centralWidget);

    // 连接滑块信号到显示更新
    connect(tempoSlider, &QSlider::valueChanged, [tempoLabel](int value) {
        tempoLabel->setText(QString("%1 BPM").arg(value));
    });

    connect(volumeSlider, &QSlider::valueChanged, [volumeLabel](int value) {
        volumeLabel->setText(QString("%1%").arg(value));
    });

    // 连接退出按钮
    connect(exitButton, &QPushButton::clicked, this, &ScoreEditor::exitRequested);
}



ScoreEditor::~ScoreEditor() {}
