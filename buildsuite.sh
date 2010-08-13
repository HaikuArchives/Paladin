#!/bin/sh

# ----------------------------------------------------------------------------
# SETUP
# ----------------------------------------------------------------------------
if [ "$1" == "clean" ]
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
echo "Building for $PLATFORM"

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
	
	# do build here
	sed 's/CCDEBUG=yes//' "$PROJNAME" | sed 's/CCOPLEVEL=[0-9]/CCOPLEVEL=3/' > "$TMPPROJ"
	
	Paladin -b "$TMPPROJ"
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
rm Paladin Paladin.new
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
if [ -ne /boot/home/config/Paladin ]
then
	ln -s Paladin /boot/home/config/bin/
fi

cd ../

# Now attempt ccache
cd ccache
BuildNoDebug ccache

# fastdep doesn't use a project file to build itself
cd fastdep-0.16
if [ "$MAKECLEAN" -eq 1 ]
then
	configure
	make clean
fi
make

# PalEdit
cd PalEdit
if [ "$MAKECLEAN" == 1 ]
then
	jam clean
fi
jam -q
cd ..

cd PSfx
BuildNoDebug PSfx
cd ..

cd SymbolFinder
BuildNoDebug SymbolFinder

# ----------------------------------------------------------------------------
# PACKAGE
# ----------------------------------------------------------------------------
