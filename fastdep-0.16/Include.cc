#include "Include.h"
#include "FileCache.h"
#include "FileStructure.h"
#include "CompileState.h"
#include "os.h"

Include::Include(FileStructure* aStructure, const std::string& aFilename, bool aSystem)
	: Element(aStructure), Filename(aFilename), System(aSystem)
{
}

Include::Include(const Include& anOther)
	: Element(anOther), Filename(anOther.Filename), System(anOther.System)
{
}

Include::~Include()
{
}

Include& Include::operator=(const Include& anOther)
{
	if (this != &anOther)
	{
		Element::operator=(*this);
		Filename = anOther.Filename;
		System = anOther.System;
	}
	return *this;
}

Element* Include::copy() const
{
	return new Include(*this);
}

void Include::getDependencies(CompileState* aState)
{
	if (Filename.length() > 0)
	{
/*		std::string Fullname;
		if (Filename[0] == cPathSep)
			Fullname = Filename;
		else
			Fullname = getStructure()->getPath()+"/"+Filename; 
		aState->addDependencies(Fullname); */
		FileStructure* S = getCache()->update(getStructure()->getPath(),Filename,System);
		if (S /* && (std::string(S->getFileName(),0,4) != "/usr") */)
		{
			aState->addDependencies(S->getFileName());
			S->getDependencies(aState);
		}
	}
}
