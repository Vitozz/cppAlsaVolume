/*
 * tools.h
 * Copyright (C) 2012-2025 Vitaly Tonkacheyev
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

#ifndef FILEWORK_H
#define FILEWORK_H

#include <string>
#include <vector>
#include <algorithm>

enum SwitchType {
    PLAYBACK = 0,
    CAPTURE = 1,
    ENUM = 2
};

enum ListType {
    CARDS = 0,
    MIXERS = 1
};

struct iconPosition {
    int iconX_;
    int iconY_;
    int iconWidth_;
    int iconHeight_;
    int screenHeight_;
    int screenWidth_;
    bool geometryAvailable_;
    bool trayAtTop_;
};

typedef std::pair<std::string, bool> switchcap;

namespace Tools {
std::string getCWD();
inline std::string getHomePath() {return std::string(getenv("HOME"));};
std::vector<std::string> getProjectPathes();
std::string getResPath(const char *resName);
std::string getDirPath(const char *dirName);
bool checkFileExists(const std::string &fileName);
bool checkDirExists(const std::string &fileName);
bool compareDouble(const double &a, const double &b);
void createDirectory(const std::string &dirName);
void saveFile(const std::string &fileName, const std::string &fileData);

#ifdef IS_DEBUG
void printList(const std::vector<std::string> &list);
#endif
//Template functions
template <class T>
int itemIndex(const std::vector<T> &vect, const T &item) {
    int index = 0;
    typename std::vector<T>::const_iterator it = std::find(vect.begin(), vect.end(), item);
    if (it != vect.end()) {
        index = it - vect.begin();
    }
    return index;
}
template <class T>
bool itemExists(const std::vector<T> &vect, const T &item) {
    typename std::vector<T>::const_iterator it = std::find(vect.begin(), vect.end(), item);
    return (it != vect.end());
}
}
#endif // FILEWORK_H
