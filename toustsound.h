#ifndef TOUSTSOUND_H
#define TOUSTSOUND_H
#include<vector>


class note //音符类
{
public:
    note();
    virtual void play_single_note(); //对某个单音符进行演奏
};

//note衍生出的具体乐器类，如：钢琴类
class piano_note:public note //音符衍生出的乐器类
{
public:
    void play_single_note(); //针对单个音符衍生的演奏函数
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
    virtual void creat_score() //将original_score进行处理，转化为toplay_score
    {

    }
};

//乐谱类衍生出的具体乐器类
//将节拍器视作一种具体乐器来制作乐谱以进行演奏
class etronome_score:public score
{
public:
    void creat_score()
    {

    }
};

class piano_score:public score
{
public:
    void creat_score()
    {

    }
};

class toustsound
{
public:
    toustsound();
    void generate_etronome(int rhythm); // 直接产生一个节拍器进行演奏

};

#endif // TOUSTSOUND_H
