#include "settings.h"
#include "filework.h"
#include "glibmm/keyfile.h"
#include "glibmm/fileutils.h"
#include "fstream"

Settings::Settings()
{
	std::string configDir = std::string(std::string(getenv("HOME")) + "/.config/cppAlsaVolume");
	iniFileName_ = configDir + std::string("/config.ini");
	configFile_ = new Glib::KeyFile();
	FileWork::createDirectory(configDir);
	loadConfig(iniFileName_.c_str());
}

Settings::~Settings()
{
	delete configFile_;
}

void Settings::loadConfig(const std::string& fileName)
{
	if (!FileWork::checkFileExists(fileName)) {
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
	FileWork::saveFile(iniFileName_, keyFileData);
}
