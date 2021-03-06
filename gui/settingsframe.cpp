/*
 * settingsframe.cpp
 * Copyright (C) 2012-2019 Vitaly Tonkacheyev
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

#include "settingsframe.h"
#include "libintl.h"
#include <iostream>
#include <utility>

#define _(String) gettext(String)
#define STATUS _("Status")
#define PB_SWITCH_NAME _("Playback Switch")
#define CP_SWITCH_NAME _("Capture Switch")
#define EN_SWITCH_NAME _("Enumerated Control")

SettingsFrame::SettingsFrame(BaseObjectType* cobject,
                             const Glib::RefPtr<Gtk::Builder>& refGlade)
    : Gtk::Dialog(cobject),
      okButton_(nullptr),
      cancelButton_(nullptr),
      sndCardBox_(nullptr),
      mixerBox_(nullptr),
      playbackSwitchTree_(nullptr),
      captureSwitchTree_(nullptr),
      otherSwitchTree_(nullptr),
      isAutoRun_(nullptr),
      tabPos_(nullptr),
      tabWidget_(nullptr),
      pulseHBox_(nullptr),
      alsaHBox_(nullptr),
      usePulse_(nullptr),
      usePolling_(nullptr),
      #ifdef HAVE_PULSE
      pulseBox_(nullptr),
      pulseDev_(0),
      pulseCards_(Glib::RefPtr<Gtk::ListStore>()),
      #endif
      cards_(Glib::RefPtr<Gtk::ListStore>()),
      mixers_(Glib::RefPtr<Gtk::ListStore>()),
      pbSwitches_(Glib::RefPtr<Gtk::ListStore>()),
      capSwitches_(Glib::RefPtr<Gtk::ListStore>()),
      enumSwitches_(Glib::RefPtr<Gtk::ListStore>()),
      settings_(settingsStr::Ptr()),
      mixerId_(0),
      cardId_(0),
      isPulse_(false)
{
    //init all lists
    const Glib::RefPtr<Gtk::Builder>& builder = refGlade;
    builder->get_widget("ok_button", okButton_);
    builder->get_widget("cancel_button", cancelButton_);
    builder->get_widget("sndcardbox", sndCardBox_);
    builder->get_widget("mixerBox", mixerBox_);
    builder->get_widget("playbacktree", playbackSwitchTree_);
    builder->get_widget("capturetree", captureSwitchTree_);
    builder->get_widget("othertree", otherSwitchTree_);
    builder->get_widget("is_autorun", isAutoRun_);
    builder->get_widget("tabspos", tabPos_);
    builder->get_widget("tabwidget", tabWidget_);
    builder->get_widget("pulseBox", pulseHBox_);
    builder->get_widget("alsaBox", alsaHBox_);
    builder->get_widget("usePulse", usePulse_);
    builder->get_widget("usePolling", usePolling_);
#ifdef HAVE_PULSE
    builder->get_widget("pulseDevices", pulseBox_);
#endif
    //signals
    if (tabPos_)
        tabPos_->signal_toggled().connect(sigc::mem_fun(*this, &SettingsFrame::onTabPos));
    if (okButton_)
        okButton_->signal_pressed().connect(sigc::mem_fun(*this, &SettingsFrame::onOkButton));
    if (cancelButton_)
        cancelButton_->signal_pressed().connect(sigc::mem_fun(*this, &SettingsFrame::onCancelButton));
    if (isAutoRun_)
        isAutoRun_->signal_toggled().connect(sigc::mem_fun(*this, &SettingsFrame::onAutorunToggled));
    if (sndCardBox_)
        sndCardBox_->signal_changed().connect(sigc::mem_fun(*this, &SettingsFrame::sndBoxChanged));
    if (mixerBox_)
        mixerBox_->signal_changed().connect(sigc::mem_fun(*this, &SettingsFrame::mixerBoxChanged));
    if (usePolling_)
        usePolling_->signal_toggled().connect(sigc::mem_fun(*this, &SettingsFrame::onUsePollingToggled));
#ifdef HAVE_PULSE
    if (usePulse_)
        usePulse_->signal_toggled().connect(sigc::mem_fun(*this, &SettingsFrame::onPulseToggled));
    if (pulseBox_)
        pulseBox_->signal_changed().connect(sigc::mem_fun(*this, &SettingsFrame::onPulseDeviceChanged));
#else
    pulseHBox_->set_visible(false);
    usePulse_->set_visible(false);
#endif
    this->signal_delete_event().connect(sigc::mem_fun(*this, &SettingsFrame::onDeleteEvent));
    pulseHBox_->set_visible(false);
}

SettingsFrame::~SettingsFrame()
{
#ifdef HAVE_PULSE
    delete pulseBox_;
#endif
    delete usePulse_;
    delete pulseHBox_;
    delete okButton_;
    delete cancelButton_;
    delete sndCardBox_;
    delete mixerBox_;
    delete playbackSwitchTree_;
    delete captureSwitchTree_;
    delete otherSwitchTree_;
    delete isAutoRun_;
    delete tabPos_;
    delete tabWidget_;
}

void SettingsFrame::initParms(const settingsStr::Ptr &str)
{
    settings_ = str;
    if (tabPos_)
        tabPos_->set_active(settings_->notebookOrientation());
    if (isAutoRun_)
        isAutoRun_->set_active(settings_->isAutorun());
    if (usePolling_)
        usePolling_->set_active(settings_->usePolling());
#ifdef HAVE_PULSE
    if (usePulse_)
        usePulse_->set_active(settings_->usePulse());
    pulseDev_ = settings_->pulseDeviceId();
#endif
    mixerId_ = settings_->mixerId();
    cardId_ = settings_->cardId();
    setupTreeModels();
}

void SettingsFrame::setTabPos(bool orient)
{
    if (tabWidget_) {
        if(orient) {
            tabWidget_->set_tab_pos(Gtk::POS_TOP);
        }
        else {
            tabWidget_->set_tab_pos(Gtk::POS_LEFT);
        }
        settings_->setNotebookOrientation(orient);
    }
}

void SettingsFrame::onTabPos()
{
    setTabPos(tabPos_->get_active());
}

void SettingsFrame::onOkButton()
{
    settings_->setMixerId(mixerId_);
    settings_->setCardId(cardId_);
    response(OK_RESPONSE);
    hide();
}

void SettingsFrame::onCancelButton()
{
    response(CANCEL_RESPONSE);
    hide();
}

bool SettingsFrame::onDeleteEvent(GdkEventAny *event)
{
    if (event->type == GDK_DESTROY || event->type == GDK_DELETE)
        onCancelButton();
    return true;
}

void SettingsFrame::setupTreeModels()
{
    //treeview setup
    setupSoundCards();
#ifdef HAVE_PULSE
    setupPulseDevices();
#endif
}

void SettingsFrame::setupSoundCards()
{
    if (sndCardBox_ && settings_){
        sndCardBox_->clear();
        cards_ = Gtk::ListStore::create(m_Columns);
        sndCardBox_->set_model(cards_);
        Gtk::TreeModel::Row row;
        uint i = 0;
        for(const std::string &name : settings_->cardList()){
            row = *(cards_->append());
            row[m_Columns.m_col_name] = Glib::ustring(name);
            if (i == cardId_) {
                sndCardBox_->set_active(row);
            }
            ++i;
        }
        sndCardBox_->pack_start(m_Columns.m_col_name);
    }
}

#ifdef HAVE_PULSE
void SettingsFrame::setupPulseDevices()
{
    if(pulseBox_ && settings_) {
        pulseBox_->clear();
        pulseCards_ = Gtk::ListStore::create(m_Columns);
        pulseBox_->set_model(pulseCards_);
        Gtk::TreeModel::Row row;
        uint i = 0;
        for(const std::string &name : settings_->pulseDevices()){
            row = *(pulseCards_->append());
            row[m_Columns.m_col_name] = Glib::ustring(name);
            if (i == uint(pulseDev_)) {
                pulseBox_->set_active(row);
            }
            ++i;
        }
        pulseBox_->pack_start(m_Columns.m_col_name);
    }
}
#endif

void SettingsFrame::setupMixers()
{
    if (mixerBox_ && settings_) {
        mixerBox_->clear();
        mixers_ = Gtk::ListStore::create(m_Columns);
        mixerBox_->set_model(mixers_);
        if (!settings_->mixerList().empty()) {
            Gtk::TreeModel::Row row;
            uint i = 0;
            for(const std::string &name : settings_->mixerList()) {
                row = *(mixers_->append());
                row[m_Columns.m_col_name] = Glib::ustring(name);
                if (i == mixerId_) {
                    mixerBox_->set_active(row);
                }
                ++i;
            }
            mixerBox_->pack_start(m_Columns.m_col_name);
        }
    }
}

void SettingsFrame::updateSwitchTree()
{
    if (!settings_)
        return;

    if (playbackSwitchTree_) {
        playbackSwitchTree_->remove_all_columns();
        Gtk::CellRendererToggle *pcell = Gtk::manage(new Gtk::CellRendererToggle);
        pcell->set_activatable(true);
        pcell->signal_toggled().connect(sigc::mem_fun(*this, &SettingsFrame::onPlaybackCellToggled));
        if (pbSwitches_)
            pbSwitches_->clear();
        pbSwitches_ = Glib::RefPtr<Gtk::ListStore>(Gtk::ListStore::create(m_TColumns));
        playbackSwitchTree_->set_model(pbSwitches_);
        Gtk::TreeModel::Row row;
        const int colsCount = playbackSwitchTree_->append_column(STATUS, *pcell);
        Gtk::TreeViewColumn* pColumn = playbackSwitchTree_->get_column(colsCount -1);
        if (colsCount)
            pColumn->add_attribute(pcell->property_active(), m_TColumns.m_col_toggle);
        for(const switchcap &scap : settings_->switchList()->playbackSwitchList()) {
            row = *(pbSwitches_->append());
            row[m_TColumns.m_col_toggle] = scap.second;
            row[m_TColumns.m_col_name] = scap.first;
        }
        playbackSwitchTree_->append_column(PB_SWITCH_NAME, m_TColumns.m_col_name);
        playbackSwitchTree_->show_all_children();
    }
    if (captureSwitchTree_) {
        captureSwitchTree_->remove_all_columns();
        Gtk::CellRendererToggle *rcell = Gtk::manage(new Gtk::CellRendererToggle);
        rcell->set_activatable(true);
        rcell->set_radio(true);
        rcell->signal_toggled().connect(sigc::mem_fun(*this, &SettingsFrame::onCaptureCellToggled));
        if (capSwitches_)
            capSwitches_->clear();
        capSwitches_ = Glib::RefPtr<Gtk::ListStore>(Gtk::ListStore::create(m_TColumns));
        captureSwitchTree_->set_model(capSwitches_);
        Gtk::TreeModel::Row row;
        const int colsCount = captureSwitchTree_->append_column(STATUS, *rcell);
        Gtk::TreeViewColumn* pColumn = captureSwitchTree_->get_column(colsCount -1);
        if (colsCount)
            pColumn->add_attribute(rcell->property_active(), m_TColumns.m_col_toggle);
        for (const switchcap &scap : settings_->switchList()->captureSwitchList()) {
            row = *(capSwitches_->append());
            row[m_TColumns.m_col_toggle] = scap.second;
            row[m_TColumns.m_col_name] = scap.first;
        }
        captureSwitchTree_->append_column(CP_SWITCH_NAME, m_TColumns.m_col_name);
        captureSwitchTree_->show_all_children();
    }
    if (otherSwitchTree_) {
        otherSwitchTree_->remove_all_columns();
        Gtk::CellRendererToggle *ecell = Gtk::manage(new Gtk::CellRendererToggle);
        ecell->set_activatable(true);
        ecell->signal_toggled().connect(sigc::mem_fun(*this, &SettingsFrame::onEnumCellToggled));
        if (enumSwitches_)
            enumSwitches_->clear();
        enumSwitches_ = Glib::RefPtr<Gtk::ListStore>(Gtk::ListStore::create(m_TColumns));
        otherSwitchTree_->set_model(enumSwitches_);
        Gtk::TreeModel::Row row;
        const int colsCount = otherSwitchTree_->append_column(STATUS, *ecell);
        Gtk::TreeViewColumn* pColumn = otherSwitchTree_->get_column(colsCount -1);
        if (colsCount)
            pColumn->add_attribute(ecell->property_active(), m_TColumns.m_col_toggle);
        for(const switchcap &scap : settings_->switchList()->enumSwitchList()) {
            row = *(enumSwitches_->append());
            row[m_TColumns.m_col_toggle] = scap.second;
            row[m_TColumns.m_col_name] = scap.first;
        }
        otherSwitchTree_->append_column(EN_SWITCH_NAME, m_TColumns.m_col_name);
        otherSwitchTree_->show_all_children();
    }
}

void SettingsFrame::sndBoxChanged()
{
    cardId_ = uint(sndCardBox_->get_active_row_number());
    settings_->setCardId(cardId_);
    m_signal_sndcard_changed(int(cardId_));
}

#ifdef HAVE_PULSE
void SettingsFrame::onPulseDeviceChanged()
{
    pulseDev_ = pulseBox_->get_active_row_number();
    settings_->setPulseDeviceId(pulseDev_);
    m_signal_pulsedev_changed(pulseDev_);
}
#endif

void SettingsFrame::updateMixers(const std::vector<std::string> &mixers)
{
    settings_->setList(MIXERS, mixers);
    setupMixers();
}

void SettingsFrame::updateSwitches(const MixerSwitches::Ptr &slist)
{
    //settings_->clearSwitches();
    settings_->addMixerSwitch(slist);
    updateSwitchTree();
}

void SettingsFrame::mixerBoxChanged()
{
    const uint row = uint(mixerBox_->get_active_row_number());
    if (mixerId_ != row) {
        mixerId_ = row;
    }
}

void SettingsFrame::onPlaybackCellToggled(const Glib::ustring& path)
{
    Gtk::TreeModel::iterator it = pbSwitches_->get_iter(path);
    Gtk::TreeModel::Row row = *it;
    row[m_TColumns.m_col_toggle] = !bool(row.get_value(m_TColumns.m_col_toggle));
    m_type_toggled_signal(row.get_value(m_TColumns.m_col_name),
                          PLAYBACK,
                          bool(row.get_value(m_TColumns.m_col_toggle)));
}

void SettingsFrame::onCaptureCellToggled(const Glib::ustring& path)
{
    Gtk::TreeModel::Row row;
    std::for_each(capSwitches_->children().begin(), capSwitches_->children().end(), [&](Gtk::TreeModel::Row r){
        row = std::move(r);
        row[m_TColumns.m_col_toggle] = false;
    });
    Gtk::TreeModel::iterator iter = capSwitches_->get_iter(path);
    row = *iter;
    if (!bool(row.get_value(m_TColumns.m_col_toggle))) {
        row[m_TColumns.m_col_toggle] = true;
    }
    m_type_toggled_signal(row.get_value(m_TColumns.m_col_name),
                          CAPTURE,
                          bool(row.get_value(m_TColumns.m_col_toggle)));
}

void SettingsFrame::onEnumCellToggled(const Glib::ustring& path)
{
    Gtk::TreeModel::iterator it = enumSwitches_->get_iter(path);
    Gtk::TreeModel::Row row = *it;
    row[m_TColumns.m_col_toggle] = !bool(row.get_value(m_TColumns.m_col_toggle));
    m_type_toggled_signal(row.get_value(m_TColumns.m_col_name),
                          ENUM,
                          bool(row.get_value(m_TColumns.m_col_toggle)));
}

void SettingsFrame::onAutorunToggled()
{
    settings_->setIsAutorun(isAutoRun_->get_active());
}

#ifdef HAVE_PULSE
void SettingsFrame::onPulseToggled()
{
    isPulse_ = usePulse_->get_active();
    alsaHBox_->set_visible(!isPulse_);
    pulseHBox_->set_visible(isPulse_);
    m_signal_pulse_toggled(isPulse_);
}
#endif

void SettingsFrame::onUsePollingToggled()
{
    bool usePoll = usePolling_->get_active();
    settings_->setUsePolling(usePoll);
}

void SettingsFrame::disablePulseCheckButton()
{
    usePulse_->set_visible(false);
}

SettingsFrame::type_toggled_signal SettingsFrame::signal_switches_toggled()
{
    return m_type_toggled_signal;
}

SettingsFrame::type_int_signal SettingsFrame::signal_sndcard_changed()
{
    return m_signal_sndcard_changed;
}

#ifdef HAVE_PULSE
SettingsFrame::type_int_signal SettingsFrame::signal_pulsedevices_changed()
{
    return m_signal_pulsedev_changed;
}

SettingsFrame::type_bool_signal SettingsFrame::signal_pulsdev_toggled()
{
    return m_signal_pulse_toggled;
}
#endif
