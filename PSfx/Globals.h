#ifndef GLOBALS_H
#define GLOBALS_H

#include <String.h>

#include "ObjectList.h"
#include "PackageInfo.h"

extern bool gCommandLineMode;
extern BObjectList<BString> gArgList;
extern int gReturnValue;

#endif
