//========================================================================
//	MThread.cpp
//	Copyright 1996 Metrowerks Corporation, All Rights Reserved.
//========================================================================	
//	BDS

#include <OS.h>
#include "MThread.h"


// ---------------------------------------------------------------------------
//		¥ MThread
// ---------------------------------------------------------------------------
//	Constructor

MThread::MThread(
	const char *	inThreadName,
	long			inPriority)
	: fLock(inThreadName)
{
	fCanceled = false;
	fThread = spawn_thread((thread_entry) ThreadEntry, inThreadName, inPriority, this);
}

// ---------------------------------------------------------------------------
//		¥ ~MThread
// ---------------------------------------------------------------------------
//	Destructor

MThread::~MThread()
{
	Kill();
}

// ---------------------------------------------------------------------------
//		¥ Cancel
// ---------------------------------------------------------------------------
//	Call Cancel to cancel a running thread.

void
MThread::Cancel()
{
	Lock();
	fCanceled = true;
	Unlock();
}

// ---------------------------------------------------------------------------
//		¥ Cancelled
// ---------------------------------------------------------------------------
//	The Execute function should call Cancelled periodically to see if the 
//	thread has been cancelled.  If so it should simply return from Execute.

bool
MThread::Cancelled()
{
	Lock();
	bool	isCancelled = fCanceled;
	Unlock();
	
	return isCancelled;
}

// ---------------------------------------------------------------------------
//		¥ Kill
// ---------------------------------------------------------------------------
//	To kill a thread immediately call Kill.  This may have bad results.

void
MThread::Kill()
{
	Lock();

	if (fThread >= B_NO_ERROR)
	{
		long		ignore;

		kill_thread(fThread);
		wait_for_thread(fThread, &ignore);
		fThread = -1;
	}

	Unlock();
}

// ---------------------------------------------------------------------------
//		¥ Run
// ---------------------------------------------------------------------------
//	Call Run to start the thread running.

long
MThread::Run()
{
	long 		err;

	if (fThread >= B_NO_ERROR)
		err = resume_thread(fThread);
	else
		err = fThread;
	
	return err;
}

// ---------------------------------------------------------------------------
//		¥ ThreadEntry
// ---------------------------------------------------------------------------

long
MThread::ThreadEntry(
	MThread*	inObject)
{
	long	result = inObject->Execute();

	inObject->Lock();
	inObject->fThread = -1;
	inObject->Unlock();
	
	inObject->LastCall();

	return result;
}

// ---------------------------------------------------------------------------
//		¥ Execute
// ---------------------------------------------------------------------------
//	Override this virtual function and do whatever it is that the thread
//	object does.

long
MThread::Execute()
{
	return B_NO_ERROR;
}

// ---------------------------------------------------------------------------
//		¥ LastCall
// ---------------------------------------------------------------------------
//	Called just before the thread entry function returns.  Override if you
//	don't want to delete this or you need to do something in addition.
//	Alternatively final actions can be performed in the object's destructor.

void
MThread::LastCall()
{
	delete this;
}
