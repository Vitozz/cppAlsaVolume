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
#include "glib.h"
#include "glib/gstdio.h"
#include "unistd.h"
#include <fstream>
#include <cstdlib>
#include <vector>

bool Tools::checkFileExists(const std::string &fileName)
{
	return g_file_test(fileName.c_str(), G_FILE_TEST_EXISTS);
}

bool Tools::checkDirExists(const std::string &fileName)
{
	return g_file_test(fileName.c_str(), G_FILE_TEST_IS_DIR);
}

std::string Tools::getCWD()
{
	const size_t cwdSize = 255;
	char cwdBuffer[255];
	return getcwd(cwdBuffer, cwdSize);
}

std::string Tools::getHomePath()
{
	return std::string(getenv("HOME")) + "/.local" + PathSuffix;
}

std::string Tools::getResPath(const char *resName)
{
	const std::string resName_(resName);
	std::vector<std::string> list;
	list.push_back(getHomePath() + resName_);
	list.push_back(getCWD() + "/" + resName_);
	list.push_back("/usr"+ PathSuffix + resName_);
	list.push_back("/usr/local" + PathSuffix + resName_);
	std::vector<std::string>::iterator it = list.begin();
	std::string result;
	while (it != list.end()) {
		if (checkFileExists(*it)) {
			result = std::string(*it);
			return result;
		}
		++it;
	}
	return result;
}

void Tools::createDirectory(const std::string &dirName)
{
	if (!g_file_test(dirName.c_str(), G_FILE_TEST_IS_DIR)) {
		std::cerr << "Directory " << dirName << " not found. Attempting to create it.." << std::endl;
		gint err = 0;
		err = g_mkdir_with_parents(dirName.c_str(), 0755);
		if (err < 0) {
			std::cerr << g_file_error_from_errno(err) << std::endl;
		}
	}
}

void Tools::saveFile(const std::string &fileName, const std::string &fileData)
{
	try {
		std::ofstream ofile(fileName.c_str());
		ofile << fileData << std::endl;
		ofile.close();
	}
	catch ( const std::exception & ex ) {
		std::cerr << "settings.cpp::57::Parsing failed:: " << ex.what() << std::endl;
	}
}

std::string Tools::pathToFileName(const std::string &path)
{
	return g_path_get_basename(path.c_str());
}

void Tools::printList(const std::vector<std::string> &list)
{
	std::cout << "Printing vector contents" << std::endl;
	const std::vector<std::string> vector(list);
	std::vector<std::string>::const_iterator it = vector.begin();
	while (it != vector.end()) {
		std::cout << std::string(*it) << std::endl;
		++it;
	}
}
