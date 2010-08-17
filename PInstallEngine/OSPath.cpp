#include "OSPath.h"
#include <VolumeRoster.h>
#include <stdio.h>

static FindData sR5FindData[] = {
	{ B_DESKTOP_DIRECTORY, "B_DESKTOP_DIRECTORY", "/boot/home/Desktop" },
	{ B_TRASH_DIRECTORY, "B_TRASH_DIRECTORY", "/boot/home/Desktop/Trash" },
	
	{ B_BEOS_DIRECTORY, "B_BEOS_DIRECTORY", "/boot/beos" },
	{ B_BEOS_SYSTEM_DIRECTORY, "B_BEOS_SYSTEM_DIRECTORY", "/boot/beos/system" },
	{ B_BEOS_ADDONS_DIRECTORY, "B_BEOS_ADDONS_DIRECTORY", "/boot/beos/system/add-ons" },
	{ B_BEOS_BOOT_DIRECTORY, "B_BEOS_BOOT_DIRECTORY", "/boot/beos/system/boot" },
	{ B_BEOS_FONTS_DIRECTORY, "B_BEOS_FONTS_DIRECTORY", "/boot/beos/etc/fonts" },
	{ B_BEOS_LIB_DIRECTORY, "B_BEOS_LIB_DIRECTORY", "/boot/beos/system/lib" },
	{ B_BEOS_SERVERS_DIRECTORY, "B_BEOS_SERVERS_DIRECTORY", "/boot/beos/system/servers" },
	{ B_BEOS_APPS_DIRECTORY, "B_BEOS_APPS_DIRECTORY", "/boot/beos/apps" },
	{ B_BEOS_BIN_DIRECTORY, "B_BEOS_BIN_DIRECTORY", "/boot/beos/bin" },
	{ B_BEOS_ETC_DIRECTORY, "B_BEOS_ETC_DIRECTORY", "/boot/beos/etc" },
	{ B_BEOS_DOCUMENTATION_DIRECTORY, "B_BEOS_DOCUMENTATION_DIRECTORY", "/boot/beos/documentation" },
	{ B_BEOS_PREFERENCES_DIRECTORY, "B_BEOS_PREFERENCES_DIRECTORY", "/boot/beos/preferences" },
	{ B_BEOS_TRANSLATORS_DIRECTORY, "B_BEOS_TRANSLATORS_DIRECTORY", "/boot/beos/system/add-ons/Translators" },
	{ B_BEOS_MEDIA_NODES_DIRECTORY, "B_BEOS_MEDIA_NODES_DIRECTORY", "/boot/beos/system/add-ons/media" },
	{ B_BEOS_SOUNDS_DIRECTORY, "B_BEOS_SOUNDS_DIRECTORY", "/boot/beos/etc/sounds" },
	{ (directory_which)B_BEOS_DATA_DIRECTORY, "B_BEOS_DATA_DIRECTORY", "/boot/beos/data" },
	
	{ B_COMMON_DIRECTORY, "B_COMMON_DIRECTORY", "/boot/home" },
	{ B_COMMON_SYSTEM_DIRECTORY, "B_COMMON_SYSTEM_DIRECTORY", "/boot/home/config" },
	{ B_COMMON_ADDONS_DIRECTORY, "B_COMMON_ADDONS_DIRECTORY", "/boot/home/config/add-ons" },
	{ B_COMMON_BOOT_DIRECTORY, "B_COMMON_BOOT_DIRECTORY", "/boot/home/config/boot" },
	{ B_COMMON_FONTS_DIRECTORY, "B_COMMON_FONTS_DIRECTORY", "/boot/home/config/fonts" },
	{ B_COMMON_LIB_DIRECTORY, "B_COMMON_LIB_DIRECTORY", "/boot/home/config/lib" },
	{ B_COMMON_SERVERS_DIRECTORY, "B_COMMON_SERVERS_DIRECTORY", "/boot/home/config/servers" },
	{ B_COMMON_BIN_DIRECTORY, "B_COMMON_BIN_DIRECTORY", "/boot/home/config/bin" },
	{ B_COMMON_ETC_DIRECTORY, "B_COMMON_ETC_DIRECTORY", "/boot/home/config/etc" },
	{ B_COMMON_DOCUMENTATION_DIRECTORY, "B_COMMON_DOCUMENTATION_DIRECTORY", "/boot/home/config/documentation" },
	{ B_COMMON_SETTINGS_DIRECTORY, "B_COMMON_SETTINGS_DIRECTORY", "/boot/home/config/settings" },
	{ B_COMMON_DEVELOP_DIRECTORY, "B_COMMON_DEVELOP_DIRECTORY", "/boot/develop" },
	{ B_COMMON_LOG_DIRECTORY, "B_COMMON_LOG_DIRECTORY", "/boot/var/log" },
	{ B_COMMON_SPOOL_DIRECTORY, "B_COMMON_SPOOL_DIRECTORY", "/boot/var/spool" },
	{ B_COMMON_TEMP_DIRECTORY, "B_COMMON_TEMP_DIRECTORY", "/boot/var/tmp" },
	{ B_COMMON_VAR_DIRECTORY, "B_COMMON_VAR_DIRECTORY", "/boot/var" },
	{ B_COMMON_TRANSLATORS_DIRECTORY, "B_COMMON_TRANSLATORS_DIRECTORY", "/boot/home/config/add-ons/Translators" },
	{ B_COMMON_MEDIA_NODES_DIRECTORY, "B_COMMON_MEDIA_NODES_DIRECTORY", "/boot/home/config/add-ons/media" },
	{ B_COMMON_SOUNDS_DIRECTORY, "B_COMMON_SOUNDS_DIRECTORY", "/boot/home/config/sounds" },
	{ (directory_which)B_COMMON_DATA_DIRECTORY, "B_COMMON_DATA_DIRECTORY", "/boot/home/config/data" },
	
	{ B_USER_DIRECTORY, "B_USER_DIRECTORY", "/boot/home" },
	{ B_USER_CONFIG_DIRECTORY, "B_USER_CONFIG_DIRECTORY", "/boot/home/config" },
	{ B_USER_ADDONS_DIRECTORY, "B_USER_ADDONS_DIRECTORY", "/boot/home/config/add-ons" },
	{ B_USER_BOOT_DIRECTORY, "B_USER_BOOT_DIRECTORY", "/boot/home/config/boot" },
	{ B_USER_FONTS_DIRECTORY, "B_USER_FONTS_DIRECTORY", "/boot/home/config/fonts" },
	{ B_USER_LIB_DIRECTORY, "B_USER_LIB_DIRECTORY", "/boot/home/config/lib" },
	{ B_USER_SETTINGS_DIRECTORY, "B_USER_SETTINGS_DIRECTORY", "/boot/home/config/settings" },
	{ B_USER_DESKBAR_DIRECTORY, "B_USER_DESKBAR_DIRECTORY", "/boot/home/config/be" },
	{ B_USER_PRINTERS_DIRECTORY, "B_USER_PRINTERS_DIRECTORY", "/boot/home/config/settings/printers" },
	{ B_USER_TRANSLATORS_DIRECTORY, "B_USER_TRANSLATORS_DIRECTORY", "/boot/home/config/add-ons/Translators" },
	{ B_USER_MEDIA_NODES_DIRECTORY, "B_USER_MEDIA_NODES_DIRECTORY", "/boot/home/config/add-ons/media" },
	{ B_USER_SOUNDS_DIRECTORY, "B_USER_SOUNDS_DIRECTORY", "/boot/home/config/sounds" },
	{ (directory_which)B_USER_DATA_DIRECTORY, "B_USER_DATA_DIRECTORY", "/boot/home/config/data" },
	{ (directory_which)B_USER_CACHE_DIRECTORY, "B_USER_CACHE_DIRECTORY", "/boot/home/config/cache" },
	
	{ B_APPS_DIRECTORY, "B_APPS_DIRECTORY", "/boot/apps" },
	{ B_PREFERENCES_DIRECTORY, "B_PREFERENCES_DIRECTORY", "/boot/preferences" },
	{ B_UTILITIES_DIRECTORY, "B_UTILITIES_DIRECTORY", "/boot/utilities" },
	{ (directory_which) -1, "", "" }
};


