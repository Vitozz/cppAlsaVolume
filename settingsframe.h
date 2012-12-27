#ifndef SETTINGSFRAME_H
#define SETTINGSFRAME_H
#include "tools.h"
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
	virtual ~SettingsFrame();
	void initParms(settingsStr str);
	//
	typedef sigc::signal<void, settingsStr> type_void_signal;
	type_void_signal signal_ok_pressed();
	typedef sigc::signal<void, std::string, int ,bool> type_toggled_signal;
	type_toggled_signal signal_switches_toggled();
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
	//signals
	type_void_signal m_signal_ok_pressed;
	type_toggled_signal m_type_toggled_signal;
private:
	void setupTreeModels();
	void setTabPos(bool orient);
private:
	Gtk::Button *okButton_;
	Gtk::Button *cancelButton_;
	Gtk::ComboBox *sndCardBox_;
	Gtk::ComboBox *mixerBox_;
	Gtk::Entry *extMixer_;
	Gtk::TreeView *playbackSwitchTree_;
	Gtk::TreeView *captureSwitchTree_;
	Gtk::TreeView *otherSwitchTree_;
	Gtk::ComboBox *iconPacks_;
	Gtk::CheckButton *isAutoRun_;
	Gtk::CheckButton *tabPos_;
	Gtk::Notebook *tabWidget_;
	Glib::RefPtr<Gtk::ListStore> cards_;
	Glib::RefPtr<Gtk::ListStore> mixers_;
	Glib::RefPtr<Gtk::ListStore> pbSwitches_;
	Glib::RefPtr<Gtk::ListStore> capSwitches_;
	Glib::RefPtr<Gtk::ListStore> enumSwitches_;
	settingsStr settings_;
};

#endif // SETTINGSFRAME_H
