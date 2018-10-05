/*
 * Copyright 2001-2010 DarkWyrm <bpmagic@columbus.rr.com>
 * Copyright 2014 John Scipione <jscipione@gmail.com>
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		DarkWyrm, bpmagic@columbus.rr.com
 *		John Scipione, jscipione@gmail.com
 */


#include "AsciiWindow.h"

#include <algorithm>

#include <stdio.h>
#include <stdlib.h>

#include <Catalog.h>
#include <Deskbar.h>
#include <LayoutBuilder.h>
#include <ListView.h>
#include <Locale.h>
#include <Screen.h>
#include <ScrollView.h>
#include <String.h>
#include <View.h>

#include "ObjectList.h"


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "AsciiWindow"



static const float kWindowWidth = 400.0f;
static const float kWindowHeight = 600.0f;


typedef struct {
	int value;
	char code[4];
	BString description;
} ascii_info;


static ascii_info sAsciiTable[128] = {
	{	0,		"NUL",	B_TRANSLATE_COMMENT("NULL", "ASCII table description")						},
	{	1,		"SOH",	B_TRANSLATE_COMMENT("Start of heading", "ASCII table description")			},
	{	2,		"STX",	B_TRANSLATE_COMMENT("Start of text", "ASCII table description")				},
	{	3,		"ETX",	B_TRANSLATE_COMMENT("End of text", "ASCII table description")				},
	{	4,		"EOT",	B_TRANSLATE_COMMENT("End of transmission", "ASCII table description")		},
	{	5,		"ENQ",	B_TRANSLATE_COMMENT("Enquiry", "ASCII table description")					},
	{	6,		"ACK",	B_TRANSLATE_COMMENT("Acknowledge", "ASCII table description")				},
	{	7,		"BEL",	B_TRANSLATE_COMMENT("Bell", "ASCII table description")						},
	{	8,		"BS",	B_TRANSLATE_COMMENT("Backspace", "ASCII table description")					},
	{	9,		"TAB",	B_TRANSLATE_COMMENT("Horizontal tab", "ASCII table description")			},
	{	10,		"LF",	B_TRANSLATE_COMMENT("New line", "ASCII table description")					},
	{	11,		"VT",	B_TRANSLATE_COMMENT("Vertical tab", "ASCII table description")				},
	{	12,		"FF",	B_TRANSLATE_COMMENT("Form feed", "ASCII table description")					},
	{	13,		"CR",	B_TRANSLATE_COMMENT("Carriage return", "ASCII table description")			},
	{	14,		"SO",	B_TRANSLATE_COMMENT("Shift out", "ASCII table description")					},
	{	15,		"SI",	B_TRANSLATE_COMMENT("Shift in", "ASCII table description")					},
	{	16,		"DLE",	B_TRANSLATE_COMMENT("Data link escape", "ASCII table description")			},
	{	17,		"DC1",	B_TRANSLATE_COMMENT("Device Control 1", "ASCII table description")			},
	{	18,		"DC2",	B_TRANSLATE_COMMENT("Device Control 2", "ASCII table description")			},
	{	19,		"DC3",	B_TRANSLATE_COMMENT("Device Control 3", "ASCII table description")			},
	{	20,		"DC4",	B_TRANSLATE_COMMENT("Device Control 4", "ASCII table description")			},
	{	21,		"NAK",	B_TRANSLATE_COMMENT("Negative acknowledge", "ASCII table description")		},
	{	22,		"SYN",	B_TRANSLATE_COMMENT("Synchronous idle", "ASCII table description")			},
	{	23,		"ETB",	B_TRANSLATE_COMMENT("End of transmission block", "ASCII table description")	},
	{	24,		"CAN",	B_TRANSLATE_COMMENT("Cancel", "ASCII table description")					},
	{	25,		"EM",	B_TRANSLATE_COMMENT("End of medium", "ASCII table description")				},
	{	26,		"SUB",	B_TRANSLATE_COMMENT("Substitute", "ASCII table description")				},
	{	27,		"ESC",	B_TRANSLATE_COMMENT("Escape", "ASCII table description")					},
	{	28,		"FS",	B_TRANSLATE_COMMENT("File separator", "ASCII table description")			},
	{	29,		"GS",	B_TRANSLATE_COMMENT("Group separator", "ASCII table description")			},
	{	30,		"RS",	B_TRANSLATE_COMMENT("Record separator", "ASCII table description")			},
	{	31,		"US",	B_TRANSLATE_COMMENT("Unit separator", "ASCII table description")			},
	{	32,		" ",	B_TRANSLATE_COMMENT("Space", "ASCII table description")						},
	{	33,		"!",	""																			},
	{	34,		"\"",	""																			},
	{	35,		"#",	""																			},
	{	36,		"$",	""																			},
	{	37,		"%",	""																			},
	{	38,		"&",	""																			},
	{	39,		"'",	""																			},
	{	40,		"(",	""																			},
	{	41,		")",	""																			},
	{	42,		"*",	""																			},
	{	43,		"+",	""																			},
	{	44,		",",	""																			},
	{	45,		"-",	""																			},
	{	46,		".",	""																			},
	{	47,		"/",	""																			},
	{	48,		"0",	""																			},
	{	49,		"1",	""																			},
	{	50,		"2",	""																			},
	{	51,		"3",	""																			},
	{	52,		"4",	""																			},
	{	53,		"5",	""																			},
	{	54,		"6",	""																			},
	{	55,		"7",	""																			},
	{	56,		"8",	""																			},
	{	57,		"9",	""																			},
	{	58,		":",	""																			},
	{	59,		";",	""																			},
	{	60,		"<",	""																			},
	{	61,		"=",	""																			},
	{	62,		">",	""																			},
	{	63,		"?",	""																			},
	{	64,		"@",	""																			},
	{	65,		"A",	""																			},
	{	66,		"B",	""																			},
	{	67,		"C",	""																			},
	{	68,		"D",	""																			},
	{	69,		"E",	""																			},
	{	70,		"F",	""																			},
	{	71,		"G",	""																			},
	{	72,		"H",	""																			},
	{	73,		"I",	""																			},
	{	74,		"J",	""																			},
	{	75,		"K",	""																			},
	{	76,		"L",	""																			},
	{	77,		"M",	""																			},
	{	78,		"N",	""																			},
	{	79,		"O",	""																			},
	{	80,		"P",	""																			},
	{	81,		"Q",	""																			},
	{	82,		"R",	""																			},
	{	83,		"S",	""																			},
	{	84,		"T",	""																			},
	{	85,		"U",	""																			},
	{	86,		"V",	""																			},
	{	87,		"W",	""																			},
	{	88,		"X",	""																			},
	{	89,		"Y",	""																			},
	{	90,		"Z",	""																			},
	{	91,		"[",	""																			},
	{	92,		"\\",	""																			},
	{	93,		"]",	""																			},
	{	94,		"^",	""																			},
	{	95,		"_",	""																			},
	{	96,		"`",	""																			},
	{	97,		"a",	""																			},
	{	98,		"b",	""																			},
	{	99,		"c",	""																			},
	{	100,	"d",	""																			},
	{	101,	"e",	""																			},
	{	102,	"f",	""																			},
	{	102,	"g",	""																			},
	{	104,	"h",	""																			},
	{	105,	"i",	""																			},
	{	106,	"j",	""																			},
	{	107,	"k",	""																			},
	{	108,	"l",	""																			},
	{	109,	"m",	""																			},
	{	110,	"n",	""																			},
	{	111,	"o",	""																			},
	{	112,	"p",	""																			},
	{	113,	"q",	""																			},
	{	114,	"r",	""																			},
	{	115,	"s",	""																			},
	{	116,	"t",	""																			},
	{	117,	"u",	""																			},
	{	118,	"v",	""																			},
	{	119,	"w",	""																			},
	{	120,	"x",	""																			},
	{	121,	"y",	""																			},
	{	122,	"z",	""																			},
	{	123,	"{",	""																			},
	{	124,	"|",	""																			},
	{	125,	"}",	""																			},
	{	126,	"~",	""																			},
	{	127,	"DEL",	B_TRANSLATE_COMMENT("Delete", "ASCII table description")					}
};


