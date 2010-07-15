// CFilterChoiceDialog.h

#ifndef CFILTERCHOICEDIALOG_H
#define CFILTERCHOICEDIALOG_H

#include <String.h>
#include "HDialog.h"

class BListView;
class BListItem;
class BTextControl;

// CFilterChoiceItem
class CFilterChoiceItem {
public:
	CFilterChoiceItem();
	virtual ~CFilterChoiceItem();

	virtual bool IsSeparator() const;
	virtual bool IsItalic() const;
	virtual const char *Name() const = 0;
	virtual uint32 NestLevel() const;
};

// CSeparatorFilterChoiceItem
class CSeparatorFilterChoiceItem : public CFilterChoiceItem {
public:
	CSeparatorFilterChoiceItem();
	virtual ~CSeparatorFilterChoiceItem();

	virtual bool IsSeparator() const;
	virtual const char *Name() const;

	static const CSeparatorFilterChoiceItem *UNNAMED_SEPARATOR;
};

// CFilterChoiceModel
class CFilterChoiceModel {
public:
	CFilterChoiceModel();
	virtual ~CFilterChoiceModel();

	virtual int CountChoiceGroups() const = 0;
	virtual int CountChoiceItems(int groupIndex) const = 0;
	virtual CFilterChoiceItem *ChoiceItemAt(int groupIndex,
		int index) const = 0;
};

// CFilterChoiceDialog
class CFilterChoiceDialog : public HDialog {
public:
	class Listener {
	public:
		Listener();
		virtual ~Listener();

		virtual void FilterChoiceDialogCommitted(CFilterChoiceDialog *dialog,
			CFilterChoiceItem *choice);
		virtual void FilterChoiceDialogAborted(CFilterChoiceDialog *dialog);
	};

public:
	CFilterChoiceDialog(const char *title, CFilterChoiceModel *model,
		int defaultSelectGroup = 0);
	virtual ~CFilterChoiceDialog();

	virtual void MessageReceived(BMessage *message);
	virtual void DispatchMessage(BMessage *message, BHandler *handler);
	virtual void Quit();
	virtual void WindowActivated(bool state);
	virtual void WorkspacesActivated(int32 workspace, bool active);

	CFilterChoiceModel *Model() const;

	void SetListener(Listener *listener);

protected:
	virtual void DialogCommitted(CFilterChoiceItem *choice);
	virtual void DialogAborted();

private:
	struct ChoiceGroupInfo;
	class ChoiceListItem;
	struct ChoiceItemInfo;
	class SeparatorListItem;
	class Filter;

	void _ResizeWindow();

	void _DispatchKeyDown(BMessage *message, BHandler *handler);
	void _DispatchMouseDown(BMessage *message, BHandler *handler);

	void _SetFilter(const char *newFilterText);
	void _RebuildList();
	void _CoalesceSeparators();

	ChoiceItemInfo* _InfoForItem(BListItem *item) const;
	bool _FilterItem(ChoiceItemInfo *info) const;

	int32 _FirstVisibleIndex() const;
	int32 _LastVisibleIndex() const;
	int32 _FirstVisibleSelectableIndex() const;
	int32 _LastVisibleSelectableIndex() const;
	bool _IsSelectableItem(int32 index) const;
	void _SelectAnyVisibleItem();
	void _SelectItem(int index, bool searchForward);
	void _SelectPreviousItem();
	void _SelectNextItem();
	void _SelectFirstVisibleItem();
	void _SelectLastVisibleItem();

private:
	CFilterChoiceModel	*fModel;
	Listener			*fListener;
	BString				fFilterString;
	BTextControl		*fFilterStringControl;
	BListView			*fChoicesList;
	ChoiceGroupInfo		*fGroupInfos;
	int					fGroupCount;
	int					fDefaultSelectGroup;
	ChoiceItemInfo		*fItemInfos;
	int					fItemCount;
	ChoiceItemInfo		*fChosenItem;
	BFont				fPlainFont;
	BFont				fItalicFont;
	bool				fWindowActive;
};

#endif	// CFILTERCHOICEDIALOG_H
