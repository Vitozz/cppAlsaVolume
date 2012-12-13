#include "sliderwindow.h"
#include "filework.h"
#include <iostream>
#include "gtkmm/aboutdialog.h"

AlsaVolume::AlsaVolume(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade)
: Gtk::Window(cobject)
{
	Glib::RefPtr<Gtk::Builder>builder = refGlade;
	volumeSlider_ = 0;
	builder->get_widget("volume_slider", volumeSlider_);
	if (volumeSlider_) {
		volumeSlider_->signal_value_changed().connect(sigc::mem_fun(*this, &AlsaVolume::on_volume_slider));
	}
	set_events(Gdk::LEAVE_NOTIFY_MASK);
	signal_leave_notify_event().connect(sigc::mem_fun(*this, &AlsaVolume::on_focus_out));
}

AlsaVolume::~AlsaVolume()
{
}

void AlsaVolume::runAboutDialog()
{
	Gtk::AboutDialog *dialog = new Gtk::AboutDialog();
	dialog->set_transient_for(*this);
	dialog->set_title("About cppAlsaVolume");
	dialog->set_program_name("Alsa Volume Changer");
	dialog->set_comments("Tray Alsa Volume Changer written using gtkmm");
	dialog->set_version("0.0.1");
	dialog->set_copyright("2012 (c) Vitaly Tonkacheyev (thetvg@gmail.com)");
	dialog->set_website("http://sites.google.com/site/thesomeprojects/");
	dialog->set_website_label("Program Website");
	Glib::RefPtr<Gdk::Pixbuf> logo = Gdk::Pixbuf::create_from_file(getResPath("icons/volume.png"));
	dialog->set_logo(logo);
	dialog->run();
	delete dialog;
}

void AlsaVolume::setWindowPosition(int x_, int y_)
{
	move(x_,y_);
}

void AlsaVolume::on_volume_slider()
{
	std::cout << volumeSlider_->get_value() << std::endl;
}

bool AlsaVolume::on_focus_out(GdkEventCrossing* event)
{
	if ((event->type |(GDK_LEAVE_NOTIFY == 0))
			&& (event->x < 0
			    ||event->x >= get_width()
			    ||event->y < 0
			    ||event->y >= get_height())){
		hide();
	}
	return false;
}

void AlsaVolume::setVolumeValue(double value)
{
	if (value < 100) {
		volumeSlider_->set_value(value);
	}
	else if (value > 0) {
		volumeSlider_->set_value(100);
	}
	else if (value < 0) {
		volumeSlider_->set_value(0);
	}
}

double AlsaVolume::getVolumeValue()
{
	return volumeSlider_->get_value();
}
