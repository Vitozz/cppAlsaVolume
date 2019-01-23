/*
 * mixerswitches.h
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

#ifndef MIXERSWITCHES_H
#define MIXERSWITCHES_H

#include "../tools/tools.h"
#include <memory>

class MixerSwitches
{
public:
    MixerSwitches();
    MixerSwitches(const MixerSwitches &ms);
    typedef std::shared_ptr<MixerSwitches> Ptr;
    void pushBack(SwitchType sType, switchcap &item);
    void setCaptureSwitchList(const std::vector<switchcap> &list);
    void setPlaybackSwitchList(const std::vector<switchcap> &list);
    void setEnumSwitchList(const std::vector<switchcap> &list);
    void clear(SwitchType sType);
    void clearAll();
    const std::vector<switchcap> &captureSwitchList() const;
    const std::vector<switchcap> &playbackSwitchList() const;
    const std::vector<switchcap> &enumSwitchList() const;
    bool isEmpty();

private:
    std::vector<switchcap> captureSwitchList_;
    std::vector<switchcap> playbackSwitchList_;
    std::vector<switchcap> enumSwitchList_;
};

#endif // MIXERSWITCHES_H
