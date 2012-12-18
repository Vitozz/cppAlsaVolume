#include "alsawork.h"

AlsaWork::AlsaWork()
{
	getSndCardCtlName();
}

AlsaWork::~AlsaWork()
{
	snd_hctl_close(alsaControls_->hctl);
	snd_ctl_close(alsaControls_->ctl);
	snd_config_update_free_global();
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
	updateControls();
	snd_mixer_t *handle;
	checkError(snd_mixer_open(&handle, 0));
	checkError(snd_mixer_attach_hctl(handle, alsaControls_->hctl));
	checkError(snd_mixer_selem_register(handle, NULL, NULL));
	checkError(snd_mixer_load(handle));
	return handle;
}

void AlsaWork::getSndCardHCtl()
{
	checkError(snd_hctl_open_ctl(&alsaControls_->hctl, alsaControls_->ctl));
}

void AlsaWork::getSndCardCtl(const char *cardCtlName)
{
	checkError(snd_ctl_open(&alsaControls_->ctl, cardCtlName, 0));
}

void AlsaWork::updateControls()
{
	getSndCardCtl(alsaControls_->name);
	getSndCardHCtl();
}

void AlsaWork::getSndCardCtlName()
{
	if (getTotalCards() >= 1) {
		formatCardName(0);
	}
	else {
		std::cerr << "alsawork.cpp::86 No sound cards found" << std::endl;
		exit(1);
	}
}

void AlsaWork::formatCardName(int id)
{
	size_t size = 64;
	char *name = (char*)malloc(size);
	alsaControls_->name = (char*)malloc(size+1);
	sprintf(name, "hw:%d", id);
	strcpy(alsaControls_->name, name);
	free (name);
}

int AlsaWork::getTotalCards()
{
	int cards = 0;
	int index = -1;
	for (;;) {
		checkError(snd_card_next(&index));
		if (index < 0) {
			break;
		}
		++cards;
	}
	return cards;
}

std::string AlsaWork::getCardName()
{
	updateControls();
	snd_ctl_card_info_t *cardInfo;
	snd_ctl_card_info_alloca(&cardInfo);
	checkError(snd_ctl_card_info(alsaControls_->ctl, cardInfo));
	const char *cardName = snd_ctl_card_info_get_name(cardInfo);
	return std::string(cardName);
}
