#ifndef SETTINGSFRAME_H
#define SETTINGSFRAME_H
#include "sliderwindow.h"
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
protected:
	class ModelColumns : public Gtk::TreeModel::ColumnRecord
	{
		public:
			ModelColumns() { add(m_col_name); }
			Gtk::TreeModelColumn<Glib::ustring> m_col_name;
	};

	ModelColumns m_Columns;

	void onTabPos();
	void onOkButton();
	void onCancelButton();
	bool onDeleteEvent(GdkEventAny* event);
	void sndBoxChanged();
	void mixerBoxChanged();
	type_void_signal m_signal_ok_pressed;
private:
	void setupTreeModels();
	void setTabPos(bool orient);
private:
	Gtk::Button *okButton_;
	Gtk::Button *cancelButton_;
	Gtk::ComboBox *sndCardBox_;
	Gtk::ComboBox *mixerBox_;
	Gtk::Entry *extMixer_;
	Gtk::TreeView *switchTree_;
	Gtk::ComboBox *iconPacks_;
	Gtk::CheckButton *isAutoRun_;
	Gtk::CheckButton *tabPos_;
	Gtk::Notebook *tabWidget_;
	Glib::RefPtr<Gtk::ListStore> cards_;
	Glib::RefPtr<Gtk::ListStore> mixers_;
	settingsStr settings_;
};

#endif // SETTINGSFRAME_H
