/*
 * volumemixers.h
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

#ifndef VOLUMEMIXERS_H
#define VOLUMEMIXERS_H

#include <string>
#include <vector>

class VolumeMixers
{
public:
	VolumeMixers();
	std::vector<std::string> &playback();
	std::vector<std::string> &capture();
	void setPlayback(const std::vector<std::string> &list);
	void setCapture(const std::vector<std::string> &list);
	void playBackClear();
	void captureClear();
	void pushBack(int mixerType, const std::string &item);
private:
	std::vector<std::string> playback_;
	std::vector<std::string> capture_;
};

#endif // VOLUMEMIXERS_H
