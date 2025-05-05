#include "sound.h"
#include <iostream>
#include <Windows.h>
#pragma comment(lib,"winmm.lib")
void Note::Play(int bmp, const HMIDIOUT& handle) {
	int scale, volume = 0x7f, type = 0x94, voice;//¸ÖÇÙ0x94
	int sleep = 60000 / bmp;
	scale = 59 + f;
	voice = (volume << 16) + (scale << 8) + type;
	printf("%p\n", voice);
	midiOutShortMsg(handle, voice);
	Sleep(duration * sleep);
}
void Pause::Play(int bmp, const HMIDIOUT& handle) {
	int sleep = 60000 / bmp;
	Sleep(sleep*duration);
}
void Track::Play() {
	HMIDIOUT handle;
	midiOutOpen(&handle, 0, 0, 0, CALLBACK_NULL);
	for (auto it = line.begin(); it != line.end(); it++) {
		(*it)->Play(bmp,handle);
	}
	midiOutClose(handle);
}
void Track::setLine() {
	int name = 1,bia = 0;
	float  times = 1;
	while (std::cin >> name) {
		if (name == 0) {//pause
			std::cin >> times;
			Pause* temp = new Pause(times);
			temp->setduration(times);
			line.push_back(temp);
		}
		else {//Note
			std::cin >> bia >> times;
			Note* temp = new Note();
			temp->setf(name, bia);
			temp->setduration(times);
			line.push_back(temp);
		}
	}
}
