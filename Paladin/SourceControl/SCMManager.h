#ifndef SCMMANAGER_H
#define SCMMANAGER_H

#include "SourceControl.h"
#include "Project.h"

SourceControl *	GetSCM(const scm_t &type);
scm_t			DetectSCM(const char *path);
bool			HaveSCM(const scm_t &type);
BString			SCM2LongName(const scm_t &type);
BString			SCM2ShortName(const scm_t &type);

#endif
