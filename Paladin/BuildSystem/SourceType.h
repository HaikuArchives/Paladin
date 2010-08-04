#ifndef SOURCETYPE_H
#define SOURCETYPE_H

#include <String.h>
#include <View.h>

class SourceFile;
class SourceOptionView;

class SourceType
{
public:
	virtual				~SourceType(void);
	virtual	int32		CountExtensions(void) const;
	virtual	BString		GetExtension(const int32 &index);
			bool		HasExtension(const char *ext);
	
	virtual	SourceFile *		CreateSourceFile(const char *path);
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
