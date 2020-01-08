#include "CommandOutputHandler.h"

#include <string>
#include <iostream>

#include <Handler.h>

#include "CommandThread.h"

CommandOutputHandler::CommandOutputHandler(bool reto)
	: BHandler(),
	  out(),
	  err(),
	  redirectErrToOut(reto),
	  exited(false),
	  failed(false)
{
	;
}

CommandOutputHandler::~CommandOutputHandler()
{
	;
}

std::string
CommandOutputHandler::GetOut() const
{
	return std::string(out);
}

std::string
CommandOutputHandler::GetErr() const
{
	return std::string(err);
}

bool
CommandOutputHandler::IsErrRedirectedToOut() const
{
	return redirectErrToOut;
}

bool 
CommandOutputHandler::HasExited() const
{
	return exited;
}

void
CommandOutputHandler::WaitForExit() const
{
	while (!HasExited()) {
		snooze(100000);
	}
}

bool
CommandOutputHandler::HasFailed() const
{
	return failed;
}

void
CommandOutputHandler::MessageReceived(BMessage* msg)
{
	//std::cout << "CommandOutputHandler::MessageReceived" << std::endl;
	BString content;
	switch (msg->what)
	{
		case M_COMMAND_RECEIVE_STDOUT:
		{
			//std::cout << "received std out" << std::endl;
			if (B_OK == msg->FindString("output", &content))
			{
				//std::cout << "Content: " << content << std::endl;
				out += content;
			}
		}
		case M_COMMAND_RECEIVE_STDERR:
		{
			//std::cout << "received std err" << std::endl;
			if (redirectErrToOut)
			{
				if (B_OK == msg->FindString("error", &content))
				{
					//std::cout << "Content: " << content << std::endl;
					out += content;
				}
			} else {
				if (B_OK == msg->FindString("error", &content))
				{
					//std::cout << "Content: " << content << std::endl;
					err += content;
				}
			}
			break;
		}
		case M_COMMAND_AWAITING_QUIT:
		{
			std::cout << "Awaiting quit" << std::endl;
			break;
		}
		case M_COMMAND_EXITED:
		{
			exited = true;
			std::cout << "Command exited normally" << std::endl;
			break;
		}
		case M_COMMAND_EXITED_IN_ERROR:
		{
			exited = true;
			failed = true;
			std::cout << "Command exited in error" << std::endl;
			break;
		}
		default:
		{
			BHandler::MessageReceived(msg);
		}
	}
}
