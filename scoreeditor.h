// scoreeditor.h
#ifndef SCOREEDITOR_H
#define SCOREEDITOR_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>

class ScoreEditor : public QMainWindow
{
    Q_OBJECT
public:
    explicit ScoreEditor(QWidget *parent = nullptr);
    ~ScoreEditor();

signals:
    void exitRequested(); // 退出信号，用于返回主界面

private slots:
    void onExitClicked(); // 处理退出按钮点击

private:
    QTextEdit *scoreTextEdit; // 数字谱编辑器
    QPushButton *playButton;  // 播放按钮
    QPushButton *pauseButton; // 暂停按钮
    QPushButton *saveButton;  // 保存按钮
    QPushButton *exitButton;  // 退出按钮
};

#endif // SCOREEDITOR_H
