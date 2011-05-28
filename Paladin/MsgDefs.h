#ifndef MSGDEFS_H
#define MSGDEFS_H

enum
{
	M_REGISTER_WINDOW = 'rgwn',
	M_DEREGISTER_WINDOW = 'drgw',
	M_NEW_PROJECT = 'nwpj',
	M_CREATE_PROJECT = 'crpj',
	M_OPEN_PROJECT = 'oppj',
	M_SHOW_OPEN_PROJECT = 'shop',
	M_BACKUP_PROJECT = 'bkup',
	M_JUMP_TO_MSG = 'jmpm',
	M_OPEN_PARENT_FOLDER = 'oppf',
	M_REMOVE_FILES = 'rmfl',
	M_REBUILD_FILE = 'rbfl',
	M_NEW_GROUP = 'nwgr',
	M_SORT_GROUP = 'srgr',
	M_SHOW_RENAME_GROUP = 'shrg',
	M_CULL_EMPTY_GROUPS = 'cleg',
	M_IMPORT_REFS = 'imrf',
	M_QUICK_IMPORT = 'qipr',
	M_ADD_FILES = 'adfl',
	M_EMPTY_CCACHE = 'emcc',
	M_ADD_SELECTION_TO_REPO = 'a2rp',
	M_REMOVE_SELECTION_FROM_REPO = 'rfrp',
	M_REVERT_SELECTION = 'rvsl',
	M_DIFF_SELECTION = 'dfsl',
	
	PALEDIT_OPEN_FILE  = 'Cmdl',
	
	// Scripting messages
	
	// for experimental Compile on Save feature
	M_BUILD_FILE = 'PBFl',
	
	// Duplicate the same message code as BeIDE for pe's sake
	M_MAKE_PROJECT = 'MMak',
	
	M_RUN_PROJECT = 'PRun',
	M_RUN_IN_TERMINAL = 'PRnT',
	M_RUN_IN_DEBUGGER = 'PRnD',
	M_RUN_WITH_ARGS = 'PRnA',
	
	M_ADD_FILE = 'PAFl',
	M_REMOVE_FILE = 'PRFl',
	M_FORCE_REBUILD = 'PFoR',
	M_OPEN_PARTNER = 'POpP',

	M_TOGGLE_ERROR_WINDOW = 'PTgE',
	M_SHOW_ERROR_WINDOW = 'PShE',
	M_SHOW_ADD_NEW_PANEL = 'PSnP',
	M_SHOW_FIND_AND_OPEN_PANEL = 'PShF',
	M_FIND_AND_OPEN_FILE = 'PFaO',
	M_SHOW_FIND_IN_PROJECT_FILES = 'PFif'
};

#endif
