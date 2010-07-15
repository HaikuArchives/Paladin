//
// BeBookFetch: looks up the selected text against BeBook's doc bookmarks.
//
// Contains (legally) stolen code from:
//  - Alan Ellis's Be API Fetch for Eddie (http://www.bebits.com/app/6).
//
// Pe version and new bugs by Oscar Lesta.
//
// ToDo: improve the popup display by reading doc_bookmark attributes to avoid
//       duplicates items (append, for example, "be:kit_name" attr content).
//

#include "PeAddOn.h"

#include <vector>

#define kTrackerSig		"application/x-vnd.Be-TRAK"

using std::vector;

//------------------------------------------------------------------------------

uint32
FetchQuery(query_op op, const char* selection, vector<BString>& results,
			bool caseSensitive = false)
{
	BQuery query;

	query.PushAttr("BEOS:TYPE");
	query.PushString("application/x-vnd.Be-doc_bookmark");
	query.PushOp(B_EQ);
	query.PushAttr("name");
	query.PushString(selection, caseSensitive);
	query.PushOp(op);
	query.PushOp(B_AND);

	BVolume vol;
	BVolumeRoster roster;

	roster.GetBootVolume(&vol);
	query.SetVolume(&vol);

	if (B_NO_INIT == query.Fetch())
		return 0;

	BEntry entry;
	BPath path;
	int32 counter = 0;

	while (query.GetNextEntry(&entry) != B_ENTRY_NOT_FOUND) {
		if (entry.InitCheck() == B_OK) {
			entry.GetPath(&path);

			if (path.InitCheck() == B_OK) {
				results.push_back(path.Path());
				counter++;
			}
		}
	}

	return counter;
}


void
OpenDoc(const char* fileName)
{
// Note:
// I don't just call be_roster->Launch() here because:
// if you happen to have another Tracker exec somewhere on your boot volume...
// it will get executed instead of just calling the already-running-Tracker's
// ArgRecv method. And that's bad, because you get two Tracker running, and it
// takes too long to get the results :-)

	BMessage msg(B_ARGV_RECEIVED);
	msg.AddString("cwd", "/");
	msg.AddString("argv", "Tracker");
	msg.AddString("argv", fileName);
	msg.AddInt32("argc", 2);

	BMessenger messenger(kTrackerSig);
	if (messenger.IsValid())
		messenger.SendMessage(&msg);
	else
		be_roster->Launch(kTrackerSig, 1, const_cast<char**>(&fileName));
}


BString
FormatResultString(const BString& resultPath)
{
	BString result;

	int32 end = resultPath.FindFirst("CodeName") - 2;
	int32 start = resultPath.FindLast('/', end) + 1;

	if (end - start > 1) {
		resultPath.CopyInto(result, start, end - start + 1);
		result += "::";
		result += &resultPath.String()[resultPath.FindLast('/',
					resultPath.Length()) + 1];
	} else {
		int32 secbegin = resultPath.FindLast('/', resultPath.Length()) + 1;
		resultPath.CopyInto(result, secbegin, resultPath.Length() - secbegin);
	}

	return result;
}


void
PopUpResults(vector<BString>& results, MTextAddOn* addon)
{
	BPopUpMenu selections("API Selections", false, false);
	selections.SetFont(be_plain_font);

	for (uint32 i = 0; i < results.size(); i++) {
		BMenuItem* item = new BMenuItem(FormatResultString(results[i]).String(),
										new BMessage(i));
		selections.AddItem(item);
	}

	// Here we get a nice BPoint to popup the results (end of the toolbar).
	BPoint menupos;

	if (addon->Window()->Lock()) {
		float x = addon->Window()->FindView("ButtonBar")->Frame().right + 2;
		float y = addon->Window()->FindView("ToolBar")->Frame().Height() - 2;

		menupos = addon->Window()->Frame().LeftTop() + BPoint(x, y);
		addon->Window()->Unlock();
	}

	BMenuItem* item = selections.Go(menupos, false, true);

	if (item != NULL) {
		int index = item->Command();
		OpenDoc(results[index].String());
	}
}


void
DisplayResults(vector<BString>& results, MTextAddOn* addon)
{
	if (results.size() < 2)
		OpenDoc(results[0].String());
	else
		PopUpResults(results, addon);
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
					"Make sure that the selection doesn't contains punctuation "
					"marks or other symbols.").Go();
		return B_ERROR;
	}

	vector<BString> results;

	if (FetchQuery(B_EQ, selection.String(), results) > 0) {
		DisplayResults(results, addon);
		return B_OK;
	}

	// No results... Lets try the same selection as a function name...
	BString funcname(selection.String());
	funcname += "()";

	if (FetchQuery(B_EQ, funcname.String(), results) > 0) {
		DisplayResults(results, addon);
		return B_OK;
	}

	// Still nothing? Lets Slap a B on the front and check for Classes!
	BString section("B");
	section += selection.String();

	if (FetchQuery(B_EQ, section.String(), results, false) > 0) {
		DisplayResults(results, addon);
		return B_OK;
	}

	// Last try... partial match.
	if (FetchQuery(B_CONTAINS, selection.String(), results, false) > 0) {
		DisplayResults(results, addon);
		return B_OK;
	}

	BString message("Nothing was found for: ");
	message += selection.String();
	MInfoAlert(message.String()).Go();

	return B_OK;
}
