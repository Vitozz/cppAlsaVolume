#ifndef ALSAWORK_H
#define ALSAWORK_H

#include "alsa/asoundlib.h"
#include <iostream>
#include <vector>

class AlsaWork
{
public:
	AlsaWork();
	~AlsaWork();
	void setAlsaVolume(std::string mixer, double volume);
	double getAlsaVolume(std::string mixer);
	std::string getCardName(int index);
	std::vector<std::string> getCardsList();
	std::vector<std::string> getMixersList(int cardIndex);

private:
	int getTotalCards();
	snd_mixer_t *getMixerHanlde(int id);
	snd_mixer_elem_t *initMixerElement(snd_mixer_t *handle, const char *mixer);
	void setVolume(snd_mixer_elem_t *element, snd_mixer_t *handle, double volume);
	std::string formatCardName(int id);
	void checkError (int errorIndex);
	std::string getMixerName(int index);
	void getCards();
	void getMixers(int cardIndex);
private:
	std::vector<std::string> cardList_;
	std::vector<std::string> mixerList_;
	int cardId_;
};
#endif // ALSAWORK_H
