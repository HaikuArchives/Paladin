#ifndef CLICKABLE_STRING_VIEW_H
#define CLICKABLE_STRING_VIEW_H

#include <Invoker.h>
#include <StringView.h>

class ClickableStringView : public BStringView, public BInvoker
{
public:
					ClickableStringView(BRect frame, const char *name,
										const char *text,
										uint32 resize = B_FOLLOW_LEFT |
														B_FOLLOW_TOP,
										uint32 flags = B_WILL_DRAW);
	virtual	void	MouseUp(BPoint pt);
	virtual	void	AttachedToWindow(void);
	virtual	void	DetachedFromWindow(void);
};

#endif
