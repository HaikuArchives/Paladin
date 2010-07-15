/*	$Id: Scripting.h,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
	Copyright 1996, 1997, 1998, 2002
	        Hekkelman Programmatuur B.V.  All rights reserved.
	
	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:
	1. Redistributions of source code must retain the above copyright notice,
	   this list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright notice,
	   this list of conditions and the following disclaimer in the documentation
	   and/or other materials provided with the distribution.
	3. All advertising materials mentioning features or use of this software
	   must display the following acknowledgement:
	   
	    This product includes software developed by Hekkelman Programmatuur B.V.
	
	4. The name of Hekkelman Programmatuur B.V. may not be used to endorse or
	   promote products derived from this software without specific prior
	   written permission.
	
	THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
	FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
	AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
	PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
	OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
	WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 	

	Created: 02-06-02
*/

#ifndef SCRIPTING_H
#define SCRIPTING_H

/*	Scripting.h
 *	$Id: Scripting.h,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
 *	Generic dataq structures needed for implementing a proper scripting interface
 */

class BMessage;
class ScriptHandler;

enum EForm {
	formDirect,			/*	No data used, this is it */
	formFirst,			/*	No data needed, first of list */
	formLast,			/*	No data needed, last of list */
	formIndex,			/*	index 1 == first item */
	formReverseIndex,	/*	index -1 == last item */
	formIndexRange,		/*	range.startIndex extends range.length */
	formID,				/*	id is unique id */
	formName,			/*	name  as C string */
	formUser = 31		/*	This is the last form reserved by the standard */
};

const unsigned long PROPERTY_TYPE = 'prop';
const long kPropNameLength = 64;

/*	These are the default parameter names for various parameters. Use them 
 *	wherever appropriate.
 */
extern const char kDefaultTargetName[];		//	target of verb
extern const char kDefaultDataName[];			//	return data, and init data
extern const char kDefaultErrorName[];			//	return a long error code != 0 here if error
extern const char kDefaultErrorTextName[];		//	return a string error description here if error


/*	The data in a PROPERTY_TYPE item in a BMessage has the PropertyItem structure
 */
union SData {
	long	index;
	long	id;
	struct {
		long	start;
		long	length;
	}		range;
	char	name[kPropNameLength];	//	actually, this will be unbounded - put the element last
};									//	do not create one on the stack
struct PropertyItem {
	char	property[kPropNameLength];
	EForm	form;
	SData	data;					//	unbounded size - this element is last
};


/*	These are default verb names and IDs. You are of course welcome to use your 
 *	own versions, but a suggested default is always good.
 *	The reason we have a "close" and a "quit" message is that "quit" typically can't 
 *	take a target parameter - it just quits its direct receipient. Same thing for "open" 
 *	and "receive", as well as "save" and "saveas".
 */
const unsigned long kOpenVerb =			'ssm!';		//	"open"
const unsigned long kReceiveVerb =		B_REFS_RECEIVED;	//	"receive"
const unsigned long kSaveVerb =			'sav!';		//	"save"
const unsigned long kSaveAsVerb =		B_SAVE_REQUESTED;	//	"saveas"
const unsigned long kCloseVerb =		'bye!';		//	"close"
const unsigned long kQuitVerb =			B_QUIT_REQUESTED;	//	"quit"
const unsigned long kSetVerb =			'set!';		//	"set"
const unsigned long kGetVerb =			'get?';		//	"get"
const unsigned long kMakeVerb =			'mak+';		//	"make"
const unsigned long kCreateVerb =		'cre+';		//	"create"
const unsigned long kDeleteVerb =		'del-';		//	"delete"
const unsigned long kCountVerb =		'num?';		//	"count"

const unsigned long kReplyVerb =		'done';		//	not really a verb


/*	For recording support, an application (or handler?) should respond to these 
 *	messages. StartRecording should add the messenger to a list of messengers that 
 *	get sent BMessages for everything the application does.
 *	StopRecording should delete the messenger from that list.
 *	Playing back these messages should cause the application to re-do what it just did 
 *	in a semantic sense - not necessarily mouse-down and key-down wise.
 */
const unsigned long kStartRecordingVerb =	'rec!';	//	not really a verb
const unsigned long kStopRecordingVerb =	'!cer';	//	not really a verb
extern const char kRecordingTargetName[];			//	type B_MESSENGER_TYPE


/*	This is what verbs look like in the resource description. As for properties, they 
 *	are found in one named and numbered resource per verb.
 */

const unsigned long VERB_TYPE =		'verb';

enum VerbFlags {	//	bit mask flags for "flags" part of verbArgument
	verbArgumentRequired	= 1,
	verbArgumentIsList		= 2
};
struct VerbArgument {
	char				name[kPropNameLength];
	unsigned long		type;
	unsigned long		flags;
};
struct VerbDescription {
	unsigned long		what;					//	message "what" constant
	char				name[kPropNameLength];	//	user-readable name
	int					numArgs;				//	num arguments
	VerbArgument		argument[1];			//	actually numArgs
};

enum {
	SCRIPT_BASE_ERROR		= (int) 0xE1230000
};

enum {
//	there is never such a property in this handler
	SCRIPT_NO_PROPERTY		= SCRIPT_BASE_ERROR,
//	there is no current such property in this handler (index out of range etc)
	SCRIPT_NO_ITEM,
//	don't understand the form used (formRange if no list, etc)
	SCRIPT_BAD_FORM,
//	item doesn't understand this verb
	SCRIPT_BAD_VERB,
//	the property is read-only
	SCRIPT_READ_ONLY,
//	given data could not be coerced into expected type
	SCRIPT_BAD_TYPE,
//	some data that was required isn't there
	SCRIPT_MISSING_DATA
};


/*	This function is a work-horse. In any BHandler, if you get a BMessage you don't 
 *	recognize in any other way, call TryScriptMessage(). If it returns TRUE, it was a 
 *	script verb, and the message was handled (although not necessarily successfully). 
 *	Pass in a list of verbs you want to recognize; if the message->what is one of these 
 *	it will try to process it.If you pass in NULL, it will use its own built-in list of verbs 
 *	which may be OK unless you do something special.  Also pass in the number of 
 *	verbs - if left blank, it'll assume the list of verbs is terminated by a 0 (NULL) verb. 
 *	For "root" pass in the ScriptHandler that corresponds to the BHandler calling the 
 *	function.
 *
 *	Typically:
 *
 *	class MyHandler : public BHandler, public ScriptHandler { ... };
 *
 *	void MyHandler::MessageReceived(BMessage *msg) {
 *		switch (msg->what) {
 *		case MY_MENU_COMMAND:
 *			DoMyMenuItem(); break;
 *		default:
 *			if (!TryScriptMessage(msg, this))	//	use default list of verbs
 *				inherited::MessageReceived(msg);
 *		}
 *	}
 */
extern bool TryScriptMessage(
		BMessage *				message, 
		ScriptHandler *			root, 
		const unsigned long *	verbs = NULL,
		int						numVerbs = -1);


#endif // SCRIPTING_H
