/*
 * sliderwindow.h
 * Copyright (C) 2012 Vitaly Tonkacheyev
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

#ifndef SLIDERWINDOW_H
#define SLIDERWINDOW_H

#include "gtkmm/button.h"
#include "gtkmm/window.h"
#include "gtkmm/builder.h"
#include "gtkmm/scale.h"
#include "settings.h"
#include "alsawork.h"
#include "tools.h"
#include <vector>

class SliderWindow : public Gtk::Window
{
public:
	SliderWindow(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder>&refGlade);
	virtual ~SliderWindow();
	void runAboutDialog();
	void setWindowPosition(int x_, int y_, int height_, int width_);
	bool getVisible();
	void showWindow();
	void hideWindow();
	int getHeight() const;
	int getWidth() const;
	void setVolumeValue(double value);
	double getVolumeValue();
	std::string getSoundCardName() const;
	std::string getActiveMixer() const;
	void setActiveCard(int card);
	void setActiveMixer(int index);
	void saveSettings();
	void runSettings();
	std::vector<std::string> getMixers();
	std::vector<std::string> getCardsList();
	void switchChanged (const std::string& name, int id, bool enabled);
	void soundMuted(bool mute);
	bool getMuted();
	//signal
	typedef sigc::signal<void, double, std::string, std::string> type_sliderwindow_signal;
	type_sliderwindow_signal signal_volume_changed();

protected:
	void on_volume_slider();
	bool on_focus_out(GdkEventCrossing* event);
	void onSettingsDialogOk(settingsStr str);
	void onSettingsDialogAutostart(bool isAutorun);
	//signal
	type_sliderwindow_signal m_signal_volume_changed;

private:
	void updateControls(int cardId);
	void createSettingsDialog();

private:
	Glib::RefPtr<Gtk::Builder> builder_;
	Gtk::Scale *volumeSlider_;
	double volumeValue_;
	Settings *settings_;
	AlsaWork *alsaWork_;
	std::vector<std::string> cardList_;
	std::vector<std::string> mixerList_;
	MixerSwitches switches_;
	int cardId_, mixerId_;
	Glib::ustring mixerName_;
	bool orient_;
};

#endif // SLIDERWINDOW_H
