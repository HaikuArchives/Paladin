#ifndef __PREFS__
#define __PREFS__


bool ReadPrefs(const char *prefsname, void *prefs, size_t sz);
bool WritePrefs(const char *prefsname, void *prefs, size_t sz);



#endif // __PERFS__