static FindData sZetaFindData[] = {
	{ B_DESKTOP_DIRECTORY, "B_DESKTOP_DIRECTORY", "/boot/home/Desktop" },
	{ B_TRASH_DIRECTORY, "B_TRASH_DIRECTORY", "/boot/home/Desktop/Trash" },
	
	{ B_BEOS_DIRECTORY, "B_BEOS_DIRECTORY", "/boot/beos" },
	{ B_BEOS_SYSTEM_DIRECTORY, "B_BEOS_SYSTEM_DIRECTORY", "/boot/beos/system" },
	{ B_BEOS_ADDONS_DIRECTORY, "B_BEOS_ADDONS_DIRECTORY", "/boot/beos/system/add-ons" },
	{ B_BEOS_BOOT_DIRECTORY, "B_BEOS_BOOT_DIRECTORY", "/boot/beos/system/boot" },
	{ B_BEOS_FONTS_DIRECTORY, "B_BEOS_FONTS_DIRECTORY", "/boot/beos/etc/fonts" },
	{ B_BEOS_LIB_DIRECTORY, "B_BEOS_LIB_DIRECTORY", "/boot/beos/system/lib" },
	{ B_BEOS_SERVERS_DIRECTORY, "B_BEOS_SERVERS_DIRECTORY", "/boot/beos/system/servers" },
	{ B_BEOS_APPS_DIRECTORY, "B_BEOS_APPS_DIRECTORY", "/boot/beos/apps" },
	{ B_BEOS_BIN_DIRECTORY, "B_BEOS_BIN_DIRECTORY", "/boot/beos/bin" },
	{ B_BEOS_ETC_DIRECTORY, "B_BEOS_ETC_DIRECTORY", "/boot/beos/etc" },
	{ B_BEOS_DOCUMENTATION_DIRECTORY, "B_BEOS_DOCUMENTATION_DIRECTORY", "/boot/beos/documentation" },
	{ B_BEOS_PREFERENCES_DIRECTORY, "B_BEOS_PREFERENCES_DIRECTORY", "/boot/beos/preferences" },
	{ B_BEOS_TRANSLATORS_DIRECTORY, "B_BEOS_TRANSLATORS_DIRECTORY", "/boot/beos/system/add-ons/Translators" },
	{ B_BEOS_MEDIA_NODES_DIRECTORY, "B_BEOS_MEDIA_NODES_DIRECTORY", "/boot/beos/system/add-ons/media" },
	{ B_BEOS_SOUNDS_DIRECTORY, "B_BEOS_SOUNDS_DIRECTORY", "/boot/beos/etc/sounds" },
	{ (directory_which)B_BEOS_DATA_DIRECTORY, "B_BEOS_DATA_DIRECTORY", "/boot/beos/data" },
	
	{ B_COMMON_DIRECTORY, "B_COMMON_DIRECTORY", "/boot/home" },
	{ B_COMMON_SYSTEM_DIRECTORY, "B_COMMON_SYSTEM_DIRECTORY", "/boot/home/config" },
	{ B_COMMON_ADDONS_DIRECTORY, "B_COMMON_ADDONS_DIRECTORY", "/boot/home/config/add-ons" },
	{ B_COMMON_BOOT_DIRECTORY, "B_COMMON_BOOT_DIRECTORY", "/boot/home/config/boot" },
	{ B_COMMON_FONTS_DIRECTORY, "B_COMMON_FONTS_DIRECTORY", "/boot/home/config/fonts" },
	{ B_COMMON_LIB_DIRECTORY, "B_COMMON_LIB_DIRECTORY", "/boot/home/config/lib" },
	{ B_COMMON_SERVERS_DIRECTORY, "B_COMMON_SERVERS_DIRECTORY", "/boot/home/config/servers" },
	{ B_COMMON_BIN_DIRECTORY, "B_COMMON_BIN_DIRECTORY", "/boot/home/config/bin" },
	{ B_COMMON_ETC_DIRECTORY, "B_COMMON_ETC_DIRECTORY", "/boot/home/config/etc" },
	{ B_COMMON_DOCUMENTATION_DIRECTORY, "B_COMMON_DOCUMENTATION_DIRECTORY", "/boot/home/config/documentation" },
	{ B_COMMON_SETTINGS_DIRECTORY, "B_COMMON_SETTINGS_DIRECTORY", "/boot/home/config/settings" },
	{ B_COMMON_DEVELOP_DIRECTORY, "B_COMMON_DEVELOP_DIRECTORY", "/boot/develop" },
	{ B_COMMON_LOG_DIRECTORY, "B_COMMON_LOG_DIRECTORY", "/boot/var/log" },
	{ B_COMMON_SPOOL_DIRECTORY, "B_COMMON_SPOOL_DIRECTORY", "/boot/var/spool" },
	{ B_COMMON_TEMP_DIRECTORY, "B_COMMON_TEMP_DIRECTORY", "/boot/var/tmp" },
	{ B_COMMON_VAR_DIRECTORY, "B_COMMON_VAR_DIRECTORY", "/boot/var" },
	{ B_COMMON_TRANSLATORS_DIRECTORY, "B_COMMON_TRANSLATORS_DIRECTORY", "/boot/home/config/add-ons/Translators" },
	{ B_COMMON_MEDIA_NODES_DIRECTORY, "B_COMMON_MEDIA_NODES_DIRECTORY", "/boot/home/config/add-ons/media" },
	{ B_COMMON_SOUNDS_DIRECTORY, "B_COMMON_SOUNDS_DIRECTORY", "/boot/home/config/sounds" },
	{ (directory_which)B_COMMON_DATA_DIRECTORY, "B_COMMON_DATA_DIRECTORY", "/boot/home/config/data" },
	
	{ B_USER_DIRECTORY, "B_USER_DIRECTORY", "/boot/home" },
	{ B_USER_CONFIG_DIRECTORY, "B_USER_CONFIG_DIRECTORY", "/boot/home/config" },
	{ B_USER_ADDONS_DIRECTORY, "B_USER_ADDONS_DIRECTORY", "/boot/home/config/add-ons" },
	{ B_USER_BOOT_DIRECTORY, "B_USER_BOOT_DIRECTORY", "/boot/home/config/boot" },
	{ B_USER_FONTS_DIRECTORY, "B_USER_FONTS_DIRECTORY", "/boot/home/config/fonts" },
	{ B_USER_LIB_DIRECTORY, "B_USER_LIB_DIRECTORY", "/boot/home/config/lib" },
	{ B_USER_SETTINGS_DIRECTORY, "B_USER_SETTINGS_DIRECTORY", "/boot/home/config/settings" },
	{ B_USER_DESKBAR_DIRECTORY, "B_USER_DESKBAR_DIRECTORY", "/boot/home/config/ZetaMenu" },
	{ B_USER_PRINTERS_DIRECTORY, "B_USER_PRINTERS_DIRECTORY", "/boot/home/config/settings/printers" },
	{ B_USER_TRANSLATORS_DIRECTORY, "B_USER_TRANSLATORS_DIRECTORY", "/boot/home/config/add-ons/Translators" },
	{ B_USER_MEDIA_NODES_DIRECTORY, "B_USER_MEDIA_NODES_DIRECTORY", "/boot/home/config/add-ons/media" },
	{ B_USER_SOUNDS_DIRECTORY, "B_USER_SOUNDS_DIRECTORY", "/boot/home/config/sounds" },
	{ (directory_which)B_USER_DATA_DIRECTORY, "B_USER_DATA_DIRECTORY", "/boot/home/config/data" },
	{ (directory_which)B_USER_CACHE_DIRECTORY, "B_USER_CACHE_DIRECTORY", "/boot/home/config/cache" },
	
	{ B_APPS_DIRECTORY, "B_APPS_DIRECTORY", "/boot/apps" },
	{ B_PREFERENCES_DIRECTORY, "B_PREFERENCES_DIRECTORY", "/boot/preferences" },
	{ B_UTILITIES_DIRECTORY, "B_UTILITIES_DIRECTORY", "/boot/utilities" },
	{ (directory_which) -1, "", "" }
};


