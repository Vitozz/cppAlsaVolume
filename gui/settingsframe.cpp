/*
 * settingsframe.cpp
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

#include "settingsframe.h"
#include <iostream>

const std::string STATUS = "Status";

SettingsFrame::SettingsFrame(BaseObjectType* cobject,
			     const Glib::RefPtr<Gtk::Builder>& refGlade)
: Gtk::Dialog(cobject),
  okButton_(0),
  cancelButton_(0),
  sndCardBox_(0),
  mixerBox_(0),
  playbackSwitchTree_(0),
  captureSwitchTree_(0),
  otherSwitchTree_(0),
  isAutoRun_(0),
  tabPos_(0),
  tabWidget_(0),
  pulseHBox_(0),
  cards_(0),
  mixers_(0)
{
	//init all lists
	Glib::RefPtr<Gtk::Builder> builder = refGlade;
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
	builder->get_widget("usePulse", usePulse_);
#ifdef HAVE_PULSE
	builder->get_widget("pulseDevices", pulseBox_);
#endif
	//signals
	if (tabPos_) {
		tabPos_->signal_toggled().connect(sigc::mem_fun(*this, &SettingsFrame::onTabPos));
	}
	if (okButton_) {
		okButton_->signal_pressed().connect(sigc::mem_fun(*this, &SettingsFrame::onOkButton));
	}
	if (cancelButton_) {
		cancelButton_->signal_pressed().connect(sigc::mem_fun(*this, &SettingsFrame::onCancelButton));
	}
	if (isAutoRun_) {
		isAutoRun_->signal_toggled().connect(sigc::mem_fun(*this, &SettingsFrame::onAutorunToggled));
	}
	if (sndCardBox_) {
		sndCardBox_->signal_changed().connect(sigc::mem_fun(*this, &SettingsFrame::sndBoxChanged));
	}
	if (mixerBox_) {
		mixerBox_->signal_changed().connect(sigc::mem_fun(*this, &SettingsFrame::mixerBoxChanged));
	}
#ifdef HAVE_PULSE
	if (usePulse_) {
		usePulse_->signal_toggled().connect(sigc::mem_fun(*this, &SettingsFrame::onPulseToggled));
	}

#else
	pulseHBox_->set_visible(false);
	usePulse_->set_visible(false);
#endif
	this->signal_delete_event().connect(sigc::mem_fun(*this, &SettingsFrame::onDeleteEvent));
	settings_ = new settingsStr();
	mixerId_ = 0;
	cardId_ = 0;
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
	delete settings_;
}

void SettingsFrame::initParms(settingsStr &str)
{
	if (settings_) {
		settings_ = 0;
	}
	settings_ = new settingsStr(str);
	if (tabPos_) {
		tabPos_->set_active(settings_->notebookOrientation());
	}
	if (isAutoRun_) {
		isAutoRun_->set_active(settings_->isAutorun());
	}
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
	if (mixerId_ < 0) {
		mixerId_ = 0;
		settings_->setMixerId(0);
	}
	else {
		settings_->setMixerId(mixerId_);
	}
	if (cardId_ < 0) {
		cardId_ = 0;
		settings_->setCardId(0);
	}
	else {
		settings_->setCardId(cardId_);
	}
	m_signal_ok_pressed(*settings_);
	this->hide();
}

void SettingsFrame::onCancelButton()
{
	mixerId_ = ((uint)mixerId_ == settings_->mixerId()) ? mixerId_ : settings_->mixerId();
	this->hide();
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
}

void SettingsFrame::setupSoundCards()
{
	if (sndCardBox_){
		sndCardBox_->clear();
		cards_ = Gtk::ListStore::create(m_Columns);
		sndCardBox_->set_model(cards_);
		Gtk::TreeModel::Row row;
		std::vector<std::string>::iterator it = settings_->cardList().begin();
		uint i = 0;
		while (it != settings_->cardList().end()) {
			row = *(cards_->append());
			row[m_Columns.m_col_name] = Glib::ustring(*it);
			if (i == (uint)cardId_) {
				sndCardBox_->set_active(row);
			}
			++it;
			++i;
		}
		sndCardBox_->pack_start(m_Columns.m_col_name);
	}
}

void SettingsFrame::setupMixers()
{
	if (mixerBox_) {
		mixerBox_->clear();
		mixers_ = Gtk::ListStore::create(m_Columns);
		mixerBox_->set_model(mixers_);
		if (settings_->mixerList().size() > 0) {
			Gtk::TreeModel::Row row;
			std::vector<std::string>::iterator it = settings_->mixerList().begin();
			uint i = 0;
			while (it != settings_->mixerList().end()) {
				row = *(mixers_->append());
				row[m_Columns.m_col_name] = Glib::ustring(*it);
				if (i == (uint)mixerId_) {
					mixerBox_->set_active(row);
				}
				++it;
				i++;
			}
			mixerBox_->pack_start(m_Columns.m_col_name);
		}
	}
}

void SettingsFrame::updateSwitchTree()
{
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
		int colsCount = playbackSwitchTree_->append_column(STATUS, *pcell);
		Gtk::TreeViewColumn* pColumn = playbackSwitchTree_->get_column(colsCount -1);
		if (colsCount) {
			pColumn->add_attribute(pcell->property_active(), m_TColumns.m_col_toggle);
		}
		std::vector<switchcap>::iterator it = settings_->switchList().playbackSwitchList().begin();
		while (it != settings_->switchList().playbackSwitchList().end()) {
			row = *(pbSwitches_->append());
			row[m_TColumns.m_col_toggle] = (*it).second;
			row[m_TColumns.m_col_name] = (*it).first;
			++it;
		}
		playbackSwitchTree_->append_column("Playback Switch", m_TColumns.m_col_name);
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
		int colsCount = captureSwitchTree_->append_column(STATUS, *rcell);
		Gtk::TreeViewColumn* pColumn = captureSwitchTree_->get_column(colsCount -1);
		if (colsCount) {
			pColumn->add_attribute(rcell->property_active(), m_TColumns.m_col_toggle);
		}
		std::vector<switchcap>::iterator it = settings_->switchList().captureSwitchList().begin();
		while (it != settings_->switchList().captureSwitchList().end()) {
			row = *(capSwitches_->append());
			row[m_TColumns.m_col_toggle] = (*it).second;
			row[m_TColumns.m_col_name] = (*it).first;
			++it;
		}
		captureSwitchTree_->append_column("Capture Switch", m_TColumns.m_col_name);
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
		int colsCount = otherSwitchTree_->append_column(STATUS, *ecell);
		Gtk::TreeViewColumn* pColumn = otherSwitchTree_->get_column(colsCount -1);
		if (colsCount) {
			pColumn->add_attribute(ecell->property_active(), m_TColumns.m_col_toggle);
		}
		std::vector<switchcap>::iterator it = settings_->switchList().enumSwitchList().begin();
		while (it != settings_->switchList().enumSwitchList().end()) {
			row = *(enumSwitches_->append());
			row[m_TColumns.m_col_toggle] = (*it).second;
			row[m_TColumns.m_col_name] = (*it).first;
			++it;
		}
		otherSwitchTree_->append_column("Enumerated Control", m_TColumns.m_col_name);
		otherSwitchTree_->show_all_children();
	}
}

void SettingsFrame::sndBoxChanged()
{
	cardId_ = sndCardBox_->get_active_row_number();
	settings_->setCardId(cardId_);
	m_signal_sndcard_changed(cardId_);
}

void SettingsFrame::updateMixers(const std::vector<std::string> &mixers)
{
	settings_->setList(MIXERS, mixers);
	setupMixers();
}

void SettingsFrame::updateSwitches(const MixerSwitches &slist)
{
	settings_->clearSwitches();
	settings_->addMixerSwitch(slist);
	updateSwitchTree();
}

void SettingsFrame::mixerBoxChanged()
{
	mixerId_ = mixerBox_->get_active_row_number();
}

void SettingsFrame::onPlaybackCellToggled(const Glib::ustring& path)
{
	Gtk::TreeModel::iterator it = pbSwitches_->get_iter(path);
	Gtk::TreeModel::Row row = *it;
	if (bool(row.get_value(m_TColumns.m_col_toggle))) {
		row[m_TColumns.m_col_toggle] = false;
	}
	else {
		row[m_TColumns.m_col_toggle] = true;
	}
	m_type_toggled_signal(row.get_value(m_TColumns.m_col_name),
			      PLAYBACK,
			      bool(row.get_value(m_TColumns.m_col_toggle)));
}

void SettingsFrame::onCaptureCellToggled(const Glib::ustring& path)
{
	Gtk::TreeModel::iterator iter = capSwitches_->children().begin();
	Gtk::TreeModel::Row row;
	while (iter != capSwitches_->children().end()) {
		row = *iter;
		row[m_TColumns.m_col_toggle] = false;
		++iter;
	}
	iter = capSwitches_->get_iter(path);
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
	if (bool(row.get_value(m_TColumns.m_col_toggle))) {
		row[m_TColumns.m_col_toggle] = false;
	}
	else {
		row[m_TColumns.m_col_toggle] = true;
	}
	m_type_toggled_signal(row.get_value(m_TColumns.m_col_name),
			      ENUM,
			      bool(row.get_value(m_TColumns.m_col_toggle)));
}

void SettingsFrame::onAutorunToggled()
{
	m_signal_autorun_toggled(isAutoRun_->get_active());
}

#ifdef HAVE_PULSE
void SettingsFrame::onPulseToggled()
{
	m_signal_pulse_toggled(usePulse_->get_active());
}
#endif

SettingsFrame::type_toggled_signal SettingsFrame::signal_switches_toggled()
{
	return m_type_toggled_signal;
}

SettingsFrame::type_void_signal SettingsFrame::signal_ok_pressed()
{
	return m_signal_ok_pressed;
}

SettingsFrame::type_bool_signal SettingsFrame::signal_autorun_toggled()
{
	return m_signal_autorun_toggled;
}

SettingsFrame::type_int_signal SettingsFrame::signal_sndcard_changed()
{
	return m_signal_sndcard_changed;
}
