//========================================================================
//	MScripting.h
//	Copyright 1996 Metrowerks Corporation, All Rights Reserved.
//========================================================================	

#ifndef MSCRIPTING_H
#define MSCRIPTING_H

#include "Scripting.h"

// Scripting commands understood by BeIDE
enum
{
	kMakeProject		= 'MMak'
};

// Errors that may be returned by various script commands
enum
{
	M_ERROR_BASE = SCRIPT_BASE_ERROR + 0x9000,
	M_NO_PROJECT = M_ERROR_BASE,
	M_MAKE_BUSY,
	M_MAKE_ERRORS,
	M_MAKE_WARNINGS,
	M_DUPLICATE_FILE
};

// properties of the application

const char kProjectProp[] = "project";
// also messenger

// properties of the project

const char kFilesProp[] = "files";					// readonly
const char kFileProp[] = "file";					// readwrite
const char kRecordRefProp[] = "record_ref";			// readonly
const char kNameProp[] = "name";					// readonly
const char kDependenciesProp[] = "dependencies";	// readonly
// also messenger

// kMakeProject
/*
	tell BeIDE Make project
	takes no parameters	
	returns
		B_NO_ERROR
		M_MAKE_BUSY - the project is currently compiling
		M_NO_PROJECT - no project file is open
		M_MAKE_ERRORS - errors occurred during the make
		M_MAKE_WARNINGS - warnings were generated during the make
*/

// AddFileToProject is the same as kCreateVerb
/*
	takes a record_ref or full path as parameter
	returns
		B_NO_ERROR
		M_MAKE_BUSY - the project is currently compiling
		M_NO_PROJECT - no project file is open
		B_FILE_NOT_FOUND - target file not found
		M_DUPLICATE_FILE - this file or file with this name already in project
*/

#endif
