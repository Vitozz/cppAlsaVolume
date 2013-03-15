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

SettingsFrame::SettingsFrame(BaseObjectType* cobject,
			     const Glib::RefPtr<Gtk::Builder>& refGlade)
: Gtk::Dialog(cobject),
  okButton_(0),
  cancelButton_(0),
  sndCardBox_(0),
  mixerBox_(0),
  extMixer_(0),
  playbackSwitchTree_(0),
  captureSwitchTree_(0),
  otherSwitchTree_(0),
  iconPacks_(0),
  isAutoRun_(0),
  tabPos_(0),
  tabWidget_(0),
  cards_(0),
  mixers_(0)
{
	//init all lists
	settings_.cardList.reserve(settings_.cardList.size());
	settings_.mixerList.reserve(settings_.mixerList.size());
	settings_.switchList.playbackSwitchList_.reserve(settings_.switchList.playbackSwitchList_.size());
	settings_.switchList.captureSwitchList_.reserve(settings_.switchList.captureSwitchList_.size());
	Glib::RefPtr<Gtk::Builder> builder = refGlade;
	builder->get_widget("ok_button", okButton_);
	builder->get_widget("cancel_button", cancelButton_);
	builder->get_widget("sndcardbox", sndCardBox_);
	builder->get_widget("mixerBox", mixerBox_);
	builder->get_widget("ext_mixer", extMixer_);
	builder->get_widget("playbacktree", playbackSwitchTree_);
	builder->get_widget("capturetree", captureSwitchTree_);
	builder->get_widget("othertree", otherSwitchTree_);
	builder->get_widget("iconpacks", iconPacks_);
	builder->get_widget("is_autorun", isAutoRun_);
	builder->get_widget("tabspos", tabPos_);
	builder->get_widget("tabwidget", tabWidget_);
	//signals
	if (tabPos_) {
		tabPos_->signal_toggled().connect(sigc::mem_fun(*this, &SettingsFrame::onTabPos));
	}
	if (okButton_)
	{
		okButton_->signal_pressed().connect(sigc::mem_fun(*this, &SettingsFrame::onOkButton));
	}
	if (cancelButton_)
	{
		cancelButton_->signal_pressed().connect(sigc::mem_fun(*this, &SettingsFrame::onCancelButton));
	}
	if (isAutoRun_)
	{
		isAutoRun_->signal_toggled().connect(sigc::mem_fun(*this, &SettingsFrame::onAutorunToggled));
	}
	this->signal_delete_event().connect(sigc::mem_fun(*this, &SettingsFrame::onDeleteEvent));
	if (extMixer_) {
		extMixer_->set_sensitive(false);
	}
}

SettingsFrame::~SettingsFrame()
{
	delete okButton_;
	delete cancelButton_;
	delete sndCardBox_;
	delete mixerBox_;
	delete extMixer_;
	delete playbackSwitchTree_;
	delete captureSwitchTree_;
	delete otherSwitchTree_;
	delete iconPacks_;
	delete isAutoRun_;
	delete tabPos_;
	delete tabWidget_;
}

void SettingsFrame::initParms(settingsStr *str)
{
	settings_ = *str;
	if (tabPos_) {
		tabPos_->set_active(settings_.notebookOrientation);
	}
	if (isAutoRun_) {
		isAutoRun_->set_active(settings_.isAutorun);
	}
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
		settings_.notebookOrientation = orient;
	}
}

void SettingsFrame::onTabPos()
{
	setTabPos(tabPos_->get_active());
}

void SettingsFrame::onOkButton()
{
	if (!settings_.mixerId) {
		settings_.mixerId = 0;
	}
	m_signal_ok_pressed.emit(settings_);
	onCancelButton();
}

