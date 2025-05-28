#include "song.h"
#include <windows.h>
#include <mmsystem.h> // timeBeginPeriod 需要

RtMidiOut midiOut;
/*
Track Metronome(0,9);

void setMetronome(int n,int m) {//n分音符为1拍，每小节m拍,设置节拍器
    Metronome.clear();
    double duration = 4.0 / n;
    Metronome.addNote(new Note(75, duration));
    for (int i = 0; i < m - 1; i++) {
        Metronome.addNote(new Note(76, duration));
    }
}
void playMetronome(int bpm) {
    while (1) {
        Metronome.play(midiOut, bpm);
    }
}
*/

int MidiNote::noteNameToMidi(const std::string& name) {
    int pitch = 12;//C0
    char c = name[0];
    int octave = name[1] - '0';
    int bia = 0;
    if (name.size() == 3) {
        if (name[2] == 's') {//sharp升调
            bia = 1;
        }
        else if (name[2] == 'f') {//flat降调
            bia = -1;
        }
    }
    if (c == 'c' || c == 'C') {
        pitch += 0;
    }
    else if(c=='d'||c=='D') {
        pitch += 2;
    }
    else if (c == 'e' || c == 'E') {
        pitch += 4;
    }
    else if (c == 'f' || c == 'F') {
        pitch += 5;
    }
    else if (c == 'g' || c == 'G') {
        pitch += 7;
    }
    else if (c == 'a' || c == 'A') {
        pitch += 9;
    }
    else if (c == 'b' || c == 'B') {
        pitch += 11;
    }
    pitch += 12 * octave+bia;
    return pitch;
}
std::string MidiNote::MidiTonoteName()const {
    int octave = pitch / 12 - 1;
    int p = pitch % 12;
    char name = 'C';
    char oct = octave + '0';
    int delta=(p+int(p>=3)+int(p>=7))/2;
    name += delta;
    if (name >= 'H') {
        name -= 7;
    }
    std::string temp;
    temp += name;
    temp += oct;
    if (p - 2 * delta + int(delta >= 3)!=0) {
        if (p - 2 * delta + int(delta >= 3) == 1) {
            temp += 's';
        }
        else {
            temp += 'f';
        }
    }
    return temp;
}

void Note::play(RtMidiOut& midiOut, int bpm, int channel) {
    std::vector<unsigned char> message;

    // 音符开 (0x90 = Note On, channel)
    message.push_back(0x90 | channel);
    message.push_back(pitch);
    message.push_back(velocity);
    midiOut.sendMessage(&message);

    // 计算持续时间(毫秒)
    int ms = (60000.0 / bpm) * duration; // 四分音符 = 60000/bpm 毫秒

    //暂停时间
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));

    // 音符关
    message[0] = 0x80 | channel; // 0x80 = Note Off
    message[2] = 0; // 力度为0表示音符关
    midiOut.sendMessage(&message);
}
void MultiNote::play(RtMidiOut& midiOut, int bpm, int channel) {
    std::vector<unsigned char> message;

    // 音符开 (0x90 = Note On, channel)
    message.push_back(0x90 | channel);
    message.push_back(pitch);
    message.push_back(velocity);
    midiOut.sendMessage(&message);
    // 发送所有 Note On 消息（每个音符单独发送）
    for (const auto& note : notes) {
        std::vector<unsigned char> message;
        message.push_back(0x90 | channel);  // Note On + 通道
        message.push_back(note.getpitch());  // 音高
        message.push_back(note.getvelocity()); // 力度
        midiOut.sendMessage(&message);
    }
    // 计算持续时间(毫秒)
    int ms = (60000.0 / bpm) * duration; // 四分音符 = 60000/bpm 毫秒

    //暂停时间
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));

    // 音符关
    message[0] = 0x80 | channel; // 0x80 = Note Off
    message[2] = 0; // 力度为0表示音符关
    midiOut.sendMessage(&message);
    for (const auto& note : notes) {
        std::vector<unsigned char> message;
        message.push_back(0x80 | channel);  // Note Off + 通道
        message.push_back(note.getpitch()); // 音高
        message.push_back(0);               // 力度=0
        midiOut.sendMessage(&message);
    }


}
void Track::play(RtMidiOut& midiOut, int bpm) {
    // 设置音色
    std::vector<unsigned char> programChange;
    programChange.push_back(0xC0 | channel); // Program Change
    programChange.push_back(program);
    midiOut.sendMessage(&programChange);

    // 播放所有音符
    for (auto& note : notes) {
        note->play(midiOut, bpm, channel);
    }
}

