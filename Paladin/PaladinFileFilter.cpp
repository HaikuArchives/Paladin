/*
 * Copyright 2018 Adam Fowler <adamfowleruk@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 * 		Adam Fowler, adamfowleruk@gmail.com
 */
#include "PaladinFileFilter.h"

#include "Project.h"

const char * PaladinFileFilter::valid_filetypes[] = {
	"application/x-vnd.Be-directory",
	PROJECT_MIME_TYPE
};

PaladinFileFilter::PaladinFileFilter() 
{
}

PaladinFileFilter::~PaladinFileFilter()
{
}

bool
PaladinFileFilter::Filter(const entry_ref *ref, BNode *node, struct stat_beos *stat, const char *mimeType)
{
	for (int i = 0; valid_filetypes[i]; i++) {
		if (0 == strcmp(valid_filetypes[i],mimeType)) {
			return true;
		}
		BString name(ref->name);
		name.ToUpper();
		int32 l = name.FindLast('.');
		if (l != B_ERROR) {
			if (name.FindFirst(".PLD", l) != B_ERROR) return true;
		}
	}
	return false;
}

