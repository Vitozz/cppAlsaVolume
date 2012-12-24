#include "trayicon.h"
#include "tools.h"
#include "gtkmm/separatormenuitem.h"
#include "gtkmm/stock.h"
#include "glibmm.h"
#include <iostream>

const uint MIDDLE_BUTTON = 2;
const int OFFSET = 2;

TrayIcon::TrayIcon(double volume, std::string cardName, std::string mixerName, bool muted)
: volumeValue_(volume)
{
	menu_ = Gtk::manage(new Gtk::Menu());
	//menu creation
	restoreItem_ = Gtk::manage(new Gtk::ImageMenuItem(Gtk::Stock::GO_UP));
	restoreItem_->signal_activate().connect(sigc::mem_fun(*this, &TrayIcon::onHideRestore));
	menu_->append(*Gtk::manage(restoreItem_));
	restoreItem_->set_label("Restore");
	Gtk::SeparatorMenuItem *separator1 = Gtk::manage(new Gtk::SeparatorMenuItem());
	Gtk::SeparatorMenuItem *separator2 = Gtk::manage(new Gtk::SeparatorMenuItem());
	menu_->append(*Gtk::manage(separator1));
	mixerItem_ = Gtk::manage(new Gtk::ImageMenuItem(Gtk::Stock::MEDIA_RECORD));
	mixerItem_->signal_activate().connect(sigc::mem_fun(*this, &TrayIcon::runMixerApp));
	mixerItem_->set_label("Mixer");
	menu_->append(*Gtk::manage(mixerItem_));
	settingsItem_ = Gtk::manage(new Gtk::ImageMenuItem(Gtk::Stock::PREFERENCES));
	settingsItem_->signal_activate().connect(sigc::mem_fun(*this, &TrayIcon::runSettings));
	menu_->append(*Gtk::manage(settingsItem_));
	muteItem_ = Gtk::manage(new Gtk::CheckMenuItem("Mute"));
	muteItem_->signal_toggled().connect(sigc::mem_fun(*this, &TrayIcon::onMute));
	menu_->append(*Gtk::manage(muteItem_));
	aboutItem_ = Gtk::manage(new Gtk::ImageMenuItem(Gtk::Stock::ABOUT));
	aboutItem_->signal_activate().connect(sigc::mem_fun(*this, &TrayIcon::onAbout));
	menu_->append(*Gtk::manage(aboutItem_));
	menu_->append(*Gtk::manage(separator2));
	quitItem_ = Gtk::manage(new Gtk::ImageMenuItem(Gtk::Stock::QUIT));
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
	muted_ = muted;
	std::cout << muted << std::endl;
	muteItem_->set_active(muted_);
}

TrayIcon::~TrayIcon()
{}

void TrayIcon::onHideRestore()
{
	Glib::RefPtr<Gdk::Screen> screen;
	Gdk::Rectangle area;
	Gtk::Orientation orientation;
	if (get_geometry(screen, area, orientation)) {
		int aY = area.get_y();
		int aX = area.get_x();
		int aH = area.get_height();
		int aW = area.get_width();
		m_signal_on_restore.emit(aX, aY, aH, aW);
	}
}

void TrayIcon::onQuit()
{
	m_signal_save_settings.emit();
	exit(0);
}

void TrayIcon::runMixerApp()
{
	//not implemented yet
}

void TrayIcon::runSettings()
{
	m_signal_ask_settings.emit();
}

void TrayIcon::onAbout()
{
	m_signal_ask_dialog.emit();
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
	m_signal_on_mute.emit(!muted_);
}

void TrayIcon::setIcon(double value)
{
	Glib::ustring iconPath;
	if (value <= 0) {
		iconPath = Glib::ustring("icons/tb_icon0.png");
	}
	if (value >0 && (value < 40)) {
		iconPath = Glib::ustring("icons/tb_icon20.png");
	}
	if (value >=40 && (value < 60)) {
		iconPath = Glib::ustring("icons/tb_icon40.png");
	}
	if (value >=60 && (value < 80)) {
		iconPath = Glib::ustring("icons/tb_icon60.png");
	}
	if (value >=80 && (value < 100)) {
		iconPath = Glib::ustring("icons/tb_icon80.png");
	}
	if (value >= 100) {
		iconPath = Glib::ustring("icons/tb_icon100.png");
	}
	if (!iconPath.empty()) {
		try {
			set_from_file(Tools::getResPath(iconPath.c_str()));
		}
		catch (Glib::FileError &err) {
			std::cerr << "FileError::trayicon.cpp::138:: " << err.what() << std::endl;
		}
	}
}

void TrayIcon::setTooltip(const Glib::ustring &message)
{
	if (!message.empty())
		set_tooltip_text(message);
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
	if (event->button == MIDDLE_BUTTON) {
		muteItem_->set_active(!muteItem_->get_active());
		onMute();
	}
	return false;
}

void TrayIcon::on_signal_volume_changed(double volume, std::string cardName, std::string mixerName)
{
	volumeValue_ = volume;
	cardName_ = cardName;
	mixerName_ = mixerName;
	setIcon(volumeValue_);
	Glib::ustring tip = Glib::ustring("Card: ")
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
