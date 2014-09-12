/*
 * alsawork.cpp
 * Copyright (C) 2012 Vitaly Tonkacheyev
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "mixerswitches.h"
#include "alsawork.h"
#include <stdexcept>

AlsaWork::AlsaWork()
: cardList_(std::vector<std::string>())
{
	getCards();
	int id = 0;
	std::vector<std::string>::iterator it = cardList_.begin();
	while (it != cardList_.end()) {
		std::string name = *it;
		devices_.push_back(new AlsaDevice(id, name));
		++it;
		++id;
	}
	setCurrentCard(0);
}

AlsaWork::~AlsaWork()
{
	if(!devices_.empty())
		devices_.clear();
	delete currentAlsaDevice_;
	snd_config_update_free_global();
}

//public
void AlsaWork::setCurrentCard(int cardId)
{
	if(cardId < (int)devices_.size()) {
		currentAlsaDevice_ = devices_.at(cardId);
	}
}

void AlsaWork::setCurrentMixer(const std::string &mixer)
{
	currentAlsaDevice_->setCurrentMixer(mixer);
}

void AlsaWork::setCurrentMixer(int id)
{
	currentAlsaDevice_->setCurrentMixer(id);
}

void AlsaWork::setAlsaVolume(double volume)
{
	currentAlsaDevice_->setDeviceVolume(volume);
}

double AlsaWork::getAlsaVolume()
{
	return currentAlsaDevice_->getVolume();
}

std::string AlsaWork::getCardName(int index)
{
	std::string card(formatCardName(index));
	snd_ctl_t *ctl;
	checkError(snd_ctl_open(&ctl, card.c_str(), SND_CTL_NONBLOCK));
	snd_ctl_card_info_t *cardInfo;
	snd_ctl_card_info_alloca(&cardInfo);
	checkError(snd_ctl_card_info(ctl, cardInfo));
	const char *cardName = snd_ctl_card_info_get_name(cardInfo);
	return std::string(cardName);
}

std::string AlsaWork::getCurrentMixerName() const
{
	return currentAlsaDevice_->currentMixer();
}

std::vector<std::string> &AlsaWork::getCardsList()
{
	getCards();
	return cardList_;
}

std::vector<std::string> AlsaWork::getVolumeMixers()
{
	return currentAlsaDevice_->mixers();
}

MixerSwitches AlsaWork::getSwitchList()
{
	return currentAlsaDevice_->switches();
}

void AlsaWork::setMute(bool enabled)
{
	currentAlsaDevice_->setMute(enabled);
}

bool AlsaWork::getMute()
{
	return currentAlsaDevice_->getMute();
}

void AlsaWork::setSwitch(const std::string &mixer, int id, bool enabled)
{
	currentAlsaDevice_->setSwitch(mixer, id, enabled);
}

bool AlsaWork::checkCardId(int cardId)
{
	try {
		if (cardId < int(cardList_.size()) && !cardList_.at(cardId).empty()) {
			return true;
		}
	}
	catch (std::out_of_range &ex) {
		std::cerr << "alsawork.cpp::124:: Item out of Range " << ex.what() << std::endl;
	}
	return false;
}

//private
void AlsaWork::checkError (int errorIndex)
{
	if (errorIndex < 0) {
		std::cerr << snd_strerror(errorIndex) << std::endl;
	}
}

std::string AlsaWork::formatCardName(int id)
{
	size_t size = 64;
	char *name = (char*)malloc(size);
	sprintf(name, "hw:%d", id);
	return std::string(name);
}

int AlsaWork::getTotalCards()
{
	int cards = 0;
	int index = -1;
	while (true) {
		checkError(snd_card_next(&index));
		if (index < 0) {
			break;
		}
		++cards;
	}
	return cards;
}

void AlsaWork::getCards()
{
	if (!cardList_.empty())
		cardList_.clear();
	totalCards_ = getTotalCards();
	if (totalCards_ >= 1) {
		for (int card = 0; card < totalCards_; card++) {
			std::string cname(getCardName(card));
			cardList_.push_back(cname);
		}
	}
}

bool AlsaWork::haveVolumeMixers()
{
	return currentAlsaDevice_->haveMixers();
}

bool AlsaWork::cardExists(int id)
{
	if (id >= 0 && id < totalCards_) {
		return true;
	}
	return false;
}

bool AlsaWork::mixerExists(const std::string &name)
{
	return Tools::itemExists(currentAlsaDevice_->mixers(), name).first;
}

bool AlsaWork::mixerExists(int id)
{
	return bool(id >=0 && id < (int)currentAlsaDevice_->mixers().size());
}

int AlsaWork::getFirstCardWithMixers()
{
	std::vector< AlsaDevice* >::iterator it = devices_.begin();
	int inc = 0;
	while (it != devices_.end()) {
		AlsaDevice *dev = *it;
		if(dev->haveMixers()) {
			delete dev;
			return inc;
		}
		++it;
		++inc;
		delete dev;
	}
	return 0;
}

int AlsaWork::getCurrentMixerId()
{
	return currentAlsaDevice_->currentMixerId();
}
