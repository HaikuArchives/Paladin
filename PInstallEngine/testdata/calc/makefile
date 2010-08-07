## BeOS Generic Makefile v2.0b4 ##

## Fill in this file to specify the project being created, and the referenced
## makefile-engine will do all of the hard work for you.  This handles both
## Intel and PowerPC builds of the BeOS.

## Application Specific Settings ---------------------------------------------

# specify the name of the binary
NAME= calc

# specify the type of binary
#	APP:	Application
#	SHARED:	Shared library or add-on
#	STATIC:	Static library archive
#	DRIVER: Kernel Driver
TYPE= APP

# specify the source files to use
#	full paths or paths relative to the makefile can be included
# 	all files, regardless of directory, will have their object
#	files created in the common object directory.
#	Note that this means this makefile will not work correctly
#	if two source files with the same name (source.c or source.cpp)
#	are included from different directories.  Also note that spaces
#	in folder names do not work well with this makefile.
SRCS=	source/Calculator.cpp \
		source/CalcWindow.cpp \
		source/CalcView.cpp \
		source/FrameView.cpp \
		source/CalcEngine.cpp \
		source/Prefs.cpp
		
# specify the resource files to use
#	full path or a relative path to the resource file can be used.
RSRCS=	source/Calculator_x86.rsrc

#specify additional libraries to link against
#	if libName.so or libName.a is the name of the library to link against
# 	then simply specify Name in the LIBS list
# 	if there is another naming scheme use the full binary
#	name: my_library.so or my_lib.a
#	libroot.so never needs to be specified here, although libbe.so does
LIBS=	be \
		stdc++.r4
		

#	specify the paths to directories where additional
# 	libraries are to be found.  /boot/develop/lib/PLATFORM/ is
#	already set.  The paths can be full or relative to this
#	makefile.  The paths included may not be recursive, so
#	specify all of the needed paths explicitly
#	Directories containing source-files are automatically added.
LIBPATHS= 

#	specify additional directories where header files can be found
# 	directories where sources are found are included automatically
#	included.
INCPATHS= 

#	specify the level of optimization that you desire
#	NONE, SOME, FULL
OPTIMIZE= 

#	specify any preprocessor symbols to be defined.  The symbols
#	will be set to a value of 1.  For example specify DEBUG if you want
#	DEBUG=1 to be set when compiling.
DEFINES= 

#	specify special warning levels
#	if unspecified default warnings will be used
#	NONE = supress all warnings
#	ALL = enable all warnings
WARNINGS = 

#	specify whether image symbols will be created
#	so that stack crawls in the debugger are meaningful
#	if TRUE symbols will be created
SYMBOLS = 


#	specify debug settings
#	if TRUE will allow application to be run from
#	a source-level debugger
DEBUGGER = 



## include the makefile-engine
include /boot/develop/etc/makefile-engine

