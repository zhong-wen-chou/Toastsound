#include "pianokeys.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStyleOption>
#include <QPainter>
#include <QDebug>
#include<QMap>
#include<QKeySequence>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QVector>
#include <QPair>
#include <QString>

#include "song.h"
#include "scoreeditor.h"
// 琴键尺寸定义
const int WHITE_KEY_WIDTH = 35;   // 缩小宽度以适应更多琴键
const int BLACK_KEY_WIDTH = 24;   // 黑键宽度比例
const int WHITE_KEY_HEIGHT = 150; // 增加白键高度
const int BLACK_KEY_HEIGHT = 90;  // 黑键高度为白键高度的60%
const int OCTAVE_WHITE_KEYS = 7;  // 每八度白键数
const int OCTAVE_BLACK_KEYS = 5;  // 每八度黑键数
const int OCTAVE_COUNT = 4;       // 4个八度
int initbpm=120; // 初始化节奏和音量
int initvolumn=90;
bool isstart=false; // 是否开始录制
QElapsedTimer timer; // 计时器
qint64 notebegin; // 按下的时间
qint64 noteend; // 松开的时间
QVector<std::tuple<std::string, qint64, qint64>> keyLogs;


PianoKeys::PianoKeys(QWidget *parent) : QWidget(parent)
{
    int totalWhiteKeys = OCTAVE_WHITE_KEYS * OCTAVE_COUNT;
    //int totalBlackKeys = OCTAVE_BLACK_KEYS * OCTAVE_COUNT;
    setMinimumSize(totalWhiteKeys * WHITE_KEY_WIDTH , WHITE_KEY_HEIGHT ); // 动态计算宽度
    createKeys();
    //setupLayout();
    initKeyMap(); // 初始化键盘映射
    initKeytoname();//初始化键盘到音名映射
}

void PianoKeys::createKeys()
{
    // 创建白键
    for (int i = 0; i < OCTAVE_WHITE_KEYS * OCTAVE_COUNT; ++i) {
        QPushButton *key = new QPushButton(this);
        key->setGeometry(i * WHITE_KEY_WIDTH, 0, WHITE_KEY_WIDTH, WHITE_KEY_HEIGHT);
        key->setStyleSheet("QPushButton { background: white; border: 1px solid #333; }"
                           "QPushButton:pressed { background: #f0f0f0; }");
        whiteKeys.append(key);
    }

    // 创建黑键
    for (int octave = 0; octave < OCTAVE_COUNT; ++octave) {
        int baseIndex = octave * OCTAVE_WHITE_KEYS;

        // 黑键位置：位于相邻白键之间
        createBlackKey(baseIndex + 1, baseIndex); // C#
        createBlackKey(baseIndex + 2, baseIndex + 1); // D#
        createBlackKey(baseIndex + 4, baseIndex + 3); // F#
        createBlackKey(baseIndex + 5, baseIndex + 4); // G#
        createBlackKey(baseIndex + 6, baseIndex + 5); // A#
    }
}

// 创建黑键
void PianoKeys::createBlackKey(int positionIndex, int leftWhiteIndex)
{
    QPushButton *key = new QPushButton(this);

    // 精确计算水平位置：位于左侧白键的右侧
    int xPos = leftWhiteIndex * WHITE_KEY_WIDTH + WHITE_KEY_WIDTH - (BLACK_KEY_WIDTH / 2);

    // 关键修改：使用相同的y坐标0，确保顶部对齐
    key->setGeometry(xPos, 0, BLACK_KEY_WIDTH, BLACK_KEY_HEIGHT);
    key->setStyleSheet("QPushButton { background: black; border: none; }"
                       "QPushButton:pressed { background: #333; }");
    key->raise(); // 确保黑键显示在白键上方
    blackKeys.append(key);
}




