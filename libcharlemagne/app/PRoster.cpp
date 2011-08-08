#include "PRoster.h"
#include <malloc.h>
#include <string.h>

int32_t PRosterActivateApp(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PRosterAddToRecentDocuments(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PRosterAddToRecentFolders(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PRosterBroadcast(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PRosterFindApp(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PRosterGetActiveAppInfo(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PRosterGetAppInfo(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PRosterGetAppList(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PRosterGetRecentApps(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PRosterGetRecentDocuments(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PRosterGetRecentFolders(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PRosterGetRunningAppInfo(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PRosterIsRunning(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PRosterLaunch(void *pobject, void *in, void *out, void *ptr = NULL);
int32_t PRosterTeamFor(void *pobject, void *in, void *out, void *ptr = NULL);

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
	
	pmi.AddArg("team", B_INT32_TYPE, "Team ID of the app to activate");
	pmi.AddReturnValue("status", B_INT32_TYPE, "Error status of the call");
	AddMethod(new PMethod("ActivateApp", PRosterActivateApp, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("message", B_LIST_TYPE, "Argument list containing the message data");
	pmi.AddReturnValue("status", B_INT32_TYPE, "Error status of the call");
	AddMethod(new PMethod("Broadcast", PRosterBroadcast, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("signature", B_STRING_TYPE, "MIME type of the desired app", PMIFLAG_OPTIONAL);
	pmi.AddArg("path", B_STRING_TYPE, "path of the desired app", PMIFLAG_OPTIONAL);
	pmi.AddReturnValue("entry_ref", B_LIST_TYPE, "List containing entry_ref information");
	pmi.AddReturnValue("status", B_INT32_TYPE, "Error status of the call");
	AddMethod(new PMethod("FindApp", PRosterFindApp, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("signature", B_STRING_TYPE, "MIME type of the desired app", PMIFLAG_OPTIONAL);
	pmi.AddArg("path", B_STRING_TYPE, "path of the desired app", PMIFLAG_OPTIONAL);
	pmi.AddReturnValue("info_list", B_LIST_TYPE, "List of lists containing the app_info information");
	pmi.AddReturnValue("status", B_INT32_TYPE, "Error status of the call");
	AddMethod(new PMethod("GetAppInfo", PRosterGetAppInfo, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("signature", B_STRING_TYPE, "Optional MIME type of a running app", PMIFLAG_OPTIONAL);
	pmi.AddReturnValue("teams", B_LIST_TYPE, "List of team_id values");
	AddMethod(new PMethod("GetAppList", PRosterGetAppList, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddReturnValue("app_info", B_LIST_TYPE, "List containing the app_info information");
	pmi.AddReturnValue("status", B_INT32_TYPE, "Error status of the call");
	AddMethod(new PMethod("GetActiveAppInfo", PRosterGetActiveAppInfo, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("signature", B_STRING_TYPE, "MIME type of the possibly running app", PMIFLAG_OPTIONAL);
	pmi.AddArg("path", B_STRING_TYPE, "path of the possibly running app", PMIFLAG_OPTIONAL);
	pmi.AddReturnValue("value", B_BOOL_TYPE, "True if the specified app is running");
	AddMethod(new PMethod("IsRunning", PRosterIsRunning, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("signature", B_STRING_TYPE, "MIME type of the desired app", PMIFLAG_OPTIONAL);
	pmi.AddArg("path", B_STRING_TYPE, "path of the desired app", PMIFLAG_OPTIONAL);
	pmi.AddReturnValue("value", B_INT32_TYPE, "team_id of the specified app");
	AddMethod(new PMethod("TeamFor", PRosterTeamFor, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("path", B_STRING_TYPE, "Path of the document to add to the recent list.");
	pmi.AddArg("signature", B_STRING_TYPE, "MIME type of the app used to open the document.", PMIFLAG_OPTIONAL);
	AddMethod(new PMethod("AddToRecentDocuments", PRosterAddToRecentDocuments, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("path", B_STRING_TYPE, "Path of the folder to add to the recent list.");
	pmi.AddArg("signature", B_STRING_TYPE, "MIME type of the app used to open the folder.", PMIFLAG_OPTIONAL);
	AddMethod(new PMethod("AddToRecentFolders", PRosterAddToRecentFolders, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("maxcount", B_INT32_TYPE, "The maximum number of recent apps to retrieve.");
	pmi.AddReturnValue("refs", B_LIST_TYPE, "List of entry_refs to recently-run applications");
	AddMethod(new PMethod("GetRecentApps", PRosterGetRecentApps, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("maxcount", B_INT32_TYPE, "The maximum number of recent documents to retrieve.");
	pmi.AddArg("types", B_LIST_TYPE, "List of mimetypes desired.", PMIFLAG_OPTIONAL);
	pmi.AddArg("signature", B_STRING_TYPE, "MIME type of the application used to open the "
										"specified documents.",	PMIFLAG_OPTIONAL);
	pmi.AddReturnValue("refs", B_LIST_TYPE, "List of entry_refs to recently-used documents");
	AddMethod(new PMethod("GetRecentDocuments", PRosterGetRecentDocuments, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("maxcount", B_INT32_TYPE, "The maximum number of recent documents to retrieve.");
	pmi.AddArg("signature", B_STRING_TYPE, "MIME type of the application used to open the "
										"specified documents.",	PMIFLAG_OPTIONAL);
	pmi.AddReturnValue("refs", B_LIST_TYPE, "List of entry_refs to recently-used folders");
	AddMethod(new PMethod("GetRecentFolders", PRosterGetRecentFolders, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("team", B_INT32_TYPE, "TeamID of the desired application");
	pmi.AddReturnValue("app_info", B_LIST_TYPE, "List containing the app_info information");
	pmi.AddReturnValue("status", B_INT32_TYPE, "Error status of the call");
	AddMethod(new PMethod("GetRunningAppInfo", PRosterGetRunningAppInfo, &pmi));
	pmi.MakeEmpty();
	
	pmi.AddArg("signature", B_STRING_TYPE, "MIME signature of the application to launch");
	pmi.AddArg("arglist", B_LIST_TYPE, "List of string arguments (argc)", PMIFLAG_OPTIONAL);
	pmi.AddReturnValue("status", B_INT32_TYPE, "Error status of the call");
	pmi.AddReturnValue("team", B_INT32_TYPE, "Team ID of the launched process");
	AddMethod(new PMethod("Launch", PRosterLaunch, &pmi));
}


#pragma mark - Method functions


int32_t
PRosterActivateApp(void *pobject, void *in, void *out, void *ptr)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PRoster *roster = static_cast<PRoster*>(pobject);
	if (!roster)
		return B_BAD_TYPE;
	
	PArgs *args = static_cast<PArgs*>(in);
	
	int32 team;
	if (args->FindInt32("team", &team) != B_OK)
		return B_ERROR;
	
	int32 status = be_roster->ActivateApp(team);
	
	PArgs *outArgs = static_cast<PArgs*>(out);
	outArgs->MakeEmpty();
	outArgs->AddInt32("status", status);
	
	return B_OK;
}


int32_t
PRosterAddToRecentDocuments(void *pobject, void *in, void *out, void *ptr)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PRoster *roster = static_cast<PRoster*>(pobject);
	if (!roster)
		return B_BAD_TYPE;
	
	PArgs *args = static_cast<PArgs*>(in);
	
	BString path;
	if (args->FindString("path", &path) != B_OK)
		return B_ERROR;
	
	BString signature;
	if (args->FindString("signature", &signature) != B_OK)
		signature = "";
	
	BEntry entry(path.String());
	if (entry.InitCheck() != B_OK)
		return entry.InitCheck();
	
	entry_ref ref;
	entry.GetRef(&ref);
	be_roster->AddToRecentDocuments(&ref, signature.String());
	
	return B_OK;
}


int32_t
PRosterAddToRecentFolders(void *pobject, void *in, void *out, void *ptr)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PRoster *roster = static_cast<PRoster*>(pobject);
	if (!roster)
		return B_BAD_TYPE;
	
	PArgs *args = static_cast<PArgs*>(in);
	
	BString path;
	if (args->FindString("path", &path) != B_OK)
		return B_ERROR;
	
	BString signature;
	if (args->FindString("signature", &signature) != B_OK)
		signature = "";
	
	BEntry entry(path.String());
	if (entry.InitCheck() != B_OK)
		return entry.InitCheck();
	
	entry_ref ref;
	entry.GetRef(&ref);
	be_roster->AddToRecentFolders(&ref, signature.String());
	
	return B_OK;
}


int32_t
PRosterBroadcast(void *pobject, void *in, void *out, void *ptr)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PRoster *roster = static_cast<PRoster*>(pobject);
	if (!roster)
		return B_BAD_TYPE;
	 
	PArgs *args = static_cast<PArgs*>(in);
	
	PArgs list;
	if (args->FindPArg("message", &list) != B_OK)
		return B_ERROR;
	
	BMessage msg = list.GetBackend();
	int32 status = be_roster->Broadcast(&msg);
	
	PArgs *outArgs = static_cast<PArgs*>(out);
	outArgs->MakeEmpty();
	outArgs->AddInt32("status", status);
	
	return B_OK;
}


int32_t
PRosterFindApp(void *pobject, void *in, void *out, void *ptr)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PRoster *roster = static_cast<PRoster*>(pobject);
	if (!roster)
		return B_BAD_TYPE;
	
	PArgs *args = static_cast<PArgs*>(in);
	
	BString path;
	if (args->FindString("path", &path) != B_OK)
		path = "";
	
	BString signature;
	if (args->FindString("signature", &signature) != B_OK)
		signature = "";
	
	entry_ref ref;
	status_t status;
	if (path.CountChars() > 1)
	{
		entry_ref pathRef;
		BEntry entry(path.String());
		entry.GetRef(&pathRef);
		
		status = be_roster->FindApp(&pathRef, &ref);
	}
	else if(signature.CountChars() < 1)
	{
		status = be_roster->FindApp(signature.String(), &ref);
	}
	else
		return B_ERROR;
	
	PArgs refList;
	refList.AddInt32("device", ref.device);
	refList.AddInt64("directory", ref.directory);
	refList.AddString("name", ref.name);
	
	PArgs *outArgs = static_cast<PArgs*>(out);
	outArgs->MakeEmpty();
	outArgs->AddPArg("entry_ref", refList);
	outArgs->AddInt32("status", status);
	
	return B_OK;
}


int32_t
PRosterGetActiveAppInfo(void *pobject, void *in, void *out, void *ptr)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PRoster *roster = static_cast<PRoster*>(pobject);
	if (!roster)
		return B_BAD_TYPE;
	
	app_info info;
	status_t status = be_roster->GetActiveAppInfo(&info);
	
	PArgs infoList;
	infoList.AddInt32("thread", info.thread);
	infoList.AddInt32("team", info.team);
	infoList.AddInt32("port", info.port);
	infoList.AddInt32("flags", info.flags);
	infoList.AddRef("entry_ref", info.ref);
	infoList.AddString("signature", info.signature);
	
	PArgs *outArgs = static_cast<PArgs*>(out);
	outArgs->MakeEmpty();
	outArgs->AddPArg("app_info", infoList);
	outArgs->AddInt32("status", status);
	
	return B_OK;
}


int32_t
PRosterGetAppInfo(void *pobject, void *in, void *out, void *ptr)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PRoster *roster = static_cast<PRoster*>(pobject);
	if (!roster)
		return B_BAD_TYPE;
	
	PArgs *args = static_cast<PArgs*>(in);
	
	BString path;
	if (args->FindString("path", &path) != B_OK)
		path = "";
	
	BString signature;
	if (args->FindString("signature", &signature) != B_OK)
		signature = "";
	
	app_info info;
	status_t status;
	if (path.CountChars() > 1)
	{
		entry_ref pathRef;
		BEntry entry(path.String());
		entry.GetRef(&pathRef);
		
		status = be_roster->GetAppInfo(&pathRef, &info);
	}
	else if(signature.CountChars() < 1)
	{
		status = be_roster->GetAppInfo(signature.String(), &info);
	}
	else
		return B_ERROR;
	
	PArgs infoList;
	infoList.AddInt32("thread", info.thread);
	infoList.AddInt32("team", info.team);
	infoList.AddInt32("port", info.port);
	infoList.AddInt32("flags", info.flags);
	infoList.AddRef("entry_ref", info.ref);
	infoList.AddString("signature", info.signature);
	
	PArgs *outArgs = static_cast<PArgs*>(out);
	outArgs->MakeEmpty();
	outArgs->AddPArg("app_info", infoList);
	outArgs->AddInt32("status", status);
	
	return B_OK;
}


int32_t
PRosterGetAppList(void *pobject, void *in, void *out, void *ptr)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PRoster *roster = static_cast<PRoster*>(pobject);
	if (!roster)
		return B_BAD_TYPE;
	
	PArgs *args = static_cast<PArgs*>(in);
	
	BString signature;
	PArgs teamList;
	if (args->FindString("signature", &signature) != B_OK)
		signature = "";
	
	BList idList;
	if (signature.CountChars() > 0)
		be_roster->GetAppList(signature.String(), &idList);
	else
		be_roster->GetAppList(&idList);
	
	PArgs outList;
	for (int32 i = 0; i < idList.CountItems(); i++)
	{
		team_id *team = (team_id*)idList.ItemAt(i);
		
		BString indexString;
		indexString << i;
		
		outList.AddInt32(indexString.String(), *team);
	}
	
	PArgs *outArgs = static_cast<PArgs*>(out);
	outArgs->MakeEmpty();
	outArgs->AddPArg("teams", *outArgs);
	
	return B_OK;
}


int32_t
PRosterGetRecentApps(void *pobject, void *in, void *out, void *ptr)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PRoster *roster = static_cast<PRoster*>(pobject);
	if (!roster)
		return B_BAD_TYPE;
	
	PArgs *args = static_cast<PArgs*>(in);
	
	int32 max;
	if (args->FindInt32("maxcount", &max) != B_OK)
		return B_ERROR;
	
	BMessage refmsg;
	be_roster->GetRecentApps(&refmsg, max);
	
	PArgs outRefList;
	int32 i = 0;
	entry_ref appRef;
	while (refmsg.FindRef("refs", i, &appRef) == B_OK)
	{
		BString indexString;
		indexString << i;
		outRefList.AddRef(indexString.String(), appRef);
		i++;
	}
	
	PArgs *outArgs = static_cast<PArgs*>(out);
	outArgs->MakeEmpty();
	outArgs->AddPArg("paths", outRefList);
	
	return B_OK;
}


int32_t
PRosterGetRecentDocuments(void *pobject, void *in, void *out, void *ptr)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PRoster *roster = static_cast<PRoster*>(pobject);
	if (!roster)
		return B_BAD_TYPE;
	
	PArgs *args = static_cast<PArgs*>(in);
	
	int32 max;
	if (args->FindInt32("maxcount", &max) != B_OK)
		return B_ERROR;
	
	BString signature;
	if (args->FindString("signature", &signature) != B_OK)
		signature = "";
	
	PArgs typeList;
	int32 typeCount = 0;
	BMessage refmsg;
	if (typeList.CountFieldItems("types", &typeCount) == B_OK)
	{
		char *types[typeCount];
		
		BString type;
		int32 typeIndex = 0;
		while (typeList.FindString("types", &type, typeIndex) == B_OK)
		{
			types[typeIndex] = strdup(type.String());
			typeIndex++;
		}
		
		be_roster->GetRecentDocuments(&refmsg, max, (const char **)types, typeIndex + 1,
									signature.String());
		
		for (int32 i = 0; i < typeCount; i++)
			free(types[i]);
		
	}
	else
		return B_BAD_DATA;
	
	PArgs outRefList;
	int32 i = 0;
	entry_ref appRef;
	while (refmsg.FindRef("refs", i, &appRef) == B_OK)
	{
		BString indexString;
		indexString << i;
		outRefList.AddRef(indexString.String(), appRef);
		i++;
	}
	
	PArgs *outArgs = static_cast<PArgs*>(out);
	outArgs->MakeEmpty();
	outArgs->AddPArg("paths", outRefList);
	
	return B_OK;
}


int32_t
PRosterGetRecentFolders(void *pobject, void *in, void *out, void *ptr)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PRoster *roster = static_cast<PRoster*>(pobject);
	if (!roster)
		return B_BAD_TYPE;
	
	PArgs *args = static_cast<PArgs*>(in);
	
	int32 max;
	if (args->FindInt32("maxcount", &max) != B_OK)
		return B_ERROR;
	
	BString signature;
	if (args->FindString("signature", &signature) != B_OK)
		signature = "";
	
	BMessage refmsg;
	be_roster->GetRecentFolders(&refmsg, max, signature.String());
	
	PArgs outRefList;
	int32 i = 0;
	entry_ref appRef;
	while (refmsg.FindRef("refs", i, &appRef) == B_OK)
	{
		BString indexString;
		indexString << i;
		outRefList.AddRef(indexString.String(), appRef);
		i++;
	}
	
	PArgs *outArgs = static_cast<PArgs*>(out);
	outArgs->MakeEmpty();
	outArgs->AddPArg("refs", outRefList);
	
	return B_OK;
}


int32_t
PRosterGetRunningAppInfo(void *pobject, void *in, void *out, void *ptr)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PRoster *roster = static_cast<PRoster*>(pobject);
	if (!roster)
		return B_BAD_TYPE;
	
	PArgs *args = static_cast<PArgs*>(in);
	
	int32 team;
	if (args->FindInt32("team", &team) != B_OK)
		return B_ERROR;
	
	app_info info;
	status_t status = be_roster->GetRunningAppInfo(team, &info);
	
	PArgs infoList;
	infoList.AddInt32("thread", info.thread);
	infoList.AddInt32("team", info.team);
	infoList.AddInt32("port", info.port);
	infoList.AddInt32("flags", info.flags);
	infoList.AddRef("entry_ref", info.ref);
	infoList.AddString("signature", info.signature);
	
	PArgs *outArgs = static_cast<PArgs*>(out);
	outArgs->MakeEmpty();
	outArgs->AddPArg("app_info", infoList);
	outArgs->AddInt32("status", status);
	
	return B_OK;
}


int32_t
PRosterIsRunning(void *pobject, void *in, void *out, void *ptr)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PRoster *roster = static_cast<PRoster*>(pobject);
	if (!roster)
		return B_BAD_TYPE;
	
	PArgs *args = static_cast<PArgs*>(in);
	
	BString path;
	if (args->FindString("path", &path) != B_OK)
		path = "";
	
	BString signature;
	if (args->FindString("signature", &signature) != B_OK)
		signature = "";
	
	bool isRunning;
	if (path.CountChars() > 1)
	{
		entry_ref pathRef;
		BEntry entry(path.String());
		entry.GetRef(&pathRef);
		
		isRunning = be_roster->IsRunning(&pathRef);
	}
	else if(signature.CountChars() < 1)
	{
		isRunning = be_roster->IsRunning(signature.String());
	}
	else
		return B_ERROR;
	
	PArgs *outArgs = static_cast<PArgs*>(out);
	outArgs->MakeEmpty();
	outArgs->AddBool("value", isRunning);
	
	return B_OK;
}


int32_t
PRosterLaunch(void *pobject, void *in, void *out, void *ptr)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PRoster *roster = static_cast<PRoster*>(pobject);
	if (!roster)
		return B_BAD_TYPE;
	
	PArgs *args = static_cast<PArgs*>(in);
	
	BString signature;
	if (args->FindString("signature", &signature) != B_OK)
		signature = "";

	PArgs argList;
	int32 argCount = 0;
	team_id team;
	status_t status;
	if (argList.CountFieldItems("arglist", &argCount) == B_OK)
	{
		char *launchArgs[argCount];
		
		BString arg;
		int32 argIndex = 0;
		while (argList.FindString("args", &arg, argIndex) == B_OK)
		{
			launchArgs[argIndex] = strdup(arg.String());
			argIndex++;
		}
		
		status = be_roster->Launch(signature.String(), argIndex + 1,
										launchArgs, &team);
		
		for (int32 i = 0; i < argCount; i++)
			free(launchArgs[i]);
		
	}
	else
		return B_BAD_DATA;
	
	PArgs *outArgs = static_cast<PArgs*>(out);
	outArgs->MakeEmpty();
	outArgs->AddInt32("team", team);
	outArgs->AddInt32("status", status);
	
	return B_OK;
}


int32_t
PRosterTeamFor(void *pobject, void *in, void *out, void *ptr)
{
	if (!pobject || !in || !out)
		return B_ERROR;
	
	PRoster *roster = static_cast<PRoster*>(pobject);
	if (!roster)
		return B_BAD_TYPE;
	
	PArgs *args = static_cast<PArgs*>(in);
	
	BString path;
	if (args->FindString("path", &path) != B_OK)
		path = "";
	
	BString signature;
	if (args->FindString("signature", &signature) != B_OK)
		signature = "";
	
	team_id team;
	if (path.CountChars() > 1)
	{
		entry_ref pathRef;
		BEntry entry(path.String());
		entry.GetRef(&pathRef);
		
		team = be_roster->TeamFor(&pathRef);
	}
	else if(signature.CountChars() < 1)
	{
		team = be_roster->TeamFor(signature.String());
	}
	else
		return B_ERROR;
	
	PArgs *outArgs = static_cast<PArgs*>(out);
	outArgs->MakeEmpty();
	outArgs->AddInt32("value", team);
	
	return B_OK;
}
