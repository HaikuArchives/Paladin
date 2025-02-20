#include "BuildInfo.h"

BuildInfo::BuildInfo(void)
#if B_HAIKU_VERSION > B_HAIKU_VERSION_1_BETA_5
	:	includeList(20)
#else
	:	includeList(20,true)
#endif
{
}
