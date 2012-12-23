#ifndef FILEWORK_H
#define FILEWORK_H

#include "glibmm/ustring.h"
#include <iostream>
#include <vector>
#include <map>

struct settingsStr {
	int cardId;
	int mixerId;
	bool notebookOrientation;
	std::vector<std::string> cardList;
	std::vector<std::string> mixerList;
	std::vector<std::string> switchList;
};

namespace Tools {
	Glib::ustring getResPath(const char *resName);
	bool checkFileExists(const std::string &fileName);
	void createDirectory(const std::string &dirName);
	void saveFile(const std::string &fileName, const Glib::ustring &fileData);
	std::pair<bool, int> itemExists(std::vector<std::string> vector, Glib::ustring item);
}

#endif // FILEWORK_H
