/*
 * tools.cpp
 * Copyright (C) 2012-2019 Vitaly Tonkacheyev
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "tools.h"
#include "glib.h"
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

bool Tools::compareDouble(const double &a, const double &b)
{
    const double EPSILON = 1.0e-5;
    return (a-b) < EPSILON;
}

std::string Tools::getCWD()
{
    const size_t cwdSize = 255;
    char cwdBuffer[255];
    return getcwd(cwdBuffer, cwdSize);
}

std::vector<std::string> Tools::getProjectPathes()
{
    const std::string cwd = getCWD();
    std::vector<std::string> list({getHomePath() + std::string("/.local") + PATH_SUFFIX,
                                   cwd + "/",
                                   cwd + "/" + PATH_SUFFIX,
                                   cwd.substr(0, cwd.find_last_of('/')) + PATH_SUFFIX,
                                   std::string("/usr") + PATH_SUFFIX,
                                   std::string("/usr/local") + PATH_SUFFIX}
                                  );
    return list;
}

std::string Tools::getResPath(const char *resName)
{
    const std::string resName_(resName);
    for(const std::string &path : getProjectPathes()) {
        std::string fileName = path + resName_;
        if (checkFileExists(fileName))
            return fileName;
    }
    return std::string();
}

std::string Tools::getDirPath(const char *dirName)
{
    const std::string dirName_(dirName);
    for(const std::string &path : getProjectPathes()) {
        std::string directoryName = path + dirName_;
        if (checkDirExists(directoryName))
            return directoryName;
    }
    return std::string();
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

#ifdef IS_DEBUG
void Tools::printList(const std::vector<std::string> &list)
{
    std::cout << "Printing vector contents" << std::endl;
    for(const std::string &item : list) {
        std::cout << item << std::endl;
    }
}
#endif
