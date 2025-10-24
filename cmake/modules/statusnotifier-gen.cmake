cmake_minimum_required( VERSION 3.5 )

find_file(DLFCN_H NAMES dlfcn.h PATHS /usr/include)
if(NOT "${DLFCN_H}" STREQUAL "DLFCN_H-NOTFOUND")
    set(HAVE_DLFCN_H 1)
else()
    set(HAVE_DLFCN_H 0)
endif()

find_file(INTTYPES_H NAMES inttypes.h PATHS /usr/include)
if(NOT "${INTTYPES_H}" STREQUAL "INTTYPES_H-NOTFOUND")
    set(HAVE_INTTYPES_H 1)
else()
    set(HAVE_INTTYPES_H 0)
endif()

find_file(MEMORY_H NAMES memory.h PATHS /usr/include)
if(NOT "${MEMORY_H}" STREQUAL "MEMORY_H-NOTFOUND")
    set(HAVE_MEMORY_H 1)
else()
    set(HAVE_MEMORY_H 0)
endif()

find_file(STDINT_H NAMES stdint.h PATHS /usr/include)
if(NOT "${STDINT_H}" STREQUAL "STDINT_H-NOTFOUND")
    set(HAVE_STDINT_H 1)
else()
    set(HAVE_STDINT_H 0)
endif()

find_file(STDLIB_H NAMES stdlib.h PATHS /usr/include)
if(NOT "${STDLIB_H}" STREQUAL "STDLIB_H-NOTFOUND")
    set(HAVE_STDLIB_H 1)
else()
    set(HAVE_STDLIB_H 0)
endif()

find_file(STRINGS_H NAMES strings.h PATHS /usr/include)
if(NOT "${STRINGS_H}" STREQUAL "STRINGS_H-NOTFOUND")
    set(HAVE_STRINGS_H 1)
else()
    set(HAVE_STRINGS_H 0)
endif()

find_file(STRING_H NAMES string.h PATHS /usr/include)
if(NOT "${STRING_H}" STREQUAL "STRING_H-NOTFOUND")
    set(HAVE_STRING_H 1)
else()
    set(HAVE_STRING_H 0)
endif()

find_file(SYS_STAT_H NAMES stat.h PATHS /usr/include
         PATH_SUFFIXES
         x86_64-linux-gnu/sys
         i386-linux-gnu/sys
         /sys)
if(NOT "${SYS_STAT_H}" STREQUAL "SYS_STAT_H-NOTFOUND")
    set(HAVE_SYS_STAT_H 1)
else()
    set(HAVE_SYS_STAT_H 0)
endif()

find_file(SYS_TYPES_H NAMES types.h PATHS /usr/include
         PATH_SUFFIXES
         x86_64-linux-gnu/sys
         i386-linux-gnu/sys
         /sys)
if(NOT "${SYS_TYPES_H}" STREQUAL "SYS_TYPES_H-NOTFOUND")
    set(HAVE_SYS_TYPES_H 1)
else()
    set(HAVE_SYS_TYPES_H 0)
endif()

find_file(UNISTD_H NAMES unistd.h PATHS /usr/include)
if(NOT "${UNISTD_H}" STREQUAL "UNISTD_H-NOTFOUND")
    set(HAVE_UNISTD_H 1)
else()
    set(HAVE_UNISTD_H 0)
endif()

set(PKG_NAME "statusnotifier")
set(PACKAGE "${PKG_NAME}")
set(PACKAGE_BUGREPORT "jjk@jjacky.com")
set(PACKAGE_NAME "${PKG_NAME}")
set(PACKAGE_VERSION "1.0.0")
set(PACKAGE_STRING "${PKG_NAME} ${PACKAGE_VERSION}")
set(PACKAGE_TARNAME "${PKG_NAME}")
set(PACKAGE_URL "")
set(STDC_HEADERS 1)
set(VERSION "${PACKAGE_VERSION}")

configure_file(config.h.in config.h @ONLY)
