#ifndef GLOBALS_H
#define GLOBALS_H

#include <Menu.h>
#include <MenuItem.h>
#include <String.h>

#include "ObjectList.h"
#include "PackageInfo.h"

extern bool gCommandLineMode;
extern BObjectList<BString> gArgList;
extern int gReturnValue;
extern BString gPlatformName;

#define PFX_MIME_TYPE "text/x-vnd.dw-psfx.project"
#define APP_SIGNATURE "application/x-vnd.dw-PSfx"

void	InitFileTypes(void);
void	InitGlobals(void);

void	GeneratePathMenu(BMenu *menu, int32 msgCmd);

// Items placed into the menu by GeneratePathMenu
class PathMenuItem : public BMenuItem
{
public:
			PathMenuItem(const char *label, const int32 &constant, BMessage *msg);
	int32	GetPath(void) const;
	void	SetPath(const int32 &path);

private:
	int32	fPathConstant;
};




#endif
