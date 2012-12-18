#ifndef ALSAWORK_H
#define ALSAWORK_H

#include "alsa/asoundlib.h"
#include <iostream>

struct AlsaControls {
	char *name;
	snd_ctl_t *ctl;
	snd_hctl_t *hctl;
};

class AlsaWork
{
public:
	AlsaWork();
	~AlsaWork();
	void setAlsaVolume(const char *mixer, double volume);
	double getAlsaVolume();
	std::string getCardName();

private:
	void getSndCardCtlName();
	void getSndCardHCtl();
	void getSndCardCtl(const char *cardCtlName);
	void updateControls();
	int getTotalCards();
	snd_mixer_t *getMixerHanlde();
	snd_mixer_elem_t *initMixerElement(snd_mixer_t *handle, const char *mixer);
	void setVolume(snd_mixer_elem_t *element, snd_mixer_t *handle, double volume);
	void formatCardName(int id);
	void checkError (int errorIndex);
private:
	AlsaControls *alsaControls_;
	std::string cardName_;
};
#endif // ALSAWORK_H
