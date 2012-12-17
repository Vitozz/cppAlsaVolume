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
	char *getSndCardCtlName();
	snd_hctl_t *getSndCardHCtl(snd_ctl_t *ctl);
	snd_ctl_t *getSndCardCtl(const char *cardCtlName);
	snd_mixer_t *getMixerHanlde();
	void onError (int errorIndex);
};
#endif // ALSAWORK_H
