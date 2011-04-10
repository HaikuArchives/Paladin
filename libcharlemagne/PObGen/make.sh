#!/bin/sh
lua PObGen.lua PBoxGen.lua
lua PObGen.lua PButtonGen.lua
lua PObGen.lua PCheckBox.lua
lua PObGen.lua PClipboardGen.lua
lua PObGen.lua PColorControlGen.lua
lua PObGen.lua PLabelGen.lua
lua PObGen.lua PSliderGen.lua
lua PObGen.lua PTextControlGen.lua
lua PObGen.lua PTextViewGen.lua

mv -f *.cpp ..
mv -f *.h ..
