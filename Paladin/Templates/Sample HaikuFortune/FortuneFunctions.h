#ifndef FORTUNEFUNCTIONS_H
#define FORTUNEFUNCTIONS_H

#include <List.h>
#include <String.h>

extern BString gFortunePath;

class FortuneAccess
{
public:
				FortuneAccess(void);
				FortuneAccess(const char *folder);
				~FortuneAccess(void);
	
	status_t	SetFolder(const char *folder);
	status_t	GetFortune(BString &target);
	int32		CountFiles(void) const;
	status_t	LastFilename(BString &target);
	
private:
	void		ScanFolder(void);
	void		MakeEmpty(void);
	
	BString		fPath,
				fLastFile;
	BList		fRefList;
};

#endif
