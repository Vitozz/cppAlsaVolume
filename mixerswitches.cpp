#include "mixerswitches.h"

MixerSwitches::MixerSwitches()
{
	playbackSwitchList_ = new std::vector<switchcap>();
	captureSwitchList_ = new std::vector<switchcap>();
	enumSwitchList_ = new std::vector<switchcap>();
}

MixerSwitches::~MixerSwitches()
{
	delete playbackSwitchList_;
	delete captureSwitchList_;
	delete enumSwitchList_;
}

void MixerSwitches::setCaptureSwitchList(std::vector<switchcap> &list)
{
	captureSwitchList_ = &list;
}

void MixerSwitches::setPlaybackSwitchList(std::vector<switchcap> &list)
{
	playbackSwitchList_ = &list;
}

void MixerSwitches::setEnumSwitchList(std::vector<switchcap> &list)
{
	enumSwitchList_ = &list;
}

void MixerSwitches::pushBack(SwitchType sType, switchcap &item)
{
	switch (sType) {
	case PLAYBACK:
		playbackSwitchList_->push_back(item);
		break;
	case CAPTURE:
		captureSwitchList_->push_back(item);
		break;
	case ENUM:
		enumSwitchList_->push_back(item);
		break;
	}
}

void MixerSwitches::clear(SwitchType sType)
{
	switch (sType) {
	case PLAYBACK:
		if (!playbackSwitchList_->empty())
			playbackSwitchList_->clear();
		break;
	case CAPTURE:
		if (!captureSwitchList_->empty())
			captureSwitchList_->clear();
		break;
	case ENUM:
		if (!enumSwitchList_->empty())
			enumSwitchList_->clear();
		break;
	}
}

std::vector<switchcap> &MixerSwitches::captureSwitchList()
{
	return *captureSwitchList_;
}

std::vector<switchcap> &MixerSwitches::playbackSwitchList()
{
	return *playbackSwitchList_;
}

std::vector<switchcap> &MixerSwitches::enumSwitchList()
{
	return *enumSwitchList_;
}
