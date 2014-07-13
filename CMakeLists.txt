set (PROJECT alsavolume)
cmake_minimum_required( VERSION 2.6 )

if ( NOT UNIX )
	message (FATAL_ERROR "Not Unix!")
endif ( NOT UNIX )

find_package ( PkgConfig REQUIRED )

pkg_check_modules ( GTKMM REQUIRED gtkmm-3.0 )
pkg_check_modules ( ALSA REQUIRED alsa )

link_directories (
    ${GTKMM_LIBRARY_DIRS}
    ${ALSA_LIBRARY_DIRS}
)

include_directories (
    ${GTKMM_INCLUDE_DIRS}
    ${ALSA_INCLUDE_DIRS}
)

set ( CMAKE_BUILD_TYPE Release )
add_definitions ( ${GTKMM_CFLAGS_OTHER} ${ALSA_FLAGS_OTHER} )
    
set ( alsavolume_HDRS
	alsawork/alsawork.h
	alsawork/volumemixers.h
	alsawork/mixerswitches.h
	gui/trayicon.h
	gui/sliderwindow.h
	gui/settingsframe.h
	tools/settings.h
	tools/tools.h
	tools/settingsstr.h
	tools/core.h
)

set ( alsavolume_SRCS
	main.cpp
	gui/trayicon.cpp
	gui/sliderwindow.cpp
	gui/settingsframe.cpp
	alsawork/alsawork.cpp
	alsawork/volumemixers.cpp
	alsawork/mixerswitches.cpp
	tools/settings.cpp
	tools/tools.cpp
	tools/settingsstr.cpp
	tools/core.cpp
)

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
	
add_executable ( ${PROJECT} ${alsavolume_SRCS} )

target_link_libraries ( ${PROJECT} ${GTKMM_LIBRARIES} ${ALSA_LIBRARIES} )

INSTALL ( TARGETS ${PROJECT} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
INSTALL ( FILES ${DESKTOP_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}share/applications )
INSTALL ( FILES ${ICON_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${PROJECT}/icons )
INSTALL ( FILES ${GLADE_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${PROJECT}/gladefiles )