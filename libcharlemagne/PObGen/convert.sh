#!/bin/sh
lua ConvertToPObj.lua PBoxGen.lua > PBox.pobj
lua ConvertToPObj.lua PButtonGen.lua > PButton.pobj
lua ConvertToPObj.lua PCheckBoxGen.lua > PCheckBox.pobj
lua ConvertToPObj.lua PColorControlGen.lua > PColorControl.pobj
lua ConvertToPObj.lua PLabelGen.lua > PLabel.pobj
lua ConvertToPObj.lua PListViewGen.lua > PListView.pobj
lua ConvertToPObj.lua PRadioButtonGen.lua > PRadioButton.pobj
lua ConvertToPObj.lua PSliderGen.lua > PSlider.pobj
lua ConvertToPObj.lua PTextControlGen.lua > PTextControl.pobj
lua ConvertToPObj.lua PTextViewGen.lua > PTextView.pobj
