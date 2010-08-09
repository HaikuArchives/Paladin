#include "InstallEngine.h"

#include <Alert.h>
#include <Application.h>
#include <Resources.h>
#include <Roster.h>
#include <stdlib.h>
#include <StorageKit.h>

#include "DepItem.h"
#include "DPath.h"
#include "FileItem.h"
#include "Globals.h"
#include "PackageInfo.h"

extern PackageInfo gPkgInfo;
extern BResources *gResources;

using namespace std;

#define TRACE_INSTALL

#ifdef TRACE_INSTALL
	#define STRACE(x) printf x
#else
	#define STRACE(x) /* */
#endif

InstallEngine::InstallEngine(void)
	:	fMessenger(be_app_messenger),
		fInstallThread(-1),
		fQuitFlag(0)
{
}


InstallEngine::~InstallEngine(void)
{
	if (fInstallThread >= 0)
	{
		atomic_add(&fQuitFlag, 1);
		while (fQuitFlag)
			snooze(10000);
	}
}


void
InstallEngine::SetTarget(BMessenger target)
{
	Lock();
	fMessenger = target;
	Unlock();
}


const char *
InstallEngine::GetInstallLog(void) const
{
	return fLogText.String();
}


void
InstallEngine::StartInstall(void)
{
	Lock();
	
	if (fInstallThread >= 0)
	{
		Unlock();
		return;
	}
	
	fInstallThread = spawn_thread(InstallThread,"installthread",B_NORMAL_PRIORITY,this);
	if (fInstallThread >= 0)
		resume_thread(fInstallThread);
	
	Unlock();
}


void
InstallEngine::StopInstall(void)
{
	Lock();
	
	if (fInstallThread >= 0)
	{
		atomic_add(&fQuitFlag, 1);
		while (fQuitFlag)
			snooze(10000);
	}
	
	Unlock();
}


bool
InstallEngine::IsInstalling(void)
{
	
	Lock();
	bool value = (fInstallThread >= 0);
	Unlock();
	
	return value;
}


void
InstallEngine::Log(const char *string)
{
	if (!string || strlen(string) < 1)
		return;
	
	BMessage msg(M_LOG_MESSAGE);
	msg.AddString("message",string);
	fMessenger.SendMessage(&msg);
}


int32
InstallEngine::Query(const char *str, const char *first, const char *second, const char *third)
{
	int32 result = -1;
	if (!str || strlen(str) < 1)
		return -1;
	
	
	if (gCommandLineMode)
	{
		printf(str);
		int answer = 0;
		if (second || third)
		{
			// Having first and one of the other two implies a choice, but if there is only
			// one option, we'll just print the message and return
			
			answer = -1;
			
			printf("\nEnter a number for one of the following choices:\n");
			
			while (answer < 0)
			{
				char choicenum = 49;
				printf("%c) %s\n",choicenum++, first);
				
				if (second)
					printf("%c) %s\n",choicenum++, second);
				
				if (third)
					printf("%c) %s\n",choicenum, third);
				
				scanf("%1d",&answer);
				
				if (answer == 1 || (answer == 2) || (third && answer == 3))
				{
					// Do nothing
				}
				else
				{
					printf("Sorry, but that isn't a choice. Please enter a number "
							"for the corresponding option\n");
					answer = -1;
				}
			}
		}
		return answer - 1;
	}
	else
	{
		if (!first || strlen(first) < 1)
			return -1;
		
		BAlert *alert = new BAlert("Package Installer",str,first,second,third);
		return alert->Go();
	}
	
	return result;
}


int32
InstallEngine::InstallThread(void *data)
{
	((InstallEngine*)data)->DoInstall();
	return 0;
}


