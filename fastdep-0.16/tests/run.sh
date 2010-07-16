#!/bin/sh

runonetest()
{
	echo "Testing " $1
	rm -f last/$1.out
	../fastdep $1 > last/$1.out
	diff -u good/$1.out last/$1.out || exit 1
}

runonescript()
{
	echo "Testing " $1
	rm -f last/$1.out
	./$1 > last/$1.out
	diff -u good/$1.out last/$1.out
}

mkdir -p last

for i in *.cc; do
	runonetest $i
done

#for i in *.script; do
#	runonescript $i
#done
