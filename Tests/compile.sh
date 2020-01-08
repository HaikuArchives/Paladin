#!/bin/sh
echo "Assembling Paladin as a static library (You MUST compile it successfully first)"
cd ../Paladin/objects*
ar rvs paladin.a *.o
echo "Compiling and linking tests against Paladin static library"
cd ../../Tests
g++ 	Main.cpp \
	ProjectTests.cpp \
	CompileCommandsJSONTests.cpp \
	CommandOutputHandlerTests.cpp \
	../Paladin/objects*/paladin.a -o ./tests.o -Wall -lUnitTest++ -I../Paladin -I../Paladin/SourceControl -I../Paladin/BuildSystem -I../Paladin/ThirdParty -I../Paladin/PreviewFeatures -fprofile-arcs -ftest-coverage -lgcov -lbe -llocalestub
echo "Done. Now execute ./tests.o"

