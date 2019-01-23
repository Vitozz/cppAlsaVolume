/*
 * mixerswitches.cpp
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

#include "mixerswitches.h"

MixerSwitches::MixerSwitches()
{
}

MixerSwitches::MixerSwitches(const MixerSwitches &ms)
    : captureSwitchList_(ms.captureSwitchList()),
      playbackSwitchList_(ms.playbackSwitchList()),
      enumSwitchList_(ms.enumSwitchList())
{
}

void MixerSwitches::setCaptureSwitchList(const std::vector<switchcap> &list)
{
    captureSwitchList_.assign(list.begin(), list.end());
}

void MixerSwitches::setPlaybackSwitchList(const std::vector<switchcap> &list)
{
    playbackSwitchList_.assign(list.begin(), list.end());
}

void MixerSwitches::setEnumSwitchList(const std::vector<switchcap> &list)
{
    enumSwitchList_.assign(list.begin(), list.end());
}

void MixerSwitches::pushBack(SwitchType sType, switchcap &item)
{
    switch (sType) {
    case PLAYBACK:
        playbackSwitchList_.push_back(item);
        break;
    case CAPTURE:
        captureSwitchList_.push_back(item);
        break;
    case ENUM:
        enumSwitchList_.push_back(item);
        break;
    }
}

void MixerSwitches::clear(SwitchType sType)
{
    switch (sType) {
    case PLAYBACK:
        if (!playbackSwitchList_.empty())
            playbackSwitchList_.clear();
        break;
    case CAPTURE:
        if (!captureSwitchList_.empty())
            captureSwitchList_.clear();
        break;
    case ENUM:
        if (!enumSwitchList_.empty())
            enumSwitchList_.clear();
        break;
    }
}

void MixerSwitches::clearAll()
{
    if (!playbackSwitchList_.empty()) {
        playbackSwitchList_.clear();
    }
    if (!captureSwitchList_.empty()) {
        captureSwitchList_.clear();
    }
    if (!enumSwitchList_.empty()) {
        enumSwitchList_.clear();
    }
}

bool MixerSwitches::isEmpty()
{
    return (playbackSwitchList_.empty() && captureSwitchList_.empty() && enumSwitchList_.empty());
}

const std::vector<switchcap> &MixerSwitches::captureSwitchList() const
{
    return captureSwitchList_;
}

const std::vector<switchcap> &MixerSwitches::playbackSwitchList() const
{
    return playbackSwitchList_;
}

const std::vector<switchcap> &MixerSwitches::enumSwitchList() const
{
    return enumSwitchList_;
}
