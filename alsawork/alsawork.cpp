/*
 * alsawork.cpp
 * Copyright (C) 2012-2019 Vitaly Tonkacheyev
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "mixerswitches.h"
#include "alsawork.h"
#include <memory>
#include <stdexcept>

AlsaWork::AlsaWork()
    : cardList_(std::vector<std::string>()),
    totalCards_(0)
{
    getCards();
    int id = 0;
    for(const std::string &name : cardList_){
        devices_.push_back(std::make_shared<AlsaDevice>(id, name));
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
        currentAlsaDevice_ = devices_.at(ulong(cardId));
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

std::string AlsaWork::getCardName(int index)
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

std::string AlsaWork::getMixerName(int index)
{
    std::string mixerName;
    if (index >= 0 && index < int(currentAlsaDevice_->mixers().size())) {
        mixerName = currentAlsaDevice_->mixers().at(ulong(index));
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

bool AlsaWork::cardExists(int id)
{
    return bool(id >= 0 && id < totalCards_);
}

int AlsaWork::getFirstCardWithMixers()
{
    auto it = std::find_if(devices_.begin(),
                           devices_.end(),
                           [](const AlsaDevice::Ptr &dev){return dev->haveMixers();});
    return (it != devices_.end()) ? int(it - devices_.begin()) : 0;
}

AlsaWork::AlsaWork(AlsaWork const &aw)
: totalCards_(aw.totalCards_)
{
}
