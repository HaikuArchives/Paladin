/*
 * Copyright 2002-2007, Haiku. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm (darkwyrm@earthlink.net)
 */
#include "ColorWell.h"
#include <Box.h>
#include <Button.h>
#include <ColorControl.h>
#include <Menu.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <Messenger.h>
#include <PropertyInfo.h>
#include <Screen.h>
#include <TypeConstants.h>

class ColorWellEditor : public BWindow
{
public:
			ColorWellEditor(BRect frame, BMessenger target, const rgb_color &col);
	void	MessageReceived(BMessage *msg);
	
private:
	BMessenger		fTarget;
	BBox			*fBox;
	BColorControl	*fEditor;
	BMenuField		*fField;
};

void AddPresetColor(BMenu *menu, const char *name, const rgb_color &col);
void AddPresetColor(BMenu *menu, const char *name, const uint8 &r, const uint8 &g, const uint8 &b);

static property_info sProperties[] = {
	{ "Red", { B_GET_PROPERTY, 0 }, { B_DIRECT_SPECIFIER, 0 },
		"Returns the red value for the color well.", 0, { B_INT32_TYPE }
	},
	{ "Red", { B_SET_PROPERTY, 0 }, { B_DIRECT_SPECIFIER, 0},
		"Sets the red value for the color well.", 0, { B_INT32_TYPE }
	},
	
	{ "Green", { B_GET_PROPERTY, 0 }, { B_DIRECT_SPECIFIER, 0 },
		"Returns the green value for the color well.", 0, { B_INT32_TYPE }
	},
	{ "Green", { B_SET_PROPERTY, 0 }, { B_DIRECT_SPECIFIER, 0},
		"Sets the green value for the color well.", 0, { B_INT32_TYPE }
	},
	
	{ "Blue", { B_GET_PROPERTY, 0 }, { B_DIRECT_SPECIFIER, 0 },
		"Returns the blue value for the color well.", 0, { B_INT32_TYPE }
	},
	{ "Blue", { B_SET_PROPERTY, 0 }, { B_DIRECT_SPECIFIER, 0},
		"Sets the blue value for the color well.", 0, { B_INT32_TYPE }
	},
};

enum
{
	M_SET_COLOR = 'stcl',
	M_UPDATE_COLOR = 'upcl',
	M_CHOOSE_PRESET = 'chps'
};


ColorWell::ColorWell(BRect frame, const char *name, BMessage *message,
					int32 resize, int32 flags, int32 style)
	:	BControl(frame,name,NULL,message, resize, flags | B_SUBPIXEL_PRECISE),
				fStyle(style),
				fUseEditor(true)
{
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	SetLowColor(0,0,0);
	
	fDisabledColor.red = 190;
	fDisabledColor.green = 190;
	fDisabledColor.blue = 190;
	fDisabledColor.alpha = 255;
}


ColorWell::ColorWell(BMessage *data)
	:	BControl(data),
		fUseEditor(true)
{
	// This will update fColor
	SetValue(Value());
}


ColorWell::~ColorWell(void)
{
}


BArchivable *
ColorWell::Instantiate(BMessage *data)
{
	if (validate_instantiation(data, "ColorWell"))
		return new ColorWell(data);

	return NULL;
}


status_t
ColorWell::Archive(BMessage *data, bool deep) const
{
	status_t status = BControl::Archive(data, deep);
	data->AddString("class","ColorWell");
	
	return status;
}

		
status_t
ColorWell::GetSupportedSuites(BMessage *msg)
{
	msg->AddString("suites","suite/vnd.DW-colorwell");
	
	BPropertyInfo prop_info(sProperties);
	msg->AddFlat("messages",&prop_info);
	return BView::GetSupportedSuites(msg);
}


BHandler *
ColorWell::ResolveSpecifier(BMessage *msg, int32 index, BMessage *specifier,
							int32 form, const char *property)
{
	return BControl::ResolveSpecifier(msg, index, specifier, form, property);
}


