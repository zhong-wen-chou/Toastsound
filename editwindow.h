#ifndef EDITWINDOW_H
#define EDITWINDOW_H

#include <QMainWindow>
#include "pianokeys.h"
#include <QListWidget>
#include <QPaintEvent>
#include <QVector>
#include <QPushButton> // 添加按钮头文件

class EditWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EditWindow(QWidget *parent = nullptr);
    ~EditWindow();

private slots:
    void onPianoKeyPressed(int keyIndex); // 钢琴键按下
    void onPianoKeyReleased(int keyIndex); // 钢琴键释放
    void exitToMain(); // 退出到主界面
    void startPlayback(); // 开始播放
    void saveScore(); // 保存乐谱

private:
    void createWidgets();
    void setupLayout();
    void paintEvent(QPaintEvent *event) override;
    int getTrackY(int keyIndex);
    void updateNoteVisual(int keyIndex, bool isPressed); // 更新琴键视觉
    void connectSignals();

    PianoKeys *pianoKeys;
    QListWidget *trackList;
    QVector<std::tuple<int, int>> notes;
    int notePosition = 0;
    const int NOTE_WIDTH = 50;
    const int TRACK_SPACING = 20; // 减小音轨间距为20像素
    const int BASE_Y = 20; // 基准Y坐标
    QPushButton *exitButton, *playButton, *saveButton; // 新增底部按钮
    const QColor BLACK_KEY_COLOR = QColor("#888888"); // 黑键音符颜色（深灰）
    const QColor WHITE_KEY_COLOR = QColor("#2196F3"); // 白键音符颜色（蓝色）
};

#endif // EDITWINDOW_H
