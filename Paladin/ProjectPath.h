#ifndef PROJECTPATH_H
#define PROJECTPATH_H

#include <Entry.h>
#include <String.h>

class ProjectPath
{
public:
					ProjectPath(const char *base, const char *path = NULL);
					ProjectPath(const ProjectPath &from);
					ProjectPath(void);
	ProjectPath &	operator=(const ProjectPath &path);
	bool			operator==(const ProjectPath &path);
	bool			operator!=(const ProjectPath &path);
				
	void			Set(const char *base, const char *path);
	
	void			SetBase(const char *base);
	BString			GetBase(void) const;
	
	void			SetPath(const char *path);
	BString			GetPath(void) const;
				
	BString			Absolute(void) const;
	BString			Relative(void) const;
	
private:
	BString		fBase,
				fPath;
};

#endif
