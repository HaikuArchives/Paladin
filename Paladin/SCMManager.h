#ifndef SCMMANAGER_H
#define SCMMANAGER_H

#include "SourceControl.h"
#include "Project.h"

SourceControl *	GetSCM(const scm_t &type);
bool			HaveSCM(const scm_t &type);

#endif