AsciiWindow::AsciiWindow(void)
	:
	DWindow(BRect(0.0f, 0.0f, kWindowWidth, kWindowHeight),
		B_TRANSLATE("ASCII table"), B_TITLED_WINDOW, B_AUTO_UPDATE_SIZE_LIMITS),
	fIsZoomed(false),
	fLastFrame(Frame())
{
	BListView* listView = new BListView("listView", B_MULTIPLE_SELECTION_LIST,
		B_WILL_DRAW);
	BScrollView* listScrollView = new BScrollView("scrollView", listView, 0,
		false, true, B_NO_BORDER);

	listView->SetFont(be_fixed_font);
	listView->AddItem(new BStringItem(B_TRANSLATE("  Dec  Hex  Oct  Code  Description")));
	listView->AddItem(new BStringItem(""));

	for (int i = 0; i < 128; i++) {
		char* row;
		asprintf(&row, "  %3d   %2x  %3o   %3s  %s", i, i, i,
			sAsciiTable[i].code, sAsciiTable[i].description.String());
		listView->AddItem(new BStringItem(row));
		free(row);
	}

	BLayoutBuilder::Group<>(this, B_VERTICAL)
		.Add(listScrollView)
		.SetInsets(-1)
		.End();

	CenterOnScreen();
}


