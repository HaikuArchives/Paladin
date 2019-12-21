#ifndef PROJECT_BUILDER_H
#define PROJECT_BUILDER_H

#include <vector>

#include <Locker.h>
#include <Messenger.h>
#include <String.h>

#include "CompileCommand.h"
#include "ErrorParser.h"

enum
{
	POSTBUILD_NOTHING = 0,
	POSTBUILD_RUN,
	POSTBUILD_DEBUG,
	POSTBUILD_RUN_IN_TERMINAL
};

enum
{
	M_EXAMINING_FILE = 'exfl',
	M_BUILDING_FILE = 'blfl',
	M_BUILDING_DONE = 'bldn',
	M_LINKING_PROJECT = 'bllp',
	M_UPDATING_RESOURCES = 'blur',
	M_DOING_POSTBUILD = 'bldp',
	M_BUILD_MESSAGES = 'blms',
	M_BUILD_WARNINGS = 'blwr',
	M_BUILD_FAILURE = 'blfa',
	M_BUILD_SUCCESS = 'blsc',
	M_FILE_NEEDS_BUILD = 'fnbl',
	M_BUILD_MONITOR = 'blmn'
};

class Project;

class ThreadManager
{
public:
						ThreadManager(uint8 max = 32);
						~ThreadManager(void);
						
	thread_id			SpawnThread(thread_func func, void *data);
	void				RemoveThread(thread_id tid);
	
	uint8				CountRunningThreads(void);
	void				QuitAllThreads(void);
	void				KillAllThreads(bigtime_t quit_timeout = 0);
	
	bool				ThreadCheckQuit(void);
	
private:
	int8				FindFreeSlot(void);
	
	BLocker				fLock;
	uint8				fMaxThreads;
	uint8				fThreadCount;
	bool				fQuitFlag;
	thread_id			*fThreadArray;
};

class ProjectBuilder : public BLocker
{
public:
						ProjectBuilder(void);
						ProjectBuilder(const BMessenger &target);
						~ProjectBuilder(void);
						
			void		BuildProject(Project *proj, int32 postbuild);
			void		QuitBuild(void);
			bool		IsBuilding(void);
			
private:
			void		DoBuild(void);
			void		DoPostBuild(void);
			void		SendErrorMessage(ErrorList &list);
	static	int32		BuildThread(void *data);
	
	BMessenger			fMsgr;
	Project				*fProject;
	bool				fIsLinking;
	bool				fIsBuilding;
	int32				fTotalFilesToBuild;
	int32				fTotalFilesBuilt;
	
	int32				fPostBuildAction;
	
	ThreadManager		fManager;
	
	std::vector<CompileCommand>	fCommands;
};

#endif
