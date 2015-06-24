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

#include "pulsedevice.h"
#include <cmath>

#define ALSA_CARDID_PROPERTY "alsa.card"

static uint32_t getCardId(pa_proplist *pl)
{
	const char* cardId = pa_proplist_gets(pl, ALSA_CARDID_PROPERTY);
	return (cardId != NULL) ? std::stoi(std::string(cardId)) : 0;
}

PulseDevice::PulseDevice()
: index_(0),
  card_(0),
  type_(SINK),
  name_(std::string()),
  description_(std::string()),
  mute_(false)
{
}

PulseDevice::PulseDevice(const pa_source_info* i)
: index_(i->index),
  card_(getCardId(i->proplist)),
  type_(SOURCE),
  name_(std::string(i->name)),
  description_(std::string(i->description))
{
	volume.channels = i->volume.channels;
	int n;
	for (n = 0; n < volume.channels; ++n) {
		volume.values[n] = i->volume.values[n];
	}
	mute_ = i->mute == 1;
}

PulseDevice::PulseDevice(const pa_sink_info* i)
: index_(i->index),
  card_(getCardId(i->proplist)),
  type_(SINK),
  name_(std::string(i->name)),
  description_(std::string(i->description))
{
	volume.channels = i->volume.channels;
	int n;
	for (n = 0; n < volume.channels; ++n) {
		volume.values[n] = i->volume.values[n];
	}
	mute_ = i->mute == 1;
}

int PulseDevice::percent(pa_cvolume& volume_) const
{
	return (int) round(((double) pa_cvolume_avg(&volume_) * 100.) / PA_VOLUME_NORM);
}

double PulseDevice::round(double value) const
{
	return (value > 0.0) ? floor(value + 0.5) : ceil(value - 0.5);
}

uint32_t PulseDevice::index() const
{
	return index_;
}

uint32_t PulseDevice::card() const
{
	return card_;
}

device_type PulseDevice::type() const
{
	return type_;
}

const std::string &PulseDevice::name() const
{
	return name_;
}

const std::string &PulseDevice::description() const
{
	return description_;
}

int PulseDevice::volume_percent()
{
	return percent(volume);
}

bool PulseDevice::mute() const
{
	return mute_;
}

