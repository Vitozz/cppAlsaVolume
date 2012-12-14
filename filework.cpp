#include "filework.h"
#include <list>
#include <cstdlib>
#include "glibmm/fileutils.h"

bool checkFileExists(const char *fileName)
{
	return Glib::file_test(std::string(fileName), Glib::FILE_TEST_EXISTS);
}

Glib::ustring getResPath(const char *resName)
{
	const Glib::ustring pathSuffix("/share/alsavolume/");
	const Glib::ustring resName_(resName);
	std::list<Glib::ustring> list;
	Glib::ustring homepath(Glib::ustring(getenv("HOME")) + "/.local" + pathSuffix);
	size_t cwdSize = 255;
	char cwdBuffer[cwdSize];
	Glib::ustring cwd(getcwd(cwdBuffer, cwdSize));
	list.push_back(homepath + resName_);
	list.push_back(cwd + "/" + resName_);
	list.push_back("/usr"+ pathSuffix + resName_);
	list.push_back("/usr/local" + pathSuffix + resName_);
	std::list<Glib::ustring>::iterator i = list.begin();
	for (uint j=0; j < list.size(); j++) {
		std::advance(i,j);
		const char *fileName = (char*)i->c_str();
		if (checkFileExists(fileName)) {
			return Glib::ustring(*i);
		}
	}
	return "";
}
