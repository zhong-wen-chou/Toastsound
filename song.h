#pragma once
#include <vector>
#include <memory>
#include<chrono>
#include <thread>
#include<queue>
#include<iostream>
#include<fstream>
#include "RtMidi.h"

struct TimedMessage {
    int timestamp;  // 优先级依据
    std::vector<unsigned char> message;  // 实际数据

    // 构造函数
    TimedMessage(int t, const std::vector<unsigned char>& d)
        : timestamp(t), message(d) {}
};
// 比较函数（时间小的优先）
struct CompareTimedMessage {
    bool operator()(const TimedMessage& a, const TimedMessage& b) {
        if (a.timestamp == b.timestamp) {
            // 优先级：Note Off > 控制消息 > Note On > 其他
            uint8_t a_type = a.message[0] & 0xF0;
            uint8_t b_type = b.message[0] & 0xF0;

            if (a_type == 0x80) return false; // Note Off 最高优先级
            if (b_type == 0x80) return true;
            if (a_type == 0xB0) return false; // 控制消息 (CC)
            if (b_type == 0xB0) return true;
            if (a_type == 0x90) return false; // Note On
            if (b_type == 0x90) return true;
            return a.message[0] > b.message[0]; // 其他按默认排序
        }
        return a.timestamp > b.timestamp; // 时间小的优先
    }
};

class MidiNote {
protected:
    int pitch;     // MIDI音符编号 (60 = C4)
    double duration; // 音符时值 (1.0 = 四分音符)
    virtual void output(std::ostream& os)const {};//文件保存相关
    virtual void input(std::istream& is) {};
public:
    MidiNote(int pitch = 60, double duration = 1.0): pitch(pitch), duration(duration) {}

    virtual void play(RtMidiOut& midiOut, int bpm, int channel = 0) = 0;
    virtual ~MidiNote() = default;
    virtual void inserttoqueue(std::priority_queue<TimedMessage, std::vector<TimedMessage>, CompareTimedMessage>& pq,int bpm, int channel, int& t) {};//优先队列，速度，时间戳
    virtual MidiNote* clone()const = 0;//用于拷贝构造函数中基类指针赋值为派生类指针的情况

    friend std::ostream& operator<<(std::ostream& os, const MidiNote& a);//文件保存相关
    friend std::istream& operator>>(std::istream& is, MidiNote& a);

    // 工具方法：将音名转换为MIDI音符编号，以及转换回去
    static int noteNameToMidi(const std::string& name);
    std::string MidiTonoteName()const;

    //获取和编辑数据的方法
    virtual int getpitch() const{
        return pitch;
    }
    virtual double getduration() const{
        return duration;
    }
    virtual int getvelocity() const{ return 0; }
    virtual void setpitch(int n) {
        pitch = n;
    }
    virtual void setduration(double a) {
        duration = a;
    }
    virtual void setvelocity(int n) {}
    //演奏专用
    virtual void NoteOn(RtMidiOut& midiOut, int bpm, int channel = 0) {};
	virtual void NoteOff(RtMidiOut& midiOut, int bpm, int channel = 0) {};
};
class Note : public MidiNote {
    int velocity; // 力度 (0-127)，暂时没写修改力度的功能
    void output(std::ostream& os)const;
    void input(std::istream& is);
public:
    Note(int pitch = 60, double duration = 1.0, int velocity = 90)
        : MidiNote(pitch, duration), velocity(velocity) {}

    void play(RtMidiOut& midiOut, int bpm, int channel = 0) override;
    void inserttoqueue(std::priority_queue<TimedMessage, std::vector<TimedMessage>, CompareTimedMessage>& pq, int bpm, int channel, int& t);
    MidiNote* clone()const {
        return new Note(pitch, duration, velocity);
    }
    int getvelocity() const{
        return velocity;
    }
    void setvelocity(int n) {
        velocity = n;
    }
	void NoteOn(RtMidiOut& midiOut, int bpm, int channel = 0) {
		std::vector<unsigned char> message = {
			static_cast<unsigned char>(0x90 | channel), // Note On + 通道
			static_cast<unsigned char>(pitch),          // 音高 (0-127)
			static_cast<unsigned char>(velocity)        // 力度 (1-127)
		};
		midiOut.sendMessage(&message);
	}
    void NoteOff(RtMidiOut& midiOut, int bpm, int channel = 0) {
        std::vector<unsigned char> message = {
            static_cast<unsigned char>(0x80 | channel), // Note Off + 通道
            static_cast<unsigned char>(pitch),          // 音高 (0-127)
            static_cast<unsigned char>(0)        // 力度 (1-127)
        };
        midiOut.sendMessage(&message);
    }
};
class Rest : public MidiNote {
    void output(std::ostream& os)const;
    void input(std::istream& is);
public:
    Rest(double duration = 1.0) : MidiNote(0, duration) {}

