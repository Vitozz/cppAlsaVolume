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

#include "glibmm/ustring.h"
#include <iostream>
#include <vector>
#include <map>

struct switchcap {
	bool enabled;
	std::string name;
};

struct VolumeMixers{
	std::vector<std::string> playback;
	std::vector<std::string> capture;
};

struct MixerSwitches{
	std::vector<switchcap> captureSwitchList_;
	std::vector<switchcap> playbackSwitchList_;
	std::vector<switchcap> enumSwitchList_;
};

struct settingsStr {
	unsigned int cardId;
	unsigned int mixerId;
	bool notebookOrientation;
	bool isAutorun;
	std::vector<std::string> cardList;
	std::vector<std::string> mixerList;
	MixerSwitches switchList;
};

enum SwitchType {
	PLAYBACK = 0,
	CAPTURE = 1,
	ENUM = 2
};

namespace Tools {
	Glib::ustring getResPath(const char *resName);
	bool checkFileExists(const std::string &fileName);
	void createDirectory(const std::string &dirName);
	void saveFile(const std::string &fileName, const Glib::ustring &fileData);
	std::pair<bool, int> itemExists(std::vector<std::string> vector, const Glib::ustring& item);
	std::vector<std::string> getFileList(const std::string& dir);
	//Program version
	const std::string version = "0.0.7";
}

#endif // FILEWORK_H
