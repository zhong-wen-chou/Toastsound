#ifndef PIANOKEYS_H
#define PIANOKEYS_H

#include <QWidget>
#include <QPushButton>
#include <QList>
#include <QMap>
#include <QElapsedTimer>

extern int initbpm; // 初始化节奏和音量
extern int initvolumn;
extern bool isstart; // 是否开始录制
extern QElapsedTimer timer; // 计时器s
extern qint64 notebegin; // 按下的时间
extern qint64 noteend; // 松开的时间
extern QVector<std::tuple<std::string, qint64, qint64>> keyLogs;

class PianoKeys : public QWidget
{
    Q_OBJECT

public:
    explicit PianoKeys(QWidget *parent = nullptr);
    void setKeyPressCallback(std::function<void(int)> callback); // 设置按键回调函数

    QList<QPushButton*> whiteKeys;  // 28个白键列表
    QList<QPushButton*> blackKeys;  // 20个黑键列表
    QMap<QKeySequence, int> keyMap; // 键盘按键到琴键的映射
    QMap<QKeySequence,std::string>keytoname;//键盘按键到音名的映射

protected:
    void keyPressEvent(QKeyEvent *event) override;  // 键盘事件处理
    void keyReleaseEvent(QKeyEvent *event) override; // 键盘释放事件处理

private:
    //改为公有成员让editwindow.cpp使用，实际上这几个变量应该设为共有变量也可以
    /**
    QList<QPushButton*> whiteKeys;  // 28个白键列表
    QList<QPushButton*> blackKeys;  // 20个黑键列表
    QMap<QKeySequence, int> keyMap; // 键盘按键到琴键的映射
    QMap<QKeySequence,std::string>keytoname;//键盘按键到音名的映射
    **/
    std::function<void(int)> keyCallback; // 按键回调函数

    void initKeyMap();
    void initKeytoname();
    void createKeys();           // 创建琴键
    void setupLayout();          // 布局管理
    void updateKeyVisual(int keyIndex, bool isPressed); // 更新琴键视觉状态

    void createBlackKey(int whiteIndex, int octave, int blackKeyWidth, int blackKeyHeight, int blackKeyYOffset); // 新增函数声明
    int whiteKeyWidth = 0;
};

#endif // PIANOKEYS_H
