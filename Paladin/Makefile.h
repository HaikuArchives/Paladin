#ifndef MAKEFILE_H
#define MAKEFILE_H

/* HOW TO UPDATE THIS FILE:
 * 1. Create a new Makefile using Tracker. Modify the header.
 * 2. Delete the following sections:
 	- APP_VERSION
 * 3. Manually add the $@...@$ constants in.
 * 4. Make the following replacements with Pe in RegEx mode:
 	- " with \\"
 	- \n with \\n\\\n
 * 5. Paste it into here.
 */

const char* template_makefile = "## Haiku Generic Makefile v2.6 ##\n\
\n\
## Fill in this file to specify the project being created, and the referenced\n\
## Makefile-Engine will do all of the hard work for you. This handles any\n\
## architecture of Haiku.\n\
\n\
# The name of the binary.\n\
NAME = $@NAME@$\n\
\n\
# The type of binary, must be one of:\n\
#	APP:	Application\n\
#	SHARED:	Shared library or add-on\n\
#	STATIC:	Static library archive\n\
#	DRIVER: Kernel driver\n\
TYPE = $@TYPE@$\n\
\n\
# 	If you plan to use localization, specify the application's MIME signature.\n\
APP_MIME_SIG = $@APP_MIME_SIG@$\n\
\n\
#	The following lines tell Pe and Eddie where the SRCS, RDEFS, and RSRCS are\n\
#	so that Pe and Eddie can fill them in for you.\n\
#%{\n\
# @src->@ \n\
\n\
#	Specify the source files to use. Full paths or paths relative to the \n\
#	Makefile can be included. All files, regardless of directory, will have\n\
#	their object files created in the common object directory. Note that this\n\
#	means this Makefile will not work correctly if two source files with the\n\
#	same name (source.c or source.cpp) are included from different directories.\n\
#	Also note that spaces in folder names do not work well with this Makefile.\n\
SRCS = $@SRCS@$\n\
\n\
#	Specify the resource definition files to use. Full or relative paths can be\n\
#	used.\n\
RDEFS = $@RDEFS@$\n\
\n\
#	Specify the resource files to use. Full or relative paths can be used.\n\
#	Both RDEFS and RSRCS can be utilized in the same Makefile.\n\
RSRCS = $@RSRCS@$\n\
\n\
# End Pe/Eddie support.\n\
# @<-src@ \n\
#%}\n\
\n\
#	Specify libraries to link against.\n\
#	There are two acceptable forms of library specifications:\n\
#	-	if your library follows the naming pattern of libXXX.so or libXXX.a,\n\
#		you can simply specify XXX for the library. (e.g. the entry for\n\
#		\"libtracker.so\" would be \"tracker\")\n\
#\n\
#	-	for GCC-independent linking of standard C++ libraries, you can use\n\
#		$(STDCPPLIBS) instead of the raw \"stdc++[.r4] [supc++]\" library names.\n\
#\n\
#	- 	if your library does not follow the standard library naming scheme,\n\
#		you need to specify the path to the library and it's name.\n\
#		(e.g. for mylib.a, specify \"mylib.a\" or \"path/mylib.a\")\n\
LIBS = $@LIBS@$\n\
\n\
#	Specify additional paths to directories following the standard libXXX.so\n\
#	or libXXX.a naming scheme. You can specify full paths or paths relative\n\
#	to the Makefile. The paths included are not parsed recursively, so\n\
#	include all of the paths where libraries must be found. Directories where\n\
#	source files were specified are	automatically included.\n\
LIBPATHS = $@LIBPATHS@$\n\
\n\
#	Additional paths to look for system headers. These use the form\n\
#	\"#include <header>\". Directories that contain the files in SRCS are\n\
#	NOT auto-included here.\n\
SYSTEM_INCLUDE_PATHS = $@SYSTEM_INCLUDE_PATHS@$\n\
\n\
#	Additional paths paths to look for local headers. These use the form\n\
#	#include \"header\". Directories that contain the files in SRCS are\n\
#	automatically included.\n\
LOCAL_INCLUDE_PATHS = $@LOCAL_INCLUDE_PATHS@$\n\
\n\
#	Specify the level of optimization that you want. Specify either NONE (O0),\n\
#	SOME (O1), FULL (O2), or leave blank (for the default optimization level).\n\
OPTIMIZE := $@OPTIMIZE@$\n\
\n\
# 	Specify the codes for languages you are going to support in this\n\
# 	application. The default \"en\" one must be provided too. \"make catkeys\"\n\
# 	will recreate only the \"locales/en.catkeys\" file. Use it as a template\n\
# 	for creating catkeys for other languages. All localization files must be\n\
# 	placed in the \"locales\" subdirectory.\n\
LOCALES = $@LOCALES@$\n\
\n\
#	Specify all the preprocessor symbols to be defined. The symbols will not\n\
#	have their values set automatically; you must supply the value (if any) to\n\
#	use. For example, setting DEFINES to \"DEBUG=1\" will cause the compiler\n\
#	option \"-DDEBUG=1\" to be used. Setting DEFINES to \"DEBUG\" would pass\n\
#	\"-DDEBUG\" on the compiler's command line.\n\
DEFINES = $@DEFINES@$\n\
\n\
#	Specify the warning level. Either NONE (suppress all warnings),\n\
#	ALL (enable all warnings), or leave blank (enable default warnings).\n\
WARNINGS = $@WARNINGS@$\n\
\n\
#	With image symbols, stack crawls in the debugger are meaningful.\n\
#	If set to \"TRUE\", symbols will be created.\n\
SYMBOLS := $@SYMBOLS@$\n\
\n\
#	Includes debug information, which allows the binary to be debugged easily.\n\
#	If set to \"TRUE\", debug info will be created.\n\
DEBUGGER := $@DEBUGGER@$\n\
\n\
#	Specify any additional compiler flags to be used.\n\
COMPILER_FLAGS = $@COMPILER_FLAGS@$\n\
\n\
#	Specify any additional linker flags to be used.\n\
LINKER_FLAGS = $@LINKER_FLAGS@$\n\
\n\
#	(Only used when \"TYPE\" is \"DRIVER\"). Specify the desired driver install\n\
#	location in the /dev hierarchy. Example:\n\
#		DRIVER_PATH = video/usb\n\
#	will instruct the \"driverinstall\" rule to place a symlink to your driver's\n\
#	binary in ~/add-ons/kernel/drivers/dev/video/usb, so that your driver will\n\
#	appear at /dev/video/usb when loaded. The default is \"misc\".\n\
DRIVER_PATH = $@DRIVER_PATH@$\n\
\n\
## Include the Makefile-Engine\n\
DEVEL_DIRECTORY := \\\n\
	$(shell findpaths -r \"makefile_engine\" B_FIND_PATH_DEVELOP_DIRECTORY)\n\
include $(DEVEL_DIRECTORY)/etc/makefile-engine\n";

#endif // MAKEFILE_H
