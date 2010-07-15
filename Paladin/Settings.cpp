#include "Settings.h"
#include <Autolock.h>
#include <File.h>

Settings gSettings;

Settings::Settings(const char *path)
	:	BLocker("Settings")
{
	if (path)
		Load(path);
}


void
Settings::SetPath(const char *path)
{
	fPath = path;
}


const char *
Settings::GetPath(void)
{
	Lock();
	BString temp = fPath;
	Unlock();
	return temp.String();
}


status_t
Settings::Load(const char *path)
{
	BAutolock lock(this);
	
	if (path)
		fPath = path;
	
	if (fPath.CountChars() < 1)
		return B_NO_INIT;
	
	BFile file(fPath.String(),B_READ_ONLY);
	if (file.InitCheck() != B_OK)
		return file.InitCheck();
	
	return gSettings.Unflatten(&file);
}


status_t
Settings::Save(const char *path)
{
	BAutolock lock(this);
	
	if (path)
		fPath = path;
	
	if (fPath.CountChars() < 1)
		return B_NO_INIT;
	
	BFile file(fPath.String(),B_READ_WRITE | B_CREATE_FILE | 
															B_ERASE_FILE);
	if (file.InitCheck() != B_OK)
		return file.InitCheck();
	return gSettings.Flatten(&file);
}


void
Settings::SetBool(const char *name, bool value)
{
	gSettings.Lock();
	if (gSettings.ReplaceBool(name,value) != B_OK)
		gSettings.AddBool(name,value);
	gSettings.Unlock();
}


bool
Settings::GetBool(const char *name, bool failsafe)
{
	gSettings.Lock();
	bool temp;
	if (FindBool(name,&temp) != B_OK)
		temp = failsafe;
	gSettings.Unlock();
	return temp;
}


void
Settings::SetInt8(const char *name, int8 value)
{
	gSettings.Lock();
	if (gSettings.ReplaceInt8(name,value) != B_OK)
		gSettings.AddInt8(name,value);
	gSettings.Unlock();
}


int8
Settings::GetInt8(const char *name, int8 failsafe)
{
	gSettings.Lock();
	int8 temp;
	if (FindInt8(name,&temp) != B_OK)
		temp = failsafe;
	gSettings.Unlock();
	return temp;
}


void
Settings::SetInt32(const char *name, int32 value)
{
	gSettings.Lock();
	if (gSettings.ReplaceInt32(name,value) != B_OK)
		gSettings.AddInt32(name,value);
	gSettings.Unlock();
}


int32
Settings::GetInt32(const char *name, int32 failsafe)
{
	gSettings.Lock();
	int32 temp;
	if (FindInt32(name,&temp) != B_OK)
		temp = failsafe;
	gSettings.Unlock();
	return temp;
}


void
Settings::SetFloat(const char *name, float value)
{
	gSettings.Lock();
	if (gSettings.ReplaceFloat(name,value) != B_OK)
		gSettings.AddFloat(name,value);
	gSettings.Unlock();
}


float
Settings::GetFloat(const char *name, float failsafe)
{
	gSettings.Lock();
	float temp;
	if (FindFloat(name,&temp) != B_OK)
		temp = failsafe;
	gSettings.Unlock();
	return temp;
}


void
Settings::SetRect(const char *name, BRect value)
{
	gSettings.Lock();
	if (gSettings.ReplaceRect(name,value) != B_OK)
		gSettings.AddRect(name,value);
	gSettings.Unlock();
}


BRect
Settings::GetRect(const char *name, BRect failsafe)
{
	gSettings.Lock();
	BRect temp;
	if (FindRect(name,&temp) != B_OK)
		temp = failsafe;
	gSettings.Unlock();
	return temp;
}


void
Settings::SetPoint(const char *name, BPoint value)
{
	gSettings.Lock();
	if (gSettings.ReplacePoint(name,value) != B_OK)
		gSettings.AddPoint(name,value);
	gSettings.Unlock();
}


BPoint
Settings::GetPoint(const char *name, BPoint failsafe)
{
	gSettings.Lock();
	BPoint temp;
	if (FindPoint(name,&temp) != B_OK)
		temp = failsafe;
	gSettings.Unlock();
	return temp;
}


void
Settings::SetColor(const char *name, rgb_color value)
{
	int32 colorval = (value.red << 24) | (value.green << 16) | 
						(value.blue << 8) | (value.alpha);
	
	gSettings.Lock();
	if (gSettings.ReplaceInt32(name,colorval) != B_OK)
		gSettings.AddInt32(name,colorval);
	gSettings.Unlock();
}


rgb_color
Settings::GetColor(const char *name, rgb_color failsafe)
{
	gSettings.Lock();
	int32 temp;
	rgb_color returnval;
	if (FindInt32(name,&temp) != B_OK)
		returnval = failsafe;
	else
	{
		returnval.red = (temp >> 24) & 0xff;
		returnval.green = (temp >> 16) & 0xff;
		returnval.blue = (temp >> 8) & 0xff;
		returnval.alpha = temp & 0xff;
	}
	gSettings.Unlock();
	
	return returnval;
}


void
Settings::SetString(const char *name, const char *value)
{
	SetString(name,BString(value));
}


void
Settings::SetString(const char *name, BString value)
{
	gSettings.Lock();
	if (gSettings.ReplaceString(name,value) != B_OK)
		gSettings.AddString(name,value);
	gSettings.Unlock();
}


BString
Settings::GetString(const char *name, const char *failsafe)
{
	gSettings.Lock();
	BString temp;
	if (FindString(name,&temp) != B_OK)
		temp = failsafe;
	gSettings.Unlock();
	return temp;
}

