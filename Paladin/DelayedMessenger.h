#ifndef DELAYED_MSG_H
#define DELAYED_MSG_H

#include <MessageRunner.h>
#include <Messenger.h>
#include <Message.h>
#include <time.h>

class DelayedMessenger
{
public:
				DelayedMessenger(const BHandler *handler,
								const BLooper *looper = NULL,
								status_t *error = NULL);
				DelayedMessenger(const char *signature, team_id team = -1,
								status_t *error = NULL);
	
				~DelayedMessenger(void);
	
	void		SetTarget(const BMessenger &target);
	BMessenger	Target(void) const;
	
	status_t	SendMessage(const BMessage &msg, const uint32 &delay_seconds);
	
	uint32		SendTime(void) const;
	
private:
	BMessage		fMessage;
	BMessenger		fMessenger;
	BMessageRunner	*fRunner;
	uint32			fSendTime;
};

#endif
