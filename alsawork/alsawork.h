/*
 * alsawork.h
 * Copyright (C) 2012-2025 Vitaly Tonkacheyev
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

#ifndef ALSAWORK_H
#define ALSAWORK_H

#include "mixerswitches.h"
#include "alsadevice.h"
#include <vector>
#include <memory>

typedef std::vector<AlsaDevice::Ptr> AlsaDevicePtrList;

class AlsaWork
{
public:
    AlsaWork();
    AlsaWork(AlsaWork const &);
    ~AlsaWork();
    typedef std::shared_ptr<AlsaWork> Ptr;
    void setCurrentCard(int cardId);
    void setCurrentMixer(const std::string &mixer);
    void setCurrentMixer(int id);
    void setAlsaVolume(double volume);
    double getAlsaVolume() const;
    static std::string getCardName(int index);
    std::string getMixerName(int index);
    std::string getCurrentMixerName() const;
    const std::vector<std::string> &getCardsList() const;
    const std::vector<std::string> &getVolumeMixers() const;
    MixerSwitches::Ptr getSwitchList() const;
    void setSwitch(const std::string& mixer, int id, bool enabled);
    void setMute(bool enabled);
    bool getMute();
    bool cardExists(int id);
    int getFirstCardWithMixers();

private:
    static int getTotalCards();
    static void checkError (int errorIndex);
    void getCards();

private:
    std::vector<std::string> cardList_;
    int totalCards_;
    AlsaDevice::Ptr currentAlsaDevice_;
    AlsaDevicePtrList devices_;
};
#endif // ALSAWORK_H
