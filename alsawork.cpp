#include "alsawork.h"

AlsaWork::AlsaWork()
{
	getCards();
	getMixers();
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
	if (snd_mixer_selem_has_playback_volume(element) == 1) {
		long min, max;
		checkError(snd_mixer_selem_get_playback_volume_range(element, &min, &max));
		long volume_ = (long)volume;
		std::cout << "min= "<< min <<" max = " << max << std::endl;
		checkError(snd_mixer_selem_set_playback_volume_all(element, volume_ * max / 100));
	}
	else{
		std::cerr << "Selected mixer has no playback volume" << std::endl;
	}
	checkError(snd_mixer_close(handle));
}

double AlsaWork::getAlsaVolume(const char *mixer)
{
	snd_mixer_t *handle = getMixerHanlde();
	snd_mixer_elem_t *elem = initMixerElement(handle, mixer);
	if (snd_mixer_selem_has_playback_volume(elem) == 1) {
		long minv, maxv;
		long outvol;
		snd_mixer_selem_get_playback_volume_range (elem, &minv, &maxv);
		snd_mixer_selem_channel_id_t chanelid = SND_MIXER_SCHN_FRONT_CENTER;
		std::cout << snd_mixer_selem_channel_name(chanelid) << std::endl;
		if (snd_mixer_selem_has_playback_channel(elem, chanelid) == 1) {
			checkError(snd_mixer_selem_get_playback_volume(elem, chanelid, &outvol));
		}
		else {
			checkError(snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_MONO, &outvol));
		}
		outvol -= minv;
		maxv -= minv;
		minv = 0;
		outvol = 100 * outvol / maxv;
		return (double)outvol;
	}
	checkError(snd_mixer_close(handle));
	return 0.0;
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

std::string AlsaWork::getCardName(int index)
{
	updateControls();
	snd_ctl_t *ctl;
	formatCardName(index);
	checkError(snd_ctl_open(&ctl, alsaControls_->name, 0));
	snd_ctl_card_info_t *cardInfo;
	snd_ctl_card_info_alloca(&cardInfo);
	checkError(snd_ctl_card_info(ctl, cardInfo));
	const char *cardName = snd_ctl_card_info_get_name(cardInfo);
	snd_ctl_close(ctl);
	return std::string(cardName);
}

void AlsaWork::getMixers()
{
	snd_mixer_t *handle = getMixerHanlde();
	snd_mixer_selem_id_t *smid;
	snd_mixer_selem_id_alloca(&smid);
	for (snd_mixer_elem_t *element = snd_mixer_first_elem(handle);
	     element;
	     element = snd_mixer_elem_next(element)) {
		snd_mixer_selem_get_id(element, smid);
		mixerList_.push_back(std::string(snd_mixer_selem_id_get_name(smid)));
	}
	snd_mixer_close(handle);
}

void AlsaWork::getCards()
{
	int total = getTotalCards();
	if (total >= 1) {
		for (int card = 0; card < total; card++) {
			cardList_.push_back(getCardName(card));
		}
	}
}

std::list<std::string> AlsaWork::getCardsMap()
{

	return cardList_;
}

std::list<std::string> AlsaWork::getMixersList()
{
	return mixerList_;
}
