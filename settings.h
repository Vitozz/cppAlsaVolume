#ifndef SETTINGS_H
#define SETTINGS_H
#include "glibmm/keyfile.h"

class Settings
{
public:
	Settings();
	virtual ~Settings();
	void saveVolume(double volume);
	double getVolume() const;
private:
	void parseDefaultConfig();
	void parseConfig(const Glib::ustring& keyFile);
	void loadConfig(const std::string& fileName);
	void createConfigDir(const std::string& dirname);

private:
	Glib::KeyFile *configFile_;
	std::string iniFileName_;
};

#endif // SETTINGS_H
