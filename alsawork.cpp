#include "alsawork.h"

AlsaWork::AlsaWork()
{
}

void AlsaWork::onError (int errorIndex)
{
	if (errorIndex < 0) {
		std::cerr << snd_strerror(errorIndex) << std::endl;
	}
}

void AlsaWork::setAlsaVolume(const char *mixer, double volume)
{
	//initialize mixer
	snd_mixer_t *handle = getMixerHanlde();
	snd_mixer_selem_id_t *smid;
	snd_mixer_selem_id_alloca(&smid);
	snd_mixer_selem_id_set_index(smid, 0);
	snd_mixer_selem_id_set_name(smid, mixer);
	snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, smid);
	//
	long min, max;
	onError(snd_mixer_selem_get_playback_volume_range(elem, &min, &max));
	long volume_ = (long)volume;
	onError(snd_mixer_selem_set_playback_volume_all(elem, volume_ * max / 100));
	onError(snd_mixer_close(handle));
}

double AlsaWork::getAlsaVolume()
{
	return 0;
}

snd_mixer_t *AlsaWork::getMixerHanlde()
{
	std::cout << "Obtaining mixer handle" << std::endl;
	snd_ctl_t *ctl = getSndCardCtl(getSndCardCtlName());
	snd_hctl_t *hctl = getSndCardHCtl(ctl);
	snd_mixer_t *handle;
	int err = snd_mixer_open(&handle, 0);
	onError(err);
	err = snd_mixer_attach_hctl(handle, hctl);
	onError(err);
	err = snd_mixer_selem_register(handle, NULL, NULL);
	onError(err);
	err = snd_mixer_load(handle);
	onError(err);
	return handle;
}

snd_hctl_t *AlsaWork::getSndCardHCtl(snd_ctl_t *ctl)
{
	snd_hctl_t *hctl;
	std::cout << "Obtaining snd_hctl" << std::endl;
	onError(snd_hctl_open_ctl(&hctl, ctl));
	return hctl;
}

snd_ctl_t *AlsaWork::getSndCardCtl(const char *cardCtlName)
{
	snd_ctl_t *ctl;
	std::cout << "Obtaining snd_ctl " << cardCtlName << std::endl;
	onError(snd_ctl_open(&ctl, cardCtlName, 0));
	return ctl;
}

char *AlsaWork::getSndCardCtlName()
{
	std::cout << "Obtaining snd card index" << std::endl;
	int cardIndex = -1;
	onError(snd_card_next(&cardIndex));
	size_t size = 64;
	char ctlName[size];
	sprintf(ctlName, "hw:%i", cardIndex);
	char *cardCtlName = (char*)malloc(size+1);
	strcpy(cardCtlName, ctlName);
	std::cout << cardCtlName << std::endl;
	return cardCtlName;
}
