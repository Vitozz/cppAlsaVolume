#ifndef SETTINGS_H
#define SETTINGS_H
#include "glibmm/keyfile.h"

class Settings
{
public:
	Settings();
	virtual ~Settings();
	void saveVolume(double volume);
	double getVolume();
private:
	void parseDefaultConfig();
	void parseConfig(Glib::ustring keyFile);
	void loadConfig(const char *fileName);
private:
	Glib::KeyFile *configFile_;
};

#endif // SETTINGS_H
