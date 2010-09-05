#!/bin/sh
cd `dirname $0`

# Test script for the command-line mode of PSfx

# Script Setup
PKGPATH=/boot/home/Desktop/calc.sfx
PFXPATH=/boot/home/Desktop/calc.pfx

rm -f $PFXPATH
rm -f $PKGPATH

# Set some information for the package
PSfx setpkginfo $PFXPATH appname="Calc" appversion="1.0.0" author="Peter Wagner" authorcontact="pwagner@standordalumni.org" releasedate="August 17, 2010" installfolder="/boot/apps" createfoldername="calc" url="http://darkwyrm.beemulated.net" pkgversion=1.1

# File operations tests
PSfx addfile $PFXPATH ../PInstallEngine/testdata/calc/calc.gcc4 installedname=calc platform=HaikuGCC4 category=Accessories group=Minimal,Full link=calc
PSfx addfile $PFXPATH ../PInstallEngine/testdata/calc/ReadMe.Calculator.txt group=Minimal,Full link=Readme
PSfx addfile $PFXPATH ../PInstallEngine/testdata/calc/source/calc.pld group=Full
PSfx addfile $PFXPATH ../PInstallEngine/testdata/calc/source/CalcEngine.cpp group=Full
PSfx addfile $PFXPATH ../PInstallEngine/testdata/calc/source/CalcEngine.h group=Full
PSfx addfile $PFXPATH ../PInstallEngine/testdata/calc/source/Calculator.cpp group=Full
PSfx addfile $PFXPATH ../PInstallEngine/testdata/calc/source/Calculator.h group=Full
PSfx addfile $PFXPATH ../PInstallEngine/testdata/calc/source/Calculator_x86.rsrc group=Full
PSfx addfile $PFXPATH ../PInstallEngine/testdata/calc/source/CalcView.cpp group=Full
PSfx addfile $PFXPATH ../PInstallEngine/testdata/calc/source/CalcView.h group=Full
PSfx addfile $PFXPATH ../PInstallEngine/testdata/calc/source/CalcWindow.cpp group=Full
PSfx addfile $PFXPATH ../PInstallEngine/testdata/calc/source/CalcWindow.h group=Full
PSfx addfile $PFXPATH ../PInstallEngine/testdata/calc/source/FrameView.cpp group=Full
PSfx addfile $PFXPATH ../PInstallEngine/testdata/calc/source/FrameView.h group=Full
PSfx addfile $PFXPATH ../PInstallEngine/testdata/calc/source/Prefs.cpp group=Full
PSfx addfile $PFXPATH ../PInstallEngine/testdata/calc/source/Prefs.h group=Full

PSfx makepkg $PFXPATH $PKGPATH
