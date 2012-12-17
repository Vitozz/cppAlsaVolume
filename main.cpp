#include "sliderwindow.h"
#include "trayicon.h"
#include "filework.h"
#include "gtkmm/application.h"
#include "gtkmm/builder.h"
#include "glibmm.h"

int main (int argc, char *argv[])
{
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.gtkmm.alsavolume");

	Glib::RefPtr<Gtk::Builder> refBuilder = Gtk::Builder::create();
	try {
		Glib::ustring ui_ = getResPath("gladefiles/SliderFrame.glade");
		if (ui_.empty()) {
			std::cerr << "No SliderFrame.glade file found" << std::endl;
			return 1;
		}
		refBuilder->add_from_file(ui_);
	}
	catch(const Glib::FileError& ex) {
		std::cerr << "FileError: " << ex.what() << std::endl;
		return 1;
	}
	catch(const Glib::MarkupError& ex) {
		std::cerr << "MarkupError: " << ex.what() << std::endl;
		return 1;
	}
	catch(const Gtk::BuilderError& ex) {
		std::cerr << "BuilderError: " << ex.what() << std::endl;
		return 1;
	}
	SliderWindow *volumechanger = 0;
	refBuilder->get_widget_derived("volumeFrame", volumechanger);
	Glib::RefPtr<TrayIcon> trayicon = Glib::RefPtr<TrayIcon>(new TrayIcon(volumechanger));
	app->hold();
	volumechanger->set_visible(false);
	app->add_window(*volumechanger);
	return app->run();
	delete volumechanger;
	return 0;
}
