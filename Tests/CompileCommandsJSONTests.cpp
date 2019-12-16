#include <UnitTest++/UnitTest++.h>

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include "CompileCommand.h"
#include "CompileCommandWriter.h"

SUITE(CompileCommandsJSON)
{

	TEST(CommandSetters) 
	{
		std::string file("myfile.c");
		std::string cc("g++ -o objfolder/myfile.o myfile.cpp");
		std::string folder("objfolder");
		CompileCommand c(file,cc,folder);
		CHECK_EQUAL(c.file, file);
		CHECK_EQUAL(c.command, cc);
		CHECK_EQUAL(c.directory,folder);
	}
	
	TEST(WriteJSON)
	{
		CompileCommand c1("a.c","g++ a.c","objfolder");
		CompileCommand c2("b.c","g++ b.c","objfolder");
		CompileCommand c3("b.c","g++ b.c","objfolder");
		std::vector<CompileCommand> commands = std::vector<CompileCommand>{c1,c2,c3};
		
		std::ostringstream oss; // in memory for this test
		CompileCommandWriter::ToJSONFile(oss,commands);
		std::string output(oss.str());
		//std::cout << "Output string:-" << std::endl << output << std::endl;
		//printf("Output length: %lu\n",output.length());
		CHECK_EQUAL(237,output.length());
	}

}
