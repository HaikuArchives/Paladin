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
#include "PkgPath.h"

extern PackageInfo gPkgInfo;
extern BResources *gResources;

using namespace std;

InstallEngine::InstallEngine(void)
	:	fMessenger(be_app_messenger),
		fInstalledSize(0),
		fInstalledCount(0),
		fInstallThread(-1),
		fQuitFlag(0),
		fDebugMode(false)
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


BMessage
InstallEngine::GetInstallArchive(void) const
{
	return fLogMsg;
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
InstallEngine::SetDebugMode(bool value)
{
	fDebugMode = value;
}


bool
InstallEngine::GetDebugMode(void)
{
	return fDebugMode;
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
	
	fLogMsg.MakeEmpty();
	fLogMsg.AddString("package_version", gPkgInfo.GetAppVersion());
	
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
	STRACE(1,("Platform: %s\nInstall Volume Name: %sInstall Group: %s\n",
			OSTypeToString(ospath.GetOS()).String(),
			ospath.GetVolumeName(),
			gPkgInfo.GetGroup()));
	
	// 1) Check Dependencies
	STRACE(1,("Checking dependencies...\n"));
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
			BString deppath(dep->GetPath().ResolveToString());
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
				STRACE(1,("%s is not to be installed for this platform. Skipping.\n",
					fileItem->GetName()));
				continue;
			}
			
			if (gPkgInfo.GetGroup() && strlen(gPkgInfo.GetGroup()) > 0)
			{
				if (!fileItem->BelongsToGroup(gPkgInfo.GetGroup()))
				{
					STRACE(1,("%s doesn't belong to this install group. Skipping.\n",
						fileItem->GetName()));
					continue;
				}
			}
			
			BString cookedPath(gPkgInfo.GetResolvedPath());
			if (installVol.Device() != bootVol.Device())
			{
				if (cookedPath.FindFirst("boot") == 1)
				{
					STRACE(1,("Original file install path: %s\n", cookedPath.String()));
					cookedPath.ReplaceFirst("boot",installVolName.String());
					STRACE(1,("Updated file install path: %s\n", cookedPath.String()));
				}
			}
			
			// Figure the actual destination path
			DPath destpath(cookedPath.String());
			
			if (gPkgInfo.GetInstallFolderName() && strlen(gPkgInfo.GetInstallFolderName()) > 0)
			{
				destpath.Append(gPkgInfo.GetInstallFolderName());
				STRACE(1,("Final file install path: %s\n", destpath.GetFullPath()));
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
		
		if (gNonBootInstall && gLinksOnTargetVolume)
		{
			BString tempPath(logpath.Path());
			if (tempPath.FindFirst("boot") == 1)
			{
				tempPath.ReplaceFirst("boot", installVolName.String());
				logpath = tempPath.String();
			}
		}
		
		BFile logfile;
		
		if (gTargetPlatform != OS_HAIKU &&
			gTargetPlatform != OS_HAIKU_GCC4)
		{
			logfile.SetTo(logpath.Path(),B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
			if (logfile.InitCheck() == B_OK)
			{
				logfile.Write(fLogText.String(),fLogText.Length());
				logfile.Unset();
			}
		}
		
		fLogMsg.AddInt64("package_size", fInstalledSize);
		fLogMsg.AddInt32("file_count", fInstalledCount);
		logpath = gLogArchivePath.String();
		logname = gPkgInfo.GetName();
		logname << ".pdb";
		logpath.Append(logname.String());
		
		if (gNonBootInstall && gLinksOnTargetVolume)
		{
			BString tempPath(logpath.Path());
			if (tempPath.FindFirst("boot") == 1)
			{
				tempPath.ReplaceFirst("boot", installVolName.String());
				logpath = tempPath.String();
			}
		}
		
		if (gTargetPlatform == OS_HAIKU || gTargetPlatform == OS_HAIKU_GCC4)
		{
			logfile.SetTo(logpath.Path(), B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
			if (logfile.InitCheck() == B_OK)
			{
				fLogMsg.Flatten(&logfile);
				logfile.Unset();
			}
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
		// CheckClobber returns M_ABORT_INSTALL if the install should abort,
		// B_ERROR if the file should be skipped, and B_OK if it should
		// be overwritten
		if (src->GetReplaceMode() != PKG_REPLACE_UPGRADE)
		{
			status_t result = CheckClobber(src, destpath.GetFullPath());
			if (result == M_INSTALL_ABORT)
				return B_ERROR;
			else
			if (result == B_ERROR)
				return B_OK;
		}
	}
	else
	{
		// This mode requires the file to be overwritten
		if (src->GetReplaceMode() == PKG_REPLACE_UPGRADE)
			return B_OK;
	}
	
	msg = "Installing ";
	msg << src->GetName() << "\n";
	Log(msg.String());
	
	// 2) If src contains a relative path, check to see if all of the directories exist and
	// 		create, as needed
	InstallFolder(destpath.GetFolder());
	
	// 3) Extract file to the destination
	BString command;
	command << "unzip -o '" << zipfile << "' '" << src->GetName() << "' -d '" 
			<< dest << "' > /dev/null";
	STRACE(1,("Unzip command: %s\n", command.String()));
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
		
		fInstalledSize += fileSize;
		fInstalledCount++;
		fLogMsg.AddString("items", destpath.GetFullPath());
		
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
	
	STRACE(1,("Creating folder %s\n", path));
	
	// Need to interatively create and log all folders created
	create_directory(path,0777);

	fLogText << path << "\n\tType: Folder\n";
}


void
InstallEngine::MakeLinks(FileItem *item, const char *installVolName)
{
	// All link entries are strings, so we have to do a lookup in the global constant map
//	debugger("");
	
	// Determine the source folder. This only need be done once because we can make
	// multiple links to the same source file.
	DPath srcPath;
	if (item->GetPathConstant() == M_CUSTOM_DIRECTORY)
		srcPath = item->GetResolvedPath();
	else if (item->GetPathConstant() == M_INSTALL_DIRECTORY)
		srcPath = gPkgInfo.GetResolvedPath();
	else
	{
		BPath findPath;
		find_directory((directory_which)item->GetPathConstant(), &findPath);
		srcPath = findPath.Path();
	}
	
	if (gPkgInfo.GetInstallFolderName() && strlen(gPkgInfo.GetInstallFolderName()) > 0)
		srcPath.Append(gPkgInfo.GetInstallFolderName());
	
	BString pathstr(srcPath.GetFullPath());
	if (gNonBootInstall && installVolName && strlen(installVolName) > 0)
	{
		if (pathstr.FindFirst("boot") == 1)
			pathstr.ReplaceFirst("boot",installVolName);
	}
	srcPath.SetTo(pathstr.String());
	
	OSPath targetResolver;
	if (gNonBootInstall && gLinksOnTargetVolume)
	{
		targetResolver.SetOS(gTargetPlatform);
		BVolume installVol(gPkgInfo.GetInstallVolume());
		targetResolver.SetVolume(installVol);
	}
	else
		targetResolver.SetOS(gPlatform);
	
	for (int32 i = 0; i < item->CountLinks(); i++)
	{
		// Instead of starting with a DPath, we'll use a BString to be able
		// to replace the name "boot" with the name of the install volume
		BString tempStr(item->LinkAt(i));
		
		DPath destPath;
		if (tempStr.IFindFirst("B_USER_DESKBAR_DIRECTORY") == 0)
		{
			destPath = targetResolver.DirToString(B_USER_DESKBAR_DIRECTORY);

			if (item->GetCategory() && strlen(item->GetCategory()) > 0)
			{
				BString cat(item->GetCategory());
				if (gTargetPlatform == OS_ZETA)
				{
					// The programs folder in Zeta is actually named Software. Go figure.
					// If the developer specified Applications (which is proper), then put it
					// in the right place
					cat.IReplaceFirst("Applications","Software");
				}
				destPath.Append(cat.String());
				if (!BEntry(destPath.GetFullPath()).Exists())
					create_directory(destPath.GetFullPath(),0777);
			}
		}
		else
		{
			BString dirString = item->LinkAt(i);
			int32 end = dirString.FindFirst("/");
			if (end >= 0)
				dirString.Truncate(end);
			int32 which = targetResolver.StringToDir(dirString.String());
			if (which == B_ERROR)
			{
				if (dirString[0] == '/')
					destPath = dirString;
				else
				{
					STRACE(1,("Couldn't create link %s\n", item->LinkAt(i)));
					return;
				}
			}
			else
			{
				BPath findPath;
				find_directory((directory_which)which, &findPath);
				destPath = findPath.Path();
			}
			
		}
		
		srcPath.Append(item->GetName());
		
		// Because FileItem::GetName() can be a relative path, we need to make sure that
		// we get the leaf.
		BString linkname = item->GetName();
		if (item->GetInstalledName() && strlen(item->GetInstalledName()) > 0)
			linkname = item->GetInstalledName();
		else
		{
			linkname = item->GetName();
			int32 slashpos = linkname.FindLast("/");
			if (slashpos >= 0)
			{
				if (slashpos < linkname.CountChars() - 1)
					linkname = item->GetName() + slashpos;
			}
		}
		destPath.Append(linkname);
		
		BString command;
		command << "ln -sf '" << srcPath.GetFullPath() << "' '" << destPath.GetFullPath() << "'";
		STRACE(1,("Link command: %s\n", command.String()));
		system(command.String());
	}
}


status_t
InstallEngine::CheckClobber(FileItem *item, const char *dest)
{
	// This handles all the different replacement mode code
	switch (item->GetReplaceMode())
	{
		case PKG_REPLACE_ASK_ALWAYS:
		{
			// Do nothing. User will be asked unless the decision has
			// been saved already.
			break;
		}
		case PKG_REPLACE_NEVER_REPLACE:
		{
			return B_ERROR;
			break;
		}
		case PKG_REPLACE_RENAME_EXISTING:
		{
			BString newPath(dest);
			int32 i;
			for (i = 0; i < 10; i++)
				if (BEntry(newPath.String()).Exists())
					newPath << ".old." << real_time_clock();
			
			DPath destPath(newPath);
			BEntry entry(dest);
			entry.Rename(destPath.GetFileName());
			return B_OK;
			break;
		}
		case PKG_REPLACE_ASK_NEWER_VERSION:
		{
			if (!IsNewerVersion(item->GetName(), dest))
				return B_ERROR;
			break;
		}
		case PKG_REPLACE_REPLACE_NEWER_VERSION:
		{
			return IsNewerVersion(item->GetName(), dest) ? B_OK : B_ERROR;
			break;
		}
		// The upgrade mode is handled outside of this function
		default:
		{
			break;
		}
	}

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
			BString msg(dest);
			msg << " exists. Do you want to replace it, skip installing this file, "
				<< "or cancel installation?";
			result = Query(msg.String(), "Replace","Skip","Cancel");
			break;
		}
	}
	
	int32 returnValue = B_OK;
	switch (result)
	{
		case 1:
		{
			STRACE(1,("Destination exists. Skipped installing %s\n", item->GetName()));
			BString msg("Skipped installing ");
			msg << item->GetName() << "\n";
			Log(msg.String());
			returnValue = B_ERROR;
			break;
		}
		case 2:
		{
			STRACE(1,("Destination %s exists. Aborting install\n", item->GetName()));
			Log("Canceled installation.\n");
			fMessenger.SendMessage(M_INSTALL_ABORT);
			return M_INSTALL_ABORT;
			break;
		}
		default:
			break;
	}

	// Follow the same "remember this decision?" style as Haiku's installer
	if (gClobberMode == CLOBBER_ASK)
	{
		if (result == 0)
		{
			int32 val = Query("Do you want to remember this decision for the rest of the install? All "
					"existing files will be overwritten.", "Replace all", "Always ask");
			
			gClobberMode = (val == 0) ? CLOBBER_OVERWRITE : CLOBBER_ALWAYS_ASK;
		}
		else
		{
			// Skip result
			int32 val = Query("Do you want to remember this decision for the rest of the install? All "
					"existing files will be skipped.", "Skip all", "Always ask");
			gClobberMode = (val == 0) ? CLOBBER_SKIP : CLOBBER_ALWAYS_ASK;
		}
	}
	
	return returnValue;
}


status_t
InstallEngine::GetVersion(const entry_ref &ref, version_info &info)
{
	BFile file(&ref, B_READ_ONLY);
	if (file.InitCheck() != B_OK)
		return file.InitCheck();
	
	BAppFileInfo fileInfo(&file);
	return fileInfo.GetVersionInfo(&info, B_APP_VERSION_KIND);
}


bool
InstallEngine::IsNewerVersion(const char *src, const char *dest)
{
	entry_ref ref;
	BEntry(dest).GetRef(&ref);
	
	version_info destVersion;
	if (GetVersion(ref, destVersion) != B_OK)
		return false;
	
	BEntry(src).GetRef(&ref);
	version_info srcVersion;
	GetVersion(ref, srcVersion);
	
	if (srcVersion.major > destVersion.major ||
		srcVersion.middle > destVersion.middle ||
		srcVersion.minor > destVersion.minor ||
		srcVersion.variety > destVersion.variety ||
		srcVersion.internal > destVersion.internal)
		return true;
	
	return false;
}
