#include "trayicon.h"
#include "filework.h"
#include "settings.h"
#include "gtkmm/separatormenuitem.h"
#include "gtkmm/stock.h"
#include <iostream>
#include "gdk/gdk.h"

const uint MIDDLE_BUTTON = 2;
const int OFFSET = 2;

TrayIcon::TrayIcon(AlsaVolume *parent)
: avWindow_(parent)
{
	avWindow_->hideWindow();
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
	//
	signal_popup_menu().connect(sigc::mem_fun(*this, &TrayIcon::onPopup));
	signal_activate().connect(sigc::mem_fun(*this, &TrayIcon::onHideRestore));
	signal_scroll_event().connect(sigc::mem_fun(*this, &TrayIcon::onScrollEvent));
	signal_button_press_event().connect(sigc::mem_fun(*this, &TrayIcon::onButtonClick));
	volumeValue_ = avWindow_->getVolumeValue();
	setIcon(volumeValue_);
	setTooltip(Glib::ustring("Master volume: ") + Glib::ustring::format(volumeValue_,"%"));
}

TrayIcon::~TrayIcon()
{
}

void TrayIcon::onHideRestore()
{
	if (!avWindow_->getVisible()) {
		Glib::RefPtr<Gdk::Screen> screen;
		Gdk::Rectangle area;
		Gtk::Orientation orientation;
		int x_ = 0;
		int y_ = 0;
		if (get_geometry(screen, area, orientation)) {
			if (area.get_y() <= 200) {
				y_ = area.get_y() + area.get_height() + 2;
			}
			else {
				y_ = area.get_y() - avWindow_->getHeight() - 4;
			}
			x_ = area.get_x();
		}
		avWindow_->setWindowPosition(x_, y_);
		avWindow_->showWindow();
	}
	else {
		avWindow_->hideWindow();
	}
}

void TrayIcon::onQuit()
{
	avWindow_->saveSettings();
	exit(0);
}

void TrayIcon::runMixerApp()
{
}

void TrayIcon::runSettings()
{
}

void TrayIcon::onAbout()
{
	avWindow_->runAboutDialog();
}

void TrayIcon::onMute()
{
	if (muteItem_->get_active()) {
		setIcon(0);
	}
	else {
		setIcon(volumeValue_);
	}
}

void TrayIcon::setIcon(int value)
{
	Glib::ustring iconPath;
	if (value <= 0) {
		iconPath = Glib::ustring("icons/tb_icon0.png");
	}
	if (value >0 && value < 40) {
		iconPath = Glib::ustring("icons/tb_icon20.png");
	}
	if (value >40 && value < 60) {
		iconPath = Glib::ustring("icons/tb_icon40.png");
	}
	if (value >60 && value < 80) {
		iconPath = Glib::ustring("icons/tb_icon60.png");
	}
	if (value >80 && value < 100) {
		iconPath = Glib::ustring("icons/tb_icon80.png");
	}
	if (value >= 100) {
		iconPath = Glib::ustring("icons/tb_icon100.png");
	}
	if (!iconPath.empty())
		set_from_file(getResPath(iconPath.c_str()));
}

void TrayIcon::setTooltip(Glib::ustring message)
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
	double value = avWindow_->getVolumeValue();
	if (event->direction == GDK_SCROLL_UP) {
		value+=OFFSET;
	}
	else if(event->direction == GDK_SCROLL_DOWN) {
		value-=OFFSET;
	}
	avWindow_->setVolumeValue(value);
	volumeValue_ = (int)avWindow_->getVolumeValue();
	setIcon(volumeValue_);
	setTooltip(Glib::ustring("Master volume: ") + Glib::ustring::format(volumeValue_,"%"));
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
