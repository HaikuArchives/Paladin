/*
 * Copyright 1996, 1997, 1998, 2002 Hekkelman Programmatuur B.V.  All rights reserved.
 * Copyright 2018 Adam Fowler <adamfowleruk@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Adam Fowler, adamfowleruk@gmail.com
 */
#ifndef PROJECTSTATUS_H
#define PROJECTSTATUS_H

#include <StringView.h>

class ProjectStatus : public BView
{
public:
							ProjectStatus(BRect frame, const char* msg);
							~ProjectStatus();

	virtual	void 			Draw(BRect updateRect);

			void 			SetOffset(int newOffset);
			
			void			SetStatus(const char* msg);
	
private:
			const char*	fMsg;
			int 			fOffset;
			float 			fBaseline;
};
			

#endif
