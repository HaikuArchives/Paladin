/*
 * Copyright 2004-2010, Jérôme Duval. All rights reserved.
 * Copyright 2019, Haiku Inc. All right reserved.
 * Distributed under the terms of the MIT License.
 * Original code from ZipOMatic by jonas.sundstrom@kirilla.com
 * Code extracted from Expander and made more general purpose
 *   in 2019 by Adam Fowler
 *			2019	Adam Fowler, adamfowleruk@gmail.com
 */
#include "CommandThread.h"

#include <errno.h>
#include <image.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>

#include <Messenger.h>
#include <Path.h>
#include <String.h>


const char* CommandThreadName = "CommandThread";


CommandThread::CommandThread(BMessage* refs_message, BMessenger* messenger)
	:
	GenericThread(CommandThreadName, B_NORMAL_PRIORITY, refs_message),
	fWindowMessenger(messenger),
	fThreadId(-1),
	fStdIn(-1),
	fStdOut(-1),
	fStdErr(-1),
	fThreadOutput(NULL),
	fThreadError(NULL)
{
	SetDataStore(new BMessage(*refs_message));
		// leak?
	// prevents bug with B_SIMPLE_DATA
	// (drag&drop messages)
}


CommandThread::~CommandThread()
{
	//DO NOT delete fWindowMessenger;
}


status_t
CommandThread::ThreadStartup()
{
	status_t status = B_OK;
	entry_ref pwd;
	BString cmd;


	if ((status = GetDataStore()->FindRef("pwd", &pwd)) == B_OK) {
		BPath path(&pwd);
		BString pt(path.Path());
		//printf("SWITCHING DIR\n");
		//printf(pt);
		chdir(path.Path());
	}

	if ((status = GetDataStore()->FindString("cmd", &cmd)) != B_OK)
		return status;

	int32 argc = 3;
	const char** argv = new const char * [argc + 1];

	argv[0] = strdup("/bin/sh");
	argv[1] = strdup("-c");
	argv[2] = strdup(cmd.String());
	argv[argc] = NULL;

	fThreadId = PipeCommand(argc, argv, fStdIn, fStdOut, fStdErr);

	delete [] argv;

	if (fThreadId < 0)
		return fThreadId;

	// lower the command priority since it is a background task.
	set_thread_priority(fThreadId, B_LOW_PRIORITY);

	resume_thread(fThreadId); // is this too early, before stdout is available?

	int flags = fcntl(fStdOut, F_GETFL, 0);
	flags |= O_NONBLOCK;
	fcntl(fStdOut, F_SETFL, flags);
	flags = fcntl(fStdErr, F_GETFL, 0);
	flags |= O_NONBLOCK;
	fcntl(fStdErr, F_SETFL, flags);

	fThreadOutput = fdopen(fStdOut, "r");
	fThreadError = fdopen(fStdErr, "r");
	
	SetExitCallback(ExitCallback, this);

	return B_OK;
}

void
CommandThread::ExitCallback(void* data)
{
	CommandThread* me = (CommandThread*)data;
	if (me->fStdIn)
		close(me->fStdIn);
		
	//if (me->fStdOut)
	//	close(me->fStdOut);
	//if (me->fStdErr)
	//	close(me->fStdErr);
}

