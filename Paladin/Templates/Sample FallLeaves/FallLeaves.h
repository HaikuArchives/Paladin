/*
 * Copyright 2011 David Couzelis. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _FALLLEAVES_H_
#define _FALLLEAVES_H_


#include <ObjectList.h>
#include <ScreenSaver.h>


// The number of leaves on the screen
const int32 kMaxAmount = 50;
const int32 kMinAmount = 10;
const int32 kDefaultAmount = 35;

// The speed of the leaves
const int32 kMaxSpeed = 10;
const int32 kMinSpeed = 1;
const int32 kDefaultSpeed = 5;


typedef class Leaf;


class FallLeaves : public BScreenSaver
{
public:
							FallLeaves(BMessage* archive, image_id thisImage);
							~FallLeaves();
	
	void					StartConfig(BView* configView);
	status_t				StartSaver(BView* view, bool preview);
	status_t				SaveState(BMessage* into) const;
	
	void					Draw(BView* view, int32 frame);
	
	void					SetAmount(int32 amount);
	void					SetSpeed(int32 speed);
private:
	Leaf*					_CreateLeaf(BView* view, bool above);
	BBitmap*				_RandomBitmap(int32 size);
	
	BObjectList<Leaf>*		fLeaves;
	
	int32					fSize;
								// The size of the biggest possible leaf
	
	int32					fAmount;
								// The amount of leaves on the screen
	int32					fSpeed;
								// The speed of the fastest leaf
	
	BBitmap*				fBackBitmap;
	BView*					fBackView;
								// For double buffering,
								// used to reduce flicker
	
	bool					fZUsed[101];
								// Used to give each leaf a unique Z depth
};


#endif
