#include "pianokeys.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStyleOption>
#include <QPainter>
#include <QDebug>
#include<QMap>
#include<QKeySequence>
#include <QKeyEvent>
#include "song.h"

// 琴键尺寸定义
const int WHITE_KEY_WIDTH = 35;   // 缩小宽度以适应更多琴键
const int BLACK_KEY_WIDTH = 24;   // 黑键宽度比例
const int KEY_HEIGHT = 100;
const int OCTAVE_WHITE_KEYS = 7;  // 每八度白键数
const int OCTAVE_BLACK_KEYS = 5;  // 每八度黑键数
const int OCTAVE_COUNT = 4;       // 4个八度

PianoKeys::PianoKeys(QWidget *parent) : QWidget(parent)
{
    int totalWhiteKeys = OCTAVE_WHITE_KEYS * OCTAVE_COUNT;
    int totalBlackKeys = OCTAVE_BLACK_KEYS * OCTAVE_COUNT;
    setMinimumSize(totalWhiteKeys * WHITE_KEY_WIDTH + 100, KEY_HEIGHT + 50); // 动态计算宽度
    createKeys();
    setupLayout();
    initKeyMap(); // 初始化键盘映射
}

void PianoKeys::createKeys()
{
    whiteKeyWidth = 1200 / (OCTAVE_WHITE_KEYS * OCTAVE_COUNT);
    int blackKeyWidth = whiteKeyWidth * (BLACK_KEY_WIDTH / (float)WHITE_KEY_WIDTH);
    int blackKeyHeight = KEY_HEIGHT; // 黑键高度与白键一致
    int blackKeyYOffset = KEY_HEIGHT - blackKeyHeight; // 黑键顶部偏移量，实现下端重合

    // 创建白键
    for (int i = 0; i < OCTAVE_WHITE_KEYS * OCTAVE_COUNT; ++i) {
        QPushButton *key = new QPushButton(this);
        key->setFixedSize(whiteKeyWidth, KEY_HEIGHT);
        key->setStyleSheet("QPushButton { background: white; border: 1px solid #333; }"
                           "QPushButton:pressed { background: #f0f0f0; }");
        whiteKeys.append(key);
    }

    // 创建黑键（每组5个，共4组）
    int blackKeyIndex = 0;
    for (int octave = 0; octave < OCTAVE_COUNT; ++octave) {
        // 两黑一组的黑键（C#、D#）
        int whiteIndex = octave * OCTAVE_WHITE_KEYS + 1;
        createBlackKey(whiteIndex, octave, blackKeyWidth, blackKeyHeight, blackKeyYOffset);
        blackKeyIndex++;

        whiteIndex = octave * OCTAVE_WHITE_KEYS + 2; // 原代码此处为+3，修正为+2（两黑组应为C#1和D#1，对应白键1和2之间）
        createBlackKey(whiteIndex, octave, blackKeyWidth, blackKeyHeight, blackKeyYOffset);
        blackKeyIndex++;

        // 三黑一组的黑键（F#、G#、A#）
        whiteIndex = octave * OCTAVE_WHITE_KEYS + 4;
        createBlackKey(whiteIndex, octave, blackKeyWidth, blackKeyHeight, blackKeyYOffset);
        blackKeyIndex++;

        whiteIndex = octave * OCTAVE_WHITE_KEYS + 5;
        createBlackKey(whiteIndex, octave, blackKeyWidth, blackKeyHeight, blackKeyYOffset);
        blackKeyIndex++;

        whiteIndex = octave * OCTAVE_WHITE_KEYS + 6;
        createBlackKey(whiteIndex, octave, blackKeyWidth, blackKeyHeight, blackKeyYOffset);
        blackKeyIndex++;
    }
}

