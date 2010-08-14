#include "App.h"

#include <VolumeRoster.h>
#include <Volume.h>

#include "AboutWindow.h"
#include "Globals.h"
#include "MainWindow.h"
#include "ObjectList.h"

bool ProcessArgs(int32 argc, char **argv);
bool IsCommandFlag(const BString &str);
void PrintUsage(void);

App::App(void)
	:	BApplication("application/x-vnd.dw-PInstallEngine")
{
	InitGlobals();
	
	if (gCommandLineMode)
	{
		if (gPkgInfo.LoadFromResources() != B_OK)
		{
			printf("Can't load package information -- it is either corrupted or missing.\n");
			PostMessage(B_QUIT_REQUESTED);
		}
		
		if (gPrintInfoMode)
		{
			gPkgInfo.PrintToStream();
			PostMessage(B_QUIT_REQUESTED);
		}
		else
		{
			if (gGroupName.CountChars() > 0)
				gPkgInfo.SetGroup(gGroupName.String());
			
			if (gVolumeName.CountChars() > 0)
			{
				BVolumeRoster roster;
				BVolume vol;
				while (roster.GetNextVolume(&vol) == B_OK)
				{
					char buffer[B_OS_NAME_LENGTH];
					vol.GetName(buffer);
					if (gVolumeName.Compare(buffer) == 0)
					{
						gPkgInfo.SetInstallVolume(vol.Device());
						break;
					}
				}
				
			}
			
			if (gInstallPath.CountChars() > 0)
				gPkgInfo.SetInstallPath(gInstallPath.String());
			
			gInstallEngine.SetTarget(be_app_messenger);
			gInstallEngine.StartInstall();
		}
	}
	else
	{
		ShowSplash();
		MainWindow *win = new MainWindow();
		win->Show();
	}
}


void
App::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_LOG_MESSAGE:
		{
			BString str;
			if (msg->FindString("message",&str) == B_OK)
				printf(str.String());
			break;
		}
		case M_INSTALL_ABORT:
		case M_INSTALL_FAILURE:
		case M_INSTALL_SUCCESS:
		{
			PostMessage(B_QUIT_REQUESTED);
			break;
		}
		default:
		{
			BApplication::MessageReceived(msg);
			break;
		}
	}
}


int
main(int argc, char **argv)
{
	if (!ProcessArgs(argc,argv))
		return -1;
	
	App app;
	app.Run();
	
	return 0;
}


void
PrintUsage(void)
{
	printf("Usage: %s [options]\n"
			"Options:\n"
			"\t-h, --help: Prints this message\n"
			"\t-g, --group <install group name>: Sets the type of install\n"
			"\t-v, --volume <install volume name>: Sets the disk to install to\n"
			"\t-f, --folder <install folder name>: Sets the install folder\n"
			"\t-c, --clobber <ask, skip, cancel, or overwrite>: Sets the action for file conflicts\n"
			"\t-p, --platform <r5, zeta, or haiku>: Install for the specified OS\n"
			"\t-t, --target-links: Create links on target volume instead of boot volume\n"
			"\t-m, --missing <ask, continue, or cancel>: Sets the action for missing dependencies\n"
			"\t-i, --info: Prints info about the package\n"
			"\t-d, --debug: Print debugging info for the install engine\n",gAppName.String());
}


bool
IsCommandFlag(const BString &str)
{
	if (str.ICompare("--group") == 0 || str.ICompare("-g") == 0 ||
		str.ICompare("--help") == 0 || str.ICompare("-h") == 0 ||
		str.ICompare("--volume") == 0 || str.ICompare("-v") == 0 ||
		str.ICompare("--folder") == 0 || str.ICompare("-f") == 0 ||
		str.ICompare("--clobber") == 0 || str.ICompare("-c") == 0 ||
		str.ICompare("--platform") == 0 || str.ICompare("-p") == 0 ||
		str.ICompare("--target-links") == 0 || str.ICompare("-t") == 0 ||
		str.ICompare("--missing") == 0 || str.ICompare("-m") == 0 ||
		str.ICompare("--info") == 0 || str.ICompare("-i") == 0 ||
		str.ICompare("--debug") == 0 || str.ICompare("-d") == 0)
		return true;
	return false;
}


