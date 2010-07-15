/*	$Id: CDiff.cpp,v 1.2 2009/12/31 14:48:41 darkwyrm Exp $
	
	Copyright 1996, 1997, 1998, 2002
	        Hekkelman Programmatuur B.V.  All rights reserved.
	
	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:
	1. Redistributions of source code must retain the above copyright notice,
	   this list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright notice,
	   this list of conditions and the following disclaimer in the documentation
	   and/or other materials provided with the distribution.
	3. All advertising materials mentioning features or use of this software
	   must display the following acknowledgement:
	   
	    This product includes software developed by Hekkelman Programmatuur B.V.
	
	4. The name of Hekkelman Programmatuur B.V. may not be used to endorse or
	   promote products derived from this software without specific prior
	   written permission.
	
	THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
	FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
	AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
	PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
	OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
	WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 	

	Created: 03/29/98 15:23:27
	
	The algorithm used in this module comes from:
	
	"An O(ND) Difference Algorithm and Its Variations" by Eugene W. Myers
	published in Algorithmica (1986) 1: 251-266
	
*/

#include "pe.h"
#include "CDiff.h"
#include "CDiffWindow.h"
#include "HError.h"

CDiff::CDiff(vector<int>& vx, vector<int>& vy)
{
	fVX = fVY = fD = NULL;

	fN = vx.size();
	fM = vy.size();
	
	fVX = (int *)malloc(fN * sizeof(int));
	FailNil(fVX);
	copy(vx.begin(), vx.end(), fVX);
	
	fVY = (int *)malloc(fM * sizeof(int));
	FailNil(fVY);
	copy(vy.begin(), vy.end(), fVY);

	int diags = fN + fM + 3;
	fD = (int *)malloc(diags * (2 * sizeof(int)));
	FailNil(fD);
	
	fFD = fD + fM + 1;
	fBD = fD + diags + fM + 1;

	fCX.insert(fCX.begin(), fN + 1, false);
	fCY.insert(fCY.begin(), fM + 1, false);
	
	Seq(0, 0, fN, fM);
} /* CDiff::CDiff */

CDiff::~CDiff()
{
	if (fVX) free(fVX);
	if (fVY) free(fVY);
	if (fD) free(fD);
} /* CDiff::~CDiff */

void CDiff::Report(BListView *lst)
{
	int ix = 0, iy = 0;
	
	while (ix <= fN || iy <= fM)
	{
		if (fCX[ix] || fCY[iy])
		{
			int lx = ix, ly = iy;
			
			while (fCX[ix] && ix < fN) ix++;
			while (fCY[iy] && iy < fM) iy++;
			
			lst->AddItem(new CDiffItem(lx, ix, ly, iy));
		}
		
		ix++, iy++;
	}
} /* CDiff::Report */

int CDiff::MiddleSnake(int x, int y, int u, int v, int& px, int& py)
{
	int dmin = x - v;
	int dmax = u - y;
	int fmid = x - y;
	int bmid = u - v;
	int fmin = fmid, fmax = fmid;
	int bmin = bmid, bmax = bmid;
	bool odd = (fmid - bmid) & 1;
	int c;

	fFD[fmid] = x;
	fBD[bmid] = u;
	
	for (c = 1;; ++c)
	{
		int d;
		
		fmin > dmin ? fFD[--fmin - 1] = -1 : ++fmin;
		fmax < dmax ? fFD[++fmax + 1] = -1 : --fmax;
		for (d = fmax; d >= fmin; d -= 2)
		{
			int nx, ny;
			
			if (fFD[d - 1] >= fFD[d + 1])
				nx = fFD[d - 1] + 1;
			else
				nx = fFD[d + 1];
			
			ny = nx - d;
			
			while (nx < u && ny < v && fVX[nx] == fVY[ny])
				++nx, ++ny;
			
			fFD[d] = nx;
			
			if (odd && bmin <= d && d <= bmax && fBD[d] <= nx)
			{
				px = nx;
				py = ny;
				return 2 * c - 1;
			}
		}
		
		bmin > dmin ? fBD[--bmin - 1] = INT_MAX : ++bmin;
		bmax < dmax ? fBD[++bmax + 1] = INT_MAX : --bmax;
		for (d = bmax; d >= bmin; d -= 2)
		{
			int nx, ny;
			
			if (fBD[d - 1] < fBD[d + 1])
				nx = fBD[d - 1];
			else
				nx = fBD[d + 1] - 1;
			
			ny = nx - d;
			
			while (nx > x && ny > y && fVX[nx - 1] == fVY[ny - 1])
				--nx, --ny;
			
			fBD[d] = nx;
			
			if (! odd && fmin <= d && d <= fmax && nx <= fFD[d])
			{
				px = nx;
				py = ny;
				return 2 * c;
			}
		}
	}
} /* CDiff::MiddleSnake */

void CDiff::Seq(int x, int y, int u, int v)
{
	while (x < u && y < v && fVX[x] == fVY[y])
		x++, y++;
	while (u > x && v > y && fVX[u - 1] == fVY[v - 1])
		u--, v--;
	
	if (x == u)
		while (y < v)
			fCY[y++] = true;
	else if (y == v)
		while (x < u)
			fCX[x++] = true;
	else
	{
		int px, py;
		
		int c = MiddleSnake(x, y, u, v, px, py);
		
		if (c == 1)	// should never happen
			abort();
		else
		{
			Seq(x, y, px, py);
			Seq(px, py, u, v);
		}
	}
} /* CDiff::Seq */
