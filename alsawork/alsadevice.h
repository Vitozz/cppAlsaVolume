/*
 * alsadevice.h
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
#ifndef ALSADEVICE_H
#define ALSADEVICE_H

#include "alsa/asoundlib.h"
#include "../tools/tools.h"
#include "mixerswitches.h"
//#include "volumemixers.h"
#include <iostream>
#include <vector>

class AlsaDevice
{
public:
	AlsaDevice(int id, const std::string &card);
	~AlsaDevice();
	AlsaDevice(AlsaDevice const &);
	const std::string &name() const;
	int id() const;
	const std::vector<std::string> &mixers() const;
	const MixerSwitches &switches() const;
	const std::string &currentMixer() const;
	int currentMixerId() const;
	bool havePlaybackMixers();
	bool haveCaptureMixers();
	bool haveMixers();
	const double getVolume();
	bool getMute();
	void setDeviceVolume(double volume);
	void setCurrentMixer(int id);
	void setCurrentMixer(const std::string &mixer);
	void setSwitch(const std::string &mixer, int id, bool enabled);
	void setMute(bool enabled);

private:
	snd_mixer_t *getMixerHanlde(int id);
	std::string formatCardName(int id) const;
	snd_mixer_selem_channel_id_t checkMixerChannels(snd_mixer_elem_t *element);
	snd_mixer_elem_t *initMixerElement(snd_mixer_t *handle, const char *mixer);
	void checkError (int errorIndex);
	void initMixerList();

private:
	int id_;
	std::string name_;
	std::vector<std::string> volumeMixers_;
	std::vector<std::string> captureMixers_;
	std::vector<std::string> mixers_;
	MixerSwitches *switches_;
	int currentMixerId_;
	std::string currentMixerName_;

};

#endif // ALSADEVICE_H