bool
ProcessArgs(int32 argc, char **argv)
{
	if (argc > 1)
		gCommandLineMode = true;
	
	BObjectList<BString>argList(20,true);
		
	for (int32 i = 1; i < argc; i++)
		argList.AddItem(new BString(argv[i]));
	
	// Possible arguments:
	// -g, --group <group name>
	// -v, --volume <volume name>
	// -f, --folder <path>
	// -c, --clobber <ask|skip|overwrite>
	// -p, --platform <r5|zeta|haiku>	--> sets gNonBootInstall
	// -t, --target-links --> set gLinkOnTargetVolume
	// -i, --info --> shows package info and exits
	// -d, --debug --> set debug mode
	
	gAppName = argv[0];
	for (int32 i = 0; i < argList.CountItems(); i++)
	{
		BString *arg = argList.ItemAt(i);
		BString *nextArg = argList.ItemAt(i + 1);
		
		if (!IsCommandFlag(*arg))
		{
			printf("Unrecognized option %s\n",arg->String());
			return false;
		}
		
		if (arg->ICompare("--help") == 0 || arg->ICompare("-h") == 0)
		{
			PrintUsage();
			return false;
		}
		
		if (arg->ICompare("--info") == 0 || arg->ICompare("-i") == 0)
		{
			gPrintInfoMode = true;
			break;
		}
		
		if (arg->ICompare("--debug") == 0 || arg->ICompare("-d") == 0)
		{
			if (argc == 2)
				gCommandLineMode = false;
			
			gDebugMode = 1;
			break;
		}
		
		if (!nextArg || IsCommandFlag(*nextArg))
		{
			PrintUsage();
			return false;
		}
		
		if (arg->ICompare("--group") == 0 || arg->ICompare("-g") == 0)
		{
			gGroupName = *nextArg;
			i++;
		}
		else if (arg->ICompare("--volume") == 0 || arg->ICompare("-v") == 0)
		{
			gVolumeName = *nextArg;
			i++;
		}
		else if (arg->ICompare("--folder") == 0 || arg->ICompare("-f") == 0)
		{
			gInstallPath = *nextArg;
			i++;
		}
		else if (arg->ICompare("--clobber") == 0 || arg->ICompare("-c") == 0)
		{
			if (nextArg->ICompare("ask") == 0)
				gClobberMode = CLOBBER_ASK;
			else if (nextArg->ICompare("overwrite") == 0)
				gClobberMode = CLOBBER_OVERWRITE;
			else if (nextArg->ICompare("skip") == 0)
				gClobberMode = CLOBBER_SKIP;
			else if (nextArg->ICompare("cancel") == 0)
				gClobberMode = CLOBBER_CANCEL;
			else
			{
				printf("Unrecognized clobber mode '%s'\n",nextArg->String());
				return false;
			}
			
			i++;
		}
		else if (arg->ICompare("--missing") == 0 || arg->ICompare("-m") == 0)
		{
			if (nextArg->ICompare("ask") == 0)
				gDepMissingMode = DEPMISSING_ASK;
			else if (nextArg->ICompare("continue") == 0)
				gDepMissingMode = DEPMISSING_CONTINUE;
			else if (nextArg->ICompare("cancel") == 0)
				gDepMissingMode = DEPMISSING_CANCEL;
			else
			{
				printf("Unrecognized missing dependency action '%s'\n",nextArg->String());
				return false;
			}
			
			i++;
		}
		else if (arg->ICompare("--platform") == 0 || arg->ICompare("-p") == 0)
		{
			ostype_t os = StringToOSType(nextArg->String());
			if (os == OS_NONE)
			{
				printf("Unrecognized platform '%s'\n",nextArg->String());
				return false;
			}
			gTargetPlatform = os;
			gNonBootInstall = true;
			i++;
		}
		else if (arg->ICompare("--target-links") == 0 || arg->ICompare("-t") == 0)
		{
			gLinksOnTargetVolume = true;
		}
	}
	return true;
}

