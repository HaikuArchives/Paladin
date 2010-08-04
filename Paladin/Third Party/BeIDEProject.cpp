/*
	BeIDEProject.cpp: a class for reading BeIDE projects
	©2010 DarkWyrm
	
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:
	
	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.
	
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include "BeIDEProject.h"

#include <ByteOrder.h>
#include <File.h>
#include <stdio.h>
#include <string.h>

// Define this if you intend on using this class in a command line import
// utility -- it will show adequate feedback for the user as to what
// settings have been found.
//#define TRACE_PROGRESS

#ifdef TRACE_PROGRESS
	#define STRACE(x) printf x
#else
	#define STRACE(x) /* */
#endif

// Define this only if you want very verbose tracing information for
// debugging the reader class
//#define DEBUG_PROGRESS

#ifdef DEBUG_PROGRESS
	#define DTRACE(x) printf x
#else
	#define DTRACE(x) /* */
#endif

/*
	Plan of action for reading a project file:
	1) Read in the access paths
	2) Skip to the compiler options and read only the expected records -- skip
	   unrecognized ones
	3) Read in file list.
	
	Aside from the unknown data, the only part of these project files that are
	skipped are the file handling rules. It is unlikely that anyone will not
	use them and the code for parsing them isn't very simple, so they are not
	worth bothering with.
*/

BeIDEProject::BeIDEProject(const char *path)
{
	InitObject();
	SetTo(path);
}


BeIDEProject::BeIDEProject(void)
{
	InitObject();
}


BeIDEProject::~BeIDEProject(void)
{
}


status_t
BeIDEProject::InitCheck(void) const
{
	return fInit;
}


status_t
BeIDEProject::SetTo(const char *path)
{
	BFile file(path, B_READ_ONLY);
	if (file.InitCheck() != B_OK)
	{
		fInit = file.InitCheck();
		return fInit;
	}
	
	file.GetSize(&fBufferSize);
	
	if (fBufferSize < 1)
	{
		fInit = B_BAD_DATA;
		return fInit;
	}
	
	fBuffer = new uint8[fBufferSize];
	if (!fBuffer)
	{
		fInit = B_NO_MEMORY;
		return fInit;
	}
	
	if (file.Read(fBuffer, fBufferSize) != fBufferSize)
	{
		fInit = B_ERROR;
		return fInit;
	}
	
	ParseData();
	
	delete [] fBuffer;
	fBuffer = NULL;
	
	return fInit;
}


void
BeIDEProject::Unset(void)
{
	fInit = B_NO_INIT;
	if (fBuffer)
		delete [] fBuffer;
	fBuffer = NULL;
	fProjectFiles.clear();
	
	fTargetName = "BeApp";
	fTargetType = TARGET_APPLICATION;
	fSystemIncludesAsLocal = false;
	fFileTypeMode = FILE_TYPES_AUTODETECT;
	fLangOpts = 0;
	fWarnMode = WARNMODE_ENABLED;
	fWarnings = 0;
	fCodeGenFlags = 0;
	fOpMode = OPTIMIZE_NONE;
	fStripFlags = 0;
	fCompilerOptions = "";
	fLinkerOptions = "";
	
	fFileTypeRules.clear();
	fSysIncludes.clear();
	fLocalIncludes.clear();
}

void
BeIDEProject::SetTargetName(const char *name)
{
	fTargetName = name;
}


const char *
BeIDEProject::TargetName(void) const
{
	return fTargetName.String();
}


void
BeIDEProject::SetTargetType(const uint32 &type)
{
	fTargetType = type;
}


uint32
BeIDEProject::TargetType(void) const
{
	return fTargetType;
}


void
BeIDEProject::SetSystemIncludesAsLocal(bool value)
{
	fSystemIncludesAsLocal = value;
}


bool
BeIDEProject::SystemIncludesAsLocal(void) const
{
	return fSystemIncludesAsLocal;
}


void
BeIDEProject::SetFileDetectionMode(const uint32 &mode)
{
	fFileTypeMode = mode;
}


uint32
BeIDEProject::FileDetectionMode(void) const
{
	return fFileTypeMode;
}


void
BeIDEProject::SetLanguageOptions(const uint32 &opts)
{
	fLangOpts = opts;
}


uint32
BeIDEProject::LanguageOptions(void) const
{
	return fLangOpts;
}


