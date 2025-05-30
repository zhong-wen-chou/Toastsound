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

class NoteCanvas : public QWidget {
    Q_OBJECT
public:
    explicit NoteCanvas(QWidget *parent = nullptr);
    void setNotes(const QVector<std::tuple<int, int>>& notes);
    void addNote(int keyIndex, int position); // 添加音符到当前画布
    void clearNotes(); // 清空当前画布的音符
    void updateCanvasSize(); // 更新画布大小
    int getLastNotePosition() const; // 获取最后一个音符的位置

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<std::tuple<int, int>> m_notes;
    int getNoteY(int keyIndex) const;
    int lastNotePosition = 0; // 跟踪当前画布的音符位置
};

class EditWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EditWindow(QWidget *parent = nullptr);
    ~EditWindow();

private slots:
    void onPianoKeyPressed(int keyIndex,int midinum);
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
