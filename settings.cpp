#include "settings.h"
#include "tools.h"
#include "glibmm/keyfile.h"
#include "glibmm/fileutils.h"
#include "fstream"

const std::string configDir = std::string(std::string(getenv("HOME")) + "/.config/cppAlsaVolume");
const std::string iniFileName = configDir + std::string("/config.ini");
const std::string DesktopFilePath = std::string(std::string(getenv("HOME")) + "/.config/autostart/alsavolume.desktop");

Settings::Settings()
{
	configFile_ = new Glib::KeyFile();
	desktopFile_ = new Glib::KeyFile();
	Tools::createDirectory(configDir);
	loadConfig(iniFileName.c_str());
	loadDesktopFile(DesktopFilePath.c_str());
}

Settings::~Settings()
{
	delete configFile_;
	delete desktopFile_;
}

void Settings::loadConfig(const std::string& fileName)
{
	if (!Tools::checkFileExists(fileName)) {
		parseConfig(iniFileName, std::string(""));
	}
	try {
		configFile_->load_from_file(fileName);
	}
	catch (Glib::FileError &err) {
		std::cerr << "settings.cpp::34:: " << fileName << " - " << err.what() << std::endl;
	}
}

void Settings::saveVolume(double volume)
{
	configFile_->set_double(Glib::ustring("main"),Glib::ustring("volume"), volume);
	parseConfig(iniFileName, configFile_->to_data());
}

double Settings::getVolume() const
{
	double value = 0;
	try {
		value = (double)configFile_->get_integer(Glib::ustring("main"),Glib::ustring("volume"));
	}
	catch (const Glib::KeyFileError& ex) {
		std::cerr << "settings.cpp::51::KeyFileError " << ex.what() << std::endl;
	}

	return value;
}

void Settings::parseConfig(const Glib::ustring& keyFileName, const Glib::ustring& keyFileData)
{
	Tools::saveFile(keyFileName, keyFileData);
}

int Settings::getSoundCard()
{
	int card = 0;
	try {
		card = (int)configFile_->get_integer(Glib::ustring("main"),Glib::ustring("card"));
	}
	catch (const Glib::KeyFileError& ex) {
		std::cerr << "settings.cpp::69::KeyFileError " << ex.what() << std::endl;
	}
	return card;
}

void Settings::saveSoundCard(int soundCard)
{
	configFile_->set_integer(Glib::ustring("main"),Glib::ustring("card"),soundCard);
	parseConfig(iniFileName, configFile_->to_data());
}

Glib::ustring Settings::getMixer()
{
	Glib::ustring mixer("");
	try {
		mixer = Glib::ustring(configFile_->get_string(Glib::ustring("main"),Glib::ustring("mixer")));
	}
	catch (const Glib::KeyFileError& ex) {
		std::cerr << "settings.cpp::87::KeyFileError " << ex.what() << std::endl;
	}
	return mixer;
}

void Settings::saveMixer(const std::string &mixerName)
{
	configFile_->set_string(Glib::ustring("main"),Glib::ustring("mixer"),mixerName);
	parseConfig(iniFileName, configFile_->to_data());
}

void Settings::saveNotebookOrientation(bool orient)
{
	configFile_->set_boolean(Glib::ustring("main"),Glib::ustring("orient"),orient);
	parseConfig(iniFileName, configFile_->to_data());
}

bool Settings::getNotebookOrientation()
{
	bool orient = false;
	try {
		orient = bool(configFile_->get_boolean(Glib::ustring("main"),Glib::ustring("orient")));
	}
	catch (const Glib::KeyFileError& ex) {
		std::cerr << "settings.cpp::111::KeyFileError " << ex.what() << std::endl;
	}
	return orient;
}

void Settings::loadDesktopFile(const std::string &fileName)
{
	if (!Tools::checkFileExists(fileName)) {
		initDesktopFileData();
		parseConfig(DesktopFilePath, desktopFile_->to_data());
	}
	try {
		desktopFile_->load_from_file(fileName);
	}
	catch (Glib::FileError &err) {
		std::cerr << "settings.cpp::126:: " << fileName << " - " << err.what() << std::endl;
	}
}

void Settings::initDesktopFileData()
{
	desktopFile_->set_string(Glib::ustring("Desktop Entry"),Glib::ustring("Encoding"),Glib::ustring("UTF-8"));
	desktopFile_->set_string(Glib::ustring("Desktop Entry"),Glib::ustring("Name"),Glib::ustring("AlsaVolume"));
	desktopFile_->set_string(Glib::ustring("Desktop Entry"),Glib::ustring("Comment"),Glib::ustring("Changes the volume of ALSA from the system tray"));
	desktopFile_->set_string(Glib::ustring("Desktop Entry"),Glib::ustring("Exec"),Glib::ustring("alsavolume"));
	desktopFile_->set_string(Glib::ustring("Desktop Entry"),Glib::ustring("Type"),Glib::ustring("Application"));
	desktopFile_->set_string(Glib::ustring("Desktop Entry"),Glib::ustring("Version"),Glib::ustring("0.0.6"));
	desktopFile_->set_boolean(Glib::ustring("Desktop Entry"),Glib::ustring("X-GNOME-Autostart-enabled"),false);
	desktopFile_->set_string(Glib::ustring("Desktop Entry"),Glib::ustring("Comment[ru]"),Glib::ustring("Регулятор громкости ALSA"));
}

void Settings::setAutorun(bool isAutorun)
{
	desktopFile_->set_boolean(Glib::ustring("Desktop Entry"),Glib::ustring("X-GNOME-Autostart-enabled"),isAutorun);
	parseConfig(DesktopFilePath, desktopFile_->to_data());
}

bool Settings::getAutorun()
{
	bool isAutorun = false;
	try {
		isAutorun = bool(desktopFile_->get_boolean(Glib::ustring("Desktop Entry"),Glib::ustring("X-GNOME-Autostart-enabled")));
	}
	catch (const Glib::KeyFileError& ex) {
		std::cerr << "settings.cpp::153::KeyFileError " << ex.what() << std::endl;
	}
	return isAutorun;
}

void Settings::setVersion(Glib::ustring &version)
{
	desktopFile_->set_string(Glib::ustring("Desktop Entry"),Glib::ustring("Version"), version);
	parseConfig(DesktopFilePath, desktopFile_->to_data());
}
