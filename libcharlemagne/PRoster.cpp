#include "PRoster.h"

int32_t PRosterActivateApp(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PRosterAddToRecentDocuments(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PRosterAddToRecentFolders(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PRosterBroadcast(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PRosterFindApp(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PRosterGetActiveAppInfo(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PRosterGetAppInfo(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PRosterGetAppList(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PRosterGetRecentApps(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PRosterGetRecentDocuments(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PRosterGetRecentFolders(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PRosterGetRunningAppInfo(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PRosterIsRunning(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PRosterLaunch(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);
int32_t PRosterTeamFor(void *pobject, PArgList *in, PArgList *out, void *ptr = NULL);

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
	PMethodInterface pmi;
	
	pmi.AddArg("team", PARG_INT32, "Team ID of the app to activate");
	pmi.AddReturnValue("status", PARG_INT32, "Error status of the call");
	AddMethod(new PMethod("ActivateApp", PRosterActivateApp, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("list", PARG_LIST, "Argument list containing the message data");
	pmi.AddReturnValue("status", PARG_INT32, "Error status of the call");
	AddMethod(new PMethod("Broadcast", PRosterBroadcast, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("signature", PARG_STRING, "MIME type of the desired app", PMIFLAG_OPTIONAL);
	pmi.AddArg("path", PARG_STRING, "path of the desired app", PMIFLAG_OPTIONAL);
	pmi.AddReturnValue("list", PARG_LIST, "List containing entry_ref information");
	pmi.AddReturnValue("status", PARG_INT32, "Error status of the call");
	AddMethod(new PMethod("FindApp", PRosterFindApp, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("signature", PARG_STRING, "Optional MIME type of a running app", PMIFLAG_OPTIONAL);
	pmi.AddReturnValue("list", PARG_LIST, "List of team_id values");
	AddMethod(new PMethod("GetAppList", PRosterGetAppList, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("signature", PARG_STRING, "MIME type of the desired app", PMIFLAG_OPTIONAL);
	pmi.AddArg("path", PARG_STRING, "path of the desired app", PMIFLAG_OPTIONAL);
	pmi.AddReturnValue("list", PARG_LIST, "List of lists containing the app_info information");
	pmi.AddReturnValue("status", PARG_INT32, "Error status of the call");
	AddMethod(new PMethod("GetAppInfo", PRosterGetAppInfo, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddReturnValue("list", PARG_LIST, "List containing the app_info information");
	pmi.AddReturnValue("status", PARG_INT32, "Error status of the call");
	AddMethod(new PMethod("GetActiveAppInfo", PRosterGetActiveAppInfo, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("signature", PARG_STRING, "MIME type of the possibly running app", PMIFLAG_OPTIONAL);
	pmi.AddArg("path", PARG_STRING, "path of the possibly running app", PMIFLAG_OPTIONAL);
	pmi.AddReturnValue("value", PARG_BOOL, "True if the specified app is running");
	AddMethod(new PMethod("IsRunning", PRosterIsRunning, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("signature", PARG_STRING, "MIME type of the desired app", PMIFLAG_OPTIONAL);
	pmi.AddArg("path", PARG_STRING, "path of the desired app", PMIFLAG_OPTIONAL);
	pmi.AddReturnValue("value", PARG_INT32, "team_id of the specified app");
	AddMethod(new PMethod("TeamFor", PRosterTeamFor, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("path", PARG_STRING, "Path of the document to add to the recent list.");
	pmi.AddArg("signature", PARG_STRING, "MIME type of the app used to open the document.", PMIFLAG_OPTIONAL);
	AddMethod(new PMethod("AddToRecentDocuments", PRosterAddToRecentDocuments, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("path", PARG_STRING, "Path of the folder to add to the recent list.");
	pmi.AddArg("signature", PARG_STRING, "MIME type of the app used to open the folder.", PMIFLAG_OPTIONAL);
	AddMethod(new PMethod("AddToRecentDocuments", PRosterAddToRecentFolders, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("maxcount", PARG_INT32, "The maximum number of recent apps to retrieve.");
	pmi.AddReturnValue("list", PARG_LIST, "List of paths to recently-run applications");
	AddMethod(new PMethod("GetRecentApps", PRosterGetRecentApps, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("maxcount", PARG_INT32, "The maximum number of recent documents to retrieve.");
	pmi.AddArg("types", PARG_LIST, "List of mimetypes desired.", PMIFLAG_OPTIONAL);
	pmi.AddArg("maxtypes", PARG_INT32, "Maximum number of documents of the specified type "
										"to retrieve.", PMIFLAG_OPTIONAL);
	pmi.AddArg("signature", PARG_STRING, "MIME type of the application used to open the "
										"specified documents.",	PMIFLAG_OPTIONAL);
	pmi.AddReturnValue("list", PARG_LIST, "List of paths to recently-used documents");
	AddMethod(new PMethod("GetRecentDocuments", PRosterGetRecentDocuments, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("maxcount", PARG_INT32, "The maximum number of recent documents to retrieve.");
	pmi.AddArg("signature", PARG_STRING, "MIME type of the application used to open the "
										"specified documents.",	PMIFLAG_OPTIONAL);
	pmi.AddReturnValue("list", PARG_LIST, "List of paths to recently-used folders");
	AddMethod(new PMethod("GetRecentFolders", PRosterGetRecentFolders, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("team", PARG_INT32, "TeamID of the desired application");
	pmi.AddReturnValue("list", PARG_LIST, "List containing the app_info information");
	pmi.AddReturnValue("status", PARG_INT32, "Error status of the call");
	AddMethod(new PMethod("GetRunningAppInfo", PRosterGetRunningAppInfo, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("signature", PARG_STRING, "MIME signature of the application to launch");
	pmi.AddArg("arglist", PARG_LIST, "List of string arguments (argc)", PMIFLAG_OPTIONAL);
	pmi.AddReturnValue("status", PARG_INT32, "Error status of the call");
	AddMethod(new PMethod("Launch", PRosterLaunch, &pmi));
}


int32_t
PRosterActivateApp(void *pobject, PArgList *in, PArgList *out, void *ptr)
{
//	pmi.AddArg("team", PARG_INT32, "Team ID of the app to activate");
//	pmi.AddReturnValue("status", PARG_INT32, "Error status of the call");
//	AddMethod(new PMethod("ActivateApp", PRosterActivateApp, &pmi));
//	pmi.MakeEmpty();
	return 0;
}


int32_t
PRosterAddToRecentDocuments(void *pobject, PArgList *in, PArgList *out, void *ptr)
{
//	pmi.AddArg("path", PARG_STRING, "Path of the document to add to the recent list.");
//	pmi.AddArg("signature", PARG_STRING, "MIME type of the app used to open the document.", PMIFLAG_OPTIONAL);
//	AddMethod(new PMethod("AddToRecentDocuments", PRosterAddToRecentDocuments, &pmi));
//	pmi.MakeEmpty();
	return 0;
}


int32_t
PRosterAddToRecentFolders(void *pobject, PArgList *in, PArgList *out, void *ptr)
{
//	pmi.AddArg("path", PARG_STRING, "Path of the folder to add to the recent list.");
//	pmi.AddArg("signature", PARG_STRING, "MIME type of the app used to open the folder.", PMIFLAG_OPTIONAL);
//	AddMethod(new PMethod("AddToRecentDocuments", PRosterAddToRecentFolders, &pmi));
//	pmi.MakeEmpty();
	return 0;
}


int32_t
PRosterBroadcast(void *pobject, PArgList *in, PArgList *out, void *ptr)
{
//	pmi.AddArg("list", PARG_LIST, "Argument list containing the message data");
//	pmi.AddReturnValue("status", PARG_INT32, "Error status of the call");
//	AddMethod(new PMethod("Broadcast", PRosterBroadcast, &pmi));
//	pmi.MakeEmpty();
	return 0;
}


int32_t
PRosterFindApp(void *pobject, PArgList *in, PArgList *out, void *ptr)
{
//	pmi.AddArg("signature", PARG_STRING, "MIME type of the desired app", PMIFLAG_OPTIONAL);
//	pmi.AddArg("path", PARG_STRING, "path of the desired app", PMIFLAG_OPTIONAL);
//	pmi.AddReturnValue("list", PARG_LIST, "List containing entry_ref information");
//	pmi.AddReturnValue("status", PARG_INT32, "Error status of the call");
//	AddMethod(new PMethod("FindApp", PRosterFindApp, &pmi));
//	pmi.MakeEmpty();
	return 0;
}


int32_t
PRosterGetActiveAppInfo(void *pobject, PArgList *in, PArgList *out, void *ptr)
{
//	pmi.AddReturnValue("list", PARG_LIST, "List containing the app_info information");
//	pmi.AddReturnValue("status", PARG_INT32, "Error status of the call");
//	AddMethod(new PMethod("GetActiveAppInfo", PRosterGetActiveAppInfo, &pmi));
//	pmi.MakeEmpty();
	return 0;
}


int32_t
PRosterGetAppInfo(void *pobject, PArgList *in, PArgList *out, void *ptr)
{
//	pmi.AddArg("signature", PARG_STRING, "MIME type of the desired app", PMIFLAG_OPTIONAL);
//	pmi.AddArg("path", PARG_STRING, "path of the desired app", PMIFLAG_OPTIONAL);
//	pmi.AddReturnValue("list", PARG_LIST, "List of lists containing the app_info information");
//	pmi.AddReturnValue("status", PARG_INT32, "Error status of the call");
//	AddMethod(new PMethod("GetAppInfo", PRosterGetAppInfo, &pmi));
//	pmi.MakeEmpty();
	return 0;
}


int32_t
PRosterGetAppList(void *pobject, PArgList *in, PArgList *out, void *ptr)
{
//	pmi.AddArg("signature", PARG_STRING, "Optional MIME type of a running app", PMIFLAG_OPTIONAL);
//	pmi.AddReturnValue("list", PARG_LIST, "List of team_id values");
//	AddMethod(new PMethod("GetAppList", PRosterGetAppList, &pmi));
//	pmi.MakeEmpty();
	return 0;
}


int32_t
PRosterGetRecentApps(void *pobject, PArgList *in, PArgList *out, void *ptr)
{
//	pmi.AddArg("maxcount", PARG_INT32, "The maximum number of recent apps to retrieve.");
//	pmi.AddReturnValue("list", PARG_LIST, "List of paths to recently-run applications");
//	AddMethod(new PMethod("GetRecentApps", PRosterGetRecentApps, &pmi));
//	pmi.MakeEmpty();
	return 0;
}


int32_t
PRosterGetRecentDocuments(void *pobject, PArgList *in, PArgList *out, void *ptr)
{
//	pmi.AddArg("maxcount", PARG_INT32, "The maximum number of recent documents to retrieve.");
//	pmi.AddArg("types", PARG_LIST, "List of mimetypes desired.", PMIFLAG_OPTIONAL);
//	pmi.AddArg("maxtypes", PARG_INT32, "Maximum number of documents of the specified type "
//										"to retrieve.", PMIFLAG_OPTIONAL);
//	pmi.AddArg("signature", PARG_STRING, "MIME type of the application used to open the "
//										"specified documents.",	PMIFLAG_OPTIONAL);
//	pmi.AddReturnValue("list", PARG_LIST, "List of paths to recently-used documents");
//	AddMethod(new PMethod("GetRecentDocuments", PRosterGetRecentDocuments, &pmi));
//	pmi.MakeEmpty();
	return 0;
}


int32_t
PRosterGetRecentFolders(void *pobject, PArgList *in, PArgList *out, void *ptr)
{
//	pmi.AddArg("maxcount", PARG_INT32, "The maximum number of recent documents to retrieve.");
//	pmi.AddArg("signature", PARG_STRING, "MIME type of the application used to open the "
//										"specified documents.",	PMIFLAG_OPTIONAL);
//	pmi.AddReturnValue("list", PARG_LIST, "List of paths to recently-used folders");
//	AddMethod(new PMethod("GetRecentFolders", PRosterGetRecentFolders, &pmi));
//	pmi.MakeEmpty();
	return 0;
}


int32_t
PRosterGetRunningAppInfo(void *pobject, PArgList *in, PArgList *out, void *ptr)
{
//	pmi.AddArg("team", PARG_INT32, "TeamID of the desired application");
//	pmi.AddReturnValue("list", PARG_LIST, "List containing the app_info information");
//	pmi.AddReturnValue("status", PARG_INT32, "Error status of the call");
//	AddMethod(new PMethod("GetRunningAppInfo", PRosterGetRunningAppInfo, &pmi));
//	pmi.MakeEmpty();
	return 0;
}


int32_t
PRosterIsRunning(void *pobject, PArgList *in, PArgList *out, void *ptr)
{
//	pmi.AddArg("signature", PARG_STRING, "MIME type of the possibly running app", PMIFLAG_OPTIONAL);
//	pmi.AddArg("path", PARG_STRING, "path of the possibly running app", PMIFLAG_OPTIONAL);
//	pmi.AddReturnValue("value", PARG_BOOL, "True if the specified app is running");
//	AddMethod(new PMethod("IsRunning", PRosterIsRunning, &pmi));
//	pmi.MakeEmpty();
	return 0;
}


int32_t
PRosterLaunch(void *pobject, PArgList *in, PArgList *out, void *ptr)
{
//	pmi.AddArg("signature", PARG_STRING, "MIME signature of the application to launch");
//	pmi.AddArg("arglist", PARG_LIST, "List of string arguments (argc)", PMIFLAG_OPTIONAL);
//	pmi.AddReturnValue("status", PARG_INT32, "Error status of the call");
//	AddMethod(new PMethod("Launch", PRosterLaunch, &pmi));
	return 0;
}


int32_t
PRosterTeamFor(void *pobject, PArgList *in, PArgList *out, void *ptr)
{
//	pmi.AddArg("signature", PARG_STRING, "MIME type of the desired app", PMIFLAG_OPTIONAL);
//	pmi.AddArg("path", PARG_STRING, "path of the desired app", PMIFLAG_OPTIONAL);
//	pmi.AddReturnValue("value", PARG_INT32, "team_id of the specified app");
//	AddMethod(new PMethod("TeamFor", PRosterTeamFor, &pmi));
//	pmi.MakeEmpty();
	return 0;
}
