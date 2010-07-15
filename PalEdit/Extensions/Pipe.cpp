/*	$Id: Pipe.cpp,v 1.1 2008/12/20 23:35:48 darkwyrm Exp $

	Copyright 1996, 1997, 1998, 2002
	        Hekkelman Programmatuur B.V.  All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:
	1. Redistributions of source code must retain the above copyright notice,
	   this list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright notice,
	   this list of conditions and the following disclaimer in the documentation
	   and/or other materials provided with the distribution.
	3. All advertising materials mentioning features or use of this software
	   must display the following acknowledgement:

	    This product includes software developed by Hekkelman Programmatuur B.V.

	4. The name of Hekkelman Programmatuur B.V. may not be used to endorse or
	   promote products derived from this software without specific prior
	   written permission.

	THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
	FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
	AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
	PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
	OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
	WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Created: 09/15/97 02:33:13
*/

#include "PeAddOn.h"
#include "Pipe.dlog.r.h"
#include <cerrno>

#if __INTEL__
extern "C" _EXPORT long perform_edit(MTextAddOn *addon);
#else
#pragma export on
extern "C" {
long perform_edit(MTextAddOn *addon);
}
#pragma export reset
#endif

long Pipe(MTextAddOn *addon);
long SuffixLines(MTextAddOn *addon);

static bool sOK = false;
static char *sText = NULL;

const unsigned long
	kWindowWidth = 250,
	kWindowHeight = 125,
	kMsgOK = 'ok  ',
	kMsgCancel = 'cncl',
	kBufferSize = 1024;

const rgb_color
	kViewColor = { 0xDB, 0xDB, 0xDB, 0 };

class CPipeDialog : public HDialog {
public:
		enum { sResID = 1 };

		CPipeDialog(BRect frame, const char *name, window_type type, int flags,
			BWindow *owner, BPositionIO* data);

virtual bool OkClicked();
virtual bool CancelClicked();
};

CPipeDialog::CPipeDialog(BRect frame, const char *name, window_type type, int flags,
			BWindow *owner, BPositionIO* data)
	: HDialog(frame, name, type, flags, owner, data)
{
	FindView("command")->MakeFocus(true);
} /* CPipeDialog::CPipeDialog */

bool CPipeDialog::OkClicked()
{
	sOK = true;
	sText = strdup(GetText("command"));
	return true;
} /* CPipeDialog::OkClicked */

bool CPipeDialog::CancelClicked()
{
	sOK = false;
	sText = NULL;
	return true;
} /* CPipeDialog::CancelClicked */

long perform_edit(MTextAddOn *addon)
{
	long result = B_NO_ERROR;

	try
	{
		long s, e, l;
		addon->GetSelection(&s, &e);

		BMemoryIO tmpl(kDLOG1, kDLOG1Size);
		CPipeDialog *p = DialogCreator<CPipeDialog>::CreateDialog(addon->Window(), tmpl);
		p->Show();

		wait_for_thread(p->Thread(), &l);
		if (sOK)
			result = Pipe(addon);

		if (sText)
			free(sText);
	}
	catch (HErr& e)
	{
		e.DoError();
		result = B_ERROR;
	}

	return result;
} /* perform_edit */

long Pipe(MTextAddOn *addon)
{
	char fn[PATH_MAX];
	long start, end;

	addon->GetSelection(&start, &end);

	tmpnam(fn);
	FILE *f = fopen(fn, "wb");
	if (!f) return B_ERROR;

	fwrite(addon->Text() + start, end - start, 1, f);
	fclose(f);

	int ofd[2], pid, err;

	try
	{
		FailOSErr(pipe(ofd));

		pid = fork();
		if (pid < 0) FailOSErr(pid);

		if (pid == 0)		// child
		{
			fflush(stdout);
			close(STDOUT_FILENO);
			err = dup(ofd[1]);		// What will happen if this fails???
			if (err < 0) throw HErr(err);

			fflush(stderr);
			close(STDERR_FILENO);
			err = dup(ofd[1]);
			if (err < 0) throw HErr(err);

			close(ofd[0]);
			close(ofd[1]);

			char cmd[1024];
			sprintf(cmd, "cat %s | %s", fn, sText);

			char *args[4];

			args[0] = "/bin/sh";
			args[1] = "-c";
			args[2] = cmd;
			args[3] = NULL;

			if (execvp(args[0], args) < 0)
				beep();	// what else can we do???
		}
		else if (pid >= 0)	// parent
		{
			close(ofd[1]);

			char buf[kBufferSize + 1];
			int flags;
			bool prepared = false;

			fcntl(ofd[0], F_GETFL, &flags);
			fcntl(ofd[0], F_SETFL, flags | O_NONBLOCK);

			int r;

			while ((r = read(ofd[0], buf, kBufferSize)) != 0)
			{
				if (r > 0)
				{
					if (!prepared)
					{
						prepared = true;
						addon->Delete();
						addon->Select(start, start);
						end = start;
					}

					addon->Insert(buf, r);
					end += r;
					addon->Select(end, end);
				}
				else if (errno == EAGAIN)
					snooze(100000);
				else
					break;
			}
		}

		close(ofd[0]);
//		remove(fn);
	}
	catch (HErr e)
	{
		e.DoError();
	}

	return 0;
} /* Pipe */
