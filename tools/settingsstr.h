/*
 * settingsstr.h
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

#ifndef SETTINGSSTR_H
#define SETTINGSSTR_H

#include "../alsawork/mixerswitches.h"
#include <string>
#include <vector>
#include <memory>

class settingsStr
{
public:
    settingsStr();
    settingsStr(settingsStr &str);
    ~settingsStr() = default;
    typedef std::shared_ptr<settingsStr> Ptr;
    unsigned int cardId() const;
    unsigned int mixerId() const;
    int pulseDeviceId() const;
    bool notebookOrientation();
    bool isAutorun();
    bool usePulse();
    bool usePolling();
    const std::string &pulseDeviceName() const;
    const std::string &pulseDeviceDesc() const;
    const std::vector<std::string> &cardList() const;
    const std::vector<std::string> &mixerList() const;
    const std::vector<std::string> &pulseDevices() const;
    MixerSwitches::Ptr switchList() const;
    void setCardId(unsigned int id);
    void setMixerId(unsigned int id);
    void setNotebookOrientation(bool orient);
    void setIsAutorun(bool autorun);
    void addMixerSwitch(const MixerSwitches::Ptr &switchItem);
    void setList(ListType listType, const std::vector<std::string> &list);
    void clear(ListType listType);
    void clearSwitches();
    void setPulseDeviceId(int id);
    void setPulseDeviceName(const std::string &name);
    void setPulseDeviceDesc(const std::string &description);
    void setPulseDevices(const std::vector<std::string> &devices);
    void setUsePulse(bool use);
    void setUsePolling(bool use);

private:
    unsigned int cardId_;
    unsigned int mixerId_;
    bool notebookOrientation_;
    bool isAutorun_;
    bool usePulse_;
    bool usePolling_;
    std::vector<std::string> cardList_;
    std::vector<std::string> mixerList_;
    MixerSwitches::Ptr switchList_;
    std::vector<std::string> pulseDevices_;
    int pulseDeviceId_;
    std::string pulseDeviceName_;
    std::string pulseDeviceDesc_;
};

#endif // SETTINGSSTR_H
