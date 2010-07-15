#ifndef BUILDOPTS_H
#define BUILDOPTS_H

class CompileOptions
{
	void		AddLocalInclude(const char *path);
	void		RemoveLocalInclude(const char *path);
	bool		HasLocalInclude(const char *path);
	int32		CountLocalIncludes(void) const;
	const char *LocalIncludeAt(const int32 &index);
	
	void		AddSystemInclude(const char *path);
	void		RemoveSystemInclude(const char *path);
	bool		HasSystemInclude(const char *path);
	int32		CountSystemIncludes(void) const;
	const char *SystemIncludeAt(const int32 &index);

	void		SetDebug(bool value) { fDebug = value; }
	bool		Debug(void) const { return fDebug; }
	
	void		SetProfiling(bool value) { fProfile = value; }
	bool		Profiling(void) const { return fProfile; }
	
	void		SetOpForSize(bool value) { fOpSize = value; }
	bool		OpForSize(void) const { return fOpSize; }
	
	void		SetOpLevel(uint8 level);
	uint8		OpLevel(void) const { return fOpLevel; }
	
	void		SetTargetType(int32 type) { fTargetType = type; }
	int32		TargetType(void) const { return fTargetType; }
	
	void		SetExtraCompilerOptions(const char *opt) { fExtraCompilerOptions = opt; }
	const char *ExtraCompilerOptions(void) { return fExtraCompilerOptions.String(); }
	

	
	BObjectList<BString>		fLocalIncludeList,
								fSystemIncludeList;
	
	bool		fDebug;
	bool		fProfile;
	bool		fOpSize;
	uint8		fOpLevel;
	
	BString		fExtraCompilerOptions;
};


class LinkOptions
{
	void		SetExtraLinkerOptions(const char *opt) { fExtraLinkerOptions = opt; }
	const char *ExtraLinkerOptions(void) { return fExtraLinkerOptions.String(); }
	
	BString		fExtraLinkerOptions;
};


#endif
