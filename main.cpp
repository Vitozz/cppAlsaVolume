/*
 * main.cpp
 * Copyright (C) 2012-2015 Vitaly Tonkacheyev
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

#include "tools/core.h"
#include "gui/sliderwindow.h"
#include "gui/trayicon.h"
#include "tools/tools.h"
#ifndef IS_GTK_2
#include "gtkmm/application.h"
#else
#include "gtkmm/main.h"
#endif
#include "gtkmm/builder.h"
#include "glibmm.h"
#include "libintl.h"
#define _(String) gettext(String)
#define N_(String) gettext_noop (String)
#define PACKAGE "alsavolume"
#define CODEC "UTF-8"

int main (int argc, char *argv[])
{
	bindtextdomain(PACKAGE, Tools::getDirPath("locale").c_str());
	bind_textdomain_codeset(PACKAGE, CODEC);
	textdomain(PACKAGE);
#ifndef IS_GTK_2
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.gtkmm.alsavolume");
	Glib::ustring slider_ui_ = Tools::getResPath("gladefiles/SliderFrame.glade");
	Glib::ustring settings_ui_ = Tools::getResPath("gladefiles/SettingsFrame.glade");
#else
	Gtk::Main app(argc, argv);
	Glib::ustring slider_ui_ = Tools::getResPath("gladefiles/SliderFrame_2.glade");
	Glib::ustring settings_ui_ = Tools::getResPath("gladefiles/SettingsFrame_2.glade");
#endif
	if (slider_ui_.empty()) {
		std::cerr << "No SliderFrame.glade file found" << std::endl;
		return 1;
	}
	if (settings_ui_.empty()) {
		std::cerr << "No SettingsFrame.glade file found" << std::endl;
		return 1;
	}
	Glib::RefPtr<Gtk::Builder> refBuilder = Gtk::Builder::create();
	try {
		refBuilder->add_from_file(slider_ui_);
		refBuilder->add_from_file(settings_ui_);
	}
	catch(const Gtk::BuilderError& ex) {
		std::cerr << "BuilderError::main.cpp::62 " << ex.what() << std::endl;
		return 1;
	}
	catch(const Glib::MarkupError& ex) {
		std::cerr << "MarkupError::main.cpp::62 " << ex.what() << std::endl;
		return 1;
	}
	catch(const Glib::FileError& ex) {
		std::cerr << "FileError::main.cpp::62 " << ex.what() << std::endl;
		return 1;
	}
	Core *core = new Core(refBuilder);
#ifndef IS_GTK_2
	app->hold();
#endif
	SliderWindow *sliderWindow = 0;
	refBuilder->get_widget_derived("volumeFrame", sliderWindow);
	TrayIcon *trayIcon = new TrayIcon(core->getVolumeValue(),
					  core->getSoundCardName(),
					  core->getActiveMixer(),
					  core->getMuted());
	if (trayIcon && sliderWindow) {
		sliderWindow->setVolumeValue(core->getVolumeValue());
		core->signal_value_changed().connect(sigc::mem_fun(*trayIcon, &TrayIcon::on_signal_volume_changed));
		core->signal_mixer_muted().connect(sigc::mem_fun(*trayIcon, &TrayIcon::setMuted));
		core->signal_volume_changed().connect(sigc::mem_fun(*sliderWindow, &SliderWindow::setVolumeValue));
		sliderWindow->signal_volume_changed().connect(sigc::mem_fun(*core, &Core::onVolumeSlider));
		trayIcon->signal_ask_dialog().connect(sigc::mem_fun(*core, &Core::runAboutDialog));
		trayIcon->signal_ask_settings().connect(sigc::mem_fun(*core, &Core::runSettings));
		trayIcon->signal_on_restore().connect(sigc::mem_fun(*sliderWindow, &SliderWindow::setWindowPosition));
		trayIcon->signal_save_settings().connect(sigc::mem_fun(*core, &Core::saveSettings));
		trayIcon->signal_on_mute().connect(sigc::mem_fun(*core, &Core::soundMuted));
		trayIcon->signal_value_changed().connect(sigc::mem_fun(*core, &Core::onTrayIconScroll));
		sliderWindow->set_visible(false);
#ifndef IS_GTK_2
		return app->run();
#else
		Gtk::Main::run();
#endif
	}
	delete sliderWindow;
	delete trayIcon;
	delete core;
	return 0;
}
