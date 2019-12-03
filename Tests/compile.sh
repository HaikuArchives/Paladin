#!/bin/sh
echo "Assembling Paladin as a static library (You MUST compile it successfully first)"
cd ../Paladin/\(Objects.Paladin\)
ar rvs paladin.a *.o
echo "Compiling and linking tests against Paladin static library"
cd ../../Tests
g++ *.cpp ../Paladin/\(Objects.Paladin\)/paladin.a -o \(Objects.PaladinTests\)/tests.o -Wall -lUnitTest++ -I../Paladin -I../Paladin/SourceControl -I../Paladin/BuildSystem -I../Paladin/ThirdParty -lbe -lstdc++ -llocalestub
echo "Done. Now execute ./\(Objects.PaladinTests\)/tests.o"