static FindData sHaikuFindData[] = {
	{ B_DESKTOP_DIRECTORY, "B_DESKTOP_DIRECTORY", "/boot/home/Desktop" },
	{ B_TRASH_DIRECTORY, "B_TRASH_DIRECTORY", "/boot/home/trash" },
	
	{ B_BEOS_DIRECTORY, "B_BEOS_DIRECTORY", "/boot/system" },
	{ B_BEOS_SYSTEM_DIRECTORY, "B_BEOS_SYSTEM_DIRECTORY", "/boot/system" },
	{ B_BEOS_ADDONS_DIRECTORY, "B_BEOS_ADDONS_DIRECTORY", "/boot/system/add-ons" },
	{ B_BEOS_BOOT_DIRECTORY, "B_BEOS_BOOT_DIRECTORY", "/boot/system/boot" },
	{ B_BEOS_FONTS_DIRECTORY, "B_BEOS_FONTS_DIRECTORY", "/boot/system/fonts" },
	{ B_BEOS_LIB_DIRECTORY, "B_BEOS_LIB_DIRECTORY", "/boot/system/lib" },
	{ B_BEOS_SERVERS_DIRECTORY, "B_BEOS_SERVERS_DIRECTORY", "/boot/system/servers" },
	{ B_BEOS_APPS_DIRECTORY, "B_BEOS_APPS_DIRECTORY", "/boot/system/apps" },
	{ B_BEOS_BIN_DIRECTORY, "B_BEOS_BIN_DIRECTORY", "/boot/system/bin" },
	{ B_BEOS_ETC_DIRECTORY, "B_BEOS_ETC_DIRECTORY", "/boot/system/etc" },
	{ B_BEOS_DOCUMENTATION_DIRECTORY, "B_BEOS_DOCUMENTATION_DIRECTORY", "/boot/system/documentation" },
	{ B_BEOS_PREFERENCES_DIRECTORY, "B_BEOS_PREFERENCES_DIRECTORY", "/boot/system/preferences" },
	{ B_BEOS_TRANSLATORS_DIRECTORY, "B_BEOS_TRANSLATORS_DIRECTORY", "/boot/system/add-ons/Translators" },
	{ B_BEOS_MEDIA_NODES_DIRECTORY, "B_BEOS_MEDIA_NODES_DIRECTORY", "/boot/system/add-ons/media" },
	{ B_BEOS_SOUNDS_DIRECTORY, "B_BEOS_SOUNDS_DIRECTORY", "/boot/system/data/sounds" },
	{ (directory_which)B_BEOS_DATA_DIRECTORY, "B_BEOS_DATA_DIRECTORY", "/boot/system/data" },
	
	{ B_COMMON_DIRECTORY, "B_COMMON_DIRECTORY", "/boot/common" },
	{ B_COMMON_SYSTEM_DIRECTORY, "B_COMMON_SYSTEM_DIRECTORY", "/boot/common" },
	{ B_COMMON_ADDONS_DIRECTORY, "B_COMMON_ADDONS_DIRECTORY", "/boot/common/add-ons" },
	{ B_COMMON_BOOT_DIRECTORY, "B_COMMON_BOOT_DIRECTORY", "/boot/common/boot" },
	{ B_COMMON_FONTS_DIRECTORY, "B_COMMON_FONTS_DIRECTORY", "/boot/common/fonts" },
	{ B_COMMON_LIB_DIRECTORY, "B_COMMON_LIB_DIRECTORY", "/boot/common/lib" },
	{ B_COMMON_SERVERS_DIRECTORY, "B_COMMON_SERVERS_DIRECTORY", "/boot/common/servers" },
	{ B_COMMON_BIN_DIRECTORY, "B_COMMON_BIN_DIRECTORY", "/boot/common/bin" },
	{ B_COMMON_ETC_DIRECTORY, "B_COMMON_ETC_DIRECTORY", "/boot/common/etc" },
	{ B_COMMON_DOCUMENTATION_DIRECTORY, "B_COMMON_DOCUMENTATION_DIRECTORY", "/boot/common/documentation" },
	{ B_COMMON_SETTINGS_DIRECTORY, "B_COMMON_SETTINGS_DIRECTORY", "/boot/common/settings" },
	{ B_COMMON_DEVELOP_DIRECTORY, "B_COMMON_DEVELOP_DIRECTORY", "/boot/develop" },
	{ B_COMMON_LOG_DIRECTORY, "B_COMMON_LOG_DIRECTORY", "/boot/var/log" },
	{ B_COMMON_SPOOL_DIRECTORY, "B_COMMON_SPOOL_DIRECTORY", "/boot/var/spool" },
	{ B_COMMON_TEMP_DIRECTORY, "B_COMMON_TEMP_DIRECTORY", "/boot/var/tmp" },
	{ B_COMMON_VAR_DIRECTORY, "B_COMMON_VAR_DIRECTORY", "/boot/var" },
	{ B_COMMON_TRANSLATORS_DIRECTORY, "B_COMMON_TRANSLATORS_DIRECTORY", "/boot/common/add-ons/Translators" },
	{ B_COMMON_MEDIA_NODES_DIRECTORY, "B_COMMON_MEDIA_NODES_DIRECTORY", "/boot/common/add-ons/media" },
	{ B_COMMON_SOUNDS_DIRECTORY, "B_COMMON_SOUNDS_DIRECTORY", "/boot/common/data/sounds" },
	{ (directory_which)B_COMMON_DATA_DIRECTORY, "B_COMMON_DATA_DIRECTORY", "/boot/common/data" },
	
	{ B_USER_DIRECTORY, "B_USER_DIRECTORY", "/boot/home" },
	{ B_USER_CONFIG_DIRECTORY, "B_USER_CONFIG_DIRECTORY", "/boot/home/config" },
	{ B_USER_ADDONS_DIRECTORY, "B_USER_ADDONS_DIRECTORY", "/boot/home/config/add-ons" },
	{ B_USER_BOOT_DIRECTORY, "B_USER_BOOT_DIRECTORY", "/boot/home/config/boot" },
	{ B_USER_FONTS_DIRECTORY, "B_USER_FONTS_DIRECTORY", "/boot/home/config/fonts" },
	{ B_USER_LIB_DIRECTORY, "B_USER_LIB_DIRECTORY", "/boot/home/config/lib" },
	{ B_USER_SETTINGS_DIRECTORY, "B_USER_SETTINGS_DIRECTORY", "/boot/home/config/settings" },
	{ B_USER_DESKBAR_DIRECTORY, "B_USER_DESKBAR_DIRECTORY", "/boot/home/config/be" },
	{ B_USER_PRINTERS_DIRECTORY, "B_USER_PRINTERS_DIRECTORY", "/boot/home/config/settings/printers" },
	{ B_USER_TRANSLATORS_DIRECTORY, "B_USER_TRANSLATORS_DIRECTORY", "/boot/home/config/add-ons/Translators" },
	{ B_USER_MEDIA_NODES_DIRECTORY, "B_USER_MEDIA_NODES_DIRECTORY", "/boot/home/config/add-ons/media" },
	{ B_USER_SOUNDS_DIRECTORY, "B_USER_SOUNDS_DIRECTORY", "/boot/home/config/data/sounds" },
	{ (directory_which)B_USER_DATA_DIRECTORY, "B_USER_DATA_DIRECTORY", "/boot/home/config/data" },
	{ (directory_which)B_USER_CACHE_DIRECTORY, "B_USER_CACHE_DIRECTORY", "/boot/home/config/cache" },
	
	{ B_APPS_DIRECTORY, "B_APPS_DIRECTORY", "/boot/apps" },
	{ B_PREFERENCES_DIRECTORY, "B_PREFERENCES_DIRECTORY", "/boot/preferences" },
	{ B_UTILITIES_DIRECTORY, "B_UTILITIES_DIRECTORY", "/boot/utilities" },
	{ (directory_which) -1, "", "" }
};

