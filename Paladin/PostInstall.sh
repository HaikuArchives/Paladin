#!/bin/sh
OSTYPE=`uname -o`
case $OSTYPE in
"Zeta")
	mkdir /boot/home/config/ZetaMenu/Software/Development/Paladin/
	ln -s /boot/apps/Paladin/Paladin /boot/home/config/ZetaMenu/Software/Development/Paladin/
	ln -s /boot/apps/Paladin/PalEdit /boot/home/config/ZetaMenu/Software/Development/Paladin/
	ln -s /boot/apps/Paladin/Paladin\ 1.2\ Documentation.pdf /boot/home/config/ZetaMenu/Software/Development/Paladin/
	mv -f ccache.zeta /boot/home/config/bin/ccache
	;;
"Haiku")
	mkdir /boot/home/config/be/Applications/Paladin/
	ln -s /boot/apps/Paladin/Paladin /boot/home/config/be/Applications/Paladin/
	ln -s /boot/apps/Paladin/PalEdit /boot/home/config/be/Applications/Paladin/
	ln -s /boot/apps/Paladin/Paladin\ 1.2\ Documentation.pdf /boot/home/config/be/Applications/Paladin/
	mv -f ccache.haiku /boot/home/config/bin/ccache
	;;
*)
	mkdir /boot/home/config/be/Applications/Paladin/
	ln -s /boot/apps/Paladin/Paladin /boot/home/config/be/Applications/Paladin/
	ln -s /boot/apps/Paladin/PalEdit /boot/home/config/be/Applications/Paladin/
	ln -s /boot/apps/Paladin/Paladin\ 1.2\ Documentation.pdf /boot/home/config/be/Applications/Paladin/
	;;
esac
