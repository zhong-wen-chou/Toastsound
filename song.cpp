#include "song.h"
#include <windows.h>
#include <mmsystem.h> // timeBeginPeriod ��Ҫ

RtMidiOut midiOut;

Track Metronome(0,9);

void setMetronome(int n,int m) {//n������Ϊ1�ģ�ÿС��m��,���ý�����
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

int MidiNote::noteNameToMidi(const std::string& name) {
	int pitch = 12;//C0
	char c = name[0];
	int octave = name[1] - '0';
	int bia = 0;
	if (name.size() == 3) {
		if (name[2] == 's') {//sharp����
			bia = 1;
		}
		else if (name[2] == 'f') {//flat����
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

	// ������ (0x90 = Note On, channel)
	message.push_back(0x90 | channel);
	message.push_back(pitch);
	message.push_back(velocity);
	midiOut.sendMessage(&message);

	// �������ʱ��(����)
	int ms = (60000.0 / bpm) * duration; // �ķ����� = 60000/bpm ����

	//��ͣʱ��
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));

	// ������
	message[0] = 0x80 | channel; // 0x80 = Note Off
	message[2] = 0; // ����Ϊ0��ʾ������
	midiOut.sendMessage(&message);
}
void MultiNote::play(RtMidiOut& midiOut, int bpm, int channel) {
	std::vector<unsigned char> message;

	// ������ (0x90 = Note On, channel)
	message.push_back(0x90 | channel);
	message.push_back(pitch);
	message.push_back(velocity);
	midiOut.sendMessage(&message);
	// �������� Note On ��Ϣ��ÿ�������������ͣ�
	for (const auto& note : notes) {
		std::vector<unsigned char> message;
		message.push_back(0x90 | channel);  // Note On + ͨ��
		message.push_back(note.getpitch());  // ����
		message.push_back(note.getvelocity()); // ����
		midiOut.sendMessage(&message);
	}
	// �������ʱ��(����)
	int ms = (60000.0 / bpm) * duration; // �ķ����� = 60000/bpm ����

	//��ͣʱ��
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));

	// ������
	message[0] = 0x80 | channel; // 0x80 = Note Off
	message[2] = 0; // ����Ϊ0��ʾ������
	midiOut.sendMessage(&message);
	for (const auto& note : notes) {
		std::vector<unsigned char> message;
		message.push_back(0x80 | channel);  // Note Off + ͨ��
		message.push_back(note.getpitch()); // ����
		message.push_back(0);               // ����=0
		midiOut.sendMessage(&message);
	}


}
void Track::play(RtMidiOut& midiOut, int bpm) {
	// ������ɫ
	std::vector<unsigned char> programChange;
	programChange.push_back(0xC0 | channel); // Program Change
	programChange.push_back(program);
	midiOut.sendMessage(&programChange);

	// ������������
	for (auto& note : notes) {
		note->play(midiOut, bpm, channel);
	}
}

void Score::play() {
	//����ʱ���������У�Ԫ��Ϊrtmidi��Ϣ,���ȶ�Ϊʱ��
	std::priority_queue<TimedMessage, std::vector<TimedMessage>, CompareTimedMessage> midimessages;
	for (auto& track : tracks) {
		track.inserttoqueue(midimessages, bpm);
	}

	timeBeginPeriod(1); // ����Ϊ 1ms ����
	//ͨ��ʱ��ѭ�������ȶ��е���Ϣ����������
	int currentTime = 0;
	while (!midimessages.empty()) {
		//�������ȶ��еĵ�һ��Ԫ��
		TimedMessage msg = midimessages.top();
		midimessages.pop();
		// �����ӳ�
		int delay = msg.timestamp - currentTime;
		if (delay > 0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(delay));
		}
		// ����MIDI��Ϣ
		midiOut.sendMessage(&msg.message);
		currentTime =msg.timestamp;
	}
	timeEndPeriod(1); // �ָ�ϵͳĬ�Ͼ���
	// ��ѭ������������Բ����������һ����
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
}
void Note::inserttoqueue(std::priority_queue<TimedMessage, std::vector<TimedMessage>, CompareTimedMessage>& pq, int bpm, int channel, int& t) {
	std::vector<unsigned char> message;

	// ������ (0x90 = Note On, channel)
	message.push_back(0x90 | channel);
	message.push_back(pitch);
	message.push_back(velocity);
	pq.push(TimedMessage(t, message));

	// �������ʱ��(����)
	t += (60000.0 / bpm) * duration; // �ķ����� = 60000/bpm ����

	//������
	message[0] = 0x80 | channel; // 0x80 = Note Off
	message[2] = 0; // ����Ϊ0��ʾ������
	pq.push(TimedMessage(t, message));
}
void MultiNote::inserttoqueue(std::priority_queue<TimedMessage, std::vector<TimedMessage>, CompareTimedMessage>& pq, int bpm, int channel, int& t) {
	std::vector<unsigned char> message;

	// ������ (0x90 = Note On, channel)
	message.push_back(0x90 | channel);
	message.push_back(pitch);
	message.push_back(velocity);
	pq.push(TimedMessage(t, message));
	for (const auto& note : notes) {
		std::vector<unsigned char> message;
		message.push_back(0x90 | channel);  // Note On + ͨ��
		message.push_back(note.getpitch());  // ����
		message.push_back(note.getvelocity()); // ����
		pq.push(TimedMessage(t, message));
	}

	// �������ʱ��(����)
	t += (60000.0 / bpm) * duration; // �ķ����� = 60000/bpm ����
	//������
	message[0] = 0x80 | channel; // 0x80 = Note Off
	message[2] = 0; // ����Ϊ0��ʾ������
	pq.push(TimedMessage(t, message));
	for (const auto& note : notes) {
		std::vector<unsigned char> message;
		message.push_back(0x80 | channel);  // Note Off + ͨ��
		message.push_back(note.getpitch()); // ����
		message.push_back(0);               // ����=0
		pq.push(TimedMessage(t, message));
	}
}
void Track::inserttoqueue(std::priority_queue<TimedMessage, std::vector<TimedMessage>, CompareTimedMessage>& pq, int bpm) {
	// ������ɫ
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
	std::ifstream fin;
	fin.open(s);
	if (!fin.is_open()) {
		std::cerr << "�޷����ļ����б��棡" << std::endl;
		return;
	}
	std::string a;
	fin >> a;
	if (a != "midifile") {
		std::cerr << "��ʽ����" << std::endl;
		return;
	}
	fin >> *this;
	fin.close();
}
void Score::save(std::string s) {
	std::ofstream fout;
	fout.open(s);
	if (!fout.is_open()) {
		std::cerr << "�޷����ļ����ж�ȡ��" << std::endl;
		return;
	}
	fout << "midifile\n";
	fout << *this;
	fout.close();
}