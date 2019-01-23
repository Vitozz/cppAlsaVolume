/*
 * core.cpp
 * Copyright (C) 2013-2019 Vitaly Tonkacheyev
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "core.h"
#include "gtkmm/builder.h"
#include "gtkmm/aboutdialog.h"
#include "gtkmm/messagedialog.h"
#include "glibmm/markup.h"
#include "glibmm/fileutils.h"
#include "glibmm/main.h"
#include "../gui/settingsframe.h"
#include <iostream>
#include "libintl.h"
#define _(String) gettext(String)
#define N_(String) gettext_noop (String)

#define TITLE _("About AlsaVolume")
#define PROGNAME _("Alsa Volume Changer")
#define COMMENTS _("Tray Alsa Volume Changer written using gtkmm")
#define COPYRIGHT _("2012-2018 (c) Vitaly Tonkacheyev (thetvg@gmail.com)")
#define WEBSITE "http://sites.google.com/site/thesomeprojects/"
#define WEBSITELABEL _("Program Website")
#define VERSION "0.3.2"

#define POLLING_INTERVAL 2000

Core::Core(const Glib::RefPtr<Gtk::Builder> &refGlade)
    : settings_(Settings::Ptr(new Settings())),
      alsaWork_(AlsaWork::Ptr(new AlsaWork())),
      settingsStr_(settingsStr::Ptr(new settingsStr())),
      mixerName_(settings_->getMixer()),
      volumeValue_(0.0),
      pollVolume_(0.0),
      settingsDialog_(nullptr),
      isPulse_(false),
      isMuted_(false)
{
#ifdef HAVE_PULSE
    isPulse_ = settings_->usePulse();
    initPulseAudio();
#else
    isPulse_ = false;
#endif
    settingsStr_->setUsePulse(isPulse_);
    alsaCards_ = alsaWork_->getCardsList();
    settingsStr_->setList(CARDS, alsaCards_);
    settingsStr_->setMixerId(settings_->getMixerId());
    int cardId = settings_->getSoundCard();
    cardId = (alsaWork_->cardExists(settings_->getSoundCard())) ? cardId : alsaWork_->getFirstCardWithMixers();
    if (mixerName_.empty()) {
        mixerName_ = alsaWork_->getMixerName(settingsStr_->mixerId());
    }
    updateControls(cardId);
#ifdef IS_DEBUG
    std::cout << "Id in settings - " << settingsStr_->mixerId() << std::endl;
#endif
    pollVolume_ = volumeValue_;
    settingsStr_->setNotebookOrientation(settings_->getNotebookOrientation());
    settings_->setVersion(VERSION);
    settingsStr_->setUsePolling(settings_->usePolling());
    refGlade->get_widget_derived("settingsDialog", settingsDialog_);
    //connect signals
    if (settingsDialog_) {
        signal_switches_ = settingsDialog_->signal_switches_toggled().connect(sigc::mem_fun(*this, &Core::switchChanged));
        signal_sndcard_ = settingsDialog_->signal_sndcard_changed().connect(sigc::mem_fun(*this, &Core::updateControls));
#ifdef HAVE_PULSE
        signal_pulsdev_ = settingsDialog_->signal_pulsdev_toggled().connect(sigc::mem_fun(*this, &Core::onSettingsDialogUsePulse));
        signal_pulsedevices_ = settingsDialog_->signal_pulsedevices_changed().connect(sigc::mem_fun(*this, &Core::updatePulseDevices));
#endif
    }
    signal_timer_ = Glib::signal_timeout().connect(sigc::mem_fun(*this,&Core::onTimeout), POLLING_INTERVAL);
}

Core::~Core()
{
    if (settingsDialog_)
        delete settingsDialog_;
}

void Core::runAboutDialog()
{
    std::shared_ptr<Gtk::AboutDialog> dialog(new Gtk::AboutDialog());
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

#ifdef HAVE_PULSE
void Core::initPulseAudio()
{
    if (!pulse_) {
        pulse_ = PulseCore::Ptr(new PulseCore("alsavolume"));
        if (pulse_->available()) {
            pulseDevice_ = settings_->pulseDeviceName();
            if (pulseDevice_.empty() || !pulse_->deviceNameExists(pulseDevice_)) {
                pulseDevice_ = pulse_->defaultSink();
            }
            pulse_->setCurrentDevice(pulseDevice_);
            updatePulseDevices(pulse_->getCurrentDeviceIndex());
        }
        else {
            errorDialog(_("Can't start PulseAudio! Using Alsa by default"));
            pulse_.reset();
            isPulse_ = false;
            settingsStr_->setUsePulse(isPulse_);
            settings_->setUsePulse(isPulse_);
        }
    }
}
#endif

void Core::errorDialog(const std::string &errorMessage)
{
    std::shared_ptr<Gtk::MessageDialog> warn_(new Gtk::MessageDialog(Glib::ustring(errorMessage)));
    warn_->run();
}

void Core::blockAllSignals(bool isblock)
{
    signal_switches_.block(isblock);
    signal_sndcard_.block(isblock);
#ifdef HAVE_PULSE
    signal_pulsdev_.block(isblock);
    signal_pulsedevices_.block(isblock);
#endif
}

void Core::runSettings()
{
    if (settingsDialog_) {
#ifdef HAVE_PULSE
        if (pulse_) {
            updatePulseDevices(pulse_->getCurrentDeviceIndex());
        }
        else {
            settingsDialog_->disablePulseCheckButton();
        }
#endif
        blockAllSignals(true);
        settingsDialog_->initParms(settingsStr_);
        settingsDialog_->updateMixers(settingsStr_->mixerList());
        settingsDialog_->updateSwitches(settingsStr_->switchList());
        blockAllSignals(false);
        int response = settingsDialog_->run();
        if ( response == settingsDialog_->OK_RESPONSE ) {
            onSettingsDialogOk(settingsDialog_->getSettings());
        }
    }
}

void Core::saveSettings()
{
    settings_->saveSoundCard(settingsStr_->cardId());
    settings_->saveMixer(std::string(mixerName_.c_str()));
    settings_->saveNotebookOrientation(settingsStr_->notebookOrientation());
    settings_->setUsePulse(isPulse_);
    settings_->setAutorun(settingsStr_->isAutorun());
    settings_->setUsePolling(settingsStr_->usePolling());
    settings_->saveMixerId(settingsStr_->mixerId());
#ifdef HAVE_PULSE
    if (pulse_) {
        settings_->savePulseDeviceName(pulseDevice_);
    }
#endif
}

void Core::onSettingsDialogOk(const settingsStr::Ptr &str)
{
    settingsStr_->setCardId(str->cardId());
    settingsStr_->setMixerId(str->mixerId());
    settingsStr_->setNotebookOrientation(str->notebookOrientation());
    settingsStr_->setIsAutorun(str->isAutorun());
    settingsStr_->setUsePolling(str->usePolling());
    updateControls(settingsStr_->cardId());
#ifdef HAVE_PULSE
    if (isPulse_ && pulse_) {
        volumeValue_ = pulse_->getVolume();
    }
#endif
    saveSettings();
}

#ifdef HAVE_PULSE
void Core::onSettingsDialogUsePulse(bool isPulse)
{
    if (pulse_) {
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
    signal_timer_.block(true);
    if (!isPulse_) {
        isMuted_ = mute;
        alsaWork_->setMute(mute);
    }
#ifdef HAVE_PULSE
    else if (pulse_) {
        isMuted_ = mute;
        pulse_->setMute(mute);
    }
#endif
    signal_timer_.block(false);
}

bool Core::getMuted()
{
    if (!isPulse_) {
        isMuted_ = !alsaWork_->getMute();
        return isMuted_;
    }
#ifdef HAVE_PULSE
    else if (pulse_) {
        isMuted_ = pulse_->getMute();
        return isMuted_;
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
    settingsStr_->setCardId(soundCardId);
    settingsStr_->setList(MIXERS, alsaWork_->getVolumeMixers());
    settingsStr_->addMixerSwitch(alsaWork_->getSwitchList());
    const std::string newName = alsaWork_->getMixerName(settingsStr_->mixerId());
    if (mixerName_ != newName) {
        mixerName_ = newName;
        alsaWork_->setCurrentMixer(mixerName_);
    }
    if (!isPulse_) {
        volumeValue_ = alsaWork_->getAlsaVolume();
    }
#ifdef HAVE_PULSE
    else if (pulse_) {
        volumeValue_ = pulse_->getVolume();
    }
#endif
    m_signal_volume_changed(volumeValue_);
    updateTrayIcon(volumeValue_);
    m_signal_mixer_muted(getMuted());
    if(settingsDialog_) {
        blockAllSignals(true);
        settingsDialog_->updateMixers(settingsStr_->mixerList());
        settingsDialog_->updateSwitches(settingsStr_->switchList());
        blockAllSignals(false);
    }
}

#ifdef HAVE_PULSE
void Core::updatePulseDevices(int deviceId)
{
    if (pulse_) {
        const std::string olddev = settingsStr_->pulseDeviceName();
        pulse_->refreshDevices();
        settingsStr_->setPulseDevices(pulse_->getCardList());
        const std::string currDev = pulse_->getDeviceNameByIndex(deviceId);
        pulse_->setCurrentDevice(currDev);
        pulseDevice_ = currDev;
        pulseDeviceDesc_ = pulse_->getDeviceDescription(pulseDevice_);
        settingsStr_->setPulseDeviceName(pulseDevice_);
        settingsStr_->setPulseDeviceDesc(pulseDeviceDesc_);
        settingsStr_->setPulseDeviceId(pulse_->getCurrentDeviceIndex());
        if ( olddev != currDev ) {
            updateControls(pulse_->getCardIndex());
        }
    }
}
#endif

std::string Core::getSoundCardName() const
{
    std::string result;
    if (!isPulse_) {
        result = alsaWork_->getCardName(settingsStr_->cardId());
    }
#ifdef HAVE_PULSE
    else if (pulse_) {
        result = pulseDeviceDesc_;
    }
#endif
    return result;
}

std::string Core::getActiveMixer() const
{
    return ((!isPulse_) ? alsaWork_->getCurrentMixerName() : std::string());
}

double Core::getVolumeValue() const
{
    if (!isPulse_) {
        return alsaWork_->getAlsaVolume();
    }
#ifdef HAVE_PULSE
    else if (pulse_) {
        return pulse_->getVolume();
    }
#endif
    return 0.0;
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
    m_signal_volume_changed(volumeValue_); //send signal to sliderwindow
}

void Core::onVolumeSlider(double value)
{
    signal_timer_.block(true);
    volumeValue_ = value;
    pollVolume_ = value;
    if (!isPulse_) {
        alsaWork_->setAlsaVolume(value);
        pollVolume_ = alsaWork_->getAlsaVolume();
    }
#ifdef HAVE_PULSE
    else if (pulse_) {
        pulse_->setVolume(int(value));
    }
#endif
    updateTrayIcon(value);
    signal_timer_.block(false);
}

void Core::updateTrayIcon(double value)
{
    if (!isPulse_) {
        const std::string mixer = getActiveMixer();
        if (!mixer.empty()) {
            m_signal_value_changed(value, getSoundCardName(), mixer);
        }
        else {
            errorDialog(_("Sound card not contains any volume control mixers"));
            m_signal_value_changed(value, getSoundCardName(), std::string("N/A"));
        }
    }
    else {
        m_signal_value_changed(value, getSoundCardName(), std::string());
    }
}

bool Core::onTimeout()
{
    if (settingsStr_->usePolling()) {
        if (!isPulse_) {
#ifdef IS_DEBUG
            std::cout << "mixerName= " << mixerName_ << std::endl;
            std::cout << "CurrentMixerName= " << alsaWork_->getCurrentMixerName() << std::endl;
#endif
            if (mixerName_ != alsaWork_->getCurrentMixerName()) {
                return true;
            }
            const double volume = alsaWork_->getAlsaVolume();
            bool ismute = !alsaWork_->getMute();
            if (pollVolume_ != volume) {
                pollVolume_ = volume;
                m_signal_volume_changed(pollVolume_);
            }
            if (ismute != isMuted_) {
                isMuted_ = ismute;
                m_signal_mixer_muted(isMuted_);
            }
        }
#ifdef HAVE_PULSE
        if (isPulse_ && pulse_) {
            const int index = pulse_->getCurrentDeviceIndex();
            if (pulseDevice_ != pulse_->getDeviceNameByIndex(index)) {
                return true;
            }
            const int volume = pulse_->getVolume();
            bool ismute = pulse_->getMute();
            if (pollVolume_ != volume) {
                pollVolume_ = volume;
                m_signal_volume_changed(pollVolume_);
            }
            if (ismute != isMuted_) {
                isMuted_ = ismute;
                m_signal_mixer_muted(isMuted_);
            }
        }

#endif
    }
    return true;
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
