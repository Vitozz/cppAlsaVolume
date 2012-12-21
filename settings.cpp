#include "settings.h"
#include "tools.h"
#include "glibmm/keyfile.h"
#include "glibmm/fileutils.h"
#include "fstream"

Settings::Settings()
{
	std::string configDir = std::string(std::string(getenv("HOME")) + "/.config/cppAlsaVolume");
	iniFileName_ = configDir + std::string("/config.ini");
	configFile_ = new Glib::KeyFile();
	Tools::createDirectory(configDir);
	loadConfig(iniFileName_.c_str());
}

Settings::~Settings()
{
	delete configFile_;
}

void Settings::loadConfig(const std::string& fileName)
{
	if (!Tools::checkFileExists(fileName)) {
		parseConfig(std::string(""));
	}
	try {
		configFile_->load_from_file(fileName);
	}
	catch (Glib::FileError &err) {
		std::cerr << "settings.cpp::27:: " << fileName << " - " << err.what() << std::endl;
	}
}

void Settings::saveVolume(double volume)
{
	configFile_->set_double(Glib::ustring("main"),Glib::ustring("volume"), volume);
	parseConfig(configFile_->to_data());
}

double Settings::getVolume() const
{
	double value = 0;
	try {
		value = (double)configFile_->get_integer(Glib::ustring("main"),Glib::ustring("volume"));
	}
	catch (const Glib::KeyFileError& ex) {
		std::cerr << "settings.cpp::40::KeyFileError " << ex.what() << std::endl;
	}

	return value;
}

void Settings::parseConfig(const Glib::ustring& keyFileData)
{
	Tools::saveFile(iniFileName_, keyFileData);
}

int Settings::getSoundCard()
{
	int card = 0;
	try {
		card = (int)configFile_->get_integer(Glib::ustring("main"),Glib::ustring("card"));
	}
	catch (const Glib::KeyFileError& ex) {
		std::cerr << "settings.cpp::62::KeyFileError " << ex.what() << std::endl;
	}
	return card;
}

void Settings::saveSoundCard(int soundCard)
{
	configFile_->set_integer(Glib::ustring("main"),Glib::ustring("card"),soundCard);
	parseConfig(configFile_->to_data());
}

Glib::ustring Settings::getMixer()
{
	Glib::ustring mixer("");
	try {
		mixer = Glib::ustring(configFile_->get_string(Glib::ustring("main"),Glib::ustring("mixer")));
	}
	catch (const Glib::KeyFileError& ex) {
		std::cerr << "settings.cpp::80::KeyFileError " << ex.what() << std::endl;
	}
	return mixer;
}

void Settings::saveMixer(const std::string &mixerName)
{
	configFile_->set_string(Glib::ustring("main"),Glib::ustring("mixer"),mixerName);
	parseConfig(configFile_->to_data());
}

void Settings::saveNotebookOrientation(bool orient)
{
	configFile_->set_boolean(Glib::ustring("main"),Glib::ustring("orient"),orient);
	parseConfig(configFile_->to_data());
}

bool Settings::getNotebookOrientation()
{
	bool orient = false;
	try {
		orient = bool(configFile_->get_boolean(Glib::ustring("main"),Glib::ustring("orient")));
	}
	catch (const Glib::KeyFileError& ex) {
		std::cerr << "settings.cpp::104::KeyFileError " << ex.what() << std::endl;
	}
	return orient;
}
