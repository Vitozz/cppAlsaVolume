#include "sliderwindow.h"
#include "settingsframe.h"
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
	if (!mixerName_.empty()) {
		std::pair<bool, int> isMixer = Tools::itemExists(mixerList_, mixerName_);
		if (isMixer.first) {
			mixerId_ = isMixer.second;
		}
	}
	else {
		mixerId_ = 0;
		mixerName_ = mixerList_.at(mixerId_);
	}
	volumeValue_ = settings_->getVolume();
	volumeSlider_->set_value(volumeValue_);
	orient_ = settings_->getNotebookOrientation();
	switchList_ = alsaWork_->getSwitchList(cardId_);
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
	dialog->set_version("0.0.3");
	dialog->set_copyright("2012 (c) Vitaly Tonkacheyev (thetvg@gmail.com)");
	dialog->set_website("http://sites.google.com/site/thesomeprojects/");
	dialog->set_website_label("Program Website");
	Glib::RefPtr<Gdk::Pixbuf> logo = Gdk::Pixbuf::create_from_file(Tools::getResPath("icons/volume.png"));
	dialog->set_icon_from_file(Tools::getResPath("icons/tb_icon100.png"));
	dialog->set_logo(logo);
	dialog->run();
	delete dialog;
}

void SliderWindow::setWindowPosition(int x_, int y_, int height_, int width_)
{
	if (!get_visible()) {
		int wX = 0;
		int wY = 0;
		int wWidth = get_width();
		int wHeight = get_height();
		if (y_ <= 200) { //check tray up/down position
			wY = y_ + height_ + 2;
		}
		else {
			wY = y_ - wHeight - 4;
		}
		if (wWidth > 1) {//at first run window widht = 1
			wX = (x_ + width_/2) - wWidth/2;
		}
		else {
			wX = x_;
		}
		move(wX,wY);
		show_all();
	}
	else {
		hide();
	}
}

void SliderWindow::on_volume_slider()
{
	volumeValue_ = volumeSlider_->get_value();
	alsaWork_->setAlsaVolume(mixerName_, volumeValue_);
	m_signal_volume_changed.emit(volumeValue_, getSoundCardName(), mixerName_);
	std::cout << "Volume= " << alsaWork_->getAlsaVolume(mixerName_) << std::endl;
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
	double volume = getVolumeValue() + value;
	if (volume >= 100) {
		volumeValue_ = 100;
	}
	else if (volume <= 0){
		volumeValue_ = 0;
	}
	else if (volume < 100) {
		volumeValue_ = volume;
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
	settings_->saveNotebookOrientation(orient_);

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
	Glib::ustring ui_ = Tools::getResPath("gladefiles/SettingsFrame.glade");
	if (ui_.empty()) {
		std::cerr << "No SettingsFrame.glade file found" << std::endl;
	}
	builder_ = Gtk::Builder::create();
	try {
		builder_->add_from_file(ui_);
	}
	catch(const Gtk::BuilderError& ex) {
		std::cerr << "BuilderError::sliderwindow.cpp::166 " << ex.what() << std::endl;
	}
	catch(const Glib::MarkupError& ex) {
		std::cerr << "MarkupError::sliderwindow.cpp::166 " << ex.what() << std::endl;
	}
	catch(const Glib::FileError& ex) {
		std::cerr << "FileError::sliderwindow.cpp::166 " << ex.what() << std::endl;
	}
	builder_->get_widget_derived("settingsDialog", settingsDialog);

	if (settingsDialog) {
		settingsStr str;
		str.cardId = cardId_;
		str.mixerId = mixerId_;
		str.cardList = cardList_;
		str.mixerList = mixerList_;
		str.switchList = switchList_;
		str.notebookOrientation = orient_;
		settingsDialog->initParms(str);
		settingsDialog->signal_ok_pressed().connect(sigc::mem_fun(*this, &SliderWindow::onSettingsDialogOk));
		settingsDialog->run();
		delete settingsDialog;
	}

}

void SliderWindow::runSettings()
{
	createSettingsDialog();
}

std::vector<std::string> SliderWindow::getMixersList()
{
	return mixerList_;
}

std::vector<std::string> SliderWindow::getCardsList()
{
	return cardList_;
}

void SliderWindow::setActiveCard(int card)
{
	alsaWork_->setCardId(card);
}

void SliderWindow::onSettingsDialogOk(settingsStr str)
{
	cardId_ = str.cardId;
	alsaWork_->setCardId(cardId_);
	mixerId_ = str.mixerId;
	mixerName_ = mixerList_.at(mixerId_);
	orient_ = str.notebookOrientation;
}
