/*
 * trayicon.h
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

#ifndef TRAYICON_H
#define TRAYICON_H

#include "gtkmm/statusicon.h"
#include "gtkmm/menu.h"
#include "gtkmm/menuitem.h"
#include "gtkmm/imagemenuitem.h"
#include "gtkmm/checkmenuitem.h"
#include "gtkmm/aboutdialog.h"

class TrayIcon : public Gtk::StatusIcon
{
public:
	TrayIcon(double volume, const std::string &cardName, const std::string &mixerName, bool muted);
	void on_signal_volume_changed(double volume, const std::string &cardName, const std::string &mixerName);
	void setMuted(bool isit);
	//signals
	typedef sigc::signal<void> type_trayicon_simple_signal;
	type_trayicon_simple_signal signal_ask_dialog();
	type_trayicon_simple_signal signal_ask_settings();
	type_trayicon_simple_signal signal_save_settings();
	type_trayicon_simple_signal signal_ask_extmixer();
	typedef sigc::signal<void, int, int, int ,int> type_trayicon_4int_signal;
	type_trayicon_4int_signal signal_on_restore();
	typedef sigc::signal<void, double> type_trayicon_double_signal;
	type_trayicon_double_signal signal_value_changed();
	typedef sigc::signal<void, double> type_trayicon_bool_signal;
	type_trayicon_bool_signal signal_on_mute();

protected:
	//menu actions
	void onHideRestore();
	void runMixerApp();
	void runSettings();
	void onMute();
	void onAbout();
	void onQuit();
	//icon actions
	void onPopup(guint button, guint32 activate_time);
	bool onScrollEvent(GdkEventScroll *event);
	bool onButtonClick(GdkEventButton *event);
	//signals
	type_trayicon_simple_signal m_signal_ask_dialog;
	type_trayicon_simple_signal m_signal_ask_settings;
	type_trayicon_simple_signal m_signal_save_settings;
	type_trayicon_4int_signal m_signal_on_restore;
	type_trayicon_double_signal m_signal_value_changed;
	type_trayicon_bool_signal m_signal_on_mute;
	type_trayicon_simple_signal m_signal_ask_extmixer;

private:
	void setIcon(double value);
	void setTooltip(const Glib::ustring &message);
	Glib::ustring getIconName(double value) const;

private:
	double volumeValue_;
	std::string cardName_;
	std::string mixerName_;
	bool muted_;
	Gtk::Menu *menu_;
	Gtk::ImageMenuItem *restoreItem_, *mixerItem_, *settingsItem_, *aboutItem_, *quitItem_;
	Gtk::CheckMenuItem* muteItem_;

};

#endif // TRAYICON_H
