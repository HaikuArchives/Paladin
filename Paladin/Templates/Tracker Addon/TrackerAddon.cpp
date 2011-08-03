#include "TrackerAddon.h"

#include <Alert.h>
#include <String.h>

extern "C" {

void
process_refs(entry_ref dir_ref, BMessage *msg, void *reserved)
{
	BString out("Entries sent to Tracker addon:\n\n");
	
	entry_ref ref;
	int32 i = 0;
	while (msg->FindRef("refs", i++, &ref) == B_OK)
		out << ref.name << "\n";
	
	BAlert *alert = new BAlert("SampleAddon", out.String(), "OK");
	alert->Go();
}

}
