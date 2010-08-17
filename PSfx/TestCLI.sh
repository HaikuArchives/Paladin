#!/bin/sh
cd `dirname $0`

# Test script for the command-line mode of PSfx

# Script Setup
PKGPATH=/boot/home/Desktop/calc.sfx

rm -f $PKGPATH

# Create a package
PSfx $PKGPATH makepkg
