#ifndef ALSAWORK_H
#define ALSAWORK_H

#include "alsa/asoundlib.h"
#include <iostream>

class AlsaWork
{
public:
	AlsaWork();
	void setAlsaVolume(const char *mixer, double volume);
	double getAlsaVolume();

private:
	void getSndCardCtlName();
	snd_hctl_t *getSndCardHCtl(snd_ctl_t *ctl);
	snd_ctl_t *getSndCardCtl(const std::string &cardCtlName);
	snd_mixer_t *getMixerHanlde();
	snd_mixer_elem_t *initMixerElement(snd_mixer_t *handle, const char *mixer);
	void setVolume(snd_mixer_elem_t *element, snd_mixer_t *handle, double volume);
	void checkError (int errorIndex);
private:
	std::string cardName_;
};
#endif // ALSAWORK_H
