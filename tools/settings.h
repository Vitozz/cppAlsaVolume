/*
 * settings.h
 * Copyright (C) 2012-2019 Vitaly Tonkacheyev
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef SETTINGS_H
#define SETTINGS_H
#include "glibmm/keyfile.h"
#include <memory>

class Settings
{
public:
    Settings();
    Settings(Settings const &);
    ~Settings();
    typedef std::shared_ptr<Settings> Ptr;
    int getSoundCard() const;
    int getMixerId() const;
    Glib::ustring getMixer() const;
    bool getNotebookOrientation();
    bool getAutorun();
    bool usePulse();
    bool usePolling();
    std::string pulseDeviceName() const;
    void saveSoundCard(int soundCard);
    void saveMixer(const std::string& mixerName);
    void saveMixerId(int mixerId);
    void saveNotebookOrientation(bool orient);
    void setAutorun(bool isAutorun);
    void setVersion(const Glib::ustring& version);
    void setUsePulse(bool use);
    void savePulseDeviceName(const std::string &name);
    void setUsePolling(bool use);

private:
    void parseConfig(const Glib::ustring& keyFileName, const Glib::ustring& keyFileData);
    void loadConfig(const std::string& fileName);
    void loadDesktopFile(const std::string& fileName);
    void initDesktopFileData();

private:
    Glib::KeyFile *configFile_;
    Glib::KeyFile *desktopFile_;
    std::string iniFileName_;
    std::string desktopFilePath_;
    std::string version_;
};

#endif // SETTINGS_H
