/*
 * trayicon.cpp
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

#include "trayicon.h"
#include "../tools/tools.h"
#include "gtkmm/separatormenuitem.h"
#include "gtkmm/stock.h"
#include "glibmm.h"
#include <iostream>

const int OFFSET = 2;
const std::string ICON_PREFIX = "tb_icon";

TrayIcon::TrayIcon(double volume, const std::string &cardName, const std::string &mixerName, bool muted)
: volumeValue_(volume),
  cardName_(cardName),
  mixerName_(mixerName),
  muted_(muted),
  menu_(Gtk::manage(new Gtk::Menu())),
  restoreItem_(Gtk::manage(new Gtk::ImageMenuItem(Gtk::Stock::GO_UP))),
  settingsItem_(Gtk::manage(new Gtk::ImageMenuItem(Gtk::Stock::PREFERENCES))),
  aboutItem_(Gtk::manage(new Gtk::ImageMenuItem(Gtk::Stock::ABOUT))),
  quitItem_(Gtk::manage(new Gtk::ImageMenuItem(Gtk::Stock::QUIT))),
  muteItem_(Gtk::manage(new Gtk::CheckMenuItem("Mute"))),
  mouseX_(0),
  mouseY_(0),
  pixbufWidth_(0)
{
	restoreItem_->signal_activate().connect(sigc::mem_fun(*this, &TrayIcon::onHideRestore));
	menu_->append(*Gtk::manage(restoreItem_));
	restoreItem_->set_label("Restore");
	Gtk::SeparatorMenuItem *separator1 = Gtk::manage(new Gtk::SeparatorMenuItem());
	Gtk::SeparatorMenuItem *separator2 = Gtk::manage(new Gtk::SeparatorMenuItem());
	menu_->append(*Gtk::manage(separator1));
	settingsItem_->signal_activate().connect(sigc::mem_fun(*this, &TrayIcon::runSettings));
	menu_->append(*Gtk::manage(settingsItem_));
	muteItem_->signal_toggled().connect(sigc::mem_fun(*this, &TrayIcon::onMute));
	menu_->append(*Gtk::manage(muteItem_));
	aboutItem_->signal_activate().connect(sigc::mem_fun(*this, &TrayIcon::onAbout));
	menu_->append(*Gtk::manage(aboutItem_));
	menu_->append(*Gtk::manage(separator2));
	quitItem_->signal_activate().connect(sigc::mem_fun(*this, &TrayIcon::onQuit));
	menu_->append(*Gtk::manage(quitItem_));
	menu_->show_all_children();
	//Staus icon signals
	signal_popup_menu().connect(sigc::mem_fun(*this, &TrayIcon::onPopup));
	signal_activate().connect(sigc::mem_fun(*this, &TrayIcon::onHideRestore));
	signal_scroll_event().connect(sigc::mem_fun(*this, &TrayIcon::onScrollEvent));
	signal_button_press_event().connect(sigc::mem_fun(*this, &TrayIcon::onButtonClick));
	//
	on_signal_volume_changed(volumeValue_, cardName, mixerName);
	muteItem_->set_active(muted_);
}

void TrayIcon::onHideRestore()
{
	Glib::RefPtr<Gdk::Screen> screen;
	Gdk::Rectangle area;
	Gtk::Orientation orientation;
	iconPosition pos;
	if (get_geometry(screen, area, orientation)) {
		pos.iconX_ = area.get_x();
		pos.iconY_ = area.get_y();
		pos.iconHeight_ = area.get_height();
		pos.iconWidth_ = pixbufWidth_;
		pos.screenHeight_ = screen->get_height();
		pos.screenWidth_ = screen->get_width();
		pos.geometryAvailable_ = bool(pos.iconX_ > 0 || pos.iconY_ > 0);
		if (!pos.geometryAvailable_) {
			pos.iconX_ = mouseX_;
			pos.iconY_ = mouseY_;
		}
		pos.trayAtTop_ = bool(pos.iconY_ < pos.screenHeight_/2);
		m_signal_on_restore(pos);
	}
}

void TrayIcon::onQuit()
{
	m_signal_save_settings();
	exit(0);
}

void TrayIcon::runSettings()
{
	m_signal_ask_settings();
}

void TrayIcon::onAbout()
{
	m_signal_ask_dialog();
}

void TrayIcon::onMute()
{
	muted_ = muteItem_->get_active();
	if (muted_) {
		setIcon(0);
	}
	else {
		setIcon(volumeValue_);
	}
	m_signal_on_mute(!muted_);
}

Glib::ustring TrayIcon::getIconName(double value) const
{
	Glib::ustring iconPath;
	if (value <= 0) {
		iconPath = Glib::ustring(ICON_PREFIX + "0.png");
	}
	if (value >0 && (value < 40)) {
		iconPath = Glib::ustring(ICON_PREFIX + "20.png");
	}
	if (value >=40 && (value < 60)) {
		iconPath = Glib::ustring(ICON_PREFIX + "40.png");
	}
	if (value >=60 && (value < 80)) {
		iconPath = Glib::ustring(ICON_PREFIX + "60.png");
	}
	if (value >=80 && (value < 100)) {
		iconPath = Glib::ustring(ICON_PREFIX + "80.png");
	}
	if (value >= 100) {
		iconPath = Glib::ustring(ICON_PREFIX + "100.png");
	}
	return iconPath;
}

void TrayIcon::setIcon(double value)
{
	const Glib::ustring searchPath = Glib::ustring("icons/") + getIconName(value);
	const Glib::ustring iconPath = Tools::getResPath(searchPath.c_str());
	if (!iconPath.empty()) {
		const Glib::RefPtr<Gdk::Pixbuf> pixbuf = Glib::RefPtr<Gdk::Pixbuf>(Gdk::Pixbuf::create_from_file(iconPath));
		pixbufWidth_ = pixbuf->get_width();
		try {
			this->set(pixbuf);
		}
		catch (Glib::FileError &err) {
			std::cerr << "FileError::trayicon.cpp::161:: " << err.what() << std::endl;
		}
	}
}

void TrayIcon::setTooltip(const Glib::ustring &message)
{
	if (!message.empty())
		set_tooltip_text(message);
}

void TrayIcon::setMuted(bool isit)
{
	if (isit) {
		setIcon(0);
	}
	muteItem_->set_active(isit);
	muted_ = isit;

}
void TrayIcon::onPopup(guint button, guint32 activate_time)
{
	popup_menu_at_position(*menu_, button, activate_time);
}

bool TrayIcon::onScrollEvent(GdkEventScroll* event)
{
	double value = 0.0;
	if (event->direction == GDK_SCROLL_UP) {
		value+=OFFSET;
	}
	else if(event->direction == GDK_SCROLL_DOWN) {
		value-=OFFSET;
	}
	m_signal_value_changed(value);
	return false;
}

bool TrayIcon::onButtonClick(GdkEventButton* event)
{
	if (event->button == GDK_BUTTON_MIDDLE) {
		muteItem_->set_active(!muteItem_->get_active());
		onMute();
	}
	if (event->button == GDK_BUTTON_PRIMARY) {
		setMousePos(event->x_root, event->y_root);
	}
	return false;
}

void TrayIcon::setMousePos(const int X, const int Y)
{
	mouseX_ = X;
	mouseY_ = Y;
}

void TrayIcon::on_signal_volume_changed(double volume, const std::string &cardName, const std::string &mixerName)
{
	volumeValue_ = volume;
	cardName_ = cardName;
	mixerName_ = mixerName;
	if (!muted_) {
		setIcon(volumeValue_);
	}
	else {
		setIcon(0);
	}
	const Glib::ustring tip = Glib::ustring("Card: ")
				  + Glib::ustring(cardName_)
				  + Glib::ustring("\n")
				  + Glib::ustring(mixerName_)
				  + Glib::ustring(" - ")
				  + Glib::ustring::format(volumeValue_,"%");
	setTooltip(tip);
}

TrayIcon::type_trayicon_simple_signal TrayIcon::signal_ask_dialog()
{
	return m_signal_ask_dialog;
}

TrayIcon::type_trayicon_simple_signal TrayIcon::signal_ask_settings()
{
	return m_signal_ask_settings;
}

TrayIcon::type_trayicon_simple_signal TrayIcon::signal_save_settings()
{
	return m_signal_save_settings;
}

TrayIcon::type_trayicon_4int_signal TrayIcon::signal_on_restore()
{
	return m_signal_on_restore;
}

TrayIcon::type_trayicon_double_signal TrayIcon::signal_value_changed()
{
	return m_signal_value_changed;
}

TrayIcon::type_trayicon_bool_signal TrayIcon::signal_on_mute()
{
	return m_signal_on_mute;
}
