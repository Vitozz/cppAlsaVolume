/*
 * tools.cpp
 * Copyright (C) 2012-2015 Vitaly Tonkacheyev
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
#define PATH_SUFFIX "/share/alsavolume/"
#define MK_RIGHTS 0755

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
	return std::string(getenv("HOME"));
}

std::vector<std::string> Tools::getProjectPathes()
{
	const std::string cwd = getCWD();
	std::vector<std::string> list;
	list.push_back(getHomePath() + std::string("/.local") + PATH_SUFFIX);
	list.push_back(cwd + "/");
	list.push_back(cwd.substr(0, cwd.find_last_of("/")) + PATH_SUFFIX);
	list.push_back(std::string("/usr") + PATH_SUFFIX);
	list.push_back(std::string("/usr/local") + PATH_SUFFIX);
	return list;
}

std::string Tools::getResPath(const char *resName)
{
	const std::string resName_(resName);
	const std::vector<std::string> list = getProjectPathes();
	std::string fileName;
	std::vector<std::string>::const_iterator it = std::find_if(list.begin(),
								   list.end(),
								   [&](const std::string &path){
		fileName = path + resName_;
		return checkFileExists(fileName);
	});
	return (it != list.end()) ? fileName : std::string();
}

std::string Tools::getDirPath(const char *dirName)
{
	const std::string dirName_(dirName);
	const std::vector<std::string> list = getProjectPathes();
	std::string directoryName;
	std::vector<std::string>::const_iterator it = std::find_if(list.begin(),
								   list.end(),
								   [&](const std::string &path){
		directoryName = path + dirName_;
		return checkDirExists(directoryName);
	});
	return (it != list.end()) ? directoryName : std::string();
}

void Tools::createDirectory(const std::string &dirName)
{
	if (!checkDirExists(dirName)) {
		std::cerr << "Directory " << dirName << " not found. Attempting to create it.." << std::endl;
		gint err  = g_mkdir_with_parents(dirName.c_str(), MK_RIGHTS);
		if (err < 0) {
			std::cerr << "tools.cpp::102::createDirectory:: " << g_file_error_from_errno(err) << std::endl;
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
		std::cerr << "tools.cpp::112::saveFile:: " << ex.what() << std::endl;
	}
}

std::string Tools::pathToFileName(const std::string &path)
{
	return g_path_get_basename(path.c_str());
}

#ifdef IS_DEBUG
void Tools::printList(const std::vector<std::string> &list)
{
	std::cout << "Printing vector contents" << std::endl;
	std::for_each(list.begin(),
		      list.end(),
		      []( const std::string &item){ std::cout << item << std::endl; });
}
#endif
