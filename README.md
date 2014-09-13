This is a simple application to control one of the Alsa's mixers volume in tray.

Application uses GTK+ interface via gtkmm library

Build Deps:

	- gtkmm-3.0 (libgtkmm-3.0-dev package in Ubuntu)
	- glib (libglibmm-2.4-dev package in Ubuntu)
	- alsa/asoundlib (libasound2-dev package in Ubuntu)
	- libpulse (only for pulseaudio support)

Howto build application:

if You have Qt installed:

>qmake

>make

to build wiht pulseaudio support:

>qmake CONFIG+=pulseaudio

>make

if You have cmake installed

>mkdir build && cd build

>cmake ..

>make

to build wiht pulseaudio support:

>cmake -DUSE_PULSE=ON ..

>make

else:

>g++ -W -o alsavolume main.cpp  gui/*.cpp alsawork/*.cpp tools/*.cpp \`pkg-config --cflags gtkmm-3.0 alsa \` \`pkg-config --libs gtkmm-3.0 alsa \`

to build wiht pulseaudio support:

>g++ -W -o alsavolume main.cpp  gui/*.cpp alsawork/*.cpp tools/*.cpp pulsework/*.cpp \`pkg-config --cflags gtkmm-3.0 alsa libpulse \` \`pkg-config --libs gtkmm-3.0 alsa libpulse \` 

What features available:

	- sound card selection
	- sound card mixer selection
	- volume control by "scroll" on tray icon
	- volume control using popup window with slider
	- mute sound on middleclick and by checkbox selection
	- enable or disable sound card switches in settings dialog
	- use PulseAaudio volume controll (optionally)
