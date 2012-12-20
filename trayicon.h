#ifndef TRAYICON_H
#define TRAYICON_H

#include "sliderwindow.h"
#include "gtkmm/statusicon.h"
#include "gtkmm/menu.h"
#include "gtkmm/menuitem.h"
#include "gtkmm/imagemenuitem.h"
#include "gtkmm/checkmenuitem.h"
#include "gtkmm/aboutdialog.h"

class TrayIcon : public Gtk::StatusIcon
{
public:
	TrayIcon(SliderWindow *parent);
	virtual ~TrayIcon();
	void setIcon(double value);
	void setTooltip(const Glib::ustring &message);

protected:
	//menu actions
	void onHideRestore();
	void runMixerApp();
	void runSettings();
	void onMute();
	void onAbout();
	void onQuit();
	//icon actions
	virtual void onPopup(guint button, guint32 activate_time);
	virtual bool onScrollEvent(GdkEventScroll *event);
	virtual bool onButtonClick(GdkEventButton *event);
	void on_signal_volume_changed(double volume);
private:
	SliderWindow *sliderWindow_;
	double volumeValue_;
	Gtk::Menu *menu_;
	Gtk::ImageMenuItem *restoreItem_, *mixerItem_, *settingsItem_, *aboutItem_, *quitItem_;
	Gtk::CheckMenuItem* muteItem_;
};

#endif // TRAYICON_H
