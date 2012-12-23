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
   switchTree_(0),
   iconPacks_(0),
   isAutoRun_(0),
   tabPos_(0),
   tabWidget_(0),
   cards_(0),
   mixers_(0)
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

void SettingsFrame::initParms(settingsStr str)
{
	settings_ = str;
	tabPos_->set_active(settings_.notebookOrientation);
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
	m_signal_ok_pressed.emit(settings_);
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
		for (int i = 0; i < static_cast<int>(settings_.cardList.size()); i++) {
			row = *(cards_->append());
			row[m_Columns.m_col_name] = Glib::ustring(settings_.cardList.at(i));
			if (i == settings_.cardId) {
				sndCardBox_->set_active(row);
			}
		}
		sndCardBox_->pack_start(m_Columns.m_col_name);
	}
	if (mixerBox_) {
		mixers_ = Glib::RefPtr<Gtk::ListStore>(Gtk::ListStore::create(m_Columns));
		mixerBox_->set_model(mixers_);
		mixerBox_->signal_changed().connect(sigc::mem_fun(*this, &SettingsFrame::mixerBoxChanged));
		Gtk::TreeModel::Row row;
		for (int i = 0; i < (int)settings_.mixerList.size(); i++) {
			row = *(mixers_->append());
			row[m_Columns.m_col_name] = Glib::ustring(settings_.mixerList.at(i));
			if (i == settings_.mixerId) {
				mixerBox_->set_active(row);
			}
		}
		mixerBox_->pack_start(m_Columns.m_col_name);
	}
	if (switchTree_) {
		switches_ = Glib::RefPtr<Gtk::ListStore>(Gtk::ListStore::create(m_TColumns));
		switchTree_->set_model(switches_);
		Gtk::TreeModel::Row row;
		Gtk::CellRendererToggle *cell = Gtk::manage(new Gtk::CellRendererToggle);
		int colsCount = switchTree_->append_column("Status", *cell);
		cell->set_activatable(true);
		Gtk::TreeViewColumn* pColumn = switchTree_->get_column(colsCount -1);
		if (colsCount) {
			pColumn->add_attribute(cell->property_active(), m_TColumns.m_col_toggle);
		}
		cell->signal_toggled().connect(sigc::mem_fun(*this, &SettingsFrame::onCellToggled));
		for (int i =0; i < (int)settings_.switchList.size(); i++) {
			row = *(switches_->append());
			row[m_TColumns.m_col_name] = Glib::ustring(settings_.switchList.at(i));
		}
		switchTree_->append_column("Switch", m_TColumns.m_col_name);
		switchTree_->show_all_children();
	}
}

void SettingsFrame::sndBoxChanged()
{
	settings_.cardId = sndCardBox_->get_active_row_number();
	std::cout << settings_.cardId << std::endl;
}

void SettingsFrame::mixerBoxChanged()
{
	settings_.mixerId = mixerBox_->get_active_row_number();
	//example howto get name from TreeModel
	/*Gtk::TreeModel::iterator iter = mixerBox_->get_active();
	if(iter)
	{
		Gtk::TreeModel::Row row = *iter;
		if(row) {
			mixerName_ = row[m_Columns.m_col_name];
			std::cout << mixerName_.c_str() << std::endl;
		}
	}*/
}

SettingsFrame::type_void_signal SettingsFrame::signal_ok_pressed()
{
	return m_signal_ok_pressed;
}

void SettingsFrame::onCellToggled(const Glib::ustring& path)
{
	std::cout << path << std::endl;
	Gtk::TreeModel::iterator it = switches_->get_iter(path);
	Gtk::TreeModel::Row row = *it;
	if (bool(row.get_value(m_TColumns.m_col_toggle))) {
		row[m_TColumns.m_col_toggle] = false;
	}
	else {
		row[m_TColumns.m_col_toggle] = true;
	}
	std::cout << bool(row.get_value(m_TColumns.m_col_toggle)) << std::endl;
}
