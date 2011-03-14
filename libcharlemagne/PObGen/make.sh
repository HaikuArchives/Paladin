#!/bin/sh
lua PObGen.lua PButtonGen.lua
lua PObGen.lua PSliderGen.lua
lua PObGen.lua PTextViewGen.lua
lua PObGen.lua PBoxGen.lua

mv -f *.cpp ..
mv -f *.h ..
