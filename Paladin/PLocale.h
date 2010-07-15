#ifndef PLOCALE_H
#define PLOCALE_H

// This is for massaging the localization stuff

#if defined(__HAIKU__) || defined(__ZETA__)

#ifdef __HAIKU__
//#include <Catalog.h>
//#include <Locale.h>
//#define TR_CONTEXT "Paladin"
	#define TR(x)  x
#endif

#ifdef __ZETA__
#include <locale/Locale.h>
#define TR(x)  be_locale.GetString(x)
#endif

#else
	// Not Haiku or Zeta -- must be R5. We don't do translations
	// under R5, but we do want to make it at least compile
	#define TR(x)  x
#endif

#endif