void
ColorWell::SetValue(int32 value)
{
	BControl::SetValue(value);
	
	fColor.red = (value & 0xFF000000) >> 24;
	fColor.green = (value & 0x00FF0000) >> 16;
	fColor.blue = (value & 0x0000FF00) >> 8;
	
	SetHighColor(fColor);
	if (Window())
	{
		Draw(Bounds());
		Invalidate();
	}
}

void
ColorWell::SetValue(const rgb_color &col)
{
	fColor = col;
	
	SetHighColor(col);
	if (Window())
	{
		Draw(Bounds());
		Invalidate();
	}
}


void
ColorWell::SetValue(const uint8 &r,const uint8 &g, const uint8 &b)
{
	fColor.red = r;
	fColor.green = g;
	fColor.blue = b;
	
	SetHighColor(r,g,b);
	if (Window())
	{
		Draw(Bounds());
		Invalidate();
	}
}


void
ColorWell::SetStyle(const int32 &style)
{
	if (style != fStyle) {
		fStyle = style;
		Invalidate();
	}
}


int32
ColorWell::Style(void) const
{
	return fStyle;
}


void
ColorWell::UseEditor(const bool &value)
{
	fUseEditor = value;
}


bool
ColorWell::UsesEditor(void) const
{
	return fUseEditor;
}


void
ColorWell::MessageReceived(BMessage *msg)
{
	// If we received a dropped message, try to see if it has color data in it
	if (msg->WasDropped()) {
		
		uint8 *data;
		ssize_t size;
		
		// Note that the pointer is not deleted because the BMessage retains
		// ownership of the data
		if (msg->FindData("RGBColor",B_RGB_COLOR_TYPE,
							(const void**)&data,&size) == B_OK) {
			fColor = *((rgb_color*)data);
			SetHighColor(fColor);
		}
	}
	if (msg->what == M_SET_COLOR)
	{
		rgb_color *col;
		ssize_t size;
		if (msg->FindData("color",B_RGB_COLOR_TYPE,(const void **)&col,&size) == B_OK)
		{
			SetValue(*col);
			Invalidate();
			Invoke();
		}
		return;
	}
	else
		BView::MessageReceived(msg);
}


void
ColorWell::Draw(BRect update)
{
	if (fStyle == COLORWELL_SQUARE_WELL)
		DrawSquare();
	else
		DrawRound();
}


rgb_color
ColorWell::ValueAsColor(void) const
{
	return fColor;
}


void
ColorWell::MouseUp(BPoint pt)
{
	if (UsesEditor())
	{
		// Figure out where the window should go. Ideally, it should go to the right
		// of the view.
		BRect screen(BScreen().Frame());
		BRect r(BRect(100,100,500,400));
		ColorWellEditor *editor = new ColorWellEditor(r,BMessenger(this),
														ValueAsColor());
		r = editor->Frame();
		BRect frame(Bounds());
		ConvertToScreen(&frame);
		if (frame.right + 5.0 + r.Width() > screen.right)
			r.OffsetTo(frame.left - 5.0 - r.Width(),
						frame.top + ((frame.Height() - r.Height()) / 2.0));
		else
			r.OffsetTo(frame.right + 5.0,frame.top + ((frame.Height() - r.Height()) / 2.0));
		
		if (r.top < 0)
			r.OffsetBy(0.0,(-r.top) + 10);
		else if (r.bottom > screen.bottom)
			r.OffsetBy(0.0,screen.bottom - r.bottom - 10.0);
		editor->MoveTo(r.left,r.top);
		editor->Show();
	}
}


