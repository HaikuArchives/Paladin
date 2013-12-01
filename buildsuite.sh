#!/bin/sh

START_DIRECTORY=`pwd`

# ----------------------------------------------------------------------------
# SETUP
# ----------------------------------------------------------------------------
if [ -z "$1" ]
then
	echo "usage: $0 <cpucount>"
	exit -1;
else
	CPUCOUNT=$1
fi

if [ "$2" == "clean" ]
then
	MAKECLEAN=1
else
	MAKECLEAN=0
fi

PLATFORM=`uname -o`

# the R5 version of uname lacks a -o switch
if [ "$?" -ne 0 ]
then
	PLATFORM="beos"
fi
echo "Platform: $PLATFORM"

# This function creates a temporary project from which we make
# sure that the target is being built without debugging enabled
BuildNoDebug ()
{
	if [ -z "$1" ]
	then
		echo "Function BuildNoDebug called without parameter\n"
		exit -1
	fi

	PROJEXT=".pld"
	TEMPEXT=".nodebug.pld"
	PROJNAME=$1$PROJEXT
	TMPPROJ=$1$TEMPEXT
	
	echo "Building temporary project $TMPPROJ"
	OBJDIRNAME='(Objects.'$1'.nodebug)'
	mkdir -p $OBJDIRNAME
	
	# do build here
	# using absolute paths here because chroot folder for some reasons that are
	# unknown to me includes /system/develop, but does not contain /boot/system/develop
	sed 's/CCDEBUG=yes//' "$PROJNAME" | sed 's/CCOPLEVEL=[0-9]/CCOPLEVEL=3/' | sed 's_/boot/develop_/system/develop_' | sed 's_/system/develop/headers/cpp_/system/develop/headers/c++_' | sed 's_/system/develop/lib/x86_/system/develop/lib_' > "$TMPPROJ"
	
	$START_DIRECTORY/Paladin/Paladin -b "$TMPPROJ"
	SUCCESS=$?
	
	if [ "$MAKECLEAN" == 1 ]
	then
		rm "$TMPPROJ"
		if [ -d "$OBJDIRNAME" ]
		then
			rm -r "$OBJDIRNAME"
		fi
	fi
	
	if [ "$SUCCESS" -ne 0 ]
	then
		echo "Failed to build $TMPPROJ. Error code $SUCCESS" 
		exit 2;
	else
		echo "$PROJNAME has been built"
	fi
}

# ----------------------------------------------------------------------------
# BUILD
# ----------------------------------------------------------------------------

# Attempt to build Paladin
cd Paladin
rm -f Paladin Paladin.new
echo "Building Paladin"
if [ "$PLATFORM" == "Haiku" ]
then
	if [ -e /boot/system/lib/libsupc++.so ]
	then
		buildhaikugcc4.sh
	else
		buildhaikugcc2.sh
	fi
else
	build.sh
fi
if [ ! -e Paladin ]
then
	echo "Paladin build failed."
	exit 1
fi

# Make sure there's a link to make calling it easier
#if [ ! -e /boot/home/config/bin/Paladin ]
#then
#	ln -s --target-directory=/boot/home/config/bin/ `pwd`"/Paladin"
#fi

# Paladin usually expects PalEdit (or a link) to be in the same directory
# The link will be broken for the moment, but will work once the script
# finishes
ln -s ../PalEdit/generated/distro/PalEdit

cd ../

# Now attempt ccache
echo "\nBuilding ccache"
cd ccache
BuildNoDebug ccache
cd ..

# fastdep doesn't use a project file to build itself
echo "\nBuilding fastdep"
cd fastdep-0.16
if [ "$MAKECLEAN" -eq 1 ]
then
	configure
	make clean
fi
make
cd ..

# PalEdit
cd PalEdit
# substitute old paths with new ones
PE_FILES_SED='Jamrules Extensions/Jamfile Sources/Jamfile build/BuildSettings'
sed -i 's_/boot/home/config/include_'`finddir B_USER_HEADERS_DIRECTORY`'_' $PE_FILES_SED
sed -i 's_/boot/develop/lib/x86_'`finddir B_SYSTEM_LIB_DIRECTORY`'_' $PE_FILES_SED
sed -i 's_/boot/home/config/etc_'`finddir B_USER_SETTINGS_DIRECTORY`'_' $PE_FILES_SED
sed -i 's_/boot/common/include_'`finddir B_SYSTEM_HEADERS_DIRECTORY`'_' $PE_FILES_SED
sed -i 's_/boot/develop/headers_'`finddir B_SYSTEM_HEADERS_DIRECTORY`'_' $PE_FILES_SED
sed -i 's|COMMON_FOLDER = /boot/common|COMMON_FOLDER = '`finddir B_USER_CONFIG_DIRECTORY`'|' $PE_FILES_SED

if [ "$MAKECLEAN" == 1 ]
then
	jam clean
fi
jam -q -j"$CPUCOUNT"
cd ..

cd SymbolFinder
BuildNoDebug SymbolFinder
cd ..

cd uncrustify
configure --prefix=`finddir B_USER_CONFIG_DIRECTORY`
make -j$CPUCOUNT
cd ..
