/*
 * Copyright 2005-2008, Pe Development Team. All Rights Reserved.
 * Licensed under the terms of the MIT license.
 */
#ifndef PADDON_H
#define PADDON_H


#include <Handler.h>

class BBitmap;
class BString;
class BMenuItem;
class BView;

class PDoc;
class PText;
class PExtCmd;
class PAddOn;


class PEditText
{
public:
					PEditText(PAddOn* addOn, const char *action);
					~PEditText();

	const char*		Text();
	int32			TextLength() const;

	void			GetSelection(int32* _start, int32* _end) const;
	void			Select(int32 newStart, int32 newEnd);
	void			SelectCurrentWord();
	void			Delete();

	void			Insert(const char* text);
	void			Insert(const char* text, int32 length);
	void			Insert(BString& string);

private:
	PText&			fText;
	PExtCmd*		fCommand;
	bool			fIsDirty;
};

class PAddOn : public BHandler
{
public:
					PAddOn(const char *name);
	virtual			~PAddOn();

	/* BHandler methods */
	virtual void	MessageReceived(BMessage* message);

	/* own methods */
	virtual void	AttachedToDocument();
	virtual void	DetachedFromDocument();

	virtual void	LanguageChanged(BMessage& message);

protected:
	/* buttons/menus */
	void*			AddButton(BBitmap& bitmap, const char* toolTip, uint32 kind,
						BMessage* message);
	void*			AddMenuItem(const char* label, uint32 kind, BMessage* message);
	void*			AddMenuItem(BMenuItem* item);

	void			RemoveButton(void* handle);
	void			RemoveMenuItem(void* handle);

	/* text & language support */
	status_t		Perform(BMessage& message);
	status_t		LanguagePerform(BMessage& message);
	bool			LanguageSupports(uint32 code);

private:
	friend class PDoc;
	friend class PEditText;

	void			SetContext(PDoc* document, PText* text);
	PText&			Text();

private:
	PDoc*			fDocument;
	PText*			fText;
};


#ifdef __cplusplus
extern "C" {
#endif

extern PAddOn* new_pe_add_on(void);
extern BView* new_pe_prefs_view(void);

struct key_binding {
	uint8		key;
	uint32		modifier;
	BMessage*	message;
};

extern void get_pe_key_bindings(key_binding** _bindings);

#ifdef __cplusplus
}
#endif

#endif	// PADDON_H
