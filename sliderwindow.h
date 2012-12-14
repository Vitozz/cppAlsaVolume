#ifndef ALSAVOLUME_H
#define ALSAVOLUME_H

#include "gtkmm/button.h"
#include "gtkmm/window.h"
#include "gtkmm/builder.h"
#include "gtkmm/scale.h"
#include "settings.h"

class AlsaVolume : public Gtk::Window
{
public:
	AlsaVolume(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder>&refGlade);
	virtual ~AlsaVolume();
	void runAboutDialog();
	void setWindowPosition(int x_, int y_);
	bool getVisible();
	void showWindow();
	void hideWindow();
	int getHeight();
	int getWidth();
	void setVolumeValue(double value);
	double getVolumeValue();
	void saveSettings();
protected:
	void on_volume_slider();
	bool on_focus_out(GdkEventCrossing* event);
private:
	Gtk::Scale *volumeSlider_;
	double volumeValue_;
	Settings *settings_;
};

#endif // ALSAVOLUME_H
