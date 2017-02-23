project(alsavolume)
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

if( ${USE_KDE} AND ${USE_APPINDICATOR} )
	message(FATAL_ERROR "
Both Flags USE_KDE and USE_APPINDICATOR are enabled!
Please disable at least one of them.")
endif( ${USE_KDE} AND ${USE_APPINDICATOR} )

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

if( ${USE_KDE} )
	set( statusnotifier_SRCS
		third-party/statusnotifier/src/closures.c
		third-party/statusnotifier/src/statusnotifier.c
	)

	set( statusnotifier_HDRS
		third-party/statusnotifier/src/interfaces.h
		third-party/statusnotifier/src/statusnotifier.h
		third-party/statusnotifier/src/closures.h
	)
	file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/enums")
	file(COPY ${PROJECT_SOURCE_DIR}/third-party/statusnotifier/src/statusnotifier.h DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/enums/)
	find_program( GLIB_MKENUMS glib-mkenums )
	mark_as_advanced( GLIB_MKENUMS )
	set(enums_h "${CMAKE_CURRENT_BINARY_DIR}/enums/enums.h")
	set(enums_c "${CMAKE_CURRENT_BINARY_DIR}/enums/enums.c")
	add_custom_command(OUTPUT ${enums_h}
		COMMAND ${GLIB_MKENUMS} ARGS
		--fhead \"\#ifndef __STATUS_NOTIFIER_ENUMS_H__\\n\#define __STATUS_NOTIFIER_ENUMS_H__\\n\#include \\\"statusnotifier.h\\\"\"
		--vhead \"GType \@enum_name\@_get_type \(void\)\;\\n\#define TYPE_\@ENUMNAME\@ \(\@enum_name\@_get_type\(\)\)\"
		--ftail \"G_END_DECLS\\n\\n\#endif /* __STATUS_NOTIFIER_ENUMS_H__ */\"
		statusnotifier.h > enums.h
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/enums
		)
	message("${enums.h}")
	add_custom_command(OUTPUT ${enums_c}
		COMMAND ${GLIB_MKENUMS} ARGS
		--fhead \"\#include \\\"enums.h\\\"\\n\"
		--vhead \"GType\\n\@enum_name\@_get_type \(void\)\\n{\\n  static GType etype = 0\;\\n  if \(etype == 0\) {\\n    static const G\@Type\@Value values[] = {\"
		--vprod \" { \@VALUENAME\@, \\\"\@VALUENAME\@\\\", \\\"\@valuenick\@\\\" },\"
		--vtail \"      { 0, NULL, NULL }\\n    }\;\\n    etype = g_\@type\@_register_static \(\\\"\@EnumName\@\\\", values\)\;\\n  }\\n  return etype\;\\n}\\n\"
		statusnotifier.h > enums.c
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/enums
		)
	add_custom_target(build_enums_h DEPENDS ${enums_h} SOURCES "${statusnotifier_SRCS}")
	add_custom_target(build_enums_c DEPENDS ${enums_c} SOURCES "${statusnotifier_SRCS}")
	set( statusnotifier_SRCS
		${statusnotifier_SRCS}
		${CMAKE_CURRENT_BINARY_DIR}/enums/enums.c
		)
	set( statusnotifier_HDRS
		${statusnotifier_HDRS}
		${CMAKE_CURRENT_BINARY_DIR}/enums/enums.h
		)
	include_directories(
		${CMAKE_CURRENT_BINARY_DIR}/enums
		)
endif()

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

set( DESKTOP_FILES ${PROJECT_NAME}.desktop )
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

add_executable( ${PROJECT_NAME} ${alsavolume_SRCS} )
if( ${USE_KDE} )
	add_dependencies(${PROJECT_NAME} build_enums_h build_enums_c)
endif()
target_link_libraries( ${PROJECT_NAME} ${CUSTOM_LDFLAGS} ${DEPLIBS} )

set( DATADIR_PATH ${CMAKE_INSTALL_PREFIX}/share )

INSTALL( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
INSTALL( FILES ${DESKTOP_FILES} DESTINATION ${DATADIR_PATH}/applications )
INSTALL( FILES ${ICON_FILES} DESTINATION ${DATADIR_PATH}/${PROJECT_NAME}/icons )
INSTALL( FILES ${GLADE_FILES} DESTINATION ${DATADIR_PATH}/${PROJECT_NAME}/gladefiles )

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
	${DATADIR_PATH}/${PROJECT_NAME}/locale
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
add_custom_command(OUTPUT ${PO_FULL_DIR}/${PROJECT_NAME}.pot
	COMMAND ${AV_XGETTEXT} --files-from ${PO_FULL_DIR}/POTFILES.in --keyword=_ -o ${PO_FULL_DIR}/${PROJECT_NAME}.pot --no-wrap --no-location
	DEPENDS ${PO_FULL_DIR}/POTFILES.in
	WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
)
set( AV_CATALOG_TARGETS )
foreach( locale ${alsavolume_LOCALES} )
		add_custom_target( alsavolume-update-${locale}.po
			${AV_MSGMERGE} ${locale} ${PO_FULL_DIR}/${PROJECT_NAME}.pot -o ${locale}.po --no-wrap
			DEPENDS ${locale}.po
			WORKING_DIRECTORY ${PO_FULL_DIR}
		)
		file( MAKE_DIRECTORY
			"${LOCALES_WORKDIR}/${locale}/LC_MESSAGES"
		)
		add_custom_command(
			OUTPUT
			${LOCALES_WORKDIR}/${locale}/LC_MESSAGES/${PROJECT_NAME}.mo
			COMMAND
			${AV_MSGFMT} -c -o ${LOCALES_WORKDIR}/${locale}/LC_MESSAGES/${PROJECT_NAME}.mo ${PO_FULL_DIR}/${locale}.po
			DEPENDS
			${PO_FULL_DIR}/${locale}.po
		)

		install(
			FILES
			${LOCALES_WORKDIR}/${locale}/LC_MESSAGES/${PROJECT_NAME}.mo
			DESTINATION
			${LOCALES_INSTALLPATH}/${locale}/LC_MESSAGES
		)

		set(AV_CATALOG_TARGETS
			${AV_CATALOG_TARGETS}
			${LOCALES_WORKDIR}/${locale}/LC_MESSAGES/${PROJECT_NAME}.mo
		)
endforeach( locale )
add_custom_target(alsavolume-transtalions
	ALL
	DEPENDS
	${PO_FULL_DIR}/${PROJECT_NAME}.pot
	${AV_CATALOG_TARGETS}
)
