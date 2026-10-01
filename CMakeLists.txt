cmake_minimum_required( VERSION 3.10.0 )
project(alsavolume)

if( NOT UNIX )
	message( FATAL_ERROR "Not Unix!" )
endif()

if(POLICY CMP0058)
    cmake_policy(SET CMP0058 NEW)
endif()

option( USE_PULSE "Enable pulseaudio support" OFF )
set( CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" )
option( USE_APPINDICATOR "Use appindicator support" OFF )
option( USE_SNI "Enable StatusNotifierItem support" ON )
option( USE_CPACK "Enable CPack support" OFF )

include(GNUInstallDirs)

find_package( PkgConfig REQUIRED )
find_package( Intl REQUIRED )

if( USE_SNI AND USE_APPINDICATOR )
	message(FATAL_ERROR "
Both Flags USE_SNI and USE_APPINDICATOR are enabled!
Please disable at least one of them.")
endif()

if( USE_SNI )
	option(USE_DBUSMENU "Use libdbusmenu to create context menu for StatusNotifierItem" ON)
endif()

set( GTKMM_NAME "gtkmm-3.0" )
if( USE_APPINDICATOR )
	set(APPINDICATOR_NAME "appindicator3-0.1")
endif()
if( USE_DBUSMENU )
	set(DBUSMENU_NAME "dbusmenu-gtk3-0.4")
endif()

pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET "${GTKMM_NAME}")

find_package( ALSA REQUIRED )
set( CUSTOM_LDFLAGS "-Wl,-as-needed" )
set( CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")

if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" OR ( "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo" ) )
	add_definitions( -DIS_DEBUG )
	set( CMAKE_CXX_FLAGS_DEBUG "-O0 ${CMAKE_CXX_FLAGS_DEBUG}")
endif()

if(ALSA_FOUND)
	message(STATUS "Found ALSA: (found version \"${ALSA_VERSION}\")")
endif()

include_directories(
	${ALSA_INCLUDE_DIRS}
)
set( DEPLIBS
	PkgConfig::GTKMM
	${ALSA_LIBRARIES}
)

if( USE_APPINDICATOR )
	pkg_check_modules( APPINDICATOR REQUIRED IMPORTED_TARGET ${APPINDICATOR_NAME} )
	add_definitions( -DUSE_APPINDICATOR )
	list(APPEND DEPLIBS
		PkgConfig::APPINDICATOR
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
	include(statusnotifier-gen)
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
	if(GLIB_MKENUMS)
		mark_as_advanced( GLIB_MKENUMS )
		message(STATUS "GLIB_MKENUMS: ${GLIB_MKENUMS}")
	endif()
	set(enums_h "${CMAKE_CURRENT_BINARY_DIR}/enums/enums.h")
	set(enums_c "${CMAKE_CURRENT_BINARY_DIR}/enums/enums.c")
	add_custom_command(
		OUTPUT
		${enums_h}
		${enums_c}
		COMMAND
		${GLIB_MKENUMS}
		ARGS
		--template ${CMAKE_CURRENT_SOURCE_DIR}/third-party/statusnotifier/src/enums.h.template
		${CMAKE_CURRENT_SOURCE_DIR}/third-party/statusnotifier/src/statusnotifier.h --output ${enums_h}
		COMMAND
		${GLIB_MKENUMS}
		ARGS
		--template ${CMAKE_CURRENT_SOURCE_DIR}/third-party/statusnotifier/src/enums.c.template
		${CMAKE_CURRENT_SOURCE_DIR}/third-party/statusnotifier/src/statusnotifier.h --output ${enums_c}
		BYPRODUCTS byproducts.txt
		DEPENDS
		${GLIB_MKENUMS}
		${CMAKE_CURRENT_SOURCE_DIR}/third-party/statusnotifier/src/enums.h.template
		${CMAKE_CURRENT_SOURCE_DIR}/third-party/statusnotifier/src/enums.c.template
		${CMAKE_CURRENT_SOURCE_DIR}/third-party/statusnotifier/src/statusnotifier.h
		VERBATIM
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/enums
	)
        message(STATUS "${enums_h}")
	message(STATUS "${enums_c}")
	add_custom_target(prepare_enums DEPENDS ${enumc_h} ${enums_c})
	add_library(statusnotifier OBJECT ${enums_c} ${enumc_h} ${statusnotifier_SRCS} ${statusnotifier_HDRS})
	target_include_directories(statusnotifier BEFORE PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/enums)
	if(USE_DBUSMENU)
		pkg_check_modules( DBUSMENU_GTK REQUIRED ${DBUSMENU_NAME} )
		target_compile_definitions(statusnotifier PRIVATE -DUSE_DBUSMENU)
		target_link_directories(statusnotifier PUBLIC ${DBUSMENU_GTK_LIBRARY_DIRS})
		target_include_directories(statusnotifier PUBLIC ${DBUSMENU_GTK_INCLUDE_DIRS})
	endif()
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

include(fix-codestyle)
include(gen-resources)

if( USE_SNI )
    add_executable( ${PROJECT_NAME} ${RESOURCE_C} ${alsavolume_SRCS} ${alsavolume_HDRS} $<TARGET_OBJECTS:statusnotifier>)
	target_compile_definitions(${PROJECT_NAME} PRIVATE -DUSE_KDE)
    add_dependencies( ${PROJECT_NAME} prepare_enums )
	if( USE_DBUSMENU )
		target_link_libraries(${PROJECT_NAME} ${DBUSMENU_GTK_LDFLAGS})
	endif()
else()
    add_executable( ${PROJECT_NAME} ${RESOURCE_C} ${alsavolume_SRCS} ${alsavolume_HDRS} )
endif()

add_dependencies(${PROJECT_NAME} resources_target)

target_link_libraries( ${PROJECT_NAME} ${CUSTOM_LDFLAGS} ${DEPLIBS} )

set( DATADIR_PATH ${CMAKE_INSTALL_DATAROOTDIR} )
set( ICON_FILES_PREFIX "icons/logo_")
set( ICON_INSTALL_PREFIX ${DATADIR_PATH}/icons/hicolor)

INSTALL( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} )
set( PREFIX "${CMAKE_INSTALL_PREFIX}" )
configure_file(${DESKTOP_FILES}.in "${PROJECT_BINARY_DIR}/${DESKTOP_FILES}" COPYONLY)
INSTALL( FILES "${PROJECT_BINARY_DIR}/${DESKTOP_FILES}" DESTINATION ${DATADIR_PATH}/applications )
set(RESOLUTIONS 16 32 48 64 128)
foreach(resolution ${RESOLUTIONS})
	unset(LOGO_NAME)
	set(LOGO_NAME "${ICON_FILES_PREFIX}${resolution}.png")
	install(FILES ${LOGO_NAME} DESTINATION "${ICON_INSTALL_PREFIX}/${resolution}x${resolution}/apps" RENAME ${PROJECT_NAME}.png)
endforeach()

if(USE_CPACK AND DPKG_PATH)
    INSTALL( FILES ${PROJECT_BINARY_DIR}/copyright DESTINATION ${CMAKE_INSTALL_DOCDIR} )
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
if(USE_CPACK)
	include(version)
	include(use_cpack)
endif()
