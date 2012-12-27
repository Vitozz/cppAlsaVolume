#include "sliderwindow.h"
#include "trayicon.h"
#include "tools.h"
#include "gtkmm/application.h"
#include "gtkmm/builder.h"
#include "glibmm.h"

int main (int argc, char *argv[])
{
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.gtkmm.alsavolume");

	Glib::ustring ui_ = Tools::getResPath("gladefiles/SliderFrame.glade");
	if (ui_.empty()) {
		std::cerr << "No SliderFrame.glade file found" << std::endl;
		return 1;
	}
	Glib::RefPtr<Gtk::Builder> refBuilder = Gtk::Builder::create();
	try {
		refBuilder->add_from_file(ui_);
	}
	catch(const Gtk::BuilderError& ex) {
		std::cerr << "BuilderError::main.cpp::19 " << ex.what() << std::endl;
		return 1;
	}
	catch(const Glib::MarkupError& ex) {
		std::cerr << "MarkupError::main.cpp::19 " << ex.what() << std::endl;
		return 1;
	}
	catch(const Glib::FileError& ex) {
		std::cerr << "FileError::main.cpp::19 " << ex.what() << std::endl;
		return 1;
	}
	app->hold();
	SliderWindow *sliderWindow = 0;
	refBuilder->get_widget_derived("volumeFrame", sliderWindow);
	TrayIcon *trayIcon = new TrayIcon(sliderWindow->getVolumeValue(),
					  sliderWindow->getSoundCardName(),
					  sliderWindow->getActiveMixer(),
					  sliderWindow->getMuted());
	if (sliderWindow && trayIcon) {
		sliderWindow->signal_volume_changed().connect(sigc::mem_fun(*trayIcon, &TrayIcon::on_signal_volume_changed));
		trayIcon->signal_ask_dialog().connect(sigc::mem_fun(*sliderWindow, &SliderWindow::runAboutDialog));
		trayIcon->signal_ask_settings().connect(sigc::mem_fun(*sliderWindow, &SliderWindow::runSettings));
		trayIcon->signal_on_restore().connect(sigc::mem_fun(*sliderWindow, &SliderWindow::setWindowPosition));
		trayIcon->signal_save_settings().connect(sigc::mem_fun(*sliderWindow, &SliderWindow::saveSettings));
		trayIcon->signal_value_changed().connect(sigc::mem_fun(*sliderWindow, &SliderWindow::setVolumeValue));
		trayIcon->signal_on_mute().connect(sigc::mem_fun(*sliderWindow, &SliderWindow::soundMuted));
		sliderWindow->set_visible(false);
		return app->run();
	}
	delete sliderWindow;
	delete trayIcon;
	return 0;
}
