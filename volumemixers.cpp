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
	playback_ = list;
}

void VolumeMixers::setCapture(std::vector<std::string> &list)
{
	capture_ = list;
}
