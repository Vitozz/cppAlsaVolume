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

//public
void AlsaWork::setAlsaVolume(const std::string &mixer, double volume)
{
	snd_mixer_t *handle = getMixerHanlde(cardId_);
	snd_mixer_elem_t *element = initMixerElement(handle, mixer.c_str());
	setVolume(element, handle, volume);
}

double AlsaWork::getAlsaVolume(const std::string& mixer)
{
	snd_mixer_t *handle = getMixerHanlde(cardId_);
	snd_mixer_elem_t *elem = initMixerElement(handle, mixer.c_str());
	if (snd_mixer_selem_has_playback_volume(elem)) {
		long minv, maxv, outvol;
		checkError(snd_mixer_selem_get_playback_volume_range (elem, &minv, &maxv));
		snd_mixer_selem_channel_id_t chanelid = checkMixerChannels(elem);
		if (snd_mixer_selem_has_playback_channel(elem, chanelid)) {
			checkError(snd_mixer_selem_get_playback_volume(elem, chanelid, &outvol));
		}
		outvol -= minv;
		maxv -= minv;
		minv = 0;
		outvol = 100 * outvol / maxv;
		return double(outvol);
	}
	checkError(snd_mixer_close(handle));
	return 0.0;
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

std::vector<std::string> AlsaWork::getCardsList()
{
	getCards();
	return cardList_;
}

std::vector<std::string> AlsaWork::getVolumeMixers(int cardIndex)
{
	updateMixers(cardIndex);
	if (!mixerList_.empty())
		mixerList_.clear();
	mixerList_.reserve(volumeMixers_.playback.size() + volumeMixers_.capture.size());
	mixerList_.insert(mixerList_.end(), volumeMixers_.playback.begin(), volumeMixers_.playback.end());
	mixerList_.insert(mixerList_.end(), volumeMixers_.capture.begin(), volumeMixers_.capture.end());
	return mixerList_;
}

MixerSwitches AlsaWork::getSwitchList(int cardIndex)
{
	updateMixers(cardIndex);
	return switches_;
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

void AlsaWork::setSwitch(int cardId, const std::string &mixer, int id, bool enabled)
{
	snd_mixer_t *handle = getMixerHanlde(cardId);
	snd_mixer_elem_t* elem = initMixerElement(handle, mixer.c_str());
	if (id == PLAYBACK || (id == PLAYBACK_JOINED)) {
		checkError(snd_mixer_selem_set_playback_switch_all(elem, int(enabled)));
	}
	if (id == CAPTURE || (id == CAPTURE_JOINED) || (id == CAPTURE_JOINED)) {
		checkError(snd_mixer_selem_set_capture_switch_all(elem, int(enabled)));
	}
	snd_mixer_close(handle);
}

void AlsaWork::setMute(int cardId, const std::string &mixer, bool enabled)
{
	snd_mixer_t *handle = getMixerHanlde(cardId);
	snd_mixer_elem_t* elem = initMixerElement(handle, mixer.c_str());
	if (snd_mixer_selem_has_playback_switch(elem) || snd_mixer_selem_has_playback_switch_joined(elem)) {
		checkError(snd_mixer_selem_set_playback_switch_all(elem, int(enabled)));
	}
	if (snd_mixer_selem_has_capture_switch(elem)
	    || snd_mixer_selem_has_capture_switch_joined(elem)
	    || snd_mixer_selem_has_capture_switch_exclusive(elem)) {
		checkError(snd_mixer_selem_set_capture_switch_all(elem, int(enabled)));
	}
	snd_mixer_close(handle);
}

bool AlsaWork::getMute(int cardId, const std::string &mixer)
{
	snd_mixer_t *handle = getMixerHanlde(cardId);
	snd_mixer_elem_t* elem = initMixerElement(handle, mixer.c_str());
	snd_mixer_selem_channel_id_t channel = checkMixerChannels(elem);

	if (snd_mixer_selem_has_playback_switch(elem) || snd_mixer_selem_has_playback_switch_joined(elem)) {
		int value = 0;
		checkError(snd_mixer_selem_get_playback_switch(elem, channel, &value));
		return bool(value);
	}
	if (snd_mixer_selem_has_capture_switch(elem)
	    || snd_mixer_selem_has_capture_switch_joined(elem)
	    || snd_mixer_selem_has_capture_switch_exclusive(elem)) {
		int value = 0;
		checkError(snd_mixer_selem_get_capture_switch(elem, channel, &value));
		return bool(value);
	}
	snd_mixer_close(handle);
	return true;
}
//private
void AlsaWork::checkError (int errorIndex)
{
	if (errorIndex < 0) {
		std::cerr << snd_strerror(errorIndex) << std::endl;
	}
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
	if (snd_mixer_selem_has_playback_volume(element)) {
		long min, max;
		checkError(snd_mixer_selem_get_playback_volume_range(element, &min, &max));
		long volume_ = long(volume);
		checkError(snd_mixer_selem_set_playback_volume_all(element, volume_ * max / 100));
	}
	else if (snd_mixer_selem_has_capture_volume(element)) {
		long min, max;
		checkError(snd_mixer_selem_get_capture_volume_range(element, &min, &max));
		long volume_ = long(volume);
		checkError(snd_mixer_selem_set_capture_volume_all(element, volume_ * max /100));
	}
	else{
		std::cerr << "Selected mixer has no playback or capture volume" << std::endl;
	}
	checkError(snd_mixer_close(handle));
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
	do {
		checkError(snd_card_next(&index));
		++cards;
	} while (index > 0);
	return cards;
}

void AlsaWork::updateMixers(int cardIndex)
{
	//clearing lists
	if (!volumeMixers_.playback.empty())
		volumeMixers_.playback.clear();
	if (!volumeMixers_.capture.empty())
		volumeMixers_.capture.clear();
	if (!switches_.captureSwitchList_.empty())
		switches_.captureSwitchList_.clear();
	if (!switches_.playbackSwitchList_.empty())
		switches_.playbackSwitchList_.clear();
	if (!switches_.playbackJoinedSwitchList_.empty())
		switches_.playbackJoinedSwitchList_.clear();
	if (!switches_.captureJoinedSwitchList_.empty())
		switches_.captureJoinedSwitchList_.clear();
	if (!switches_.captureExsclusiveSwitchList_.empty())
		switches_.captureExsclusiveSwitchList_.clear();
	//
	snd_mixer_t *handle = getMixerHanlde(cardIndex);
	snd_mixer_selem_id_t *smid;
	snd_mixer_selem_id_alloca(&smid);
	for (snd_mixer_elem_t *element = snd_mixer_first_elem(handle);
	     element;
	     element = snd_mixer_elem_next(element)) {
		snd_mixer_selem_get_id(element, smid);
		std::string name(snd_mixer_selem_id_get_name(smid));
		snd_mixer_selem_channel_id_t channel = checkMixerChannels(element);

		if (snd_mixer_selem_has_playback_volume(element) || snd_mixer_selem_has_playback_volume_joined(element)) {
			volumeMixers_.playback.push_back(name);
		}
		if (snd_mixer_selem_has_capture_volume(element) || snd_mixer_selem_has_capture_volume_joined(element)) {
			volumeMixers_.capture.push_back(name);
		}
		if (snd_mixer_selem_has_capture_switch(element)){
			int value = 0;
			switchcap item;
			checkError(snd_mixer_selem_get_capture_switch(element, channel, &value));
			item.enabled = bool(value);
			item.name = name;
			switches_.captureSwitchList_.push_back(item);
		}
		if (snd_mixer_selem_has_capture_switch_joined(element)){
			int value = 0;
			switchcap item;
			checkError(snd_mixer_selem_get_capture_switch(element, channel, &value));
			item.enabled = bool(value);
			item.name = name;
			switches_.captureJoinedSwitchList_.push_back(item);
		}
		if (snd_mixer_selem_has_capture_switch_exclusive(element)){
			int value = 0;
			switchcap item;
			checkError(snd_mixer_selem_get_capture_switch(element, channel, &value));
			item.enabled = bool(value);
			item.name = name;
			switches_.captureExsclusiveSwitchList_.push_back(item);
		}
		if (snd_mixer_selem_has_playback_switch(element)){
			int value = 0;
			switchcap item;
			checkError(snd_mixer_selem_get_playback_switch(element, channel, &value));
			item.enabled = bool(value);
			item.name = name;
			switches_.playbackSwitchList_.push_back(item);
		}
		if (snd_mixer_selem_has_playback_switch_joined(element)){
			int value = 0;
			switchcap item;
			checkError(snd_mixer_selem_get_playback_switch(element, channel, &value));
			item.enabled = bool(value);
			item.name = name;
			switches_.playbackJoinedSwitchList_.push_back(item);
		}
	}
	snd_mixer_close(handle);
}

void AlsaWork::getCards()
{
	if (!cardList_.empty()) cardList_.clear();
	int total = getTotalCards();
	if (total >= 1) {
		for (int card = 0; card < total; card++) {
			std::string cname(getCardName(card));
			cardList_.push_back(cname);
		}
	}
}

snd_mixer_selem_channel_id_t AlsaWork::checkMixerChannels(snd_mixer_elem_t *element)
{
	if (snd_mixer_selem_is_playback_mono(element)) {
		return SND_MIXER_SCHN_MONO;
	}
	else {
		for (int channel = 0; channel <= SND_MIXER_SCHN_LAST; channel++) {
			if (snd_mixer_selem_has_playback_channel(element, (snd_mixer_selem_channel_id_t)channel)) {
				return (snd_mixer_selem_channel_id_t)channel;
			}
		}
	}
	if (snd_mixer_selem_is_capture_mono(element)) {
		return SND_MIXER_SCHN_MONO;
	}
	else {
		for (int channel = 0; channel <= SND_MIXER_SCHN_LAST; channel++) {
			if (snd_mixer_selem_has_capture_channel(element, (snd_mixer_selem_channel_id_t)channel)) {
				return (snd_mixer_selem_channel_id_t)channel;
			}
		}
	}
	return SND_MIXER_SCHN_UNKNOWN;
}
