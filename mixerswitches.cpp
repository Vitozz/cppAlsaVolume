#include "mixerswitches.h"

MixerSwitches::MixerSwitches()
{
}

void MixerSwitches::setCaptureSwitchList(boost::ptr_vector<switchcap> list)
{
	captureSwitchList_ = list;
}

void MixerSwitches::setPlaybackSwitchList(boost::ptr_vector<switchcap> list)
{
	playbackSwitchList_ = list;
}

void MixerSwitches::setEnumSwitchList(boost::ptr_vector<switchcap> list)
{
	enumSwitchList_ = list;
}

void MixerSwitches::pushBack(SwitchType sType, switchcap *item)
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
	/*switch (sType) {
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
	}*/
}

boost::ptr_vector<switchcap> MixerSwitches::captureSwitchList()
{
	return captureSwitchList_;
}

boost::ptr_vector<switchcap> MixerSwitches::playbackSwitchList()
{
	return playbackSwitchList_;
}

boost::ptr_vector<switchcap> MixerSwitches::enumSwitchList()
{
	return enumSwitchList_;
}
