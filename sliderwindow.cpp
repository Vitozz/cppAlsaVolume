#include "sliderwindow.h"
#include "settingsframe.h"
#include "tools.h"
#include "gtkmm/aboutdialog.h"
#include "glibmm.h"
#include <iostream>
#include <map>

SliderWindow::SliderWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade)
: Gtk::Window(cobject)
{
	alsaWork_ = new AlsaWork();
	Glib::RefPtr<Gtk::Builder> builder = refGlade;
	volumeSlider_ = 0;
	builder->get_widget("volume_slider", volumeSlider_);
	if (volumeSlider_) {
		volumeSlider_->signal_value_changed().connect(sigc::mem_fun(*this, &SliderWindow::on_volume_slider));
	}
	set_events(Gdk::LEAVE_NOTIFY_MASK);
	signal_leave_notify_event().connect(sigc::mem_fun(*this, &SliderWindow::on_focus_out));
	settings_ = new Settings();
	cardList_ = alsaWork_->getCardsList();
	cardId_ = settings_->getSoundCard();
	mixerList_ = alsaWork_->getMixersList(cardId_);
	mixerName_ = settings_->getMixer();
	std::pair<bool, int> isMixer = FileWork::itemExists(mixerList_, mixerName_);
	if (isMixer.first) {
		mixerId_ = isMixer.second;
	}
	else {
		mixerId_ = 0;
		mixerName_ = mixerList_.at(mixerId_);
	}
	volumeValue_ = settings_->getVolume();
	volumeSlider_->set_value(volumeValue_);
}

SliderWindow::~SliderWindow()
{
	delete settings_;
	delete alsaWork_;
}

void SliderWindow::runAboutDialog()
{
	Gtk::AboutDialog *dialog = new Gtk::AboutDialog();
	dialog->set_transient_for(*this);
	dialog->set_title("About cppAlsaVolume");
	dialog->set_program_name("Alsa Volume Changer");
	dialog->set_comments("Tray Alsa Volume Changer written using gtkmm");
	dialog->set_version("0.0.2");
	dialog->set_copyright("2012 (c) Vitaly Tonkacheyev (thetvg@gmail.com)");
	dialog->set_website("http://sites.google.com/site/thesomeprojects/");
	dialog->set_website_label("Program Website");
	Glib::RefPtr<Gdk::Pixbuf> logo = Gdk::Pixbuf::create_from_file(FileWork::getResPath("icons/volume.png"));
	dialog->set_icon_from_file(FileWork::getResPath("icons/tb_icon100.png"));
	dialog->set_logo(logo);
	dialog->run();
	delete dialog;
}

void SliderWindow::setWindowPosition(int x_, int y_)
{
	move(x_,y_);
}

void SliderWindow::on_volume_slider()
{
	volumeValue_ = volumeSlider_->get_value();
	alsaWork_->setAlsaVolume(mixerList_.at(mixerId_), volumeValue_);
	m_signal_volume_changed.emit(volumeValue_);
	std::cout << "Volume= " << alsaWork_->getAlsaVolume(mixerList_.at(mixerId_)) << std::endl;
}

bool SliderWindow::on_focus_out(GdkEventCrossing* event)
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

void SliderWindow::setVolumeValue(double value)
{
	if (value >= 100) {
		volumeValue_ = 100;
	}
	else if (value <= 0){
		volumeValue_ = 0;
	}
	else if (value < 100) {
		volumeValue_ = value;
	}
	volumeSlider_->set_value(volumeValue_);
}

std::string SliderWindow::getActiveMixer() const
{
	return mixerList_.at(mixerId_);
}

double SliderWindow::getVolumeValue() const
{
	return volumeValue_;
}

bool SliderWindow::getVisible()
{
	return property_visible();
}

void SliderWindow::showWindow()
{
	show_all();
}

void SliderWindow::hideWindow()
{
	hide();
}

int SliderWindow::getHeight() const
{
	return get_height();
}

int SliderWindow::getWidth() const
{
	return get_width();
}

void SliderWindow::saveSettings()
{
	settings_->saveVolume(volumeValue_);
	settings_->saveSoundCard(cardId_);
	settings_->saveMixer(std::string(mixerName_.c_str()));
}

SliderWindow::type_sliderwindow_signal SliderWindow::signal_volume_changed()
{
	return m_signal_volume_changed;
}

std::string SliderWindow::getSoundCardName() const
{
	return alsaWork_->getCardName(cardId_);
}

void SliderWindow::createSettingsDialog()
{
	SettingsFrame *settingsDialog = 0;
	Glib::ustring ui_ = FileWork::getResPath("gladefiles/SettingsFrame.glade");
	if (ui_.empty()) {
		std::cerr << "No SliderFrame.glade file found" << std::endl;
	}
	builder_ = Gtk::Builder::create();
	try {
		//refBuilder->create_from_file(ui_);
		builder_->add_from_file(ui_);
	}
	catch(const Gtk::BuilderError& ex) {
		std::cerr << "BuilderError::main.cpp::19 " << ex.what() << std::endl;
	}
	catch(const Glib::MarkupError& ex) {
		std::cerr << "MarkupError::main.cpp::19 " << ex.what() << std::endl;
	}
	catch(const Glib::FileError& ex) {
		std::cerr << "FileError::main.cpp::19 " << ex.what() << std::endl;
	}
	builder_->get_widget_derived("settingsDialog", settingsDialog);

	if (settingsDialog) {
		settingsDialog->runDialog(this);
		//delete settingsDialog;
	}

}

void SliderWindow::runSettings()
{
	createSettingsDialog();
}
