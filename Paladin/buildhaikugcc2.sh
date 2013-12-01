#!/bin/sh

# A magical bit of bash wizardry to use all C++ files except those in the
# Templates folder.
export SOURCES=`find ./ -name "*.cpp" -print | grep -v "Templates" | sed 's_\n__'`
echo $SOURCES
gcc -pipe -D_ZETA_TS_FIND_DIR_ -o Paladin $SOURCES -I ./BuildSystem -I ./ -I ./ThirdParty -I ./SourceControl -I $(finddir B_USER_HEADERS_DIRECTORY) -L $(finddir B_USER_DEVELOP_DIRECTORY)/lib -lbe -lroot -ltracker -ltranslation -lstdc++.r4 -lpcre -llocale
xres -o Paladin Paladin.rsrc
mimeset -all Paladin
