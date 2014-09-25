/*
 * settingsstr.h
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

#ifndef SETTINGSSTR_H
#define SETTINGSSTR_H

#include "../alsawork/mixerswitches.h"
#include <string>
#include <vector>

class settingsStr
{
public:
	settingsStr();
	settingsStr(settingsStr &str);
	~settingsStr();
	unsigned int cardId() const;
	unsigned int mixerId() const;
	int pulseDeviceId() const;
	bool notebookOrientation();
	bool isAutorun();
	bool usePulse();
	const std::string &pulseDeviceName() const;
	const std::string &pulseDeviceDesc() const;
	const std::vector<std::string> &cardList() const;
	const std::vector<std::string> &mixerList() const;
	const std::vector<std::string> &pulseDevices() const;
	const MixerSwitches &switchList() const;
	void setCardId(unsigned int id);
	void setMixerId(unsigned int id);
	void setNotebookOrientation(bool orient);
	void setIsAutorun(bool autorun);
	void pushBack(ListType listType, const std::string &item);
	void addMixerSwitch(const MixerSwitches &switchItem);
	void setList(ListType listType, const std::vector<std::string> &list);
	void clear(ListType listType);
	void clearSwitches();
	void setPulseDeviceId(int id);
	void setPulseDeviceName(const std::string &name);
	void setPulseDeviceDesc(const std::string &description);
	void setPulseDevices(const std::vector<std::string> &devices);
	void setUsePulse(bool use);

private:
	unsigned int cardId_;
	unsigned int mixerId_;
	bool notebookOrientation_;
	bool isAutorun_;
	bool usePulse_;
	std::vector<std::string> cardList_;
	std::vector<std::string> mixerList_;
	MixerSwitches *switchList_;
	std::vector<std::string> pulseDevices_;
	int pulseDeviceId_;
	std::string pulseDeviceName_;
	std::string pulseDeviceDesc_;
};

#endif // SETTINGSSTR_H
