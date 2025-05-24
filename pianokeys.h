#ifndef PIANOKEYS_H
#define PIANOKEYS_H

#include <QWidget>
#include <QPushButton>
#include <QList>
#include <QMap>

class PianoKeys : public QWidget
{
    Q_OBJECT

public:
    explicit PianoKeys(QWidget *parent = nullptr);
    void setKeyPressCallback(std::function<void(int)> callback); // 设置按键回调函数

protected:
    void keyPressEvent(QKeyEvent *event) override;  // 键盘事件处理
    void keyReleaseEvent(QKeyEvent *event) override; // 键盘释放事件处理

private:
    QList<QPushButton*> whiteKeys;  // 28个白键列表
    QList<QPushButton*> blackKeys;  // 20个黑键列表
    QMap<QKeySequence, int> keyMap; // 键盘按键到琴键的映射
    std::function<void(int)> keyCallback; // 按键回调函数

    void initKeyMap();

    void createKeys();           // 创建琴键
    void setupLayout();          // 布局管理
    void updateKeyVisual(int keyIndex, bool isPressed); // 更新琴键视觉状态
};

#endif // PIANOKEYS_H
