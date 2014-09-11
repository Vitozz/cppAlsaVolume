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

#define TITLE "About AlsaVolume"
#define PROGNAME "Alsa Volume Changer"
#define COMMENTS "Tray Alsa Volume Changer written using gtkmm"
#define COPYRIGHT "2012 (c) Vitaly Tonkacheyev (thetvg@gmail.com)"
#define WEBSITE "http://sites.google.com/site/thesomeprojects/"
#define WEBSITELABEL "Program Website"
#define VERSION "0.1.9"

Core::Core(const Glib::RefPtr<Gtk::Builder> &refGlade)
: settings_(new Settings()),
  alsaWork_(new AlsaWork()),
  settingsStr_(new settingsStr()),
  mixerName_(settings_->getMixer()),
  volumeValue_(0.0),
  settingsDialog_(0)
{
#ifdef HAVE_PULSE
	pulse_ = new PulseCore("alsavolume");
	pulseDevice_ = pulse_->defaultSink();
	pulseDeviceDesc_ = pulse_->getDeviceDescription(pulseDevice_);
	pulseVolume_ = pulse_->getVolume();
	pulseMuted_ = pulse_->getMute();
	isPulse_ = true;
#else
	isPulse_ = false;
#endif
	//init settings dialog
	refGlade->get_widget_derived("settingsDialog", settingsDialog_);
	//get cardId from alsa or from settings
	int cardId = settings_->getSoundCard();
	cardId = (settings_->getSoundCard() >=0) ? cardId : 0;
	updateControls(cardId);
	//restore last mixer on start or use first available
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
	if (!isPulse_) {
		volumeValue_ = alsaWork_->getAlsaVolume();
	}
#ifdef HAVE_PULSE
	else {
		volumeValue_ = pulseVolume_;
	}
#endif
	settingsStr_->setNotebookOrientation(settings_->getNotebookOrientation());
	settingsStr_->addMixerSwitch(alsaWork_->getSwitchList());
	settings_->setVersion(VERSION);
	//connect signals
	if (settingsDialog_) {
		settingsDialog_->signal_ok_pressed().connect(sigc::mem_fun(*this, &Core::onSettingsDialogOk));
		settingsDialog_->signal_switches_toggled().connect(sigc::mem_fun(*this, &Core::switchChanged));
		settingsDialog_->signal_autorun_toggled().connect(sigc::mem_fun(*this, &Core::onSettingsDialogAutostart));
		settingsDialog_->signal_sndcard_changed().connect(sigc::mem_fun(*this, &Core::updateControls));
	}
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
	volumeValue_ = alsaWork_->getAlsaVolume();
	onVolumeSlider(volumeValue_);
	m_signal_mixer_muted(getMuted());
	settingsStr_->setExternalMixer(str.externalMixer());
	saveSettings();
}

void Core::onSettingsDialogAutostart(bool isAutorun)
{
	settings_->setAutorun(isAutorun);
}

void Core::switchChanged(const std::string &name, int id, bool enabled)
{
	alsaWork_->setSwitch(name, id, enabled);
}

void Core::soundMuted(bool mute)
{
	if (!isPulse_) {
		alsaWork_->setMute(mute);
	}
#ifdef HAVE_PULSE
	else {
		pulse_->setMute(mute);
	}
#endif
}

bool Core::getMuted()
{
	if (!isPulse_) {
		return !bool(alsaWork_->getMute());
	}
#ifdef HAVE_PULSE
	else {
		return pulse_->getMute();
	}
#endif
	return false;
}

void Core::updateControls(int cardId)
{
	settingsStr_->clear(MIXERS);
	settingsStr_->clear(CARDS);
	settingsStr_->clearSwitches();
	std::vector<std::string> cards = alsaWork_->getCardsList();
	settingsStr_->setList(CARDS, cards);
	int soundCardId = (cardId >=0 && (cardId < (int)cards.size())) ? cardId : 0;
	alsaWork_->setCurrentCard(soundCardId);
	if (!alsaWork_->haveVolumeMixers()) {
		soundCardId = alsaWork_->getFirstCardWithMixers();
		alsaWork_->setCurrentCard(soundCardId);
	}
	if (!mixerName_.empty()) {
		alsaWork_->setCurrentMixer(mixerName_);
	}
	settingsStr_->setCardId(soundCardId);
	settingsStr_->setList(MIXERS, alsaWork_->getVolumeMixers());
	settingsStr_->addMixerSwitch(alsaWork_->getSwitchList());
	settingsStr_->setIsAutorun(settings_->getAutorun());
	if(settingsDialog_) {
		settingsDialog_->updateMixers(settingsStr_->mixerList());
		settingsDialog_->updateSwitches(settingsStr_->switchList());
	}
}

std::string Core::getSoundCardName() const
{
	std::string result = std::string();
	if (!isPulse_) {
		result = alsaWork_->getCardName(settingsStr_->cardId());
	}
#ifdef HAVE_PULSE
	else {
		int id = pulse_->getCurrentDeviceIndex();
		result = pulse_->getDeviceDescription(pulse_->getDeviceNameByIndex(id));
	}
#endif
	return result;
}

std::string Core::getActiveMixer() const
{
	std::string result = std::string();
	if (!isPulse_) {
		result = mixerName_;
	}
#ifdef HAVE_PULSE
	else {
		result = pulseDeviceDesc_;
	}
#endif
	return std::string();
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
	if (!isPulse_) {
		alsaWork_->setAlsaVolume(value);
	}
#ifdef HAVE_PULSE
	else {
		pulse_->setVolume((int)value);
	}
#endif
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
