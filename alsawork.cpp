#include "alsawork.h"
#include <sstream>

AlsaWork::AlsaWork()
{
	getSndCardCtlName();
	getSndCardHCtl(getSndCardCtl(cardName_));
}

void AlsaWork::checkError (int errorIndex)
{
	if (errorIndex < 0) {
		std::cerr << snd_strerror(errorIndex) << std::endl;
	}
}

void AlsaWork::setAlsaVolume(const char *mixer, double volume)
{
	snd_mixer_t *handle = getMixerHanlde();
	snd_mixer_elem_t *element = initMixerElement(handle, mixer);
	setVolume(element, handle, volume);
}

snd_mixer_elem_t *AlsaWork::initMixerElement(snd_mixer_t *handle, const char *mixer)
{
	snd_mixer_selem_id_t *smid;
	snd_mixer_selem_id_alloca(&smid);
	snd_mixer_selem_id_set_index(smid, 0);
	snd_mixer_selem_id_set_name(smid, mixer);
	snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, smid);
	return elem;
}

void AlsaWork::setVolume(snd_mixer_elem_t *element, snd_mixer_t *handle, double volume)
{
	long min, max;
	checkError(snd_mixer_selem_get_playback_volume_range(element, &min, &max));
	long volume_ = (long)volume;
	checkError(snd_mixer_selem_set_playback_volume_all(element, volume_ * max / 100));
	checkError(snd_mixer_close(handle));
}

double AlsaWork::getAlsaVolume()
{
	return 0;
}

snd_mixer_t *AlsaWork::getMixerHanlde()
{
	snd_hctl_t *hctl = getSndCardHCtl(getSndCardCtl(cardName_));
	snd_mixer_t *handle;
	checkError(snd_mixer_open(&handle, 0));
	checkError(snd_mixer_attach_hctl(handle, hctl));
	checkError(snd_mixer_selem_register(handle, NULL, NULL));
	checkError(snd_mixer_load(handle));
	return handle;
}

snd_hctl_t *AlsaWork::getSndCardHCtl(snd_ctl_t *ctl)
{
	snd_hctl_t *hctl;
	checkError(snd_hctl_open_ctl(&hctl, ctl));
	return hctl;
}

snd_ctl_t *AlsaWork::getSndCardCtl(const std::string &cardCtlName)
{
	snd_ctl_t *ctl;
	checkError(snd_ctl_open(&ctl, cardCtlName.c_str(), 0));
	return ctl;
}

void AlsaWork::getSndCardCtlName()
{
	int cardIndex = -1;
	checkError(snd_card_next(&cardIndex));
	std::stringstream ss;
	ss << "hw:" << cardIndex;
	cardName_ = ss.str();
}
