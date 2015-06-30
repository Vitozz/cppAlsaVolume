/*
 * settings.cpp
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

#include "settings.h"
#include "tools.h"
#include "glibmm/keyfile.h"
#include "glibmm/fileutils.h"
#include "fstream"

#define MAIN "main"
#define VOLUME "volume"
#define CARD "card"
#define MIXER "mixer"
#define MIXERID "mixerid"
#define ORIENT "orient"
#define ISPULSE "ispulse"
#define PULSEDEV "pulsedev"
#define USEPOLL "usepolling"

Settings::Settings()
: configFile_(new Glib::KeyFile()),
  desktopFile_(new Glib::KeyFile())
{
	const std::string configDir = std::string(Tools::getHomePath() + "/.config/cppAlsaVolume");
	iniFileName_ = configDir + std::string("/config.ini");
	desktopFilePath_ = std::string(Tools::getHomePath() + "/.config/autostart/alsavolume.desktop");
	Tools::createDirectory(configDir);
	loadConfig(iniFileName_);
	loadDesktopFile(desktopFilePath_);
}

Settings::~Settings()
{
	delete configFile_;
	delete desktopFile_;
}

void Settings::loadConfig(const std::string& fileName)
{
	if (!Tools::checkFileExists(fileName)) {
		parseConfig(iniFileName_, std::string(""));
	}
	try {
		configFile_->load_from_file(fileName);
	}
	catch (Glib::FileError &err) {
		std::cerr << "settings.cpp::51:: " << fileName << " - " << err.what() << std::endl;
	}
}

void Settings::parseConfig(const Glib::ustring& keyFileName, const Glib::ustring& keyFileData)
{
	Tools::saveFile(keyFileName, keyFileData);
}

void Settings::saveSoundCard(int soundCard)
{
	configFile_->set_integer(Glib::ustring(MAIN),Glib::ustring(CARD),soundCard);
	parseConfig(iniFileName_, configFile_->to_data());
}

void Settings::saveMixer(const std::string &mixerName)
{
	configFile_->set_string(Glib::ustring(MAIN),Glib::ustring(MIXER),mixerName);
	parseConfig(iniFileName_, configFile_->to_data());
}

void Settings::saveMixerId(int mixerId)
{
	configFile_->set_integer(Glib::ustring(MAIN),Glib::ustring(MIXERID),mixerId);
	parseConfig(iniFileName_, configFile_->to_data());
}


void Settings::saveNotebookOrientation(bool orient)
{
	configFile_->set_boolean(Glib::ustring(MAIN),Glib::ustring(ORIENT),orient);
	parseConfig(iniFileName_, configFile_->to_data());
}

void Settings::loadDesktopFile(const std::string &fileName)
{
	if (!Tools::checkFileExists(fileName)) {
		initDesktopFileData();
		parseConfig(desktopFilePath_, desktopFile_->to_data());
	}
	try {
		desktopFile_->load_from_file(fileName);
	}
	catch (Glib::FileError &err) {
		std::cerr << "settings.cpp::143:: " << fileName << " - " << err.what() << std::endl;
	}
}

void Settings::initDesktopFileData()
{
	const Glib::ustring entry = Glib::ustring("Desktop Entry");
	desktopFile_->set_string(entry, Glib::ustring("Encoding"),Glib::ustring("UTF-8"));
	desktopFile_->set_string(entry,Glib::ustring("Name"),Glib::ustring("AlsaVolume"));
	desktopFile_->set_string(entry,Glib::ustring("Comment"),Glib::ustring("Changes the volume of ALSA from the system tray"));
	desktopFile_->set_string(entry,Glib::ustring("Exec"),Glib::ustring("alsavolume"));
	desktopFile_->set_string(entry,Glib::ustring("Type"),Glib::ustring("Application"));
	desktopFile_->set_string(entry,Glib::ustring("Version"),Glib::ustring(version_));
	desktopFile_->set_boolean(entry,Glib::ustring("Hidden"),true);
	desktopFile_->set_string(entry,Glib::ustring("Comment[ru]"),Glib::ustring("Регулятор громкости ALSA"));
}

void Settings::setAutorun(bool isAutorun)
{
	desktopFile_->set_boolean(Glib::ustring("Desktop Entry"),Glib::ustring("Hidden"),!isAutorun);
	parseConfig(desktopFilePath_, desktopFile_->to_data());
}

void Settings::setVersion(const Glib::ustring &version)
{
	version_ = version;
	desktopFile_->set_string(Glib::ustring("Desktop Entry"),Glib::ustring("Version"), version);
	parseConfig(desktopFilePath_, desktopFile_->to_data());
}

void Settings::setUsePulse(bool use)
{
	configFile_->set_boolean(Glib::ustring(MAIN), Glib::ustring(ISPULSE), use);
	parseConfig(iniFileName_, configFile_->to_data());
}

void Settings::setUsePolling(bool use)
{
	configFile_->set_boolean(Glib::ustring(MAIN), Glib::ustring(USEPOLL), use);
	parseConfig(iniFileName_, configFile_->to_data());
}

void Settings::savePulseDeviceName(const std::string &name)
{
	configFile_->set_string(Glib::ustring(MAIN), Glib::ustring(PULSEDEV), Glib::ustring(name));
	parseConfig(iniFileName_, configFile_->to_data());
}

int Settings::getSoundCard() const
{
	int card = 0;
	try {
		card = (int)configFile_->get_integer(Glib::ustring(MAIN),Glib::ustring(CARD));
	}
	catch (const Glib::KeyFileError& ex) {
		std::cerr << "settings.cpp::160::KeyFileError " << ex.what() << std::endl;
	}
	return card;
}

int Settings::getMixerId() const
{
	int id = 0;
	try {
		id = (int)configFile_->get_integer(Glib::ustring(MAIN),Glib::ustring(MIXERID));
	}
	catch (const Glib::KeyFileError& ex) {
		std::cerr << "settings.cpp::172::KeyFileError " << ex.what() << std::endl;
	}
	return id;
}

Glib::ustring Settings::getMixer() const
{
	Glib::ustring mixer("");
	try {
		mixer = Glib::ustring(configFile_->get_string(Glib::ustring(MAIN),Glib::ustring(MIXER)));
	}
	catch (const Glib::KeyFileError& ex) {
		std::cerr << "settings.cpp::184::KeyFileError " << ex.what() << std::endl;
	}
	return mixer;
}

bool Settings::getNotebookOrientation()
{
	bool orient = false;
	try {
		orient = bool(configFile_->get_boolean(Glib::ustring(MAIN),Glib::ustring(ORIENT)));
	}
	catch (const Glib::KeyFileError& ex) {
		std::cerr << "settings.cpp::196::KeyFileError " << ex.what() << std::endl;
	}
	return orient;
}

bool Settings::getAutorun()
{
	bool isAutorun = false;
	try {
		isAutorun = !bool(desktopFile_->get_boolean(Glib::ustring("Desktop Entry"),Glib::ustring("Hidden")));
	}
	catch (const Glib::KeyFileError& ex) {
		std::cerr << "settings.cpp::208::KeyFileError " << ex.what() << std::endl;
	}
	return isAutorun;
}

std::string Settings::pulseDeviceName() const
{
	std::string device("");
	try {
		device = std::string(configFile_->get_string(Glib::ustring(MAIN),Glib::ustring(PULSEDEV)));
	}
	catch (const Glib::KeyFileError& ex) {
		std::cerr << "settings.cpp::220::KeyFileError " << ex.what() << std::endl;
	}
	return device;
}

bool Settings::usePulse()
{
	bool isPulse = false;
	try {
		isPulse = bool(configFile_->get_boolean(Glib::ustring(MAIN),Glib::ustring(ISPULSE)));
	}
	catch (const Glib::KeyFileError& ex) {
		std::cerr << "settings.cpp::232::KeyFileError " << ex.what() << std::endl;
	}
	return isPulse;
}

bool Settings::usePolling()
{
	bool isPolling = true;
	try {
		isPolling = bool(configFile_->get_boolean(Glib::ustring(MAIN),Glib::ustring(USEPOLL)));
	}
	catch (const Glib::KeyFileError& ex) {
		std::cerr << "settings.cpp::244::KeyFileError " << ex.what() << std::endl;
	}
	return isPolling;
}
