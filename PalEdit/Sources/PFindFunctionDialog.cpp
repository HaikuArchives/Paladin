// PFindFunctionDialog.cpp

#include <new>

#include <Message.h>
#include <List.h>

#include "HError.h"
#include "PFindFunctionDialog.h"
#include "PMessages.h"

// Choice
struct PFindFunctionDialogController::Choice : public CFilterChoiceItem {
	Choice(const char *name, uint32 what, bool isSeparator, bool italic, 
		uint32 nestLevel = 0);

	virtual bool IsSeparator() const	{ return isSeparator; }
	virtual bool IsItalic() const		{ return italic; }
	virtual const char *Name() const	{ return name.String(); }
	virtual uint32 NestLevel() const	{ return nestLevel; }

	BString		name;
	BMessage	message;
	bool		isSeparator;
	bool		italic;
	uint32		nestLevel;
};

// constructor
PFindFunctionDialogController::Choice::Choice(const char *nm, uint32 what, 
	bool isSeparator, bool italic, uint32 nestLevel)
	: name(nm),
	  message(what), 
	  isSeparator(isSeparator), 
	  italic(italic),
	  nestLevel(nestLevel)
{
	if (nestLevel)
	{
		int indent = 4 * nestLevel;
		name.Prepend(' ', indent);
	}
}


// ChoiceModel
class PFindFunctionDialogController::ChoiceModel : public CFilterChoiceModel {
public:
	ChoiceModel();
	virtual ~ChoiceModel();

	virtual int CountChoiceGroups() const;
	virtual int CountChoiceItems(int groupIndex) const;
	virtual CFilterChoiceItem *ChoiceItemAt(int groupIndex, int index) const;

	void AddIncludeChoice(Choice *choice);
	void AddFunctionChoice(Choice *choice);
	Choice *ChoiceAt(int32 index) const;

private:
	BList	fIncludeChoices;
	BList	fFunctionChoices;
};

// constructor
PFindFunctionDialogController::ChoiceModel::ChoiceModel()
	: fIncludeChoices(),
	  fFunctionChoices()
{
}

// destructor
PFindFunctionDialogController::ChoiceModel::~ChoiceModel()
{
	for (int32 i = 0; Choice *choice = (Choice*)fIncludeChoices.ItemAt(i); i++)
		delete choice;
	for (int32 i = 0; Choice *choice = (Choice*)fFunctionChoices.ItemAt(i); i++)
		delete choice;
}

// CountChoiceGroups
int
PFindFunctionDialogController::ChoiceModel::CountChoiceGroups() const
{
	return 2;
}

// CountChoiceItems
int
PFindFunctionDialogController::ChoiceModel::CountChoiceItems(
	int groupIndex) const
{
	if (groupIndex == 0)
		return fIncludeChoices.CountItems();
	if (groupIndex == 1)
		return fFunctionChoices.CountItems();
	return 0;
}

// ChoiceItemAt
CFilterChoiceItem *
PFindFunctionDialogController::ChoiceModel::ChoiceItemAt(int groupIndex,
	int index) const
{
	if (groupIndex == 0)
		return (CFilterChoiceItem*)fIncludeChoices.ItemAt(index);
	if (groupIndex == 1)
		return (CFilterChoiceItem*)fFunctionChoices.ItemAt(index);
	return NULL;
}

// AddIncludeChoice
void
PFindFunctionDialogController::ChoiceModel::AddIncludeChoice(Choice *choice)
{
	if (choice)
		fIncludeChoices.AddItem(choice);
}

// AddFunctionChoice
void
PFindFunctionDialogController::ChoiceModel::AddFunctionChoice(Choice *choice)
{
	if (choice)
		fFunctionChoices.AddItem(choice);
}


// #pragma mark -

// constructor
PFindFunctionDialogController::PFindFunctionDialogController(PText *text)
	: CFilterChoiceDialog::Listener(),
	  CFunctionScanHandler(),
	  fDialog(NULL),
	  fTarget(text),
	  fChoiceModel(new(std::nothrow) ChoiceModel)
{
	FailNil(fChoiceModel);
	text->ScanForFunctions(*this);
	fDialog = new(std::nothrow) CFilterChoiceDialog("Find Function", fChoiceModel, 1);
	FailNil(fDialog);
	fDialog->SetCaller(text->Window());
	fDialog->SetListener(this);
	fDialog->Show();
}

// destructor
PFindFunctionDialogController::~PFindFunctionDialogController()
{
}

// FilterChoiceDialogCommitted
void
PFindFunctionDialogController::FilterChoiceDialogCommitted(
	CFilterChoiceDialog *dialog, CFilterChoiceItem *_choice)
{
	if (Choice *choice = dynamic_cast<Choice*>(_choice))
		fTarget.SendMessage(&choice->message);
	delete this;
}

// FilterChoiceDialogAborted
void
PFindFunctionDialogController::FilterChoiceDialogAborted(
	CFilterChoiceDialog *dialog)
{
	delete this;
}

// AddFunction
void
PFindFunctionDialogController::AddFunction(const char *name, const char *match,
	int offset, bool italic, uint32 nestLevel, const char *params)
{
	Choice *choice = new(std::nothrow) Choice(name, msg_JumpToProcedure, false,
		italic, nestLevel);
	FailNil(choice);
	choice->message.AddInt32("offset", offset);
	choice->message.AddString("function", match);
	fChoiceModel->AddFunctionChoice(choice);
}

// AddInclude
void
PFindFunctionDialogController::AddInclude(const char *name, const char *open,
	bool italic)
{
	Choice *choice = new(std::nothrow) Choice(name, msg_OpenInclude, false, italic);
	FailNil(choice);
	choice->message.AddString("include", open);
	fChoiceModel->AddIncludeChoice(choice);
}

// AddSeparator
void
PFindFunctionDialogController::AddSeparator(const char* name)
{
	Choice *choice = new(std::nothrow) Choice(name, 0, true, false);
	FailNil(choice);
	fChoiceModel->AddFunctionChoice(choice);
}

