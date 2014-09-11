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

#ifndef PULSEDEVICE_H
#define PULSEDEVICE_H

#include "pulse/pulseaudio.h"
#include <iostream>
#ifdef QT_VESRION
#include <QString>
#endif

enum device_type {
	SOURCE,
	SINK
};

class PulseDevice
{
public:
	PulseDevice();
	PulseDevice(const pa_source_info* i_);
	PulseDevice(const pa_sink_info* i);
	uint32_t index() const;
	device_type type() const;
#ifdef QT_VESRION
	const QString &name() const;
	const QString &description() const;
#else
	const std::string &name() const;
	const std::string &description() const;
#endif
	pa_cvolume volume;
	int volume_percent() const;
	bool mute() const;
	double round(double value) const;
private:
	int percent(pa_cvolume& volume) const;
private:
	uint32_t index_;
	device_type type_;
#ifdef QT_VESRION
	QString name_;
	QString description_;
#else
	std::string name_;
	std::string description_;
#endif
	int volume_percent_;
	bool mute_;
};

#endif // PULSEDEVICE_H
