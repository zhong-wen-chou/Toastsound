#include "toustsound.h"
#include <QtMultimedia/QMediaPlayer>
#include <fstream>
#include <QFileDialog>
#include<QString>
#include<QMessageBox>
#include<iostream>
toustsound::toustsound() {}

void toustsound::generate_etronome(int rythm) //直接产生一个节拍器进行持续演奏，直至关闭
{

}

void toustsound::compose_score(int rhythm,int note)
{

}

void toustsound::close_etronome() //关闭运行的节拍器
{

}

void toustsound::key_play(Instrument instru,int play_time,int col) //键盘演奏点击单个键后发出声音
{
    note* p;
    switch(instru)
    {
    case 0: //节拍器
        p=new class etronome(play_time,col);
        break;
    case 1: //钢琴
        p=new class piano_note(play_time,col);
        break;
    default:
        p=nullptr;
        std::cout<<"乐器未开发！\n";
    }
    if(p!=nullptr)p->play_single_note(); //依据自身一储存的play_time进行演奏  //此处进行调用
    delete p;
}

void toustsound::add_instrument(Instrument instru) //乐谱演奏页面中选择添加要加入演奏的乐器
{
    switch(instru)
    {
    case 0: //节拍器
        scores.push_back(etronome_score(rhythm,syllable,original_score)); //可能不需要储存一整个score
        break;
    case 1: //钢琴
        scores.push_back(piano_score(rhythm,syllable,original_score));
        break;
    default:
        std::cout<<"乐器未开发！\n";
    }
    //排除重复的还未实现
}

void toustsound::start_to_play() //点击开始演奏，自动关闭其他所有声音
{

}

void toustsound::load_score(const QString& filename)
{
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);

        // 读取节拍和音节
        QString line = in.readLine();
        rhythm = line.section(":", 1).trimmed().toInt();

        line = in.readLine();
        syllable = line.section(":", 1).trimmed().toInt();

        // 跳过 "score:" 行
        in.readLine();

        // 读取乐谱数据
        original_score.clear();
        while (!in.atEnd()) {
            QString rowLine = in.readLine();
            QStringList parts = rowLine.split(" ", Qt::SkipEmptyParts);
            std::vector<int> row;
            for (const QString& part : parts) {
                row.push_back(part.toInt());
            }
            if (!row.empty()) {
                original_score.push_back(row);
            }
        }

        file.close();
        //update();  // 可选：刷新 UI
        QMessageBox::information(nullptr, "加载成功", "乐谱数据已成功从文件加载");
    } else {
        QMessageBox::critical(nullptr, "加载失败", "无法打开文件进行加载");
    }
}
void toustsound::save_score(const QString& filename)
{
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);

        // 写入节拍和音节
        out << "rhythm: " << rhythm << "\n";
        out << "syllable: " << syllable << "\n";
        out << "score:\n";

        // 写入乐谱二维数据
        for (const auto& row : original_score) {
            for (int note : row) {
                out << note << " ";
            }
            out << "\n";
        }

        file.close();
        QMessageBox::information(nullptr, "保存成功", "乐谱数据已成功保存到文件");
    } else {
        QMessageBox::critical(nullptr, "保存失败", "无法打开文件进行保存");
    }
}
