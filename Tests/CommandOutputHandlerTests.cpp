#include <UnitTest++/UnitTest++.h>

#include <string>
#include <sstream>
#include <vector>
#include <iostream>

#include <Looper.h>

#include "CommandThread.h"
#include "CommandOutputHandler.h"

SUITE(CommandOutputHandler)
{

	TEST(StdOut) 
	{
		BMessage cmd;
		cmd.AddString("cmd","echo 'first'; echo 'second'; echo 'third'");
		
		CommandOutputHandler handler(false);
		CHECK(!handler.IsErrRedirectedToOut());
		
		BLooper* looper = new BLooper();
		looper->AddHandler(&handler);
		BMessenger msgr(&handler,looper);
		thread_id looperThread = looper->Run();
		CHECK(looperThread > 0);
		
		CommandThread thread(&cmd,&msgr);
		status_t startStatus = thread.Start();
		CHECK_EQUAL(startStatus,B_OK);
		
		status_t okReturn = B_OK;
		status_t waitStatus = thread.WaitForThread(&okReturn);
		CHECK_EQUAL(waitStatus,B_OK);
		
		std::string expected("first\nsecond\nthird\n");
		std::string actual(handler.GetOut());
		CHECK_EQUAL(expected,actual);
		
		std::string expectedErr("");
		std::string actualErr(handler.GetErr());
		CHECK_EQUAL(expectedErr,actualErr);
	}
	

	TEST(StdErr) 
	{
		BMessage cmd;
		cmd.AddString("cmd",">&2 echo 'first'; >&2 echo 'second'; >&2 echo 'third'");
		
		CommandOutputHandler handler(false);
		CHECK(!handler.IsErrRedirectedToOut());
		
		BLooper* looper = new BLooper();
		looper->AddHandler(&handler);
		BMessenger msgr(&handler,looper);
		thread_id looperThread = looper->Run();
		CHECK(looperThread > 0);
		
		CommandThread thread(&cmd,&msgr);
		status_t startStatus = thread.Start();
		CHECK_EQUAL(startStatus,B_OK);
		
		status_t okReturn = B_OK;
		status_t waitStatus = thread.WaitForThread(&okReturn);
		CHECK_EQUAL(waitStatus,B_OK);
		
		std::string expected("");
		std::string actual(handler.GetOut());
		CHECK_EQUAL(expected,actual);
		
		std::string expectedErr("first\nsecond\nthird\n");
		std::string actualErr(handler.GetErr());
		CHECK_EQUAL(expectedErr,actualErr);
	}
	
	

	TEST(StdErrRedirect) 
	{
		BMessage cmd;
		cmd.AddString("cmd",">&2 echo 'first'; >&2 echo 'second'; >&2 echo 'third'");
		
		CommandOutputHandler handler(true); // enable redirect
		CHECK(handler.IsErrRedirectedToOut());
		
		BLooper* looper = new BLooper();
		looper->AddHandler(&handler);
		BMessenger msgr(&handler,looper);
		thread_id looperThread = looper->Run();
		CHECK(looperThread > 0);
		
		CommandThread thread(&cmd,&msgr);
		status_t startStatus = thread.Start();
		CHECK_EQUAL(startStatus,B_OK);
		
		status_t okReturn = B_OK;
		status_t waitStatus = thread.WaitForThread(&okReturn);
		CHECK_EQUAL(waitStatus,B_OK);
		
		std::string expected("first\nsecond\nthird\n");
		std::string actual(handler.GetOut());
		CHECK_EQUAL(expected,actual);
		
		std::string expectedErr("");
		std::string actualErr(handler.GetErr());
		CHECK_EQUAL(expectedErr,actualErr);
	}
}
