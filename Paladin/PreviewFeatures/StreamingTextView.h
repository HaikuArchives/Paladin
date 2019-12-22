#ifndef STREAMINGTEXTVIEW_H
#define STREAMINGTEXTVIEW_H

#include <View.h>

class BTextView;
class BScrollBar;

class StreamingTextView : public BView
{
public:
								StreamingTextView(const char* name);
	virtual						~StreamingTextView();
	
	void						Append(BString& txt);

protected:


private:
	BTextView*					fText;
	BScrollBar*					fTextScroll;

};

#endif
