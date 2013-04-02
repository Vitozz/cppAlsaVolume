### eqmake4 was here ###
include(conf.pri)

CONFIG -= debug_and_release debug
CONFIG += release

TARGET = alsavolume
TEMPLATE = app
CONFIG += console
CONFIG -= qt

CONFIG += link_pkgconfig
PKGCONFIG += gtkmm-3.0 \
	     alsa \
	     libarchive

SOURCES += main.cpp \
	gui/trayicon.cpp \
	gui/sliderwindow.cpp \
	gui/settingsframe.cpp \
	alsawork/alsawork.cpp \
	alsawork/volumemixers.cpp \
	alsawork/mixerswitches.cpp \
	tools/settings.cpp \
	tools/tools.cpp \
	tools/settingsstr.cpp

HEADERS += \
	alsawork/alsawork.h \
	alsawork/volumemixers.h \
	alsawork/mixerswitches.h \
	gui/trayicon.h \
	gui/sliderwindow.h \
	gui/settingsframe.h \
	tools/settings.h \
	tools/tools.h \
	tools/settingsstr.h

INCLUDEPATH += \
	/usr/include/gtkmm-3.0 \
	/usr/lib64/gtkmm-3.0/include \
	/usr/include/atkmm-1.6 \
	/usr/include/giomm-2.4 \
	/usr/lib64/giomm-2.4/include \
	/usr/include/pangomm-1.4 \
	/usr/lib64/pangomm-1.4/include \
	/usr/include/gtk-3.0 \
	/usr/include/cairomm-1.0 \
	/usr/lib64/cairomm-1.0/include \
	/usr/include/gdk-pixbuf-2.0 \
	/usr/include/gtk-3.0/unix-print \
	/usr/include/gdkmm-3.0 \
	/usr/lib64/gdkmm-3.0/include \
	/usr/include/atk-1.0 \
	/usr/include/glibmm-2.4 \
	/usr/lib64/glibmm-2.4/include \
	/usr/include/glib-2.0 \
	/usr/lib64/glib-2.0/include \
	/usr/include/sigc++-2.0 \
	/usr/lib64/sigc++-2.0/include \
	/usr/include/pango-1.0 \
	/usr/include/cairo \
	/usr/include/pixman-1 \
	/usr/include/freetype2 \
	/usr/include/libpng15 \
	/usr/include/qt4 \
	/usr/include/qt4/QtGui \
	/usr/include/libdrm \
	/usr/include/qt4/QtCore \
	/usr/include/gio-unix-2.0

unix{
	target.path = $$BINDIR
	INSTALLS += target
	dt.path = $$PREFIX/share/applications/
	dt.files = AlsaVolume.desktop
	gf1.path = $$PREFIX/share/alsavolume/gladefiles/
	gf1.files = gladefiles/SliderFrame.glade SettingsFrame.glade
	gf2.path = $$PREFIX/share/alsavolume/gladefiles/
	gf2.files = gladefiles/SettingsFrame.glade
	icon.path = $$PREFIX/share/alsavolume/icons/
	icon.files = icons/tb_icon0.png \
		      icons/tb_icon20.png \
		      icons/tb_icon40.png \
		      icons/tb_icon60.png \
		      icons/tb_icon80.png \
		      icons/tb_icon100.png \
		      icons/volume.png \
		      icons/volume_ico.png
	INSTALLS += dt \
		    gf1 \
		    gf2 \
		    icon
}
