#include "trayicon.h"
#include "filework.h"
#include "settings.h"
#include "gtkmm/separatormenuitem.h"
#include "gtkmm/stock.h"
#include "glibmm.h"
#include <iostream>

const uint MIDDLE_BUTTON = 2;
const int OFFSET = 2;

TrayIcon::TrayIcon(SliderWindow *parent)
: sliderWindow_(parent)
{
	sliderWindow_->hideWindow();
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
	sliderWindow_->signal_volume_changed().connect(sigc::mem_fun(*this, &TrayIcon::on_signal_volume_changed));
	//
	on_signal_volume_changed(sliderWindow_->getVolumeValue());
}

TrayIcon::~TrayIcon()
{
}

void TrayIcon::onHideRestore()
{
	if (!sliderWindow_->getVisible()) {
		Glib::RefPtr<Gdk::Screen> screen;
		Gdk::Rectangle area;
		Gtk::Orientation orientation;
		int x_ = 0;
		int y_ = 0;
		int winHeight = sliderWindow_->getHeight();
		int winWidth = sliderWindow_->getWidth();
		if (get_geometry(screen, area, orientation)) {
			int aY = area.get_y();
			int aX = area.get_x();
			int aH = area.get_height();
			int aW = area.get_width();
			if (aY <= 200) { //check tray up/down position
				y_ = aY + aH + 2;
			}
			else {
				y_ = aY - winHeight - 4;
			}
			if (winWidth > 1) {//on first run window widht = 1
				x_ = (aX + aW/2) - winWidth/2;
			}
			else {
				x_ = aX - aW/2;
			}
		}
		sliderWindow_->setWindowPosition(x_, y_);
		sliderWindow_->showWindow();
	}
	else {
		sliderWindow_->hideWindow();
	}
}

void TrayIcon::onQuit()
{
	sliderWindow_->saveSettings();
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
	sliderWindow_->runAboutDialog();
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
			set_from_file(getResPath(iconPath.c_str()));
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
	double value = sliderWindow_->getVolumeValue();
	if (event->direction == GDK_SCROLL_UP) {
		value+=OFFSET;
	}
	else if(event->direction == GDK_SCROLL_DOWN) {
		value-=OFFSET;
	}
	sliderWindow_->setVolumeValue(value);
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

void TrayIcon::on_signal_volume_changed(double volume)
{
	volumeValue_ = volume;
	setIcon(volumeValue_);
	setTooltip(Glib::ustring("Master volume: ") + Glib::ustring::format(volumeValue_,"%"));
}
