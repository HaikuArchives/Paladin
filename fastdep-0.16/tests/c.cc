#define F

#if defined(A)
#include "a.h"
#elif defined(B)
#include "b.h"
#elif defined(F)
#include "f.h"
#endif

#if !defined(G)
#include "g.h"
#endif
