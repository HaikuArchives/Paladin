#include "PApplication.h"

#include <Messenger.h>
#include <malloc.h>
#include <Path.h>
#include <stdio.h>

#include "MiscProperties.h"
#include "PArgs.h"
#include "PObjectBroker.h"
#include "PMethod.h"
#include "PView.h"
#include "PWindow.h"
#include "PWindowPriv.h"

int32_t PAppObscureCursor(void *pobject, PArgList *in, PArgList *out);
int32_t PAppRun(void *pobject, PArgList *in, PArgList *out);
int32_t PAppQuit(void *pobject, PArgList *in, PArgList *out);
int32_t PAppWindowAt(void *pobject, PArgList *in, PArgList *out);


class PAppBackend : public BApplication
{
public:
			PAppBackend(PObject *owner, const char *signature);
	
	void	AboutRequested(void);
	void	AppActivated(bool active);
	void	ArgvReceived(int32 argc, char **argv);
	void	Pulse(void);
	bool	QuitRequested(void);
	void	ReadyToRun(void);
	void	RefsReceived(BMessage *msg);
	
private:
	PObject *fOwner;
};


PApplication::PApplication(void)
	:	fPulseRate(0)
{
	fType = "PApplication";
	fFriendlyType = "Application";
	AddInterface("PApplication");
	
	// This constructor starts with an empty PApplication, so we need to
	// initialize it with some properties
	InitProperties();
	InitBackend();
}


PApplication::PApplication(BMessage *msg)
	:	PHandler(msg),
		fPulseRate(0)
{
	fType = "PApplication";
	fFriendlyType = "Application";
	AddInterface("PApplication");
	InitBackend();
}


PApplication::PApplication(const char *name)
	:	PHandler(name),
		fPulseRate(0)
{
	fType = "PApplication";
	fFriendlyType = "Application";
	AddInterface("PApplication");
	InitBackend();
}


PApplication::PApplication(const PApplication &from)
	:	PHandler(from),
		fPulseRate(0)
{
	fType = "PApplication";
	fFriendlyType = "Application";
	AddInterface("PApplication");
	InitBackend();
}


PApplication::~PApplication(void)
{
	if (be_app)
	{
		be_app->Lock();
		be_app->Quit();
		be_app = NULL;
	}
}


int32
PApplication::Run(const char *signature)
{
	PAppBackend *app = NULL;
	if (!be_app)
	{
		BString sig(signature);
		if (sig.CountChars() < 1)
			sig = "application/x-vnd.dw-CharlemagneApp";
		SetStringProperty("signature", sig.String());
		app = new PAppBackend(this, sig.String());
	}
	else
		app = dynamic_cast<PAppBackend*>(be_app);
	return app->Run();
}


BArchivable *
PApplication::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "PApplication"))
		return new PApplication(data);
	
	return NULL;
}


PObject *
PApplication::Create(void)
{
	return new PApplication();
}


PObject *
PApplication::Duplicate(void) const
{
	return new PApplication(*this);
}

status_t
PApplication::GetProperty(const char *name, PValue *value, const int32 &index) const
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (str.ICompare("CursorVisible") == 0)
		((BoolProperty*)prop)->SetValue(!be_app->IsCursorHidden());
	else if (str.ICompare("Flags") == 0)
	{
		if (!be_app)
			return B_NO_INIT;
		((IntProperty*)prop)->SetValue(fAppInfo.flags);
	}
	else if (str.ICompare("IsLaunching") == 0)
		((BoolProperty*)prop)->SetValue(be_app->IsLaunching());
	else if (str.ICompare("Port") == 0)
	{
		if (!be_app)
			return B_NO_INIT;
		((IntProperty*)prop)->SetValue(fAppInfo.port);
	}
	else if (str.ICompare("PulseRate") == 0)
		((IntProperty*)prop)->SetValue(fPulseRate);
	else if (str.ICompare("Signature") == 0)
		((StringProperty*)prop)->SetValue(fAppInfo.signature);
	else if (str.ICompare("Team") == 0)
		((IntProperty*)prop)->SetValue(fAppInfo.team);
	else if (str.ICompare("Thread") == 0)
		((IntProperty*)prop)->SetValue(fAppInfo.team);
	else if (str.ICompare("WindowCount") == 0)
		((BoolProperty*)prop)->SetValue(be_app->CountWindows());
	else
		return PHandler::GetProperty(name,value,index);
	
	return B_OK;
}


