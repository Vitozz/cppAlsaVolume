/*
 * trayicon.h
 * Copyright (C) 2012-2019 Vitaly Tonkacheyev
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

#ifndef TRAYICON_H
#define TRAYICON_H

#include "gtkmm/statusicon.h"
#include "gtkmm/menu.h"
#include "gtkmm/menuitem.h"
#include "gtkmm/imagemenuitem.h"
#include "gtkmm/checkmenuitem.h"
#include "gtkmm/aboutdialog.h"
#include "../tools/tools.h"
#include <memory>

#if defined(USE_APPINDICATOR)
#include "libappindicator/app-indicator.h"
typedef std::shared_ptr<AppIndicator> StatusNotifierPtr;
#elif defined(USE_KDE)
#include "../third-party/statusnotifier/src/statusnotifier.h"
typedef std::shared_ptr<StatusNotifierItem> StatusNotifierPtr;
#endif

class TrayIcon
{
public:
    TrayIcon(double volume, const std::string &cardName, const std::string &mixerName, bool muted);
    typedef std::shared_ptr<TrayIcon> Ptr;
    void on_signal_volume_changed(double volume, const std::string &cardName, const std::string &mixerName);
    void setMuted(bool isit);
    //signals
    typedef sigc::signal<void> type_trayicon_simple_signal;
    type_trayicon_simple_signal signal_ask_dialog();
    type_trayicon_simple_signal signal_ask_settings();
    type_trayicon_simple_signal signal_save_settings();
    type_trayicon_simple_signal signal_ask_extmixer();
    typedef sigc::signal<void, iconPosition> type_trayicon_4int_signal;
    type_trayicon_4int_signal signal_on_restore();
    typedef sigc::signal<void, double> type_trayicon_double_signal;
    type_trayicon_double_signal signal_value_changed();
    typedef sigc::signal<void, double> type_trayicon_bool_signal;
    type_trayicon_bool_signal signal_on_mute();

protected:
    //menu actions
    void onHideRestore();
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

private:
    void setIcon(double value);
    void setTooltip(const Glib::ustring &message);
    Glib::ustring getIconName(double value) const;
    void setMousePos(const int X, const int Y);
#if defined(USE_APPINDICATOR)
    static void onScrollEventAI(AppIndicator *ai, gint steps, gint direction, TrayIcon *userdata);
#elif defined(USE_KDE)
    static void onActivate(StatusNotifierItem * sn, gint x, gint y, TrayIcon *userdata);
    static void onSecondaryActivate(StatusNotifierItem * sn, gint x, gint y, TrayIcon *userdata);
    static void onScroll(StatusNotifierItem * sn, gint delta, StatusNotifierScrollOrientation orient, TrayIcon *userdata);
    static void onContextMenu(StatusNotifierItem * sn, gint x, gint y, TrayIcon *userdata);
    bool checkDBusInterfaceExists(const Glib::ustring &serviceName);
    static void onRegisterError(StatusNotifierItem * sn, GError *error, TrayIcon *userdata);
#endif

private:
    double volumeValue_;
    std::string cardName_;
    std::string mixerName_;
    bool muted_;
    Gtk::Menu *menu_;
    Gtk::MenuItem *restoreItem_, *settingsItem_, *aboutItem_, *quitItem_;
    Gtk::CheckMenuItem* muteItem_;
    int mouseX_;
    int mouseY_;
    int pixbufWidth_;
    int pixbufHeight_;
    bool isLegacyIcon_;
#if defined(USE_APPINDICATOR) || defined(USE_KDE)
    StatusNotifierPtr newIcon_;
#endif
    Glib::RefPtr<Gtk::StatusIcon> legacyIcon_;
};

#endif // TRAYICON_H
