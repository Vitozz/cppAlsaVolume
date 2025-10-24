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

#ifndef PULSECORE_H
#define PULSECORE_H

#include "pulsedevice.h"
#include <pulse/mainloop.h>
#include <pulse/mainloop-api.h>
#include <pulse/context.h>
#include <vector>
#include <memory>

struct ServerInfo {
    std::string defaultSourceName;
    std::string defaultSinkName;
};
enum state {
    CONNECTING,
    CONNECTED,
    ERROR
};

typedef std::vector<PulseDevice::Ptr> PulseDevicePtrList;

class PulseCore
{
public:
    explicit PulseCore(const char *clientName);
    ~PulseCore();
    PulseCore(PulseCore const &);
    typedef std::shared_ptr<PulseCore> Ptr;
    state pState;
    std::string defaultSink();
    const std::vector<std::string> &getCardList() const;
    const std::vector<std::string> &getCardNames() const;
    std::string getDeviceDescription(const std::string &name);
    std::string getDeviceNameByIndex(int index);
    int getCurrentDeviceIndex();
    int getVolume();
    int getCardIndex();
    bool getMute();
    bool deviceNameExists(const std::string &name);
    bool deviceDescriptionExists(const std::string &description);
    void setVolume(int value);
    void setMute(bool mute);
    void setCurrentDevice(const std::string &name);
    bool available();
    void refreshDevices();
private:
    void getSinks();
    void getSources();
    PulseDevice::Ptr getSink(u_int32_t);
    PulseDevice::Ptr getSink(const std::string &name);
    PulseDevice::Ptr getSource(u_int32_t);
    PulseDevice::Ptr getSource(const std::string &name);
    PulseDevice::Ptr getDefaultSink();
    PulseDevice::Ptr getDefaultSource();
    PulseDevice::Ptr getDeviceByName(const std::string &name);
    PulseDevice::Ptr getDeviceByIndex(int index);
    const std::vector<std::string> &getSinksDescriptions() const;
    const std::vector<std::string> &getSourcesDescriptions() const;
    void setVolume_(const PulseDevice::Ptr &device, int value);
    void setMute_(const PulseDevice::Ptr &device, bool mute);
    void iterate(pa_operation* op);
    static inline void onError(const std::string &message);
    void updateDevices();
    void clearLists();
private:
    pa_mainloop* mainLoop_;
    pa_mainloop_api* mainLoopApi_;
    pa_context* context_;
    int retval_;
    PulseDevicePtrList sources_;
    PulseDevicePtrList sinks_;
    std::string currentDeviceName_;
    int currentDeviceIndex_;
    std::vector<std::string> sinksDescriptions_;
    std::vector<std::string> sourcesDescriptions_;
    std::vector<std::string> devicesNames_;
    std::vector<std::string> devicesDescs_;
    bool isAvailable_;
};

#endif // PULSECORE_H
