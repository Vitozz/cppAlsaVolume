/*
 * alsadevice.cpp
 * Copyright (C) 2014 Vitaly Tonkacheyev
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
#include "alsadevice.h"

const double ZERO = 0.0;

AlsaDevice::AlsaDevice(int id, const std::string &card)
: id_(id),
  name_(card),
  volumeMixers_(std::vector<std::string>()),
  captureMixers_(std::vector<std::string>()),
  mixers_(std::vector<std::string>()),
  switches_(new MixerSwitches()),
  currentMixerId_(0),
  currentMixerName_(std::string())
{
	snd_mixer_t *handle = getMixerHanlde(id);
	snd_mixer_selem_id_t *smid;
	snd_mixer_selem_id_alloca(&smid);
	std::string deviceName;
	for (snd_mixer_elem_t *element = snd_mixer_first_elem(handle);
	     element;
	     element = snd_mixer_elem_next(element)) {
		switchcap sCap;
		snd_mixer_selem_get_id(element, smid);
		deviceName = snd_mixer_selem_id_get_name(smid);
		snd_mixer_selem_channel_id_t channel = checkMixerChannels(element);

		if (snd_mixer_selem_has_playback_volume(element)
		    || snd_mixer_selem_has_playback_volume_joined(element)
		    || snd_mixer_selem_has_common_volume(element)) {
			volumeMixers_.push_back(deviceName);
		}
		if (snd_mixer_selem_has_capture_volume(element)
		    || snd_mixer_selem_has_capture_volume_joined(element)) {
			captureMixers_.push_back(deviceName);
		}
		if (snd_mixer_selem_has_capture_switch(element)
		    || snd_mixer_selem_has_common_switch(element)
		    || snd_mixer_selem_has_capture_switch_joined(element)
		    || snd_mixer_selem_has_capture_switch_exclusive(element)){
			int value = 0;
			checkError(snd_mixer_selem_get_capture_switch(element, channel, &value));
			sCap = std::make_pair(deviceName, bool(value));
			switches_->pushBack(CAPTURE, sCap);
		}
		if (snd_mixer_selem_has_playback_switch(element)
		    || snd_mixer_selem_has_playback_switch_joined(element)){
			int value = 0;
			checkError(snd_mixer_selem_get_playback_switch(element, channel, &value));
			sCap = std::make_pair(deviceName, bool(value));
			switches_->pushBack(PLAYBACK, sCap);
		}
		if (snd_mixer_selem_is_enumerated(element)) {
			uint value = 0;
			checkError(snd_mixer_selem_get_enum_item(element, channel, &value));
			sCap = std::make_pair(deviceName, bool(value));
			switches_->pushBack(ENUM, sCap);
		}
	}
	checkError(snd_mixer_close(handle));
	initMixerList();
}

AlsaDevice::~AlsaDevice()
{
	delete switches_;
}

void AlsaDevice::initMixerList()
{
	bool isplay = havePlaybackMixers();
	bool isrec = haveCaptureMixers();
	if (!mixers_.empty()) {
		mixers_.clear();
	}
	if (isplay) {
		mixers_.assign(volumeMixers_.begin(), volumeMixers_.end());
	}
	if (isrec && !mixers_.empty()) {
		mixers_.insert(mixers_.end(), captureMixers_.begin(), captureMixers_.end());
	}
	if (!mixers_.empty()) {
		setCurrentMixer(0);
	}
}

snd_mixer_elem_t *AlsaDevice::initMixerElement(snd_mixer_t *handle, const char *mixer)
{
	snd_mixer_selem_id_t *smid;
	snd_mixer_selem_id_alloca(&smid);
	snd_mixer_selem_id_set_index(smid, 0);
	snd_mixer_selem_id_set_name(smid, mixer);
	snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, smid);
	return elem;
}

snd_mixer_t *AlsaDevice::getMixerHanlde(int id)
{
	std::string card(formatCardName(id));
	snd_ctl_t *ctl;
	checkError(snd_ctl_open(&ctl, card.c_str(), SND_CTL_NONBLOCK));
	snd_hctl_t *hctl;
	checkError(snd_hctl_open_ctl(&hctl, ctl));
	snd_mixer_t *handle;
	checkError(snd_mixer_open(&handle, 0));
	checkError(snd_mixer_attach_hctl(handle, hctl));
	checkError(snd_mixer_selem_register(handle, NULL, NULL));
	checkError(snd_mixer_load(handle));
	return handle;
}

snd_mixer_selem_channel_id_t AlsaDevice::checkMixerChannels(snd_mixer_elem_t *element)
{
	if (snd_mixer_selem_is_playback_mono(element)) {
		return SND_MIXER_SCHN_MONO;
	}
	else {
		for (int channel = 0; channel <= SND_MIXER_SCHN_LAST; channel++) {
			if (snd_mixer_selem_has_playback_channel(element, (snd_mixer_selem_channel_id_t)channel)) {
				return (snd_mixer_selem_channel_id_t)channel;
			}
		}
	}
	if (snd_mixer_selem_is_capture_mono(element)) {
		return SND_MIXER_SCHN_MONO;
	}
	else {
		for (int channel = 0; channel <= SND_MIXER_SCHN_LAST; channel++) {
			if (snd_mixer_selem_has_capture_channel(element, (snd_mixer_selem_channel_id_t)channel)) {
				return (snd_mixer_selem_channel_id_t)channel;
			}
		}
	}
	return SND_MIXER_SCHN_UNKNOWN;
}

void AlsaDevice::setDeviceVolume(double volume)
{
	if (!currentMixerName_.empty()) {
		snd_mixer_t *handle = getMixerHanlde(id_);
		snd_mixer_elem_t *element = initMixerElement(handle, currentMixerName_.c_str());
		long min, max, realVolume;
		if (snd_mixer_selem_has_playback_volume(element)) {
			checkError(snd_mixer_selem_get_playback_volume_range(element, &min, &max));
			realVolume = long(volume)*((max - min) / 100);
			checkError(snd_mixer_selem_set_playback_volume_all(element, realVolume));
		}
		else if (snd_mixer_selem_has_capture_volume(element)) {
			checkError(snd_mixer_selem_get_capture_volume_range(element, &min, &max));
			realVolume = long(volume)*((max - min) / 100);
			checkError(snd_mixer_selem_set_capture_volume_all(element, realVolume));
		}
		else {
			std::cerr << "Selected mixer has no playback or capture volume" << std::endl;
		}
		checkError(snd_mixer_close(handle));
	}
}

double AlsaDevice::getVolume()
{
	if (!currentMixerName_.empty()) {
		snd_mixer_t *handle = getMixerHanlde(id_);
		snd_mixer_elem_t *elem = initMixerElement(handle, currentMixerName_.c_str());
		long minv = 0L;
		long maxv = 0L;
		long outvol = 0L;
		double min, max, volume;
		snd_mixer_selem_channel_id_t chanelid = checkMixerChannels(elem);
		if (snd_mixer_selem_has_playback_volume(elem) || snd_mixer_selem_has_playback_volume_joined(elem)) {
			checkError(snd_mixer_selem_get_playback_volume_range(elem, &minv, &maxv));
			if (snd_mixer_selem_has_playback_channel(elem, chanelid)) {
				checkError(snd_mixer_selem_get_playback_volume(elem, chanelid, &outvol));
			}
			min = (double)minv;
			max = (double)maxv;
			volume = (double)outvol;
			if ((max - min) != 0) {
				double delta = 100/(max - min);
				volume = (volume - min)/delta;
				return volume;
			}
		}
		if (snd_mixer_selem_has_capture_volume(elem) || snd_mixer_selem_has_capture_volume_joined(elem)) {
			checkError(snd_mixer_selem_get_capture_volume_range(elem, &minv, &maxv));
			if (snd_mixer_selem_has_capture_channel(elem, chanelid)) {
				checkError(snd_mixer_selem_get_capture_volume(elem, chanelid, &outvol));
			}
			min = (double)minv;
			max = (double)maxv;
			volume = (double)outvol;
			if ((max - min) != 0) {
				double delta = 100/(max - min);
				volume = (volume - min)/delta;
				return volume;
			}
		}
		checkError(snd_mixer_close(handle));
	}
	return ZERO;
}

void AlsaDevice::setSwitch(const std::string &mixer, int id, bool enabled)
{
	snd_mixer_t *handle = getMixerHanlde(id_);
	snd_mixer_elem_t* elem = initMixerElement(handle, mixer.c_str());
	switch (id) {
	case PLAYBACK:
		checkError(snd_mixer_selem_set_playback_switch_all(elem, int(enabled)));
		break;
	case CAPTURE:
		checkError(snd_mixer_selem_set_capture_switch_all(elem, int(enabled)));
		break;
	case ENUM:
		snd_mixer_selem_channel_id_t channel = checkMixerChannels(elem);
		checkError(snd_mixer_selem_set_enum_item(elem, channel, uint(enabled)));
		break;
	}
	checkError(snd_mixer_close(handle));
}

void AlsaDevice::setMute(bool enabled)
{
	if (!currentMixerName_.empty()) {
		snd_mixer_t *handle = getMixerHanlde(id_);
		snd_mixer_elem_t* elem = initMixerElement(handle, currentMixerName_.c_str());
		if (snd_mixer_selem_has_playback_switch(elem)
		   || snd_mixer_selem_has_playback_switch_joined(elem)) {
			checkError(snd_mixer_selem_set_playback_switch_all(elem, int(enabled)));
		}
		if (snd_mixer_selem_has_capture_switch(elem)
		    || snd_mixer_selem_has_common_switch(elem)
		    || snd_mixer_selem_has_capture_switch_joined(elem)
		    || snd_mixer_selem_has_capture_switch_exclusive(elem)) {
			checkError(snd_mixer_selem_set_capture_switch_all(elem, int(enabled)));
		}
		checkError(snd_mixer_close(handle));
	}
}

bool AlsaDevice::getMute()
{
	if (!currentMixerName_.empty()) {
		snd_mixer_t *handle = getMixerHanlde(id_);
		snd_mixer_elem_t* elem = initMixerElement(handle, currentMixerName_.c_str());
		snd_mixer_selem_channel_id_t channel = checkMixerChannels(elem);
		if (snd_mixer_selem_has_playback_switch(elem)
		    || snd_mixer_selem_has_playback_switch_joined(elem)) {
			int value = 0;
			checkError(snd_mixer_selem_get_playback_switch(elem, channel, &value));
			return bool(value);
		}
		if (snd_mixer_selem_has_capture_switch(elem)
		    || snd_mixer_selem_has_common_switch(elem)
		    || snd_mixer_selem_has_capture_switch_joined(elem)
		    || snd_mixer_selem_has_capture_switch_exclusive(elem)) {
			int value = 0;
			checkError(snd_mixer_selem_get_capture_switch(elem, channel, &value));
			return bool(value);
		}
		checkError(snd_mixer_close(handle));
	}
	return true;
}

std::string AlsaDevice::formatCardName(int id) const
{
	size_t size = 64;
	char *name = (char*)malloc(size);
	sprintf(name, "hw:%d", id);
	return std::string(name);
}

void AlsaDevice::setCurrentMixer(int id)
{
	if(id >= 0 && id < (int)mixers_.size()) {
		currentMixerId_ = id;
		currentMixerName_ = mixers_.at(id);
	}
}

void AlsaDevice::setCurrentMixer(const std::string &mixer)
{
	std::pair<bool, int> exists = Tools::itemExists(mixers_, mixer);
	if(exists.first){
		currentMixerId_ = exists.second;
		currentMixerName_ = mixer;
	}
}

const std::string &AlsaDevice::name() const
{
	return name_;
}

int AlsaDevice::id()
{
	return id_;
}

const std::vector<std::string> &AlsaDevice::mixers() const
{
	return mixers_;
}

void AlsaDevice::checkError (int errorIndex)
{
	if (errorIndex < 0) {
		std::cerr << snd_strerror(errorIndex) << std::endl;
	}
}

bool AlsaDevice::havePlaybackMixers()
{
	return !volumeMixers_.empty();
}

bool AlsaDevice::haveCaptureMixers()
{
	return !captureMixers_.empty();
}

bool AlsaDevice::haveMixers()
{
	return !mixers_.empty();
}

const MixerSwitches &AlsaDevice::switches() const
{
	return *switches_;
}

int AlsaDevice::currentMixerId()
{
	return currentMixerId_;
}

const std::string &AlsaDevice::currentMixer() const
{
	return currentMixerName_;
}