static FindData sHaikuCompatData[] = {
	{ B_BEOS_DIRECTORY, "B_SYSTEM_DIRECTORY", "/boot/system" },
	{ B_BEOS_SYSTEM_DIRECTORY, "B_SYSTEM_SYSTEM_DIRECTORY", "/boot/system" },
	{ B_BEOS_ADDONS_DIRECTORY, "B_SYSTEM_ADDONS_DIRECTORY", "/boot/system/add-ons" },
	{ B_BEOS_BOOT_DIRECTORY, "B_SYSTEM_BOOT_DIRECTORY", "/boot/system/boot" },
	{ B_BEOS_FONTS_DIRECTORY, "B_SYSTEM_FONTS_DIRECTORY", "/boot/system/fonts" },
	{ B_BEOS_LIB_DIRECTORY, "B_SYSTEM_LIB_DIRECTORY", "/boot/system/lib" },
	{ B_BEOS_SERVERS_DIRECTORY, "B_SYSTEM_SERVERS_DIRECTORY", "/boot/system/servers" },
	{ B_BEOS_APPS_DIRECTORY, "B_SYSTEM_APPS_DIRECTORY", "/boot/system/apps" },
	{ B_BEOS_BIN_DIRECTORY, "B_SYSTEM_BIN_DIRECTORY", "/boot/system/bin" },
	{ B_BEOS_ETC_DIRECTORY, "B_SYSTEM_ETC_DIRECTORY", "/boot/system/etc" },
	{ B_BEOS_DOCUMENTATION_DIRECTORY, "B_SYSTEM_DOCUMENTATION_DIRECTORY", "/boot/system/documentation" },
	{ B_BEOS_PREFERENCES_DIRECTORY, "B_SYSTEM_PREFERENCES_DIRECTORY", "/boot/system/preferences" },
	{ B_BEOS_TRANSLATORS_DIRECTORY, "B_SYSTEM_TRANSLATORS_DIRECTORY", "/boot/system/add-ons/Translators" },
	{ B_BEOS_MEDIA_NODES_DIRECTORY, "B_SYSTEM_MEDIA_NODES_DIRECTORY", "/boot/system/add-ons/media" },
	{ B_BEOS_SOUNDS_DIRECTORY, "B_SYSTEM_SOUNDS_DIRECTORY", "/boot/system/data/sounds" },
	{ (directory_which)B_BEOS_DATA_DIRECTORY, "B_SYSTEM_DATA_DIRECTORY", "/boot/system/data" },
	{ (directory_which) -1, "", "" }
};

