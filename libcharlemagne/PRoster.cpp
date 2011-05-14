#include "PRoster.h"

int32_t PRosterActivateApp(void *pobject, PArgList *in, PArgList *out);
int32_t PRosterAddToRecentDocuments(void *pobject, PArgList *in, PArgList *out);
int32_t PRosterAddToRecentFolders(void *pobject, PArgList *in, PArgList *out);
int32_t PRosterBroadcast(void *pobject, PArgList *in, PArgList *out);
int32_t PRosterFindApp(void *pobject, PArgList *in, PArgList *out);
int32_t PRosterGetActiveAppInfo(void *pobject, PArgList *in, PArgList *out);
int32_t PRosterGetAppInfo(void *pobject, PArgList *in, PArgList *out);
int32_t PRosterGetAppList(void *pobject, PArgList *in, PArgList *out);
int32_t PRosterGetRecentApps(void *pobject, PArgList *in, PArgList *out);
int32_t PRosterGetRecentDocuments(void *pobject, PArgList *in, PArgList *out);
int32_t PRosterGetRecentFolders(void *pobject, PArgList *in, PArgList *out);
int32_t PRosterGetRunningAppInfo(void *pobject, PArgList *in, PArgList *out);
int32_t PRosterIsRunning(void *pobject, PArgList *in, PArgList *out);
int32_t PRosterLaunch(void *pobject, PArgList *in, PArgList *out);
int32_t PRosterStartWatching(void *pobject, PArgList *in, PArgList *out);
int32_t PRosterStopWatching(void *pobject, PArgList *in, PArgList *out);
int32_t PRosterTeamFor(void *pobject, PArgList *in, PArgList *out);

PRoster::PRoster(void)
{
	fType = "PRoster";
	fFriendlyType = "Roster";
	AddInterface("PRoster");
	
	InitMethods();
	InitBackend();
}


PRoster::PRoster(BMessage *msg)
	:	PObject(msg)
{
	fType = "PRoster";
	fFriendlyType = "Roster";
	AddInterface("PRoster");
	InitBackend();
}


PRoster::PRoster(const char *name)
	:	PObject(name)
{
	fType = "PRoster";
	fFriendlyType = "Roster";
	AddInterface("PRoster");
	InitMethods();
	InitBackend();
}


PRoster::PRoster(const PRoster &from)
	:	PObject(from)
{
	fType = "PRoster";
	fFriendlyType = "Roster";
	AddInterface("PRoster");
	InitMethods();
	InitBackend();
}


PRoster::~PRoster(void)
{
}


BArchivable *
PRoster::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PRoster"))
		return new PRoster(data);

	return NULL;
}


PObject *
PRoster::Create(void)
{
	return new PRoster();
}


PObject *
PRoster::Duplicate(void) const
{
	return new PRoster(*this);
}


void
PRoster::InitBackend(void)
{
	StringValue sv("A application roster service object");
	SetProperty("Description",&sv);
}


void
PRoster::InitMethods(void)
{
/*
			ActivateApp
			AddToRecentDocuments
			AddToRecentFolders
			Broadcast
			FindApp
			GetActiveAppInfo
			GetRecentApps
			GetRecentDocuments
			GetRecentFolders
			GetRunningAppInfo
			Launch
			StartWatching
			StopWatching
*/
	PMethodInterface pmi;
	
	// TODO: This needs to be finished once the ListValue interface is complete for
			argument passing
	pmi.AddArgument("mimetype", PARG_STRING, "Optional MIME type of a running app", PMIFLAG_OPTIONAL);
	pmi.AddReturnValue("list", PARG_LIST, "List of team_id values");
	AddMethod(new PMethod("GetAppList", PRosterGetAppList, &pmi))
	pmi.MakeEmpty();
	
	pmi.AddArgument("mimetype", PARG_STRING, "MIME type of the desired app", PMIFLAG_OPTIONAL);
	pmi.AddArgument("path", PARG_STRING, "path of the desired app", PMIFLAG_OPTIONAL);
	AddMethod(new PMethod("GetAppInfo", PRosterGetAppInfo, &pmi))
	pmi.MakeEmpty();
	
	AddMethod(new PMethod("GetActiveAppInfo", PRosterGetActiveAppInfo, &pmi))
	pmi.MakeEmpty();
	
	pmi.AddArgument("mimetype", PARG_STRING, "MIME type of the possibly running app", PMIFLAG_OPTIONAL);
	pmi.AddArgument("path", PARG_STRING, "path of the possibly running app", PMIFLAG_OPTIONAL);
	AddMethod(new PMethod("IsRunning", PRosterIsRunning, &pmi))
	pmi.MakeEmpty();
	
	pmi.AddArgument("mimetype", PARG_STRING, "MIME type of the desired app", PMIFLAG_OPTIONAL);
	pmi.AddArgument("path", PARG_STRING, "path of the desired app", PMIFLAG_OPTIONAL);
	AddMethod(new PMethod("TeamFor", PRosterTeamFor, &pmi))
	pmi.MakeEmpty();
	
}


int32_t
PRosterActivateApp(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PRosterAddToRecentDocuments(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PRosterAddToRecentFolders(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PRosterBroadcast(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PRosterFindApp(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PRosterGetActiveAppInfo(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PRosterGetAppInfo(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PRosterGetAppList(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PRosterGetRecentApps(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PRosterGetRecentDocuments(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PRosterGetRecentFolders(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PRosterGetRunningAppInfo(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PRosterIsRunning(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PRosterLaunch(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PRosterStartWatching(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PRosterStopWatching(void *pobject, PArgList *in, PArgList *out)
{
}


int32_t
PRosterTeamFor(void *pobject, PArgList *in, PArgList *out)
{
}


