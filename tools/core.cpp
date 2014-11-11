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
#include "libintl.h"
#define _(String) gettext(String)
#define N_(String) gettext_noop (String)

#define TITLE _("About AlsaVolume")
#define PROGNAME _("Alsa Volume Changer")
#define COMMENTS _("Tray Alsa Volume Changer written using gtkmm")
#define COPYRIGHT _("2012-2014 (c) Vitaly Tonkacheyev (thetvg@gmail.com)")
#define WEBSITE "http://sites.google.com/site/thesomeprojects/"
#define WEBSITELABEL _("Program Website")
#define VERSION "0.2.4"

Core::Core(const Glib::RefPtr<Gtk::Builder> &refGlade)
: settings_(new Settings()),
  alsaWork_(new AlsaWork()),
  settingsStr_(new settingsStr()),
  mixerName_(settings_->getMixer()),
  volumeValue_(0.0),
  settingsDialog_(0),
  isPulse_(false)
{
	refGlade->get_widget_derived("settingsDialog", settingsDialog_);
#ifdef HAVE_PULSE
	pulse_ = new PulseCore("alsavolume");
	if (pulse_->available()) {
		isPulse_ = settings_->usePulse();
		settingsStr_->setUsePulse(isPulse_);
		settingsStr_->setPulseDevices(pulse_->getCardList());
		pulseDevice_ = settings_->pulseDeviceName();
		if (pulseDevice_.empty() || !pulse_->deviceNameExists(pulseDevice_)) {
			pulseDevice_ = pulse_->defaultSink();
		}
		pulse_->setCurrentDevice(pulseDevice_);
		pulseDeviceDesc_ = pulse_->getDeviceDescription(pulseDevice_);
		settingsStr_->setPulseDeviceName(pulseDevice_);
		settingsStr_->setPulseDeviceDesc(pulseDeviceDesc_);
		settingsStr_->setPulseDeviceId(pulse_->getCurrentDeviceIndex());
	}
	else {
		settingsStr_->setUsePulse(isPulse_);
	}
#else
	isPulse_ = false;
	settingsStr_->setUsePulse(isPulse_);
#endif
	alsaCards_ = alsaWork_->getCardsList();
	settingsStr_->setList(CARDS, alsaCards_);
	int cardId = settings_->getSoundCard();
	cardId = (alsaWork_->cardExists(settings_->getSoundCard())) ? cardId : alsaWork_->getFirstCardWithMixers();
	updateControls(cardId);
	if (!mixerName_.empty()) {
		alsaWork_->setCurrentMixer(mixerName_);
		settingsStr_->setMixerId(alsaWork_->getCurrentMixerId());
	}
	else {
		alsaWork_->setCurrentMixer(0);
		settingsStr_->setMixerId(0);
		mixerName_ = settingsStr_->mixerList().at(settingsStr_->mixerId());
	}
	if (!isPulse_) {
		volumeValue_ = alsaWork_->getAlsaVolume();
	}
#ifdef HAVE_PULSE
	else if (pulse_->available()) {
		volumeValue_ = pulse_->getVolume();
	}
#endif
	settingsStr_->setNotebookOrientation(settings_->getNotebookOrientation());
	settingsStr_->addMixerSwitch(alsaWork_->getSwitchList());
	settings_->setVersion(VERSION);
	//connect signals
	if (settingsDialog_) {
		signal_ok_ = settingsDialog_->signal_ok_pressed().connect(sigc::mem_fun(*this, &Core::onSettingsDialogOk));
		signal_switches_ = settingsDialog_->signal_switches_toggled().connect(sigc::mem_fun(*this, &Core::switchChanged));
		signal_autorun_ = settingsDialog_->signal_autorun_toggled().connect(sigc::mem_fun(*this, &Core::onSettingsDialogAutostart));
		signal_sndcard_ = settingsDialog_->signal_sndcard_changed().connect(sigc::mem_fun(*this, &Core::updateControls));
		signal_mixer_ = settingsDialog_->signal_mixer_changed().connect(sigc::mem_fun(*this, &Core::mixerChanged));
#ifdef HAVE_PULSE
		signal_pulsdev_ = settingsDialog_->signal_pulsdev_toggled().connect(sigc::mem_fun(*this, &Core::onSettingsDialogUsePulse));
		signal_pulsedevices_ = settingsDialog_->signal_pulsedevices_changed().connect(sigc::mem_fun(*this, &Core::updatePulseDevices));
#endif
	}
}

Core::~Core()
{
#ifdef HAVE_PULSE
	delete pulse_;
#endif
	delete alsaWork_;
	delete settings_;
	delete settingsStr_;
	delete settingsDialog_;
}

void Core::blockAllSignals(bool isblock)
{
	signal_ok_.block(isblock);
	signal_switches_.block(isblock);
	signal_autorun_.block(isblock);
	signal_sndcard_.block(isblock);
	signal_mixer_.block(isblock);
#ifdef HAVE_PULSE
	signal_pulsdev_.block(isblock);
	signal_pulsedevices_.block(isblock);
#endif
}

