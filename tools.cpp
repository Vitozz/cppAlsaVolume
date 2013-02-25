/*
 * tools.cpp
 * Copyright (C) 2012 Vitaly Tonkacheyev
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "tools.h"
#include "glibmm/fileutils.h"
#include "glibmm/keyfile.h"
#include <fstream>
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
	std::vector<Glib::ustring> list;
	list.reserve(list.size()+4);
	Glib::ustring homepath(Glib::ustring(getenv("HOME")) + "/.local" + pathSuffix);
	size_t cwdSize = 255;
	char cwdBuffer[cwdSize];
	Glib::ustring cwd(getcwd(cwdBuffer, cwdSize));
	list.push_back(homepath + resName_);
	list.push_back(cwd + "/" + resName_);
	list.push_back("/usr"+ pathSuffix + resName_);
	list.push_back("/usr/local" + pathSuffix + resName_);
	std::vector<Glib::ustring>::iterator it = list.begin();
	while (it != list.end()) {
		if (checkFileExists(*it)) {
			return Glib::ustring(*it);
		}
		it++;
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

std::pair<bool, int> Tools::itemExists(std::vector<std::string> vector, const Glib::ustring& item)
{
	int index = 0;
	int i = 0;
	bool exists = false;
	std::vector<std::string>::iterator it = vector.begin();
	while (it != vector.end()) {
		Glib::ustring answ(*it);
		if (answ == item) {
			index = i;
			exists = true;
			break;
		}
		it++;
		i++;
	}
	std::pair<bool, int> result(exists, index);
	return result;
}

std::vector<std::string> Tools::getFileList(const std::string &dir)
{
	uint pos = dir.find_last_of("/");
	std::string dirname = dir.substr(0, pos);
	std::cout << dirname << std::endl;
	Glib::Dir dir_ (dirname);
	std::vector<std::string> entries (dir_.begin(), dir_.end());
	return entries;
}
