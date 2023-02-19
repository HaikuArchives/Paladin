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

if [ ! -f "/boot/system/develop/headers/pcre.h" ]
then
	echo "pcre.h does not exist. Have you run pkgman install devel:libpcre ?"
	exit -1;
fi

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
	
	$START_DIRECTORY/Paladin/Paladin -d -v -s -b "$TMPPROJ"
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
make -j$CPUCOUNT 
make catkeys
make bindcatalogs
cd ../

cd SymbolFinder
BuildNoDebug SymbolFinder
cd ..

# Attempt to build and run tests
cd Tests
if [ ! -f "/boot/system/develop/headers/UnitTest++/UnitTest++.h" ]
then
	echo "UnitTest++.h does not exist. Have you run pkgman install unittest++_devel ?"
	exit -1;
fi

./compile.sh

if [ "$?" -eq "0" ]
then
	echo "Building tests completed OK"
else
	echo "Building tests failed"
	exit 1;
fi

for file in tests.o-*; do
    if [[ -f $file ]]; then
        rm $file
    fi
done

./tests.o

if [ "$?" -eq "0" ]
then
	echo "Tests succeeded OK"
else
	echo "Tests failed."
	exit 1;
fi
cd ..

