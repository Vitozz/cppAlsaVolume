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
#include "glibmm.h"
#include <iostream>
#include <map>

SliderWindow::SliderWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade)
: Gtk::Window(cobject)
{
	mixerList_.reserve(mixerList_.size());
	cardList_.reserve(cardList_.size());
	switches_.captureSwitchList_.reserve(switches_.captureSwitchList_.size());
	switches_.playbackSwitchList_.reserve(switches_.playbackSwitchList_.size());
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
	mixerList_ = alsaWork_->getVolumeMixers(cardId_);
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
	switches_ = alsaWork_->getSwitchList(cardId_);
	settings_->setVersion(Tools::version);
	const std::string iPack = settings_->getCurrIconPack();
	if (!iPack.empty() && iPack != "default") {
		Tools::extractArchive(iPack, Tools::getTmpDir());
	}
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
	dialog->set_title("About AlsaVolume");
	dialog->set_program_name("Alsa Volume Changer");
	dialog->set_comments("Tray Alsa Volume Changer written using gtkmm");
	dialog->set_version(Tools::version);
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
		std::cerr << "BuilderError::sliderwindow.cpp::190 " << ex.what() << std::endl;
	}
	catch(const Glib::MarkupError& ex) {
		std::cerr << "MarkupError::sliderwindow.cpp::190 " << ex.what() << std::endl;
	}
	catch(const Glib::FileError& ex) {
		std::cerr << "FileError::sliderwindow.cpp::190 " << ex.what() << std::endl;
	}
	builder_->get_widget_derived("settingsDialog", settingsDialog);
	updateControls(cardId_);
	if (settingsDialog) {
		settingsStr *str = new settingsStr();
		str->cardId = cardId_;
		str->mixerId = mixerId_;
		str->cardList = cardList_;
		str->mixerList = mixerList_;
		str->switchList = switches_;
		str->notebookOrientation = orient_;
		str->iconPacks = Tools::getIconPacks();
		str->currIconPack = settings_->getCurrIconPack();
		str->isAutorun = settings_->getAutorun();
		settingsDialog->initParms(str);
		settingsDialog->signal_ok_pressed().connect(sigc::mem_fun(*this, &SliderWindow::onSettingsDialogOk));
		settingsDialog->signal_switches_toggled().connect(sigc::mem_fun(*this, &SliderWindow::switchChanged));
		settingsDialog->signal_autorun_toggled().connect(sigc::mem_fun(*this, &SliderWindow::onSettingsDialogAutostart));
		settingsDialog->signal_iconpack_changed().connect(sigc::mem_fun(*this, &SliderWindow::onSettingsDialogIconpack));
		settingsDialog->run();
		delete settingsDialog;
		delete str;
	}

}

void SliderWindow::runSettings()
{
	createSettingsDialog();
}

std::vector<std::string> SliderWindow::getMixers()
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
	updateControls(card);
}

void SliderWindow::onSettingsDialogOk(settingsStr str)
{
	cardId_ = str.cardId;
	mixerId_ = str.mixerId;
	updateControls(cardId_);
	if (mixerList_.size() > 0 ) {
		mixerName_ = mixerList_.at(mixerId_);
	}
	else {
		mixerName_ = "N/A";
	}
	orient_ = str.notebookOrientation;
	volumeValue_ = alsaWork_->getAlsaVolume(mixerName_);
	volumeSlider_->set_value(volumeValue_);
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
	settings_->setCurrIconPack(path);
	if (path != "default") {
		Tools::extractArchive(path, tmpDir);
	}
	else {
		Tools::clearTempDir(tmpDir+"/");
	}
}

void SliderWindow::switchChanged(const std::string &name, int id, bool enabled)
{
	alsaWork_->setSwitch(cardId_, name, id, enabled);
}

void SliderWindow::soundMuted(bool mute)
{
	alsaWork_->setMute(cardId_, mixerName_, mute);
}

bool SliderWindow::getMuted()
{
	bool muted = alsaWork_->getMute(cardId_, mixerName_);
	return !muted;
}

void SliderWindow::updateControls(int cardId)
{
	if (!mixerList_.empty())
		mixerList_.clear();
	if (!cardList_.empty())
		cardList_.clear();
	if (!switches_.captureSwitchList_.empty())
		switches_.captureSwitchList_.clear();
	if (!switches_.playbackSwitchList_.empty())
		switches_.playbackSwitchList_.clear();
	cardList_ = alsaWork_->getCardsList();
	alsaWork_->setCardId(cardId);
	mixerList_ = alsaWork_->getVolumeMixers(cardId);
	switches_ = alsaWork_->getSwitchList(cardId);
}
