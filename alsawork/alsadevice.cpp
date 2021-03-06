/*
 * alsadevice.cpp
 * Copyright (C) 2014-2019 Vitaly Tonkacheyev
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

#include "alsadevice.h"

#include <memory>
#include <utility>
#include "cmath"

#define ZERO 0.0

AlsaDevice::AlsaDevice(int id, std::string card)
    : id_(id),
      name_(std::move(card)),
      volumeMixers_(std::vector<std::string>()),
      captureMixers_(std::vector<std::string>()),
      mixers_(std::vector<std::string>()),
      switches_(std::make_shared<MixerSwitches>()),
      currentMixerId_(0),
      currentMixerName_(std::string())
{
    updateElements();
}

AlsaDevice::~AlsaDevice()
= default;

void AlsaDevice::updateElements()
{
    if (!volumeMixers_.empty()) {
        volumeMixers_.clear();
    }
    if (!captureMixers_.empty()) {
        captureMixers_.clear();
    }
    if (switches_ && !switches_->isEmpty()) {
        switches_->clearAll();
    }
    snd_mixer_t *handle = getMixerHanlde(id_);
    snd_mixer_selem_id_t *smid;
    snd_mixer_selem_id_alloca(&smid);
    std::string deviceName;
    snd_mixer_elem_t *element;
    for (element = snd_mixer_first_elem(handle);
         element;
         element = snd_mixer_elem_next(element)) {
        if (!snd_mixer_elem_empty(element)) {
            switchcap sCap;
            snd_mixer_selem_get_id(element, smid);
            deviceName = snd_mixer_selem_id_get_name(smid);
            snd_mixer_selem_channel_id_t channel = checkMixerChannels(element);

            if (snd_mixer_selem_has_playback_volume(element)
                    || snd_mixer_selem_has_playback_volume_joined(element)
                    || snd_mixer_selem_has_common_volume(element)) {
                volumeMixers_.push_back(deviceName);
            }
            if (snd_mixer_selem_has_capture_volume(element)
                    || snd_mixer_selem_has_capture_volume_joined(element)) {
                captureMixers_.push_back(deviceName);
            }
            if (snd_mixer_selem_has_capture_switch(element)
                    || snd_mixer_selem_has_common_switch(element)
                    || snd_mixer_selem_has_capture_switch_joined(element)
                    || snd_mixer_selem_has_capture_switch_exclusive(element)){
                int value = 0;
                checkError(snd_mixer_selem_get_capture_switch(element, channel, &value));
                sCap = std::make_pair(deviceName, bool(value));
                switches_->pushBack(CAPTURE, sCap);
            }
            if (snd_mixer_selem_has_playback_switch(element)
                    || snd_mixer_selem_has_playback_switch_joined(element)){
                int value = 0;
                checkError(snd_mixer_selem_get_playback_switch(element, channel, &value));
                sCap = std::make_pair(deviceName, bool(value));
                switches_->pushBack(PLAYBACK, sCap);
            }
            if (snd_mixer_selem_is_enumerated(element)) {
                uint value = 0;
                checkError(snd_mixer_selem_get_enum_item(element, channel, &value));
                sCap = std::make_pair(deviceName, bool(value));
                switches_->pushBack(ENUM, sCap);
            }
        }
    }
    checkError(snd_mixer_close(handle));
    initMixerList();
}

void AlsaDevice::initMixerList()
{
    if (!mixers_.empty()) {
        mixers_.clear();
    }
    if (havePlaybackMixers()) {
        mixers_.assign(volumeMixers_.begin(), volumeMixers_.end());
    }
    if (haveCaptureMixers() && !mixers_.empty()) {
        mixers_.insert(mixers_.end(), captureMixers_.begin(), captureMixers_.end());
    }
    if (!mixers_.empty()) {
        setCurrentMixer(0);
    }
}

snd_mixer_elem_t *AlsaDevice::initMixerElement(snd_mixer_t *handle, const char *mixer)
{
    snd_mixer_selem_id_t *smid;
    snd_mixer_selem_id_alloca(&smid);
    snd_mixer_selem_id_set_index(smid, 0);
    snd_mixer_selem_id_set_name(smid, mixer);
    snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, smid);
    return elem;
}

snd_mixer_t *AlsaDevice::getMixerHanlde(int id)
{
    const std::string card(formatCardName(id));
    snd_mixer_t *handle;
    checkError(snd_mixer_open(&handle, 0));
    checkError(snd_mixer_attach(handle, card.c_str()));
    checkError(snd_mixer_selem_register(handle, nullptr, nullptr));
    checkError(snd_mixer_load(handle));
    return handle;
}

snd_mixer_selem_channel_id_t AlsaDevice::checkMixerChannels(snd_mixer_elem_t *element)
{
    if (snd_mixer_selem_is_playback_mono(element)) {
        return SND_MIXER_SCHN_MONO;
    }
    else {
        for (int channel = 0; channel <= SND_MIXER_SCHN_LAST; channel++) {
            if (snd_mixer_selem_has_playback_channel(element, static_cast<snd_mixer_selem_channel_id_t>(channel))) {
                return static_cast<snd_mixer_selem_channel_id_t>(channel);
            }
        }
    }
    if (snd_mixer_selem_is_capture_mono(element)) {
        return SND_MIXER_SCHN_MONO;
    }
    else {
        for (int channel = 0; channel <= SND_MIXER_SCHN_LAST; channel++) {
            if (snd_mixer_selem_has_capture_channel(element, static_cast<snd_mixer_selem_channel_id_t>(channel))) {
                return static_cast<snd_mixer_selem_channel_id_t>(channel);
            }
        }
    }
    return SND_MIXER_SCHN_UNKNOWN;
}

//This part of code from alsa-utils.git/alsamixer/volume_mapping.c
//Copyright (c) 2010 Clemens Ladisch <clemens@ladisch.de>
double AlsaDevice::getExp10(double value)
{
    return exp(value * log(10));
}

double AlsaDevice::getNormVolume(snd_mixer_elem_t *element)
{
    long min, max, value;
    double norm, minNorm;
    int err;
    snd_mixer_selem_channel_id_t chanelid = checkMixerChannels(element);
    if (snd_mixer_selem_has_playback_volume(element)) {
        err = snd_mixer_selem_get_playback_dB_range(element, &min, &max);
        if (err < 0 || min >= max) {
            err = snd_mixer_selem_get_playback_volume_range(element, &min, &max);
            if (err < 0 || min == max) {
                return 0;
            }
            err = snd_mixer_selem_get_playback_volume(element, chanelid, &value);
            if (err < 0) {
                return 0;
            }
            return double(value - min) / double(max-min);
        }
        err = snd_mixer_selem_get_playback_dB(element, chanelid, &value);
        if (err < 0) {
            return 0;
        }
        if (useLinearDb(min, max)) {
            return double(value - min) / double(max-min);
        }
        norm = getExp10(double(value - max) / 6000.0);
        if (min != SND_CTL_TLV_DB_GAIN_MUTE) {
            minNorm = getExp10(double(min - max) / 6000.0);
            norm = (norm - minNorm)/(1 - minNorm);
        }
        return norm;
    }
    else if (snd_mixer_selem_has_capture_volume(element)) {
        err = snd_mixer_selem_get_capture_dB_range(element, &min, &max);
        if (err < 0 || min >= max) {
            err = snd_mixer_selem_get_capture_volume_range(element, &min, &max);
            if (err < 0 || min == max) {
                return 0;
            }
            err = snd_mixer_selem_get_capture_volume(element, chanelid, &value);
            if (err < 0) {
                return 0;
            }
            return double(value - min) / double(max - min);
        }

        err = snd_mixer_selem_get_capture_dB(element, chanelid, &value);
        if (err < 0) {
            return 0;
        }
        if (useLinearDb(min, max)) {
            return double(value - min) / double(max - min);
        }
        norm = getExp10(double(value - max) / 6000.0);
        if (min != SND_CTL_TLV_DB_GAIN_MUTE) {
            minNorm = getExp10(double(min - max) / 6000.0);
            norm = (norm - minNorm)/(1 - minNorm);
        }
        return norm;
    }
    return ZERO;
}

bool AlsaDevice::useLinearDb(long min, long max)
{
    const long maxDB = 24;
    return (max - min) <= maxDB*100;
}

void AlsaDevice::setNormVolume(snd_mixer_elem_t *element, double volume)
{
    long min, max, value;
    double min_norm;
    int err;
    if (snd_mixer_selem_has_playback_volume(element)) {
        err = snd_mixer_selem_get_playback_dB_range(element, &min, &max);
        if (err < 0 || min >= max) {
            err = snd_mixer_selem_get_playback_volume_range(element, &min, &max);
            if (err < 0) {
                return;
            }
            value = lrint(volume*double(max-min)) + min;
            checkError(snd_mixer_selem_set_playback_volume_all(element, value));
            return;
        }
        if (useLinearDb(min, max)) {
            value = lrint(volume*double(max-min)) + min;
            checkError(snd_mixer_selem_set_playback_dB_all(element, value, 1));
            return;
        }
        if (min != SND_CTL_TLV_DB_GAIN_MUTE) {
            min_norm = getExp10(double(min-max)/6000.0);
            volume = volume * (1-min_norm) + min_norm;
        }
        value = lrint(6000.0 * log10(volume))+max;
        checkError(snd_mixer_selem_set_playback_dB_all(element, value, 1));
        return;
    }
    else if (snd_mixer_selem_has_capture_volume(element)) {
        err = snd_mixer_selem_get_capture_dB_range(element, &min, &max);
        if (err < 0 || min >= max) {
            err = snd_mixer_selem_get_capture_volume_range(element, &min, &max);
            if (err < 0) {
                return;
            }
            value = lrint(volume*double(max-min)) + min;
            checkError(snd_mixer_selem_set_capture_volume_all(element, value));
            return;
        }
        if (useLinearDb(min, max)) {
            value = lrint(volume*double(max-min)) + min;
            checkError(snd_mixer_selem_set_capture_dB_all(element, value, 1));
            return;
        }
        if (min != SND_CTL_TLV_DB_GAIN_MUTE) {
            min_norm = getExp10(double(min-max)/6000.0);
            volume = volume * (1-min_norm) + min_norm;
        }
        value = lrint(6000.0 * log10(volume))+max;
        checkError(snd_mixer_selem_set_capture_dB_all(element, value, 1));
    }
}

//This part of code from alsa-utils.git/alsamixer/volume_mapping.c

void AlsaDevice::setDeviceVolume(double volume)
{
    if (!currentMixerName_.empty()) {
        snd_mixer_t *handle = getMixerHanlde(id_);
        snd_mixer_elem_t *element = initMixerElement(handle, currentMixerName_.c_str());
        if (!snd_mixer_elem_empty(element)) {
            setNormVolume(element, volume/100);
        }
        checkError(snd_mixer_close(handle));
    }
}

double AlsaDevice::getVolume()
{
    double result = ZERO;
    if (!currentMixerName_.empty()) {
        snd_mixer_t *handle = getMixerHanlde(id_);
        snd_mixer_elem_t *elem = initMixerElement(handle, currentMixerName_.c_str());
        if (!snd_mixer_elem_empty(elem)) {
            result = getNormVolume(elem)*100;
        }
        checkError(snd_mixer_close(handle));
    }
    return round(result);
}

void AlsaDevice::setSwitch(const std::string &mixer, int id, bool enabled)
{
    snd_mixer_t *handle = getMixerHanlde(id_);
    snd_mixer_elem_t* elem = initMixerElement(handle, mixer.c_str());
    if (!snd_mixer_elem_empty(elem)) {
        switch (id) {
        case PLAYBACK:
        default:
            checkError(snd_mixer_selem_set_playback_switch_all(elem, int(enabled)));
            break;
        case CAPTURE:
            checkError(snd_mixer_selem_set_capture_switch_all(elem, int(enabled)));
            break;
        case ENUM:
            snd_mixer_selem_channel_id_t channel = checkMixerChannels(elem);
            checkError(snd_mixer_selem_set_enum_item(elem, channel, uint(enabled)));
            break;
        }
    }
    checkError(snd_mixer_close(handle));
}

void AlsaDevice::setMute(bool enabled)
{
    if (!currentMixerName_.empty()) {
        snd_mixer_t *handle = getMixerHanlde(id_);
        snd_mixer_elem_t* elem = initMixerElement(handle, currentMixerName_.c_str());
        if (!snd_mixer_elem_empty(elem)) {
            if (snd_mixer_selem_has_playback_switch(elem)
                    || snd_mixer_selem_has_playback_switch_joined(elem)) {
                checkError(snd_mixer_selem_set_playback_switch_all(elem, int(enabled)));
            }
            if (snd_mixer_selem_has_capture_switch(elem)
                    || snd_mixer_selem_has_common_switch(elem)
                    || snd_mixer_selem_has_capture_switch_joined(elem)
                    || snd_mixer_selem_has_capture_switch_exclusive(elem)) {
                checkError(snd_mixer_selem_set_capture_switch_all(elem, int(enabled)));
            }
        }
        checkError(snd_mixer_close(handle));
    }
}

bool AlsaDevice::getMute()
{
    if (!currentMixerName_.empty()) {
        snd_mixer_t *handle = getMixerHanlde(id_);
        snd_mixer_elem_t* elem = initMixerElement(handle, currentMixerName_.c_str());
        if (!snd_mixer_elem_empty(elem)) {
            snd_mixer_selem_channel_id_t channel = checkMixerChannels(elem);
            if (snd_mixer_selem_has_playback_switch(elem)
                    || snd_mixer_selem_has_playback_switch_joined(elem)) {
                int value = 0;
                checkError(snd_mixer_selem_get_playback_switch(elem, channel, &value));
                checkError(snd_mixer_close(handle));
                return bool(value);
            }
            if (snd_mixer_selem_has_capture_switch(elem)
                    || snd_mixer_selem_has_common_switch(elem)
                    || snd_mixer_selem_has_capture_switch_joined(elem)
                    || snd_mixer_selem_has_capture_switch_exclusive(elem)) {
                int value = 0;
                checkError(snd_mixer_selem_get_capture_switch(elem, channel, &value));
                checkError(snd_mixer_close(handle));
                return bool(value);
            }
        }
        checkError(snd_mixer_close(handle));
    }
    return true;
}

std::string AlsaDevice::formatCardName(long long int id)
{
    return std::string("hw:") + std::to_string(id);
}

void AlsaDevice::setCurrentMixer(int id)
{
    if(id >= 0 && id < int(mixers_.size())) {
        currentMixerId_ = id;
        currentMixerName_ = mixers_.at(ulong(id));
    }
}

void AlsaDevice::setCurrentMixer(const std::string &mixer)
{
    if(Tools::itemExists(mixers_, mixer)){
        currentMixerId_ = Tools::itemIndex(mixers_, mixer);
        currentMixerName_ = mixer;
#ifdef IS_DEBUG
        std::cout << "MID " << currentMixerId_ << std::endl;
        std::cout << "MN " << currentMixerName_ << std::endl;
#endif
    }
}

const std::string &AlsaDevice::name() const
{
    return name_;
}

int AlsaDevice::id() const
{
    return id_;
}

const std::vector<std::string> &AlsaDevice::mixers() const
{
    return mixers_;
}

void AlsaDevice::checkError (int errorIndex)
{
    if (errorIndex < 0) {
        std::cerr << snd_strerror(errorIndex) << std::endl;
    }
}

bool AlsaDevice::havePlaybackMixers()
{
    return !volumeMixers_.empty();
}

bool AlsaDevice::haveCaptureMixers()
{
    return !captureMixers_.empty();
}

bool AlsaDevice::haveMixers()
{
    return !mixers_.empty();
}

MixerSwitches::Ptr AlsaDevice::switches()
{
    updateElements();
    return switches_;
}

int AlsaDevice::currentMixerId() const
{
    return currentMixerId_;
}

const std::string &AlsaDevice::currentMixer() const
{
    return currentMixerName_;
}

AlsaDevice::AlsaDevice(AlsaDevice const &ad)
 : currentMixerId_(ad.currentMixerId()),
   id_(ad.id())
{
}
