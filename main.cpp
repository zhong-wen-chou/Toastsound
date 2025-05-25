#include "mainwindow.h"
#include <QApplication>
#include "song.h"

int main(int argc, char *argv[])
{
    try {
        unsigned int ports = midiOut.getPortCount();
        if (ports == 0) {
            throw std::runtime_error("没有可用的MIDI输出端口");
        }
        midiOut.openPort(0);
        qDebug()<<"成功打开midi设备";
    }
    catch (RtMidiError& error) {
        error.printMessage();
        throw;
    }
    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}