    void play(RtMidiOut& midiOut, int bpm, int channel = 0) override {
        int ms = (60000.0 / bpm) * duration; // 四分音符 = 60000/bpm 毫秒
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }
    void inserttoqueue(std::priority_queue<TimedMessage, std::vector<TimedMessage>, CompareTimedMessage>& pq, int bpm, int channel, int& t) {
        t+= (60000.0 / bpm) * duration;
    }
    MidiNote* clone()const {
        return new Rest(duration);
    }
};
class MultiNote :public MidiNote {
	std::vector<Note> notes;//音符数组
    int velocity; // 力度 (0-127)，暂时没写修改力度的功能
    void output(std::ostream& os)const;
    void input(std::istream& is);
public:
	MultiNote(int pitch = 60, double duration = 1.0, int velocity = 90)
		: MidiNote(pitch, duration), velocity(velocity) {}
	void addNote(int pitch,int velocity=90) {
		notes.push_back(Note(pitch,duration,velocity));
	}
    MidiNote* clone()const {
		MultiNote* mNote = new MultiNote(pitch, duration, velocity);
        for (const auto& note : notes) {
            mNote->addNote(note.getpitch(), note.getvelocity());
        }
        return mNote;
    }
    void play(RtMidiOut& midiOut, int bpm, int channel = 0) override;
    void inserttoqueue(std::priority_queue<TimedMessage, std::vector<TimedMessage>, CompareTimedMessage>& pq, int bpm, int channel, int& t);
};
class Track {
    std::vector<MidiNote*> notes;//音符指针数组
    int channel;//用于播放的通道,默认0,9是打击乐通道，0到15
    int program; // MIDI音色号,0是钢琴
public:
    Track(int program = 0, int channel = 0)//默认0通道，钢琴音色
        :program(program), channel(channel) {}
    ~Track() {
        for (auto& note : notes) {
            delete note;
        }
    }
    // 移动构造函数
    Track(Track&& other) noexcept
        : notes(std::move(other.notes)),
          channel(other.channel),
          program(other.program) 
    {
        other.notes.clear();  // 确保原对象不再持有指针
    }
    // 深拷贝构造函数
    Track(const Track& other): channel(other.channel),program(other.program){
        // 深拷贝 notes 数组
        for (const MidiNote* note : other.notes) {
            // 使用虚函数 clone() 复制派生类对象
            notes.push_back(note->clone());
        }
    }
    void addNote(MidiNote* note) {
        notes.push_back(note);
    }

    void play(RtMidiOut& midiOut, int bpm);
    void inserttoqueue(std::priority_queue<TimedMessage, std::vector<TimedMessage>, CompareTimedMessage>& pq, int bpm);

    friend std::ostream& operator<<(std::ostream& os, const Track& a);
    friend std::istream& operator>>(std::istream& is, Track& a);

    //获取和编辑数据的方法
    int getchannel() const{
        return channel;
    }
    void setchannel(int n) {
        channel = n;
    }
    int getprogram() const{
        return program;
    }
    void setprogram(int n) {
        program = n;
    }
    int getnotesnum() const{
        return notes.size();
    }
    MidiNote* getnotesbyn(int n)const {
        if (n < notes.size()||n>=0) {
            return notes[n];
        }
        return NULL;
    }
};
class Score {
    std::vector<Track> tracks;
    RtMidiOut midiOut;
    int bpm;

    unsigned int chooseMidiPort(RtMidiOut& midiOut) {
        // 获取midi端口数量,设备相关
        unsigned int ports = midiOut.getPortCount();
        if (ports == 0) {
            throw std::runtime_error("没有可用的MIDI输出端口");
        }

        return 0; // 默认选择第一个端口
    }
public:
    Score(int bpm=120):bpm(bpm) {
        // 初始化MIDI输出
        try {
            unsigned int port = chooseMidiPort(midiOut);
            midiOut.openPort(port);
        }
        catch (RtMidiError& error) {
            error.printMessage();
            throw;
        }
    }

    void addTrack(Track track) {
        tracks.push_back(std::move(track));
    }

    void play();
    void save(std::string s);//保存文件到s,s是路径
    void load(std::string s);//加载s文件

    friend std::ostream& operator<<(std::ostream& os, const Score& a);
    friend std::istream& operator>>(std::istream& is, Score& a);

    //获取和编辑数据的方法
    int getbpm() const{
        return bpm;
    }
    void setbpm(int n) {
        bpm = n;
    }
    int gettracksnum() const{
        return tracks.size();
    }
    Track& gettrackbyn(int n) {
        if (n < tracks.size()||n>=0) {
            return tracks[n];
        }
        else {
            throw std::out_of_range("Index out of range");
        }
    }
};