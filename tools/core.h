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
#include "settingsstr.h"
#include <vector>
#ifdef HAVE_PULSE
#include "../pulsework/pulsecore.h"
#endif

class Core
{
public:
	explicit Core(const Glib::RefPtr<Gtk::Builder>& refGlade);
	Core(Core const &);
	~Core();
	void runAboutDialog();
	std::string getSoundCardName() const;
	std::string getActiveMixer() const;
	void setActiveMixer(int index);
	void saveSettings();
	void runSettings();
	void switchChanged (const std::string& name, int id, bool enabled);
	void soundMuted(bool mute);
	bool getMuted();
	void onExtMixerSignal();
	double getVolumeValue() const;
	void onTrayIconScroll(double value);
	void onVolumeSlider(double value);
	void onSettingsDialogOk(const settingsStr::Ptr &str);
	void onSettingsDialogUsePulse(bool isPulse);

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
#ifdef HAVE_PULSE
	void updatePulseDevices(int deviceId);
	void initPulseAudio();
#endif
	void updateSettings(int cardId);
	void mixerChanged(int mixerId);
	void updateTrayIcon(double value);
	void blockAllSignals(bool isblock);
	void errorDialog(const std::string &errorMessage);
	bool onTimeout();

private:
	Settings::Ptr settings_;
	AlsaWork::Ptr alsaWork_;
	settingsStr::Ptr settingsStr_;
	std::string mixerName_;
	double volumeValue_;
	double pollVolume_;
	SettingsFrame *settingsDialog_;
	bool isPulse_;
	bool isMuted_;
	std::vector<std::string> alsaCards_;
	sigc::connection signal_switches_;
	sigc::connection signal_sndcard_;
	sigc::connection signal_mixer_;
	sigc::connection signal_timer_;
#ifdef HAVE_PULSE
	PulseCore::Ptr pulse_;
	std::string pulseDevice_;
	std::string pulseDeviceDesc_;
	std::vector<std::string> pulseDevices_;
	sigc::connection signal_pulsdev_;
	sigc::connection signal_pulsedevices_;

#endif
};

#endif // CORE_H
