// CLang_jam.cpp
// 
// Copyright (c) 2004, Ingo Weinhold (bonefish@cs.tu-berlin.de)
// 
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
// 
// Except as contained in this notice, the name of a copyright holder shall
// not be used in advertising or otherwise to promote the sale, use or other
// dealings in this Software without prior written authorization of the
// copyright holder.

#include <String.h>
#include "CLanguageAddOn.h"
#include "HColorUtils.h"

class ParseContext;
class Token;

_EXPORT const char kLanguageName[] = "Jam";
_EXPORT const char kLanguageExtensions[] = "Jamfile;Jamrules;Jambase";
_EXPORT const char kLanguageCommentStart[] = "#";
_EXPORT const char kLanguageCommentEnd[] = "";
_EXPORT const char kLanguageKeywordFile[] = "keywords.jam";
_EXPORT const int16 kInterfaceVersion = 2;

// #pragma mark ----- tokens -----

// Token
struct Token {
	enum token_type {
		STRING,
		COMMENT,
		TEXT,
		KEYWORD,
		LBRACE,
		RBRACE,
		LBRACKET,
		RBRACKET,
		COLON,
		SEMICOLON,
		NOT,
		AND,
		LESS,
		GREATER,
		OR,
		EQUAL,
		UNEQUAL,
		ADD_ASSIGN,
		LESS_EQUAL,
		GREATER_EQUAL,
		CONDITIONAL_ASSIGN,
		ACTIONS,
	};

	bool operator==(const char* other) const
	{
		int otherLength = strlen(other);
		return (otherLength == length && strncmp(string, other, length) == 0);
	}

	bool operator!=(const char* other) const
	{
		return !(*this == other);
	}

	const char*	string;
	int			offset;
	int			length;
	token_type	type;
	union {
		int		keyword;
		bool	terminated;
	};
};

// Tokenizer
class Tokenizer {
public:
	Tokenizer(CLanguageProxy& proxy, int offset, int length)
		: fProxy(proxy),
		  fLine(proxy.Text()),
		  fLength(offset + length),
		  fOffset(offset),
		  fNoKeyword(false)
	{
	}

	Tokenizer(CLanguageProxy& proxy, const char* line, int offset, int length)
		: fProxy(proxy),
		  fLine(line),
		  fLength(offset + length),
		  fOffset(offset),
		  fNoKeyword(false)
	{
	}

	bool GetNextToken(Token& token);
	int GetOffset() const				{ return fOffset; }

private:
	inline int _GetCurrentChar();
	inline int _GetNextChar();
	inline void _PutLastChar();

private:
	CLanguageProxy&	fProxy;
	const char*		fLine;
	int				fLength;
	int				fOffset;
	bool			fNoKeyword;
};