void
CommandThread::CheckForOutput()
{

	// read output and error from command
	// send it to listener

	bool outputAdded = false;
	int32 readsPerPass = 200; // high so that we don't incur a 
	// large cost from get_thread_info
	BString toSend;
	for (int32 i = 0; NULL != fThreadOutput && -1 != fStdOut && !feof(fThreadOutput) && i < readsPerPass; i++) {
		char* output_string = fgets(fThreadOutputBuffer , LINE_MAX,
			fThreadOutput);
		if (output_string == NULL)
		{
			break;
		}
		
		toSend << output_string;
		outputAdded = true;
	}
	if (outputAdded)
	{
		BMessage* message = new BMessage(M_COMMAND_RECEIVE_STDOUT);
		message->AddString("output", strdup(toSend.String()));
		message->AddUInt64("thread_id",fThreadId);
		
		// Send through any context, if provided
		void* ctx = NULL;
		if (B_OK == GetDataStore()->FindPointer("context", &ctx)) {
			message->AddPointer("context",ctx);
		}
		fWindowMessenger->SendMessage(message);
	}

	//if (feof(fThreadOutput))
	//	return EOF;
	// Note (AF): The above is a bad way to detect the thread 
	//            has finished - it hides stderr data!

	bool errorsAdded = false;
	BString toSendErr;
	for (int32 i = 0; NULL != fThreadError && -1 != fStdErr && !feof(fThreadError) && i < readsPerPass; i++) {
		char* error_string = fgets(fThreadOutputBuffer, LINE_MAX,
			fThreadError);
		if (error_string == NULL) 
		{
			break;
		}
		
		toSendErr << error_string;
		errorsAdded = true;
	}
	if (errorsAdded)
	{
		BMessage* message = new BMessage(M_COMMAND_RECEIVE_STDERR);
		message->AddString("error", strdup(toSendErr.String()));
		message->AddUInt64("thread_id",fThreadId);
		
		// Send through any context, if provided
		void* ctx = NULL;
		if (B_OK == GetDataStore()->FindPointer("context", &ctx)) {
			message->AddPointer("context",ctx);
		}
		fWindowMessenger->SendMessage(message);
	}
}

status_t
CommandThread::ExecuteUnit(void)
{
	CheckForOutput();

	// streams are non blocking, sleep every 100ms
	// snooze(100000);
	
	// check thread's status instead of sleeping, and 
	//   return an EOF if complete/died
	thread_info info;
	status_t status = get_thread_info(fThreadId, &info);
	if (B_OK == status)
	{
		// no need for a complete flag - get_thread_info 
		//   won't return B_OK if it's not running
		return B_OK;
	} else {
		return EOF; // some failure (thread doesn't exist anymore)
	}

	return B_OK;
}


void
CommandThread::PushInput(BString text)
{
	// TODO should we allow a way to not add the newline 
	//      incase of special input needs?
	//  This is probably here because we execute in a ptty, 
	//    and need the newline to send the data
	text += "\n";
	write(fStdIn, text.String(), text.Length());
}


status_t
CommandThread::ThreadShutdown(void)
{
	//close(fStdIn);
	
	while (EOF != ExecuteUnit()) {
    	// catches any remaining stderr/stdout data
		// awaiting execute unit to have no more thread running
		BMessage message(M_COMMAND_AWAITING_QUIT);
		message.AddUInt64("thread_id",fThreadId);
		
		// Send through any context, if provided
		void* ctx = NULL;
		if (B_OK == GetDataStore()->FindPointer("context", &ctx)) {
			message.AddPointer("context",ctx);
		}
		fWindowMessenger->SendMessage(&message);
		
		snooze(100000);
	}
	//close(fStdOut);
	//close(fStdErr);

	return B_OK;
}


void
CommandThread::ThreadStartupFailed(status_t status)
{
	fprintf(stderr, "CommandThread::ThreadStartupFailed() : %s\n",
		strerror(status));

	Quit();
}


void
CommandThread::ExecuteUnitFailed(status_t status)
{
	CheckForOutput();
	
	if (status == EOF) {
		// thread has finished, been quit or killed, we don't know
		BMessage* message = new BMessage(M_COMMAND_EXITED);
		
		message->AddUInt64("thread_id",fThreadId);
		
		// Send through any context, if provided
		void* ctx = NULL;
		if (B_OK == GetDataStore()->FindPointer("context", &ctx)) {
			message->AddPointer("context",ctx);
		}
		fWindowMessenger->SendMessage(message);
	} else {
		// explicit error - communicate error to Window
		BMessage* message = new BMessage(M_COMMAND_EXITED_IN_ERROR);
		
		message->AddUInt64("thread_id",fThreadId);
		
		// Send through any context, if provided
		void* ctx = NULL;
		if (B_OK == GetDataStore()->FindPointer("context", &ctx)) {
			message->AddPointer("context",ctx);
		}
		fWindowMessenger->SendMessage(message);
	}

	Quit();
}


