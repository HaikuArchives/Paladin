#ifndef __FRAME_VIEW__
#define __FRAME_VIEW__

#ifndef _VIEW_H
#include <View.h>
#endif

class FrameView : public BView
{

public:
				FrameView(BRect frame, const char *name,
						  ulong resizeMask, int bevel_indent=1);

				FrameView(BMessage *data);
virtual status_t Archive(BMessage *msg, bool deep) const;
static BArchivable *Instantiate(BMessage *archive);

virtual	void	Draw(BRect updateRect);
virtual	void	FrameResized(float new_width, float new_height);
virtual void	AttachedToWindow();
		void	MouseDown(BPoint where);

void 			ColoringBasis(rgb_color view_color);


				typedef struct
				{
					char	*label;
					ulong	message;
				} ClusterInfo;

				FrameView(const BPoint &where, const char *name,
						  uint32 resizeMask,
						  int items_wide, int items_high, int bevel, 
						  int key_border, int key_height, int key_width,
						  const ClusterInfo button_info[]);

private:
	int			bevel_indent;
};

#endif
