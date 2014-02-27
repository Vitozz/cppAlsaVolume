/*
 * volumemixers.cpp
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

#include "volumemixers.h"

VolumeMixers::VolumeMixers()
{
}

std::vector<std::string> VolumeMixers::playback()
{
	if (!playback_.empty()) {
		return playback_;
	}
	return std::vector<std::string>();
}

std::vector<std::string> VolumeMixers::capture()
{
	if (!capture_.empty()) {
		return capture_;
	}
	return std::vector<std::string>();
}

void VolumeMixers::setPlayback(const std::vector<std::string> &list)
{
	playback_.assign(list.begin(), list.end());
}

void VolumeMixers::setCapture(const std::vector<std::string> &list)
{
	capture_.assign(list.begin(), list.end());
}

void VolumeMixers::playBackClear()
{
	if (!playback_.empty())
		playback_.clear();
}

void VolumeMixers::captureClear()
{
	if (!capture_.empty())
		capture_.clear();
}

void VolumeMixers::pushBack(int mixerType, const std::string &item)
{
	switch (mixerType) {
	case 0:
		playback_.push_back(item);
		break;
	case 1:
		capture_.push_back(item);
		break;
	}
}

bool VolumeMixers::isEmpty()
{
	if (playback_.empty() && capture_.empty()) {
		return true;
	}
	return false;
}

