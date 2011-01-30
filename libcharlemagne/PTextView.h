#ifndef PTEXTVIEW_H
#define PTEXTVIEW_H

#include <TextView.h>
#include "PView.h"

/*
	PTextView
		Properties:
			All PView Properties
			Alignment
			AutoIndent
			ColorSpace
			CurrentLine
			Editable
			FontColor
			FontFace
			FontName
			HideTyping
			LineCount (read-only)
			MaxBytes
			Resizable
			Selectable
			Stylable
			TabWidth
			Text
			TextLength (read-only)
			TextRect
			Undoable
			UndoState
			UseWordWrap
		
		Methods
			All PView Methods
			AllowChars
			ByteAt
			CanEndLine
			Cut
			Copy
			Delete
			DisallowChars
			FindWord
			GetInsets
			GetSelection
			GetText
			Highlight
			Insert
			LineAt
			LineHeight
			LineWidth
			OffsetAt
			Paste
			PointAt
			ScrollToOffset
			ScrollToSelection
			Select
			SelectAll
			SetInsets
			TextHeight
			Undo
			
		Events
			All PView Events
*/

class PTextView : public PView
{
public:
							PTextView(void);
							PTextView(BMessage *msg);
							PTextView(const char *name);
							PTextView(const PTextView &from);
							~PTextView(void);
			
	static	BArchivable *	Instantiate(BMessage *data);

	virtual	status_t		GetProperty(const char *name, PValue *value, const int32 &index = 0) const;
	virtual	status_t		SetProperty(const char *name, PValue *value, const int32 &index = 0);
	
	static	PObject *		Create(void);
	virtual	PObject *		Duplicate(void) const;
	
protected:
	virtual void			InitBackend(BView *view = NULL);

private:
	void					InitProperties(void);
	void					InitMethods(void);
};


#endif
