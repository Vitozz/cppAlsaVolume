#ifndef ALSADEVICE_H
#define ALSADEVICE_H

#include "alsa/asoundlib.h"
#include "../tools/tools.h"
#include "mixerswitches.h"
#include "volumemixers.h"
#include <iostream>
#include <vector>

class AlsaDevice
{
public:
	AlsaDevice(int id, const std::string &card);
	~AlsaDevice();
	const std::string &name() const;
	int id();
	const std::vector<std::string> &mixers() const;
	MixerSwitches switches();
	const std::string &currentMixer() const;
	int currentMixerId();
	bool havePlaybackMixers();
	bool haveCaptureMixers();
	bool haveMixers();
	double getVolume();
	bool getMute();
	void setDeviceVolume(double volume);
	void setCurrentMixer(int id);
	void setCurrentMixer(const std::string &mixer);
	void setSwitch(const std::string &mixer, int id, bool enabled);
	void setMute(bool enabled);

private:
	snd_mixer_t *getMixerHanlde(int id);
	std::string formatCardName(int id);
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
