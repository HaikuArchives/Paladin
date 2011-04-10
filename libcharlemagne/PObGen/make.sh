#!/bin/sh
lua PObGen.lua PBoxGen.lua
lua PObGen.lua PButtonGen.lua
lua PObGen.lua PClipboardGen.lua
lua PObGen.lua PSliderGen.lua
lua PObGen.lua PTextControlGen.lua
lua PObGen.lua PTextViewGen.lua

mv -f *.cpp ..
mv -f *.h ..
