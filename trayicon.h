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
	TrayIcon(double volume, std::string cardName, std::string mixerName);
	virtual ~TrayIcon();
	void on_signal_volume_changed(double volume, std::string cardName, std::string mixerName);
	//signals
	typedef sigc::signal<void> type_trayicon_simple_signal;
	type_trayicon_simple_signal signal_ask_dialog();
	type_trayicon_simple_signal signal_ask_settings();
	type_trayicon_simple_signal signal_save_settings();
	typedef sigc::signal<void, int, int, int ,int> type_trayicon_4int_signal;
	type_trayicon_4int_signal signal_on_restore();
	typedef sigc::signal<void, double> type_trayicon_double_signal;
	type_trayicon_double_signal signal_value_changed();

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

private:
	void setIcon(double value);
	void setTooltip(const Glib::ustring &message);

private:
	double volumeValue_;
	std::string cardName_;
	std::string mixerName_;
	Gtk::Menu *menu_;
	Gtk::ImageMenuItem *restoreItem_, *mixerItem_, *settingsItem_, *aboutItem_, *quitItem_;
	Gtk::CheckMenuItem* muteItem_;
};

#endif // TRAYICON_H
