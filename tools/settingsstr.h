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
	unsigned int cardId() const;
	unsigned int mixerId() const;
	bool notebookOrientation();
	bool isAutorun();
	std::string &externalMixer();
	std::vector<std::string> &cardList();
	std::vector<std::string> &mixerList();
#ifdef HAVE_ICONPACKS
	std::string &currIconPack();
	std::vector<std::string> &iconPacks();
	void setCurrIconPack(const std::string &iconPack);
#endif
	MixerSwitches &switchList();
	void setCardId(unsigned int id);
	void setMixerId(unsigned int id);
	void setNotebookOrientation(bool orient);
	void setIsAutorun(bool autorun);
	void pushBack(ListType listType, const std::string &item);
	void addMixerSwitch(const MixerSwitches &switchItem);
	void setList(ListType listType, const std::vector<std::string> &list);
	void clear(ListType listType);
	void clearSwitches();
	void setExternalMixer(const std::string &name);


private:
	unsigned int cardId_;
	unsigned int mixerId_;
	bool notebookOrientation_;
	bool isAutorun_;
	std::vector<std::string> cardList_;
	std::vector<std::string> mixerList_;
#ifdef HAVE_ICONPACKS
	std::string currIconPack_;
	std::vector<std::string> iconPacks_;
#endif
	MixerSwitches switchList_;
	std::string extMixer_;
};

#endif // SETTINGSSTR_H
