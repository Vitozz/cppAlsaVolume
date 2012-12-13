#include "trayicon.h"
#include "filework.h"
#include "gtkmm/separatormenuitem.h"
#include "gtkmm/stock.h"
#include <iostream>

TrayIcon::TrayIcon(AlsaVolume *parent, const char *iconName)
: hidden_(true)
{
	avWindow_ = parent;
	avWindow_->hide();
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
	signal_activate().connect(sigc::mem_fun(*this, &TrayIcon::onLeftClick));
	signal_scroll_event().connect(sigc::mem_fun(*this, &TrayIcon::onScrollEvent));
	getGeometry();
	setIcon(Glib::ustring(iconName));
}

TrayIcon::~TrayIcon()
{
}

void TrayIcon::onHideRestore()
{
	hidden_ = avWindow_->property_visible();
	if (hidden_) {
		avWindow_->set_visible(false);
	}
	else {
		getGeometry();
		int x_, y_;
		if (geometry_[1] <= 200) {
			y_ = geometry_[1] + geometry_[2] + 2;
		}
		else {
			y_ = geometry_[1] - avWindow_->get_height() - 4;
		}
		x_ = geometry_[0];
		avWindow_->setWindowPosition(x_, y_);
		avWindow_->show_all();
	}
}

void TrayIcon::onQuit()
{
	exit(0);
}

void TrayIcon::runMixerApp()
{}

void TrayIcon::runSettings()
{}

void TrayIcon::onAbout()
{
	avWindow_->runAboutDialog();
}

void TrayIcon::onMute()
{
	std::cout << muteItem_->get_active() << std::endl;
}

void TrayIcon::setIcon(Glib::ustring iconPath)
{
	if (!iconPath.empty())
		set_from_file(getResPath(iconPath.c_str()));
}

void TrayIcon::setTooltip(Glib::ustring message)
{
	if (!message.empty())
		set_tooltip_text(message);
}

void TrayIcon::onLeftClick()
{
	onHideRestore();
}

void TrayIcon::onPopup(guint button, guint32 activate_time)
{
	popup_menu_at_position(*menu_, button, activate_time);
}

bool TrayIcon::onScrollEvent(GdkEventScroll* event)
{
	double value = avWindow_->getVolumeValue();
	if (event->direction == GDK_SCROLL_UP) {
		value+=2;
	}
	else if(event->direction == GDK_SCROLL_DOWN) {
		value-=2;
	}
	avWindow_->setVolumeValue(value);
	return false;
}

bool TrayIcon::onButtonClick(GdkEventButton* event)
{
	(void)event;
	return false;
}

void TrayIcon::getGeometry()
{
	Glib::RefPtr<Gdk::Screen> screen;
	Gdk::Rectangle area;
	Gtk::Orientation orientation;
	if (get_geometry(screen, area, orientation)) {
		geometry_[0] = area.get_x();
		geometry_[1] = area.get_y();
		geometry_[2] = area.get_height();
		geometry_[3] = area.get_width();
	}
}
