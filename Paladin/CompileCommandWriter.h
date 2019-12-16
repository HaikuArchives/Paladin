#ifndef COMPILECOMMANDWRITER_H
#define COMPILECOMMANDWRITER_H

#include <sstream>
#include <vector>

class CompileCommand;

/*!
 * This class writes the CompileCommands to a veriety of formats
 */
class CompileCommandWriter
{
public:
	/*!
	 * Writes a set of commands to a file. Replaces existing file. Outputs in JSON array format.
	 */
	static	int				ToJSONFile(std::ostream& oss,std::vector<CompileCommand>& commands);
};

#endif
