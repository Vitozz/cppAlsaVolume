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
	void saveNotebookOrientation(bool orient);
	bool getNotebookOrientation();
	void setAutorun(bool isAutorun);
	bool getAutorun();
	void setVersion(Glib::ustring& version);
private:
	void parseConfig(const Glib::ustring& keyFileName, const Glib::ustring& keyFileData);
	void loadConfig(const std::string& fileName);
	void loadDesktopFile(const std::string& fileName);
	void initDesktopFileData();

private:
	Glib::KeyFile *configFile_;
	Glib::KeyFile *desktopFile_;
};

#endif // SETTINGS_H
