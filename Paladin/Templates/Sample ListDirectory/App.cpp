#include <Directory.h>
#include <Entry.h>
#include <Path.h>
#include <stdio.h>
#include <String.h>

// It's better to use constant global integers instead of #defines because constants
// provide strong typing and don't lead to weird errors like #defines can.
const uint16 BYTES_PER_KB = 1024;
const uint32 BYTES_PER_MB = 1048576;
const uint64 BYTES_PER_GB = 1099511627776ULL;

int		ListDirectory(const entry_ref &dirRef);
BString	MakeSizeString(const uint64 &size);


int
main(int argc, char **argv)
{
	// We want to require one argument in addition to the program name when invoked
	// from the command line.
	if (argc != 2)
	{
		printf("Usage: listdir <path>\n");
		return 0;
	}
	
	// Here we'll do some sanity checks to make sure that the path we were given
	// actually exists and it's not a file.
	
	BEntry entry(argv[1]);
	if (!entry.Exists())
	{
		printf("%s does not exist\n",argv[1]);
		return 1;
	}
	
	if (!entry.IsDirectory())
	{
		printf("%s is not a directory\n",argv[1]);
		return 1;
	}
	
	// An entry_ref is a typedef'ed structure which points to a file, directory, or
	// symlink on disk. The entry must actually exist, but unlike a BFile or BEntry, it
	// doesn't use up a file handle.
	entry_ref ref;
	entry.GetRef(&ref);
	return ListDirectory(ref);
}


int
ListDirectory(const entry_ref &dirRef)
{
	// This function does all the work of the program
	
	BDirectory dir(&dirRef);
	if (dir.InitCheck() != B_OK)
	{
		printf("Couldn't read directory %s\n",dirRef.name);
		return 1;
	}
	
	// First thing we'll do is quickly scan the directory to find the length of the
	// longest entry name. This makes it possible to left justify the file sizes
	int32 entryCount = 0;
	uint32 maxChars = 0;
	entry_ref ref;
	
	// Calling Rewind() moves the BDirectory's index to the beginning of the list.
	dir.Rewind();
	
	// GetNextRef() will return B_ERROR when the BDirectory has gotten to the end of
	// its list of entries.
	while (dir.GetNextRef(&ref) == B_OK)
	{
		if (ref.name)
			maxChars = MAX(maxChars,strlen(ref.name));
	}
	maxChars++;
	char padding[maxChars];
	
	BEntry entry;
	dir.Rewind();
	
	// Here we'll call GetNextEntry() instead of GetNextRef() because a BEntry will
	// enable us to get certain information about each entry, such as the entry's size.
	// Also, because it inherits from BStatable, we can differentiate between
	// directories and files with just one function call.
	while (dir.GetNextEntry(&entry) == B_OK)
	{
		char name[B_FILE_NAME_LENGTH];
		entry.GetName(name);
		
		BString formatString;
		formatString << "%s";
		
		unsigned int length = strlen(name);
		if (length < maxChars)
		{
			uint32 padLength = maxChars - length;
			memset(padding, ' ', padLength);
			padding[padLength - 1] = '\0';
			formatString << padding;
		}
		
		if (entry.IsDirectory())
		{
			// We'll display the "size" of a directory by listing how many
			// entries it contains
			BDirectory subdir(&entry);
			formatString << "\t" << subdir.CountEntries() << " items";
		}
		else
		{
			off_t fileSize;
			entry.GetSize(&fileSize);
			formatString << "\t" << MakeSizeString(fileSize);
		}
		formatString << "\n";
		printf(formatString.String(),name);
		entryCount++;
	}
	printf("%ld entries\n",entryCount);
	return 0;
}


BString
MakeSizeString(const uint64 &size)
{
	// This function just makes converts the raw byte counts provided by
	// BEntry's GetSize() method into something more people-friendly.
	BString sizeString;
	if (size < BYTES_PER_KB)
		sizeString << size << " bytes";
	else if (size < BYTES_PER_MB)
		sizeString << (float(size) / float(BYTES_PER_KB)) << " KB";
	else if (size < BYTES_PER_GB)
		sizeString << (float(size) / float(BYTES_PER_MB)) << " MB";
	else
		sizeString << (float(size) / float(BYTES_PER_GB)) << " GB";
	return sizeString;
}

