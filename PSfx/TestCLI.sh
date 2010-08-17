#!/bin/sh
cd `dirname $0`

# Test script for the command-line mode of PSfx

# Script Setup
PKGPATH=/boot/home/Desktop/calc.sfx

rm -f $PKGPATH

# Create a package
PSfx $PKGPATH makepkg

# Set some information for the package
PSfx $PKGPATH setpkginfo appname="Calc" appversion="1.0.0" author="CalcAuthor" authorcontact="authoremail@foo.com" releasedate="August 17, 2010" installfolder="/boot/apps" createfoldername="calc" url="http://darkwyrm.beemulated.net"

# We'll use rez and libcurl as fake dependencies for testing
PSfx $PKGPATH adddep rez file /boot/home/config/bin foourl
PSfx $PKGPATH adddep libcurl library libcurl.so "http://curl.haxx.se/"

# Just to make sure that setdep works even though adddep has almost
# the exact same code path
PSfx $PKGPATH setdep rez file /boot/home/config/bin

# Now test remove libcurl to test deldep
PSfx $PKGPATH deldep libcurl

# File operations tests
PSfx $PKGPATH addfile ../PInstallEngine/testdata/calc/calc.x86
PSfx $PKGPATH addfile ../PInstallEngine/testdata/calc/calc.x86.proj
PSfx $PKGPATH addfile ../PInstallEngine/testdata/calc/makefile
PSfx $PKGPATH addfile ../PInstallEngine/testdata/calc/ReadMe.Calculator.txt

PSfx $PKGPATH dumppkg
