/*
 * settingsframe.h
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

#ifndef SETTINGSFRAME_H
#define SETTINGSFRAME_H
#include "../tools/settingsstr.h"
#include "gtkmm/dialog.h"
#include "gtkmm/builder.h"
#include "gtkmm/button.h"
#include "gtkmm/combobox.h"
#include "gtkmm/entry.h"
#include "gtkmm/checkbutton.h"
#include "gtkmm/treeview.h"
#include "gtkmm/notebook.h"
#include "gtkmm/liststore.h"
#include <vector>

class SettingsFrame : public Gtk::Dialog
{
public:
	SettingsFrame(BaseObjectType* cobject,
		      const Glib::RefPtr<Gtk::Builder>& refGlade);
	~SettingsFrame();
	enum RESPONCES {
		OK_RESPONSE = 1,
		CANCEL_RESPONSE = 0
	};
	void initParms(const settingsStr::Ptr &str);
	void updateControls(const settingsStr::Ptr &str);
	void updateMixers(const std::vector<std::string> &mixers);
	void updateSwitches(const MixerSwitches::Ptr &slist);
	void disablePulseCheckButton();
	settingsStr::Ptr getSettings() const;
	//
	typedef sigc::signal<void, std::string, int ,bool> type_toggled_signal;
	type_toggled_signal signal_switches_toggled();
	typedef sigc::signal<void, bool> type_bool_signal;
	typedef sigc::signal<void, int> type_int_signal;
	type_int_signal signal_sndcard_changed();
	type_int_signal signal_pulsedevices_changed();
	type_bool_signal signal_pulsdev_toggled();
protected:
	class ModelCheckBox : public Gtk::TreeModel::ColumnRecord
	{
	public:
		inline ModelCheckBox() { add(m_col_name); }
		Gtk::TreeModelColumn<Glib::ustring> m_col_name;
	};
	ModelCheckBox m_Columns;
	class ModelTreeView : public Gtk::TreeModel::ColumnRecord
	{
	public:
		inline ModelTreeView() { add(m_col_toggle); add(m_col_name); }
		Gtk::TreeModelColumn<bool> m_col_toggle;
		Gtk::TreeModelColumn<Glib::ustring> m_col_name;
	};
	ModelTreeView m_TColumns;
	void onTabPos();
	void onOkButton();
	void onCancelButton();
	bool onDeleteEvent(GdkEventAny* event);
	void sndBoxChanged();
	void mixerBoxChanged();
	void onPlaybackCellToggled(const Glib::ustring &path);
	void onCaptureCellToggled(const Glib::ustring &path);
	void onEnumCellToggled(const Glib::ustring &path);
	void onAutorunToggled();
	void onUsePollingToggled();
#ifdef HAVE_PULSE
	void onPulseToggled();
	void onPulseDeviceChanged();
#endif
	//signals
	type_toggled_signal m_type_toggled_signal;
#ifdef HAVE_PULSE
	type_bool_signal m_signal_pulse_toggled;
	type_int_signal m_signal_pulsedev_changed;
#endif
	type_int_signal m_signal_sndcard_changed;

private:
	void setupTreeModels();
	void setupSoundCards();
#ifdef HAVE_PULSE
	void setupPulseDevices();
#endif
	void setupMixers();
	void updateSwitchTree();
	void setTabPos(bool orient);

private:
	Gtk::Button *okButton_;
	Gtk::Button *cancelButton_;
	Gtk::ComboBox *sndCardBox_;
	Gtk::ComboBox *mixerBox_;
	Gtk::TreeView *playbackSwitchTree_;
	Gtk::TreeView *captureSwitchTree_;
	Gtk::TreeView *otherSwitchTree_;
	Gtk::CheckButton *isAutoRun_;
	Gtk::CheckButton *tabPos_;
	Gtk::Notebook *tabWidget_;
	Gtk::Box *pulseHBox_;
	Gtk::Box *alsaHBox_;
	Gtk::CheckButton *usePulse_;
	Gtk::CheckButton *usePolling_;
#ifdef HAVE_PULSE
	Gtk::ComboBox *pulseBox_;
	int pulseDev_;
	Glib::RefPtr<Gtk::ListStore> pulseCards_;
#endif
	Glib::RefPtr<Gtk::ListStore> cards_;
	Glib::RefPtr<Gtk::ListStore> mixers_;
	Glib::RefPtr<Gtk::ListStore> pbSwitches_;
	Glib::RefPtr<Gtk::ListStore> capSwitches_;
	Glib::RefPtr<Gtk::ListStore> enumSwitches_;
	settingsStr::Ptr settings_;
	int mixerId_;
	int cardId_;
	bool isPulse_;
};

#endif // SETTINGSFRAME_H
