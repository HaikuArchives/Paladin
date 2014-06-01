/*
	AutoTextControl.cpp: A BTextControl which notifies on each keypress
	Written by DarkWyrm <darkwyrm@earthlink.net>, Copyright 2007
	Released under the MIT license.
*/


#include "AutoTextControl.h"
#include <Window.h>
#include <String.h>
#include <stdio.h>
#include <ctype.h>
#include <PropertyInfo.h>


static property_info sProperties[] = {
	{
		"CharacterLimit",
		{ B_GET_PROPERTY, 0 },
		{ B_DIRECT_SPECIFIER, 0 },
		"Returns the maximum number of characters that the AutoTextControl "
		"will allow.",
		0,
		{ B_INT32_TYPE }
	},

	{
		"CharacterLimit",
		{ B_SET_PROPERTY, 0 },
		{ B_DIRECT_SPECIFIER, 0},
		"Sets the maximum number of characters that the AutoTextControl "
		"will allow.",
		0,
		{ B_INT32_TYPE }
	},
};


AutoTextControl::AutoTextControl(const BRect& frame, const char* name,
	const char* label, const char* text, BMessage* message,
	uint32 resizingMode, uint32 flags)
	:
	BTextControl(frame, name, label, text, message, resizingMode, flags),
 	fFilter(NULL),
 	fCharLimit(0)
{
	SetFilter(new AutoTextControlFilter(this));
}


AutoTextControl::AutoTextControl(const char* name, const char* label,
	const char* text, BMessage* message, uint32 flags)
	:
	BTextControl(name, label, text, message, flags),
 	fFilter(NULL),
 	fCharLimit(0)
{
	SetFilter(new AutoTextControlFilter(this));
}


AutoTextControl::~AutoTextControl(void)
{
	if (Window() != NULL)
		Window()->RemoveCommonFilter(fFilter);

	delete fFilter;
}


AutoTextControl::AutoTextControl(BMessage *data)
	:
	BTextControl(data)
{
	if (data->FindInt32("_charlimit",(int32*)&fCharLimit) != B_OK)
		fCharLimit = 0;
}


BArchivable*
AutoTextControl::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "AutoTextControl"))
		return new AutoTextControl(data);

	return NULL;
}


status_t
AutoTextControl::Archive(BMessage *data, bool deep) const
{
	status_t status = BTextControl::Archive(data,deep);

	if (status == B_OK)
		status = data->AddInt32("_charlimit",fCharLimit);

	if (status == B_OK)
		status = data->AddString("class","AutoTextControl");

	return status;
}


status_t
AutoTextControl::GetSupportedSuites(BMessage *message)
{
	message->AddString("suites","suite/vnd.DW-autotextcontrol");

	BPropertyInfo prop_info(sProperties);
	message->AddFlat("messages",&prop_info);
	return BTextControl::GetSupportedSuites(message);
}


BHandler*
AutoTextControl::ResolveSpecifier(BMessage* message, int32 index,
	BMessage* specifier, int32 form, const char *property)
{
	return BControl::ResolveSpecifier(message, index, specifier,
		form, property);
}


void
AutoTextControl::AttachedToWindow(void)
{
	BTextControl::AttachedToWindow();

	if (fFilter != NULL)
		Window()->AddCommonFilter(fFilter);
}


void
AutoTextControl::DetachedFromWindow(void)
{
	if (fFilter != NULL)
		Window()->RemoveCommonFilter(fFilter);

	BTextControl::DetachedFromWindow();
}


void
AutoTextControl::SetCharacterLimit(const uint32& limit)
{
	fCharLimit = limit;
}


uint32
AutoTextControl::GetCharacterLimit(const uint32& limit)
{
	return fCharLimit;
}


void
AutoTextControl::AllowCharacters(const char* string)
{
	uint32 i = 0;
	while (string[i])
		TextView()->AllowChar(string[i++]);
}


void
AutoTextControl::DisallowCharacters(const char* string)
{
	uint32 i = 0;
	while (string[i])
		TextView()->DisallowChar(string[i++]);
}


void
AutoTextControl::SetFilter(AutoTextControlFilter* filter)
{
	if (fFilter != NULL) {
		if (Window() != NULL)
			Window()->RemoveCommonFilter(fFilter);

		delete fFilter;
	}
	
	fFilter = filter;
	if (Window() != NULL)
		Window()->AddCommonFilter(fFilter);
}


AutoTextControlFilter::AutoTextControlFilter(AutoTextControl* box)
	:
	BMessageFilter(B_PROGRAMMED_DELIVERY, B_ANY_SOURCE, B_KEY_DOWN),
 	fBox(box),
 	fCurrentMessage(NULL)
{
}


AutoTextControlFilter::~AutoTextControlFilter(void)
{
}


filter_result
AutoTextControlFilter::Filter(BMessage* message, BHandler **target)
{
	int32 rawchar,mod;
	message->FindInt32("raw_char",&rawchar);
	message->FindInt32("modifiers",&mod);
	
	BView* view = dynamic_cast<BView*>(*target);
	if (view == NULL || view->Name() == NULL
		|| strcmp("_input_", view->Name()) != 0) {
		return B_DISPATCH_MESSAGE;
	}

	AutoTextControl* text = dynamic_cast<AutoTextControl*>(view->Parent());
	if (text == NULL || text != fBox)
		return B_DISPATCH_MESSAGE;
	
	fCurrentMessage = message;
	filter_result result = KeyFilter(rawchar,mod);
	fCurrentMessage = NULL;
	
	if (fBox->fCharLimit && result == B_DISPATCH_MESSAGE) {
		// See to it that we still allow shortcut keys
		if (mod & B_COMMAND_KEY)
			return B_DISPATCH_MESSAGE;
		
		// We don't use strlen() because it is not UTF-8 aware, which can affect
		// how many characters can be typed.
		if (isprint(rawchar)
			&& (uint32)BString(text->Text()).CountChars() == text->fCharLimit) {
			return B_SKIP_MESSAGE;
		}
	}

	return result;
}


filter_result
AutoTextControlFilter::KeyFilter(const int32& rawchar, const int32& mod)
{
	if (fBox != NULL)
		fBox->Invoke();

	return B_DISPATCH_MESSAGE;
}