OSPath::OSPath(void)
{
	SetOS(OS_R5);
}


OSPath::OSPath(ostype_t os)
{
	SetOS(os);
}


OSPath::OSPath(const OSPath &from)
{
	*this = from;
}


OSPath &
OSPath::operator=(const OSPath &from)
{
	fOS = from.fOS;
	fVolume = from.fVolume;
	fVolumeName = from.fVolumeName;
	
	// Normally, just doing a shallow copy would be a bad thing, but the pointer is
	// just a way to choose from the different static FindData objects
	fData = from.fData;
	fDataSize = from.fDataSize;
	return *this;
}


ostype_t
OSPath::GetOS(void) const
{
	return fOS;
}


void
OSPath::SetOS(ostype_t os)
{
	fOS = os;
	switch (fOS)
	{
		case OS_ZETA:
		{
			fData = sZetaFindData;
			break;
		}
		case OS_HAIKU:
		{
printf("OS set to Haiku\n");
			fData = sHaikuFindData;
			break;
		}
		default:
		{
printf("OS set to R5\n");
			fData = sR5FindData;
			break;
		}
	}
	
	fDataSize = 0;
	while (fData[fDataSize].which != -1)
		fDataSize++;
}


void
OSPath::SetVolume(BVolume vol)
{
	BVolume boot;
	BVolumeRoster().GetBootVolume(&boot);
	
	fVolume = vol;
	
	if (vol == boot)
		fVolumeName = "";
	else
	{
		fVolumeName = "";
		char *buffer = fVolumeName.LockBuffer(B_OS_NAME_LENGTH);
		fVolume.GetName(buffer);
		fVolumeName.UnlockBuffer();
	}
}


