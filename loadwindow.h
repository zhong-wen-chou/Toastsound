#ifndef LOADWINDOW_H
#define LOADWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>
#include <QListWidget>
#include <QStackedWidget>
#include "song.h"
//#include "editwindow.h"

class old_Notecanvas : public QWidget {
    Q_OBJECT
public:
    explicit old_Notecanvas(QWidget *parent = nullptr);
    void setNotes(const QVector<std::tuple<int, int>>& notes);
    void addNote(int keyIndex, int position); // 添加音符到当前画布
    void clearNotes(); // 清空当前画布的音符
    void updateCanvasSize(); // 更新画布大小
    int getLastNotePosition() const; // 获取最后一个音符的位置

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<std::tuple<int, int>> m_notes;
    int getNoteY(int keyIndex) const;
    int lastNotePosition = 0; // 跟踪当前画布的音符位置
};

class LoadWindow : public QMainWindow
{
    Q_OBJECT

public:
    LoadWindow(QWidget *parent = nullptr);
    ~LoadWindow();

private slots:
    void onReturnToMain(); // 返回主界面
    void showVolumeControl(); // 显示音量控制
    void selectAudio();
    void beginplay();
    void stopplay();
    void addTrack();        // 添加音轨
    void removeCurrentTrack(); // 删除当前音轨
    void switchTrack(int index); // 切换音轨

private:
    void createWidgets(); // 创建控件
    void setupLayout();   // 设置布局
    void connectSignals();// 连接信号

    Score score;
    QListWidget *trackList; // 音轨列表
    QStackedWidget *stackedWidget; // 堆栈窗口管理多个画布
    QList<old_Notecanvas*> canvases; // 每个音轨对应的画布

    // 按钮
    QPushButton *returnButton; // 返回按钮
    QPushButton *selectAudioButton; // 选择音频按钮
    QPushButton *playButton; // 开始播放按钮
    QPushButton *stopButton; // 中止按钮
    QPushButton *volumeButton; // 音量按钮
    QPushButton *addTrackButton; // 添加音轨按钮
    QPushButton *removeTrackButton; // 删除音轨按钮

    QSlider *volumeSlider; // 音量滑块（隐藏，点击按钮时显示）
    bool isVolumeVisible = false; // 音量滑块可见状态
    int currentTrackIndex = 0; // 当前音轨索引
};

#endif // LOADWINDOW_H
