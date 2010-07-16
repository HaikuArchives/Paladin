#!/bin/sh

# Shell script to detect fastdep (http://www.irule.be/bvh/c++/fastdep/)
#
# Sets FASTDEP_BIN

# setp 1 : check command line

fastdep_commandline()
{
	for i in $@
	do
		value=${i#--with-fastdep=}
		if [ ${#i} -ne ${#value} ]
		then
			fastdepcheck=$value
			nofastdepcheck=
		fi
		value=${i#--without-fastdep}
		if [ ${#i} -ne ${#value} ]
		then
			nofastdepcheck=true
		fi
	done
}

fastdep_go()
{
	if [ -z "$nofastdepcheck" ]
	then
		echo -n "Checking for fastdep... "
		if [ -z "$fastdepcheck" ]
		then
			if [ -n "$FASTDEP" ]
			then
				fastdepcheck=$FASTDEP
			else
				fastdepcheck=/usr/bin/fastdep
			fi
		fi

		if [ -x "$fastdepcheck" ]
		then
			echo "FASTDEP_BIN=$fastdepcheck" >> config.me
			echo $fastdepcheck
		else
			echo not found
		fi
	fi
}

fastdep_help()
{
	echo -e "\t--with-fastdep="
	echo -e "\t\tsets fastdep binary"
	echo -e "\t--without-fastdep"
	echo -e "\t\tdon't use fastdep to generate C/C++ dependencies"
}
