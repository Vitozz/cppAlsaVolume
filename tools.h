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

struct VolumeMixers{
	std::vector<std::string> playback;
	std::vector<std::string> capture;
};

struct MixerSwitches{
	std::vector<switchcap> captureSwitchList_;
	std::vector<switchcap> playbackSwitchList_;
	std::vector<switchcap> enumSwitchList_;
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
	PLAYBACK = 0,
	CAPTURE = 1,
	ENUM = 2
};

namespace Tools {
	Glib::ustring getResPath(const char *resName);
	bool checkFileExists(const std::string &fileName);
	void createDirectory(const std::string &dirName);
	void saveFile(const std::string &fileName, const Glib::ustring &fileData);
	std::pair<bool, int> itemExists(std::vector<std::string> vector, const Glib::ustring& item);
	std::vector<std::string> getFileList(const std::string& dir);
}

#endif // FILEWORK_H
