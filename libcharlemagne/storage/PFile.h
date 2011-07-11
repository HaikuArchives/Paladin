#ifndef PFILE_H
#define PFILE_H

#include "PNode.h"
#include <Directory.h>
#include <File.h>
#include <Path.h>


class BFile;


class PFile : public PNode
{
public:
							PFile(void);
							PFile(BMessage *msg);
							PFile(const char *name);
							PFile(const PFile &from);
							~PFile(void);

	static	BArchivable *	Instantiate(BMessage *data);

	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);

			BFile *	GetBackend(void) const;

protected:
	virtual	void			InitBackend(void);

private:
			void			InitProperties(void);
			void			InitMethods(void);

			BFile		*fBackend;

};

#endif

