#!/bin/sh
OSTYPE=`uname -o`
case $OSTYPE in
"Zeta")
	mkdir /boot/home/config/ZetaMenu/Software/Development/Paladin/
	ln -s /boot/apps/Paladin/Paladin /boot/home/config/ZetaMenu/Software/Development/Paladin/
	ln -s /boot/apps/Paladin/PalEdit /boot/home/config/ZetaMenu/Software/Development/Paladin/
	ln -s /boot/apps/Paladin/README /boot/home/config/ZetaMenu/Software/Development/Paladin/
	ln -s /boot/apps/Paladin/WHAT\'S\ NEW /boot/home/config/ZetaMenu/Software/Development/Paladin/
	ln -s /boot/apps/Paladin/Paladin\ Documentation.pdf /boot/home/config/ZetaMenu/Software/Development/Paladin/
	mv -f ccache.zeta /boot/home/config/bin/ccache
	;;
"Haiku")
	mkdir /boot/home/config/be/Applications/Paladin/
	ln -s /boot/apps/Paladin/Paladin /boot/home/config/be/Applications/Paladin/
	ln -s /boot/apps/Paladin/PalEdit /boot/home/config/be/Applications/Paladin/
	ln -s /boot/apps/Paladin/README /boot/home/config/be/Applications/Paladin/
	ln -s /boot/apps/Paladin/WHAT\'S\ NEW /boot/home/config/be/Applications/Paladin/
	ln -s /boot/apps/Paladin/Paladin\ Documentation.pdf /boot/home/config/be/Applications/Paladin/
	mv -f ccache /boot/home/config/bin/ccache
	mv -f fastdep /boot/home/config/bin/ccache
	;;
*)
	mkdir /boot/home/config/be/Applications/Paladin/
	ln -s /boot/apps/Paladin/Paladin /boot/home/config/be/Applications/Paladin/
	ln -s /boot/apps/Paladin/PalEdit /boot/home/config/be/Applications/Paladin/
	ln -s /boot/apps/Paladin/README /boot/home/config/be/Applications/Paladin/
	ln -s /boot/apps/Paladin/WHAT\'S\ NEW /boot/home/config/be/Applications/Paladin/
	ln -s /boot/apps/Paladin/Paladin\ Documentation.pdf /boot/home/config/be/Applications/Paladin/
	;;
esac