void
ColorWell::DrawRound(void)
{
	BRect r(Bounds());
	r.InsetBy(1,1);
	
	if (IsEnabled()) {
		SetHighColor(fColor);
		FillEllipse(Bounds());
		
		SetPenSize(2.0);
		SetHighColor(tint_color(fColor,B_DARKEN_1_TINT));
		StrokeArc(r, 45, 180);
		
		SetHighColor(tint_color(fColor,B_LIGHTEN_1_TINT));
		StrokeArc(r, 225, 180);
		SetPenSize(1.0);
		
		SetHighColor(0,0,0);
		StrokeEllipse(Bounds());
		
		SetHighColor(0,0,0);
		StrokeArc(Bounds(), 45, 180);
		
		SetHighColor(255,255,255);
		StrokeArc(Bounds(), 225, 180);
		
	} else {
		SetHighColor(fDisabledColor);
		FillEllipse(Bounds());
		
		SetPenSize(2.0);
		SetHighColor(tint_color(fDisabledColor,B_DARKEN_1_TINT));
		StrokeArc(r, 45, 180);
		
		SetHighColor(tint_color(fDisabledColor,B_LIGHTEN_1_TINT));
		StrokeArc(r, 225, 180);
		SetPenSize(1.0);
		
		SetHighColor(0,0,0);
		StrokeEllipse(Bounds());
		
		SetHighColor(120,120,120);
		StrokeArc(Bounds(), 45, 180);
		
		SetHighColor(160,160,160);
		StrokeArc(Bounds(), 225, 180);
	}
}


void
ColorWell::DrawSquare(void)
{
	// This code adapted from Haiku's BButton::Draw
	const rgb_color fillColor = IsEnabled() ? fColor : fDisabledColor;
	const rgb_color highlightHigh = tint_color(fillColor, IsEnabled() ?
														B_LIGHTEN_2_TINT :
														B_LIGHTEN_1_TINT);
	const rgb_color highlightLow = tint_color(fillColor, B_LIGHTEN_1_TINT);
	const rgb_color shadowHigh = tint_color(fillColor, B_DARKEN_1_TINT);
	const rgb_color shadowLow = tint_color(fillColor, IsEnabled() ?
													 B_DARKEN_2_TINT :
													 B_DARKEN_1_TINT);
	const rgb_color panelColor = ui_color(B_PANEL_BACKGROUND_COLOR);
	
	SetHighColor(fillColor);
	FillRect(Bounds());
	
	BRect rect(Bounds());
	
	// external border
	SetHighColor(0,0,0);
	StrokeRect(rect);
		
	BeginLineArray(14);

	// Corners
	AddLine(rect.LeftTop(), rect.LeftTop(), panelColor);
	AddLine(rect.LeftBottom(), rect.LeftBottom(), panelColor);
	AddLine(rect.RightTop(), rect.RightTop(), panelColor);
	AddLine(rect.RightBottom(), rect.RightBottom(), panelColor);

	rect.InsetBy(1.0f,1.0f);
	
	// Shadow
	AddLine(rect.LeftBottom(), rect.RightBottom(), highlightHigh);
	AddLine(rect.RightBottom(), rect.RightTop(), highlightHigh);
	
	// Light
	AddLine(rect.LeftTop(), rect.LeftBottom(),shadowLow);
	AddLine(rect.LeftTop(), rect.RightTop(), shadowLow);	
	
	rect.InsetBy(1.0f, 1.0f);
	
	// Shadow
	AddLine(rect.LeftBottom(), rect.RightBottom(), highlightLow);
	AddLine(rect.RightBottom(), rect.RightTop(), highlightLow);
	
	// Light
	AddLine(rect.LeftTop(), rect.LeftBottom(),shadowHigh);
	AddLine(rect.LeftTop(), rect.RightTop(), shadowHigh);	
	
	EndLineArray();	
}


