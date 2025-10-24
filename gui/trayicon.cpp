/*
 * trayicon.cpp
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

#include "trayicon.h"
#include "../tools/tools.h"
#include <gtkmm/separatormenuitem.h>
#include <glibmm/fileutils.h>
#include <libintl.h>
#include <iostream>
#ifdef IS_GTK_2
#define GDK_BUTTON_MIDDLE 2
#define GDK_BUTTON_PRIMARY 1
#endif
#include <gdkmm/devicemanager.h>
#ifdef USE_KDE
#include <giomm/dbusproxy.h>
#endif
#define _(String) gettext(String)
#define MUTEITEM _("Mute")
#define SETTSITEM _("Settings")
#define ABOUTITEM _("About")
#define QUITITEM _("Quit")
#define CARDL _("Card: ")
#define VOLUMEL _("Volume: ")
#define MIXERL _("Mixer: ")
#define RESTOREITEM _("Restore")

static const int OFFSET = 2;

TrayIcon::TrayIcon(double volume,
                   const std::string &cardName,
                   const std::string &mixerName,
                   bool muted)
    : volumeValue_(volume)
    , cardName_(cardName)
    , mixerName_(mixerName)
    , muted_(muted)
    , menu_(Gtk::manage(new Gtk::Menu()))
    , restoreItem_(Gtk::manage(new Gtk::MenuItem(RESTOREITEM)))
    , settingsItem_(Gtk::manage(new Gtk::MenuItem(SETTSITEM)))
    , aboutItem_(Gtk::manage(new Gtk::MenuItem(ABOUTITEM)))
    , quitItem_(Gtk::manage(new Gtk::MenuItem(QUITITEM)))
    , muteItem_(Gtk::manage(new Gtk::CheckMenuItem(MUTEITEM)))
    , mouseX_(0)
    , mouseY_(0)
    , pixbufWidth_(0)
    , pixbufHeight_(0)
    , isLegacyIcon_(true)
    , screen_(aboutItem_->get_screen())
#if defined(USE_APPINDICATOR) || defined(USE_KDE)
    , newIcon_(nullptr)
#endif
    , legacyIcon_(nullptr)
{
    const Glib::ustring searchPath = Glib::ustring("icons/") + getIconName(100);
    const Glib::ustring iconPath = Tools::getResPath(searchPath.c_str());
#if defined(USE_APPINDICATOR)
    newIcon_ = StatusNotifierPtr(app_indicator_new("AlsaVolume",
                                                   iconPath.c_str(),
                                                   APP_INDICATOR_CATEGORY_APPLICATION_STATUS));
    if (newIcon_) {
        isLegacyIcon_ = false;
    }
    app_indicator_set_status(newIcon_.get(), APP_INDICATOR_STATUS_ACTIVE);
    app_indicator_set_menu(newIcon_.get(), menu_->gobj());
    app_indicator_set_secondary_activate_target(newIcon_.get(), GTK_WIDGET(muteItem_->gobj()));
    g_signal_connect(newIcon_.get(), "scroll-event", (GCallback)TrayIcon::onScrollEventAI, this);
#elif defined(USE_KDE)
    if (checkDBusInterfaceExists("org.kde.StatusNotifierWatcher")) {
        newIcon_ = StatusNotifierPtr(status_notifier_item_new_from_icon_name("AlsaVolume",
                                                                             STATUS_NOTIFIER_CATEGORY_APPLICATION_STATUS,
                                                                             iconPath.c_str()));
        if ( newIcon_ ) {
            status_notifier_item_set_status(newIcon_.get(), STATUS_NOTIFIER_STATUS_ACTIVE);
            status_notifier_item_set_title(newIcon_.get(), "AlsaVolume");
            status_notifier_item_register(newIcon_.get());
            status_notifier_item_set_window_id(newIcon_.get(),0);
            StatusNotifierState state = status_notifier_item_get_state(newIcon_.get());
#ifdef IS_DEBUG
            std::cout << "StatusNotifier state " << state << std::endl;
#endif
            g_signal_connect(newIcon_.get(), "registration-failed", GCallback(TrayIcon::onRegisterError), this);
            if ( state != STATUS_NOTIFIER_STATE_NOT_REGISTERED && state != STATUS_NOTIFIER_STATE_FAILED ) {
                isLegacyIcon_ = false;
#ifdef IS_DEBUG
                std::cout << "New Icon created" << std::endl;
#endif
                status_notifier_item_set_context_menu(newIcon_.get(), reinterpret_cast<GObject *>(menu_->gobj()));
                g_signal_connect(newIcon_.get(), "activate", GCallback(TrayIcon::onActivate), this);
                g_signal_connect(newIcon_.get(), "context-menu", GCallback(TrayIcon::onContextMenu), this);
                g_signal_connect(newIcon_.get(), "secondary-activate", GCallback(TrayIcon::onSecondaryActivate), this);
                g_signal_connect(newIcon_.get(), "scroll", GCallback(TrayIcon::onScroll), this);
            }
        }
    }
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
    restoreItem_->signal_activate().connect(sigc::mem_fun(*this, &TrayIcon::onHideRestore));
    menu_->append(*Gtk::manage(restoreItem_));
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
    Gdk::Rectangle area;
    Gtk::Orientation orientation;
    iconPosition pos;
    if (isLegacyIcon_) {
        if (legacyIcon_->get_geometry(screen_, area, orientation)) {
            pos.iconX_ = area.get_x();
            pos.iconY_ = area.get_y();
            const int areaHeight = area.get_height();
            const int areaWidth = area.get_width();
            pos.iconHeight_ = (areaHeight > 0) ? areaHeight : pixbufHeight_;
            pos.iconWidth_ = (areaWidth > 0) ? areaWidth : pixbufWidth_;
            pos.screenHeight_ = screen_->get_height();
            pos.screenWidth_ = screen_->get_width();
            pos.geometryAvailable_ = bool(pos.iconX_ > 0 || pos.iconY_ > 0);
            if (!pos.geometryAvailable_) {
                getMousePosition();
                pos.iconX_ = mouseX_;
                pos.iconY_ = mouseY_;
            }
            pos.trayAtTop_ = bool(pos.iconY_ < pos.screenHeight_ / 2);
            m_signal_on_restore(pos);
        }
    }
#ifdef USE_APPINDICATOR
    else {
        getMousePosition();
        pos.iconX_ = (pos.iconX_ != mouseX_) ? mouseX_ : pos.iconX_;
        pos.iconY_ = (pos.iconY_ != mouseY_) ? mouseY_ : pos.iconY_;
#ifdef IS_DEBUG
        std::cout << "X=" << pos.iconX_ << " Y=" << pos.iconY_ << std::endl;
#endif
        //
        pos.screenHeight_ = screen_->get_height();
        pos.screenWidth_ = screen_->get_width();
        pos.trayAtTop_ = bool(pos.iconY_ < pos.screenHeight_ / 2);
        pos.iconHeight_ = pixbufHeight_;
        pos.iconWidth_ = pixbufWidth_;
        pos.geometryAvailable_ = false;
        m_signal_on_restore(pos);
    }
#elif USE_KDE
    getMousePosition();
    onActivate(isLegacyIcon_ ? nullptr : newIcon_.get(), mouseX_, mouseY_, this);
#endif
}

#ifdef USE_KDE
void TrayIcon::onActivate(StatusNotifierItem *sn, gint x, gint y, TrayIcon *userdata)
{
    (void)sn;
    iconPosition pos;
    auto screen = userdata->screen_;
    pos.iconX_ = x;
    pos.iconY_ = y;
    pos.screenHeight_ = screen->get_height();
    pos.screenWidth_ = screen->get_width();
    pos.trayAtTop_ = bool(y < pos.screenHeight_ / 2);
    pos.iconHeight_ = userdata->pixbufHeight_;
    pos.iconWidth_ = userdata->pixbufWidth_;
    pos.geometryAvailable_ = false;
    userdata->m_signal_on_restore(pos);
}

void TrayIcon::onContextMenu(StatusNotifierItem *sn, gint x, gint y, TrayIcon *userdata)
{
    (void)sn;
    (void)x;
    (void)y;
    (void)userdata;
#ifdef IS_DEBUG
    std::cout << "Menu called " << std::endl;
#endif
}

void TrayIcon::onSecondaryActivate(StatusNotifierItem *sn, gint x, gint y, TrayIcon *userdata)
{
    (void)sn;
    (void)x;
    (void)y;
    userdata->muteItem_->set_active(!userdata->muteItem_->get_active());
}

void TrayIcon::onScroll(StatusNotifierItem *sn, gint delta, StatusNotifierScrollOrientation orient, TrayIcon *userdata)
{
    (void)sn;
    (void)orient;
    double value = 0.0;
    if (delta >0) {
        value+=OFFSET;
    }
    else {
        value-=OFFSET;
    }
    //Hack to detect right scroll direction
    int screenHeight = userdata->screen_->get_height();
    userdata->getMousePosition();
    if (userdata->mouseY_ < screenHeight / 2)
        value = (-1) * value;
#ifdef IS_DEBUG
    std::cout << "Offset: " << value << std::endl;
#endif
    userdata->m_signal_value_changed(value);
}

bool TrayIcon::checkDBusInterfaceExists(const Glib::ustring &serviceName)
{
    bool isExists_;
    const std::string DBUS_SERVICE = "org.freedesktop.DBus";
    Glib::RefPtr<Gio::DBus::Proxy> proxy = Gio::DBus::Proxy::create_for_bus_sync(Gio::DBus::BUS_TYPE_SESSION,
                                                                                 DBUS_SERVICE,
                                                                                 "/",
                                                                                 DBUS_SERVICE);
    Glib::VariantContainerBase result = proxy->call_sync("ListNames");
    Glib::Variant<std::vector<Glib::ustring> > gvar = Glib::VariantBase::cast_dynamic<
            Glib::Variant<std::vector<Glib::ustring> >
            >(result.get_child(0));
    std::vector<Glib::ustring> interfaces = gvar.get();
    isExists_ = Tools::itemExists(interfaces, serviceName);
#ifdef IS_DEBUG
    std::cout << "Interface exists " << isExists_ << std::endl;
#endif
    return isExists_;
}

void TrayIcon::onRegisterError(StatusNotifierItem *sn, GError *error, TrayIcon *userdata)
{
    (void)sn;
    std::cerr << error->code << " "<< error->message << std::endl;
    if(userdata->isLegacyIcon_) {
        const Glib::ustring searchPath = Glib::ustring("icons/") + userdata->getIconName(100);
        const Glib::ustring iconPath = Tools::getResPath(searchPath.c_str());
        userdata->legacyIcon_ = Gtk::StatusIcon::create(iconPath);
        //Staus icon signals
        userdata->legacyIcon_->signal_popup_menu().connect(sigc::mem_fun(*userdata, &TrayIcon::onPopup));
        userdata->legacyIcon_->signal_activate().connect(sigc::mem_fun(*userdata, &TrayIcon::onHideRestore));
        userdata->legacyIcon_->signal_scroll_event().connect(sigc::mem_fun(*userdata, &TrayIcon::onScrollEvent));
        userdata->legacyIcon_->signal_button_press_event().connect(sigc::mem_fun(*userdata, &TrayIcon::onButtonClick));
        //
    }
}

#endif

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
    const std::string ICON_PREFIX = "tb_icon";
    int number = 100;
    Glib::ustring iconPath = Glib::ustring::compose("%1%2.png",ICON_PREFIX, Glib::ustring::format(number));
    value = (value <= 0) ? 0 : (value > 100) ? 100 : value;
    number = (value < 20) ? 20 : int(floor(value/20+0.5)*20);
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
#if defined (USE_APPINDICATOR)
            else {
                app_indicator_set_icon_full(newIcon_.get(), iconPath.c_str(), "VolumeIcon");
            }
#elif defined(USE_KDE)
            else {
                status_notifier_item_set_from_icon_name(newIcon_.get(), STATUS_NOTIFIER_ICON, iconPath.c_str());
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
    if (!message.empty()) {
        if (isLegacyIcon_) {
            legacyIcon_->set_tooltip_text(message);
        }
        else {
#if defined(USE_APPINDICATOR)
            app_indicator_set_title(newIcon_.get(), message.c_str());
#elif defined(USE_KDE)
            const Glib::ustring searchPath = Glib::ustring("icons/") + getIconName(volumeValue_);
            const Glib::ustring iconPath = Tools::getResPath(searchPath.c_str());
            status_notifier_item_set_tooltip(newIcon_.get(), iconPath.c_str(), "AlsaVolume" ,message.c_str());
#endif
        }
    }
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
#ifdef IS_DEBUG
    std::cout << "Offset: " << value << std::endl;
#endif
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
        setMousePos(int(event->x_root), int(event->y_root));
    }
    return false;
}

void TrayIcon::setMousePos(const int X, const int Y)
{
    mouseX_ = X;
    mouseY_ = Y;
}

void TrayIcon::getMousePosition()
{
    int x = 0, y = 0;
#ifdef IS_GTK_2
    Glib::RefPtr<Gdk::Display> display = restoreItem_->get_display();
    Gdk::ModifierType type;
    display->get_pointer(x, y, type);
#else
    Glib::RefPtr<Gdk::Display> display = aboutItem_->get_display();
    Glib::RefPtr<Gdk::DeviceManager> manager = display->get_device_manager();
    for (Glib::RefPtr<Gdk::Device> &item : manager->list_devices(Gdk::DEVICE_TYPE_MASTER)) {
        if (item->get_source() == Gdk::SOURCE_MOUSE)
            item->get_position(x, y);
    }
#endif
    setMousePos(x, y);
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
