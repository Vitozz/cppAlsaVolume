This is a simple application to control one of the Alsa's mixers volume in tray.

Application uses GTK+ interface via gtkmm library

Build Deps:

	- gtkmm-3.0 (libgtkmm-3.0-dev package in Ubuntu)
	- glib (libglibmm-2.4-dev package in Ubuntu)
	- alsa/asoundlib (libasound2-dev package in Ubuntu)
	- libarchive (libarchive12, libarchive-dev packages in Ubuntu)

Howto build application:

if You have Qt installed:

>qmake

>make

else:

>g++ -W -o alsavolume *.cpp \`pkg-config --cflags gtkmm-3.0 alsa libarchive\` \`pkg-config --libs gtkmm-3.0 alsa libarchive\`

What features available:

	- sound card selection
	- sound card mixer selection
	- volume control by "scroll" on tray icon
	- volume control using popup window with slider
	- mute sound on middleclick and by checkbox selection
	- enable or disable sound card switches in settings dialog
	- iconpacks support

TODO LIST:

	- external mixer launch

HOWTO USE ICONPACKS:

	Iconpack - the usual *tar.gz archive with *.png icons (filenames are the same as the original icons).
	Just copy *tar.gz archive into ~/.local/share/alsavolume/iconpacks folder and choose iconpack in settings dialog
