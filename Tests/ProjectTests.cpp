#include "UnitTest++/UnitTest++.h"
#include "../Paladin/Project.h"

SUITE(Project)
{

	TEST(BlankProject) 
	{
		Project p("Blank","");
		CHECK_EQUAL(p.GetName(), "Blank");
	}

}
