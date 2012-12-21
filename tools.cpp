#include "tools.h"
#include "glibmm/fileutils.h"
#include <fstream>
#include <list>
#include <cstdlib>
#include <vector>
#include <map>

bool Tools::checkFileExists(const std::string &fileName)
{
	return Glib::file_test(fileName, Glib::FILE_TEST_EXISTS);
}

Glib::ustring Tools::getResPath(const char *resName)
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

void Tools::createDirectory(const std::string &dirName)
{
	if (!Glib::file_test(dirName, Glib::FILE_TEST_IS_DIR)) {
		std::cout << "Directory " << dirName << " not found. Attempting to create it.." << std::endl;
		gint err = 0;
		err = g_mkdir_with_parents(dirName.c_str(), 0755);
		if (err < 0) {
			std::cerr << g_file_error_from_errno(err) << std::endl;
		}
	}
}

void Tools::saveFile(const std::string &fileName, const Glib::ustring &fileData)
{
	try {
		std::ofstream ofile(fileName.c_str());
		ofile << fileData << std::endl;
		ofile.close();
	}
	catch ( const std::exception & ex ) {
		std::cout << "settings.cpp::57::Parsing failed:: " << ex.what() << std::endl;
	}
}

std::pair<bool, int> Tools::itemExists(std::vector<std::string> vector, Glib::ustring item)
{
	int index = 0;
	bool exists = false;
	std::vector<std::string>::iterator it = vector.begin();
	for (int i=0; i < (int)vector.size(); i++) {
		std::advance(it, i);
		const std::string answ(*it);
		if (Glib::ustring(answ) == item) {
			index = i;
			exists = true;
			break;
		}
	}
	std::pair<bool, int> result(exists, index);
	return result;

}