// GetNextToken
bool
Tokenizer::GetNextToken(Token& token)
{
	// skip leading white space
	int c = _GetCurrentChar();
	while (c != 0 && isspace(c)) {
		fNoKeyword = false;
		c = _GetNextChar();
	}
	if (c == 0)
		return false;
	// there is something, let's see what it is
	token.offset = fOffset;
	token.string = fLine + token.offset;
	// string
	if (c == '"') {
		token.type = Token::STRING;
		while (true) {
			c = _GetNextChar();
			if (c == 0) {
				// unterminated string
				token.length = fOffset - token.offset;
				return true;
			}
			if (c == '"') {
				_GetNextChar();
				token.length = fOffset - token.offset;
				fNoKeyword = true;
				return true;
			}
			if (c == '\\')
				_GetNextChar();
		}
	}
	// comment
	if (c == '#') {
		// find "\n" or EOF
		do {
			c = _GetNextChar();
		} while (c != 0 && c != '\n');
		token.type = Token::COMMENT;
		token.length = fOffset - token.offset;
		token.terminated = (c == '\n');
		return true;
	}
	// text or keyword
	if (!isalpha(c))
		fNoKeyword = true;
	while (true) {
		if (c == 0 || isspace(c))
			break;
		if (c == '"') {
			fNoKeyword = true;
			break;
		}
		if (c == '\\') {
			fNoKeyword = true;
			_GetNextChar();
		}
		c = _GetNextChar();
	}
	// check whether it is a keyword or not
	token.length = fOffset - token.offset;
	int state = 0;
	if (!fNoKeyword) {
		state = 1;
		for (int i = 0; state && i < token.length; i++)
			state = fProxy.Move(token.string[i], state);
		state = fProxy.IsKeyword(state);
	}
	if (state) {
		token.type = Token::KEYWORD;
		token.keyword = state;
	} else {
		token.type = Token::TEXT;
		// handle the separators and operators here
		if (token.length == 1) {
			switch (*token.string) {
				case '{': token.type = Token::LBRACE; break;
				case '}': token.type = Token::RBRACE; break;
				case '[': token.type = Token::LBRACKET; break;
				case ']': token.type = Token::RBRACKET; break;
				case ':': token.type = Token::COLON; break;
				case ';': token.type = Token::SEMICOLON; break;
				case '!': token.type = Token::NOT; break;
				case '&': token.type = Token::AND; break;
				case '<': token.type = Token::LESS; break;
				case '>': token.type = Token::GREATER; break;
				case '|': token.type = Token::OR; break;
				case '=': token.type = Token::EQUAL; break;
			}
		} else if (token.length == 2) {
			if (token.string[1] == '=') {
				switch (*token.string) {
					case '!': token.type = Token::UNEQUAL; break;
					case '+': token.type = Token::ADD_ASSIGN; break;
					case '<': token.type = Token::LESS_EQUAL; break;
					case '>': token.type = Token::GREATER_EQUAL; break;
					case '?': token.type = Token::CONDITIONAL_ASSIGN; break;
				}
			} else if (token.string[0] == '&' && token.string[1] == '&') {
				token.type = Token::AND;
			} else if (token.string[0] == '&' && token.string[1] == '&') {
				token.type = Token::OR;
			}
		}
	}
	return true;
}

// _GetCurrentChar
int
Tokenizer::_GetCurrentChar()
{
	if (fOffset >= fLength)
		return 0;
	return fLine[fOffset];
}

// _GetNextChar
int
Tokenizer::_GetNextChar()
{
	if (fOffset >= fLength)
		return 0;
	return fLine[++fOffset];
}

// _PutLastChar
void
Tokenizer::_PutLastChar()
{
	if (fOffset > 0)
		fOffset--;
}

// #pragma mark -
// #pragma mark ----- parsing -----

// parse state types
enum {
	DEFAULT_STATE					= 0x00,
	PRE_ACTIONS_STATE				= 0x01,
	ACTIONS_STATE					= 0x02,
	COMMENT_STATE					= 0x03,
	PRE_ACTIONS_COMMENT_STATE		= 0x04,

	STATE_COUNT						= 4,
	STATE_MASK						= 0x07,
	STATE_SHIFT						= 3,
};

// ParseState
class ParseState {
public:
	ParseState(int type)	: fType(type) {}
	virtual ~ParseState()	{}

	int GetType() const	{ return fType; }

	virtual bool Parse(ParseContext& context) const;

protected:
	virtual bool ProcessToken(ParseContext& context, const Token& token) const
		{ return true; }

private:
	int	fType;
};

// DefaultState
class DefaultState : public ParseState {
public:
	DefaultState()			: ParseState(DEFAULT_STATE) {}
	virtual ~DefaultState()	{}

protected:
	virtual bool ProcessToken(ParseContext& context, const Token& token) const;
};

// PreActionsState
class PreActionsState : public ParseState {
public:
	PreActionsState()			: ParseState(PRE_ACTIONS_STATE) {}
	virtual ~PreActionsState()	{}

protected:
	virtual bool ProcessToken(ParseContext& context, const Token& token) const;
};

// ActionsState
class ActionsState : public ParseState {
public:
	ActionsState()			: ParseState(ACTIONS_STATE) {}
	virtual ~ActionsState()	{}

	virtual bool Parse(ParseContext& context) const;
};

// CommentState
class CommentState : public ParseState {
public:
	CommentState(int type = COMMENT_STATE)	: ParseState(type) {}
	virtual ~CommentState()	{}

	virtual bool Parse(ParseContext& context) const;

protected:
	virtual const ParseState* ExitState() const;
};

// PreActionsCommentState
class PreActionsCommentState : public CommentState {
public:
	PreActionsCommentState()	: CommentState(PRE_ACTIONS_COMMENT_STATE) {}
	virtual ~PreActionsCommentState()	{}

protected:
	virtual const ParseState* ExitState() const;
};

// states

