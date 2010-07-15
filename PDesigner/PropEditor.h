#ifndef PROPEDITOR_H
#define PROPEDITOR_H

#include <View.h>
#include "ObjectList.h"

class PObject;
class PProperty;

enum
{
	M_PROPERTY_EDITED = 'prpe'
};

// Base class for property editing classes
class PropertyEditor : public BView
{
public:
								PropertyEditor(PObject *obj, PProperty *prop);
	virtual						~PropertyEditor(void);
								
	virtual	void				SetTarget(BHandler *handler, BLooper *looper = NULL);
	virtual	PropertyEditor *	CreateInstance(PObject *obj, PProperty *prop);
	
	virtual	void				Update(void);
	virtual	status_t			SetProperty(PObject *obj, PProperty *prop);
			PObject *			GetObject(void) const;
			PProperty *			GetProperty(void) const;
	virtual	bool				HandlesType(const BString &type);

private:
			friend class PropertyEditorBroker;
			
			bool				InUse(void) const;
			void				SetInUse(bool value);
			
			
			PObject				*fObject;
			PProperty			*fProperty;
			
			bool				fInUse;
};

class PropertyEditorBroker
{
public:
									PropertyEditorBroker(void);
									~PropertyEditorBroker(void);
			
			PropertyEditor *		GetEditorForProperty(PObject *obj, const char *propName);
			PropertyEditor *		GetEditorForProperty(PObject *obj, PProperty *prop);
			
			void					DetachAllEditors(void);
			
	static	PropertyEditorBroker * const	GetBrokerInstance(void);
	
private:
			void					LoadClasses(void);
			PropertyEditor *		FindAvailableEditor(PObject *obj, PProperty *prop);
			
	BObjectList<PropertyEditor>		*fClassList;
	BObjectList<PropertyEditor>		*fEditorList;
};

PropertyEditor *		GetEditorForProperty(PObject *obj, const char *propName);
PropertyEditor *		GetEditorForProperty(PObject *obj, PProperty *prop);

#endif
