/*
 * tools.h
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

#ifndef FILEWORK_H
#define FILEWORK_H

#include <iostream>
#include <vector>

enum SwitchType {
	PLAYBACK = 0,
	CAPTURE = 1,
	ENUM = 2
};

enum ListType {
	CARDS = 0,
	MIXERS = 1
};

typedef std::pair<std::string, bool> switchcap;

namespace Tools {
	std::string getCWD();
	std::string getHomePath();
	std::string getResPath(const char *resName);
	bool checkFileExists(const std::string &fileName);
	bool checkDirExists(const std::string &fileName);
	void createDirectory(const std::string &dirName);
	void saveFile(const std::string &fileName, const std::string &fileData);
	std::string pathToFileName(const std::string &path);
	void printList(const std::vector<std::string> &list);
	//Constatnts
	const std::string PathSuffix = "/share/alsavolume/";
	//Template functions
	template <class T>
	int itemIndex(const std::vector<T> &vect, const T &item) {
		int index = 0;
		int i = 0;
		typename std::vector<T>::const_iterator it = vect.begin();
		while (it != vect.end()) {
			const T answ(*it);
			if (answ == item) {
				index = i;
				break;
			}
			++it;
			++i;
		}
		return index;
	}
	template <class T>
	bool itemExists(const std::vector<T> &vect, const T &item) {
		typename std::vector<T>::const_iterator it = vect.begin();
		while (it != vect.end()) {
			const T answ(*it);
			if (answ == item) {
				return true;
			}
			++it;

		}
		return false;
	}
}



#endif // FILEWORK_H