BVolume
OSPath::GetVolume(void) const
{
	return fVolume;
}


const char *
OSPath::GetVolumeName(void) const
{
	return fVolumeName.String();
}


BString
OSPath::GetPath(int32 dir)
{
if (fOS == OS_HAIKU)
	printf("GetPath OS: Haiku\n");
else
if (fOS == OS_R5)
	printf("GetPath OS: R5\n");
else
if (fOS == OS_ZETA)
	printf("GetPath OS: Zeta\n");
else
	printf("GetPath OS: something else\n");
	BString out;
	for (int32 i = 0; i < fDataSize; i++)
	{
		if (fData[i].which == dir)
		{
			out = fData[i].path;
			if (fVolumeName.CountChars() > 1)
				out.ReplaceFirst("boot",fVolumeName.String());
			break;
		}
	}
	return out;
}


BString
OSPath::GetPath(const char *dir)
{
	BString out;
	if (!dir || strlen(dir) < 1)
		return NULL;
	
	for (int32 i = 0; i < fDataSize; i++)
	{
		if (fData[i].string.ICompare(dir) == 0)
		{
			out = fData[i].path;
			if (fVolumeName.CountChars() > 1)
				out.ReplaceFirst("boot",fVolumeName.String());
			break;
		}
	}
	return out;
}


