QT       += core gui multimedia concurrent
LIBS += -lwinmm

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    RtMidi.cpp \
    editwindow.cpp \
    loadwindow.cpp \
    main.cpp \
    mainwindow.cpp \
    pianokeys.cpp \
    scoreeditor.cpp \
    song.cpp

HEADERS += \
    RtMidi.h \
    editwindow.h \
    loadwindow.h \
    mainwindow.h \
    pianokeys.h \
    scoreeditor.h \
    song.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
