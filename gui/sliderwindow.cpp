/*
 * sliderwindow.cpp
 * Copyright (C) 2012 Vitaly Tonkacheyev
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "sliderwindow.h"
#include "settingsframe.h"
#include "gtkmm/aboutdialog.h"
#include "glibmm/markup.h"
#include "glibmm/fileutils.h"
#include <iostream>

const std::string TITLE = "About AlsaVolume";
const std::string PROGNAME = "Alsa Volume Changer";
const std::string COMMENTS = "Tray Alsa Volume Changer written using gtkmm";
const std::string COPYRIGHT = "2012 (c) Vitaly Tonkacheyev (thetvg@gmail.com)";
const std::string WEBSITE = "http://sites.google.com/site/thesomeprojects/";
const std::string WEBSITELABEL = "Program Website";

SliderWindow::SliderWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade)
: Gtk::Window(cobject)
{
	Glib::RefPtr<Gtk::Builder> builder = refGlade;
	volumeSlider_ = 0;
	builder->get_widget("volume_slider", volumeSlider_);
	if (volumeSlider_) {
		volumeSlider_->signal_value_changed().connect(sigc::mem_fun(*this, &SliderWindow::on_volume_slider));
	}
	set_events(Gdk::LEAVE_NOTIFY_MASK);
	signal_leave_notify_event().connect(sigc::mem_fun(*this, &SliderWindow::on_focus_out));
	//init class variables
	alsaWork_ = new AlsaWork();
	settings_ = new Settings();
	settingsStr_ = new settingsStr();
	//
	settingsStr_->setList(CARDS, alsaWork_->getCardsList());
	settingsStr_->setCardId(settings_->getSoundCard());
	settingsStr_->setList(MIXERS, alsaWork_->getVolumeMixers(settingsStr_->cardId()));
	mixerName_ = settings_->getMixer();
	if (!mixerName_.empty()) {
		std::pair<bool, int> isMixer = Tools::itemExists(settingsStr_->mixerList(), mixerName_);
		if (isMixer.first) {
			settingsStr_->setMixerId(isMixer.second);
		}
	}
	else {
		settingsStr_->setMixerId(0);
		mixerName_ = settingsStr_->mixerList().at(settingsStr_->mixerId());
	}
	volumeValue_ = settings_->getVolume();
	volumeSlider_->set_value(volumeValue_);
	settingsStr_->setNotebookOrientation(settings_->getNotebookOrientation());
	settingsStr_->addMixerSwitch(alsaWork_->getSwitchList(settingsStr_->cardId()));
	settings_->setVersion(Tools::version);
	settingsStr_->setCurrIconPack(settings_->getCurrIconPack());
	settingsStr_->setIsAutorun(settings_->getAutorun());
	settingsStr_->setExternalMixer(settings_->getExternalMixer());
	std::string cIpack = settingsStr_->currIconPack();
	iconpacks_ = new iconpacks(cIpack, Tools::getTmpDir());
	settingsStr_->setList(ICONS, iconpacks_->getPacks());
	if (!cIpack.empty() && (cIpack != Tools::defaultIconPack)) {
		iconpacks_->extract();
	}
}

SliderWindow::~SliderWindow()
{
	delete settings_;
	delete settingsStr_;
	delete alsaWork_;
	delete iconpacks_;
}

void SliderWindow::runAboutDialog()
{
	const std::string tmpDir = Tools::getTmpDir();
	Gtk::AboutDialog *dialog = new Gtk::AboutDialog();
	dialog->set_transient_for(*this);
	dialog->set_title(TITLE);
	dialog->set_program_name(PROGNAME);
	dialog->set_comments(COMMENTS);
	dialog->set_version(Tools::version);
	dialog->set_copyright(COPYRIGHT);
	dialog->set_website(WEBSITE);
	dialog->set_website_label(WEBSITELABEL);
	Glib::RefPtr<Gdk::Pixbuf> logo, icon;
	std::string iconName, logoName;
	if (!Tools::checkDirExists(tmpDir)) {
		logoName = Tools::getResPath("icons/volume.png");
		iconName = Tools::getResPath("icons/tb_icon100.png");
	}
	else {
		logoName = tmpDir + "/volume.png";
		iconName = tmpDir + "/tb_icon100.png";
	}
	logo = Gdk::Pixbuf::create_from_file(logoName);
	icon = Gdk::Pixbuf::create_from_file(iconName);
	dialog->set_icon(icon);
	dialog->set_logo(logo);
	dialog->run();
	dialog->unset_transient_for();
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
	return mixerName_;
}

double SliderWindow::getVolumeValue()
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
	settings_->saveSoundCard(settingsStr_->cardId());
	settings_->saveMixer(std::string(mixerName_.c_str()));
	settings_->saveNotebookOrientation(settingsStr_->notebookOrientation());
	settings_->setExternalMixer(settingsStr_->externalMixer());
}

SliderWindow::type_sliderwindow_signal SliderWindow::signal_volume_changed()
{
	return m_signal_volume_changed;
}

std::string SliderWindow::getSoundCardName() const
{
	return alsaWork_->getCardName(settingsStr_->cardId());
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
		std::cerr << "BuilderError::sliderwindow.cpp::233 " << ex.what() << std::endl;
	}
	catch(const Glib::MarkupError& ex) {
		std::cerr << "MarkupError::sliderwindow.cpp::233 " << ex.what() << std::endl;
	}
	catch(const Glib::FileError& ex) {
		std::cerr << "FileError::sliderwindow.cpp::233 " << ex.what() << std::endl;
	}
	builder_->get_widget_derived("settingsDialog", settingsDialog);
	updateControls(settingsStr_->cardId());
	if (settingsDialog) {
		settingsDialog->initParms(*settingsStr_);
		settingsDialog->signal_ok_pressed().connect(sigc::mem_fun(*this, &SliderWindow::onSettingsDialogOk));
		settingsDialog->signal_switches_toggled().connect(sigc::mem_fun(*this, &SliderWindow::switchChanged));
		settingsDialog->signal_autorun_toggled().connect(sigc::mem_fun(*this, &SliderWindow::onSettingsDialogAutostart));
		settingsDialog->signal_iconpack_changed().connect(sigc::mem_fun(*this, &SliderWindow::onSettingsDialogIconpack));
		settingsDialog->run();
		delete settingsDialog;
	}

}

void SliderWindow::runSettings()
{
	createSettingsDialog();
}

std::vector<std::string> &SliderWindow::getMixers()
{
	return settingsStr_->mixerList();
}

std::vector<std::string> &SliderWindow::getCardsList()
{
	return settingsStr_->cardList();
}

void SliderWindow::setActiveCard(int card)
{
	alsaWork_->setCardId(card);
	updateControls(card);
}

void SliderWindow::onSettingsDialogOk(settingsStr &str)
{
	settingsStr_->setCardId(str.cardId());
	settingsStr_->setMixerId(str.mixerId());
	updateControls(settingsStr_->cardId());
	if (settingsStr_->mixerList().size() > 0 ) {
		mixerName_ = settingsStr_->mixerList().at(settingsStr_->mixerId());
	}
	else {
		mixerName_ = "N/A";
	}
	settingsStr_->setNotebookOrientation(str.notebookOrientation());
	settingsStr_->setIsAutorun(str.isAutorun());
	volumeValue_ = alsaWork_->getAlsaVolume(mixerName_);
	volumeSlider_->set_value(volumeValue_);
	settingsStr_->setExternalMixer(str.externalMixer());
}

void SliderWindow::onSettingsDialogAutostart(bool isAutorun)
{
	settings_->setAutorun(isAutorun);
}

void SliderWindow::onSettingsDialogIconpack(const std::string &path, int id, bool value = false)
{
	(void)id;
	(void)value;
	const std::string tmpDir = Tools::getTmpDir();
	settingsStr_->setCurrIconPack(path);
	settings_->setCurrIconPack(path);
	if (path != Tools::defaultIconPack) {
		iconpacks_ = new iconpacks(path, tmpDir);
		iconpacks_->extract();
	}
	else {
		Tools::clearTempDir(tmpDir+"/");
	}
}

void SliderWindow::switchChanged(const std::string &name, int id, bool enabled)
{
	alsaWork_->setSwitch(settingsStr_->cardId(), name, id, enabled);
}

void SliderWindow::soundMuted(bool mute)
{
	alsaWork_->setMute(settingsStr_->cardId(), mixerName_, mute);
}

bool SliderWindow::getMuted()
{
	return !bool(alsaWork_->getMute(settingsStr_->cardId(), mixerName_));
}

void SliderWindow::updateControls(int cardId)
{
	//
	settingsStr_->clear(MIXERS);
	settingsStr_->clear(CARDS);
	settingsStr_->clearSwitches();
	//
	settingsStr_->setCardId(cardId);
	int id = settingsStr_->cardId();
	settingsStr_->setList(CARDS, alsaWork_->getCardsList());
	alsaWork_->setCardId(id);
	settingsStr_->setList(MIXERS, alsaWork_->getVolumeMixers(id));
	settingsStr_->addMixerSwitch(alsaWork_->getSwitchList(id));
	settingsStr_->setIsAutorun(settings_->getAutorun());
}

void SliderWindow::onExtMixerSignal()
{
	std::string mixer = settingsStr_->externalMixer();
	if (!mixer.empty()) {
		try {
			std::cout << system(mixer.c_str()) << std::endl;
		}
		catch (const std::exception &ex) {
			std::cerr << ex.what() << std::endl;
		}
	}
}
