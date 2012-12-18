#ifndef FILEWORK_H
#define FILEWORK_H

#include <iostream>
#include <glibmm/ustring.h>

namespace FileWork {
	Glib::ustring getResPath(const char *resName);
	bool checkFileExists(const std::string &fileName);
	void createDirectory(const std::string &dirName);
	void saveFile(const std::string &fileName, const Glib::ustring &fileData);
}

#endif // FILEWORK_H