static const DefaultState kDefaultState;
static const PreActionsState kPreActionsState;
static const ActionsState kActionsState;
static const CommentState kCommentState;
static const CommentState kPreActionsCommentState;

static const ParseState* kStates[] = {
	&kDefaultState,
	&kPreActionsState,
	&kActionsState,
	&kCommentState,
	&kPreActionsCommentState,
};

// ParseAction
class ParseAction {
public:
	ParseAction()			{}
	virtual ~ParseAction()	{}

	virtual bool TokenParsed(ParseContext& context, const Token& token)
	{
		return true;
	}
};

// ParseContext
struct ParseContext {
	ParseContext(CLanguageProxy& proxy, ParseAction& action, int packedState)
		: proxy(proxy),
		  action(action),
		  offset(0),
		  size(proxy.Size()),
		  state(NULL),
		  counter(packedState >> STATE_SHIFT)
	{
		int stateType = (packedState & STATE_MASK);
		if (stateType >= 0 && stateType < STATE_COUNT)
			state = kStates[stateType];
		else
			state = &kDefaultState;
	}

	const char* GetText() const	{ return proxy.Text() + offset; }
	int GetSize() const			{ return size - offset; }

	int GetPackedState() const
		{ return (state->GetType() | counter << STATE_SHIFT); }

	CLanguageProxy&		proxy;
	ParseAction&		action;
	int					offset;
	int					size;
	const ParseState*	state;
	int					counter;
};

// Parse
bool
ParseState::Parse(ParseContext& context) const
{
	Tokenizer tokenizer(context.proxy, context.offset,
		context.size - context.offset);
	Token token;
	while (tokenizer.GetNextToken(token)) {
		if (!ProcessToken(context, token))
			return false;
		context.offset = tokenizer.GetOffset();
		if (context.state != this)
			return true;
	}
	return false;
}

// ProcessToken
bool
DefaultState::ProcessToken(ParseContext& context, const Token& token) const
{
	if (token.type == Token::KEYWORD && token.keyword == 1
		&& token == "actions") {
		context.state = &kPreActionsState;
	} else if (token.type == Token::COMMENT && !token.terminated) {
		context.state = &kCommentState;
	}
	return context.action.TokenParsed(context, token);
}

// ProcessToken
bool
PreActionsState::ProcessToken(ParseContext& context, const Token& token) const
{
	if (token.type == Token::LBRACE) {
		context.counter = 1;
		context.state = &kActionsState ;
	} else if (token.type == Token::COMMENT && !token.terminated) {
		context.state = &kPreActionsCommentState;
	}
	return context.action.TokenParsed(context, token);
}

// Parse
bool
ActionsState::Parse(ParseContext& context) const
{
	const char* text = context.GetText();
	int size = context.GetSize();
	Token token;
	token.string = text;
	token.offset = context.offset;
	token.type = Token::ACTIONS;
	// match braces
	for (int i = 0; i < size; i++) {
		if (text[i] == '{') {
			context.counter++;
		} else if (text[i] == '}') {
			context.counter--;
			if (context.counter == 0) {
				context.offset += i;
				context.state = &kDefaultState;
				if (i > 0) {
					token.length = i;
					return context.action.TokenParsed(context, token);
				}
				return true;
			}
		}
	}
	token.length = size;
	context.action.TokenParsed(context, token);
	return false;
}

// Parse
bool
CommentState::Parse(ParseContext& context) const
{
	const char* text = context.GetText();
	int size = context.GetSize();
	Token token;
	token.string = text;
	token.offset = context.offset;
	token.type = Token::COMMENT;
	// find new line or EOF
	for (int i = 0; i < size; i++) {
		if (text[i] == '\n') {
			context.offset += i;
			context.state = ExitState();
			if (i > 0) {
				token.length = i;
				token.terminated = true;
				return context.action.TokenParsed(context, token);
			}
			return true;
		}
	}
	token.length = size;
	token.terminated = false;
	context.action.TokenParsed(context, token);
	return false;
}

// ExitState
const ParseState*
CommentState::ExitState() const
{
	return &kDefaultState;
}

// ExitState
const ParseState*
PreActionsCommentState::ExitState() const
{
	return &kPreActionsState;
}


// #pragma mark -
// #pragma mark ----- coloring -----

// ColorParseAction
class ColorParseAction : public ParseAction {
public:
	ColorParseAction() : ParseAction() {}

