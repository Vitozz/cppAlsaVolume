#ifndef ALSAWORK_H
#define ALSAWORK_H

#include "alsa/asoundlib.h"
#include "tools.h"
#include <iostream>
#include <vector>

class AlsaWork
{
public:
	AlsaWork();
	~AlsaWork();
	void setAlsaVolume(const std::string& mixer, double volume);
	double getAlsaVolume(const std::string& mixer);
	std::string getCardName(int index);
	std::vector<std::string> getCardsList();
	std::vector<std::string>  getVolumeMixers(int cardIndex);
	MixerSwitches getSwitchList(int cardIndex);
	void setCardId(int cardId);
	void setSwitch(int cardId, const std::string& mixer, int id, bool enabled);
	void setMute(int cardId, const std::string& mixer, bool enabled);
	bool getMute(int cardId, const std::string& mixer);
private:
	int getTotalCards();
	snd_mixer_t *getMixerHanlde(int id);
	snd_mixer_elem_t *initMixerElement(snd_mixer_t *handle, const char *mixer);
	void setVolume(snd_mixer_elem_t *element, snd_mixer_t *handle, double volume);
	std::string formatCardName(int id);
	void checkError (int errorIndex);
	void getCards();
	void updateMixers(int cardIndex);
	snd_mixer_selem_channel_id_t checkMixerChannels(snd_mixer_elem_t *element);
private:
	std::vector<std::string> cardList_;
	std::vector<std::string> mixerList_;
	MixerSwitches switches_;
	VolumeMixers volumeMixers_;
	int cardId_;
};
#endif // ALSAWORK_H
