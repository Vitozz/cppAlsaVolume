/*
 * Copyright (C) 2011 Clément Démoulins <clement@archivel.fr>
 * Copyright (C) 2014 Vitaly Tonkacheyev <thetvg@gmail.com>
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
 */

#include "pulsecore.h"
#include "pulse/pulseaudio.h"
#include "../tools/tools.h"
#include <vector>
#include <cmath>

//Callbacks
void state_cb(pa_context* context, void* raw) {
	PulseCore *state = (PulseCore*) raw;
	switch(pa_context_get_state(context)) {
		case PA_CONTEXT_READY:
			state->pState = CONNECTED;
			break;
		case PA_CONTEXT_FAILED:
			state->pState = ERROR;
			break;
		case PA_CONTEXT_UNCONNECTED:
		case PA_CONTEXT_AUTHORIZING:
		case PA_CONTEXT_SETTING_NAME:
		case PA_CONTEXT_CONNECTING:
		case PA_CONTEXT_TERMINATED:
			break;
	}
}

void sink_list_cb(pa_context *c, const pa_sink_info *i, int eol, void *raw) {
	(void)c;
	if (eol != 0) return;
	PulseDevicePtrList *sinks = (PulseDevicePtrList*) raw;
	const PulseDevicePtr s(new PulseDevice(i));
	sinks->push_back(s);
}

void source_list_cb(pa_context *c, const pa_source_info *i, int eol, void *raw) {
	(void)c;
	if (eol != 0) return;
	PulseDevicePtrList *sources = (PulseDevicePtrList*) raw;
	const PulseDevicePtr s(new PulseDevice(i));
	sources->push_back(s);
}

void server_info_cb(pa_context* c, const pa_server_info* i, void* raw) {
	(void)c;
	ServerInfo* info = (ServerInfo*) raw;
	info->defaultSinkName = std::string(i->default_sink_name);
	info->defaultSourceName = std::string(i->default_source_name);
}

void success_cb(pa_context* c, int success, void* raw) {
	(void)c;
	(void)success;
	(void)raw;
}
//


PulseCore::PulseCore(const char *clientName)
: mainLoop_(pa_mainloop_new()),
  mainLoopApi_(pa_mainloop_get_api(mainLoop_)),
  context_(pa_context_new(mainLoopApi_,clientName))
{
	pState = CONNECTING;
	pa_context_set_state_callback(context_, &state_cb, this);
	pa_context_connect(context_, NULL, PA_CONTEXT_NOFLAGS, NULL);
	while (pState == CONNECTING) {
		pa_mainloop_iterate(mainLoop_, 1, &retval_);
	}
	if (pState == ERROR) {
		onError("Connection Error");
	}
	updateDevices();
	currentDevice_ = getDefaultSink();
}

PulseCore::~PulseCore()
{
	if (!sinks_.empty())
		sinks_.clear();
	if (!sources_.empty())
		sources_.clear();
	currentDevice_.reset();
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
	pa_operation* op = pa_context_get_sink_info_list(context_, &sink_list_cb, &sinks_);
	iterate(op);
	pa_operation_unref(op);
}

void PulseCore::getSources()
{
	pa_operation* op = pa_context_get_source_info_list(context_, &source_list_cb, &sources_);
	iterate(op);
	pa_operation_unref(op);
}

PulseDevicePtr PulseCore::getSink(u_int32_t index)
{
	if (index < sinks_.size()) {
		return sinks_.at(index);
	}
	else {
		onError("This pulseaudio sink does not exits. Default sink will be used");
	}
	return getDefaultSink();
}

PulseDevicePtr PulseCore::getSink(const std::string &name)
{
	PulseDevicePtrList::iterator it = sinks_.begin();
	while(it != sinks_.end()) {
		PulseDevicePtr device = *it;
		if (device->name() == name) {
			return device;
		}
		++it;
	}
	onError("This pulseaudio sink does not exits. Default sink will be used");
	return getDefaultSink();
}

PulseDevicePtr PulseCore::getSource(u_int32_t index)
{
	if (index < sources_.size()) {
		return sources_.at(index);
	}
	else {
		onError("This pulseaudio source does not exits. Default source will be used");
	}
	return getDefaultSource();
}

PulseDevicePtr PulseCore::getSource(const std::string &name)
{
	PulseDevicePtrList::iterator it = sources_.begin();
	while(it != sources_.end()) {
		PulseDevicePtr device = *it;
		if (device->name() == name) {
			return device;
		}
		++it;
	}
	onError("This pulseaudio source does not exits. Default source will be used");
	return getDefaultSource();
}

PulseDevicePtr PulseCore::getDefaultSink()
{
	ServerInfo info;
	pa_operation* op = pa_context_get_server_info(context_, &server_info_cb, &info);
	iterate(op);
	pa_operation_unref(op);
	return getSink(info.defaultSinkName);
}

PulseDevicePtr PulseCore::getDefaultSource()
{
	ServerInfo info;
	pa_operation* op = pa_context_get_server_info(context_, &server_info_cb, &info);
	iterate(op);
	pa_operation_unref(op);
	return getSource(info.defaultSourceName);
}

const std::vector<std::string> &PulseCore::getSinksDescriptions() const
{
	return sinksDescriptions_;
}

const std::vector<std::string> &PulseCore::getSourcesDescriptions() const
{
	return sourcesDescriptions_;
}

