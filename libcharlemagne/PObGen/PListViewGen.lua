--[[
	C++ methods to (somehow) implement:
		AddItem
		AddStringItem
		RemoveItem
		ItemAt
		IndexOf
		InvalidateItem
		IsItemSelected
	
	Methods to implement:
		[Set|Get]SelectionMessage
		[Set|Get]InvocationMessage
		ScrollToSelection
		ScrollTo
		InitiateDrag
		MakeEmpty
		IsEmpty
	
	Additional Events:
		SelectionChanged
	
	Additional Properties:
		Selection (set, get)
]]

Module = MakeModule("PListView")
Module.headerName = "PListView.h"
Module.codeFileName = "PListView.cpp"
Module.parentHeaderName = [["PView.h"]]
Module.includes = { "<Application.h>", "<ListView.h>", "<stdio.h>", "<Window.h>" }

PObject = MakePObject("PListView", "A list")
PObject.usesView = true
PObject.friendlyName = "List"
PObject.parentClass = "PView"
PObject.parentAccess = "public"

------------------------------------------------------------------------------
-- PObject Code, Properties, and Methods

PObject.properties =
{
	{ "ItemCount", "int" , { "CountItems", "void" }, { },
		"The number of items in the list. Read-only.", "0" },
	{ "SelectionType", "enum" , { "ListType", "void" }, { "SetListType", "(list_view_type)" },
		"The list's selection mode", "B_SINGLE_SELECTION_LIST",
		{ pair("Single", "B_SINGLE_SELECTION_LIST"),
			pair("Multiple", "B_MULTIPLE_SELECTION_LIST") } },
	{ "PreferredHeight", "float",  { "GetPreferredSize", "embedded" }, { }, "", "0" },
	{ "PreferredWidth", "float",  { "GetPreferredSize", "embedded" }, { }, "", "0" }
}

PObject:SetEmbeddedProperty("PreferredHeight",
[[
		if (backend->CountItems() == 0)
			((FloatProperty*)prop)->SetValue(30);
		else
		{
			float pw, ph;
			backend->GetPreferredSize(&pw, &ph);
			if (pw < 10)
				pw = 100;
			if (ph < 10)
				ph = 30;
			((FloatProperty*)prop)->SetValue(ph);
		}
]],
nil)

PObject:SetEmbeddedProperty("PreferredWidth",
[[
		if (backend->CountItems() == 0)
			((FloatProperty*)prop)->SetValue(100);
		else
		{
			float pw, ph;
			backend->GetPreferredSize(&pw, &ph);
			if (pw < 10)
				pw = 100;
			if (ph < 10)
				ph = 30;
			((FloatProperty*)prop)->SetValue(pw);
		}
]],
nil)

------------------------------------------------------------------------------
-- Backend definitions

PBackend = MakePBackend("PListViewBackend")
PBackend.parent = "BListView"
PBackend.access = "public"
PBackend.init = [[BRect(0,0,1,1),"", B_SINGLE_SELECTION_LIST]]
PBackend:AssignPViewEvents()
