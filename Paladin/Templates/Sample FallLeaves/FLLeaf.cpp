/*
 * Copyright 2011 David Couzelis. All rights reserved.
 * Distributed under the terms of the MIT License.
 */


#include "FLLeaf.h"


Leaf::Leaf(BBitmap* bitmap)
	:
	fBitmap(bitmap),
	fPos(BPoint()),
	fZ(0),
	fSpeed(0),
	fFudge(0),
	fBoundary(BRect()),
	fDead(false)
{
	// Empty
}


void
Leaf::Update(int32 ticksPerSecond)
{
	if (fDead)
		return;
	
	fFudge += fSpeed;
	
	while (fFudge >= ticksPerSecond) {
		fPos.y++;
		fFudge -= ticksPerSecond;
	}
	
	// If the leaf is out of boundary...
	if (fPos.x < fBoundary.left || fPos.x > fBoundary.right
			|| fPos.y < fBoundary.top || fPos.y > fBoundary.bottom) {
		fDead = true; // ...then it's dead
	}
}


void
Leaf::Draw(BView* view)
{
	if (fDead)
		return;
	
	view->DrawBitmap(fBitmap, fPos);
}
