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

#ifndef PULSECORE_H
#define PULSECORE_H

#include "pulsedevice.h"
#include "pulse/pulseaudio.h"
#include <vector>

struct ServerInfo {
	std::string defaultSourceName;
	std::string defaultSinkName;
};
enum state {
	CONNECTING,
	CONNECTED,
	ERROR
};

class PulseCore
{
public:
	PulseCore(const char *clientName);
	~PulseCore();
	PulseCore(PulseCore const &);
	state pState;
	std::string defaultSink();
	const std::vector<std::string> &getCardList() const;
	const std::vector<std::string> &getCardNames() const;
	std::string getDeviceDescription(const std::string &name);
	std::string getDeviceNameByIndex(int index);
	int getCurrentDeviceIndex();
	int getVolume();
	bool getMute();
	bool deviceNameExists(const std::string &name);
	bool deviceDescriptionExists(const std::string &description);
	void setVolume(int value);
	void setMute(bool mute);
	void setCurrentDevice(const std::string &name);
private:
	std::vector<PulseDevice> getSinks();
	std::vector<PulseDevice> getSources();
	PulseDevice getSink(u_int32_t);
	PulseDevice getSink(const std::string &name);
	PulseDevice getSource(u_int32_t);
	PulseDevice getSource(const std::string &name);
	PulseDevice getDefaultSink();
	PulseDevice getDefaultSource();
	PulseDevice getDeviceByName(const std::string &name);
	PulseDevice getDeviceByIndex(int index);
	const std::vector<std::string> &getSinksDescriptions() const;
	const std::vector<std::string> &getSourcesDescriptions() const;
	void setVolume_(PulseDevice &device, int value);
	void setMute_(PulseDevice &device, bool mute);
	void iterate(pa_operation* op);
	void onError(const std::string &message);
	void updateDevices();
private:
	pa_mainloop* mainLoop_;
	pa_mainloop_api* mainLoopApi_;
	pa_context* context_;
	int retval_;
	std::vector<PulseDevice> sources_;
	std::vector<PulseDevice> sinks_;
	PulseDevice *currentDevice_;
	std::vector<std::string> sinksDescriptions_;
	std::vector<std::string> sourcesDescriptions_;
	std::vector<std::string> devicesNames_;
	std::vector<std::string> devicesDescs_;
};

#endif // PULSECORE_H
