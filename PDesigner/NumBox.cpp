#include "NumBox.h"
#include "TimeSupport.h"

NumBox::NumBox(const BRect &frame, const char *name, const char *label,
			const char *text, BMessage *msg, uint32 resize, uint32 flags)
 :	AutoTextControl(frame,name,label,text,msg,resize,flags),
 	fFilterTab(true),
 	fAllowNegatives(true)
{

	const char num_disallowed[]=" `~!@#%^&*()_+=QWERTYUIOP{[}]|\\ASDFGHJKL;:'\""
								"ZXCVBNM<>?/qwertyuiopasdfghjklzxcvbnm$¥£";
	int32 i=0;
	while(num_disallowed[i])
	{
		TextView()->DisallowChar(num_disallowed[i]);
		i++;
	}
}

bool NumBox::Validate(bool alert)
{
	if(strlen(Text())<1)
		SetText("0");

	return true;
}

void NumBox::AllowNegatives(const bool &value)
{
	if(fAllowNegatives != value)
	{
		fAllowNegatives = value;
		if(fAllowNegatives)
			TextView()->AllowChar('-');
		else
			TextView()->DisallowChar('-');
	}
}
