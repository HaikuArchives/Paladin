#ifndef DTEXTVIEW_H
#define DTEXTVIEW_H

#include <Invoker.h>
#include <TextView.h>
#include <ScrollView.h>

enum
{
	M_TEXT_CHANGED = 'txch'
};

class DTextView : public BTextView, public BInvoker
{
public:
								DTextView(const BRect &frame, const char *name,
										const int32 &resize);
	
	virtual	void				FrameResized(float w, float h);

	virtual	void				InsertText(const char* inText, int32 inLength,
									int32 inOffset,
									const text_run_array* inRuns);
	
	virtual	void				DeleteText(int32 fromOffset, int32 toOffset);
	
			void				SetChangeNotifications(bool value);
			bool				IsNotifyingChanges(void) const;
	
			BScrollView *		MakeScrollView(const char *name, bool horizontal,
												bool vertical);
private:
			void	UpdateTextRect(void);
			
			bool	fNotifyChanges;
};


#endif
