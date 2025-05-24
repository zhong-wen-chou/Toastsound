#ifndef SCOREEDITOR_H
#define SCOREEDITOR_H

#include <QMainWindow>
#include "pianokeys.h" // 添加钢琴键头文件

class ScoreEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit ScoreEditor(QWidget *parent = nullptr);
    ~ScoreEditor();

signals:
    void exitRequested();

private:
    PianoKeys *pianoKeys = nullptr; // 钢琴键组件
    QPushButton *exitButton = nullptr; // 退出按钮

    void createButtonLayout(); // 创建按钮布局
};

#endif // SCOREEDITOR_H
