#ifndef DEPITEM_H
#define DEPITEM_H

#include <String.h>
#include <stdio.h>

#include "PkgPath.h"

enum
{
	DEP_FILE = 0,
	DEP_LIBRARY
};

class DepItem
{
public:
							DepItem(void);
						
			const char *	GetName(void) const;
			void			SetName(const char *name);
			
			const char *	GetFileName(void) const;
			void			SetFileName(const char *name);
			
			const char *	GetResolvedPath(void) const;
			int32			GetPathConstant(void) const;
			void			SetPath(const char *path);
			void			SetPath(int32 path);
			void			ConvertPathFromString(const char *string);

			void			SetURL(const char *url);
			const char *	GetURL(void) const;
			
			void			SetType(const char *type);
			void			SetType(int8 type);
			int8			GetType(void) const;
			
			BString			MakeInfo(void);
			void			PrintToStream(int8 indent = 0);
private:
			BString			fName,
							fFileName,
							fURL;
			
			PkgPath			fPath;
							
			int8			fType;
};

#endif
