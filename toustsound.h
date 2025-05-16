#ifndef TOUSTSOUND_H
#define TOUSTSOUND_H
#include "song.h"
#include<vector>
#include<QString>

enum Instrument
{
    etronome, //节拍器
    piano, //钢琴
    violin //小提琴
};


class note //音符类
{
public:
    int single_time; //单音符时间
    int column; //音量
    note(){}
    note(int single_time,int column):single_time(single_time),column(column){}
    virtual ~note() {}  // 虚析构函数
    virtual void play_single_note() //对某个单音符进行演奏
    {

    }
};

//note衍生出的具体乐器类，如：钢琴类
class piano_note:public note //音符衍生出的乐器类
{
public:
    piano_note(int st,int col)
    {
        single_time=st;
        column=col;
    }
    void play_single_note() //针对单个音符衍生的演奏函数
    {

    }
};
class etronome:public note
{
public:
    etronome(int st,int col)
    {
        single_time=st;
        column=col;
    }
    void play_single_note() //针对单个音符衍生的演奏函数
    {

    }
};

class score //乐谱类
{
public:
    int rhythm; //记录节拍
    int syllable; //记录音节个数
    std::vector<std::vector<int>>original_score; //存储原始乐谱
    std::vector<note>toplay_score; //处理后的乐谱
    score(){}
    score(int rhythm,int syllable,std::vector<std::vector<int>>original_score):rhythm(rhythm),syllable(syllable),original_score(original_score)
    {

    }
    virtual void create_score() //将original_score进行处理，转化为toplay_score
    {

    }
};

//乐谱类衍生出的具体乐器类
//将节拍器视作一种具体乐器来制作乐谱以进行演奏
class etronome_score:public score
{
public:
    etronome_score(){};
    etronome_score(int rhythm,int syllable,std::vector<std::vector<int>>original_score):score(rhythm,syllable,original_score)
    {
        create_score();
    };
    void create_score()
    {

    }
};

class piano_score:public score
{
public:
    piano_score(){};
    piano_score(int rhythm,int syllable,std::vector<std::vector<int>>original_score):score(rhythm,syllable,original_score)
    {
        create_score();
    };
    void create_score()
    {

    }
};

class toustsound
{
public:
    std::vector<score>scores; //包含要演奏的若干张乐谱
    std::vector<std::vector<int>>original_score; //将储存的乐谱转化为这种形式进行保存
    int rhythm; //记录节拍
    int syllable; //记录音节个数
    toustsound();
    void load_score(const QString& filename); //加载乐谱
    void save_score(const QString& filename); //储存乐谱
    void clear_scores(){scores.clear();} //刷新scores，即删除所有乐器
    void generate_etronome(int rhythm); // 直接产生一个节拍器进行持续演奏，直至关闭
    void compose_score(int rhythm,int note); //创作时琴键输入开始录制时调用，创建original_score，注意这是单个单个录入的
    void close_etronome(); //关闭运行的节拍器
    void key_play(Instrument instru,int play_time,int col); //键盘演奏点击单个键后发出声音
    void add_instrument(Instrument instru); //乐谱演奏页面中选择添加要加入演奏的乐器
    void start_to_play(); //点击开始演奏，自动关闭其他所有声音
};

#endif // TOUSTSOUND_H
