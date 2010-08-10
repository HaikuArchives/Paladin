#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <Entry.h>
#include <Message.h>
#include <String.h>


entry_ref			FindFile(entry_ref folder, const char *name);

status_t			RunPipedCommand(const char *command, BString &out,
									bool redirectStdErr,
									bool usePipeHack = false);

#endif
