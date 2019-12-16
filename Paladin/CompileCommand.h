/*
 * Copyright 2019 Adam Fowler <adamfowleruk@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Adam Fowler, adamfowleruk@gmail.com
 */
#ifndef COMPILECOMMAND_H
#define COMPILECOMMAND_H

#include <string>

class CompileCommand
{
public:
							CompileCommand(std::string ifile, std::string icommand, std::string idirectory);
							~CompileCommand();
			
		std::string			file;
		std::string			command;
		std::string			directory;
};

#endif
