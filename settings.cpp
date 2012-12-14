#include "settings.h"
#include "filework.h"
#include "glibmm/keyfile.h"
#include "glibmm/fileutils.h"
#include "fstream"

const std::string homeDir = std::string(getenv("HOME"));
const std::string configDir = std::string(homeDir + "/.config/cppAlsaVolume");
const std::string iniFileName = configDir +std::string("/config.ini");

Settings::Settings()
{
	configFile_ = new Glib::KeyFile();
	loadConfig(iniFileName.c_str());
}

Settings::~Settings()
{
	delete configFile_;
}

void Settings::loadConfig(const char *fileName)
{
	if (!checkFileExists(fileName)) {
		parseDefaultConfig();
	}
	configFile_->load_from_file(fileName);
}

void Settings::saveVolume(double volume)
{
	configFile_->set_double(Glib::ustring("main"),Glib::ustring("volume"), volume);
	parseConfig(configFile_->to_data());
}

double Settings::getVolume()
{
	double value = 0;
	try {
		value = (double)configFile_->get_integer(Glib::ustring("main"),Glib::ustring("volume"));
	}
	catch (const Glib::KeyFileError& ex) {
		std::cerr << "settings.cpp::36::KeyFileError " << ex.what() << std::endl;
	}

	return value;
}

void Settings::parseDefaultConfig()
{
	gint err = g_mkdir_with_parents(configDir.c_str(), 0755);
	if (err == 0) {
		std::ofstream ofile(iniFileName.c_str());
		ofile << "" << std::endl;
		ofile.close();
	}
	else {
		std::cerr << g_file_error_from_errno(err) << std::endl;
	}
}

void Settings::parseConfig(Glib::ustring keyFile)
{
	std::ofstream ofile(iniFileName.c_str());
	ofile << keyFile << std::endl;
	ofile.close();
}
