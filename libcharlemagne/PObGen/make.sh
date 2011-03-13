#!/bin/sh
lua PObGen.lua PButtonGen.lua
lua PObGen.lua PSliderGen.lua
lua PObGen.lua PTextViewGen.lua

mv -f *.cpp ..
mv -f *.h ..
