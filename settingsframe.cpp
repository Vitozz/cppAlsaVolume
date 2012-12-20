#include "settingsframe.h"
#include <iostream>

SettingsFrame::SettingsFrame(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refGlade)
 : Gtk::Dialog(cobject),
   parent_(0),
   okButton_(0),
   cancelButton_(0),
   sndCardBox_(0),
   mixerBox_(0),
   extMixer_(0),
   switchTree_(0),
   iconPacks_(0),
   isAutoRun_(0),
   tabPos_(0),
   tabWidget_(0)
{
	Glib::RefPtr<Gtk::Builder> builder = refGlade;
	builder->get_widget("ok_button", okButton_);
	builder->get_widget("cancel_button", cancelButton_);
	builder->get_widget("sndcardbox", sndCardBox_);
	builder->get_widget("mixerBox", mixerBox_);
	builder->get_widget("ext_mixer", extMixer_);
	builder->get_widget("switchtree", switchTree_);
	builder->get_widget("iconpacks", iconPacks_);
	builder->get_widget("is_autorun", isAutoRun_);
	builder->get_widget("tabspos", tabPos_);
	builder->get_widget("tabwidget", tabWidget_);
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
	this->signal_delete_event().connect(sigc::mem_fun(*this, &SettingsFrame::onDeleteEvent));
}

SettingsFrame::~SettingsFrame()
{
	delete okButton_;
	delete cancelButton_;
	delete sndCardBox_;
	delete mixerBox_;
	delete extMixer_;
	delete switchTree_;
	delete iconPacks_;
	delete isAutoRun_;
	delete tabPos_;
	delete tabWidget_;
}

void SettingsFrame::runDialog(SliderWindow *parent)
{
	parent_ = parent;
	run();
}

void SettingsFrame::onTabPos()
{
	if(tabWidget_){
		if(tabPos_->get_active()) {
			tabWidget_->set_tab_pos(Gtk::POS_TOP);
		}
		else {
			tabWidget_->set_tab_pos(Gtk::POS_LEFT);
		}
	}
}

void SettingsFrame::onOkButton()
{
	this->destroy_();
}

void SettingsFrame::onCancelButton()
{
	onOkButton();
}

bool SettingsFrame::onDeleteEvent(GdkEventAny *event)
{
	if (event->type == GDK_DESTROY || event->type == GDK_DELETE)
		onOkButton();
	return true;
}
