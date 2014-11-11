set( PROJECT alsavolume )
cmake_minimum_required( VERSION 2.8.0 )

if( NOT UNIX )
	message( FATAL_ERROR "Not Unix!" )
endif( NOT UNIX )

set( USE_PULSE OFF CACHE BOOL "Set pulseaudio usage" )
set( USE_GTK3 ON CACHE BOOL "Build with gtkmm-3.0 support" )

find_package( PkgConfig REQUIRED )

if( ${USE_GTK3} )
	pkg_check_modules( GTKMM REQUIRED gtkmm-3.0 )
else( ${USE_GTK3} )
	pkg_check_modules( GTKMM REQUIRED gtkmm-2.4 )
	add_definitions( -DIS_GTK_2 )
endif( ${USE_GTK3} )
pkg_check_modules( ALSA REQUIRED alsa )

set( CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")

if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
	add_definitions(-DIS_DEBUG)
endif( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )

add_definitions(
	${GTKMM_CFLAGS_OTHER}
	${ALSA_CFLAGS_OTHER}

)
link_directories(
	${GTKMM_LIBRARY_DIRS}
	${ALSA_LIBRARY_DIRS}
)

include_directories(
	${GTKMM_INCLUDE_DIRS}
	${ALSA_INCLUDE_DIRS}
)
set( DEPLIBS
	${GTKMM_LIBRARIES}
	${ALSA_LIBRARIES}
)

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

if( USE_PULSE )
	pkg_check_modules( PULSE REQUIRED libpulse )
	link_directories( ${PULSE_LIBRARY_DIRS} )
	include_directories( ${PULSE_INCLUDE_DIRS} )
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
		${PULSE_CFLAGS_OTHER}
		-DHAVE_PULSE
	)
	set( DEPLIBS
		${DEPLIBS}
		${PULSE_LIBRARIES}
	)
endif( USE_PULSE )

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
target_link_libraries( ${PROJECT} ${DEPLIBS} )

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
add_custom_target(alsavolume-update-sources
	AV_INTLTOOL -m
	WORKING_DIRECTORY ${PO_FULL_DIR}
)
set( POTFILES_CONTENTS
	"${PROJECT_SOURCE_DIR}/gladefiles/SettingsFrame.glade
${PROJECT_SOURCE_DIR}/gladefiles/SettingsFrame_2.glade
${PROJECT_SOURCE_DIR}/gladefiles/SliderFrame.glade
${PROJECT_SOURCE_DIR}/gladefiles/SliderFrame_2.glade
${PROJECT_SOURCE_DIR}/gui/settingsframe.cpp
${PROJECT_SOURCE_DIR}/gui/sliderwindow.cpp
${PROJECT_SOURCE_DIR}/gui/trayicon.cpp
${PROJECT_SOURCE_DIR}/tools/core.cpp"
)
file( WRITE ${PO_FULL_DIR}/POTFILES.in "${POTFILES_CONTENTS}" )

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
