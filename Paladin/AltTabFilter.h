#ifndef ALT_TAB_FILTER_H
#define ALT_TAB_FILTER_H

#include <MessageFilter.h>
#include <Handler.h>

#include "MsgDefs.h"

class AltTabFilter : public BMessageFilter
{
public:
					AltTabFilter(void)
						: BMessageFilter(B_PROGRAMMED_DELIVERY,
										B_ANY_SOURCE,B_KEY_DOWN)
					{
					}
					
					~AltTabFilter(void)
					{
					}
					
	filter_result	Filter(BMessage *msg, BHandler **target)
	{
		int32 rawchar,mod;
		msg->FindInt32("raw_char",&rawchar);
		msg->FindInt32("modifiers",&mod);
		
		if (rawchar == B_TAB && (mod & B_COMMAND_KEY)) {
			BLooper *loop = (*target)->Looper();
			if (loop) {
				BMessenger msgr(loop);
				msgr.SendMessage(M_OPEN_PARTNER);
				return B_SKIP_MESSAGE;
			}
		}
		return B_DISPATCH_MESSAGE;
	}

};


#endif