void PianoKeys::initKeyMap()
{
    keyMap.clear(); // 清空映射表

    // 第一八度：C4-B4（白键索引7-13，黑键索引33-37）
    keyMap = {
              // 白键
              {Qt::Key_A, 7},    {Qt::Key_S, 8},    {Qt::Key_D, 9},    {Qt::Key_F, 10},
              {Qt::Key_G, 11},    {Qt::Key_H, 12},    {Qt::Key_J, 13},
              // 黑键（C#1, D#1, F#1, G#1, A#1）
              {Qt::Key_W, 33},   {Qt::Key_E, 34},   {Qt::Key_R, 35},   {Qt::Key_T, 36},
              {Qt::Key_Y, 37},
              };

    // 第二八度：C5-B5（白键索引14-20，黑键索引38-42）
    keyMap.insert({
        {Qt::Key_K, 14},    {Qt::Key_L, 15},    {Qt::Key_Semicolon, 16}, {Qt::Key_Apostrophe, 17},
        {Qt::Key_Z, 18},   {Qt::Key_X, 19},   {Qt::Key_C, 20},
        {Qt::Key_U, 38},   {Qt::Key_O, 39},   {Qt::Key_P, 40},   {Qt::Key_BracketLeft, 41},
        {Qt::Key_BracketRight, 42}, // 右方括号键
    });

    // 第三八度：C6-B6（白键索引21-27，黑键索引43-47）
    keyMap.insert({
                   {Qt::Key_V, 21},   {Qt::Key_B, 22},   {Qt::Key_N, 23},   {Qt::Key_M, 24},
                   {Qt::Key_Comma, 25}, {Qt::Key_Period, 26}, {Qt::Key_Slash, 27},
                   {Qt::Key_2, 43},   {Qt::Key_3, 44},   {Qt::Key_5, 45},   {Qt::Key_6, 46},
                   {Qt::Key_7, 47},
                   });

    // 第四八度：C3-B3（白键索引0-6，黑键索引28-32）
    keyMap.insert({
                   {Qt::Key_4, 0},   {Qt::Key_5, 1},   {Qt::Key_6, 2},   {Qt::Key_7, 3},
                   {Qt::Key_8, 4},   {Qt::Key_9, 5},   {Qt::Key_0, 6},
                   {Qt::Key_Minus, 28}, {Qt::Key_Equal, 29}, {Qt::Key_BracketRight, 30},
                   {Qt::Key_Slash, 31}, {Qt::Key_Colon, 32},
                   });
}
void PianoKeys::initKeytoname()
{
    keytoname.clear(); // 清空映射表

    // 第一八度：C4-B4（白键索引0-6，黑键索引28-32）
    keytoname = {
                 // 白键
                 { Qt::Key_A, "c4" }, { Qt::Key_S, "d4" }, { Qt::Key_D, "e4" }, { Qt::Key_F, "f4" },
                 { Qt::Key_G, "g4" }, { Qt::Key_H, "a4" }, { Qt::Key_J, "b4" },
                 // 黑键（C#4, D#4, F#4, G#4, A#4）
                 { Qt::Key_W, "c4s" }, { Qt::Key_E, "d4s" }, { Qt::Key_R, "f4s" }, { Qt::Key_T, "g4s" },
                 { Qt::Key_Y, "a4s" },
                 };

    // 第二八度：C5-B5（白键索引7-13，黑键索引33-37）
    keytoname.insert({
        // 白键
        { Qt::Key_K, "c5" }, { Qt::Key_L, "d5" }, { Qt::Key_Semicolon, "e5" }, { Qt::Key_Apostrophe, "f5" },
        { Qt::Key_Z, "g5" }, { Qt::Key_X, "a5" }, { Qt::Key_C, "b5" },
        // 黑键（C#5, D#5, F#5, G#5, A#5）
        { Qt::Key_U, "c5s" }, { Qt::Key_O, "d5s" }, { Qt::Key_P, "f5s" }, { Qt::Key_BracketLeft, "g5s" },
        { Qt::Key_BracketRight, "a5s" }, // 右方括号键
    });

    // 第三八度：C6-B6（白键索引14-20，黑键索引38-42）
    keytoname.insert({
                      // 白键
                      { Qt::Key_V, "c6" }, { Qt::Key_B, "d6" }, { Qt::Key_N, "e6" }, { Qt::Key_M, "f6" },
                      { Qt::Key_Comma, "g6" }, { Qt::Key_Period, "a6" }, { Qt::Key_Slash, "b6" },
                      // 黑键（C#6, D#6, F#6, G#6, A#6）
                      { Qt::Key_2, "c6s" }, { Qt::Key_3, "d6s" }, { Qt::Key_4, "f6s" }, { Qt::Key_5, "g6s" },
                      { Qt::Key_7, "a6s" },
                      });

    // 第四八度：C3-B3（白键索引21-27，黑键索引43-47）
    keytoname.insert({
                      // 白键
                      { Qt::Key_4, "c3" }, { Qt::Key_5, "d3" }, { Qt::Key_6, "e3" }, { Qt::Key_7, "f3" },
                      { Qt::Key_8, "g3" }, { Qt::Key_9, "a3" }, { Qt::Key_0, "b3" },
                      // 黑键（C#3, D#3, F#3, G#3, A#3）
                      { Qt::Key_Minus, "c3s" }, { Qt::Key_Equal, "d3s" }, { Qt::Key_BracketRight, "f3s" },
                      { Qt::Key_Slash, "g3s" }, { Qt::Key_Colon, "a3s" },
                      });
}


void PianoKeys::keyPressEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        // 忽略重复按键事件
        return;
    }
    if (keyMap.contains(event->key())) {
        int keyIndex=keyMap[event->key()];
        if(isstart){
            notebegin=timer.elapsed();  // 返回毫秒数
        }
        int midinum= MidiNote::noteNameToMidi(keytoname[event->key()]);
        qDebug()<<keytoname[event->key()];
        updateKeyVisual(keyIndex, true);
        Note tmpnote(midinum,100.0);
        //tmpnote.play(midiOut,selfbpm);
        tmpnote.NoteOn(midiOut);
        if (keyCallback) keyCallback(keyIndex); // 触发回调函数
        event->accept();
    }
}

void PianoKeys::keyReleaseEvent(QKeyEvent *event)
{
    if (event->isAutoRepeat()) {
        // 忽略重复释放事件
        return;
    }
    if (keyMap.contains(event->key())) {
        int keyIndex = keyMap[event->key()];
        int midinum= MidiNote::noteNameToMidi(keytoname[event->key()]);
        Note tmpnote(midinum,100.0);
        tmpnote.NoteOff(midiOut);
        if(isstart){
            noteend=timer.elapsed();  // 返回毫秒数
            keyLogs.append(std::make_tuple(keytoname[event->key()], noteend,notebegin));
        }
        updateKeyVisual(keyIndex, false);
        event->accept();
    }
}

void PianoKeys::updateKeyVisual(int keyIndex, bool isPressed)
{
    if (keyIndex < whiteKeys.size()) {
        // 白键处理
        QPushButton *key = whiteKeys[keyIndex];
        key->setDown(isPressed);
    } else {
        // 黑键处理
        int blackIndex = keyIndex - whiteKeys.size();
        if (blackIndex >= 0 && blackIndex < blackKeys.size()) {
            QPushButton *key = blackKeys[blackIndex];
            key->setDown(isPressed);
        }
    }
}

void PianoKeys::setKeyPressCallback(std::function<void(int)> callback)
{
    keyCallback = callback;
}
