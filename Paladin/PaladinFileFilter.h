/*
 * Copyright 2018 Adam Fowler <adamfowleruk@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 * 		Adam Fowler, adamfowleruk@gmail.com
 */
#ifndef PALADINFILEFILTER_H
#define PALADINFILEFILTER_H

#include <FilePanel.h>
#include <Entry.h>

class PaladinFileFilter : public BRefFilter
{
public:
		PaladinFileFilter(void);
		~PaladinFileFilter(void);
	bool	Filter(const entry_ref *ref, BNode *node, 
			struct stat_beos *stat, const char *mimeType);
private:
	static const char *	valid_filetypes[3];
};

#endif
