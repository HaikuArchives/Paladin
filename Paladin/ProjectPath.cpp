#include "ProjectPath.h"

ProjectPath::ProjectPath(const char *base, const char *path)
{
	Set(base, path);
}


ProjectPath::ProjectPath(void)
{
}


ProjectPath::ProjectPath(const ProjectPath &from)
{
	*this = from;
}


ProjectPath &
ProjectPath::operator=(const ProjectPath &from)
{
	fBase = from.fBase;
	fPath = from.fPath;
	return *this;
}


bool
ProjectPath::operator==(const ProjectPath &from)
{
	return (fBase.Compare(from.fBase) == 0 && fPath.Compare(from.fPath) == 0);
}


bool
ProjectPath::operator!=(const ProjectPath &path)
{
	return !(*this == path);
}


void
ProjectPath::Set(const char *base, const char *path)
{
	SetBase(base);
	SetPath(path);
}


void
ProjectPath::SetBase(const char *base)
{
	fBase = base;
	if (fBase.CountChars() > 0 && fBase.ByteAt(fBase.CountChars() - 1) != '/')
		fBase << "/";
}


BString
ProjectPath::GetBase(void) const
{
	return fBase;
}


void
ProjectPath::SetPath(const char *path)
{
	fPath = path;

	// Check if path is subpath of fBase
	if (fPath.FindFirst(fBase) == 0)
	{
		// Remove fBase from fPath...
		fPath.RemoveFirst(fBase);

		// and make sure path does not start with "/" now
		if (fPath.FindFirst("/") == 0)
			fPath.RemoveFirst("/");
	}
}


BString
ProjectPath::GetPath(void) const
{
	return fPath;
}


BString
ProjectPath::Absolute(void) const
{
	BString out(fBase);
	out << fPath;
	return out;
}


BString
ProjectPath::Relative(void) const
{
	return fPath;
}

