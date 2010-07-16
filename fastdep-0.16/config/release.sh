#!/bin/sh

# Shell script to switch between debug/release versions

# step 1 : check command line

release_commandline()
{
	debugprofile=
	releaseprofile=yes
	debianprofile=
	for i in $@
	do
		value=${i#--debug}
		if [ ${#i} -ne ${#value} ]
		then
			debugprofile=yes
			releaseprofile=
			debianprofile=
		fi
		value=${i#--release}
		if [ ${#i} -ne ${#value} ]
		then
			debugprofile=
			releaseprofile=yes
			debianprofile=
		fi
		value=${i#--debian}
		if [ ${#i} -ne ${#value} ]
		then
			debugprofile=
			releaseprofile=
			debianprofile=yes
		fi
	done
}

# step 2 : react

release_go()
{
	if [ -n "$debugprofile" ]
	then
		echo "DEBUGSYMBOLS=yes" >> config.me
		echo "OPTIMIZE=no" >> config.me
	fi
	if [ -n "$releaseprofile" ]
	then
		echo "DEBUGSYMBOLS=no" >> config.me
		echo "OPTIMIZE=yes" >> config.me
	fi
	if [ -n "$debianprofile" ]
	then
		echo "DEBUGSYMBOLS=no" >> config.me
		echo "OPTIMIZE=no" >> config.me
	fi
}

# step 0 : give help

release_help()
{
	echo -e "\t--debug"
	echo -e "\t\tinclude debugging symbols"
	echo -e "\t--release [default]"
	echo -e "\t\tdon't include debugging symbols and optimize"
	echo -e "\t--debian"
	echo -e "\t\tdon't include debugging symbols, don't optimize (if you want to use CFLAGS instead)"
}
