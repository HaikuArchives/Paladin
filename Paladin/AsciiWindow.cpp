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

#include <LayoutBuilder.h>
#include <ListView.h>
#include <Screen.h>
#include <ScrollView.h>
#include <String.h>

#include "ObjectList.h"
#include "PLocale.h"


static const float kWindowWidth = 400.0f;
static const float kWindowHeight = 600.0f;


typedef struct {
	int value;
	char code[4];
	BString description;
} ascii_info;


static ascii_info sAsciiTable[128] = {
	{	0,		"NUL",	"NULL"						},
	{	1,		"SOH",	"Start of heading"			},
	{	2,		"STX",	"Start of text"				},
	{	3,		"ETX",	"End of text"				},
	{	4,		"EOT",	"End of transmission"		},
	{	5,		"ENQ",	"Enquiry"					},
	{	6,		"ACK",	"Acknowledge"				},
	{	7,		"BEL",	"Bell"						},
	{	8,		"BS",	"Backspace"					},
	{	9,		"TAB",	"Horizontal tab"			},
	{	10,		"LF",	"New line"					},
	{	11,		"VT",	"Vertical tab"				},
	{	12,		"FF",	"Form feed"					},
	{	13,		"CR",	"Carriage return"			},
	{	14,		"SO",	"Shift out"					},
	{	15,		"SI",	"Shift in"					},
	{	16,		"DLE",	"Data link escape"			},
	{	17,		"DC1",	"Device Control 1"			},
	{	18,		"DC2",	"Device Control 2"			},
	{	19,		"DC3",	"Device Control 3"			},
	{	20,		"DC4",	"Device Control 4"			},
	{	21,		"NAK",	"Negative acknowledge"		},
	{	22,		"SYN",	"Synchronous idle"			},
	{	23,		"ETB",	"End of transmission block"	},
	{	24,		"CAN",	"Cancel"					},
	{	25,		"EM",	"End of medium"				},
	{	26,		"SUB",	"Substitute"				},
	{	27,		"ESC",	"Escape"					},
	{	28,		"FS",	"File separator"			},
	{	29,		"GS",	"Group separator"			},
	{	30,		"RS",	"Record separator"			},
	{	31,		"US",	"Unit separator"			},
	{	32,		" ",	"Space"						},
	{	33,		"!",	""							},
	{	34,		"\"",	""							},
	{	35,		"#",	""							},
	{	36,		"$",	""							},
	{	37,		"%",	""							},
	{	38,		"&",	""							},
	{	39,		"'",	""							},
	{	40,		"(",	""							},
	{	41,		")",	""							},
	{	42,		"*",	""							},
	{	43,		"+",	""							},
	{	44,		",",	""							},
	{	45,		"-",	""							},
	{	46,		".",	""							},
	{	47,		"/",	""							},
	{	48,		"0",	""							},
	{	49,		"1",	""							},
	{	50,		"2",	""							},
	{	51,		"3",	""							},
	{	52,		"4",	""							},
	{	53,		"5",	""							},
	{	54,		"6",	""							},
	{	55,		"7",	""							},
	{	56,		"8",	""							},
	{	57,		"9",	""							},
	{	58,		":",	""							},
	{	59,		";",	""							},
	{	60,		"<",	""							},
	{	61,		"=",	""							},
	{	62,		">",	""							},
	{	63,		"?",	""							},
	{	64,		"@",	""							},
	{	65,		"A",	""							},
	{	66,		"B",	""							},
	{	67,		"C",	""							},
	{	68,		"D",	""							},
	{	69,		"E",	""							},
	{	70,		"F",	""							},
	{	71,		"G",	""							},
	{	72,		"H",	""							},
	{	73,		"I",	""							},
	{	74,		"J",	""							},
	{	75,		"K",	""							},
	{	76,		"L",	""							},
	{	77,		"M",	""							},
	{	78,		"N",	""							},
	{	79,		"O",	""							},
	{	80,		"P",	""							},
	{	81,		"Q",	""							},
	{	82,		"R",	""							},
	{	83,		"S",	""							},
	{	84,		"T",	""							},
	{	85,		"U",	""							},
	{	86,		"V",	""							},
	{	87,		"W",	""							},
	{	88,		"X",	""							},
	{	89,		"Y",	""							},
	{	90,		"Z",	""							},
	{	91,		"[",	""							},
	{	92,		"\\",	""							},
	{	93,		"]",	""							},
	{	94,		"^",	""							},
	{	95,		"_",	""							},
	{	96,		"`",	""							},
	{	97,		"a",	""							},
	{	98,		"b",	""							},
	{	99,		"c",	""							},
	{	100,	"d",	""							},
	{	101,	"e",	""							},
	{	102,	"f",	""							},
	{	102,	"g",	""							},
	{	104,	"h",	""							},
	{	105,	"i",	""							},
	{	106,	"j",	""							},
	{	107,	"k",	""							},
	{	108,	"l",	""							},
	{	109,	"m",	""							},
	{	110,	"n",	""							},
	{	111,	"o",	""							},
	{	112,	"p",	""							},
	{	113,	"q",	""							},
	{	114,	"r",	""							},
	{	115,	"s",	""							},
	{	116,	"t",	""							},
	{	117,	"u",	""							},
	{	118,	"v",	""							},
	{	119,	"w",	""							},
	{	120,	"x",	""							},
	{	121,	"y",	""							},
	{	122,	"z",	""							},
	{	123,	"{",	""							},
	{	124,	"|",	""							},
	{	125,	"}",	""							},
	{	126,	"~",	""							},
	{	127,	"DEL",	"Delete"					}
};


AsciiWindow::AsciiWindow(void)
	:
	BWindow(BRect(0, 0, kWindowWidth, kWindowHeight), TR("ASCII table"),
		B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS | B_AUTO_UPDATE_SIZE_LIMITS),
	fIsZoomed(false),
	fLastFrame(Frame())
{
	BListView* listView = new BListView("listView", B_SINGLE_SELECTION_LIST,
		B_WILL_DRAW);
	BScrollView* scrollView = new BScrollView("scrollView", listView, 0, false,
		true, B_NO_BORDER);

	listView->SetFont(be_fixed_font);
	listView->AddItem(new BStringItem("  Dec  Hex  Oct  Code  Description"));
	listView->AddItem(new BStringItem(""));

	for (int i = 0; i < 128; i++) {
		char* row;
		asprintf(&row, "  %3d   %2x  %3o   %3s  %s", i, i, i,
			sAsciiTable[i].code, sAsciiTable[i].description.String());
		listView->AddItem(new BStringItem(row));
		free(row);
	}

	BLayoutBuilder::Group<>(this, B_VERTICAL)
		.Add(scrollView)
		.SetInsets(-1)
		.End();

	CenterOnScreen();
}


void
AsciiWindow::Zoom(BPoint origin, float width, float height)
{
	if (!fIsZoomed) {
		fLastFrame = Frame();
		BRect screenFrame = (BScreen(this)).Frame();
		MoveTo(std::max(fLastFrame.left, screenFrame.left + 4.0f), 26.0f);
		ResizeTo(std::max(fLastFrame.Width(), kWindowWidth),
			screenFrame.bottom - 30.0f);
	} else {
		MoveTo(fLastFrame.LeftTop());
		ResizeTo(fLastFrame.Width(), fLastFrame.Height());
	}

	fIsZoomed = !fIsZoomed;
}
