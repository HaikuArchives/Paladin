#!/bin/sh

echo "Assembling Paladin as a static library"
cd ../Paladin/objects.*-release
ar rvs paladin.a *.o

LIBGCOV=-lgcov
if [[ `uname -m` == BePC ]]; then
  LIBGCOV=
fi

echo "Compiling and linking tests against Paladin static library"
cd ../../Tests
g++ Main.cpp \
	ProjectTests.cpp \
	CompileCommandsJSONTests.cpp \
	CommandOutputHandlerTests.cpp \
	../Paladin/objects*/paladin.a -o ./tests.o -Wall -lUnitTest++ -I../Paladin -I../Paladin/SourceControl -I../Paladin/BuildSystem -I../Paladin/ThirdParty -I../Paladin/PreviewFeatures -fprofile-arcs -ftest-coverage $LIBGCOV -lbe -llocalestub

echo "Done. Now execute ./tests.o"