	virtual bool TokenParsed(ParseContext& context, const Token& token);
};

// TokenParsed
bool
ColorParseAction::TokenParsed(ParseContext& context, const Token& token)
{
	switch (token.type) {
		case Token::STRING:
			context.proxy.SetColor(token.offset, kColorString1);
			break;
		case Token::COMMENT:
			context.proxy.SetColor(token.offset, kColorComment1);
			break;
		case Token::TEXT:
			context.proxy.SetColor(token.offset, kColorText);
			break;
		case Token::KEYWORD:
		{
			int color = kColorText;
			switch (token.keyword) {
				case 1:	color = kColorKeyword1; break;
				case 2:	color = kColorUserSet1; break;
				case 3:	color = kColorUserSet2; break;
				case 4:	color = kColorUserSet3; break;
				case 5:	color = kColorUserSet4; break;
			}
			context.proxy.SetColor(token.offset, color);
			break;
		}
		case Token::LBRACE:
		case Token::RBRACE:
		case Token::LBRACKET:
		case Token::RBRACKET:
			context.proxy.SetColor(token.offset, kColorSeparator1);
			break;
		case Token::COLON:
		case Token::SEMICOLON:
			context.proxy.SetColor(token.offset, kColorSeparator2);
			break;
		case Token::NOT:
		case Token::AND:
		case Token::LESS:
		case Token::GREATER:
		case Token::OR:
		case Token::EQUAL:
		case Token::UNEQUAL:
		case Token::ADD_ASSIGN:
		case Token::LESS_EQUAL:
		case Token::GREATER_EQUAL:
		case Token::CONDITIONAL_ASSIGN:
			context.proxy.SetColor(token.offset, kColorOperator1);
			break;
		case Token::ACTIONS:
			context.proxy.SetColor(token.offset, kColorText);
			break;
	}
	return true;
}

// ColorLine
_EXPORT
void
ColorLine(CLanguageProxy& proxy, int& packedState)
{
	ColorParseAction action;
	ParseContext context(proxy, action, packedState);
	while (context.state->Parse(context));
	packedState = context.GetPackedState();
}

// #pragma mark -
// #pragma mark ----- function scanning -----

// FunctionScanParseAction
class FunctionScanParseAction : public ParseAction {
public:
	FunctionScanParseAction() : ParseAction(), fBuffer(), fOffset(-1) {}

	virtual bool TokenParsed(ParseContext& context, const Token& token);

private:
	BString	fBuffer;
	int		fOffset;
};

// TokenParsed
bool
FunctionScanParseAction::TokenParsed(ParseContext& context, const Token& token)
{
	if (token.type == Token::COMMENT) {
		// a comment: check for pragma mark
		if (token.length > 0 && *token.string == '#') {
			Tokenizer tokenizer(context.proxy, token.offset + 1,
				token.length - 1);
			Token commentToken;
			if (tokenizer.GetNextToken(commentToken)
				&& commentToken.type == Token::TEXT
				&& commentToken == "pragma"
				&& tokenizer.GetNextToken(commentToken)
				&& commentToken.type == Token::TEXT
				&& commentToken == "mark"
				&& tokenizer.GetNextToken(commentToken)
				&& commentToken.length > 0) {
				int nameLength = token.offset + token.length
					- commentToken.offset;
				BString buffer(commentToken.string, nameLength);
				context.proxy.AddSeparator(buffer.String());
			}
		}
	} else if (fOffset < 0) {
		// we have read a "rule" or "actions" keyword yet: check if this is one
		if (token.type == Token::KEYWORD && token.keyword == 1
			&& (token == "actions" || token == "rule")) {
			fBuffer.SetTo(token.string, token.length);
			fOffset = token.offset;
		}
	} else {
		// we've already read a "rule" or "actions" keyword: get the text
		if (token.type == Token::TEXT) {
			fBuffer << ' ';
			int nameOffset = fBuffer.Length();
			fBuffer.Append(token.string, token.length);
			context.proxy.AddFunction(fBuffer.String(),
				fBuffer.String() + nameOffset, fOffset);
		}
		fOffset = -1;
	}
	return true;
}

// ScanForFunctions
_EXPORT
void
ScanForFunctions(CLanguageProxy& proxy)
{
	FunctionScanParseAction action;
	ParseContext context(proxy, action, DEFAULT_STATE);
	while (context.state->Parse(context));
}
