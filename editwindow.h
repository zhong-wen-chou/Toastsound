#ifndef EDITWINDOW_H
#define EDITWINDOW_H

#include <QMainWindow>
#include "pianokeys.h"
#include <QListWidget>
#include <QPaintEvent>
#include <QVector>
#include <QPushButton> // 添加按钮头文件
#include "song.h"
#include <QScrollArea>
#include <QStackedWidget>
#include <QTimer>

class NoteCanvas : public QWidget {
    Q_OBJECT
public:
    const double pixelsPerMs = 0.1;
    const qint64 viewDuration = 2*width() / pixelsPerMs; // 可视时间窗口
    explicit NoteCanvas(QWidget *parent = nullptr);
    void setNotes(const QVector<std::tuple<int, int>>& notes);
    void addNote(int keyIndex, qint64 startTime); // 添加音符到当前画布
    qint64 currentTime() const; // 获取当前时间
    void releaseNote(int keyIndex, qint64 endTime);
    void clearNotes(); // 清空当前画布的音符
    void updateCanvasSize(); // 更新画布大小
    int getLastNotePosition() const; // 获取最后一个音符的位置
    struct NoteBlock {
        int pitch;         // 如 "C4"
        qint64 startTime;      // 单位：ms
        qint64 endTime;        // 若正在演奏，则为 -1 或设为当前时间
        bool active;           // 是否还在按下状态
    };

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<NoteBlock> NoteBlocks;
    int getNoteY(int keyIndex) const;
    QElapsedTimer edittimer;
    QTimer *refreshTimer;
    //QMap<QString, int> pitchToY;  // 映射 pitch 到 y 轴
    //qint64 viewWindowMs = 8000;   // 当前窗口显示过去 8 秒
    int lastNotePosition = 0; // 跟踪当前画布的音符位置
};

class EditWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EditWindow(QWidget *parent = nullptr);
    ~EditWindow();

private slots:
    void onPianoKeyPressed(int keyIndex);
    void onPianoKeyReleased(int keyIndex);
    void exitToMain();
    void startPlayback();
    void saveScore();

    void updateTrackList();
    void addTrack();
    void removeCurrentTrack();
    void switchTrack(int index);

    void startmetronome(const Note& menote);
    void startmebutton_clicked();

private:
    void createWidgets();
    void setupLayout();
    void updateNoteVisual(int keyIndex, bool isPressed);
    void connectSignals();

    PianoKeys *pianoKeys;
    QListWidget *trackList;
    QPushButton *exitButton, *playButton, *saveButton;
    QPushButton *addTrackButton, *removeTrackButton;

    // 改为存储每个音轨的画布
    QList<NoteCanvas*> canvases;  // 每个音轨对应一个画布
    QStackedWidget *stackedWidget; // 堆栈窗口管理多个画布

    Score* score;
    int currentTrackIndex = 0;
    int selfbpm;
    bool isopenme = false;
};

#endif // EDITWINDOW_H
