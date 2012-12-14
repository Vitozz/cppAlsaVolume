### eqmake4 was here ###
CONFIG -= debug_and_release debug
CONFIG += release

TEMPLATE = app
CONFIG += console
CONFIG -= qt

CONFIG += link_pkgconfig
PKGCONFIG += gtkmm-3.0 \
	     alsa

SOURCES += main.cpp \
	alsawork.cc \
	trayicon.cpp \
	filework.cpp \
    sliderwindow.cpp \
    settings.cpp

HEADERS += \
	alsawork.h \
	trayicon.h \
	filework.h \
    sliderwindow.h \
    settings.h

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