// 新增辅助函数创建黑键
void PianoKeys::createBlackKey(int whiteIndex, int octave, int blackKeyWidth, int blackKeyHeight, int blackKeyYOffset) {
    QPushButton *key = new QPushButton(this);
    key->setFixedSize(blackKeyWidth, blackKeyHeight);
    key->move(
        whiteIndex * whiteKeyWidth + (whiteKeyWidth - blackKeyWidth) / 2, // 水平居中
        blackKeyYOffset // 垂直偏移，使黑键下端与白键上端重合
        );
    key->setStyleSheet("QPushButton { background: black; border: none; }"
                       "QPushButton:pressed { background: #333; }");
    blackKeys.append(key);
}


void PianoKeys::setupLayout()
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 添加白键到布局
    for (QPushButton *key : whiteKeys) {
        mainLayout->setAlignment(key, Qt::AlignBottom); // 底部对齐
        mainLayout->addWidget(key);
    }

    setLayout(mainLayout);
}

void PianoKeys::initKeyMap()
{
    keyMap.clear(); // 清空映射表

    // 第一八度：C1-B1（白键索引0-6，黑键索引28-32）
    keyMap = {
              // 白键
              {Qt::Key_A, 0},    {Qt::Key_S, 1},    {Qt::Key_D, 2},    {Qt::Key_F, 3},
              {Qt::Key_G, 4},    {Qt::Key_H, 5},    {Qt::Key_J, 6},
              // 黑键（C#1, D#1, F#1, G#1, A#1）
              {Qt::Key_W, 28},   {Qt::Key_E, 29},   {Qt::Key_R, 30},   {Qt::Key_T, 31},
              {Qt::Key_Y, 32},
              };

    // 第二八度：C2-B2（白键索引7-13，黑键索引33-37）
    keyMap.insert({
        {Qt::Key_K, 7},    {Qt::Key_L, 8},    {Qt::Key_Semicolon, 9}, {Qt::Key_Apostrophe, 10},
        {Qt::Key_Z, 11},   {Qt::Key_X, 12},   {Qt::Key_C, 13},
        {Qt::Key_U, 33},   {Qt::Key_O, 34},   {Qt::Key_P, 35},   {Qt::Key_BracketLeft, 36},
        {Qt::Key_BracketRight, 37}, // 右方括号键
    });

    // 第三八度：C3-B3（白键索引14-20，黑键索引38-42）
    keyMap.insert({
                   {Qt::Key_V, 14},   {Qt::Key_B, 15},   {Qt::Key_N, 16},   {Qt::Key_M, 17},
                   {Qt::Key_Comma, 18}, {Qt::Key_Period, 19}, {Qt::Key_Slash, 20},
                   {Qt::Key_2, 38},   {Qt::Key_3, 39},   {Qt::Key_5, 40},   {Qt::Key_6, 41},
                   {Qt::Key_7, 42},
                   });

    // 第四八度：C4-B4（白键索引21-27，黑键索引43-47）
    keyMap.insert({
                   {Qt::Key_4, 21},   {Qt::Key_5, 22},   {Qt::Key_6, 23},   {Qt::Key_7, 24},
                   {Qt::Key_8, 25},   {Qt::Key_9, 26},   {Qt::Key_0, 27},
                   {Qt::Key_Minus, 43}, {Qt::Key_Equal, 44}, {Qt::Key_BracketRight, 45},
                   {Qt::Key_Slash, 46}, {Qt::Key_Colon, 47},
                   });
}

void PianoKeys::keyPressEvent(QKeyEvent *event)
{
    if (keyMap.contains(event->key())) {
        int keyIndex = keyMap[event->key()];
        updateKeyVisual(keyIndex, true);
        Note tmpnote(60+keyIndex,0.25);
        tmpnote.play(midiOut, 120);
        if (keyCallback) keyCallback(keyIndex); // 触发回调函数
        event->accept();
    }
}

void PianoKeys::keyReleaseEvent(QKeyEvent *event)
{
    if (keyMap.contains(event->key())) {
        int keyIndex = keyMap[event->key()];
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
