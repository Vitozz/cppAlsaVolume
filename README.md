This is a simple application to control one of the Alsa's mixers volume in tray.

Application uses GTK+ interface via gtkmm library

Build Deps:

	- gtkmm-3.0 (libgtkmm-3.0-dev package on Ubuntu)
	- glib (libglibmm-2.4-dev package on Ubuntu)
	- alsa/asoundlib (libasound2-dev package on Ubuntu)

Howto build application:

if You have Qt installed:

>qmake

>make

else:

>g++ -W -o alsavolume *.cpp \`pkg-config --cflags gtkmm-3.0 alsa\` \`pkg-config --libs gtkmm-3.0 alsa\`

What features available:

	- sound card selection
	- sound card mixer selection
	- volume control by "scroll" on tray icon
	- volume control using popup window with slider

TODO LIST:

	- mute sound on middleclick and by checkbox selection
	- fix app crash on card selection when card has no playback mixers
	- add switches to switchlist with full functionality
	- app capture mixers to control volume
	- add support of joined mixers
