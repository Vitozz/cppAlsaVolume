#ifndef SLIDERWINDOW_H
#define SLIDERWINDOW_H

#include "gtkmm/button.h"
#include "gtkmm/window.h"
#include "gtkmm/builder.h"
#include "gtkmm/scale.h"
#include "settings.h"
#include "alsawork.h"
#include <vector>

class SliderWindow : public Gtk::Window
{
public:
	SliderWindow(BaseObjectType *cobject, const Glib::RefPtr<Gtk::Builder>&refGlade);
	virtual ~SliderWindow();
	void runAboutDialog();
	void setWindowPosition(int x_, int y_);
	bool getVisible();
	void showWindow();
	void hideWindow();
	int getHeight() const;
	int getWidth() const;
	void setVolumeValue(double value);
	double getVolumeValue() const;
	std::string getSoundCardName() const;
	std::string getActiveMixer() const;
	void saveSettings();
	void runSettings();
	//my signal
	typedef sigc::signal<void, double> type_sliderwindow_signal;
	type_sliderwindow_signal signal_volume_changed();
	//
protected:
	void on_volume_slider();
	bool on_focus_out(GdkEventCrossing* event);
	type_sliderwindow_signal m_signal_volume_changed;
private:
	void createSettingsDialog();
private:
	Glib::RefPtr<Gtk::Builder> builder_;
	Gtk::Scale *volumeSlider_;
	double volumeValue_;
	Settings *settings_;
	AlsaWork *alsaWork_;
	std::vector<std::string> cardList_;
	std::vector<std::string> mixerList_;
	int cardId_, mixerId_;
	Glib::ustring mixerName_;
};

#endif // SLIDERWINDOW_H
