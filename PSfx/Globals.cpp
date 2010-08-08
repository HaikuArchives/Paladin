#include "Globals.h"


BObjectList<BString> gArgList(20,true);
bool gCommandLineMode = false;
int gReturnValue = 0;

void
GeneratePathMenu(BMenu *menu, int32 msgCmd)
{
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_DESKTOP_DIRECTORY).String(),
									B_DESKTOP_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_USER_DESKBAR_DIRECTORY).String(),
									B_USER_DESKBAR_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_APPS_DIRECTORY).String(),
									B_APPS_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_PREFERENCES_DIRECTORY).String(),
									B_PREFERENCES_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_UTILITIES_DIRECTORY).String(),
									B_UTILITIES_DIRECTORY,new BMessage(msgCmd)));
	menu->AddSeparatorItem();
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_USER_DIRECTORY).String(),
									B_USER_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_USER_ADDONS_DIRECTORY).String(),
									B_USER_ADDONS_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_USER_BOOT_DIRECTORY).String(),
									B_USER_BOOT_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_USER_CACHE_DIRECTORY).String(),
									B_USER_CACHE_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_USER_CONFIG_DIRECTORY).String(),
									B_USER_CONFIG_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_USER_DATA_DIRECTORY).String(),
									B_USER_DATA_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_USER_FONTS_DIRECTORY).String(),
									B_USER_FONTS_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_USER_LIB_DIRECTORY).String(),
									B_USER_LIB_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_USER_MEDIA_NODES_DIRECTORY).String(),
									B_USER_MEDIA_NODES_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_USER_PRINTERS_DIRECTORY).String(),
									B_USER_PRINTERS_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_USER_SETTINGS_DIRECTORY).String(),
									B_USER_SETTINGS_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_USER_SOUNDS_DIRECTORY).String(),
									B_USER_SOUNDS_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_USER_TRANSLATORS_DIRECTORY).String(),
									B_USER_TRANSLATORS_DIRECTORY,new BMessage(msgCmd)));
	menu->AddSeparatorItem();
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_DIRECTORY).String(),
									B_COMMON_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_ADDONS_DIRECTORY).String(),
									B_COMMON_ADDONS_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_BIN_DIRECTORY).String(),
									B_COMMON_BIN_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_BOOT_DIRECTORY).String(),
									B_COMMON_BOOT_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_DATA_DIRECTORY).String(),
									B_COMMON_DATA_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_DEVELOP_DIRECTORY).String(),
									B_COMMON_DEVELOP_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_DOCUMENTATION_DIRECTORY).String(),
									B_COMMON_DOCUMENTATION_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_ETC_DIRECTORY).String(),
									B_COMMON_ETC_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_FONTS_DIRECTORY).String(),
									B_COMMON_FONTS_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_LIB_DIRECTORY).String(),
									B_COMMON_LIB_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_LOG_DIRECTORY).String(),
									B_COMMON_LOG_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_MEDIA_NODES_DIRECTORY).String(),
									B_COMMON_MEDIA_NODES_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_SERVERS_DIRECTORY).String(),
									B_COMMON_SERVERS_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_SETTINGS_DIRECTORY).String(),
									B_COMMON_SETTINGS_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_SOUNDS_DIRECTORY).String(),
									B_COMMON_SOUNDS_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_SPOOL_DIRECTORY).String(),
									B_COMMON_SPOOL_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_SYSTEM_DIRECTORY).String(),
									B_COMMON_SYSTEM_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_TEMP_DIRECTORY).String(),
									B_COMMON_TEMP_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_TRANSLATORS_DIRECTORY).String(),
									B_COMMON_TRANSLATORS_DIRECTORY,new BMessage(msgCmd)));
	menu->AddItem(new PathMenuItem(GetFriendlyPathConstantName(B_COMMON_VAR_DIRECTORY).String(),
									B_COMMON_VAR_DIRECTORY,new BMessage(msgCmd)));
}


PathMenuItem::PathMenuItem(const char *label, const int32 &constant, BMessage *msg)
	:	BMenuItem(label, msg),
		fPathConstant(constant)
{
}


int32
PathMenuItem::GetPath(void) const
{
	return fPathConstant;
}


void
PathMenuItem::SetPath(const int32 &path)
{
	fPathConstant = path;
}

