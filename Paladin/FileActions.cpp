#include "FileActions.h"

#include <Entry.h>
#include <Message.h>
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
	
	BMessage msg(B_REFS_RECEIVED);
	msg.AddRef("refs",&ref);
	
	if (gPlatform == PLATFORM_R5 || gPlatform == PLATFORM_ZETA)
		be_roster->Launch("application/x-vnd.Be-MIMA",&msg);
	else
		be_roster->Launch("application/x-vnd.Haiku-FileTypes",&msg);
}

