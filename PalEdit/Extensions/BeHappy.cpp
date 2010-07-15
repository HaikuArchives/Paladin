//
// BeHappy: looks up the selected text against BeBook inside BeHappy.
//
// Based from BeBookFetch by Oscar Lesta.
//
// new bugs by François Revol.
//

#include "PeAddOn.h"

#define kBeHappySig		"application/x-vnd.STertois.BeHappy"
#define kBeHappySigAlt	"application/x.vnd-STertois.BeHappy"

static team_id gBeHappyTeam = -1;

//------------------------------------------------------------------------------

status_t
TellBeHappy(const char *topic)
{
	status_t err;
	int32 lastWindow = 0;
	
	BMessenger msgr(NULL, gBeHappyTeam);
	// if we didn't start it yet or it's gone...
	if (gBeHappyTeam < 0 || !msgr.IsValid()) {
		// start it
		err = be_roster->Launch(kBeHappySig, (BMessage *)NULL, &gBeHappyTeam);
		if (err < B_OK) {
			err = be_roster->Launch(kBeHappySigAlt, (BMessage *)NULL, &gBeHappyTeam);
			if (err < B_OK)
				return B_LAUNCH_FAILED;
		}
		// leave it some time to start
		for(int i=0; i<10; ++i) {
			snooze(100000);
			msgr = BMessenger(NULL, gBeHappyTeam);
			if (msgr.IsValid())
				break;
		}
	}
	
	// XXX: lastWindow = (hey count Window) -1
	
	// set which book to use
	BMessage m1(B_SET_PROPERTY);
	m1.AddSpecifier("Book");
	m1.AddString("data", "Be Book");
	m1.AddSpecifier("Window", lastWindow);
	err = msgr.SendMessage(&m1);
	if (err < B_OK)
		return err;

	// some more time to load the book...
	snooze(500000);

	// send the search string
	BMessage m2(B_SET_PROPERTY);
	m2.AddSpecifier("Topic");
	m2.AddString("data", topic);
	m2.AddSpecifier("Window", lastWindow);
	BMessage reply;
	err = msgr.SendMessage(&m2, &reply, 0);
	if (err < B_OK)
		return err;

	// see if the text was found
	if (reply.FindInt32("error", &err) < B_OK)
		return EINVAL;
	if (err < B_OK)
		return err;
	
	return err;
}


//------------------------------------------------------------------------------
//	#pragma mark - Addon Entry

#if __INTEL__
	extern "C" _EXPORT long perform_edit(MTextAddOn* addon);
#else
	#pragma export on
	extern "C" {
		long perform_edit(MTextAddOn* addon);
	}
	#pragma export reset
#endif


long
perform_edit(MTextAddOn* addon)
{
	status_t err;
	long selStart;
	long selEnd;

	addon->GetSelection(&selStart, &selEnd);

	if (selEnd <= selStart)
		return B_ERROR;

	int length = selEnd - selStart;

	// Do NOT run the query if we have less than 3 chars. It works, but takes
	// ages and produces an useless giant popup menu.
	if (length < 3) {
		MIdeaAlert("The text selection is too short.\n"
					"Please select at least three characters.").Go();
		return B_ERROR;
	}

	BString selection;
	selection.SetTo(addon->Text() + selStart, length);

    // Limit the size of the search string to 30 chars, and make sure it is
    // worth of searching for (name of: function/variable/error code, etc.).
	int pos = 0;
	while ((pos < length) || pos < 30) {
		if (!isalnum(selection[pos]) && (selection[pos] != '_')) break;
		pos++;
	}

	if (!pos) return B_ERROR;

	selection.Truncate(pos);

	if (selection.CountChars() < 3) {
		MIdeaAlert("The text selection is too short.\n"
					"Make sure that the selection doesn't contain punctuation "
					"marks or other symbols.").Go();
		return B_ERROR;
	}

	err = TellBeHappy(selection.String());
	if (err >= B_OK)
		return B_OK;

	// No results... Lets try the same selection as a function name...
	BString funcname(selection.String());
	funcname += "()";
	err = TellBeHappy(funcname.String());
	if (err >= B_OK)
		return B_OK;

	// Still nothing? Lets Slap a B on the front and check for Classes!
	BString section("B");
	section += selection.String();
	err = TellBeHappy(section.String());
	if (err >= B_OK)
		return B_OK;

	if (err == B_LAUNCH_FAILED) {
		MStopAlert("Can't launch BeHappy!").Go();
		return B_OK;
	}
	
	BString message("Nothing was found by BeHappy for: ");
	message += selection.String();
	MInfoAlert(message.String()).Go();

	return B_OK;
}