status_t
PApplication::SetProperty(const char *name, PValue *value, const int32 &index)
{
	if (!name || !value)
		return B_ERROR;
	
	BString str(name);
	PProperty *prop = FindProperty(name,index);
	if (!prop)
		return B_NAME_NOT_FOUND;
	
	if (FlagsForProperty(prop) & PROPERTY_READ_ONLY)
		return B_READ_ONLY;
	
	status_t status = prop->SetValue(value);
	if (status != B_OK)
		return status;
	
	BoolValue bv;
	IntValue iv;
	StringValue sv;
	be_app->Lock();
	if (str.ICompare("CursorVisible") == 0)
	{
		prop->GetValue(&bv);
		if (*bv.value)
			be_app->ShowCursor();
		else
			be_app->HideCursor();
	}
	else if (str.ICompare("PulseRate") == 0)
	{
		prop->GetValue(&iv);
		fPulseRate = *iv.value;
		be_app->SetPulseRate(fPulseRate);
	}
	else if (str.ICompare("Signature") == 0)
	{
		if (be_app)
			return B_ERROR;
		
		prop->GetValue(&sv);
		
		PAppBackend *app = new PAppBackend(this, sv.value->String());
		
		// just to quiet the compiler
		app->IsLaunching();
	}
	else
	{
		be_app->Unlock();
		return PHandler::SetProperty(name,value,index);
	}
	
	be_app->Unlock();
	return prop->GetValue(value);
}


void
PApplication::InitProperties(void)
{
	AddProperty(new BoolProperty("CursorVisible",true,
								"Set to true if the cursor can be seen."),
								PROPERTY_HIDE_IN_EDITOR);
	AddProperty(new StringProperty("Description","The main program object. There can be only one."));
	AddProperty(new IntProperty("Flags", 0, "Launch flags for the application"),
								PROPERTY_READ_ONLY);
	AddProperty(new BoolProperty("IsLaunching", true,
								"True if the app is still launching -- before ReadyToRun is called."),
								PROPERTY_READ_ONLY);

	AddProperty(new IntProperty("Port", -1, "ID of the main communications port for the app."),
								PROPERTY_READ_ONLY);
	AddProperty(new IntProperty("Port", -1, "ID of the main communications port for the app."),
								PROPERTY_READ_ONLY);
	AddProperty(new IntProperty("PulseRate",0,"The length of time between B_PULSE ticks in microseconds"));
	AddProperty(new StringProperty("Signature","The signature of the app. This is read-only once set."));
	AddProperty(new IntProperty("Team",-1,"ID for this instance of the app."));
	AddProperty(new IntProperty("WindowCount", 0, "The number of windows in the app, visible or otherwise."));
}


status_t
PApplication::SendMessage(BMessage *msg)
{
	BMessenger msgr(NULL, be_app);
	return msgr.SendMessage(msg);
}

	
void
PApplication::InitBackend(void)
{

	PMethodInterface pmi;
	AddMethod(new PMethod("ObscureCursor", PAppObscureCursor, &pmi));
	
	pmi.AddReturnValue("thread_id", PARG_INT64, "The ID of the main thread.");
	AddMethod(new PMethod("Run", PAppRun, &pmi));
	pmi.MakeEmpty();
	
	AddMethod(new PMethod("Quit", PAppQuit, &pmi));
	
	pmi.AddArg("index", PARG_INT32, "The index of the window to return");
	AddMethod(new PMethod("WindowAt", PAppWindowAt, &pmi));
	
	AddEvent("AboutRequested", "The app has been asked to show an about window.");
	
	pmi.SetArg(0, "active", PARG_BOOL, "The active status of the app.");
	AddEvent("AppActivated", "The app has gained or lost focus.");
	pmi.MakeEmpty();
	
	AddEvent("AppSetup", "The app is being set up.", &pmi);
	
	pmi.AddArg("argv", PARG_STRING, "A list of strings containing the arguments.");
	AddEvent("ArgvReceived", "The window was launched from the command line.", &pmi);
	pmi.MakeEmpty();
	
	AddEvent("Pulse", "Run at specified clock intervals.", &pmi);
	
	pmi.AddReturnValue("value", PARG_BOOL, "Whether or not the app should really quit.");
	AddEvent("QuitRequested", "The window was asked to quit.", &pmi);
	pmi.MakeEmpty();
	
	AddEvent("ReadyToRun", "The app is ready to run.", &pmi);
	
	pmi.AddArg("refs", PARG_POINTER, "A list of entry_ref structures pointing to files/directories sent to the app.");
	AddEvent("RefsReceived", "The app was sent a list of files/directories.", &pmi);
}


