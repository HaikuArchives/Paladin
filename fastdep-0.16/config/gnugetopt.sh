#!/bin/sh

# Shell script to detect GNU getopt_long


# step 1 : check command line

gnugetopt_commandline()
{
	for i in $@
	do
		value=${i#--enable-local-gnugetopt}
		if [ ${#i} -ne ${#value} ]
		then
			enablelocalgnugetopt=true
			disablelocalgnugetopt=
		fi
		value=${i#--disable-local-gnugetopt}
		if [ ${#i} -ne ${#value} ]
		then
			enablelocalgnugetopt=
			disablelocalgnugetopt=true
		fi
	done
}

gnugetopt_trycompile()
{
	gcc gnugetopt.c 2> /dev/null > /dev/null
}


gnugetopt_go()
{
	echo -n "Checking for GNU getopt_long... "
	
	if [ -z "$enablelocalgnugetopt" -a -z "$disablelocalgnugetopt" ]
	then
		gnugetopt_trycompile
		if [ $? -ne "0" ]
		then
			enablelocalgnugetopt=true
		fi
	fi

	if [ -n "$enablelocalgnugetopt" ]
	then
		echo "provided by local copy"
		echo "LOCALGNUGETOPT=YES" >> config.me
	else
		echo "provided by standard library"
	fi
}

gnugetopt_help()
{
	echo -e "\t--enable-local-gnugetopt"
	echo -e "\t\tuse a local copy of GNU getopt_long"
	echo -e "\t--disable-local-gnugetopt"
	echo -e "\t\trely on standard libraries to provide GNU getopt_long"
}