const std::string PulseCore::getDeviceDescription(const std::string &name)
{
	std::string desc = getDeviceByName(name)->description();
	if (desc.empty()) {
		desc = getDefaultSink()->description();
	}
	return desc;
}

PulseDevicePtr PulseCore::getDeviceByName(const std::string &name)
{
	PulseDevicePtr result = PulseDevicePtr(new PulseDevice());
	PulseDevicePtrList::iterator it = sinks_.begin();
	while (it != sinks_.end()) {
		PulseDevicePtr device = *it;
		if(device->name() == name) {
			result = device;
		}
		++it;
	}
	it = sources_.begin();
	while (it != sources_.end()) {
		PulseDevicePtr device = *it;
		if (device->name() == name) {
			result = device;
		}
		++it;
	}
	return result;
}

const std::string PulseCore::getDeviceNameByIndex(int index)
{
	return getDeviceByIndex(index)->name();
}

const std::string PulseCore::defaultSink()
{
	return getDefaultSink()->name();
}

void PulseCore::setVolume_(PulseDevicePtr device, int value)
{
	pa_cvolume* new_cvolume = pa_cvolume_set(&device->volume,
						 device->volume.channels,
						 (pa_volume_t) device->round(MAX(((double)value * PA_VOLUME_NORM) / 100, 0.0))
						 );
	pa_operation* op;
	if (device->type() == SINK) {
		op = pa_context_set_sink_volume_by_index(context_, device->index(), new_cvolume, success_cb, NULL);
	}
	else {
		op = pa_context_set_source_volume_by_index(context_, device->index(), new_cvolume, success_cb, NULL);
	}
	iterate(op);
	pa_operation_unref(op);
}

void PulseCore::setMute_(PulseDevicePtr device, bool mute)
{
	pa_operation* op;
	if (device->type() == SINK) {
		op = pa_context_set_sink_mute_by_index(context_, device->index(), (int) mute, success_cb, NULL);
	}
	else {
		op = pa_context_set_source_mute_by_index(context_, device->index(), (int) mute, success_cb, NULL);
	}
	iterate(op);
	pa_operation_unref(op);
}

void PulseCore::onError(const std::string &message)
{
	std::cerr << "Error: " << message << std::endl;
}

void PulseCore::setCurrentDevice(const std::string &name)
{
	currentDevice_ = getDeviceByName(name);
}

void PulseCore::setVolume(int value)
{
	setVolume_(currentDevice_, value);
}

void PulseCore::setMute(bool mute)
{
	setMute_(currentDevice_, !mute);
}

void PulseCore::updateDevices()
{
	if (!sinks_.empty()) {
		sinks_.clear();
	}
	if (!sources_.empty()) {
		sources_.clear();
	}
	if (!sinksDescriptions_.empty()) {
		sinksDescriptions_.clear();
	}
	if(!sourcesDescriptions_.empty()) {
		sourcesDescriptions_.clear();
	}
	if(!devicesNames_.empty()) {
		devicesNames_.clear();
	}
	if(!devicesDescs_.empty()) {
		devicesDescs_.clear();
	}
	getSinks();
	getSources();
	PulseDevicePtrList::iterator it = sinks_.begin();
	while(it != sinks_.end()) {
		PulseDevicePtr device = *it;
		devicesNames_.push_back(device->name());
		sinksDescriptions_.push_back(device->description());
		++it;
	}
	PulseDevicePtrList::iterator it1 = sources_.begin();
	while(it1 != sources_.end()) {
		PulseDevicePtr device = *it1;
		devicesNames_.push_back(device->name());
		sourcesDescriptions_.push_back(device->description());
		++it1;
	}
	devicesDescs_.insert(devicesDescs_.end(), sinksDescriptions_.begin(), sinksDescriptions_.end());
	devicesDescs_.insert(devicesDescs_.end(), sourcesDescriptions_.begin(), sourcesDescriptions_.end());
}

int PulseCore::getVolume() const
{
	return currentDevice_->volume_percent();
}

bool PulseCore::getMute()
{
	return currentDevice_->mute();
}

const std::vector<std::string> &PulseCore::getCardList() const
{
	return devicesDescs_;
}

const std::vector<std::string> &PulseCore::getCardNames() const
{
	return devicesNames_;
}

PulseDevicePtr PulseCore::getDeviceByIndex(int index)
{
	PulseDevicePtr device = getDefaultSink();
	const int sinksSize = sinksDescriptions_.size();
	const int sourcesSize = sourcesDescriptions_.size();
	if (index >=0 && index < (sinksSize + sourcesSize)) {
		const int deltaIndex = sinksSize - index;
		device = (deltaIndex != 0 && deltaIndex > 0) ? getSink(index) : getSource(abs(deltaIndex));
	}
	return device;
}

int PulseCore::getCurrentDeviceIndex()
{
	const int sinksSize = sinksDescriptions_.size();
	const int absIndex = currentDevice_->index();
	const int result = (currentDevice_->type() == SINK) ? absIndex : (sinksSize + absIndex);
	return result;
}

bool PulseCore::deviceNameExists(const std::string &name)
{
	return Tools::itemExists(devicesNames_, name);
}

bool PulseCore::deviceDescriptionExists(const std::string &description)
{
	return Tools::itemExists(devicesDescs_, description);
}
