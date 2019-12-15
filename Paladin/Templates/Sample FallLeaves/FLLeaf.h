/*
 * Copyright 2011 David Couzelis. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _FLLEAF_H_
#define _FLLEAF_H_


#include <Bitmap.h>
#include <View.h>


class Leaf
{
public:
					Leaf(BBitmap* bitmap);
					~Leaf() { delete fBitmap; };
	
	void			Update(int32 ticksPerSecond);
	void			Draw(BView* view);
	
	void			SetPos(BPoint pos) { fPos = pos; };
	BPoint			Pos() { return fPos; };
	
	void			SetZ(int32 z) { fZ = z; };
	int32			Z() const { return fZ; };
						// The Z axis controls how far "in"
						// to the screen the leaf is
	
	void			SetSpeed(int32 speed) { fSpeed = speed; };
	
	int32			Width() { return fBitmap->Bounds().IntegerWidth(); };
	int32			Height() { return fBitmap->Bounds().IntegerHeight(); };
	
	bool			IsDead() { return fDead; };
	void			SetBoundary(BRect boundary) { fBoundary = boundary; };
						// A leaf is dead if it moves outside the boundary
	
private:
	BBitmap			*fBitmap;
	
	BPoint			fPos;
						// The position on the screen
	int32			fZ;
	
	int32			fSpeed;
						// In pixels per second
	int32			fFudge;
	
	BRect			fBoundary;
	bool			fDead;
};


#endif
