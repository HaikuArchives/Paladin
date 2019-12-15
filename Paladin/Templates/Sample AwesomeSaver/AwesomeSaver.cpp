//
// An awesome screensaver
//
#include <ScreenSaver.h>
#include <StringView.h>


class AwesomeSaver : public BScreenSaver
{
public:
					AwesomeSaver(BMessage* archive, image_id thisImage);
	void			StartConfig(BView* configView);
	status_t		StartSaver(BView* view, bool preview);
	void			Draw(BView* view, int32 frame);
private:
	// These variables are specific to AwesomeSaver
	int32			fX;
	int32			fY;
	int32			fChangeX;
	int32			fChangeY;
};


AwesomeSaver::AwesomeSaver(BMessage* archive, image_id thisImage)
	:
	BScreenSaver(archive, thisImage), // Call the constructor for BScreenSaver
	fX(200), // Initialize variable
	fY(100),
	fChangeX(1),
	fChangeY(1)
{
	// Empty
}


void
AwesomeSaver::StartConfig(BView* configView)
{
	// Show the name of the screensaver
	BRect rect(15, 15, 20, 20);
	BStringView* stringView = new BStringView(rect, "module", "AwesomeSaver");
	stringView->SetFont(be_bold_font);
	stringView->ResizeToPreferred();
	configView->AddChild(stringView);

	// Show some info about the screensaver
	rect.OffsetBy(0, stringView->Bounds().Height() + 4);
	stringView = new BStringView(rect, "info", "by Me!");
	stringView->ResizeToPreferred();
	configView->AddChild(stringView);
}


status_t
AwesomeSaver::StartSaver(BView* view, bool preview)
{
	if (preview) {
		fX = 20;
		fY = 10;
	}
	
	return B_OK;
}


const char* kText = "Haiku is AWESOME!";


void
AwesomeSaver::Draw(BView* view, int32 frame)
{
	if (frame == 0) {
		// Erase the screen
		view->SetLowColor(0, 0, 0);
		view->FillRect(view->Bounds(), B_SOLID_LOW);
	}
	
	// Erase the old text
	view->SetHighColor(0, 0, 0); // Black
	view->DrawString(kText, BPoint(fX, fY));
	
	// Move the text
	fX += fChangeX;
	fY += fChangeY;
	
	// Bounce off the edge of the screen
	if (fX <= 0 || fX >= view->Bounds().right)
		fChangeX = -fChangeX;
	
	if (fY <= 0 || fY >= view->Bounds().bottom)
		fChangeY = -fChangeY;
	
	// Draw the text at its new location
	view->SetHighColor(249, 210, 42); // Haiku yellow
	view->DrawString(kText, BPoint(fX, fY));
}


extern "C" _EXPORT BScreenSaver*
instantiate_screen_saver(BMessage* msg, image_id id)
{
	return new AwesomeSaver(msg, id);
}