void
AsciiWindow::Zoom(BPoint, float, float)
{
	if (!fIsZoomed) {
		fLastFrame = Frame();
		BDeskbar deskbar;
		BRect deskbarFrame = deskbar.Frame();
		BRect screenFrame = (BScreen(this)).Frame();

		switch (deskbar.Location()) {
			case B_DESKBAR_TOP:
				MoveTo(std::max(fLastFrame.left, screenFrame.left + 4.0f),
					deskbarFrame.bottom + 28.0f);
				ResizeTo(std::max(fLastFrame.Width(), kWindowWidth),
					screenFrame.bottom - deskbarFrame.Height() - 33.0f);
				break;

			case B_DESKBAR_BOTTOM:
				MoveTo(std::max(fLastFrame.left, screenFrame.left + 4.0f),
					26.0f);
				ResizeTo(std::max(fLastFrame.Width(), kWindowWidth),
					deskbarFrame.top - 33.0f);
				break;

			case B_DESKBAR_LEFT_TOP:
			case B_DESKBAR_LEFT_BOTTOM:
				MoveTo(std::max(fLastFrame.left, deskbarFrame.right + 7.0f),
					26.0f);
				ResizeTo(std::max(fLastFrame.Width(), kWindowWidth),
					screenFrame.bottom - 30.0f);
				break;

			case B_DESKBAR_RIGHT_TOP:
			case B_DESKBAR_RIGHT_BOTTOM:
				ResizeTo(std::max(fLastFrame.Width(), kWindowWidth),
					screenFrame.bottom - 30.0f);
				if (fLastFrame.right + 7.0f > deskbarFrame.left) {
					MoveTo(fLastFrame.left - 7.0f
						- (fLastFrame.right - deskbarFrame.left), 26.0f);
				} else {
					MoveTo(std::max(fLastFrame.left, screenFrame.left + 4.0f),
						26.0f);
				}
				break;

			default:
				MoveTo(std::max(fLastFrame.left, screenFrame.left + 4.0f),
					26.0f);
				ResizeTo(std::max(fLastFrame.Width(), kWindowWidth),
					screenFrame.bottom - 30.0f);
				break;
		}
	} else {
		MoveTo(fLastFrame.LeftTop());
		ResizeTo(fLastFrame.Width(), fLastFrame.Height());
	}

	fIsZoomed = !fIsZoomed;
}
