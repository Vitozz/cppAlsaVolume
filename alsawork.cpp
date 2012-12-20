#include "alsawork.h"
#include <stdexcept>

AlsaWork::AlsaWork()
{
	cardId_=0;
}

AlsaWork::~AlsaWork()
{
	snd_config_update_free_global();

}

void AlsaWork::checkError (int errorIndex)
{
	if (errorIndex < 0) {
		std::cerr << snd_strerror(errorIndex) << std::endl;
	}
}

void AlsaWork::setAlsaVolume(std::string mixer, double volume)
{
	snd_mixer_t *handle = getMixerHanlde(cardId_);
	snd_mixer_elem_t *element = initMixerElement(handle, mixer.c_str());
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
		checkError(snd_mixer_selem_set_playback_volume_all(element, volume_ * max / 100));
	}
	else{
		std::cerr << "Selected mixer has no playback volume" << std::endl;
	}
	checkError(snd_mixer_close(handle));
}

double AlsaWork::getAlsaVolume(std::string mixer)
{
	snd_mixer_t *handle = getMixerHanlde(cardId_);
	snd_mixer_elem_t *elem = initMixerElement(handle, mixer.c_str());
	if (snd_mixer_selem_has_playback_volume(elem) == 1) {
		long minv, maxv;
		long outvol;
		snd_mixer_selem_get_playback_volume_range (elem, &minv, &maxv);
		snd_mixer_selem_channel_id_t chanelid = SND_MIXER_SCHN_MONO;
		if (snd_mixer_selem_has_playback_channel(elem, chanelid) == 1) {
			checkError(snd_mixer_selem_get_playback_volume(elem, chanelid, &outvol));
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

snd_mixer_t *AlsaWork::getMixerHanlde(int id)
{
	std::string card(formatCardName(id));
	snd_ctl_t *ctl;
	checkError(snd_ctl_open(&ctl, card.c_str(), 0));
	snd_hctl_t *hctl;
	checkError(snd_hctl_open_ctl(&hctl, ctl));
	snd_mixer_t *handle;
	checkError(snd_mixer_open(&handle, 0));
	checkError(snd_mixer_attach_hctl(handle, hctl));
	checkError(snd_mixer_selem_register(handle, NULL, NULL));
	checkError(snd_mixer_load(handle));
	return handle;
}

std::string AlsaWork::formatCardName(int id)
{
	cardId_ = id;
	size_t size = 64;
	char *name = (char*)malloc(size);
	sprintf(name, "hw:%d", id);
	return std::string(name);
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
	std::string card(formatCardName(index));
	snd_ctl_t *ctl;
	checkError(snd_ctl_open(&ctl, card.c_str(), 0));
	snd_ctl_card_info_t *cardInfo;
	snd_ctl_card_info_alloca(&cardInfo);
	checkError(snd_ctl_card_info(ctl, cardInfo));
	const char *cardName = snd_ctl_card_info_get_name(cardInfo);
	return std::string(cardName);
}

void AlsaWork::getMixers(int cardIndex)
{
	if (!mixerList_.empty())
		mixerList_.clear();
	snd_mixer_t *handle = getMixerHanlde(cardIndex);
	snd_mixer_selem_id_t *smid;
	snd_mixer_selem_id_alloca(&smid);
	for (snd_mixer_elem_t *element = snd_mixer_first_elem(handle);
	     element;
	     element = snd_mixer_elem_next(element)) {
		snd_mixer_selem_get_id(element, smid);
		if (snd_mixer_selem_has_playback_volume(element) == 1) {
			std::string name(snd_mixer_selem_id_get_name(smid));
			mixerList_.push_back(name);
		}
	}
	snd_mixer_close(handle);
}

void AlsaWork::getCards()
{
	if (!cardList_.empty()) {
		cardList_.clear();
	}
	int total = getTotalCards();
	if (total >= 1) {
		for (int card = 0; card < total; card++) {
			std::string cname(getCardName(card));
			cardList_.push_back(cname);
		}
	}
}

std::vector<std::string> AlsaWork::getCardsList()
{
	getCards();
	return cardList_;
}

std::vector<std::string> AlsaWork::getMixersList(int cardIndex)
{
	getMixers(cardIndex);
	return mixerList_;
}

void AlsaWork::setCardId(int cardId)
{
	try {
		if (!cardList_.at(cardId).empty()) {
			cardId_ = cardId;
		}
	}
	catch (std::out_of_range &ex) {
		std::cerr << "alsawork.cpp::173:: Item out of Range" << ex.what() << std::endl;
	}
}
