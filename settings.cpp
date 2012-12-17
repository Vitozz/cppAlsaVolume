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
	createConfigDir(configDir);
	loadConfig(iniFileName_.c_str());
}

Settings::~Settings()
{
	delete configFile_;
}

void Settings::loadConfig(const std::string& fileName)
{
	if (!checkFileExists(fileName)) {
		parseConfig(std::string(""));
	}
	configFile_->load_from_file(fileName);
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

void Settings::parseConfig(const Glib::ustring& keyFile)
{
	try {
		std::ofstream ofile(iniFileName_.c_str());
		ofile << keyFile << std::endl;
		ofile.close();
	}
	catch ( const std::exception & ex ) {
		std::cout << "settings.cpp::57::Parsing failed:: " << ex.what() << std::endl;
	}
}

void Settings::createConfigDir(const std::string& dirname)
{
	if (Glib::file_test(dirname, Glib::FILE_TEST_IS_DIR)) {
		gint err = 0;
		err = g_mkdir_with_parents(dirname.c_str(), 0755);
		if (err < 0) {
			std::cerr << g_file_error_from_errno(err) << std::endl;
		}
	}
}
