#ifndef MISCPROPERTIES_H
#define MISCPROPERTIES_H

#include "EnumProperty.h"
#include <Window.h>

class WindowLookProperty : public EnumProperty
{
public:
							WindowLookProperty(void);
							WindowLookProperty(const char *name, window_look look,
												const char *description = NULL);
							WindowLookProperty(PValue *value);
							WindowLookProperty(BMessage *msg);
	
	static	PProperty *		Create(void);
	virtual	PProperty *		Duplicate(void);
	
	static	BArchivable *	Instantiate(BMessage *data);

private:
			void			Init(void);
};


class WindowFeelProperty : public EnumProperty
{
public:
							WindowFeelProperty(void);
							WindowFeelProperty(const char *name, window_feel feel,
												const char *description = NULL);
							WindowFeelProperty(PValue *value);
							WindowFeelProperty(BMessage *msg);
	
	static	PProperty *		Create(void);
	virtual	PProperty *		Duplicate(void);
	
	static	BArchivable *	Instantiate(BMessage *data);

private:
			void			Init(void);
};

class WindowFlagsProperty : public EnumFlagProperty
{
public:
							WindowFlagsProperty(void);
							WindowFlagsProperty(const char *name, int32 flags,
												const char *description = NULL);
							WindowFlagsProperty(PValue *value);
							WindowFlagsProperty(BMessage *msg);
	
	static	PProperty *		Create(void);
	virtual	PProperty *		Duplicate(void);
	
	static	BArchivable *	Instantiate(BMessage *data);

private:
			void			Init(void);
};

class ViewFlagsProperty : public EnumFlagProperty
{
public:
							ViewFlagsProperty(void);
							ViewFlagsProperty(const char *name, int32 flags,
												const char *description = NULL);
							ViewFlagsProperty(PValue *value);
							ViewFlagsProperty(BMessage *msg);
	
	static	PProperty *		Create(void);
	virtual	PProperty *		Duplicate(void);
	
	static	BArchivable *	Instantiate(BMessage *data);

private:
			void			Init(void);
};


class ResizeModeProperty : public EnumFlagProperty
{
public:
							ResizeModeProperty(void);
							ResizeModeProperty(const char *name, int32 resize,
												const char *description = NULL);
							ResizeModeProperty(PValue *value);
							ResizeModeProperty(BMessage *msg);
	
	static	PProperty *		Create(void);
	virtual	PProperty *		Duplicate(void);
	
	static	BArchivable *	Instantiate(BMessage *data);

private:
			void			Init(void);
};


#endif
