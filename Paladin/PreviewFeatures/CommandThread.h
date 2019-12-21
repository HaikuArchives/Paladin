/*
 * Copyright 2004-2010, Jérôme Duval. All rights reserved.
 * Copyright 2019, Haiku Inc. All rights reserved
 * Distributed under the terms of the MIT License.
 * Original code from ZipOMatic by jonas.sundstrom@kirilla.com
 * Code extracted from Expander and made more general purpose
 *   in 2019 by Adam Fowler
 *			2019	Adam Fowler, adamfowleruk@gmail.com
 */
#ifndef COMMANDTHREAD_H
#define COMMANDTHREAD_H

#include <Message.h>
#include <Volume.h>
#include <String.h>
#include <OS.h>
#include <FindDirectory.h>

#include "GenericThread.h"
#include <stdio.h>
#include <stdlib.h>

enum {
	M_COMMAND_RECEIVE_STDOUT = 'outp',
	M_COMMAND_RECEIVE_STDERR = 'errp',
	M_COMMAND_EXITED         = 'exit',
	M_COMMAND_EXITED_IN_ERROR = 'exrr'
};

extern const char* CommandThreadName;


class CommandThread : public GenericThread {
public:
								CommandThread(BMessage* refs_message,
									BMessenger* messenger);
								~CommandThread();

			status_t			SuspendCommand();
			status_t			ResumeCommand();
			status_t			InterruptCommand();
			status_t			WaitOnCommand();

			void				PushInput(BString text);

private:
	virtual	status_t			ThreadStartup();
	virtual	status_t			ExecuteUnit();
	virtual	status_t			ThreadShutdown();

	virtual	void				ThreadStartupFailed(status_t a_status);
	virtual	void				ExecuteUnitFailed(status_t a_status);
	virtual	void				ThreadShutdownFailed(status_t a_status);

			status_t			ProcessRefs(BMessage* message);

			thread_id			PipeCommand(int argc, const char** argv,
									int& in, int& out, int& err,
									const char** envp = (const char**)environ);

			BMessenger*			fWindowMessenger;

			thread_id			fThreadId;
			int					fStdIn;
			int					fStdOut;
			int					fStdErr;
			FILE*				fThreadOutput;
			FILE*				fThreadError;
			char				fThreadOutputBuffer[LINE_MAX];
};


#endif
