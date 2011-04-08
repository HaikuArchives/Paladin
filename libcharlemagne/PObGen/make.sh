#!/bin/sh
lua PObGen.lua PButtonGen.lua
lua PObGen.lua PSliderGen.lua
lua PObGen.lua PTextViewGen.lua
lua PObGen.lua PBoxGen.lua
lua PObGen.lua PClipboardGen.lua

mv -f *.cpp ..
mv -f *.h ..
