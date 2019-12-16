/*
 * Copyright 2019 Adam Fowler <adamfowleruk@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Adam Fowler, adamfowleruk@gmail.com
 */
#include <UnitTest++/UnitTest++.h>
#include "../Paladin/Project.h"

SUITE(Project)
{

	TEST(BlankProject) 
	{
		Project p("Blank","");
		CHECK_EQUAL(p.GetName(), "Blank");
		CHECK_EQUAL(p.GetTargetName(), "");
		CHECK_EQUAL(p.CountFiles(),0);
		CHECK_EQUAL(p.CountDirtyFiles(),0);
		CHECK_EQUAL(p.CountLocalIncludes(),0);
		CHECK_EQUAL(p.CountSystemIncludes(),0);
		CHECK_EQUAL(p.CountLibraries(),0);
		CHECK_EQUAL(p.CountGroups(),0);
		CHECK_EQUAL(p.GetRunArgs(),"");
	}
}
