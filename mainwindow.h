#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include "scoreeditor.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onComposeButtonClicked();

private:
    void createWidgets();
    void setupLayout();
    void connectSignals();

    // 添加成员变量保存控件指针
    QLabel *titleLabel = nullptr;
    QLabel *subtitleLabel = nullptr;
    QPushButton *composeButton = nullptr;
    QPushButton *loadButton = nullptr;
    QPushButton *helpButton = nullptr;
    QLabel *versionLabel = nullptr;
    ScoreEditor *scoreEditor = nullptr;
};

#endif // MAINWINDOW_H