void Score::play() {
    //创建时间优先序列，元素为rtmidi消息,优先度为时间
    /*shouldPlay = true;
    std::priority_queue<TimedMessage, std::vector<TimedMessage>, CompareTimedMessage> midimessages;
    for (auto& track : tracks) {
        track.inserttoqueue(midimessages, bpm);
    }

    timeBeginPeriod(1);
    int currentTime = 0;
    while (!midimessages.empty() && shouldPlay) {
        TimedMessage msg = midimessages.top();
        midimessages.pop();

        int delay = msg.timestamp - currentTime;
        if (delay > 0) {
            for (int slept = 0; slept < delay && shouldPlay; ++slept)
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        if (!shouldPlay) break;

        midiOut.sendMessage(&msg.message);
        currentTime = msg.timestamp;
    }
    timeEndPeriod(1);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));*/
    std::priority_queue<TimedMessage, std::vector<TimedMessage>, CompareTimedMessage> midimessages;
    for (auto& track : tracks) {
        track.inserttoqueue(midimessages, bpm);
    }

    timeBeginPeriod(1); // 设置为 1ms 精度
    //通过时间循环将优先队列的消息传给播放器
    int currentTime = 0;
    while (!midimessages.empty() && shouldPlay) {
        //弹出优先队列的第一个元素
        TimedMessage msg = midimessages.top();
        midimessages.pop();
        // 计算延迟
        int delay = msg.timestamp - currentTime;
        if (delay > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }
        // 发送MIDI消息
        midiOut.sendMessage(&msg.message);
        currentTime =msg.timestamp;
    }
    timeEndPeriod(1); // 恢复系统默认精度
    // 在循环结束后添加以播放完整最后一个音
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

void Score::stop(){
    shouldPlay = false;
}
void Score::restart(){
    shouldPlay=true;
}

void Note::inserttoqueue(std::priority_queue<TimedMessage, std::vector<TimedMessage>, CompareTimedMessage>& pq, int bpm, int channel, int& t) {
    std::vector<unsigned char> message;

    // 音符开 (0x90 = Note On, channel)
    message.push_back(0x90 | channel);
    message.push_back(pitch);
    message.push_back(velocity);
    pq.push(TimedMessage(t, message));

    // 计算持续时间(毫秒)
    t += (60000.0 / bpm) * duration; // 四分音符 = 60000/bpm 毫秒

    //音符关
    message[0] = 0x80 | channel; // 0x80 = Note Off
    message[2] = 0; // 力度为0表示音符关
    pq.push(TimedMessage(t, message));
}
void MultiNote::inserttoqueue(std::priority_queue<TimedMessage, std::vector<TimedMessage>, CompareTimedMessage>& pq, int bpm, int channel, int& t) {
    std::vector<unsigned char> message;

    // 音符开 (0x90 = Note On, channel)
    message.push_back(0x90 | channel);
    message.push_back(pitch);
    message.push_back(velocity);
    pq.push(TimedMessage(t, message));
    for (const auto& note : notes) {
        std::vector<unsigned char> message;
        message.push_back(0x90 | channel);  // Note On + 通道
        message.push_back(note.getpitch());  // 音高
        message.push_back(note.getvelocity()); // 力度
        pq.push(TimedMessage(t, message));
    }

    // 计算持续时间(毫秒)
    t += (60000.0 / bpm) * duration; // 四分音符 = 60000/bpm 毫秒
    //音符关
    message[0] = 0x80 | channel; // 0x80 = Note Off
    message[2] = 0; // 力度为0表示音符关
    pq.push(TimedMessage(t, message));
    for (const auto& note : notes) {
        std::vector<unsigned char> message;
        message.push_back(0x80 | channel);  // Note Off + 通道
        message.push_back(note.getpitch()); // 音高
        message.push_back(0);               // 力度=0
        pq.push(TimedMessage(t, message));
    }
}
void Track::inserttoqueue(std::priority_queue<TimedMessage, std::vector<TimedMessage>, CompareTimedMessage>& pq, int bpm) {
    // 设置音色
    std::vector<unsigned char> programChange;
    programChange.push_back(0xC0 | channel); // Program Change
    programChange.push_back(program);
    pq.push(TimedMessage(0, programChange));
    int t = 0;
    for (auto& note : notes) {
        note->inserttoqueue(pq, bpm, channel, t);
    }
}

std::ostream& operator<<(std::ostream& os, const Score& a) {
    os << a.bpm<<'\n';
    os << a.tracks.size()<<'\n';
    for (auto& track : a.tracks) {
        os<<track<<'\n';
    }
    return os;
}
std::istream& operator>>(std::istream& is, Score& a) {
    is >> a.bpm;
    int n;
    is >> n;
    Track temp;
    for (int i = 0; i < n; i++) {
        is >> temp;
        a.addTrack(std::move(temp));
    }
    return is;
}
std::ostream& operator<<(std::ostream& os, const Track& a) {
    os << a.channel<<'\n'<<a.program<<'\n'<<a.notes.size()<<'\n';
    for (auto& note : a.notes) {
        os << *note<<'\n';
    }
    return os;
}
std::istream& operator>>(std::istream& is, Track& a) {
    is >> a.channel >> a.program;
    int n;
    is >> n;
    MidiNote* temp=NULL;
    char c;
    for (int i = 0; i < n; i++) {
        is >> c;
        switch (c)
        {
        case 'n':
            temp = new Note;
            is >> *temp;
            break;
        case 'r':
            temp = new Rest;
            is >> *temp;
            break;
        case 'm':
            temp = new MultiNote;
            is >> *temp;
            break;
        default:
            break;
        }
        a.addNote(temp);
    }
    return is;
}
std::ostream& operator<<(std::ostream& os, const MidiNote& a) {
    a.output(os);
    return os;
}
void Note::output(std::ostream& os)const {
    os << 'n' << ' ' << MidiTonoteName()<<' '<<duration;
}
void Rest::output(std::ostream& os)const {
    os << 'r' << ' ' << duration;
}
void MultiNote::output(std::ostream& os)const {
    os << 'm' << ' ' << MidiTonoteName() << ' ' << duration;
    os << '\n'<< notes.size();
    for (const auto& note : notes) {
        os << '\n' << note.MidiTonoteName();
    }
}
std::istream& operator>>(std::istream& is,MidiNote& a) {
    a.input(is);
    return is;
}
void Note::input(std::istream& is) {
    std::string name;
    is >> name >> duration;
    pitch = noteNameToMidi(name);
}
void Rest::input(std::istream& is) {
    is >> duration;
}
void MultiNote::input(std::istream& is) {
    std::string name;
    is >> name >> duration;
    pitch = noteNameToMidi(name);
    int n;
    is >> n;
    for (int i = 0; i < n; i++) {
        is >> name;
        pitch = noteNameToMidi(name);
        addNote(pitch);
    }
}
void Score::load(std::string s) {
    this->clear();//清除之前的音轨
    std::ifstream fin;
    fin.open(s);
    if (!fin.is_open()) {
        std::cerr << "无法打开文件进行保存！" << std::endl;
        return;
    }
    std::string a;
    fin >> a;
    if (a != "midifile") {
        std::cerr << "格式错误！" << std::endl;
        return;
    }
    fin >> *this;
    fin.close();
}
void Score::save(std::string s) {
    std::ofstream fout;
    fout.open(s);
    if (!fout.is_open()) {
        std::cerr << "无法打开文件进行读取！" << std::endl;
        return;
    }
    fout << "midifile\n";
    fout << *this;
    fout.close();
}
