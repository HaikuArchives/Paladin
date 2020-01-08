/*
 * Copyright 2019 Adam Fowler <adamfowleruk@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Adam Fowler, adamfowleruk@gmail.com
 */
#include <string>
#include <iostream>

#include "CompileCommand.h"


CompileCommand::CompileCommand()
	:
	file(),
	command(),
	directory()
{
	//std::cout << "CompileCommand empty constructor" << std::endl;
}
CompileCommand::CompileCommand(std::string ifile, std::string icommand, std::string idirectory)
	:
	file(ifile),
	command(icommand),
	directory(idirectory)
{
	//std::cout << "CompileCommand basic constructor" << std::endl;
}

CompileCommand::CompileCommand(const CompileCommand& orig) 
	:
	file(orig.file),
	command(orig.command),
	directory(orig.directory)
{
	//std::cout << "CompileCommand copy constructor" << std::endl;
}

CompileCommand::~CompileCommand()
{
}

