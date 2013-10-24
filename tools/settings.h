/*
 * settings.h
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

#ifndef SETTINGS_H
#define SETTINGS_H
#include "glibmm/keyfile.h"

class Settings
{
public:
	Settings();
	~Settings();
	int getSoundCard() const;
	void saveSoundCard(int soundCard);
	Glib::ustring getMixer() const;
	void saveMixer(const std::string& mixerName);
	void saveNotebookOrientation(bool orient);
	bool getNotebookOrientation();
	void setAutorun(bool isAutorun);
	bool getAutorun();
	void setVersion(const Glib::ustring& version);
	void setExternalMixer(const std::string &extMixerName);
	std::string getExternalMixer();

private:
	void parseConfig(const Glib::ustring& keyFileName, const Glib::ustring& keyFileData);
	void loadConfig(const std::string& fileName);
	void loadDesktopFile(const std::string& fileName);
	void initDesktopFileData();

private:
	Glib::KeyFile *configFile_;
	Glib::KeyFile *desktopFile_;
	std::string iniFileName_;
	std::string desktopFilePath_;
	std::string version_;
};

#endif // SETTINGS_H
