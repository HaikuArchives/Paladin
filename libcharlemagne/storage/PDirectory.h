#ifndef PDIRECTORY_H
#define PDIRECTORY_H

#include "PNode.h"
#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <Path.h>
#include <SymLink.h>


class BDirectory;


class PDirectory : public PNode
{
public:
							PDirectory(void);
							PDirectory(BMessage *msg);
							PDirectory(const char *name);
							PDirectory(const PDirectory &from);
							~PDirectory(void);

	static	BArchivable *	Instantiate(BMessage *data);

	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;

			BDirectory *	GetBackend(void) const;

protected:
	virtual	void			InitBackend(void);

private:
			void			InitProperties(void);
			void			InitMethods(void);

			BDirectory		*fBackend;

};

#endif

