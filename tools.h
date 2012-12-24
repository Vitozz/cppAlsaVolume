#ifndef FILEWORK_H
#define FILEWORK_H

#include "glibmm/ustring.h"
#include <iostream>
#include <vector>
#include <map>

struct switchcap {
	bool enabled;
	std::string name;
};

struct MixerSwitches{
	std::vector<switchcap> commonSwitchList_;
	std::vector<switchcap> captureSwitchList_;
	std::vector<switchcap> captureJoinedSwitchList_;
	std::vector<switchcap> captureExsclusiveSwitchList_;
	std::vector<switchcap> playbackSwitchList_;
	std::vector<switchcap> playbackJoinedSwitchList_;
};

struct settingsStr {
	unsigned int cardId;
	unsigned int mixerId;
	bool notebookOrientation;
	std::vector<std::string> cardList;
	std::vector<std::string> mixerList;
	MixerSwitches switchList;
};

enum SwitchType {
	COMMON =0,
	PLAYBACK = 1,
	PLAYBACK_JOINED = 2,
	CAPTURE = 3,
	CAPTURE_JOINED = 4,
	CAPTURE_EXCLUSIVE = 5

};

namespace Tools {
	Glib::ustring getResPath(const char *resName);
	bool checkFileExists(const std::string &fileName);
	void createDirectory(const std::string &dirName);
	void saveFile(const std::string &fileName, const Glib::ustring &fileData);
	std::pair<bool, int> itemExists(std::vector<std::string> vector, Glib::ustring item);
}

#endif // FILEWORK_H
