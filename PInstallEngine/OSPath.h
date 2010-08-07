#ifndef OSPATH_H
#define OSPATH_H

#include <FindDirectory.h>
#include <String.h>
#include <Volume.h>

#ifndef __HAIKU__

enum
{
	B_BEOS_DATA_DIRECTORY = B_BEOS_SOUNDS_DIRECTORY + 1,
	B_COMMON_DATA_DIRECTORY = B_COMMON_SOUNDS_DIRECTORY + 1,
	B_USER_DATA_DIRECTORY = B_USER_SOUNDS_DIRECTORY + 1,
	B_USER_CACHE_DIRECTORY = B_USER_DATA_DIRECTORY + 1,
	
	
	B_SYSTEM_DIRECTORY = B_BEOS_DIRECTORY,
	B_SYSTEM_SYSTEM_DIRECTORY = B_BEOS_SYSTEM_DIRECTORY,
	B_SYSTEM_ADDONS_DIRECTORY = B_BEOS_ADDONS_DIRECTORY,
	B_SYSTEM_BOOT_DIRECTORY = B_BEOS_BOOT_DIRECTORY,
	B_SYSTEM_FONTS_DIRECTORY = B_BEOS_FONTS_DIRECTORY,
	B_SYSTEM_LIB_DIRECTORY = B_BEOS_LIB_DIRECTORY,
 	B_SYSTEM_SERVERS_DIRECTORY = B_BEOS_SERVERS_DIRECTORY,
	B_SYSTEM_APPS_DIRECTORY = B_BEOS_APPS_DIRECTORY,
	B_SYSTEM_BIN_DIRECTORY = B_BEOS_BIN_DIRECTORY,
	B_SYSTEM_ETC_DIRECTORY = B_BEOS_ETC_DIRECTORY,
	B_SYSTEM_DOCUMENTATION_DIRECTORY = B_BEOS_DOCUMENTATION_DIRECTORY,
	B_SYSTEM_PREFERENCES_DIRECTORY = B_BEOS_PREFERENCES_DIRECTORY,
	B_SYSTEM_TRANSLATORS_DIRECTORY = B_BEOS_TRANSLATORS_DIRECTORY,
	B_SYSTEM_MEDIA_NODES_DIRECTORY = B_BEOS_MEDIA_NODES_DIRECTORY,
	B_SYSTEM_SOUNDS_DIRECTORY = B_BEOS_SOUNDS_DIRECTORY,
	B_SYSTEM_DATA_DIRECTORY = B_BEOS_DATA_DIRECTORY
};

#endif

typedef enum
{
	OS_NONE = -1,
	OS_ALL = 0,
	OS_R5,
	OS_ZETA,
	OS_HAIKU,
	OS_HAIKU_GCC4
} ostype_t;

BString OSTypeToString(ostype_t type);
ostype_t StringToOSType(const char *str);

typedef struct
{
	directory_which which;
	BString			string;
	BString			path;
}  FindData;

class OSPath
{
public:
							OSPath(void);
							OSPath(ostype_t os);
							OSPath(const OSPath &from);
			OSPath &		operator=(const OSPath &from);
							
			ostype_t		GetOS(void) const;
			void			SetOS(ostype_t os);
			
			void			SetVolume(BVolume vol);
			BVolume			GetVolume(void) const;
			const char *	GetVolumeName(void) const;
			
			BString			GetPath(int32 dir);
			BString			GetPath(const char *dir);
			
			const char *	DirToString(int32 dir);
			int32			StringToDir(const char *string);
			
private:
			ostype_t		fOS;
			BVolume			fVolume;
			BString			fVolumeName;
			
			FindData		*fData;
			int32			fDataSize;
};

#endif
