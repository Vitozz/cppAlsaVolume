/*
 * settingsstr.cpp
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

#include "settingsstr.h"

settingsStr::settingsStr()
    : cardId_(0),
      mixerId_(0),
      notebookOrientation_(false),
      isAutorun_(false),
      usePulse_(false),
      usePolling_(true),
      cardList_(std::vector<std::string>()),
      mixerList_(std::vector<std::string>()),
      switchList_(MixerSwitches::Ptr()),
      pulseDevices_(std::vector<std::string>()),
      pulseDeviceId_(0),
      pulseDeviceName_(std::string()),
      pulseDeviceDesc_(std::string())
{
}

settingsStr::settingsStr(settingsStr &str)
    : cardId_(str.cardId()),
      mixerId_(str.mixerId()),
      notebookOrientation_(str.notebookOrientation()),
      isAutorun_(str.isAutorun()),
      usePulse_(str.usePulse()),
      usePolling_(str.usePolling()),
      cardList_(str.cardList()),
      mixerList_(str.mixerList()),
      switchList_(str.switchList()),
      pulseDevices_(str.pulseDevices()),
      pulseDeviceId_(str.pulseDeviceId()),
      pulseDeviceName_(str.pulseDeviceName()),
      pulseDeviceDesc_(str.pulseDeviceDesc())
{
}

settingsStr::~settingsStr()
{
}

unsigned int settingsStr::cardId() const
{
    return cardId_;
}

unsigned int settingsStr::mixerId() const
{
    return mixerId_;
}

bool settingsStr::notebookOrientation()
{
    return notebookOrientation_;
}

bool settingsStr::isAutorun()
{
    return isAutorun_;
}

bool settingsStr::usePolling()
{
    return usePolling_;
}

const std::vector<std::string> &settingsStr::cardList() const
{
    return cardList_;
}

const std::vector<std::string> &settingsStr::mixerList() const
{
    return mixerList_;
}

int settingsStr::pulseDeviceId() const
{
    return pulseDeviceId_;
}

const std::string &settingsStr::pulseDeviceName() const
{
    return pulseDeviceName_;
}

const std::string &settingsStr::pulseDeviceDesc() const
{
    return pulseDeviceDesc_;
}

const std::vector<std::string> &settingsStr::pulseDevices() const
{
    return pulseDevices_;
}

bool settingsStr::usePulse()
{
    return usePulse_;
}

void settingsStr::setCardId(unsigned int id)
{
    cardId_ = id;
}

void settingsStr::setMixerId(unsigned int id)
{
    mixerId_ = id;
}

void settingsStr::setNotebookOrientation(bool orient)
{
    notebookOrientation_ = orient;
}

void settingsStr::setIsAutorun(bool autorun)
{
    isAutorun_ = autorun;
}

void settingsStr::pushBack(ListType listType, const std::string &item)
{
    switch (listType) {
    case CARDS:
        cardList_.push_back(item);
        break;
    case MIXERS:
        mixerList_.push_back(item);
        break;
    }
}

void settingsStr::addMixerSwitch(const MixerSwitches::Ptr &switchItem)
{
    switchList_ = switchItem;
}

void settingsStr::setList(ListType listType,const  std::vector<std::string> &list)
{
    switch (listType) {
    case CARDS:
        cardList_.assign(list.begin(),list.end());
        break;
    case MIXERS:
        mixerList_.assign(list.begin(),list.end());
        break;
    }

}

MixerSwitches::Ptr settingsStr::switchList() const
{
    return switchList_;
}

void settingsStr::clear(ListType listType)
{
    switch (listType) {
    case CARDS:
        if (!cardList_.empty())
            cardList_.clear();
        break;
    case MIXERS:
        if (!mixerList_.empty())
            mixerList_.clear();
        break;
    }

}

void settingsStr::clearSwitches()
{
    if (switchList_) {
        switchList_->clear(PLAYBACK);
        switchList_->clear(CAPTURE);
        switchList_->clear(ENUM);
    }
}

void settingsStr::setPulseDeviceId(int id)
{
    pulseDeviceId_ = id;
}

void settingsStr::setPulseDeviceName(const std::string &name)
{
    pulseDeviceName_ = name;
}

void settingsStr::setPulseDeviceDesc(const std::string &description)
{
    pulseDeviceDesc_ = description;
}

void settingsStr::setPulseDevices(const std::vector<std::string> &devices)
{
    if(!pulseDevices_.empty() && !devices.empty())
        pulseDevices_.clear();
    pulseDevices_.assign(devices.begin(), devices.end());
}

void settingsStr::setUsePulse(bool use)
{
    usePulse_ = use;
}

void settingsStr::setUsePolling(bool use)
{
    usePolling_ = use;
}
