#ifndef INCLUDE_H_
#define INCLUDE_H_

#include "Element.h"

#include <string>

class FileStructure;
class CompileState;

class Include : public Element
{
	public:
		Include(FileStructure* aStructure, const std::string& aFilename, bool aSystem);
		Include(const Include& anOther);
		virtual ~Include();

		Include& operator=(const Include& anOther);

		virtual Element* copy() const;
		virtual void getDependencies(CompileState* aState);
	private:
		std::string Filename;
		bool System;
};

#endif

