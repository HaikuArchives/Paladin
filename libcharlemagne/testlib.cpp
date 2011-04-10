#include "PObject.h"
#include <Window.h>

#include <TextView.h>

int32_t
AppSetup(void *obj, PArgList *inList, PArgList *outList)
{
	PObject *win = MakeObject("PWindow");
	win->SetRectProperty("Frame", BRect(100,100,500,400));
	win->SetIntProperty("Flags", B_ASYNCHRONOUS_CONTROLS | B_QUIT_ON_WINDOW_CLOSE);
	
	PObject *top = MakeObject("PView");
	top->SetColorProperty("BackColor", ui_color(B_PANEL_BACKGROUND_COLOR));
	top->SetRectProperty("Frame", BRect(0,0,400,300));
	
	PArgs args, foo;
	args.AddInt64("id", top->GetID());
	win->RunMethod("AddChild", args.ListRef(), foo.ListRef());
	
	PObject *tview = MakeObject("PButton");
	args.MakeEmpty();
	args.AddInt64("id", tview->GetID());
	top->RunMethod("AddChild", args.ListRef(), foo.ListRef());
	
	tview->SetRectProperty("Frame", BRect(10,10,110,110));
	tview->SetStringProperty("Label", "MyButton");
//	tview->SetRectProperty("TextRect", BRect(5,5,95,95));
	
	return B_OK;
}


int
main(void)
{
	run_app("application/x-vnd.libcharlemagneTestApp", AppSetup);
	
	ShutdownObjectSystem();
	return 0;
}
