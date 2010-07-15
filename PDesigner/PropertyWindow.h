#ifndef PROPERTYWINDOW_H
#define PROPERTYWINDOW_H

#include <Window.h>

class AutoTextControl;
class PObject;

enum
{
	M_SET_OBJECT = 'stob'
};

class PropertyWindow : public BWindow
{
public:
						PropertyWindow(void);
						~PropertyWindow(void);
	bool				QuitRequested(void);
	void				MessageReceived(BMessage *msg);
	
private:
	void				SetObject(const uint64 &id);
	void				UpdateEditor(const uint64 &id, const BString &name);
	
	BView				*fTop;
	AutoTextControl		*fNameBox;
	float				fTextBoxHeight;
	PObject				*fObject;
};

#endif