void
BeIDEProject::SetWarningMode(const uint32 &mode)
{
	fWarnMode = mode;
}


uint32
BeIDEProject::WarningMode(void) const
{
	return fWarnMode;
}


void
BeIDEProject::SetWarnings(const uint32 &warnFlags)
{
	fWarnings = warnFlags;
}


uint32
BeIDEProject::Warnings(void) const
{
	return fWarnings;
}


void
BeIDEProject::SetCodeGenerationFlags(const uint32 &codeFlags)
{
	fCodeGenFlags = codeFlags;
}


uint32
BeIDEProject::CodeGenerationFlags(void) const
{
	return fCodeGenFlags;
}


void
BeIDEProject::SetOptimizationMode(const uint32 &opMode)
{
	fOpMode = opMode;
}


uint32
BeIDEProject::OptimizationMode(void) const
{
	return fOpMode;
}


void
BeIDEProject::SetStripFlags(const uint32 &stripFlags)
{
	fStripFlags = stripFlags;
}


uint32
BeIDEProject::StripFlags(void) const
{
	return fStripFlags;
}


void
BeIDEProject::SetExtraCompilerOptions(const char *string)
{
	fCompilerOptions = string;
}


const char *
BeIDEProject::ExtraCompilerOptions(void)
{
	return fCompilerOptions.String();
}


void
BeIDEProject::SetExtraLinkerOptions(const char *string)
{
	fLinkerOptions = string;
}


const char *
BeIDEProject::ExtraLinkerOptions(void)
{
	return fLinkerOptions.String();
}


int32
BeIDEProject::CountSystemIncludes(void) const
{
	return fSysIncludes.size();
}


const char *
BeIDEProject::SystemIncludeAt(const uint32 &index)
{
	if (index > fSysIncludes.size() - 1)
		return NULL;
	
	return fSysIncludes[index].String();
}


int32
BeIDEProject::CountLocalIncludes(void) const
{
	return fLocalIncludes.size();
}


const char *
BeIDEProject::LocalIncludeAt(const uint32 &index)
{
	if (index > fLocalIncludes.size() - 1)
		return NULL;
	
	return fLocalIncludes[index].String();
}


#pragma mark - Private Methods

void
BeIDEProject::InitObject(void)
{
	fInit = B_NO_INIT;
	fBuffer = NULL;
	fTargetName = "BeApp";
	fTargetType = TARGET_APPLICATION;
	fSystemIncludesAsLocal = false;
	fFileTypeMode = FILE_TYPES_AUTODETECT;
	fLangOpts = 0;
	fWarnMode = WARNMODE_ENABLED;
	fWarnings = 0;
	fCodeGenFlags = 0;
	fOpMode = OPTIMIZE_NONE;
	fStripFlags = 0;
	fCompilerOptions = "";
	fLinkerOptions = "";
}


BString
BeIDEProject::TagIDToString(const uint32 &id)
{
	BString out;
	char string[5];
	string[4] = '\0';
	string[0] = (char)((id & 0xFF000000) >>  24);
	string[1] = (char)((id & 0x00FF0000) >>  16);
	string[2] = (char)((id & 0x0000FF00) >>  8);
	string[3] = (char)((id & 0x000000FF));
	
	out = string;
	return out;
}


off_t
BeIDEProject::FindTagID(const int32 &id, const off_t &offset)
{
	if (offset < 0 || offset > fBufferSize)
		return -1;
	
	uint8 byte1 = (uint8)((id & 0xFF000000) >>  24);
	uint8 byte2 = (uint8)((id & 0x00FF0000) >>  16);
	uint8 byte3 = (uint8)((id & 0x0000FF00) >>  8);
	uint8 byte4 = (uint8)((id & 0x000000FF));
	
	uint8 *end = fBuffer + fBufferSize;
	for (uint8 *i = fBuffer + offset; i <= end; i++)
	{
		if (*i == byte1 && *(i + 1) == byte2 &&
			*(i + 2) == byte3 && *(i + 3) == byte4)
			return (i - fBuffer);
	}
	
	return -1;
}


int32
BeIDEProject::ReadInt32(off_t &offset)
{
	int32 data = *((int32*)(fBuffer + offset));
	swap_data(B_INT32_TYPE, &data, sizeof(int32), B_SWAP_BENDIAN_TO_HOST);
	offset += sizeof(int32);
	
	return data;
}


