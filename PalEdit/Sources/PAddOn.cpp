/*	Copyright 1996, 1997, 1998, 2002
	        Hekkelman Programmatuur B.V.  All rights reserved.
	
	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:
	1. Redistributions of source code must retain the above copyright notice,
	   this list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright notice,
	   this list of conditions and the following disclaimer in the documentation
	   and/or other materials provided with the distribution.
	3. All advertising materials mentioning features or use of this software
	   must display the following acknowledgement:
	   
	    This product includes software developed by Hekkelman Programmatuur B.V.
	
	4. The name of Hekkelman Programmatuur B.V. may not be used to endorse or
	   promote products derived from this software without specific prior
	   written permission.
	
	THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
	FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
	AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
	PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
	OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
	WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 	
*/


#include "PAddOn.h"

#include "HError.h"
#include "PCmd.h"
#include "PText.h"

#include <String.h>

#include <stdlib.h>


PAddOn::PAddOn(const char *name)
	: BHandler(name),
	fDocument(NULL),
	fText(NULL)
{
}


PAddOn::~PAddOn()
{
}


//	#pragma mark - Hooks


void PAddOn::MessageReceived(BMessage* message)
{
	// dispatch language changed messages to the LanguageChanged() hook
	BHandler::MessageReceived(message);
}


void PAddOn::AttachedToDocument()
{
	// to be implemented by subclasses
}


void PAddOn::DetachedFromDocument()
{
	// to be implemented by subclasses
}


void PAddOn::LanguageChanged(BMessage& message)
{
	// to be implemented by subclasses
}


//	#pragma mark - Buttons & Menus


void* PAddOn::AddButton(BBitmap& bitmap, const char* toolTip, uint32 kind,
						BMessage* message)
{
	return NULL;
}


void* PAddOn::AddMenuItem(const char* label, uint32 kind, BMessage* message)
{
	return NULL;
}


void* PAddOn::AddMenuItem(BMenuItem* item)
{
	return NULL;
}


void PAddOn::RemoveButton(void* handle)
{
}


void RemoveMenuItem(void* handle)
{
}


//	#pragma mark - Text & Language Support


status_t PAddOn::Perform(BMessage& message)
{
	return B_ERROR;
}


status_t PAddOn::LanguagePerform(BMessage& message)
{
	return B_ERROR;
}


bool PAddOn::LanguageSupports(uint32 code)
{
	return false;
}


//	#pragma mark - private functions


void PAddOn::SetContext(PDoc* document, PText* text)
{
	fDocument = document;
	fText = text;
}


PText& PAddOn::Text()
{
	return *fText;
}


//	#pragma mark - text manipulation


PEditText::PEditText(PAddOn* addOn, const char *action)
	:
	fText(addOn->Text()),
	fIsDirty(false)
{
	fCommand = new PExtCmd(&fText, action);
}


PEditText::~PEditText()
{
	fText.RedrawDirtyLines();
	if (fIsDirty)
		fText.RegisterCommand(fCommand);
}


const char* PEditText::Text()
{
	return fText.Text();
}


int32 PEditText::TextLength() const
{
	return fText.Size();
}


void PEditText::GetSelection(int32* _start, int32* _end) const
{
	int32 start = fText.Anchor(), end = fText.Caret();
	*_start = std::min(start, end);
	*_end = std::max(start, end);
}


void PEditText::Select(int32 newStart, int32 newEnd)
{
	fText.Select(newStart, newEnd, true, false);
}


void PEditText::SelectCurrentWord()
{
	// TODO!
}


void PEditText::Delete()
{
	int32 start, end;
	GetSelection(&start, &end);
	
	ExtAction action;
	action.aType = eaDelete;
	action.aOffset = start;
	action.aText = (char *)malloc(end - start + 1);
	FailNil(action.aText);
	fText.TextBuffer().Copy(action.aText, start, end - start);
	action.aText[end - start] = 0;
	fCommand->Actions().push_back(action);
	
	fText.Delete(start, end);
	fText.SetCaret(start);
	
	fIsDirty = true;
}


void PEditText::Insert(const char* text)
{
	Insert(text, strlen(text));
}


void PEditText::Insert(const char* text, int32 length)
{
	int32 start, end;
	GetSelection(&start, &end);

	if (start != end)
		Delete();

	ExtAction action;
	action.aType = eaInsert;
	action.aOffset = start;
	action.aText = (char *)malloc(length + 1);
	FailNil(action.aText);
	memcpy(action.aText, text, length);
	action.aText[length] = 0;
	fCommand->Actions().push_back(action);

	fText.Insert(text, length, start);

	fText.Select(start + length, start + length, true, false);
	fIsDirty = true;
}


void PEditText::Insert(BString& string)
{
	Insert(string.String(), string.Length());
}

