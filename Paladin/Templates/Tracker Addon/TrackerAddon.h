#ifndef TRACKER_ADDON_H
#define TRACKER_ADDON_H

#include <Entry.h>
#include <Message.h>
#include <TrackerAddOn.h>

extern "C"
{
	void	process_refs(entry_ref dir_ref, BMessage *msg, void *data);
}

#endif
