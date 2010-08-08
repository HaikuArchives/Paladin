#!/bin/sh
cd `dirname $0`
cat PInstallEngine testdata/calc/files.zip > PInstallEngineTest.sfx
chmod +x PInstallEngineTest
