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
		const std::string name = *it;
		devices_.push_back(AlsaDevice::Ptr(new AlsaDevice(id, name)));
		++it;
		++id;
	}
	setCurrentCard(0);
	setCurrentMixer(0);
}

AlsaWork::~AlsaWork()
{
	if (!devices_.empty())
		devices_.clear();
	currentAlsaDevice_.reset();
	snd_config_update_free_global();
}

//public
void AlsaWork::setCurrentCard(int cardId)
{
	if(cardId < int(devices_.size())) {
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

double AlsaWork::getAlsaVolume() const
{
	return currentAlsaDevice_->getVolume();
}

const std::string AlsaWork::getCardName(int index)
{
	const std::string card(AlsaDevice::formatCardName(index));
	snd_ctl_t *ctl;
	int err = snd_ctl_open(&ctl, card.c_str(), SND_CTL_NONBLOCK);
	if (err < 0) {
		checkError(err);
		return std::string();
	}
	snd_ctl_card_info_t *cardInfo;
	snd_ctl_card_info_alloca(&cardInfo);
	err = snd_ctl_card_info(ctl, cardInfo);
	if (err < 0) {
		checkError(err);
		return std::string();
	}
	const std::string cardName = snd_ctl_card_info_get_name(cardInfo);
	checkError(snd_ctl_close(ctl));
	return cardName;
}

const std::string AlsaWork::getMixerName(int index)
{
	std::string mixerName;
	if (index >= 0 && index < currentAlsaDevice_->mixers().size()) {
		mixerName = currentAlsaDevice_->mixers().at(index);
	}
	return mixerName;
}

std::string AlsaWork::getCurrentMixerName() const
{
	return currentAlsaDevice_->currentMixer();
}

const std::vector<std::string> &AlsaWork::getCardsList() const
{
	return cardList_;
}

const std::vector<std::string> &AlsaWork::getVolumeMixers() const
{
	return currentAlsaDevice_->mixers();
}

MixerSwitches::Ptr AlsaWork::getSwitchList() const
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
			const std::string cname(getCardName(card));
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
	return bool(id >= 0 && id < totalCards_);
}

bool AlsaWork::mixerExists(const std::string &name)
{
	return Tools::itemExists(currentAlsaDevice_->mixers(), name);
}

bool AlsaWork::mixerExists(int id)
{
	return bool(id >=0 && id < (int)currentAlsaDevice_->mixers().size());
}

int AlsaWork::getFirstCardWithMixers()
{
	AlsaDevicePtrList::iterator it = devices_.begin();
	int inc = 0;
	while (it != devices_.end()) {
		AlsaDevice::Ptr dev = *it;
		if(dev->haveMixers()) {
			return inc;
		}
		++it;
		++inc;
	}
	return 0;
}

int AlsaWork::getCurrentMixerId() const
{
	return currentAlsaDevice_->currentMixerId();
}
