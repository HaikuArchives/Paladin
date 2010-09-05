#!/bin/sh
cd `dirname $0`

# Test script for the command-line mode of PSfx

# Script Setup
PKGPATH=/boot/home/Desktop/calc.sfx
PFXPATH=/boot/home/Desktop/calc.pfx

rm -f $PFXPATH

# Set some information for the package
PSfx setpkginfo $PFXPATH appname="Calc" appversion="1.0.0" author="CalcAuthor" authorcontact="authoremail@foo.com" releasedate="August 17, 2010" installfolder="/boot/apps" createfoldername="calc" url="http://darkwyrm.beemulated.net"

# We'll use rez and libcurl as fake dependencies for testing
PSfx adddep $PFXPATH rez file /boot/home/config/bin foourl
PSfx adddep $PFXPATH libcurl library libcurl.so "http://curl.haxx.se/"

# Just to make sure that setdep works even though adddep has almost
# the exact same code path
PSfx setdep $PFXPATH rez file /boot/home/config/bin

# Now test remove libcurl to test deldep
PSfx deldep $PFXPATH libcurl

# File operations tests
PSfx addfile $PFXPATH ../PInstallEngine/testdata/calc/calc.x86
PSfx addfile $PFXPATH ../PInstallEngine/testdata/calc/calc.x86.proj
PSfx addfile $PFXPATH ../PInstallEngine/testdata/calc/makefile
PSfx addfile $PFXPATH ../PInstallEngine/testdata/calc/ReadMe.Calculator.txt

PSfx dumppkg $PFXPATH
