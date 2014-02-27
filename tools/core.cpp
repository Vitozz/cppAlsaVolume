/*
 * core.cpp
 * Copyright (C) 2013 Vitaly Tonkacheyev
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

#include "core.h"
#include "gtkmm/builder.h"
#include "gtkmm/aboutdialog.h"
#include "glibmm/markup.h"
#include "glibmm/fileutils.h"
#include "../gui/settingsframe.h"
#include <iostream>

const std::string TITLE = "About AlsaVolume";
const std::string PROGNAME = "Alsa Volume Changer";
const std::string COMMENTS = "Tray Alsa Volume Changer written using gtkmm";
const std::string COPYRIGHT = "2012 (c) Vitaly Tonkacheyev (thetvg@gmail.com)";
const std::string WEBSITE = "http://sites.google.com/site/thesomeprojects/";
const std::string WEBSITELABEL = "Program Website";
const std::string VERSION = "0.1.8";

Core::Core(const Glib::RefPtr<Gtk::Builder> &refGlade)
{
	alsaWork_ = new AlsaWork();
	settings_ = new Settings();
	settingsStr_ = new settingsStr();
	//
	settingsDialog_ = 0;
	refGlade->get_widget_derived("settingsDialog", settingsDialog_);
	int cardId = settings_->getSoundCard();
	cardId = (settings_->getSoundCard() >=0) ? cardId : 0;
	updateControls(cardId);
	if (settingsDialog_) {
		settingsDialog_->signal_ok_pressed().connect(sigc::mem_fun(*this, &Core::onSettingsDialogOk));
		settingsDialog_->signal_switches_toggled().connect(sigc::mem_fun(*this, &Core::switchChanged));
		settingsDialog_->signal_autorun_toggled().connect(sigc::mem_fun(*this, &Core::onSettingsDialogAutostart));
		settingsDialog_->signal_sndcard_changed().connect(sigc::mem_fun(*this, &Core::updateControls));
	}
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
	volumeValue_ = alsaWork_->getAlsaVolume(settingsStr_->cardId(), mixerName_);
	settingsStr_->setNotebookOrientation(settings_->getNotebookOrientation());
	settingsStr_->addMixerSwitch(alsaWork_->getSwitchList(settingsStr_->cardId()));
	settings_->setVersion(VERSION);
	settingsStr_->setExternalMixer(settings_->getExternalMixer());
	if (settingsDialog_) {
		settingsDialog_->initParms(*settingsStr_);
	}
}

Core::~Core()
{
	delete alsaWork_;
	delete settings_;
	delete settingsStr_;
	delete settingsDialog_;
}

void Core::runAboutDialog()
{
	Gtk::AboutDialog *dialog = new Gtk::AboutDialog();
	dialog->set_title(TITLE);
	dialog->set_program_name(PROGNAME);
	dialog->set_comments(COMMENTS);
	dialog->set_version(VERSION);
	dialog->set_copyright(COPYRIGHT);
	dialog->set_website(WEBSITE);
	dialog->set_website_label(WEBSITELABEL);
	Glib::RefPtr<Gdk::Pixbuf> logo, icon;
	std::string iconName, logoName;
	logoName = Tools::getResPath("icons/volume.png");
	iconName = Tools::getResPath("icons/tb_icon100.png");
	logo = Gdk::Pixbuf::create_from_file(logoName);
	icon = Gdk::Pixbuf::create_from_file(iconName);
	dialog->set_icon(icon);
	dialog->set_logo(logo);
	dialog->run();
	delete dialog;
}

void Core::runSettings()
{
	if (settingsDialog_) {
		settingsDialog_->updateMixers(settingsStr_->mixerList());
		settingsDialog_->updateSwitches(settingsStr_->switchList());
		settingsDialog_->run();
	}
}

void Core::saveSettings()
{
	settings_->saveSoundCard(settingsStr_->cardId());
	settings_->saveMixer(std::string(mixerName_.c_str()));
	settings_->saveNotebookOrientation(settingsStr_->notebookOrientation());
	settings_->setExternalMixer(settingsStr_->externalMixer());
}

std::vector<std::string> &Core::getMixers()
{
	return settingsStr_->mixerList();
}

std::vector<std::string> &Core::getCardsList()
{
	return settingsStr_->cardList();
}

void Core::setActiveCard(int card)
{
	updateControls(card);
}

void Core::onSettingsDialogOk(settingsStr &str)
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
	volumeValue_ = alsaWork_->getAlsaVolume(settingsStr_->cardId(),mixerName_);
	m_signal_volume_changed(volumeValue_);
	m_signal_mixer_muted(getMuted(mixerName_));
	settingsStr_->setExternalMixer(str.externalMixer());
	saveSettings();
}

void Core::onSettingsDialogAutostart(bool isAutorun)
{
	settings_->setAutorun(isAutorun);
}

void Core::switchChanged(const std::string &name, int id, bool enabled)
{
	alsaWork_->setSwitch(settingsStr_->cardId(), name, id, enabled);
}

void Core::soundMuted(bool mute)
{
	alsaWork_->setMute(settingsStr_->cardId(), mixerName_, mute);
}

bool Core::getMuted(const std::string &mixer)
{
	return !bool(alsaWork_->getMute(settingsStr_->cardId(), mixer));
}

void Core::updateControls(int cardId)
{
	//
	settingsStr_->clear(MIXERS);
	settingsStr_->clear(CARDS);
	settingsStr_->clearSwitches();
	//
	int soundCardId = cardId;
	std::vector<std::string> cards = alsaWork_->getCardsList();
	settingsStr_->setList(CARDS, cards);
	soundCardId = (soundCardId >=0 && (soundCardId < (int)cards.size())) ? soundCardId : 0;
	std::vector<std::string> vmixers = alsaWork_->getVolumeMixers(soundCardId);
	if (vmixers.empty()) {
		std::vector<std::string>::iterator it = cards.begin();
		int index = 0;
		while (it != cards.end()) {
			vmixers = alsaWork_->getVolumeMixers(index);
			if(!vmixers.empty()) {
				soundCardId = index;
				break;
			}
			++index;
			++it;
		}
	}
	settingsStr_->setCardId(soundCardId);
	settingsStr_->setList(MIXERS, vmixers);
	settingsStr_->addMixerSwitch(alsaWork_->getSwitchList(soundCardId));
	settingsStr_->setIsAutorun(settings_->getAutorun());

	settingsDialog_->updateMixers(settingsStr_->mixerList());
	settingsDialog_->updateSwitches(settingsStr_->switchList());
}

void Core::onExtMixerSignal()
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

std::string Core::getSoundCardName() const
{
	return alsaWork_->getCardName(settingsStr_->cardId());
}

std::string Core::getActiveMixer() const
{
	return mixerName_;
}

double Core::getVolumeValue() const
{
	return volumeValue_;
}

void Core::onTrayIconScroll(double value)
{
	m_signal_volume_changed(value);
}

void Core::onVolumeSlider(double value)
{
	alsaWork_->setAlsaVolume(settingsStr_->cardId(), mixerName_, value);
	m_signal_value_changed(value, getSoundCardName(), getActiveMixer());
}

Core::type_double_signal Core::signal_volume_changed()
{
	return m_signal_volume_changed;
}

Core::type_volumevalue_signal Core::signal_value_changed()
{
	return m_signal_value_changed;
}

Core::type_bool_signal Core::signal_mixer_muted()
{
	return m_signal_mixer_muted;
}
