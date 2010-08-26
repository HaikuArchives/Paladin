#!/bin/sh
cd `dirname $0`
APPVERSION=`version -n Paladin/Paladin | sed "s/\([0-9]\) \([0-9]\) [0-9] [a-z] [0-9]/\1.\2/"`
SRCZIPNAME=Paladin-$APPVERSION.src.zip
if [ -e $SRCZIPNAME ]
then
	rm $SRCZIPNAME
fi

zip -r $SRCZIPNAME Paladin ccache fastdep-0.16 SymbolFinder -x *.o -x *nodebug*
zip -r $SRCZIPNAME PalEdit/build PalEdit/Extensions PalEdit/Languages PalEdit/Resources PalEdit/rez PalEdit/Sources
zip $SRCZIPNAME -d Paladin/Paladin Paladin/Paladin.new SymbolFinder/SymbolFinder  ccache/ccache fastdep-0.16/fastdep
