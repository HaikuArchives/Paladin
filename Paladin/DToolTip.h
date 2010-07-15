#ifndef DTOOLTIP_H
#define DTOOLTIP_H

#include <Window.h>
#include <View.h>

using namespace std;

class DTipWindow;

class DToolTip
{
public:
						DToolTip(void);
	virtual				~DToolTip(void);

			void		SetTip(BView *view, const char *text);
			void		EnableTips(bool enable = true);
			
			void		SetTipDelay(bigtime_t delay);
			bigtime_t	GetTipDelay(void);
			
	static	DToolTip *	GetInstance(void);
	
private:
			void		MakeValid(void);
			
			DTipWindow	*fTipWindow;
};

#endif
