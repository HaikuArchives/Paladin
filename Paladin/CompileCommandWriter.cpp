/*
 * Copyright 2019 Adam Fowler <adamfowleruk@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Adam Fowler, adamfowleruk@gmail.com
 */
#include <sstream>
#include <vector>

#include "CompileCommandWriter.h"
#include "CompileCommand.h"

int
CompileCommandWriter::ToJSONFile(std::ostream& oss,std::vector<CompileCommand>& commands)
{
	bool first = true;
	oss << "[" << std::endl;
	for (auto& command: commands)
	{
		if (!first)
		{
			oss << "," << std::endl;
		}
		oss << "	{  \"directory:\": \"" << command.directory << "\"," << std::endl;
		oss << "	   \"command\": \"" << command.command << "\"," << std::endl;
		oss << "	   \"file\": \"" << command.file << "\" }"; // leave for comma or endl
		first = false;
	}
	oss << std::endl << "]" << std::endl; // start with endl to finish last json object line
	return 0;
}
