/*
 * trayicon.cpp
 * Copyright (C) 2012-2015 Vitaly Tonkacheyev
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
#include "libintl.h"
#include <iostream>
#ifdef IS_GTK_2
#define GDK_BUTTON_MIDDLE 2
#define GDK_BUTTON_PRIMARY 1
#else
#ifdef USE_APPINDICATOR
#include "gdkmm/devicemanager.h"
#endif
#endif
#define _(String) gettext(String)
#define N_(String) gettext_noop (String)
#define MUTEITEM _("Mute")
#define SETTSITEM _("Settings")
#define ABOUTITEM _("About")
#define QUITITEM _("Quit")
#define CARDL _("Card: ")
#define VOLUMEL _("Volume: ")
#define MIXERL _("Mixer: ")
#define RESTOREITEM _("Restore")

const int OFFSET = 2;
const std::string ICON_PREFIX = "tb_icon";

TrayIcon::TrayIcon(double volume, const std::string &cardName, const std::string &mixerName, bool muted)
: volumeValue_(volume),
  cardName_(cardName),
  mixerName_(mixerName),
  muted_(muted),
  menu_(Gtk::manage(new Gtk::Menu())),
  restoreItem_(Gtk::manage(new Gtk::MenuItem(RESTOREITEM))),
  settingsItem_(Gtk::manage(new Gtk::MenuItem(SETTSITEM))),
  aboutItem_(Gtk::manage(new Gtk::MenuItem(ABOUTITEM))),
  quitItem_(Gtk::manage(new Gtk::MenuItem(QUITITEM))),
  muteItem_(Gtk::manage(new Gtk::CheckMenuItem(MUTEITEM))),
  mouseX_(0),
  mouseY_(0),
  pixbufWidth_(0),
  pixbufHeight_(0),
  isLegacyIcon_(true),
#ifdef USE_APPINDICATOR
  newIcon_(0),
#endif
  legacyIcon_(0)
{
	const Glib::ustring searchPath = Glib::ustring("icons/") + getIconName(100);
	const Glib::ustring iconPath = Tools::getResPath(searchPath.c_str());
#ifdef USE_APPINDICATOR
	newIcon_ = std::shared_ptr<AppIndicator>(app_indicator_new("AlsaVolume",
								   iconPath.c_str(),
								   APP_INDICATOR_CATEGORY_APPLICATION_STATUS));
	if (newIcon_) {
		isLegacyIcon_ = false;
	}
	app_indicator_set_status(newIcon_.get(), APP_INDICATOR_STATUS_ACTIVE);
	app_indicator_set_menu(newIcon_.get(), menu_->gobj());
	app_indicator_set_secondary_activate_target(newIcon_.get(), GTK_WIDGET(muteItem_->gobj()));
	g_signal_connect(newIcon_.get(), "scroll-event", (GCallback)TrayIcon::onScrollEventAI, this);
#endif
	if(isLegacyIcon_) {
		legacyIcon_ = Gtk::StatusIcon::create(iconPath);
		//Staus icon signals
		legacyIcon_->signal_popup_menu().connect(sigc::mem_fun(*this, &TrayIcon::onPopup));
		legacyIcon_->signal_activate().connect(sigc::mem_fun(*this, &TrayIcon::onHideRestore));
		legacyIcon_->signal_scroll_event().connect(sigc::mem_fun(*this, &TrayIcon::onScrollEvent));
		legacyIcon_->signal_button_press_event().connect(sigc::mem_fun(*this, &TrayIcon::onButtonClick));
		//
	}
	Gtk::SeparatorMenuItem *separator2 = Gtk::manage(new Gtk::SeparatorMenuItem());
	settingsItem_->signal_activate().connect(sigc::mem_fun(*this, &TrayIcon::runSettings));
	if (!isLegacyIcon_) {
		restoreItem_->signal_activate().connect(sigc::mem_fun(*this, &TrayIcon::onHideRestore));
		menu_->append(*Gtk::manage(restoreItem_));
	}
	menu_->append(*Gtk::manage(settingsItem_));
	muteItem_->signal_toggled().connect(sigc::mem_fun(*this, &TrayIcon::onMute));
	menu_->append(*Gtk::manage(muteItem_));
	aboutItem_->signal_activate().connect(sigc::mem_fun(*this, &TrayIcon::onAbout));
	menu_->append(*Gtk::manage(aboutItem_));
	menu_->append(*Gtk::manage(separator2));
	quitItem_->signal_activate().connect(sigc::mem_fun(*this, &TrayIcon::onQuit));
	menu_->append(*Gtk::manage(quitItem_));
	menu_->show_all_children();
	on_signal_volume_changed(volumeValue_, cardName, mixerName);
	muteItem_->set_active(muted_);
}
#ifdef USE_APPINDICATOR
void TrayIcon::onScrollEventAI(AppIndicator *ai, gint steps, gint direction, TrayIcon *userdata)
{
	(void) ai;
	(void) steps;
	double value = 0.0;
	if (direction == GDK_SCROLL_UP) {
		value+=OFFSET;
	}
	else if(direction == GDK_SCROLL_DOWN) {
		value-=OFFSET;
	}
	userdata->m_signal_value_changed(value);
}
#endif
void TrayIcon::onHideRestore()
{
	Glib::RefPtr<Gdk::Screen> screen;
	Gdk::Rectangle area;
	Gtk::Orientation orientation;
	iconPosition pos;
	if (isLegacyIcon_) {
		if (legacyIcon_->get_geometry(screen, area, orientation)) {
			pos.iconX_ = area.get_x();
			pos.iconY_ = area.get_y();
			const int areaHeight = area.get_height();
			const int areaWidth = area.get_width();
			pos.iconHeight_ = (areaHeight > 0) ? areaHeight : pixbufHeight_;
			pos.iconWidth_ = (areaWidth > 0) ? areaWidth : pixbufWidth_;
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
#ifdef USE_APPINDICATOR
	else {
		screen = restoreItem_->get_screen();
//Dirty hack to obtain mouse position
#ifdef IS_GTK_2
		Glib::RefPtr<Gdk::Display> display = restoreItem_->get_display();
		Gdk::ModifierType type;
		display->get_pointer(pos.iconX_, pos.iconY_,type);
#else
		Glib::RefPtr<Gdk::Display> display = restoreItem_->get_display();
		Glib::RefPtr<Gdk::DeviceManager> manager = display->get_device_manager();
		std::vector<Glib::RefPtr<Gdk::Device> >list = manager->list_devices(Gdk::DEVICE_TYPE_MASTER);
		std::for_each(list.begin(),
			      list.end(),
			      [&](Glib::RefPtr<Gdk::Device> item){
									item->get_position(pos.iconX_, pos.iconY_);
								 });
#ifdef IS_DEBUG
		std::cout << "X=" << pos.iconX_ << " Y=" << pos.iconY_ << std::endl;
#endif
#endif
//
		pos.screenHeight_ = screen->get_height();
		pos.screenWidth_ = screen->get_width();
		pos.trayAtTop_ = bool(pos.iconY_ < pos.screenHeight_/2);
		pos.iconHeight_ = pixbufHeight_;
		pos.iconWidth_ = pixbufWidth_;
		pos.geometryAvailable_ = false;
		m_signal_on_restore(pos);
	}
#endif
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
	int number = 100;
	Glib::ustring iconPath = Glib::ustring::compose("%1%2.png",ICON_PREFIX, Glib::ustring::format(number));
	value = (value <= 0) ? 0 : (value > 100) ? 100 : value;
	number = (value < 20) ? 20 : floor(value/20+0.5)*20;
	if (value <= 0 || muted_) {
		number = 0;
	}
	iconPath = Glib::ustring::compose("%1%2.png",ICON_PREFIX, Glib::ustring::format(number));
#ifdef IS_DEBUG
	std::cout << iconPath << std::endl;
#endif
	return iconPath;
}

void TrayIcon::setIcon(double value)
{
	const Glib::ustring searchPath = Glib::ustring("icons/") + getIconName(value);
	const Glib::ustring iconPath = Tools::getResPath(searchPath.c_str());
	if (!iconPath.empty()) {
		const Glib::RefPtr<Gdk::Pixbuf> pixbuf = Glib::RefPtr<Gdk::Pixbuf>(Gdk::Pixbuf::create_from_file(iconPath));
		pixbufWidth_ = pixbuf->get_width();
		pixbufHeight_ = pixbuf->get_height() + 4;
		try {
			if(isLegacyIcon_) {
				legacyIcon_->set(pixbuf);
			}
#ifdef USE_APPINDICATOR
			else {
				app_indicator_set_icon_full(newIcon_.get(), iconPath.c_str(), "VolumeIcon");
			}
#endif
		}
		catch (Glib::FileError &err) {
			std::cerr << "FileError::trayicon.cpp::157:: " << err.what() << std::endl;
			exit(1);
		}
	}
}

void TrayIcon::setTooltip(const Glib::ustring &message)
{
	if (!message.empty())
		if (isLegacyIcon_) {
			legacyIcon_->set_tooltip_text(message);
		}
#ifdef USE_APPINDICATOR
		else {
			app_indicator_set_title(newIcon_.get(), message.c_str());
		}
#endif
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
	legacyIcon_->popup_menu_at_position(*menu_, button, activate_time);
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
#ifdef IS_DEBUG
		std::cout << "Pressed" << std::endl;
#endif
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
	const Glib::ustring tip = (!mixerName.empty()) ? Glib::ustring(CARDL)
							 + Glib::ustring(cardName_)
							 + Glib::ustring("\n")
							 + Glib::ustring(MIXERL)
							 + Glib::ustring(mixerName_)
							 + Glib::ustring("\n")
							 + Glib::ustring(VOLUMEL)
							 + Glib::ustring::format(volumeValue_,"%")
							 : Glib::ustring(CARDL)
							 + Glib::ustring(cardName_)
							 + Glib::ustring("\n")
							 + Glib::ustring(VOLUMEL)
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
