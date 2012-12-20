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
	int getSoundCard();
	void saveSoundCard(int soundCard);
	Glib::ustring getMixer();
	void saveMixer(const std::string& mixerName);
private:
	void parseConfig(const Glib::ustring& keyFileData);
	void loadConfig(const std::string& fileName);

private:
	Glib::KeyFile *configFile_;
	std::string iniFileName_;
};

#endif // SETTINGS_H
