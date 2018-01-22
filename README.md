This is a simple application to control one of the Alsa's mixer volume in tray.

Application uses GTK+ interface via gtkmm library

Build Deps:

	- gtkmm-3.0 (libgtkmm-3.0-dev package in Ubuntu)
	- gtkmm-2.4 (only for gtkmm-2.4 support) 
	- glib (libglibmm-2.4-dev package in Ubuntu)
	- alsa/asoundlib (libasound2-dev package in Ubuntu)
	- libpulse (only for pulseaudio support / optional)
	- cmake >= 2.8.0
	- libappindicator (libappindicator3 for GTK3 / optional)

Howto build application:

if You have cmake installed

>mkdir build && cd build

>cmake ..

>make

to build with pulseaudio support:

>cmake -DUSE_PULSE=ON ..

>make

to build with gtkmm-2.4 support:

>cmake -DUSE_GTK3=OFF ..

>make

to build with libappindicator support:

>cmake -DUSE_APPINDICATOR=ON ..

>make

to build with StatusNotifier support:

>cmake -DUSE_SNI=ON ..

>make


What features available:

	- sound card selection
	- sound card mixer selection
	- volume control by "scroll" on tray icon
	- volume control using popup window with slider
	- mute sound on middleclick and by checkbox selection
	- enable or disable sound card switches in settings dialog
	- use PulseAaudio volume controll (optionally)

Warning!!!
With AppIndicator in KDE5 the scroll-event is not working. Use Restore menu item to
control volume with slider.
