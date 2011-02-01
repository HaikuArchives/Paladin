#include "FileActions.h"

#include <Entry.h>
#include <Message.h>
#include <Node.h>
#include <NodeInfo.h>
#include <Roster.h>

#include "DPath.h"
#include "Globals.h"
#include "Project.h"

void
SpawnFileTypes(DPath path)
{
	BEntry entry(path.GetFullPath());
	entry_ref ref;
	entry.GetRef(&ref);
	
	if (path.GetExtension() && strcmp("rsrc", path.GetExtension()) == 0)
	{
		// Work around bad resource file sniffing
		BNode node(&ref);
		BNodeInfo nodeInfo(&node);
		
		char type[B_MIME_TYPE_LENGTH];
		nodeInfo.GetType(type);
		if (type[0] != '\0' && strcmp(type, "application/octet-stream") == 0)
			nodeInfo.SetType("application/x-be-resource");
	}
	
	BMessage msg(B_REFS_RECEIVED);
	msg.AddRef("refs",&ref);
	
	if (gPlatform == PLATFORM_R5 || gPlatform == PLATFORM_ZETA)
		be_roster->Launch("application/x-vnd.Be-MIMA",&msg);
	else
		be_roster->Launch("application/x-vnd.Haiku-FileTypes",&msg);
}

