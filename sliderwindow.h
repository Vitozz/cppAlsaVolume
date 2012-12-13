#ifndef ALSAVOLUME_H
#define ALSAVOLUME_H

#include "gtkmm/button.h"
#include "gtkmm/window.h"
#include "gtkmm/builder.h"
#include "gtkmm/scale.h"

class AlsaVolume : public Gtk::Window
{
public:
	AlsaVolume(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder>&refGlade);
	virtual ~AlsaVolume();
	void runAboutDialog();
	void setWindowPosition(int x_, int y_);
	void setVolumeValue(double value);
	double getVolumeValue();
protected:
	void on_volume_slider();
	bool on_focus_out(GdkEventCrossing* event);
private:
	Gtk::Scale *volumeSlider_;
};

#endif // ALSAVOLUME_H
