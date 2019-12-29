/*
 * Copyright 2019 Adam Fowler <adamfowleruk@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Adam Fowler, adamfowleruk@gmail.com
 */
#include "CompileCommandWriter.h"

#include <sstream>
#include <vector>
#include <iterator> 

#include "CompileCommand.h"

int
CompileCommandWriter::ToJSONFile(std::ostream& oss,std::vector<CompileCommand>& commands)
{
	bool first = true;
	oss << "[" << std::endl;
	// C++11: for (auto& cmd: commands)
	for (std::vector<CompileCommand>::iterator iter = commands.begin();iter < commands.end();iter++)
	{
		CompileCommand cmd = *iter;

		if (!first)
		{
			oss << "," << std::endl;
		}
		oss << "	{  \"directory:\": \"" << cmd.directory << "\"," << std::endl;
		oss << "	   \"command\": \"" << std::string(cmd.command) << "\"," << std::endl;
		oss << "	   \"file\": \"" << cmd.file << "\" }"; // leave for comma or endl
		first = false;
	}
	oss << std::endl << "]" << std::endl; // start with endl to finish last json object line
	return 0;
}
