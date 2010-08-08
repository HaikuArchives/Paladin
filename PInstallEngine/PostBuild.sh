#!/bin/sh
cd `dirname $0`
cp PInstallEngine PInstallEngineTest.sfx
rc PInstallEngineTest.rdef -o PInstallEngineTest.rsrc
xres -o PInstallEngineTest.sfx PInstallEngineTest.rsrc
rm PInstallEngineTest.rsrc
cat testdata/calc/files.zip >> PInstallEngineTest.sfx
chmod +x PInstallEngineTest.sfx
