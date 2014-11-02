FIND_PATH(ARGPARSE_INCLUDE_DIR ArgParse.h
	HINTS
	$ENV{ARGPARSE}
	PATH_SUFFIXES include/ArgParse include ArgParse
	i686-w64-mingw32/include/ArgParse
	x86_64-w64-mingw32/include/ArgParse
	PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local/include/ArgParse
	/usr/include/ArgParse
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
)

# Lookup the 64 bit libs on x64
IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
	FIND_LIBRARY(ARGPARSE_LIBRARY_TEMP ArgParse
		HINTS
		$ENV{ARGPARSE}
		PATH_SUFFIXES lib64 lib
		lib/x64
		x86_64-w64-mingw32/lib
		PATHS
		/sw
		/opt/local
		/opt/csw
		/opt
	)
# On 32bit build find the 32bit libs
ELSE(CMAKE_SIZEOF_VOID_P EQUAL 8)
	FIND_LIBRARY(ARGPARSE_LIBRARY_TEMP ArgParse
		HINTS
		$ENV{ARGPARSE}
		PATH_SUFFIXES lib
		lib/x86
		i686-w64-mingw32/lib
		PATHS
		/sw
		/opt/local
		/opt/csw
		/opt
	)
ENDIF(CMAKE_SIZEOF_VOID_P EQUAL 8)

SET(ARGPARSE_FOUND "NO")
	IF(ARGPARSE_LIBRARY_TEMP)
		# Set the final string here so the GUI reflects the final state.
		SET(ARGPARSE_LIBRARY ${ARGPARSE_LIBRARY_TEMP} CACHE STRING "Where the ArgParse Library can be found")
		# Set the temp variable to INTERNAL so it is not seen in the CMake GUI
		SET(ARGPARSE_LIBRARY_TEMP "${ARGPARSE_LIBRARY_TEMP}" CACHE INTERNAL "")

		SET(ARGPARSE_FOUND "YES")
ENDIF(ARGPARSE_LIBRARY_TEMP)

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(ARGPARSE REQUIRED_VARS ARGPARSE_LIBRARY ARGPARSE_INCLUDE_DIR)
