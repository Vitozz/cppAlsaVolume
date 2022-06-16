project(alsavolume)
cmake_minimum_required( VERSION 2.8.12 )

if( NOT UNIX )
	message( FATAL_ERROR "Not Unix!" )
endif()

option( USE_PULSE "Enable pulseaudio support" OFF )
option( USE_GTK3 "Build with gtkmm-3.0 support" ON )
set( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" )
option( USE_APPINDICATOR "Use appindicator support" OFF )
option( USE_SNI "Enable StatusNotifierItem support" OFF )
option( USE_CPACK "Enable CPack support" OFF )

find_package( PkgConfig REQUIRED )
find_package( Intl REQUIRED )

if( USE_SNI AND USE_APPINDICATOR )
	message(FATAL_ERROR "
Both Flags USE_SNI and USE_APPINDICATOR are enabled!
Please disable at least one of them.")
endif()

if( USE_GTK3 )
	pkg_check_modules( GTKMM REQUIRED gtkmm-3.0 )
	if( USE_APPINDICATOR )
		pkg_check_modules( APPINDICATOR REQUIRED appindicator3-0.1 )
		add_definitions( -DUSE_APPINDICATOR )
		link_directories( ${APPINDICATOR_LIBRARY_DIRS} )
		include_directories( ${APPINDICATOR_INCLUDE_DIRS} )
	endif()
else()
	pkg_check_modules( GTKMM REQUIRED gtkmm-2.4 )
	add_definitions( -DIS_GTK_2 )
	if( USE_APPINDICATOR )
		pkg_check_modules( APPINDICATOR REQUIRED appindicator-0.1 )
		add_definitions( -DUSE_APPINDICATOR )
		link_directories( ${APPINDICATOR_LIBRARY_DIRS} )
		include_directories( ${APPINDICATOR_INCLUDE_DIRS} )
	endif()
endif()

find_package( ALSA REQUIRED )
set( CUSTOM_LDFLAGS "-Wl,-as-needed" )
set( CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")

if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" OR ( "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo" ) )
	add_definitions( -DIS_DEBUG )
	set( CMAKE_CXX_FLAGS_DEBUG "-O0 ${CMAKE_CXX_FLAGS_DEBUG}")
endif()

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

if( USE_APPINDICATOR )
	list(APPEND DEPLIBS
		${APPINDICATOR_LIBRARIES}
	)
endif()

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

if( USE_SNI )
    include(statusnotifier-gen.cmake)
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
	set( statusnotifier_SRCS
		third-party/statusnotifier/src/closures.c
		third-party/statusnotifier/src/statusnotifier.c
	)

	set( statusnotifier_HDRS
		third-party/statusnotifier/src/interfaces.h
		third-party/statusnotifier/src/statusnotifier.h
		third-party/statusnotifier/src/closures.h
		${CMAKE_CURRENT_BINARY_DIR}/config.h
	)
	file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/enums")
	file(COPY ${PROJECT_SOURCE_DIR}/third-party/statusnotifier/src/statusnotifier.h DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/enums/)
	find_program( GLIB_MKENUMS glib-mkenums )
	mark_as_advanced( GLIB_MKENUMS )
	set(enums_h "${CMAKE_CURRENT_BINARY_DIR}/enums/enums.h")
	set(enums_c "${CMAKE_CURRENT_BINARY_DIR}/enums/enums.c")
	add_custom_command(OUTPUT ${enums_h}
		COMMAND ${GLIB_MKENUMS} ARGS
                --template ${CMAKE_CURRENT_SOURCE_DIR}/third-party/statusnotifier/src/enums.h.template
                ${CMAKE_CURRENT_SOURCE_DIR}/third-party/statusnotifier/src/statusnotifier.h > enums.h
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/enums
	)
	message("${enums.h}")
	add_custom_command(OUTPUT ${enums_c}
		COMMAND ${GLIB_MKENUMS} ARGS
                --template ${CMAKE_CURRENT_SOURCE_DIR}/third-party/statusnotifier/src/enums.c.template
                ${CMAKE_CURRENT_SOURCE_DIR}/third-party/statusnotifier/src/statusnotifier.h > enums.c
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/enums
	)
	add_custom_target(build_enums_h DEPENDS ${enums_h} SOURCES "${statusnotifier_SRCS}")
	add_custom_target(build_enums_c DEPENDS ${enums_c} SOURCES "${statusnotifier_SRCS}")
	list(APPEND statusnotifier_SRCS
		${CMAKE_CURRENT_BINARY_DIR}/enums/enums.c
	)
	set(APPEND statusnotifier_HDRS
		${CMAKE_CURRENT_BINARY_DIR}/enums/enums.h
	)
	include_directories(
		${CMAKE_CURRENT_BINARY_DIR}/enums
	)
	list(APPEND alsavolume_SRCS
		${statusnotifier_SRCS}
	)
	add_definitions( -DUSE_KDE )
endif()

if( USE_PULSE )
	find_package( PulseAudio REQUIRED )
	if( PulseAudio_FOUND )
		message(STATUS "Found PulseAudio: (found version \"${PulseAudio_VERSION}\")")
	endif()
	include_directories( ${PULSEAUDIO_INCLUDE_DIR} )
	list(APPEND alsavolume_HDRS
		pulsework/pulsecore.h
		pulsework/pulsedevice.h
	)
	list(APPEND alsavolume_SRCS
		pulsework/pulsecore.cpp
		pulsework/pulsedevice.cpp
	)
	add_definitions(
		-DHAVE_PULSE
	)
	list(APPEND DEPLIBS
		${PULSEAUDIO_LIBRARY}
	)
endif()

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
if( USE_GTK3 )
	set(GLADE_FILES
		gladefiles/SliderFrame.glade
		gladefiles/SettingsFrame.glade
	)
else()
	set(GLADE_FILES
		gladefiles/SliderFrame_2.glade
		gladefiles/SettingsFrame_2.glade
	)
endif()

add_executable( ${PROJECT_NAME} ${alsavolume_SRCS} )
if( USE_SNI )
	add_dependencies(${PROJECT_NAME} build_enums_h build_enums_c)
endif()

if(USE_CPACK)
    include(version.cmake)
    include(use_cpack.cmake)
endif()

target_link_libraries( ${PROJECT_NAME} ${CUSTOM_LDFLAGS} ${DEPLIBS} )

set( DATADIR_PATH ${CMAKE_INSTALL_PREFIX}/share )

INSTALL( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
set( PREFIX "${CMAKE_INSTALL_PREFIX}" )
configure_file(${DESKTOP_FILES}.in "${PROJECT_BINARY_DIR}/${DESKTOP_FILES}" @ONLY)
INSTALL( FILES "${PROJECT_BINARY_DIR}/${DESKTOP_FILES}" DESTINATION ${DATADIR_PATH}/applications )
INSTALL( FILES ${ICON_FILES} DESTINATION ${DATADIR_PATH}/${PROJECT_NAME}/icons )
INSTALL( FILES ${GLADE_FILES} DESTINATION ${DATADIR_PATH}/${PROJECT_NAME}/gladefiles )
if(USE_CPACK AND DPKG_PATH)
    INSTALL( FILES ${PROJECT_BINARY_DIR}/copyright DESTINATION ${DATADIR_PATH}/doc/${PROJECT_NAME} )
endif()
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
endforeach()
file( WRITE ${PO_FULL_DIR}/POTFILES.in "${POTFILES_CONTENTS}" )

add_custom_target(alsavolume-update-sources
	AV_INTLTOOL -m
	WORKING_DIRECTORY ${PO_FULL_DIR}
)
add_custom_command(OUTPUT ${PO_FULL_DIR}/${PROJECT_NAME}.pot
	COMMAND ${AV_XGETTEXT}
	ARGS
	--files-from ${PO_FULL_DIR}/POTFILES.in --keyword=_ -o ${PO_FULL_DIR}/${PROJECT_NAME}.pot --no-wrap --no-location
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
			${AV_MSGFMT}
			ARGS
			-c -o ${LOCALES_WORKDIR}/${locale}/LC_MESSAGES/${PROJECT_NAME}.mo ${PO_FULL_DIR}/${locale}.po
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
endforeach()
add_custom_target(alsavolume-transtalions
	ALL
	DEPENDS
	${PO_FULL_DIR}/${PROJECT_NAME}.pot
	${AV_CATALOG_TARGETS}
)
