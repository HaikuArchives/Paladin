#!/bin/sh
echo "Assembling Paladin as a static library (You MUST compile it successfully first)"
cd ../Paladin/\(Objects.Paladin\)
ar rvs paladin.a *.o
echo "Compiling and linking tests against Paladin static library"
cd ../../Tests
g++ Main.cpp ProjectTests.cpp CompileCommandsJSONTests.cpp ../Paladin/\(Objects.Paladin\)/paladin.a -o ./tests.o -Wall -lUnitTest++ -I../Paladin -I../Paladin/SourceControl -I../Paladin/BuildSystem -I../Paladin/ThirdParty -fprofile-arcs -ftest-coverage -lbe -lstdc++ -llocalestub
echo "Done. Now execute ./tests.o"

