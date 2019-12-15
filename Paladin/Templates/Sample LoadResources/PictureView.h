#ifndef PICTUREVIEW_H
#define PICTUREVIEW_H

#include <Message.h>
#include <Bitmap.h>
#include <String.h>
#include <View.h>

class PictureView : public BView
{
public:
					PictureView(void);
					~PictureView(void);
							
	void			Draw(BRect rect);
	void			MouseUp(BPoint pt);
	
private:
	BBitmap			*fBitmaps[5];
	int8			fBitmapIndex;
};

#endif
