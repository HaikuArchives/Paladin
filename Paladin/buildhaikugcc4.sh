#!/bin/sh

# A magical bit of bash wizardry to use all C++ files except those in the
# Templates folder.
export SOURCES=`find ./ -name "*.cpp" -print | grep -v "Templates" | sed 's_\n__'`
echo $SOURCES
gcc -pipe -D_ZETA_TS_FIND_DIR_ -o Paladin $SOURCES -I ./BuildSystem -I ./ -I ./ThirdParty -I ./SourceControl -lbe -lroot -ltracker -ltranslation -lsupc++ -lstdc++ -lpcre -llocale
rc Paladin.rdef -o Paladin.rsrc
xres -o Paladin Paladin.rsrc
mimeset -all Paladin