const char *
OSPath::DirToString(int32 dir)
{
	for (int32 i = 0; i < fDataSize; i++)
	{
		if (fData[i].which == dir)
			return fData[i].path.String();
	}
	return NULL;
}


int32
OSPath::StringToDir(const char *string)
{
	if (!string || strlen(string) < 1)
		return B_BAD_VALUE;
	
	for (int32 i = 0; sHaikuCompatData[i].which != -1; i++)
	{
		if (sHaikuCompatData[i].string.ICompare(string) == 0)
			return sHaikuCompatData[i].which;
	}
	
	for (int32 i = 0; i < fDataSize; i++)
	{
		if (fData[i].string.ICompare(string) == 0)
			return fData[i].which;
	}
	return B_ERROR;
}


BString
OSTypeToString(ostype_t type)
{
	BString out;
	switch (type)
	{
		case OS_R5:
		{
			out = "r5";
			break;
		}
		case OS_ZETA:
		{
			out = "zeta";
			break;
		}
		case OS_HAIKU:
		{
			out = "haiku";
			break;
		}
		default:
			break;
	}
	return out;
}


ostype_t
StringToOSType(const char *str)
{
	BString s(str);
	
	if (s.CountChars() < 1)
		return OS_NONE;
	
	if (s.ICompare("haiku") == 0)
		return OS_HAIKU;
	else if (s.ICompare("r5") == 0)
		return OS_R5;
	else if (s.ICompare("zeta") == 0)
		return OS_ZETA;
	
	return OS_NONE;
}