void
InstallEngine::DoInstall(void)
{
//	debugger("");
	Log("Checking Dependencies.\n");
	
	status_t status = B_OK;
	
	BEntry entry;
	BVolume installVol(gPkgInfo.GetInstallVolume());
		
	OSPath ospath;
	if (gNonBootInstall)
	{
		// For the purposes of dependency checking, we only set the volume if we're doing an install
		// to a non-boot BeOS installation. This is because OSPath forces lookups to the 
		// specified volume.
		ospath.SetVolume(installVol);
		ospath.SetOS(gTargetPlatform);
	}
	STRACE(("Platform: %s\nInstall Volume Name: %sInstall Group: %s\n",
			OSTypeToString(ospath.GetOS()).String(),
			ospath.GetVolumeName(),
			gPkgInfo.GetGroup()));
	
	// 1) Check Dependencies
	STRACE(("Checking dependencies...\n"));
	for (int32 i = 0; i < gPkgInfo.CountDependencies(); i++)
	{
		DepItem *dep = gPkgInfo.DependencyAt(i);
		
		BPath path;
		BString	missing = dep->GetName();
		if (dep->GetType() == DEP_LIBRARY)
		{
			path.SetTo(ospath.GetPath(B_USER_LIB_DIRECTORY).String());
			path.Append(dep->GetFileName());
			entry.SetTo(path.Path());
			if (entry.Exists())
				missing = "";
			else
			{
				path.SetTo(ospath.GetPath(B_COMMON_LIB_DIRECTORY).String());
				path.Append(dep->GetFileName());
				entry.SetTo(path.Path());
				if (entry.Exists())
					missing = "";
				else
				{
					path.SetTo(ospath.GetPath(B_BEOS_LIB_DIRECTORY).String());
					path.Append(dep->GetFileName());
					entry.SetTo(path.Path());
					if (entry.Exists())
						missing = "";
				}
			}
		}
		else
		{
			BString deppath(dep->GetResolvedPath());
			if (gNonBootInstall && BString(ospath.GetVolumeName()).CountChars() > 0 && 
				deppath.FindFirst("boot") == 1)
			{
				deppath.ReplaceFirst("boot",ospath.GetVolumeName());
			}
			
			path.SetTo(deppath.String());
			path.Append(dep->GetFileName());
			entry.SetTo(path.Path());
			if (entry.Exists())
				missing = "";
		}
		
		if (missing.CountChars() > 0)
		{
			int32 result = -1;
			
			switch (gDepMissingMode)
			{
				case DEPMISSING_CANCEL:
				{
					result = 1;
					break;
				}
				case DEPMISSING_CONTINUE:
				{
					result = 0;
					break;
				}
				default:
				{
					BString errmsg;
					errmsg << missing << " couldn't be found, but is needed by " << gPkgInfo.GetName()
							<< ". Would you like to continue or abort installation?";
					result = Query(errmsg.String(),"Continue", "Abort");
					break;
				}
			}
			
			if (result == 1)
			{
				Log("Aborting installation.\n");
				status = B_ERROR;
			}
		}
	}
	
	//debugger("");
	//	2) Unzip matching file, making any requested links
	BVolumeRoster volRoster;
	BVolume bootVol;
	volRoster.GetBootVolume(&bootVol);
	
	BString installVolName;
	char *buffer = installVolName.LockBuffer(B_OS_NAME_LENGTH);
	installVol.GetName(buffer);
	installVolName.UnlockBuffer();
	
	if (status == B_OK)
	{
		time_t t = time(NULL);
		struct tm timestruct = *localtime(&t);
		char installTimeStr[64];
		strftime(installTimeStr,64,"%c",&timestruct);
		
		fLogText	<< "Installation: " << gPkgInfo.GetName() << "\n"
				<< "Started: " << installTimeStr << "\n\n\n";
		
		app_info aInfo;
		be_app->GetAppInfo(&aInfo);
		DPath zipfilepath(aInfo.ref);
		
		for (int32 i = 0; i < gPkgInfo.CountFiles(); i++)
		{
			FileItem *fileItem = gPkgInfo.FileAt(i);
			
			// Are we even supposed to install the file? Check both platform and group
			if (!fileItem->BelongsToPlatform(gTargetPlatform))
			{
				STRACE(("%s is not to be installed for this platform. Skipping.\n",
					fileItem->GetName()));
				continue;
			}
			
			if (gPkgInfo.GetGroup() && strlen(gPkgInfo.GetGroup()) > 0)
			{
				if (!fileItem->BelongsToGroup(gPkgInfo.GetGroup()))
				{
					STRACE(("%s doesn't belong to this install group. Skipping.\n",
						fileItem->GetName()));
					continue;
				}
			}
			
			BString cookedPath(gPkgInfo.GetResolvedPath());
			if (installVol.Device() != bootVol.Device())
			{
				if (cookedPath.FindFirst("boot") == 1)
				{
					STRACE(("Original file install path: %s\n", cookedPath.String()));
					cookedPath.ReplaceFirst("boot",installVolName.String());
					STRACE(("Updated file install path: %s\n", cookedPath.String()));
				}
			}
			
			// Figure the actual destination path
			DPath destpath(cookedPath.String());
			
			if (gPkgInfo.GetInstallFolderName() && strlen(gPkgInfo.GetInstallFolderName()) > 0)
			{
				destpath.Append(gPkgInfo.GetInstallFolderName());
				STRACE(("Final file install path: %s\n", destpath.GetFullPath()));
			}
			
			status = InstallFromZip(zipfilepath.GetFullPath(),fileItem,destpath.GetFullPath());
			if (status != B_OK)
				break;
			
			MakeLinks(fileItem, installVolName.String());
		}
		
		t = time(NULL);
		timestruct = *localtime(&t);
		strftime(installTimeStr,64,"%c",&timestruct);
		fLogText << "\n\nCompleted: " << installTimeStr << "\n\n";
		
		BPath logpath(gLogFilePath.String());
		BString logname(gPkgInfo.GetName());
		logname << " Install Log";
		logpath.Append(logname.String());
		BFile logfile(logpath.Path(),B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
		if (logfile.InitCheck() == B_OK)
		{
			logfile.Write(fLogText.String(),fLogText.Length());
			logfile.Unset();
		}
	}
	
	Log("Installation completed successfully.\n");
	fMessenger.SendMessage(M_INSTALL_SUCCESS);
	
	Lock();
	fInstallThread = -1;
	Unlock();
	
	if (fQuitFlag > 0)
		atomic_add(&fQuitFlag, -1);
}


status_t
InstallEngine::InstallFromZip(const char *zipfile, FileItem *src, const char *dest)
{
	//	3a) Create directory, if needed
	//	3b) Move File
	
	BString msg;
	
	// 1) Check if the file already exists
	DPath destpath(dest);
	destpath.Append(src->GetName());
	if (BEntry(destpath.GetFullPath()).Exists())
	{
		int32 result = -1;
		switch (gClobberMode)
		{
			case CLOBBER_OVERWRITE:
			{
				result = 0;
				break;
			}
			case CLOBBER_SKIP:
			{
				result = 1;
				break;
			}
			case CLOBBER_CANCEL:
			{
				result = 2;
				break;
			}
			default:
			{
				msg = destpath.GetFullPath();
				msg << " exists. Do you want to overwrite it, skip installing this file, or cancel installation?";
				result = Query(msg.String(), "Overwrite","Skip","Cancel");
				break;
			}
		}
		
		switch (result)
		{
			case 1:
			{
				STRACE(("Destination exists. Skipped installing %s\n", src->GetName()));
				msg = "Skipped installing ";
				msg << src->GetName() << "\n";
				Log(msg.String());
				return B_OK;
				break;
			}
			case 2:
			{
				STRACE(("Destination %s exists. Aborting install\n", src->GetName()));
				Log("Canceled installation.\n");
				fMessenger.SendMessage(M_INSTALL_ABORT);
				return B_ERROR;
				break;
			}
			default:
				break;
		}
	}
	
	msg = "Installing ";
	msg << src->GetName() << "\n";
	Log(msg.String());
	
	// 2) If src contains a relative path, check to see if all of the directories exist and
	// 		create, as needed
	InstallFolder(destpath.GetFolder());
	
	// 3) Extract file to the destination
	BString command;
	command << "unzip -o '" << zipfile << "' '" << src->GetName() << "' -d '" << dest << "' > /dev/null";
	STRACE(("Unzip command: %s\n", command.String()));
	system(command.String());
	
	
	// Handle the log-related stuff for the file
	BFile file(destpath.GetFullPath(),B_READ_ONLY);
	if (file.InitCheck() == B_OK)
	{
		off_t fileSize;
		file.GetSize(&fileSize);
		
		BNodeInfo nodeInfo(&file);
		char type[B_MIME_TYPE_LENGTH];
		if (nodeInfo.GetType(type) != B_OK)
		{
			// try identifying the file
			update_mime_info(destpath.GetFullPath(),0,1,0);
			nodeInfo.GetType(type);
		}
		
		fLogText << destpath.GetFullPath() << "\n"
				<< "\tSize: " << fileSize << " bytes" << "\t\tType: " << type << "\n";
	}
	
	return B_OK;
}


void
InstallEngine::InstallFolder(const char *path)
{
	BEntry entry(path);
	if (entry.Exists())
		return;
	
	STRACE(("Creating folder %s\n", path));
	
	// Need to interatively create and log all folders created
	create_directory(path,0777);

	fLogText << path << "\n\tType: Folder\n";
}


void
InstallEngine::MakeLinks(FileItem *item, const char *installVolName)
{
	// All link entries are strings, so we have to do a lookup in the global constant map
	//debugger("");
	
	DPath path(item->GetResolvedPath());
	if (gPkgInfo.GetInstallFolderName() && strlen(gPkgInfo.GetInstallFolderName()) > 0)
		path.Append(gPkgInfo.GetInstallFolderName());
	BString pathstr(path.GetFullPath());
	if (installVolName && strlen(installVolName) > 0)
	{
		if (pathstr.FindFirst("boot") == 1)
			pathstr.ReplaceFirst("boot",installVolName);
	}
	path.SetTo(pathstr.String());
	
	OSPath deskbar;
	if (gNonBootInstall)
	{
		deskbar.SetOS(gTargetPlatform);
		BVolume installVol(gPkgInfo.GetInstallVolume());
		deskbar.SetVolume(installVol);
	}

	for (int32 i = 0; i < item->CountLinks(); i++)
	{
		BString linkstr(item->LinkAt(i));
		if (gNonBootInstall && installVolName && strlen(installVolName) > 0)
		{
			if (linkstr.FindFirst("boot") == 1)
				linkstr.ReplaceFirst("boot",installVolName);
		}
		
		DPath linkpath(linkstr.String());
		
		if (linkstr.FindFirst(deskbar.GetPath(B_USER_DESKBAR_DIRECTORY)) == 0)
		{
			if (item->GetCategory() && strlen(item->GetCategory()) > 0)
			{
				BString cat(item->GetCategory());
				if (gTargetPlatform == OS_ZETA)
				{
					// The programs folder in Zeta is actually named Software. Go figure.
					// If the developer specified Applications (which is proper), then put it
					// in the right place
					if (cat.IFindFirst("Applications") == 0)
						cat.IReplaceFirst("Applications","Software");
				}
				linkpath.Append(cat.String());
				if (!BEntry(linkpath.GetFullPath()).Exists())
					create_directory(linkpath.GetFullPath(),0777);
			}
		}
		
		path.Append(item->GetName());
		
		// Because FileItem::GetName() can be a relative path, we need to make sure that
		// we get the leaf.
		BString linkname = item->GetName();
		int32 slashpos = linkname.FindLast("/");
		if (slashpos >= 0)
		{
			if (slashpos < linkname.CountChars() - 1)
				linkname = item->GetName() + slashpos;
		}
		
		linkpath.Append(linkname);
		
		BString command;
		command << "ln -sf '" << path.GetFullPath() << "' '" << linkpath.GetFullPath() << "'";
		STRACE(("Link command: %s\n", command.String()));
		system(command.String());
	}
}


