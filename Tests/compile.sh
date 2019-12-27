#!/bin/sh
echo "Assembling Paladin as a static library (You MUST compile it successfully first)"
cd ../Paladin/objects.x86_64-cc8-release
ar rvs paladin.a *.o
echo "Compiling and linking tests against Paladin static library"
cd ../../Tests
g++ Main.cpp ProjectTests.cpp CompileCommandsJSONTests.cpp ../Paladin/objects.x86_64-cc8-release/paladin.a -o ./tests.o -Wall -lUnitTest++ -I../Paladin -I../Paladin/SourceControl -I../Paladin/BuildSystem -I../Paladin/ThirdParty -I../Paladin/PreviewFeatures -fprofile-arcs -ftest-coverage -lbe -lstdc++ -llocalestub
echo "Done. Now execute ./tests.o"

