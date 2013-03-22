#include "settingsstr.h"

settingsStr::settingsStr()
: cardId_(0),
  mixerId_(0),
  notebookOrientation_(false),
  isAutorun_(false)
{
	currIconPack_ = std::string();
	cardList_ = new std::vector<std::string>();
	mixerList_ = new std::vector<std::string>();
	iconPacks_ = new std::vector<std::string>();
	switchList_ = new MixerSwitches();
}

settingsStr::~settingsStr()
{
	delete cardList_;
	delete mixerList_;
	delete iconPacks_;
	delete switchList_;
}

unsigned int settingsStr::cardId()
{
	return cardId_;
}

unsigned int settingsStr::mixerId()
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
	return *cardList_;
}

std::vector<std::string> &settingsStr::mixerList()
{
	return *mixerList_;
}

std::vector<std::string> &settingsStr::iconPacks()
{
	return *iconPacks_;
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
		cardList_->push_back(item);
		break;
	case MIXERS:
		mixerList_->push_back(item);
		break;
	case ICONS:
		iconPacks_->push_back(item);
		break;
	}
}

void settingsStr::addMixerSwitch(MixerSwitches &switchItem)
{
	switchList_ = &switchItem;
}

void settingsStr::setList(ListType listType, std::vector<std::string> &list)
{
	switch (listType) {
	case CARDS:
		cardList_ = &list;
		break;
	case MIXERS:
		mixerList_ = &list;
		break;
	case ICONS:
		iconPacks_ = &list;
		break;
	}
}

void settingsStr::setCurrIconPack(const std::string &iconPack)
{
	currIconPack_ = iconPack;
}

MixerSwitches &settingsStr::switchList()
{
	return *switchList_;
}
