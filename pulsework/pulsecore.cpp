/*
 * Copyright (C) 2011 Clément Démoulins <clement@archivel.fr>
 * Copyright (C) 2014-2025 Vitaly Tonkacheyev <thetvg@gmail.com>
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

#include "pulsecore.h"
#include "../tools/tools.h"
#include <vector>
#include <cmath>
#include <pulse/operation.h>
#include <iostream>

PulseCore::PulseCore(const char *clientName)
    : mainLoop_(pa_mainloop_new()),
      mainLoopApi_(pa_mainloop_get_api(mainLoop_)),
      context_(pa_context_new(mainLoopApi_,clientName)),
      retval_(0)
{
    isAvailable_ = true;
    pState = CONNECTING;
    pa_context_set_state_callback(
        context_,
        [](pa_context *context, void *raw) {
            auto *core = static_cast<PulseCore *>(raw);
            switch (pa_context_get_state(context)) {
            case PA_CONTEXT_READY:
                core->pState = CONNECTED;
                break;
            case PA_CONTEXT_FAILED:
                core->pState = ERROR;
                break;
            case PA_CONTEXT_UNCONNECTED:
            case PA_CONTEXT_AUTHORIZING:
            case PA_CONTEXT_SETTING_NAME:
            case PA_CONTEXT_CONNECTING:
            case PA_CONTEXT_TERMINATED:
                break;
            }
        },
        this);
    pa_context_connect(context_, nullptr, PA_CONTEXT_NOFLAGS, nullptr);
    while (pState == CONNECTING) {
        pa_mainloop_iterate(mainLoop_, 1, &retval_);
    }
    if (pState == ERROR) {
        onError("Pulseaudio Connection Error");
        isAvailable_ = false;
    }
    if (isAvailable_) {
        updateDevices();
        currentDeviceName_ = defaultSink();
        currentDeviceIndex_ = getCurrentDeviceIndex();
    }
}

PulseCore::~PulseCore()
{
    if (!sinks_.empty())
        sinks_.clear();
    if (!sources_.empty())
        sources_.clear();
    if (pState == CONNECTED)
        pa_context_disconnect(context_);
    pa_mainloop_free(mainLoop_);
}

void PulseCore::iterate(pa_operation *op)
{
    while (pa_operation_get_state(op) == PA_OPERATION_RUNNING) {
        pa_mainloop_iterate(mainLoop_, 1, &retval_);
    }
}

void PulseCore::getSinks()
{
    pa_operation *op = pa_context_get_sink_info_list(
        context_,
        [](pa_context *c, const pa_sink_info *i, int eol, void *raw) {
            std::ignore = c;
            if (eol != 0)
                return;
            auto *core = static_cast<PulseCore *>(raw);
            const PulseDevice::Ptr s(new PulseDevice(i));
            core->sinks_.push_back(s);
        },
        this);
    iterate(op);
    pa_operation_unref(op);
}

void PulseCore::getSources()
{
    pa_operation *op = pa_context_get_source_info_list(
        context_,
        [](pa_context *c, const pa_source_info *i, int eol, void *raw) {
            std::ignore = c;
            if (eol != 0)
                return;
            auto *core = static_cast<PulseCore *>(raw);
            const PulseDevice::Ptr s(new PulseDevice(i));
            core->sources_.push_back(s);
        },
        this);
    iterate(op);
    pa_operation_unref(op);
}

PulseDevice::Ptr PulseCore::getSink(u_int32_t index)
{
    if (index < sinks_.size()) {
        return sinks_.at(index);
    }
    else {
        onError("ERROR in pulsecore.cpp:132. This pulseaudio sink does not exits. Default sink will be used");
    }
    return getDefaultSink();
}

PulseDevice::Ptr PulseCore::getSink(const std::string &name)
{
    auto it = std::find_if(sinks_.begin(),
                           sinks_.end(),
                           [name](const PulseDevice::Ptr &device){return (device->name() == name);});
    return (it != sinks_.end()) ? *it : getDefaultSink();
}

PulseDevice::Ptr PulseCore::getSource(u_int32_t index)
{
    if (index < sources_.size()) {
        return sources_.at(index);
    }
    else {
        onError("ERROR in pulsecore.cpp:161. This pulseaudio source does not exits. Default source will be used");
    }
    return getDefaultSource();
}

PulseDevice::Ptr PulseCore::getSource(const std::string &name)
{
    auto it = std::find_if(sources_.begin(),
                           sources_.end(),
                           [name](const PulseDevice::Ptr &device){return (device->name() == name);});
    return (it != sinks_.end()) ? *it : getDefaultSource();
}

PulseDevice::Ptr PulseCore::getDefaultSink()
{
    ServerInfo info;
    pa_operation *op = pa_context_get_server_info(
        context_,
        [](pa_context *c, const pa_server_info *i, void *raw) {
            std::ignore = c;
            auto info = static_cast<ServerInfo *>(raw);
            info->defaultSinkName = std::string(i->default_sink_name);
            info->defaultSourceName = std::string(i->default_source_name);
        },
        &info);
    iterate(op);
    pa_operation_unref(op);
    if (!info.defaultSinkName.empty()) {
        return getSink(info.defaultSinkName);
    }
    isAvailable_ = false;
    return PulseDevice::Ptr();
}

PulseDevice::Ptr PulseCore::getDefaultSource()
{
    ServerInfo info;
    pa_operation *op = pa_context_get_server_info(
        context_,
        [](pa_context *c, const pa_server_info *i, void *raw) {
            std::ignore = c;
            auto info = static_cast<ServerInfo *>(raw);
            info->defaultSinkName = std::string(i->default_sink_name);
            info->defaultSourceName = std::string(i->default_source_name);
        },
        &info);
    iterate(op);
    pa_operation_unref(op);
    if (!info.defaultSourceName.empty()) {
        return getSource(info.defaultSourceName);
    }
    isAvailable_ = false;
    return PulseDevice::Ptr();
}

const std::vector<std::string> &PulseCore::getSinksDescriptions() const
{
    return sinksDescriptions_;
}

const std::vector<std::string> &PulseCore::getSourcesDescriptions() const
{
    return sourcesDescriptions_;
}

std::string PulseCore::getDeviceDescription(const std::string &name)
{
    std::string desc = getDeviceByName(name)->description();
    return desc.empty() ? getDefaultSink()->description() : desc;
}

PulseDevice::Ptr PulseCore::getDeviceByName(const std::string &name)
{
    PulseDevice::Ptr result;
    for(const PulseDevice::Ptr &device : sinks_){
        if(device->name() == name) {
            result = device;
        }
    }
    for(const PulseDevice::Ptr &device : sources_){
        if(device->name() == name) {
            result = device;
        }
    }
    return result;
}

std::string PulseCore::getDeviceNameByIndex(int index)
{
    return getDeviceByIndex(index)->name();
}

std::string PulseCore::defaultSink()
{
    return getDefaultSink()->name();
}

void PulseCore::setVolume_(const PulseDevice::Ptr &device, int value)
{
    pa_cvolume* new_cvolume = pa_cvolume_set(&device->volume,
                                             device->volume.channels,
                                             pa_volume_t(device->round(MAX((double(value) * PA_VOLUME_NORM) / 100, 0.0)))
                                             );
    pa_operation* op;
    if (device->type() == SINK) {
        op = pa_context_set_sink_volume_by_index(
            context_,
            device->index(),
            new_cvolume,
            [](pa_context *c = nullptr, int success = 0, void *raw = nullptr) {},
            nullptr);
    }
    else {
        op = pa_context_set_source_volume_by_index(
            context_,
            device->index(),
            new_cvolume,
            [](pa_context *c = nullptr, int success = 0, void *raw = nullptr) {},
            nullptr);
    }
    iterate(op);
    pa_operation_unref(op);
}

void PulseCore::setMute_(const PulseDevice::Ptr &device, bool mute)
{
    pa_operation* op;
    if (device->type() == SINK) {
        op = pa_context_set_sink_mute_by_index(
            context_,
            device->index(),
            int(mute),
            [](pa_context *c = nullptr, int success = 0, void *raw = nullptr) {},
            nullptr);
    }
    else {
        op = pa_context_set_source_mute_by_index(
            context_,
            device->index(),
            int(mute),
            [](pa_context *c = nullptr, int success = 0, void *raw = nullptr) {},
            nullptr);
    }
    iterate(op);
    pa_operation_unref(op);
}

void PulseCore::setCurrentDevice(const std::string &name)
{
    updateDevices();
    currentDeviceName_ = getDeviceByName(name)->name();
    currentDeviceIndex_ = getCurrentDeviceIndex();
}

void PulseCore::setVolume(int value)
{
    setVolume_(getDeviceByIndex(currentDeviceIndex_), value);
}

void PulseCore::setMute(bool mute)
{
    setMute_(getDeviceByIndex(currentDeviceIndex_), !mute);
}

void PulseCore::updateDevices()
{
    clearLists();
    getSinks();
    getSources();
    for(const PulseDevice::Ptr &device : sinks_){
        devicesNames_.push_back(device->name());
        sinksDescriptions_.push_back(device->description());
    }
    for(const PulseDevice::Ptr &device : sources_){
        devicesNames_.push_back(device->name());
        sourcesDescriptions_.push_back(device->description());
    }
    devicesDescs_.insert(devicesDescs_.end(), sinksDescriptions_.begin(), sinksDescriptions_.end());
    devicesDescs_.insert(devicesDescs_.end(), sourcesDescriptions_.begin(), sourcesDescriptions_.end());
}

void PulseCore::clearLists()
{
    if (!sinks_.empty())
        sinks_.clear();
    if (!sources_.empty())
        sources_.clear();
    if (!sinksDescriptions_.empty())
        sinksDescriptions_.clear();
    if(!sourcesDescriptions_.empty())
        sourcesDescriptions_.clear();
    if(!devicesNames_.empty())
        devicesNames_.clear();
    if(!devicesDescs_.empty())
        devicesDescs_.clear();
}

int PulseCore::getVolume()
{
    return getDeviceByIndex(currentDeviceIndex_)->volume_percent();
}

bool PulseCore::getMute()
{
    return getDeviceByIndex(currentDeviceIndex_)->mute();
}

const std::vector<std::string> &PulseCore::getCardList() const
{
    return devicesDescs_;
}

const std::vector<std::string> &PulseCore::getCardNames() const
{
    return devicesNames_;
}

PulseDevice::Ptr PulseCore::getDeviceByIndex(int index)
{
    updateDevices();
    PulseDevice::Ptr device = getDefaultSink();
    const int sinksSize = int(sinksDescriptions_.size());
    const int sourcesSize = int(sourcesDescriptions_.size());
    if (index >=0 && index < (sinksSize + sourcesSize)) {
        const int deltaIndex = sinksSize - index;
        device = (deltaIndex > 0) ? getSink(uint(index)) : getSource(uint(abs(deltaIndex)));
    }
    return device;
}

int PulseCore::getCurrentDeviceIndex()
{
    const int index = Tools::itemIndex(devicesNames_, currentDeviceName_);
    return ((index > 0) ? index : 0);
}

int PulseCore::getCardIndex()
{
    return int(getDeviceByIndex(currentDeviceIndex_)->card());
}

bool PulseCore::deviceNameExists(const std::string &name)
{
    return Tools::itemExists(devicesNames_, name);
}

bool PulseCore::deviceDescriptionExists(const std::string &description)
{
    return Tools::itemExists(devicesDescs_, description);
}

bool PulseCore::available()
{
    return isAvailable_;
}

void PulseCore::refreshDevices()
{
    updateDevices();
}

PulseCore::PulseCore(PulseCore const &pc)
: pState(pc.pState),
  mainLoop_(pc.mainLoop_),
  mainLoopApi_(pc.mainLoopApi_),
  context_(pc.context_),
  retval_(pc.retval_),
  currentDeviceIndex_(pc.currentDeviceIndex_),
  isAvailable_(pc.isAvailable_)
{
}

void PulseCore::onError(const std::string &message) { std::cerr << "Error: " << message << std::endl; }