BString
BeIDEProject::ReadString(off_t &offset)
{
	BString out;
	
	if (offset < 0 || offset > fBufferSize)
		return out;
	
	uint8 *end = fBuffer + fBufferSize;
	for (uint8 *i = fBuffer + offset; i <= end; i++)
	{
		if (*i == 0)
		{
			uint32 size = i - fBuffer + 1;
			char *tempBuffer = out.LockBuffer(size);
			strncpy(tempBuffer, (char*)fBuffer + offset, size);
			out.UnlockBuffer();
			
			offset += out.CountChars() + 1;
			return out;
		}
	}
	
	return out;
}


void
BeIDEProject::ParseData()
{
	BMemoryIO io(fBuffer,fBufferSize);
	
	// Skip over the header and other junk to get to the access paths
	off_t pos = FindTagID('DAcc');
	if (pos < 0)
	{
		fInit = B_ERROR;
		return;
	}
	
	// -----------------------------------------------------------------------
	// Read the access path info
	// -----------------------------------------------------------------------
	
	pos += sizeof(int32) * 3;
	DTRACE(("Position: %lld\n", pos));
	fSystemIncludesAsLocal = (ReadInt32(pos) != 0);
	STRACE(("System includes %s treated as local includes\n",
			fSystemIncludesAsLocal ? "are" : "are not"));
	
	DTRACE(("Position: %lld\n", pos));
	int32 countSysIncludes = ReadInt32(pos);
	int32 countLocalIncludes = ReadInt32(pos);
	
	DTRACE(("System paths: %ld, Local paths: %ld\n",
			countSysIncludes, countLocalIncludes));
	
	// Read the system paths
	for (int32 sysCount = 0; sysCount < countSysIncludes; sysCount++)
	{
		pos += (sizeof(int32) * 3) + 1;
		DTRACE(("Position: %lld\n", pos));
		BString path = ReadString(pos);
		STRACE(("System path: %s\n",path.String()));
		fSysIncludes.push_back(path.String());
		
		// Skip over the rest of the path's fixed string storage
		pos += 258 - path.CountChars();
	}
	
	// Read the local paths
	for (int32 localCount = 0; localCount < countLocalIncludes; localCount++)
	{
		pos += (sizeof(int32) * 3) + 1;
		DTRACE(("Position: %lld\n", pos));
		BString path = ReadString(pos);
		STRACE(("Local path: %s\n",path.String()));
		fLocalIncludes.push_back(path.String());
		
		// Skip over the rest of the path's fixed string storage
		pos += 258 - path.CountChars();
	}
	
	// -----------------------------------------------------------------------
	// Read project settings
	// -----------------------------------------------------------------------
	pos = FindTagID('GPrf', pos);
	if (pos < 0)
	{
		fInit = B_ERROR;
		return;
	}
	STRACE(("\nReading Preferences\n"));
	STRACE(("--------------------------------------\n"));
	DTRACE(("--------------------------------------\n"));
	DTRACE(("General Preferences Tag Position: %lld\n", pos));
	DTRACE(("--------------------------------------\n"));
	DTRACE(("--------------------------------------\n"));
	
	// Skip over some unknown data
	pos += sizeof(int32) * 2;
	
	// The preferences are almost exclusively kept in a general-purpose tag record --
	// a specific preferences record nested within a general-purpose one. Thus, each
	// record has a tag and a subtag. Luckily, the container record also contains the
	// size of the internal one, so if the subtag isn't one we support, it can be
	// skipped entirely.
	
	DTRACE(("First GenB Record Position: %lld\n", pos));
	int32 currentTag = ReadInt32(pos);
	int32 subRecordSize = ReadInt32(pos);
	int32 subTag = ReadInt32(pos);
	
	// We automatically find the end of the sub-tag specifier, so knowing its size
	// isn't necessary.
	pos += sizeof(int32);
	BString subTagName = ReadString(pos);
	
	while (currentTag == 'GenB')
	{
		DTRACE(("Position: %lld\n", pos));
		DTRACE(("Tag: %s\n", TagIDToString(currentTag).String()));
		DTRACE(("Subrecord size: %ld\n", subRecordSize));
		DTRACE(("Subtag: %s\n", TagIDToString(subTag).String()));
		DTRACE(("Subtag Name: %s\n", subTagName.String()));
		
		int32 value;
		switch (subTag)
		{
			case 'cccg':
			{
				if (subTagName == "gccLanguage")
				{
					pos += sizeof(int32);
					
					value = ReadInt32(pos);
					// 0x01000000 for treat all files as C
					// 0x00010000 for treat all files as C++
					// 0x00000100 for compile for ANSI C
					// 0x00000001 for supporting ANSI trigraphs
					if (value & 0x01000000)
					{
						fFileTypeMode = FILE_TYPES_C_MODE;
						STRACE(("Treat all files as C\n"));
					}
					else if (value & 0x00010000)
					{
						fFileTypeMode = FILE_TYPES_CPP_MODE;
						STRACE(("Treat all files as C++\n"));
					}
					else
					{
						fFileTypeMode = FILE_TYPES_AUTODETECT;
						STRACE(("Autodetect file types based on extension\n"));
					}
					
					if (value & 0x100)
					{
						fLangOpts |= LANGOPTS_ANSI_C_MODE;
						STRACE(("Compile in ANSI C mode\n"));
					}
					
					if (value & 1)
					{
						fLangOpts |= LANGOPTS_SUPPORT_TRIGRAPHS;
						STRACE(("Compile with trigraph support\n"));
					}
					
					value = ReadInt32(pos);
					// Default signed char = 0x1000000
					// Default unsigned bitfields = 0x10000
					
					if (value & 0x1000000)
					{
						fLangOpts |= LANGOPTS_SIGNED_CHAR;
						STRACE(("Type 'char' is signed by default\n"));
					}
					
					if (value & 0x10000)
					{
						fLangOpts |= LANGOPTS_UNSIGNED_BITFIELDS;
						STRACE(("Bitfields are unsigned by default\n"));
					}
					
				}
				else if (subTagName == "gccCommonWarning")
				{
					pos += sizeof(int32);
					
					value = ReadInt32(pos);
					// 0x1000000 = Warn about all common mistakes
					// 0x0010000 = Warn about missing parentheses
					// 0x0000100 = Warn about inconsistent return types
					// 0x0000001 = Warn about enumerated switches missing specific cases
					
					if (value & 0x1000000)
					{
						fLangOpts |= WARN_ALL_COMMON_ERRORS;
						STRACE(("Warn about all common errors\n"));
					}
					
					if (value & 0x10000)
					{
						fLangOpts |= WARN_MISSING_PARENTHESES;
						STRACE(("Warn about missing parentheses\n"));
					}
					
					if (value & 0x100)
					{
						fLangOpts |= WARN_INCONSISTENT_RETURN;
						STRACE(("Warn about inconsistent return types\n"));
					}
					
					if (value & 1)
					{
						fLangOpts |= WARN_MISSING_ENUM_CASES;
						STRACE(("Warn about enumerated switches missing specific cases\n"));
					}
					
					value = ReadInt32(pos);
					// 0x1000000 = Warn when variable not used
					// 0x0010000 = Warn about uninitialized auto variables
					// 0x0000100 = Warn when compiler reorders member initialization
					// 0x0000001 = Warn about nonvirtual destructors
					
					if (value & 0x1000000)
					{
						fLangOpts |= WARN_UNUSED_VARS;
						STRACE(("Warn when variables aren't used\n"));
					}
					
					if (value & 0x10000)
					{
						fLangOpts |= WARN_UNINIT_AUTO_VARS;
						STRACE(("Warn about uninitialized automatic variables\n"));
					}
					
					if (value & 0x100)
					{
						fLangOpts |= WARN_INIT_REORDERING;
						STRACE(("Warn when the compiler reorders class member initialization\n"));
					}
					
					if (value & 1)
					{
						fLangOpts |= WARN_NONVIRTUAL_DESTRUCTORS;
						STRACE(("Warn about non-virtual destructors\n"));
					}
					
					value = ReadInt32(pos);
					// 0x1000000 = Warn about unrecognized pragmas
					// 0x0010000 = Warn about signed/unsigned comparisons
					// 0x0000100 = Warn about subscripts with type char
					// 0x0000001 = Warn about printf formatting anomalies
					
					if (value & 0x1000000)
					{
						fLangOpts |= WARN_UNRECOGNIZED_PRAGMAS;
						STRACE(("Warn about unrecognized pragmas\n"));
					}
					
					if (value & 0x10000)
					{
						fLangOpts |= WARN_SIGNED_UNSIGNED_COMP;
						STRACE(("Warn about signed/unsigned comparisons\n"));
					}
					
					if (value & 0x100)
					{
						fLangOpts |= WARN_CHAR_SUBSCRIPTS;
						STRACE(("Warn about subscripts with type 'char'\n"));
					}
					
					if (value & 1)
					{
						fLangOpts |= WARN_PRINTF_FORMATTING;
						STRACE(("Warn about printf() formatting anomalies\n"));
					}
					
					value = ReadInt32(pos);
					// 0x1000000 = Warn when trigraphs used
					
					if (value & 0x1000000)
					{
						fLangOpts |= WARN_TRIGRAPHS_USED;
						STRACE(("Warn when trigraphs are used\n"));
					}
					
				}
				else if (subTagName == "gccWarning")
				{
					pos += sizeof(int32);
					
					value = ReadInt32(pos);
					// 0x1000000 = Warnings are disabled
					// 0x0010000 = Warnings are treated as errors
					// 0x0000100 = Issue all warnings demanded by strict ANSI C/C++
					// 0x0000001 = Warn when one local variable shadows another
					
					if (value & 0x1000000)
					{
						fWarnMode = WARNMODE_DISABLED;
						STRACE(("Warnings are disabled\n"));
					}
					else if (value & 0x10000)
					{
						fWarnMode = WARNMODE_AS_ERRORS;
						STRACE(("Warnings are treated as errors\n"));
					}
					else
					{
						fWarnMode = WARNMODE_ENABLED;
						STRACE(("Warnings are enabled\n"));
					}
					
					if (value & 0x100)
					{
						fWarnings |= WARN_STRICT_ANSI;
						STRACE(("Issue all warnings demanded by strict ANSI C/C++\n"));
					}
					
					if (value & 1)
					{
						fWarnings |= WARN_LOCAL_SHADOW;
						STRACE(("Warn when one local variable shadows another\n"));
					}
					
					value = ReadInt32(pos);
					// 0x1000000 = Warn about casting functions to incompatible types
					// 0x0010000 = Warn about casts which discard qualifiers
					// 0x0000100 = Warn about possibly confusing type conversions
					// 0x0000001 = Warn when an inlined function cannot be inlined
					
					if (value & 0x1000000)
					{
						fWarnings |= WARN_INCOMPATIBLE_CAST;
						STRACE(("Warn about casting functions to incompatible types\n"));
					}
					
					if (value & 0x10000)
					{
						fWarnings |= WARN_CAST_QUALIFIERS;
						STRACE(("Warn about casts which discard qualifiers\n"));
					}
					
					if (value & 0x100)
					{
						fWarnings |= WARN_CONFUSING_CAST;
						STRACE(("Warn about possibly confusing type conversions\n"));
					}
					
					if (value & 1)
					{
						fWarnings |= WARN_CANT_INLINE;
						STRACE(("Warn when an inlined function cannot be inlined\n"));
					}
					
					value = ReadInt32(pos);
					// 0x1000000 = Warn when a function is declared extern, then inline
					// 0x0010000 = Mark literal strings as 'const char *'
					// 0x0000100 = Warn about overloaded virtual function names
					// 0x0000001 = Warn if a C-style cast is used in a program
					
					if (value & 0x1000000)
					{
						fWarnings |= WARN_EXTERN_TO_INLINE;
						STRACE(("Warn when a function is declared extern, then inline\n"));
					}
					
					if (value & 0x10000)
					{
						fLangOpts |= LANGOPTS_CONST_CHAR_LITERALS;
						STRACE(("Mark literal strings as 'const char *'\n"));
					}
					
					if (value & 0x100)
					{
						fWarnings |= WARN_OVERLOADED_VIRTUALS;
						STRACE(("Warn about overload virtual function names\n"));
					}
					
					if (value & 1)
					{
						fWarnings |= WARN_C_CASTS;
						STRACE(("Warn if a C-style cast is used in a program\n"));
					}
					
					value = ReadInt32(pos);
					// 0x1000000 = enable warnings about violations of "Effective C++" style rules
					
					if (value & 0x1000000)
					{
						fWarnings |= WARN_EFFECTIVE_CPP;
						STRACE(("Enable warnings about violations of "
								"\"Effective C++\" style rules\n"));
					}
				}
				else if (subTagName == "gccCodeGeneration")
				{
					pos += sizeof(int32);
					
					value = ReadInt32(pos);
					// 0x1000000 = OpMode: None
					// 0x0010000 = OpMode: Some
					// 0x0000100 = OpMode: More
					// 0x0000001 = OpMode: Full
					
					if (value & 0x1000000)
					{
						fOpMode = OPTIMIZE_NONE;
						STRACE(("Optimization: None\n"));
					}
					else
					if (value & 0x10000)
					{
						fOpMode = OPTIMIZE_SOME;
						STRACE(("Optimization: Some\n"));
					}
					else
					if (value & 0x100)
					{
						fOpMode = OPTIMIZE_MORE;
						STRACE(("Optimization: More\n"));
					}
					else
					{
						fOpMode = OPTIMIZE_FULL;
						STRACE(("Optimization: Full\n"));
					}
					
					value = ReadInt32(pos);
					// 0x1000000 = Optimize for size over speed
					// 0x0010000 = Do not generate position independent code
					// 0x0000100 = Only emit code for explicit template instantiations
					// 0x0000001 = Generate code for functions even if fully inlined
					
					if (value & 0x1000000)
					{
						fCodeGenFlags |= CODEGEN_OPTIMIZE_SIZE;
						STRACE(("Optimize for size over speed\n"));
					}
					else
					if (value & 0x10000)
					{
						fCodeGenFlags |= CODEGEN_NO_PIC;
						STRACE(("Do not generate position-independent code\n"));
					}
					else
					if (value & 0x100)
					{
						fCodeGenFlags |= CODEGEN_EXPLICIT_TEMPLATES;
						STRACE(("Only emit code for explicit template instantiations\n"));
					}
					else
					{
						fCodeGenFlags |= CODEGEN_IGNORE_INLINING;
						STRACE(("Generate code for functions even if fully inlined\n"));
					}
					
					value = ReadInt32(pos);
					// 0x1000000 = generate debug code
					// 0x0010000 = generate profiling code
					
					if (value & 0x1000000)
					{
						fCodeGenFlags |= CODEGEN_DEBUGGING;
						STRACE(("Compile with debugging information\n"));
					}
					
					if (value & 0x10000)
					{
						fCodeGenFlags |= CODEGEN_PROFILING;
						STRACE(("Compile with profiling information\n"));
					}
				}
				else if (subTagName == "AdditionalGCCCompilerOptions")
				{
					pos += sizeof(int32);
					fCompilerOptions = ReadString(pos);
					pos += 1023 - fCompilerOptions.CountChars();
					STRACE(("Extra compiler options: %s\n",
							fCompilerOptions.CountChars() > 0 ? fCompilerOptions.String() :
																"None"));
				}
				else
				{
					DTRACE(("Skipping record for unsupported 'cccg' subtag name %s\n",
							subTagName.String()));
					pos += subRecordSize - 8 - subTagName.CountChars() - 1;
				}
				break;
			}
			case 'dlcg':
			{
				if (subTagName == "gccLinker")
				{
					pos += sizeof(int32);
					value = ReadInt32(pos);
					
					if (value & 0x1000000)
					{
						fStripFlags |= STRIP_ALL_SYMBOLS;
						STRACE(("Strip all symbols\n"));
					}
					
					if (value & 0x10000)
					{
						fStripFlags |= STRIP_ALL_LOCAL_SYMBOLS;
						STRACE(("Strip all local symbols\n"));
					}
				}
				else
				if (subTagName == "AdditionalGCCLinkerOptions")
				{
					pos += sizeof(int32);
					fLinkerOptions = ReadString(pos);
					pos += 1023 - fLinkerOptions.CountChars();
					STRACE(("Extra linker options: %s\n",
							fLinkerOptions.CountChars() > 0 ? fLinkerOptions.String() :
																"None"));
				}
				else
				{
					DTRACE(("Skipping record for unsupported 'dlcg' subtag name %s\n",
							subTagName.String()));
					pos += subRecordSize - 8 - subTagName.CountChars() - 1;
				}
				break;
			}
			case 'mwcx':
			{
				// This skips 8 bytes of extra data plus 64 bytes for the binary type
				// signature, which for Haiku x86 is always the same. For other platforms,
				// BeIDE is either unsupported or doesn't run
				
				DTRACE(("Position: %lld\n", pos));
				pos += (sizeof(int32) * 2);
				
				fTargetType = fBuffer[pos];
				STRACE(("Target type is %d\n", fTargetType));
				pos += 65;
				
				fTargetName = ReadString(pos);
				pos += 66 - fTargetName.CountChars();
				STRACE(("Target name: %s\n", fTargetName.String()));
				break;
			}
			default:
			{
				DTRACE(("Skipping record for unsupported subtag %s\n",
						TagIDToString(subTag).String()));
				pos += subRecordSize - 8 - subTagName.CountChars() - 1;
				break;
			}
		}
		DTRACE(("--------------------------------------\n"));
		
		currentTag = ReadInt32(pos);
		subRecordSize = ReadInt32(pos);
		subTag = ReadInt32(pos);
		pos += sizeof(int32);
		subTagName = ReadString(pos);
	}
	
	// -----------------------------------------------------------------------
	// Read in the project files
	// -----------------------------------------------------------------------
	
	STRACE(("\nReading files and groups\n"));
	STRACE(("--------------------------------------\n"));
		
	pos = FindTagID('Sect');
	if (pos < 0)
	{
		fInit = B_ERROR;
		return;
	}
	DTRACE(("Position: %lld\n", pos));
	
	currentTag = ReadInt32(pos);
	while (currentTag == 'Sect')
	{
		// Read the group record
		pos += sizeof(int32);
		DTRACE(("Position: %lld\n", pos));
		int32 groupFileCount = ReadInt32(pos);
		DTRACE(("Count for group: %ld\n", groupFileCount));
		
		pos += (sizeof(int32) * 2) + 1;
		DTRACE(("Position: %lld\n", pos));
		BString groupName = ReadString(pos);
		STRACE(("\nGroup: %s\n", groupName.String()));
		
		// Skip over the rest of the path's fixed string storage
		pos += 18 - groupName.CountChars();
		DTRACE(("Position: %lld\n", pos));
		
		// A lot of group records have this goofy DPrf record afterward, whatever
		// that's for.
		currentTag = ReadInt32(pos);
		if (currentTag == 'DPrf')
		{
			pos += 44;
			DTRACE(("Skipping 'DPrf' tag\n"));
			DTRACE(("Position: %lld\n", pos));
			currentTag = ReadInt32(pos);
		}
		
		for (int32 i = 0; i < groupFileCount; i++)
		{
			BString fileName;
			BString mimeType;
			
			// Read file records
			switch (currentTag)
			{
				case 'SrFl':
				{
					// We skip over dependencies because they can easily be regenerated
					
					DTRACE(("Handling tag %s\n", TagIDToString(currentTag).String()));
					
					pos += sizeof(int32) * 3;
					DTRACE(("Position: %lld\n", pos));
					
					// Skip Name record and extra data
					pos += sizeof(int32) * 3;
					
					// Read dependency name and throw it away
					ReadString(pos);
					
					i--;
					break;
				}
				case 'Fil1':
				case 'PLnk':
				case 'IgFl':
				case 'Link':
				case 'SbPr':
				{
					DTRACE(("Handling tag %s\n", TagIDToString(currentTag).String()));
					
					// Skip to MIME tag
					pos += sizeof(int32) * 7;
					
					// Skip to actual MIME type held in record
					pos += sizeof(int32) * 3;
					DTRACE(("Position: %lld\n", pos));
					mimeType = ReadString(pos);
					DTRACE(("Mime type: %s\n", mimeType.String()));
					
					// Skip over the unknown MSFl record -- 4 byte tag + 12 bytes
					pos += sizeof(int32) * 4;
					
					currentTag = ReadInt32(pos);
					pos += sizeof(int32) * 2;
					
					DTRACE(("Position: %lld\n", pos));
					fileName = ReadString(pos);
					DTRACE(("File name: %s\n", fileName.String()));
					
					ProjectFile file;
					file.path = fileName;
					file.mimeType = mimeType;
					file.group = groupName;
					fProjectFiles.push_back(file);
					STRACE(("Added file %s\n", fileName.String()));
					break;
				}
				default:
				{
					printf("Unexpected tag: '%s'\n",
							TagIDToString(currentTag).String());
					break;
				}
			}
			
			currentTag = ReadInt32(pos);
		}
		
		if (currentTag != 'Sect')
		{
			pos = FindTagID('Sect', pos);
			currentTag = ReadInt32(pos);
		}
	}
	
	fInit = B_OK;
}

