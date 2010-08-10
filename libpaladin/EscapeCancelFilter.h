/*
	EscapeCancelFilter.h: An easy way to quit with the Escape key
	Written by DarkWyrm <darkwyrm@earthlink.net>, Copyright 2007
	Released under the MIT license.
*/
#ifndef ESCAPE_CANCEL_FILTER_H
#define ESCAPE_CANCEL_FILTER_H

/*
	This filter is most often useful in dialog windows where you would
	like to allow the user to effectively hit the Cancel button just by
	hitting the Escape key. Pass one of these to BWindow::AddCommonFilter
	and that is all that is necessary.
*/

#include <MessageFilter.h>
#include <Handler.h>

class EscapeCancelFilter : public BMessageFilter
{
public:
					EscapeCancelFilter(void)
						: BMessageFilter(B_PROGRAMMED_DELIVERY,
										B_ANY_SOURCE,B_KEY_DOWN)
					{
					}
					
					~EscapeCancelFilter(void)
					{
					}
					
	filter_result	Filter(BMessage *msg, BHandler **target)
	{
		int32 rawchar,mod;
		msg->FindInt32("raw_char",&rawchar);
		msg->FindInt32("modifiers",&mod);
		
		if (rawchar == B_ESCAPE && (mod & (B_SHIFT_KEY | B_COMMAND_KEY | 
				B_OPTION_KEY | B_CONTROL_KEY)) == 0) {
			BLooper *loop = (*target)->Looper();
			if (loop) {
				BMessenger msgr(loop);
				msgr.SendMessage(B_QUIT_REQUESTED);
				return B_SKIP_MESSAGE;
			}
		}
		return B_DISPATCH_MESSAGE;
	}

};


#endif

