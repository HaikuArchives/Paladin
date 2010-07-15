/*
 * Convert Tabs Add-on for Pe editor
 *
 * Copyright (C) 2008 by François Revol, <revol@free.fr>
 *
 * Derived from:
 * Convert Tabs Add-on for BeIDE's editor
 *
 * Copyright (C) 2008 by Anthony Lee <don.anthony.lee@gmail.com>
 * Distributed under the terms of the MIT License.
 */

#include <be/kernel/image.h>
#include <be/support/String.h>
#include <be/storage/Path.h>
#include <be/storage/FindDirectory.h>
#include <be/storage/File.h>
#include <be/interface/Alert.h>
#include <be/interface/Window.h>

#include "PeAddOn.h"

static bool sIsReplaceAddon = false;


// # this doesn't work in the Jamfile...
// (both object names are the same)
// ObjectC++Flags Replace\ As\ Tabs : -DEXPAND_TABS=1 ;
// so we check at run time.
static void DetermineAddon(void)
{
	int32 cookie = 0;
	image_info info;

	// determine which addon we are
	while (get_next_image_info(0, &cookie, &info) == B_OK) {
		if ((char *)&DetermineAddon >= (char *)info.text && 
			(char *)&DetermineAddon < ((char *)info.text + info.text_size)) {
			if (strstr(info.name, "Replace As Tabs"))
				sIsReplaceAddon = true;
			break;
		}
	}
}


static int32 GetTabSize(MTextAddOn *addon)
{
	char *p = NULL;
	int32 tabstop = 4;

	// note it breaks if you changed the tabstop but didn't save the file again yet...
	try {
		attr_info ai;
		entry_ref ref;
		if (addon->GetRef(ref) >= B_OK) {
			BFile file(&ref, B_READ_ONLY);
			if (file.GetAttrInfo("pe-info", &ai) == B_NO_ERROR)
			{
				p = (char *)malloc(ai.size);
				FailNil(p);

				FailIOErr(file.ReadAttr("pe-info", ai.type, 0, p, ai.size));

				BMessage settings;
				settings.Unflatten(p);
				if (settings.FindInt32("tabstop", &tabstop) < B_OK)
					tabstop = 4;
			}
		}
	}
	catch (HErr& e) {}
	free(p);
	return tabstop;
}


static void ConvertTabs(MTextAddOn *addon, BString &str)
{
	int32 tab_size = GetTabSize(addon);
	int32 found;

	for(int32 pos = 0; pos < str.Length(); pos = found + 1)
	{
		found = str.FindFirst('\n', pos);
		if(found < 0) found = str.Length();

		BString line;
		line.SetTo(str.String() + pos, found - pos);

//#ifdef EXPAND_TABS
		if (!sIsReplaceAddon) {
			int32 index;
			while((index = line.FindFirst('\t')) >= 0)
			{
				int32 width = 0;
				for(int32 k = 0; k < index; k++)
				{
					// FIXME: special multi-bytes character
					uint8 c = *((uint8*)(line.String() + k));
					if(c < 0x80) width += 1;
					else if(c < 0xc0 || c >= 0xfe) continue;
					else if(c < 0xe0) { width += 2; k += 1; }
					else if(c < 0xf0) { width += 2; k += 2; }
					else if(c < 0xf8) { width += 2; k += 3; }
					else if(c < 0xfc) { width += 2; k += 4; }
					else if(c < 0xfe) { width += 2; k += 5; }
				}

				line.Remove(index, 1);
				line.Insert(' ', tab_size - width % tab_size, index);
			}
		} else {
//#else // REPLACE_AS_TABS
			int32 count = 0;
			for(int32 k = 0; k < line.Length(); k++)
			{
				if(line[k] == '\t') continue;
				if(line[k] != ' ') break;
				if(++count < tab_size) continue;
				count = 0;
				line.Remove(k - tab_size + 1, tab_size);
				k -= tab_size - 1;
				line.Insert('\t', 1, k);
			}
		}
//#endif

		str.Remove(pos, found - pos);
		str.Insert(line, pos);
		found = pos + line.Length();
	}
}


static void ShowErrors(const char *msg)
{
	BAlert *alert = new BAlert("Convert Tabs", msg, "Quit", NULL, NULL,
							   B_WIDTH_AS_USUAL, B_STOP_ALERT);
	alert->Lock();
	alert->SetLook(B_TITLED_WINDOW_LOOK);
	alert->SetFeel(B_NORMAL_WINDOW_FEEL);
	alert->Unlock();
	alert->Go((BInvoker*)NULL);
}


extern "C" _EXPORT status_t perform_edit(MTextAddOn *addon)
{
	DetermineAddon();
	
	//if(addon->IsEditable() == false) return B_ERROR;

	int32 startIDX = 0, endIDX = 0;
	addon->GetSelection(&startIDX, &endIDX);
	if(endIDX <= startIDX)
	{
		ShowErrors("You need to select some text first.");
		return B_ERROR;
	}

	BString str;
	str.SetTo(addon->Text() + startIDX, endIDX - startIDX);
	if(str.Length() != endIDX - startIDX)
	{
		ShowErrors("Unknown error!!!");
		return B_ERROR;
	}

	ConvertTabs(addon, str);

	if(str.Compare(addon->Text() + startIDX, endIDX - startIDX) != 0)
	{
		addon->Delete();
		addon->Insert(str.String());
		addon->Select(startIDX, startIDX + str.Length());
	}

	return B_NO_ERROR;
}
