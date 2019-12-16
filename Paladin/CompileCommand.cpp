/*
 * Copyright 2019 Adam Fowler <adamfowleruk@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Adam Fowler, adamfowleruk@gmail.com
 */
#include <string>

#include "CompileCommand.h"

CompileCommand::CompileCommand(std::string ifile, std::string icommand, std::string idirectory)
	:
	file(ifile),
	command(icommand),
	directory(idirectory)
{
}

CompileCommand::~CompileCommand()
{
}

