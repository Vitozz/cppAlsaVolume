/*
 * main.cpp
 * Copyright (C) 2012-2025 Vitaly Tonkacheyev
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

#include "gui/sliderwindow.h"
#include "gui/trayicon.h"
#include "tools/core.h"
#include <glibmm/fileutils.h>
#include <glibmm/markup.h>
#include <gtkmm/application.h>
#include <gtkmm/builder.h>
#include <iostream>
#include <libintl.h>
#define _(String) gettext(String)
#define N_(String) gettext_noop(String)
#define PACKAGE "alsavolume"
#define CODEC "UTF-8"

int main(int argc, char *argv[])
{
    bindtextdomain(PACKAGE, Tools::getDirPath("locale").c_str());
    bind_textdomain_codeset(PACKAGE, CODEC);
    textdomain(PACKAGE);
    Glib::RefPtr<Gtk::Application> app          = Gtk::Application::create(argc, argv, "org.gtkmm.alsavolume");
    Glib::ustring                  slider_ui_   = "/org/vitozz/cppalsavolume/gladefiles/SliderFrame.glade";
    Glib::ustring                  settings_ui_ = "/org/vitozz/cppalsavolume/gladefiles/SettingsFrame.glade";
    Glib::RefPtr<Gtk::Builder>     refBuilder   = Gtk::Builder::create();
    try {
        refBuilder->add_from_resource(slider_ui_);
        refBuilder->add_from_resource(settings_ui_);
    } catch (const Gtk::BuilderError &ex) {
        std::cerr << "BuilderError" << "::" << __FILE_NAME__ << "::" << __LINE__ << " " << ex.what() << std::endl;
        return 1;
    } catch (const Glib::MarkupError &ex) {
        std::cerr << "MarkupError" << "::" << __FILE_NAME__ << "::" << __LINE__ << " " << ex.what() << std::endl;
        return 1;
    } catch (const Glib::FileError &ex) {
        std::cerr << "FileError" << "::" << __FILE_NAME__ << "::" << __LINE__ << " " << ex.what() << std::endl;
        return 1;
    } catch (const Gio::ResourceError &e) {
        std::cerr << "Resource error" << "::" << __FILE_NAME__ << "::" << __LINE__ << " " << e.what() << std::endl;
    } catch (const Glib::Error &e) {
        std::cerr << "GTK/GLib error" << "::" << __FILE_NAME__ << "::" << __LINE__ << " " << e.what() << std::endl;
    }
    Core::Ptr core(new Core(refBuilder));
    app->hold();
    SliderWindow *sliderWindow = nullptr;
    refBuilder->get_widget_derived("volumeFrame", sliderWindow);
    if (!sliderWindow) {
        std::cerr << "Failed to create SettingsFrame\n";
        return 1;
    }
    TrayIcon::Ptr trayIcon(
        new TrayIcon(core->getVolumeValue(), core->getSoundCardName(), core->getActiveMixer(), core->getMuted()));
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
        return app->run();
    }
    delete sliderWindow;
    return 0;
}
