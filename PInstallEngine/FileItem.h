#ifndef FILEITEM_H
#define FILEITEM_H

#include <stdio.h>
#include <String.h>

#include "ObjectList.h"
#include "PkgPath.h"

class FileItem
{
public:
							FileItem(void);
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

			void			AddPlatform(ostype_t plat);
			void			RemovePlatform(ostype_t plat);
			bool			BelongsToPlatform(ostype_t plat);
			int32			CountPlatforms(void) const;
			ostype_t 		PlatformAt(int32 index);

			void			AddLink(const char *link);
			void			RemoveLink(const char *link);
			bool			HasLink(const char *link);
			int32			CountLinks(void) const;
			const char *	LinkAt(int32 index);
			
			BString			MakeInfo(void);
			void			PrintToStream(int8 indent = 0);
private:
			BString *		FindItem(BObjectList<BString> &list,const char *string);
			
			BString					fName,
									fInstalledName;
			
			PkgPath					fPath;
			
			BString					fCategory;
			
			BObjectList<BString>	fGroups,
									fLinks;
									
			BObjectList<ostype_t>	fPlatforms;
};


#endif
