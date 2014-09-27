set(PROJECT alsavolume)
cmake_minimum_required( VERSION 2.8.11 )

if( NOT UNIX )
	message(FATAL_ERROR "Not Unix!")
endif( NOT UNIX )

set(USE_PULSE OFF CACHE BOOL "Set pulseaudio usage" )

find_package( PkgConfig REQUIRED )

pkg_check_modules( GTKMM REQUIRED gtkmm-3.0 )
pkg_check_modules( ALSA REQUIRED alsa )

set( CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
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



set ( DESKTOP_FILES ${PROJECT}.desktop )
set ( ICON_FILES 
	icons/tb_icon0.png
	icons/tb_icon20.png
	icons/tb_icon40.png
	icons/tb_icon60.png
	icons/tb_icon80.png
	icons/tb_icon100.png
	icons/volume.png
	icons/volume_ico.png
)
set(GLADE_FILES 
	gladefiles/SliderFrame.glade
	gladefiles/SettingsFrame.glade
)
	
add_executable( ${PROJECT}
	${alsavolume_SRCS}
)
target_link_libraries( ${PROJECT} ${DEPLIBS} )

INSTALL ( TARGETS ${PROJECT} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
INSTALL ( FILES ${DESKTOP_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications )
INSTALL ( FILES ${ICON_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${PROJECT}/icons )
INSTALL ( FILES ${GLADE_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${PROJECT}/gladefiles )
