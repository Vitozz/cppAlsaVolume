cmake_minimum_required( VERSION 3.10.0 )

find_program(GLIB_COMPILE_RESOURCES
    NAMES glib-compile-resources
    REQUIRED
)
if(NOT GLIB_COMPILE_RESOURCES)
    message(FATAL_ERROR "glib-compile-resources program not found")
else()
    message(STATUS "glib-compile-resources program found at ${GLIB_COMPILE_RESOURCES}")
endif()

set(RESOURCE_XML "${CMAKE_CURRENT_BINARY_DIR}/resources.xml")
set(SLIDER_GLADE "SliderFrame.glade")
set(SETTINGS_GLADE "SettingsFrame.glade")

configure_file("${PROJECT_SOURCE_DIR}/resources.xml.in" "${RESOURCE_XML}" @ONLY)


set(RESOURCE_C
    ${CMAKE_CURRENT_BINARY_DIR}/resources.c
)

add_custom_command(
    OUTPUT ${RESOURCE_C}
    COMMAND ${GLIB_COMPILE_RESOURCES}
    --generate-source
    --target=${RESOURCE_C}
    --sourcedir=${PROJECT_SOURCE_DIR}
    ${RESOURCE_XML}
    DEPENDS
        ${RESOURCE_XML}
        "gladefiles/${SLIDER_GLADE}"
        "gladefiles/${SETTINGS_GLADE}"
        VERBATIM
)

add_custom_target(resources_target
    DEPENDS ${RESOURCE_C}
)
