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
    int timestamp;  // ���ȼ�����
    std::vector<unsigned char> message;  // ʵ������

    // ���캯��
    TimedMessage(int t, const std::vector<unsigned char>& d)
        : timestamp(t), message(d) {}
};
// �ȽϺ�����ʱ��С�����ȣ�
struct CompareTimedMessage {
    bool operator()(const TimedMessage& a, const TimedMessage& b) {
        if (a.timestamp == b.timestamp) {
            // ���ȼ���Note Off > ������Ϣ > Note On > ����
            uint8_t a_type = a.message[0] & 0xF0;
            uint8_t b_type = b.message[0] & 0xF0;

            if (a_type == 0x80) return false; // Note Off ������ȼ�
            if (b_type == 0x80) return true;
            if (a_type == 0xB0) return false; // ������Ϣ (CC)
            if (b_type == 0xB0) return true;
            if (a_type == 0x90) return false; // Note On
            if (b_type == 0x90) return true;
            return a.message[0] > b.message[0]; // ������Ĭ������
        }
        return a.timestamp > b.timestamp; // ʱ��С������
    }
};

class MidiNote {
protected:
    int pitch;     // MIDI������� (60 = C4)
    double duration; // ����ʱֵ (1.0 = �ķ�����)
    virtual void output(std::ostream& os)const {};//�ļ��������
    virtual void input(std::istream& is) {};
public:
    MidiNote(int pitch = 60, double duration = 1.0): pitch(pitch), duration(duration) {}

    virtual void play(RtMidiOut& midiOut, int bpm, int channel = 0) = 0;
    virtual ~MidiNote() = default;
    virtual void inserttoqueue(std::priority_queue<TimedMessage, std::vector<TimedMessage>, CompareTimedMessage>& pq,int bpm, int channel, int& t) {};//���ȶ��У��ٶȣ�ʱ���
    virtual MidiNote* clone()const = 0;//���ڿ������캯���л���ָ�븳ֵΪ������ָ������

    friend std::ostream& operator<<(std::ostream& os, const MidiNote& a);//�ļ��������
    friend std::istream& operator>>(std::istream& is, MidiNote& a);

    // ���߷�����������ת��ΪMIDI������ţ��Լ�ת����ȥ
    static int noteNameToMidi(const std::string& name);
    std::string MidiTonoteName()const;

    //��ȡ�ͱ༭���ݵķ���
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
    //����ר��
    virtual void NoteOn(RtMidiOut& midiOut, int bpm, int channel = 0) {};
	virtual void NoteOff(RtMidiOut& midiOut, int bpm, int channel = 0) {};
};
class Note : public MidiNote {
    int velocity; // ���� (0-127)����ʱûд�޸����ȵĹ���
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
			static_cast<unsigned char>(0x90 | channel), // Note On + ͨ��
			static_cast<unsigned char>(pitch),          // ���� (0-127)
			static_cast<unsigned char>(velocity)        // ���� (1-127)
		};
		midiOut.sendMessage(&message);
	}
    void NoteOff(RtMidiOut& midiOut, int bpm, int channel = 0) {
        std::vector<unsigned char> message = {
            static_cast<unsigned char>(0x80 | channel), // Note Off + ͨ��
            static_cast<unsigned char>(pitch),          // ���� (0-127)
            static_cast<unsigned char>(0)        // ���� (1-127)
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
        int ms = (60000.0 / bpm) * duration; // �ķ����� = 60000/bpm ����
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
	std::vector<Note> notes;//��������
    int velocity; // ���� (0-127)����ʱûд�޸����ȵĹ���
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
    std::vector<MidiNote*> notes;//����ָ������
    int channel;//���ڲ��ŵ�ͨ��,Ĭ��0,9�Ǵ����ͨ����0��15
    int program; // MIDI��ɫ��,0�Ǹ���
public:
    Track(int program = 0, int channel = 0)//Ĭ��0ͨ����������ɫ
        :program(program), channel(channel) {}
    ~Track() {
        for (auto& note : notes) {
            delete note;
        }
    }
    // �ƶ����캯��
    Track(Track&& other) noexcept
        : notes(std::move(other.notes)),
          channel(other.channel),
          program(other.program) 
    {
        other.notes.clear();  // ȷ��ԭ�����ٳ���ָ��
    }
    // ������캯��
    Track(const Track& other): channel(other.channel),program(other.program){
        // ��� notes ����
        for (const MidiNote* note : other.notes) {
            // ʹ���麯�� clone() �������������
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

    //��ȡ�ͱ༭���ݵķ���
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
        // ��ȡmidi�˿�����,�豸���
        unsigned int ports = midiOut.getPortCount();
        if (ports == 0) {
            throw std::runtime_error("û�п��õ�MIDI����˿�");
        }

        return 0; // Ĭ��ѡ���һ���˿�
    }
public:
    Score(int bpm=120):bpm(bpm) {
        // ��ʼ��MIDI���
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
    void save(std::string s);//�����ļ���s,s��·��
    void load(std::string s);//����s�ļ�

    friend std::ostream& operator<<(std::ostream& os, const Score& a);
    friend std::istream& operator>>(std::istream& is, Score& a);

    //��ȡ�ͱ༭���ݵķ���
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