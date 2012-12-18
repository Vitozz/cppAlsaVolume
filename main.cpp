#include "sliderwindow.h"
#include "trayicon.h"
#include "filework.h"
#include "gtkmm/application.h"
#include "gtkmm/builder.h"
#include "glibmm.h"

int main (int argc, char *argv[])
{
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "org.gtkmm.alsavolume");

	Glib::ustring ui_ = FileWork::getResPath("gladefiles/SliderFrame.glade");
	if (ui_.empty()) {
		std::cerr << "No SliderFrame.glade file found" << std::endl;
		return 1;
	}
	Glib::RefPtr<Gtk::Builder> refBuilder = Gtk::Builder::create();
	try {
		//refBuilder->create_from_file(ui_);
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
