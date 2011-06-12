#ifndef SOURCETYPE_H
#define SOURCETYPE_H

#include <Entry.h>
#include <String.h>
#include <View.h>

class SourceFile;
class SourceOptionView;

// Option flags for SourceType::CreateSourceFile
enum
{
	SOURCEFILE_PAIR = 0x00000001	// Create a source file and a partner file, if appropriate
};

class SourceType
{
public:
	virtual				~SourceType(void);
	virtual	int32		CountExtensions(void) const;
	virtual	BString		GetExtension(const int32 &index);
	virtual	bool		HasExtension(const char *ext);
	
	virtual	SourceFile *		CreateSourceFileItem(const char *path);
	virtual	entry_ref	CreateSourceFile(const char *folder, const char *name,
												uint32 options = 0);
	virtual	SourceOptionView *	CreateOptionView(void);
	
	virtual	BString		GetName(void) const;
	
protected:
						// This is not a directly usable class
						SourceType(void);
};


class SourceOptionView : public BView
{
						SourceOptionView(BRect frame, const char *name,
										int32 resize, int32 flags);
	virtual				~SourceOptionView(void);
	
	virtual	const char *GetOptionString(void);
};


#endif
