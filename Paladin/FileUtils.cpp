#include "FileUtils.h"

#include <Alert.h>
#include <Bitmap.h>
#include <Directory.h>
#include <Mime.h>
#include <Path.h>
#include <Roster.h>

#include "Icons.h"
#include "Paladin.h"
#include "PLocale.h"
#include "Project.h"

void
InitFileTypes(void)
{
	BMimeType mime;
	BString string;
	BMessage msg,ext;
	
	BBitmap	large_icon(BRect(0, 0, B_LARGE_ICON - 1, B_LARGE_ICON - 1), B_COLOR_8_BIT);
	memcpy(large_icon.Bits(),kProjectLargeIconBits,1024);
	BBitmap	mini_icon(BRect(0, 0, B_MINI_ICON - 1, B_MINI_ICON - 1), B_COLOR_8_BIT);
	memcpy(mini_icon.Bits(),kProjectSmallIconBits,256);
	
	mime.SetType(PROJECT_MIME_TYPE);
	mime.SetShortDescription(TR("Paladin Project"));
	mime.SetLongDescription(TR("File to build a program with Paladin"));
	mime.SetIcon(&large_icon, B_LARGE_ICON);
	mime.SetIcon(&mini_icon, B_MINI_ICON);
	
	#ifdef __HAIKU__
	mime.SetIcon(kProjectVectorIconBits, sizeof(kProjectVectorIconBits));
	#endif
	
	mime.SetSnifferRule("0.50  [0:32]( -i \"NAME=\" | \"TARGETNAME=\" | "
						"\"PLATFORM=\" | \"GROUP=\" | \"SOURCEFILE=\")");
	mime.SetPreferredApp(APP_SIGNATURE);
	mime.Install();
	
	ext.AddString("extensions","pld");
	mime.SetFileExtensions(&ext);
}


void
FindAndOpenFile(BMessage *msg)
{
	if (!msg)
		return;
	
	BString filename;
	if (msg->FindString("name",&filename) == B_OK)
	{
		BString foldername;
		int32 i = 0;
		while (msg->FindString("folder",i,&foldername) == B_OK)
		{
			entry_ref folderref;
			BEntry entry(foldername.String());
			if (entry.InitCheck() == B_OK)
			{
				entry.GetRef(&folderref);
				
				entry_ref fileref = FindFile(folderref,filename.String());
				if (fileref.name)
				{
					be_roster->Launch(&fileref);
					return;
				}
			}
			i++;
		}
		
		BString errorstr = TR("Couldn't find ");
		errorstr << filename;
		BAlert *alert = new BAlert("Paladin",errorstr.String(),"OK");
		alert->Go();
	}
}


entry_ref
FindFile(entry_ref folder, const char *name)
{
	entry_ref ref,returnRef;
	if (!folder.name || !name)
		return returnRef;
	
	BDirectory dir(&folder);
	if (dir.InitCheck() != B_OK)
		return returnRef;
	
	dir.Rewind();
	while (dir.GetNextRef(&ref) == B_OK)
	{
		struct stat statData;
		stat(BPath(&ref).Path(),&statData);
		
		// Is a directory?
		if (S_ISDIR(statData.st_mode))
		{
			entry_ref innerref = FindFile(ref,name);
			if (innerref.device != -1 && innerref.directory != -1)
				return innerref;
		}
	}
	
	BEntry entry;
	if (dir.FindEntry(name,&entry) == B_OK)
		entry.GetRef(&returnRef);
	return returnRef;
}


entry_ref
FindProject(entry_ref folder, const char *name)
{
	printf("Searching for %s in folder %s\n", name, folder.name);
	entry_ref ref,returnRef;
	if (!folder.name || !name)
		return returnRef;
	
	// Because projects can now have source control folders, skip the
	// internal folders for Git, Mercurial, Subversion, and CVS
	if (strcmp(folder.name, ".hg") == 0 || strcmp(folder.name, ".git") == 0 ||
		strcmp(folder.name, ".svn") == 0 ||	strcmp(folder.name, "CVS") == 0)
		return returnRef;
	
	BDirectory dir(&folder);
	if (dir.InitCheck() != B_OK)
		return returnRef;
	
	dir.Rewind();
	while (dir.GetNextRef(&ref) == B_OK)
	{
		struct stat statData;
		stat(BPath(&ref).Path(),&statData);
		
		// Is a directory?
		if (S_ISDIR(statData.st_mode))
		{
			entry_ref innerref = FindFile(ref,name);
			if (innerref.device != -1 && innerref.directory != -1)
				return innerref;
		}
	}
	
	BEntry entry;
	if (dir.FindEntry(name,&entry) == B_OK)
		entry.GetRef(&returnRef);
	return returnRef;
}


void
SendToEditor(BMessage msg, BMessage *reply)
{
	if (!be_roster->IsRunning(EDITOR_SIGNATURE) &&
		be_roster->Launch(EDITOR_SIGNATURE) != B_OK)
	{
		BAlert *alert = new BAlert("Paladin",TR("Couldn't start up the editor"), "OK");
		alert->Go();
		return;
	}
	
	BMessenger msgr(EDITOR_SIGNATURE);
	if (reply)
		msgr.SendMessage(&msg,reply);
	else
		msgr.SendMessage(&msg);
}