int32_t
PAppObscureCursor(void *pobject, PArgList *in, PArgList *out)
{
	if (be_app)
		be_app->ObscureCursor();
	
	return be_app ? B_OK : B_NO_INIT;
}


int32_t
PAppRun(void *pobject, PArgList *in, PArgList *out)
{
	thread_id id = -1;
	if (be_app)
	{
		id = be_app->Run();
	
		PArgs outargs(out);
		outargs.MakeEmpty();
		outargs.AddInt64("thread_id", id);
	}
	
	return be_app ? B_OK : B_NO_INIT;
}


int32_t
PAppQuit(void *pobject, PArgList *in, PArgList *out)
{
	if (be_app)
		be_app->Quit();
	
	return be_app ? B_OK : B_NO_INIT;
}


int32_t
PAppWindowAt(void *pobject, PArgList *in, PArgList *out)
{
	uint64_t id = 0;
	if (be_app)
	{
		int32 windex;
		
		PArgs args(in);
		if (args.FindInt32("index", &windex) != B_OK)
			return B_ERROR;
		
		
		BWindow *win = be_app->WindowAt(windex);
		if (!win)
			return B_ERROR;
		
		PWindowBackend *pwinBack = dynamic_cast<PWindowBackend*>(win);
		if (pwinBack)
		{
			PObject *obj = pwinBack->GetOwner();
			if (obj)
				id = obj->GetID();
		}
				
		PArgs outargs(out);
		outargs.MakeEmpty();
		outargs.AddInt64("id", id);
		
	}
	
	return be_app ? B_OK : B_NO_INIT;
}



PAppBackend::PAppBackend(PObject *owner, const char *signature)
	:	BApplication(signature),
		fOwner(owner)
{
	PArgs in, out;
	
	fOwner->RunEvent("AppSetup", in.ListRef(), out.ListRef());
}


void
PAppBackend::AboutRequested(void)
{
	PArgs in, out;
	fOwner->RunEvent("AboutRequested", in.ListRef(), out.ListRef());
}


void
PAppBackend::AppActivated(bool active)
{
	PArgs in, out;
	in.AddBool("active", active);
	fOwner->RunEvent("AppActivated", in.ListRef(), out.ListRef());
}


void
PAppBackend::ArgvReceived(int32 argc, char **argv)
{
	PArgs in, out;
	
	for (int32 i = 0; i < argc; i++)
		in.AddString("argv", argv[i]);
	fOwner->RunEvent("ArgvReceived", in.ListRef(), out.ListRef());
}


void
PAppBackend::Pulse(void)
{
	PArgs in, out;
	fOwner->RunEvent("Pulse", in.ListRef(), out.ListRef());
}


bool
PAppBackend::QuitRequested(void)
{
	PArgs in, out;
	fOwner->RunEvent("QuitRequested", in.ListRef(), out.ListRef());
	
	bool quit;
	if (out.FindBool("value", &quit) != B_OK)
		quit = true;
	
	return quit;
}


void
PAppBackend::ReadyToRun(void)
{
	PArgs in, out;
	fOwner->RunEvent("ReadyToRun", in.ListRef(), out.ListRef());
}


void
PAppBackend::RefsReceived(BMessage *msg)
{
	PArgs in, out;
	int32 i = 0;
	entry_ref ref;
	
	while (msg->FindRef("refs", i++, &ref) == B_OK)
	{
		BPath path(&ref);
		if (path.InitCheck() == B_OK)
			in.AddString("path", path.Path());
	}
	
	fOwner->RunEvent("RefsReceived", in.ListRef(), out.ListRef());
}