void
CommandThread::ThreadShutdownFailed(status_t status)
{
	fprintf(stderr, "CommandThread::ThreadShutdownFailed() %s\n",
		strerror(status));
}


status_t
CommandThread::ProcessRefs(BMessage *msg)
{
	return B_OK;
}


thread_id
CommandThread::PipeCommand(int argc, const char** argv, int& in, int& out,
	int& err, const char** envp)
{
	// This function written by Peter Folk <pfolk@uni.uiuc.edu>
	// and published in the BeDevTalk FAQ
	// http://www.abisoft.com/faq/BeDevTalk_FAQ.html#FAQ-209

	// Save current FDs
	int old_out = dup(1);
	int old_err = dup(2);

	int filedes[2];
	
	// create new pipe FDs as stdout, stderr
	pipe(filedes);  dup2(filedes[1], 1); close(filedes[1]);
	out = filedes[0]; // Read from out, taken from cmd's stdout
	pipe(filedes);  dup2(filedes[1], 2); close(filedes[1]);
	err = filedes[0]; // Read from err, taken from cmd's stderr

	// taken from pty.cpp
	// create a tty for stdin, as utilities don't generally use stdin
	int master = posix_openpt(O_RDWR);
	if (master < 0)
		return -1;

	int slave;
	const char* ttyName;
	if (grantpt(master) != 0 || unlockpt(master) != 0
		|| (ttyName = ptsname(master)) == NULL
		|| (slave = open(ttyName, O_RDWR | O_NOCTTY)) < 0) {
		close(master);
		return -1;
	}

	int pid = fork();
	if (pid < 0) {
		close(master);
		close(slave);
		return -1;
	}

	// child
	if (pid == 0) {
		close(master);

		setsid();
		if (ioctl(slave, TIOCSCTTY, NULL) != 0)
			return -1;

		dup2(slave, 0); 
		close(slave);

		// "load" command.
		execv(argv[0], (char *const *)argv);

		// shouldn't return
		return -1;
	}

	// parent
	close (slave);
	in = master;

	// Restore old FDs
	close(1); dup(old_out); close(old_out);
	close(2); dup(old_err); close(old_err);

	// Theoretically I should do loads of error checking, but
	// the calls aren't very likely to fail, and that would
	// muddy up the example quite a bit. YMMV.

	return pid;
}


status_t
CommandThread::SuspendCommand()
{
	thread_info info;
	status_t status = get_thread_info(fThreadId, &info);

	if (status == B_OK)
		return send_signal(-fThreadId, SIGSTOP);
	else
		return status;
}


status_t
CommandThread::ResumeCommand()
{
	thread_info info;
	status_t status = get_thread_info(fThreadId, &info);

	if (status == B_OK)
		return send_signal(-fThreadId, SIGCONT);
	else
		return status;
}


status_t
CommandThread::InterruptCommand()
{
	thread_info info;
	status_t status = get_thread_info(fThreadId, &info);

	if (status == B_OK) {
		status = send_signal(-fThreadId, SIGINT);
		WaitOnCommand();
	}

	return status;
}


status_t
CommandThread::WaitOnCommand()
{
	thread_info info;
	status_t status = get_thread_info(fThreadId, &info);

	if (status == B_OK)
	{
		status = wait_for_thread(fThreadId, &status);
		//BeginUnit(); // forces a wait if there's output to process???
		//EndUnit();
	}
	//else
		return status;
}
