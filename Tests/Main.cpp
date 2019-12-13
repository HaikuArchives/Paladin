#include "UnitTest++/UnitTest++.h"

TEST(Sanity) 
{
   CHECK_EQUAL(1, 1);
}

int main(int, const char *[])
{
   return UnitTest::RunAllTests();
}
