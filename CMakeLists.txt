set( PROJECT alsavolume )
cmake_minimum_required( VERSION 2.8.0 )

if( NOT UNIX )
	message( FATAL_ERROR "Not Unix!" )
endif( NOT UNIX )

set( USE_PULSE OFF CACHE BOOL "Set pulseaudio usage" )
set( USE_GTK3 ON CACHE BOOL "Build with gtkmm-3.0 support" )
set( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" )
option( USE_APPINDICATOR "Use appindicator support" OFF )
option( USE_KDE "Enable KStatusNotifierItem support" OFF )

find_package( PkgConfig REQUIRED )
find_package( Intl REQUIRED )

if( ${USE_GTK3} )
	pkg_check_modules( GTKMM REQUIRED gtkmm-3.0 )
	if( ${USE_APPINDICATOR} )
		pkg_check_modules( APPINDICATOR REQUIRED appindicator3-0.1 )
		add_definitions( -DUSE_APPINDICATOR )
		link_directories( ${APPINDICATOR_LIBRARY_DIRS} )
		include_directories( ${APPINDICATOR_INCLUDE_DIRS} )
	endif( ${USE_APPINDICATOR} )
else( ${USE_GTK3} )
	pkg_check_modules( GTKMM REQUIRED gtkmm-2.4 )
	add_definitions( -DIS_GTK_2 )
	if( ${USE_APPINDICATOR} )
		pkg_check_modules( APPINDICATOR REQUIRED appindicator-0.1 )
		add_definitions( -DUSE_APPINDICATOR )
		link_directories( ${APPINDICATOR_LIBRARY_DIRS} )
		include_directories( ${APPINDICATOR_INCLUDE_DIRS} )
	endif( ${USE_APPINDICATOR} )
endif( ${USE_GTK3} )
if( ${USE_KDE} )
	set( USE_APPINDICATOR OFF )
endif( ${USE_KDE} )
find_package( ALSA REQUIRED )
set( CUSTOM_LDFLAGS "-Wl,-as-needed" )
set( CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")

if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
	add_definitions( -DIS_DEBUG )
endif( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )

add_definitions(
	${GTKMM_CFLAGS_OTHER}
)
link_directories(
	${GTKMM_LIBRARY_DIRS}
)

include_directories(
	${GTKMM_INCLUDE_DIRS}
	${ALSA_INCLUDE_DIRS}
)
set( DEPLIBS
	${GTKMM_LIBRARIES}
	${ALSA_LIBRARIES}
)

if( ${USE_APPINDICATOR} )
	set( DEPLIBS
		${DEPLIBS}
		${APPINDICATOR_LIBRARIES}
	)
endif( ${USE_APPINDICATOR} )

set( alsavolume_HDRS
	alsawork/alsawork.h
	alsawork/alsadevice.h
	alsawork/mixerswitches.h
	gui/trayicon.h
	gui/sliderwindow.h
	gui/settingsframe.h
	tools/settings.h
	tools/tools.h
	tools/settingsstr.h
	tools/core.h
)

set( alsavolume_SRCS
	main.cpp
	gui/trayicon.cpp
	gui/sliderwindow.cpp
	gui/settingsframe.cpp
	alsawork/alsawork.cpp
	alsawork/alsadevice.cpp
	alsawork/mixerswitches.cpp
	tools/settings.cpp
	tools/tools.cpp
	tools/settingsstr.cpp
	tools/core.cpp
)

set( statusnotifier_SRCS
	third-party/statusnotifier/closures.c
	third-party/statusnotifier/enums.c
	third-party/statusnotifier/statusnotifier.c
)

set( statusnotifier_HDRS
	third-party/statusnotifier/interfaces.h
	third-party/statusnotifier/statusnotifier.h
	third-party/statusnotifier/closures.h
	third-party/statusnotifier/enums.h
)

if( USE_PULSE )
	find_package( PulseAudio REQUIRED )
	include_directories( ${PULSE_INCLUDE_DIR} )
	set( alsavolume_HDRS
		${alsavolume_HDRS}
		pulsework/pulsecore.h
		pulsework/pulsedevice.h
	)
	set( alsavolume_SRCS
		${alsavolume_SRCS}
		pulsework/pulsecore.cpp
		pulsework/pulsedevice.cpp
	)
	add_definitions(
		-DHAVE_PULSE
	)
	set( DEPLIBS
		${DEPLIBS}
		${PULSEAUDIO_LIBRARY}
	)
endif( USE_PULSE )

if( ${USE_KDE} )
	set( alsavolume_SRCS
		${alsavolume_SRCS}
		${statusnotifier_SRCS}
	)
	add_definitions( -DUSE_KDE )
endif( ${USE_KDE} )

set( DESKTOP_FILES ${PROJECT}.desktop )
set( ICON_FILES
	icons/tb_icon0.png
	icons/tb_icon20.png
	icons/tb_icon40.png
	icons/tb_icon60.png
	icons/tb_icon80.png
	icons/tb_icon100.png
	icons/volume.png
	icons/volume_ico.png
)
if( ${USE_GTK3} )
	set(GLADE_FILES
		gladefiles/SliderFrame.glade
		gladefiles/SettingsFrame.glade
	)
else( ${USE_GTK3} )
	set(GLADE_FILES
		gladefiles/SliderFrame_2.glade
		gladefiles/SettingsFrame_2.glade
	)
endif( ${USE_GTK3} )
	
add_executable( ${PROJECT} ${alsavolume_SRCS} )
target_link_libraries( ${PROJECT} ${CUSTOM_LDFLAGS} ${DEPLIBS} )

set( DATADIR_PATH ${CMAKE_INSTALL_PREFIX}/share )

INSTALL( TARGETS ${PROJECT} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
INSTALL( FILES ${DESKTOP_FILES} DESTINATION ${DATADIR_PATH}/applications )
INSTALL( FILES ${ICON_FILES} DESTINATION ${DATADIR_PATH}/${PROJECT}/icons )
INSTALL( FILES ${GLADE_FILES} DESTINATION ${DATADIR_PATH}/${PROJECT}/gladefiles )

#TOOLS FOR BUILD LOCALES
find_program( AV_XGETTEXT xgettext )
mark_as_advanced( AV_XGETTEXT )
find_program( AV_INTLTOOL intltool-update )
mark_as_advanced( AV_INTLTOOL )
find_program( AV_MSGMERGE msgmerge )
mark_as_advanced( AV_MSGMERGE )
find_program( AV_MSGFMT msgfmt )
mark_as_advanced( AV_MSGFMT )

#MAKE A WORK COPY OF PO DIRECTORY TO BUILD LOCALES
file( COPY ${PROJECT_SOURCE_DIR}/po DESTINATION ${PROJECT_BINARY_DIR} )

set( alsavolume_LOCALES
	ru
	uk
)
set( PO_FULL_DIR
	${PROJECT_BINARY_DIR}/po
)
set( LOCALES_WORKDIR
	${PROJECT_BINARY_DIR}/locale
)
set( LOCALES_INSTALLPATH
	${DATADIR_PATH}/${PROJECT}/locale
)
#ADD SOURCES PREFIX
file( STRINGS ${PO_FULL_DIR}/POTFILES.in POTFILES_LINES )
set( POTFILES_CONTENTS )
foreach( line ${POTFILES_LINES} )
	set( POTFILES_CONTENTS
		"${POTFILES_CONTENTS}
${PROJECT_SOURCE_DIR}/${line}"
	)
endforeach( line ${POTFILES_LINES} )
file( WRITE ${PO_FULL_DIR}/POTFILES.in "${POTFILES_CONTENTS}" )

add_custom_target(alsavolume-update-sources
	AV_INTLTOOL -m
	WORKING_DIRECTORY ${PO_FULL_DIR}
)
add_custom_command(OUTPUT ${PO_FULL_DIR}/${PROJECT}.pot
	COMMAND ${AV_XGETTEXT} --files-from ${PO_FULL_DIR}/POTFILES.in --keyword=_ -o ${PO_FULL_DIR}/${PROJECT}.pot --no-wrap --no-location
	DEPENDS ${PO_FULL_DIR}/POTFILES.in
	WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
)
set( AV_CATALOG_TARGETS )
foreach( locale ${alsavolume_LOCALES} )
		add_custom_target( alsavolume-update-${locale}.po
			${AV_MSGMERGE} ${locale} ${PO_FULL_DIR}/${PROJECT}.pot -o ${locale}.po --no-wrap
			DEPENDS ${locale}.po
			WORKING_DIRECTORY ${PO_FULL_DIR}
		)
		file( MAKE_DIRECTORY
			"${LOCALES_WORKDIR}/${locale}/LC_MESSAGES"
		)
		add_custom_command(
			OUTPUT
			${LOCALES_WORKDIR}/${locale}/LC_MESSAGES/${PROJECT}.mo
			COMMAND
			${AV_MSGFMT} -c -o ${LOCALES_WORKDIR}/${locale}/LC_MESSAGES/${PROJECT}.mo ${PO_FULL_DIR}/${locale}.po
			DEPENDS
			${PO_FULL_DIR}/${locale}.po
		)

		install(
			FILES
			${LOCALES_WORKDIR}/${locale}/LC_MESSAGES/${PROJECT}.mo
			DESTINATION
			${LOCALES_INSTALLPATH}/${locale}/LC_MESSAGES
		)

		set(AV_CATALOG_TARGETS
			${AV_CATALOG_TARGETS}
			${LOCALES_WORKDIR}/${locale}/LC_MESSAGES/${PROJECT}.mo
		)
endforeach( locale )
add_custom_target(alsavolume-transtalions
	ALL
	DEPENDS
	${PO_FULL_DIR}/${PROJECT}.pot
	${AV_CATALOG_TARGETS}
)
