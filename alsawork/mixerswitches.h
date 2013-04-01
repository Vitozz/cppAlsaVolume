/*
 * mixerswitches.h
 * Copyright (C) 2012 Vitaly Tonkacheyev
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef MIXERSWITCHES_H
#define MIXERSWITCHES_H

#include "../tools/tools.h"

class MixerSwitches
{
public:
	MixerSwitches();
	void pushBack(SwitchType sType, switchcap &item);
	void setCaptureSwitchList(const std::vector<switchcap> &list);
	void setPlaybackSwitchList(const std::vector<switchcap> &list);
	void setEnumSwitchList(const std::vector<switchcap> &list);
	void clear(SwitchType sType);
	std::vector<switchcap> &captureSwitchList();
	std::vector<switchcap> &playbackSwitchList();
	std::vector<switchcap> &enumSwitchList();

private:
	std::vector<switchcap> captureSwitchList_;
	std::vector<switchcap> playbackSwitchList_;
	std::vector<switchcap> enumSwitchList_;
};

#endif // MIXERSWITCHES_H
