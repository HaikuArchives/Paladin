#include "CommandOutputHandler.h"

#include <string>
#include <iostream>

#include <Handler.h>

#include "CommandThread.h"

CommandOutputHandler::CommandOutputHandler(bool reto)
	: BHandler(),
	  out(),
	  err(),
	  redirectErrToOut(reto)
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
		default:
		{
			BHandler::MessageReceived(msg);
		}
	}
}