ColorWellEditor::ColorWellEditor(BRect frame, BMessenger target, const rgb_color &col)
	:	BWindow(frame,"Choose Color",B_FLOATING_WINDOW,B_ASYNCHRONOUS_CONTROLS | B_NOT_RESIZABLE),
		fTarget(target)
{
	BView *top = new BView(Bounds(),"top",B_FOLLOW_ALL,B_WILL_DRAW);
	AddChild(top);
	top->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	
	fEditor = new BColorControl(BPoint(10.0,10.0),B_CELLS_32x8,10.0,"editor",
											new BMessage(M_UPDATE_COLOR),true);
	top->AddChild(fEditor);
	fEditor->SetValue(col);
	
	BRect r = fEditor->Frame();
	r.OffsetBy(r.Width() + 10.0, 0);
	r.right = r.left + 25.0;
	fBox = new BBox(r,"box");
	top->AddChild(fBox);
	fBox->SetViewColor(col);
	
	BButton *ok = new BButton(BRect(0,0,1,1), "OK", "Cancel", new BMessage(M_SET_COLOR),
								B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM);
	ok->ResizeToPreferred();
	ok->SetLabel("OK");
	ok->MoveTo(Bounds().right - 10.0 - ok->Frame().Width(),
				Bounds().bottom - 10.0 - ok->Frame().Height());
	top->AddChild(ok);
	
	BButton *cancel = new BButton(BRect(0,0,1,1), "Cancel", "Cancel", new BMessage(B_QUIT_REQUESTED),
								B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM);
	cancel->ResizeToPreferred();
	cancel->MoveTo(ok->Frame().left - 10.0 - cancel->Frame().Width(),
				Bounds().bottom - 10.0 - cancel->Frame().Height());
	top->AddChild(cancel);
	
	BMenu *menu = new BMenu("Presets");
	BMenu *submenu = new BMenu("System Colors");
	
	// All platforms
	AddPresetColor(submenu,"Panel",ui_color(B_PANEL_BACKGROUND_COLOR));
	
	#if defined(__ZETA__) || defined(__HAIKU__)
		AddPresetColor(submenu,"Panel Text",ui_color(B_PANEL_TEXT_COLOR));
		AddPresetColor(submenu,"Document Background",ui_color(B_DOCUMENT_BACKGROUND_COLOR));
		AddPresetColor(submenu,"Document Text",ui_color(B_DOCUMENT_TEXT_COLOR));
		AddPresetColor(submenu,"Control Background",ui_color(B_CONTROL_BACKGROUND_COLOR));
		AddPresetColor(submenu,"Control Text",ui_color(B_CONTROL_TEXT_COLOR));
		AddPresetColor(submenu,"Control Border",ui_color(B_CONTROL_BORDER_COLOR));
		AddPresetColor(submenu,"Control Highlight",ui_color(B_CONTROL_HIGHLIGHT_COLOR));
		AddPresetColor(submenu,"Navigation Base",ui_color(B_NAVIGATION_BASE_COLOR));
		AddPresetColor(submenu,"Navigation Pulse",ui_color(B_NAVIGATION_PULSE_COLOR));
		AddPresetColor(submenu,"Shine",ui_color(B_SHINE_COLOR));
		AddPresetColor(submenu,"Shadow",ui_color(B_SHADOW_COLOR));
	#endif
	
	AddPresetColor(submenu,"Menu Background",ui_color(B_MENU_BACKGROUND_COLOR));
	AddPresetColor(submenu,"Menu Selection Background",ui_color(B_MENU_SELECTION_BACKGROUND_COLOR));
	AddPresetColor(submenu,"Menu Item",ui_color(B_MENU_ITEM_TEXT_COLOR));
	AddPresetColor(submenu,"Menu Item Selected Text",ui_color(B_MENU_SELECTED_ITEM_TEXT_COLOR));
	
	#if !defined(__ZETA__) && !defined(__HAIKU__)
		AddPresetColor(submenu,"Window Tab",ui_color(B_WINDOW_TAB_COLOR));
		AddPresetColor(submenu,"Keyboard Navigation",ui_color(B_KEYBOARD_NAVIGATION_COLOR));
	#endif
	
	#if defined(__ZETA__) || defined(__HAIKU__)
		#ifdef __HAIKU__
			AddPresetColor(submenu,"Tooltip Background",ui_color(B_TOOL_TIP_BACKGROUND_COLOR));
			AddPresetColor(submenu,"Tooltip Text",ui_color(B_TOOL_TIP_TEXT_COLOR));
		#else
			AddPresetColor(submenu,"Tooltip Background",ui_color(B_TOOLTIP_BACKGROUND_COLOR));
			AddPresetColor(submenu,"Tooltip Text",ui_color(B_TOOLTIP_TEXT_COLOR));
		#endif
		
		AddPresetColor(submenu,"Success",ui_color(B_SUCCESS_COLOR));
		AddPresetColor(submenu,"Failure",ui_color(B_FAILURE_COLOR));
	#endif
	menu->AddItem(submenu);
	menu->AddSeparatorItem();
	AddPresetColor(menu,"Black",0,0,0);
	AddPresetColor(menu,"Dark Gray",100,100,100);
	AddPresetColor(menu,"Light Gray",200,200,200);
	AddPresetColor(menu,"White",255,255,255);
	menu->AddSeparatorItem();
	AddPresetColor(menu,"Red",255,0,0);
	AddPresetColor(menu,"Yellow",255,255,0);
	AddPresetColor(menu,"Green",0,192,0);
	AddPresetColor(menu,"Blue",0,0,255);
	AddPresetColor(menu,"Purple",128,0,128);
	
	r.Set(0,0,100,25);
	r.OffsetTo(10.0,fEditor->Frame().bottom + 10.0);
	fField = new BMenuField(r,"field","",menu);
	fField->SetDivider(0L);
	top->AddChild(fField);
	
	ResizeTo(fBox->Frame().right + 10.0, fEditor->Frame().bottom + 20.0 + ok->Frame().Height());
	
	ok->MakeDefault(true);
	fEditor->MakeFocus();
}


