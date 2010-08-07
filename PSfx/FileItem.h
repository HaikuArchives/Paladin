#ifndef FILEITEM_H
#define FILEITEM_H

#include <stdio.h>
#include <String.h>

#include "ObjectList.h"
#include "PkgPath.h"

enum
{
	PKG_REPLACE_ASK_ALWAYS = 0,
	PKG_REPLACE_NEVER_REPLACE,
	PKG_REPLACE_RENAME_EXISTING,
	PKG_REPLACE_ASK_NEWER_VERSION,
	PKG_REPLACE_ASK_NEWER_CREATION_DATE,
	PKG_REPLACE_ASK_NEWER_MOD_DATE,
	PKG_REPLACE_REPLACE_NEWER_VERSION,
	PKG_REPLACE_REPLACE_NEWER_CREATION_DATE,
	PKG_REPLACE_REPLACE_NEWER_MOD_DATE,
//	PKG_REPLACE_MERGE_FOLDER,
	PKG_REPLACE_UPGRADE
};

class FileItem
{
public:
							FileItem(void);
							FileItem(const entry_ref &ref);
							~FileItem(void);
			
			const char *	GetName(void) const;
			void			SetName(const char *name);
			
			const char *	GetInstalledName(void) const;
			void			SetInstalledName(const char *name);
			
			const char *	GetResolvedPath(void) const;
			int32			GetPathConstant(void) const;
			void			SetPath(const char *path);
			void			SetPath(int32 path);
			void			ConvertPathFromString(const char *string);
			
			void			SetCategory(const char *cat);
			const char *	GetCategory(void) const;
			
			void			AddGroup(const char *group);
			void			RemoveGroup(const char *group);
			bool			BelongsToGroup(const char *group);
			int32			CountGroups(void) const;
			const char *	GroupAt(int32 index);
			BString			GroupString(void);

			void			AddPlatform(ostype_t plat);
			void			RemovePlatform(ostype_t plat);
			bool			BelongsToPlatform(ostype_t plat);
			int32			CountPlatforms(void) const;
			ostype_t 		PlatformAt(int32 index);
			BString			PlatformString(void);

			void			AddLink(const char *link);
			void			RemoveLink(const char *link);
			bool			HasLink(const char *link);
			int32			CountLinks(void) const;
			const char *	LinkAt(int32 index);
			BString			LinkString(void);
			
			void			SetReplaceMode(const int32 &mode);
			int32			GetReplaceMode(void) const;
			
			BString			MakeInfo(void);
			void			PrintToStream(int8 indent = 0);
private:
			BString *		FindItem(BObjectList<BString> &list,const char *string);
			
			BString					fName,
									fInstalledName;
			
			int32					fReplaceMode;
			
			PkgPath					fPath;
			
			BString					fCategory;
			
			BObjectList<BString>	fGroups,
									fLinks;
									
			BObjectList<ostype_t>	fPlatforms;
};


#endif
