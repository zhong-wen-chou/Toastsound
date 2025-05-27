#ifndef EDITWINDOW_H
#define EDITWINDOW_H

#include <QMainWindow>
#include "pianokeys.h"
#include <QListWidget>
#include <QPaintEvent>
#include <QVector>

class EditWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EditWindow(QWidget *parent = nullptr);
    ~EditWindow();

private slots:
    void onPianoKeyPressed(int keyIndex); // 钢琴键按下时添加音符

private:
    void createWidgets();
    void setupLayout();
    void paintEvent(QPaintEvent *event) override;
    int getTrackY(int keyIndex); // 根据键索引获取音轨Y坐标

    PianoKeys *pianoKeys;
    QListWidget *trackList;
    QVector<std::tuple<int, int>> notes; // 存储音符（键索引, 水平位置）
    int notePosition = 0; // 新音符水平位置（像素）
    const int NOTE_WIDTH = 50; // 音符宽度
    const int TRACK_SPACING = 40; // 音轨间距
    const int BASE_Y = 50; // 瀑布流基准Y坐标
};

#endif // EDITWINDOW_H
