-- TODO: This needs to be finished once ListView is finished up
Module = MakeModule("POutlineListView")
Module.headerName = "POutlineListView.h"
Module.codeFileName = "POutlineListView.cpp"
Module.parentHeaderName = [["PView.h"]]
Module.includes = { "<Application.h>", "<OutlineListView.h>", "<stdio.h>", "<Window.h>" }

PObject = MakePObject("POutlineListView", "A list")
PObject.usesView = true
PObject.friendlyName = "OutlineList"
PObject.parentClass = "PListView"
PObject.parentAccess = "public"

------------------------------------------------------------------------------
-- PObject Code, Properties, and Methods

PObject.properties =
{
}

------------------------------------------------------------------------------
-- Backend definitions

PBackend = MakePBackend("POutlineListViewBackend")
PBackend.parent = "BOutlineListView"
PBackend.access = "public"
PBackend.init = [[BRect(0,0,1,1),"", B_SINGLE_SELECTION_LIST]]
PBackend:AssignPViewEvents()