void SettingsFrame::onCancelButton()
{
	this->destroy_();
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
	if (sndCardBox_){
		cards_ = Glib::RefPtr<Gtk::ListStore>(Gtk::ListStore::create(m_Columns));
		sndCardBox_->set_model(cards_);
		sndCardBox_->signal_changed().connect(sigc::mem_fun(*this, &SettingsFrame::sndBoxChanged));
		Gtk::TreeModel::Row row;
		std::vector<std::string>::iterator it = settings_.cardList.begin();
		uint i = 0;
		while (it != settings_.cardList.end()) {
			row = *(cards_->append());
			row[m_Columns.m_col_name] = Glib::ustring(*it);
			if (i == settings_.cardId) {
				sndCardBox_->set_active(row);
			}
			it++;
			i++;
		}
		sndCardBox_->pack_start(m_Columns.m_col_name);
	}
	if (mixerBox_) {
		mixers_ = Glib::RefPtr<Gtk::ListStore>(Gtk::ListStore::create(m_Columns));
		mixerBox_->set_model(mixers_);
		mixerBox_->signal_changed().connect(sigc::mem_fun(*this, &SettingsFrame::mixerBoxChanged));
		Gtk::TreeModel::Row row;
		std::vector<std::string>::iterator it = settings_.mixerList.begin();
		uint i = 0;
		while (it != settings_.mixerList.end()) {
			row = *(mixers_->append());
			row[m_Columns.m_col_name] = Glib::ustring(*it);
			if (i == settings_.mixerId) {
				mixerBox_->set_active(row);
			}
			it++;
			i++;
		}
		mixerBox_->pack_start(m_Columns.m_col_name);
	}
	if (playbackSwitchTree_) {
		pbSwitches_ = Glib::RefPtr<Gtk::ListStore>(Gtk::ListStore::create(m_TColumns));
		playbackSwitchTree_->set_model(pbSwitches_);
		Gtk::TreeModel::Row row;
		Gtk::CellRendererToggle *pcell = Gtk::manage(new Gtk::CellRendererToggle);
		int colsCount = playbackSwitchTree_->append_column("Status", *pcell);
		pcell->set_activatable(true);
		Gtk::TreeViewColumn* pColumn = playbackSwitchTree_->get_column(colsCount -1);
		if (colsCount) {
			pColumn->add_attribute(pcell->property_active(), m_TColumns.m_col_toggle);
		}
		pcell->signal_toggled().connect(sigc::mem_fun(*this, &SettingsFrame::onPlaybackCellToggled));
		std::vector<switchcap>::iterator it = settings_.switchList.playbackSwitchList_.begin();
		switchcap sc;
		while (it != settings_.switchList.playbackSwitchList_.end()) {
			row = *(pbSwitches_->append());
			sc = *it;
			row[m_TColumns.m_col_toggle] = sc.enabled;
			row[m_TColumns.m_col_name] = sc.name;
			it++;
		}
		playbackSwitchTree_->append_column("Playback Switch", m_TColumns.m_col_name);
		playbackSwitchTree_->show_all_children();
	}
	if (captureSwitchTree_) {
		capSwitches_ = Glib::RefPtr<Gtk::ListStore>(Gtk::ListStore::create(m_TColumns));
		captureSwitchTree_->set_model(capSwitches_);
		Gtk::TreeModel::Row row;
		Gtk::CellRendererToggle *rcell = Gtk::manage(new Gtk::CellRendererToggle);
		int colsCount = captureSwitchTree_->append_column("Status", *rcell);
		rcell->set_activatable(true);
		rcell->set_radio(true);
		Gtk::TreeViewColumn* pColumn = captureSwitchTree_->get_column(colsCount -1);
		if (colsCount) {
			pColumn->add_attribute(rcell->property_active(), m_TColumns.m_col_toggle);
		}
		rcell->signal_toggled().connect(sigc::mem_fun(*this, &SettingsFrame::onCaptureCellToggled));
		std::vector<switchcap>::iterator it = settings_.switchList.captureSwitchList_.begin();
		switchcap sc;
		while (it != settings_.switchList.captureSwitchList_.end()) {
			row = *(capSwitches_->append());
			sc = *it;
			row[m_TColumns.m_col_toggle] = sc.enabled;
			row[m_TColumns.m_col_name] = sc.name;
			it++;
		}
		captureSwitchTree_->append_column("Capture Switch", m_TColumns.m_col_name);
		captureSwitchTree_->show_all_children();
	}
	if (otherSwitchTree_) {
		enumSwitches_ = Glib::RefPtr<Gtk::ListStore>(Gtk::ListStore::create(m_TColumns));
		otherSwitchTree_->set_model(enumSwitches_);
		Gtk::TreeModel::Row row;
		Gtk::CellRendererToggle *ecell = Gtk::manage(new Gtk::CellRendererToggle);
		int colsCount = otherSwitchTree_->append_column("Status", *ecell);
		ecell->set_activatable(true);
		Gtk::TreeViewColumn* pColumn = otherSwitchTree_->get_column(colsCount -1);
		if (colsCount) {
			pColumn->add_attribute(ecell->property_active(), m_TColumns.m_col_toggle);
		}
		ecell->signal_toggled().connect(sigc::mem_fun(*this, &SettingsFrame::onEnumCellToggled));
		std::vector<switchcap>::iterator it = settings_.switchList.enumSwitchList_.begin();
		switchcap sc;
		while (it != settings_.switchList.enumSwitchList_.end()) {
			row = *(enumSwitches_->append());
			sc = *it;
			row[m_TColumns.m_col_toggle] = sc.enabled;
			row[m_TColumns.m_col_name] = sc.name;
			it++;
		}
		otherSwitchTree_->append_column("Enumerated Control", m_TColumns.m_col_name);
		otherSwitchTree_->show_all_children();
	}
	if (iconPacks_){
		Glib::RefPtr<Gtk::ListStore> packs = Glib::RefPtr<Gtk::ListStore>(Gtk::ListStore::create(m_Columns));
		iconPacks_->set_model(packs);
		iconPacks_->signal_changed().connect(sigc::mem_fun(*this, &SettingsFrame::iconPackChanged));
		Gtk::TreeModel::Row row;
		std::vector<std::string>::iterator it = settings_.iconPacks.begin();
		std::string item;
		while (it != settings_.iconPacks.end()) {
			row = *(packs->append());
			item  = Tools::pathToFileName(*it);
			row[m_Columns.m_col_name] = Glib::ustring(item);
			if (settings_.currIconPack.empty() && (item == Tools::defaultIconPack)) {
				iconPacks_->set_active(row);
			}
			else if (item == Tools::pathToFileName(settings_.currIconPack)) {
				iconPacks_->set_active(row);
			}
			it++;
		}
		iconPacks_->pack_start(m_Columns.m_col_name);
	}
}

