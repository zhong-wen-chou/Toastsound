#include "scoreeditor.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include<QGroupBox>
#include<QLabel>
#include<QSlider>
#include <QFileDialog>
#include <QtConcurrent>
#include <qthread.h>
#include "pianokeys.h"
#include "song.h"

int selfbpm=120; // 为节拍器准备

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
    QLabel *volumeLabel = new QLabel("90%", this);
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
    QPushButton *editModeButton = new QPushButton("编辑模式", this); // 编辑模式按钮
    exitButton = new QPushButton("退出", this);
    exitButton->setStyleSheet("background-color: #FF4444; color: white;");
    editModeButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #2196F3;"
        "    color: white;"
        "    font-weight: bold;"
        "    border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #0D47A1;"
        "}"
        );
    buttonLayout->addWidget(metronomeButton);
    buttonLayout->addWidget(pauseButton);
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(editModeButton); // 编辑模式按钮
    buttonLayout->addWidget(exitButton);

    rightLayout->addLayout(buttonLayout);
    mainLayout->addLayout(rightLayout, 4); // 右侧区域占4份

    setCentralWidget(centralWidget);

    // 连接滑块信号到显示更新
    connect(tempoSlider, &QSlider::valueChanged, [tempoLabel](int value) {
        tempoLabel->setText(QString("%1 BPM").arg(value));
        initbpm=value;
        selfbpm=value;
    });

    connect(volumeSlider, &QSlider::valueChanged, [volumeLabel](int value) {
        volumeLabel->setText(QString("%1%").arg(value));
        initvolumn=value;
    });

    // 连接退出按钮
    connect(pauseButton, &QPushButton::clicked, this, &ScoreEditor::startluzhi);
    connect(exitButton, &QPushButton::clicked, this, &ScoreEditor::exitRequested);
    connect(saveButton, &QPushButton::clicked, this, &ScoreEditor::savediyscore);
    connect(metronomeButton, &QPushButton::clicked, this, &ScoreEditor::startmebutton_clicked);
    connect(editModeButton, &QPushButton::clicked, this, &ScoreEditor::openEditMode);//连接编辑模式按钮和函数
}

void ScoreEditor::startluzhi()
{
    isstart=true;
    keyLogs.clear(); // 清除旧记录
    qDebug() << "计时器已启动";
    timer.restart();
}

void ScoreEditor::savediyscore()
{
    isstart=false;
    qDebug() << "保存乐谱";
    if (keyLogs.isEmpty()) return;

    // 弹出保存文件窗口
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "保存为 MIDI-like 文本文件",
                                                    "",
                                                    "Text Files (*.txt);;All Files (*)");

    if (fileName.isEmpty()) return;

    // 确保文件名以 .txt 结尾
    if (!fileName.endsWith(".txt")) {
        fileName += ".txt";
    }

    QVector<std::tuple<std::string, qint64, qint64>> sortedLogs = keyLogs;
    std::sort(sortedLogs.begin(), sortedLogs.end(), [](const auto& a, const auto& b) {
        return std::get<1>(a) < std::get<1>(b); // 按开始时间排序
    });


    double quarterNoteMs = 60000.0 / initbpm;
    qint64 lastEndTime = 0;
    Score temp(initbpm);
    Track a;
    for (const auto& log : sortedLogs) {
        const std::string& pitch = std::get<0>(log);
        qint64 start = std::get<1>(log);
        qint64 end = std::get<2>(log);

        // 如果两个音符之间有空隙，加休止符
        if (start > lastEndTime) {
            double restDuration = (start - lastEndTime) / quarterNoteMs;
            if (restDuration >= 0.05) { // 可忽略极短休止
                a.addNote(new Rest(restDuration));
            }
        }

        // 写音符
        double noteDuration = (end - start) / quarterNoteMs;
        a.addNote(new Note(MidiNote::noteNameToMidi(pitch),noteDuration));
        lastEndTime = end;
    }
    temp.addTrack(a);
    temp.save(fileName.toStdString());
}

void ScoreEditor::startmetronome(const Note& menote)
{
    while (isopenme) {
        // 可以在这里播放节拍声（Beep、QSound、或 pianoKeys->playClickSound()）
        menote.constplay(midiOut,selfbpm,9);
        qDebug() << "tick";
        QThread::msleep(60000 / selfbpm); // 按当前 BPM 延时
    }
}

void ScoreEditor::startmebutton_clicked()
{
    //qDebug() << "clicked";
    isopenme = !isopenme;
    if (isopenme) {
        Note menote(77,0.25);
        QtConcurrent::run([this, menote]() {
            this->startmetronome(menote);
        });
    }
}

//编辑模式的槽函数
void ScoreEditor::openEditMode()
{
    if (!editWindow) {
        editWindow = new EditWindow(this);
        connect(editWindow, &QWidget::destroyed, [this]() {
            editWindow = nullptr; // 窗口关闭后重置指针
        });
    }
    hide();
    editWindow->show();
}

ScoreEditor::~ScoreEditor() {}
