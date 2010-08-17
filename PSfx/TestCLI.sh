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