void Core::runAboutDialog()
{
	Glib::RefPtr<Gtk::AboutDialog> dialog (new Gtk::AboutDialog());
	dialog->set_title(TITLE);
	dialog->set_program_name(PROGNAME);
	dialog->set_comments(COMMENTS);
	dialog->set_version(VERSION);
	dialog->set_copyright(COPYRIGHT);
	dialog->set_website(WEBSITE);
	dialog->set_website_label(WEBSITELABEL);
	const std::string logoName = Tools::getResPath("icons/volume.png");
	const std::string iconName = Tools::getResPath("icons/tb_icon100.png");
	Glib::RefPtr<Gdk::Pixbuf> logo = Gdk::Pixbuf::create_from_file(logoName);
	Glib::RefPtr<Gdk::Pixbuf> icon = Gdk::Pixbuf::create_from_file(iconName);
	dialog->set_icon(icon);
	dialog->set_logo(logo);
	dialog->run();
}

void Core::runSettings()
{
	if (settingsDialog_) {
#ifdef HAVE_PULSE
		if (pulse_->available()) {
			updatePulseDevices(pulse_->getCurrentDeviceIndex());
		}
#endif
		blockAllSignals(true);
		settingsDialog_->initParms(*settingsStr_);
		blockAllSignals(false);
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
	settings_->setUsePulse(isPulse_);
#ifdef HAVE_PULSE
	if (pulse_->available()) {
		settings_->savePulseDeviceName(pulseDevice_);
	}
#endif
}

void Core::onSettingsDialogOk(settingsStr &str)
{
	settingsStr_->setCardId(str.cardId());
	settingsStr_->setMixerId(str.mixerId());
	settingsStr_->setNotebookOrientation(str.notebookOrientation());
	settingsStr_->setIsAutorun(str.isAutorun());
	updateControls(settingsStr_->cardId());
#ifdef HAVE_PULSE
	if (isPulse_ && pulse_->available()) {
		volumeValue_ = pulse_->getVolume();
	}
#endif
	saveSettings();
}

void Core::onSettingsDialogAutostart(bool isAutorun)
{
	settings_->setAutorun(isAutorun);
}

#ifdef HAVE_PULSE
void Core::onSettingsDialogUsePulse(bool isPulse)
{
	if (pulse_->available()) {
		isPulse_ = isPulse;
		settingsStr_->setUsePulse(isPulse);
		settings_->setUsePulse(isPulse);
		updateTrayIcon(volumeValue_);
	}
}
#endif

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
	else if (pulse_->available()) {
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
	else if (pulse_->available()) {
		return pulse_->getMute();
	}
#endif
	return false;
}

void Core::updateControls(int cardId)
{
	settingsStr_->clear(MIXERS);
	settingsStr_->clearSwitches();
	const int soundCardId = (alsaWork_->cardExists(cardId)) ? cardId : alsaWork_->getFirstCardWithMixers();
	alsaWork_->setCurrentCard(soundCardId);
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

#ifdef HAVE_PULSE
void Core::updatePulseDevices(int deviceId)
{
	if (isPulse_ && pulse_->available()) {
		pulse_->refreshDevices();
		settingsStr_->setPulseDevices(pulse_->getCardList());
		const std::string currDev = pulse_->getDeviceNameByIndex(deviceId);
		pulse_->setCurrentDevice(currDev);
		pulseDevice_ = currDev;
		pulseDeviceDesc_ = pulse_->getDeviceDescription(pulseDevice_);
		settingsStr_->setPulseDeviceName(pulseDevice_);
		settingsStr_->setPulseDeviceDesc(pulseDeviceDesc_);
		settingsStr_->setPulseDeviceId(pulse_->getCurrentDeviceIndex());
		isPulse_ = settings_->usePulse();
		settingsStr_->setUsePulse(isPulse_);
	}
}
#endif

std::string Core::getSoundCardName() const
{
	std::string result = std::string();
	if (!isPulse_) {
		result = alsaWork_->getCardName(settingsStr_->cardId());
	}
#ifdef HAVE_PULSE
	else if (pulse_->available()) {
		result = pulseDeviceDesc_;
	}
#endif
	return result;
}

std::string Core::getActiveMixer() const
{
	return alsaWork_->getCurrentMixerName();
}

double Core::getVolumeValue() const
{
	return volumeValue_;
}

void Core::onTrayIconScroll(double value)
{
	volumeValue_ += value;
	if (volumeValue_ >= 100) {
		volumeValue_ = 100;
	}
	else if (volumeValue_ <= 0){
		volumeValue_ = 0;
	}
	m_signal_volume_changed(volumeValue_);
}

void Core::onVolumeSlider(double value)
{
	volumeValue_ = value;
	if (!isPulse_) {
		alsaWork_->setAlsaVolume(value);
	}
#ifdef HAVE_PULSE
	else if (pulse_->available()) {
		pulse_->setVolume((int)value);
	}
#endif
	updateTrayIcon(value);
}

void Core::updateTrayIcon(double value)
{
	if (!isPulse_) {
		const std::string mixer = getActiveMixer();
		if (!mixer.empty()) {
			m_signal_value_changed(value, getSoundCardName(), mixer);
		}
		else {
			m_signal_value_changed(value, getSoundCardName(), std::string("N/A"));
		}
	}
	else {
		m_signal_value_changed(value, getSoundCardName(), std::string(""));
	}
}

void Core::mixerChanged(int mixerId)
{
	alsaWork_->setCurrentMixer(mixerId);
	if (!isPulse_) {
		volumeValue_ = alsaWork_->getAlsaVolume();
	}
#ifdef HAVE_PULSE
	else if (pulse_->available()) {
		volumeValue_ = pulse_->getVolume();
	}
#endif
	m_signal_volume_changed(volumeValue_);
	updateTrayIcon(volumeValue_);
	m_signal_mixer_muted(getMuted());
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
