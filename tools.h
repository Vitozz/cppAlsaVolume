#ifndef FILEWORK_H
#define FILEWORK_H

#include "glibmm/ustring.h"
#include <iostream>
#include <vector>
#include <map>

namespace FileWork {
	Glib::ustring getResPath(const char *resName);
	bool checkFileExists(const std::string &fileName);
	void createDirectory(const std::string &dirName);
	void saveFile(const std::string &fileName, const Glib::ustring &fileData);
	std::pair<bool, int> itemExists(std::vector<std::string> vector, Glib::ustring item);
}

#endif // FILEWORK_H
