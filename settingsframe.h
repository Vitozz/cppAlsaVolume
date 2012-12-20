#ifndef SETTINGSFRAME_H
#define SETTINGSFRAME_H
#include "sliderwindow.h"
#include "gtkmm/dialog.h"
#include "gtkmm/builder.h"
#include "gtkmm/button.h"
#include "gtkmm/combobox.h"
#include "gtkmm/entry.h"
#include "gtkmm/checkbutton.h"
#include "gtkmm/treeview.h"
#include "gtkmm/notebook.h"

class SettingsFrame : public Gtk::Dialog
{
public:
	SettingsFrame(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade);
	~SettingsFrame();
	void runDialog(SliderWindow *parent);
protected:
	void onTabPos();
	void onOkButton();
	void onCancelButton();
	bool onDeleteEvent(GdkEventAny* event);

private:
	SliderWindow *parent_;
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
};

#endif // SETTINGSFRAME_H
