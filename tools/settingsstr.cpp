/*
 * settingsstr.cpp
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

#include "settingsstr.h"

settingsStr::settingsStr()
: cardId_(0),
  mixerId_(0),
  notebookOrientation_(false),
  isAutorun_(false)
{
	currIconPack_ = std::string();
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

std::string &settingsStr::currIconPack()
{
	return currIconPack_;
}

std::vector<std::string> &settingsStr::cardList()
{
	return cardList_;
}

std::vector<std::string> &settingsStr::mixerList()
{
	return mixerList_;
}

std::vector<std::string> &settingsStr::iconPacks()
{
	return iconPacks_;
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
	case ICONS:
		iconPacks_.push_back(item);
		break;
	}
}

void settingsStr::addMixerSwitch(const MixerSwitches &switchItem)
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
	case ICONS:
		iconPacks_.assign(list.begin(),list.end());
		break;
	}
}

void settingsStr::setCurrIconPack(const std::string &iconPack)
{
	currIconPack_ = iconPack;
}

MixerSwitches &settingsStr::switchList()
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
	case ICONS:
		if (!iconPacks_.empty())
			iconPacks_.clear();
		break;
	}
}

void settingsStr::clearSwitches()
{
	switchList_.clear(PLAYBACK);
	switchList_.clear(CAPTURE);
	switchList_.clear(ENUM);
}
