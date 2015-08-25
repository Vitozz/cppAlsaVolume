if( Intl_INCLUDE_DIR AND Intl_LIBRARY )
	set(Intl_FIND_QUIETLY TRUE)
endif( Intl_INCLUDE_DIR AND Intl_LIBRARY )

find_path( Intl_INCLUDE_DIR "libintl.h" )
find_library( Intl_LIBRARY "intl" )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
				Intl
				DEFAULT_MSG
				Intl_INCLUDE_DIR
)
if( Intl_FOUND )
	set( Intl_LIBRARIES ${IDN_LIBRARY} )
	set( Intl_INCLUDE_DIRS ${IDN_INCLUDE_DIR} )
endif( Intl_FOUND )

mark_as_advanced( Intl_INCLUDE_DIR Intl_LIBRARY )
