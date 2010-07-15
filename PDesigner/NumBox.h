#ifndef NUMBOX_H
#define NUMBOX_H

#include "AutoTextControl.h"

class NumBox : public AutoTextControl
{
public:
	NumBox(const BRect &frame, const char *name, const char *label,
			const char *text, BMessage *msg,
			uint32 resize = B_FOLLOW_LEFT | B_FOLLOW_TOP,
			uint32 flags = B_WILL_DRAW | B_NAVIGABLE);
	bool Validate(bool alert = true);
	
	void AllowNegatives(const bool &value);
	bool AllowsNegatives(void) const { return fAllowNegatives; }
private:
	friend class NumBoxFilter;
	bool fFilterTab;
	bool fAllowNegatives;
};

#endif
