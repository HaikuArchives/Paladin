#ifndef _HLibHekkel_h
#define _HLibHekkel_h

#include <BeBuild.h>

/* import-/export-declarations for the libhekkel shared-lib */
#ifdef __HAIKU__
#	define IMPEXP_LIBHEKKEL
#else
#	ifdef H_BUILDING_LIBHEKKEL
#		define IMPEXP_LIBHEKKEL _EXPORT
#	else
#		define IMPEXP_LIBHEKKEL _IMPORT
#	endif
#endif	// __HAIKU__

#endif
