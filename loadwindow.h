#ifndef LOADWINDOW_H
#define LOADWINDOW_H

#include <QMainWindow>
#include "pianokeys.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>

class LoadWindow : public QMainWindow
{
    Q_OBJECT

public:
    LoadWindow(QWidget *parent = nullptr);
    ~LoadWindow();

private slots:
    void onReturnToMain(); // 返回主界面
    void showVolumeControl(); // 显示音量控制

private:
    void createWidgets(); // 创建控件
    void setupLayout();   // 设置布局
    void connectSignals();// 连接信号

    QVBoxLayout *leftButtonLayout = nullptr;
    QHBoxLayout *bottomButtonLayout = nullptr;

    PianoKeys *pianoKeys; // 钢琴键组件
    QPushButton *returnButton; // 返回按钮
    QPushButton *selectAudioButton; // 选择音频按钮
    QPushButton *playButton; // 开始播放按钮
    QPushButton *stopButton; // 中止按钮
    QPushButton *volumeButton; // 音量按钮

    QSlider *volumeSlider; // 音量滑块（隐藏，点击按钮时显示）
    bool isVolumeVisible = false; // 音量滑块可见状态
};

#endif // LOADWINDOW_H
