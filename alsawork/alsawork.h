/*
 * alsawork.h
 * Copyright (C) 2012-2015 Vitaly Tonkacheyev
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef ALSAWORK_H
#define ALSAWORK_H

#include "alsa/asoundlib.h"
#include "../tools/tools.h"
#include "mixerswitches.h"
#include "alsadevice.h"
#include <iostream>
#include <vector>
#include <memory>

typedef std::vector<AlsaDevice::Ptr> AlsaDevicePtrList;

class AlsaWork
{
public:
	AlsaWork();
	AlsaWork(AlsaWork const &);
	~AlsaWork();
	typedef std::shared_ptr<AlsaWork> Ptr;
	void setCurrentCard(int cardId);
	void setCurrentMixer(const std::string &mixer);
	void setCurrentMixer(int id);
	void setAlsaVolume(double volume);
	double getAlsaVolume() const;
	const std::string getCardName(int index);
	const std::string getMixerName(int index);
	std::string getCurrentMixerName() const;
	const std::vector<std::string> &getCardsList() const;
	const std::vector<std::string> &getVolumeMixers() const;
	MixerSwitches::Ptr getSwitchList() const;
	void setSwitch(const std::string& mixer, int id, bool enabled);
	void setMute(bool enabled);
	bool getMute();
	bool haveVolumeMixers();
	bool cardExists(int id);
	bool mixerExists(const std::string &name);
	bool mixerExists(int id);
	int getFirstCardWithMixers();
	int getCurrentMixerId() const;

private:
	bool checkCardId(int cardId);
	int getTotalCards();
	snd_mixer_t *getMixerHanlde(int id);
	snd_mixer_elem_t *initMixerElement(snd_mixer_t *handle, const char *mixer);
	void setVolume(snd_mixer_elem_t *element, snd_mixer_t *handle, double volume);
	void checkError (int errorIndex);
	void getCards();
	void updateMixers(int cardIndex);
	void updateMixerList(int cardIndex);
	snd_mixer_selem_channel_id_t checkMixerChannels(snd_mixer_elem_t *element);

private:
	std::vector<std::string> cardList_;
	int totalCards_;
	AlsaDevice::Ptr currentAlsaDevice_;
	AlsaDevicePtrList devices_;
};
#endif // ALSAWORK_H
