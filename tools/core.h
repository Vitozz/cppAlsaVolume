/*
 * core.h
 * Copyright (C) 2013 Vitaly Tonkacheyev
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

#ifndef CORE_H
#define CORE_H

#include "settings.h"
#include "../alsawork/alsawork.h"
#include "../gui/settingsframe.h"
#include "tools.h"
#include "settingsstr.h"
#include <vector>

class Core
{
public:
	Core(const Glib::RefPtr<Gtk::Builder>& refGlade);
	~Core();
	void runAboutDialog();
	std::string getSoundCardName() const;
	std::string getActiveMixer() const;
	void setActiveCard(int card);
	void setActiveMixer(int index);
	void saveSettings();
	void runSettings();
	std::vector<std::string> &getMixers();
	std::vector<std::string> &getCardsList();
	void switchChanged (const std::string& name, int id, bool enabled);
	void soundMuted(bool mute);
	bool getMuted(const std::string &mixer);
	void onExtMixerSignal();
	double getVolumeValue() const;
	void onTrayIconScroll(double value);
	void onVolumeSlider(double value);
	void onSettingsDialogOk(settingsStr &str);
	void onSettingsDialogAutostart(bool isAutorun);

	typedef sigc::signal<void, double> type_double_signal;
	type_double_signal signal_volume_changed();
	typedef sigc::signal<void, double, std::string, std::string> type_volumevalue_signal;
	type_volumevalue_signal signal_value_changed();
	typedef sigc::signal<void, bool> type_bool_signal;
	type_bool_signal signal_mixer_muted();

private:
	type_double_signal m_signal_volume_changed;
	type_volumevalue_signal m_signal_value_changed;
	type_bool_signal m_signal_mixer_muted;
	void updateControls(int cardId);
	void updateSettings(int cardId);

private:
	Settings *settings_;
	AlsaWork *alsaWork_;
	settingsStr *settingsStr_;
	std::string mixerName_;
	double volumeValue_;
	SettingsFrame *settingsDialog_;
};

#endif // CORE_H
