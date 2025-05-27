#ifndef SCOREEDITOR_H
#define SCOREEDITOR_H

#include <QMainWindow>
#include "pianokeys.h" // 添加钢琴键头文件
#include "song.h"
#include "editwindow.h"

extern int selfbpm;

class ScoreEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit ScoreEditor(QWidget *parent = nullptr);
    bool isopenme=false; // 调音器是否打开
    ~ScoreEditor();

signals:
    void exitRequested();

private slots:
    void startluzhi();
    void savediyscore();
    void startmebutton_clicked();
    void startmetronome(const Note& menote);

    void openEditMode();//打开编辑模式

private:
    PianoKeys *pianoKeys = nullptr; // 钢琴键组件
    QPushButton *exitButton = nullptr; // 退出按钮
    EditWindow *editWindow = nullptr; // 编辑窗口指针

    void createButtonLayout(); // 创建按钮布局
    void creatediyscore(); // 标准化乐谱并储存
};

#endif // SCOREEDITOR_H
