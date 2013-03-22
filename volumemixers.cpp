#include "volumemixers.h"

VolumeMixers::VolumeMixers()
{
	playback_ = new std::vector<std::string>();
	capture_ = new std::vector<std::string>();
}

VolumeMixers::~VolumeMixers()
{
	delete playback_;
	delete capture_;
}

std::vector<std::string> &VolumeMixers::playback()
{
	return *playback_;
}

std::vector<std::string> &VolumeMixers::capture()
{
	return *capture_;
}

void VolumeMixers::setPlayback(std::vector<std::string> &list)
{
	playback_ = &list;
}

void VolumeMixers::setCapture(std::vector<std::string> &list)
{
	capture_ = &list;
}

void VolumeMixers::playBackClear()
{
	if (!playback_->empty())
		playback_->clear();
}

void VolumeMixers::captureClear()
{
	if (!capture_->empty())
		capture_->clear();
}

void VolumeMixers::pushBack(int mixerType, const std::string &item)
{
	switch (mixerType) {
	case 0:
		playback_->push_back(item);
		break;
	case 1:
		capture_->push_back(item);
		break;
	}
}