void
ColorWellEditor::MessageReceived(BMessage *msg)
{
	switch (msg->what)
	{
		case M_UPDATE_COLOR:
		{
			fBox->SetViewColor(fEditor->ValueAsColor());
			fBox->Invalidate();
			break;
		}
		case M_CHOOSE_PRESET:
		{
			rgb_color *col;
			ssize_t size;
			if (msg->FindData("color",B_RGB_COLOR_TYPE,(const void **)&col,&size) == B_OK)
			{
				fEditor->SetValue(*col);
				fBox->SetViewColor(*col);
				fBox->Invalidate();
			}
			break;
		}
		case M_SET_COLOR:
		{
			rgb_color col = fEditor->ValueAsColor();
			BMessage colormsg(M_SET_COLOR);
			colormsg.AddData("color",B_RGB_COLOR_TYPE,(const void *)&col,sizeof(rgb_color));
			fTarget.SendMessage(colormsg);
			PostMessage(B_QUIT_REQUESTED);
			break;
		}
		default:
		{
			BWindow::MessageReceived(msg);
			break;
		}
	}
}


void
AddPresetColor(BMenu *menu, const char *name, const rgb_color &col)
{
	if (!name)
		return;
	
	BMessage *msg = new BMessage(M_CHOOSE_PRESET);
	msg->AddData("color",B_RGB_COLOR_TYPE,(const void *)&col,sizeof(rgb_color));
	menu->AddItem(new BMenuItem(name,msg));
}


void
AddPresetColor(BMenu *menu, const char *name, const uint8 &r, const uint8 &g, const uint8 &b)
{
	if (!name)
		return;
	
	rgb_color col= {r,g,b,255};
	
	BMessage *msg = new BMessage(M_CHOOSE_PRESET);
	msg->AddData("color",B_RGB_COLOR_TYPE,(const void *)&col,sizeof(rgb_color));
	menu->AddItem(new BMenuItem(name,msg));
}

