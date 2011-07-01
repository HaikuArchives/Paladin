#!/bin/sh
lua PObGen.lua PBox.pobj
lua PObGen.lua PButton.pobj
lua PObGen.lua PCheckBox.pobj
lua PObGen.lua PClipboard.pobj
lua PObGen.lua PColorControl.pobj
lua PObGen.lua PLabel.pobj
lua PObGen.lua PListView.pobj
lua PObGen.lua PRadioButton.pobj
lua PObGen.lua PSlider.pobj
lua PObGen.lua PTextControl.pobj
lua PObGen.lua PTextView.pobj

mv -f *.cpp ..
mv -f *.h ..
