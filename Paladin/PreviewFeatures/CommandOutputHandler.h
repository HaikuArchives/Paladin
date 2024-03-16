#ifndef COMMANDOUTPUTHANDLER_H
#define COMMANDOUTPUTHANDLER_H

#include <string>

#include <Handler.h>


#if !defined(__cplusplus) || __cplusplus < 201103L
#define override
#endif

/*!
	\class CommandOutputHandler
	\brief Acts as a capture point for CommandThread output.
	
	Should only be used where you have to be in a synchronous execition
	environment.
 */
class CommandOutputHandler : public BHandler
{
public:
											CommandOutputHandler(bool redirectErrToOut = false);
	virtual									~CommandOutputHandler();
	
	std::string								GetOut() const;
	std::string								GetErr() const;
	bool									IsErrRedirectedToOut() const;
	bool									HasExited() const;
	void									WaitForExit() const;
	bool									HasFailed() const;
	
	// BHandler
	virtual void							MessageReceived(BMessage* msg) override;

protected:


private:
	std::string								out;
	std::string								err;
	bool									redirectErrToOut;
	bool									exited;
	bool									failed;

};


#endif
