#include "DelayedMessenger.h"


DelayedMessenger::DelayedMessenger(const BHandler *handler,
								const BLooper *looper, status_t *error)
	:	fMessenger(handler,looper,error),
		fRunner(NULL),
		fSendTime(0)
{
	
}


DelayedMessenger::DelayedMessenger(const char *signature, team_id team,
								status_t *error)
	:	fMessenger(signature,team,error),
		fRunner(NULL),
		fSendTime(0)
{
}


DelayedMessenger::~DelayedMessenger(void)
{
	delete fRunner;
}


void
DelayedMessenger::SetTarget(const BMessenger &target)
{
	fMessenger = target;
}


BMessenger
DelayedMessenger::Target(void) const
{
	return fMessenger;
}


status_t
DelayedMessenger::SendMessage(const BMessage &msg, const uint32 &delay_seconds)
{
	delete fRunner;
	fMessage = msg;
	
	fSendTime = real_time_clock() + delay_seconds;
	
	fRunner = new BMessageRunner(fMessenger,&fMessage,delay_seconds * 1000000, 1);
	return fRunner->InitCheck();
}


uint32
DelayedMessenger::SendTime(void) const
{
	 return fSendTime;
}