void SettingsFrame::sndBoxChanged()
{
	settings_.cardId = sndCardBox_->get_active_row_number();
}

void SettingsFrame::iconPackChanged()
{
	int id = iconPacks_->get_active_row_number();
	std::string path = settings_.iconPacks.at(id);
	m_signal_iconpack_changed.emit(path, id, false);
}

void SettingsFrame::mixerBoxChanged()
{
	settings_.mixerId = mixerBox_->get_active_row_number();
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
	m_type_toggled_signal.emit(row.get_value(m_TColumns.m_col_name),
				   PLAYBACK,
				   bool(row.get_value(m_TColumns.m_col_toggle)));
}

void SettingsFrame::onCaptureCellToggled(const Glib::ustring& path)
{
	Gtk::TreeModel::iterator iter = capSwitches_->children().begin();
	std::cout << capSwitches_->children().size() << std::endl;
	Gtk::TreeModel::Row row;
	while (iter != capSwitches_->children().end()) {
		row = *iter;
		row[m_TColumns.m_col_toggle] = false;
		iter++;
	}
	iter = capSwitches_->get_iter(path);
	row = *iter;
	if (!bool(row.get_value(m_TColumns.m_col_toggle))) {
		row[m_TColumns.m_col_toggle] = true;
	}
	m_type_toggled_signal.emit(row.get_value(m_TColumns.m_col_name),
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
	m_type_toggled_signal.emit(row.get_value(m_TColumns.m_col_name),
				   ENUM,
				   bool(row.get_value(m_TColumns.m_col_toggle)));
}

void SettingsFrame::onAutorunToggled()
{
	m_signal_autorun_toggled.emit(isAutoRun_->get_active());
}

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

SettingsFrame::type_toggled_signal SettingsFrame::signal_iconpack_changed()
{
	return m_signal_iconpack_changed;
}
