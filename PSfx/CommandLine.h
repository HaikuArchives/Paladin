#ifndef COMMANDLINE_H
#define COMMANDLINE_H

bool ProcessArgs(int argc, char **argv);
void DoCommandLine(void);
void BuildPackage(const char *pfxpath, const char *pkgpath, const char *platformName);

#endif
