#include "FileUtils.h"

#include <Directory.h>
#include <File.h>
#include <Path.h>
#include <Roster.h>
#include <stdio.h>
#include <stdlib.h>

entry_ref
FindFile(entry_ref folder, const char *name)
{
	entry_ref ref,returnRef;
	if (!folder.name || !name)
		return returnRef;
	
	BDirectory dir(&folder);
	if (dir.InitCheck() != B_OK)
		return returnRef;
	
	dir.Rewind();
	while (dir.GetNextRef(&ref) == B_OK)
	{
		struct stat statData;
		stat(BPath(&ref).Path(),&statData);
		
		// Is a directory?
		if (S_ISDIR(statData.st_mode))
		{
			entry_ref innerref = FindFile(ref,name);
			if (innerref.device != -1 && innerref.directory != -1)
				return innerref;
		}
	}
	
	BEntry entry;
	if (dir.FindEntry(name,&entry) == B_OK)
		entry.GetRef(&returnRef);
	return returnRef;
}


status_t
RunPipedCommand(const char *cmdstr, BString &out, bool redirectStdErr,
				bool usePipeHack)
{
	if (!cmdstr)
		return B_BAD_DATA;
	
	BString command(cmdstr);
	out = "";
	
	if (usePipeHack)
	{
		BString tmpfilename("/tmp/Paladin.build.tmp.");
		tmpfilename << real_time_clock_usecs();
		
		command << " > " << tmpfilename;
		
		if (redirectStdErr)
			command << " 2>&1";
		system(command.String());
		
		BFile file(tmpfilename.String(), B_READ_ONLY);
		if (file.InitCheck() != B_OK)
			return file.InitCheck();
		
		char buffer[1024];
		while (file.Read(buffer, 1024) > 0)
			out << buffer;
		
		file.Unset();
		BEntry(tmpfilename.String()).Remove();
	}
	else
	{
		if (redirectStdErr)
			command << " 2>&1";
		
		FILE *fd = popen(cmdstr,"r");
		if (!fd)
			return B_BUSTED_PIPE;
		
		char buffer[1024];
		BString errmsg;
		while (fgets(buffer,1024,fd))
			out += buffer;
		pclose(fd);
	}
	
	return B_OK;
}


