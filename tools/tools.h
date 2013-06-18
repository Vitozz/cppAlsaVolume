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

#include "archive.h"
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
#ifdef HAVE_ICONPACKS
	,
	ICONS = 2
#endif
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
	std::pair<bool, int> itemExists(const std::vector<std::string> &vector_, const std::string& item);
	std::vector<std::string> getFileList(const std::string& dir);
	std::string getTmpDir();
	void clearTempDir(const std::string &path);
	std::string pathToFileName(const std::string &path);
	//Constatnts
	const std::string defaultIconPack = "default";
	const std::string PathSuffix = "/share/alsavolume/";
	//Program version
	const std::string version = "0.1.3";
}

#endif // FILEWORK_H
