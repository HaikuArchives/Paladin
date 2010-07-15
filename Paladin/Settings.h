#ifndef SETTINGS_H
#define SETTINGS_H

#include <Locker.h>
#include <Message.h>
#include <String.h>
#include <GraphicsDefs.h>

class Settings : public BMessage, public BLocker
{
public:
					Settings(const char *path = NULL);
					
	void			SetPath(const char *path);
	const char *	GetPath(void);
	status_t		Load(const char *path);
	status_t		Save(const char *path = NULL);
	
	void			SetBool(const char *name, bool value);
	bool			GetBool(const char *name, bool failsafe);
	
	void			SetInt8(const char *name, int8 value);
	int8			GetInt8(const char *name, int8 failsafe);
	
	void			SetInt32(const char *name, int32 value);
	int32			GetInt32(const char *name, int32 failsafe);
	
	void			SetFloat(const char *name, float value);
	float			GetFloat(const char *name, float failsafe);
	
	void			SetRect(const char *name, BRect value);
	BRect			GetRect(const char *name, BRect failsafe);
	
	void			SetPoint(const char *name, BPoint value);
	BPoint			GetPoint(const char *name, BPoint failsafe);
	
	void			SetColor(const char *name, rgb_color value);
	rgb_color		GetColor(const char *name, rgb_color failsafe);
	
	void			SetString(const char *name, const char *value);
	void			SetString(const char *name, BString value);
	BString			GetString(const char *name, const char *failsafe);

private:
	BString			fPath;
};

extern Settings gSettings;

#endif
