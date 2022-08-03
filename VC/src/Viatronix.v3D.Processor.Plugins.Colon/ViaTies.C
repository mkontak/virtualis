// $Id: ViaTies.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ViaTies.C  
// Description: Implemenataion (17b/17) of class ViaStrip  
// Created: July 31, 2000 
// Author:	FreeWare
//			rewrited by Bin Li (binli@viatronix.net)
//
// Complete History at bottom of file.


// Implementation (17b/17) of Class ViaStrip
#include "stdafx.h"
#include "ViaStrip.h"

#define FILE_REVISION "$Revision: 1.2 $"

using namespace std ;
static int4 ties_array[60];
static int4 last = 0;

/**
 *	Clear the buffer, because we do not have the tie any more that we had before
 */
void ViaStrip::ClearTies()
{
	last = 0;
}

/**
 *	have a tie to add to the buffer
 */
void ViaStrip::AddTies(int4 id)
{
	ties_array[last++] = id;
}

/**	
 *	Alternate in what we choose to break the tie 
 *	We are just alternating between the first and second thing that we found
 */
int4 ViaStrip::AlternateTie()
{
	static int4 x = 0;
	register int4 t;
  
	t = ties_array[x];
	x++;
	if (x == 2)
		x = 0;
	return t;
}

/**
 *	Randomly choose the next face with which to break the tie
 */
int4 ViaStrip::Random_Tie()
{
	register int4 num;
  
	num = rand();
	while (num >= last)
		num = num/20;
	return (ties_array[num]);
}

/**	
 *	Look ahead at this face and save the minimum adjacency 
 *	of all the faces that are adjacent to this face.
 */
int4 ViaStrip::Look_Ahead(int4 id)
{
	return Min_Adj(id);
}

/**	
 *	had a tie within a tie in the lookahead, break it randomly 
 */
int4 ViaStrip::Random_Look(int4 id[],int4 count)
{
	register int4 num;
  
	num = rand();
	while (num >= count)
		num = num/20;
	return (id[num]);
}

/**
 *	Look ahead and find the face to go to that will give the least number of adjacencies
 */
int4 ViaStrip::Look_Ahead_Tie()
{
	int4 id[60],t,x,f=0,min = 60;
  
	for (x = 0; x < last; x++) 
	{
		t = Look_Ahead(ties_array[x]);
			// We have a tie
		if (t == min)
		id[f++] = ties_array[x];
		if (t < min) 
		{
			f = 0;
			min = t;
			id[f++] = ties_array[x];
		}
	} // end-for
	// No tie within the tie
	if ( f == 1)
	    return id[0];
	
	//	Or ties, but we are at the end of strips
	if (min == 0)
		return id[0];
	
	return (Random_Look(id,f));
} // Look_Ahead_Tie().

/**
 *	have a triangle and need to break the ties at it. 
 *	choose the edge that is sequential. 
 */
int4 ViaStrip::Sequential_Tri(int4 *index)
{
	// We have a triangle and need to break the ties at it.
	// We will choose the edge that is sequential. There
	// is definitely one since we know we have a triangle
	// and that there is a tie and there are only 2 edges for the tie.
  
	int4 reversed,e1,e2,e3,output1,output2,output3,output4;
  
	// e2 and e3 are the input edge to the triangle
	LastEdge(&e1,&e2,&e3,0);
  
	if ((e2 == 0) && (e3 == 0))
		// Starting the strip, don't need to do this
		return ties_array[0];
  
	// For the 2 ties find the edge adjacent to face id
	reversed = GetEdgeEx(&output1,&output2,index,ties_array[0],3,0,0);
	reversed = GetEdgeEx(&output3,&output4,index,ties_array[1],3,0,0);
  
	if ((output1 == e3) || (output2 == e3))
		return ties_array[0];
	if ((output3 == e3) || (output4 == e3))
		return ties_array[1];

	return NULL;
} // Sequential_Tri().

/**
 * have a quad that need to break its ties, 
 * try and choose a side that is sequential, otherwise use lookahead
 */
int4 ViaStrip::Sequential_Quad(int4 *index, int4 triangulate)
{
	// We have a quad that need to break its ties, we will try
	// and choose a side that is sequential, otherwise use lookahead
	int4 reversed,output1,output2,x,e1,e2,e3; 
  
	// e2 and e3 are the input edge to the quad
	LastEdge(&e1,&e2,&e3,0);
  
	// No input edge
	if ((e2 == 0) && (e3 == 0))
		return ties_array[0];
  
	// Go through the ties and see if there is a sequential one
	for (x = 0; x < last; x++) 
	{
		reversed = GetEdgeEx(&output1,&output2,index,ties_array[x],4,0,0);
		// Partial and whole triangulation will have different requirements
		if (((output1 == e3) || (output2 == e3)) && (triangulate == PARTIAL))
			return ties_array[x];
		if (((output1 != e3) && (output1 != e2) &&
			(output2 != e3) && (output2 != e2)))
			return ties_array[x];
	} // end-for.
	// There was not a tie that was sequential
	return Look_Ahead_Tie();
} // Sequential_Quad().

/**
 *	Used to sequentially break ties in the whole triangulation for polygons greater than 4 sides. 
 */
void ViaStrip::Whole_Output(int4 in1, int4 *index, int4 size, int4 *out1, int4 *out2)
{
	// Used to sequentially break ties in the whole triangulation for polygons
	// greater than 4 sides. We will find the output edge that is good
	// for sequential triangulation.
	int4 half;
  
	// Put the input edge first in the list
	Rearrange_IndexEx(index,size);
  
	if (!(EVEN(size))) 
	{
		if (*(index) == in1)
			half = size/2 ;
		else
			half = size/2 +1;
	}
	else
	{
		half = size/2;
	}
  
	*out1 = *(index+half);
	*out2 = *(index+half+1);
} // Whole_Output().

/**	
 *	We have a polygon of greater than 4 sides and wish to break the
 *	tie in the most sequential manner.
 */
int4 ViaStrip::Sequential_Poly(int4 size, int4 *index, int4 triangulate)
{
	// We have a polygon of greater than 4 sides and wish to break the
	// tie in the most sequential manner.
	int4 x,reversed,output1,output2,e1,e2,e3,saved1=-1,saved2=-1,output3,output4;
  
	// e2 and e3 are the input edge to the quad
	LastEdge(&e1,&e2,&e3,0);
  
	// If we are using whole, find the output edge that is sequential
	if (triangulate == WHOLE)
		Whole_Output(e2,index,size,&output3,&output4);
  
	// No input edge
	if ((e2 == 0) && (e3 == 0))
		return ties_array[0];
  
	for (x = 0; x < last ; x++) 
	{
		reversed = GetEdgeEx(&output1,&output2,index,ties_array[x],size,0,0);
		// Partial that can be removed in just one triangle
		if (((output1 == e3) || (output2 == e3)) && (triangulate == PARTIAL))
			saved1 = ties_array[x];
		// Partial removed in more than one triangle
		if ((output1 != e3) && (output1 != e2) &&
			(output2 != e3) && (output2 != e2) &&
			(triangulate == PARTIAL) && (saved2 != -1))
				saved2 = ties_array[x];
		// Whole is not so easy, since the whole polygon must be done. Given
		// an input edge there is only one way to come out, approximately half
		// way around the polygon.
      
		if (((output1 == output3) && (output2 == output4)) ||
			((output1 == output4) && (output2 == output3)) &&
			(triangulate == WHOLE))
				return ties_array[x];
	} // end-for.
	if (saved1 != -1)
		return saved1;
	if (saved2 != -1)
		return saved2;
  
	// There was not a tie that was sequential
	return Look_Ahead_Tie();
} // Sequential_Poly().

/**
 *	Break the tie by choosing the face that will not give us a swap 
 *	and is sequential. If there s not one, then do the lookahead to break the tie.
 */
int4 ViaStrip::Sequential_Tie(int4 face_id,int4 triangulate)
{
	// Break the tie by choosing the face that will
	// not give us a swap and is sequential. If there
	// is not one, then do the lookahead to break the tie.
  
	// Separate into 3 cases for simplicity, if the current
	// polygon has 3 sides, 4 sides or if the sides were 
	// greater. We can do the smaller cases faster, so that
	// is why I separated the cases.
  
	ListHead *pListFace;
	FFaces * face;
  
	// Get the polygon with id face_id
	pListFace  = m_ppPolFaces[face_id];
	face = (FFaces *) PeekList(pListFace,LISTHEAD,0);
  
	if (face->nPolSize == 3)
		return(Sequential_Tri(face->pPolygon));
	if (face->nPolSize == 4)
		return(Sequential_Quad(face->pPolygon,triangulate));
	else
		return(Sequential_Poly(face->nPolSize,face->pPolygon,triangulate));
} // Sequential_Tie().

/**
 *	Get the next face depending on what	the user specified
 */
int4 ViaStrip::Get_Next_Face(int4 t, int4 face_id, int4 triangulate)
{  
	
  stringstream tt;
  // Did not have a tie, don't do anything
	if (last == 1)
		return(ties_array[0]);
	if (t == _RANDOM)
		return Random_Tie();
	if (t == _ALTERNATE)
		return AlternateTie();
	if (t == _LOOK)
		return Look_Ahead_Tie();
	if (t == _SEQUENTIAL)
		return Sequential_Tie(face_id,triangulate);
  
  tt << "ViaStrip::Illegal option specified for ties, using first." << std::endl;
  LogWrn(tt.str(),"ViaStrip", "Get_Next_Face");
  tt.str("");
	return (ties_array[0]);
} 


// $Log: ViaTies.C,v $
// Revision 1.2  2005/07/01 12:46:05  vxconfig
// Added new Logger.
//
// Revision 1.1  2005/04/14 12:50:05  geconomos
// initial revision
//
// Revision 3.2  2002/08/05 20:42:32  dongqing
// rename the Logger to vxLogger due to the re-name of Logger Class
//
// Revision 3.1  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0  2001/10/14 23:01:50  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:45  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:39:12   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:25:46   ingmar
// Initial revision.
// Revision 1.11  2001/08/13 20:29:19  sachin
// changed exceptions ,warnings,debug and information
// according to the new logger class
//
// Revision 1.10  2001/08/07 17:04:40  sachin
// Changed wrn,dbg,fyi,tmp and cout to logger
//
// Revision 1.9  2001/05/21 00:22:55  dave
// uses & implement. of Error::DebugMsg, Warn, Abort, etc, gone
//
// Revision 1.8  2001/04/09 17:31:25  dmitry
// updated datatypes
//
// Revision 1.7  2001/03/27 19:49:51  binli
// code walkthrough: name convension
//
// Revision 1.6  2001/03/27 16:55:54  binli
// reflected new code standard.
//
// Revision 1.5  2001/03/23 20:41:20  binli
// code walkthrough
//
// Revision 1.4  2001/03/21 15:24:11  binli
// replaced all structs with classes.
//
// Revision 1.3  2001/02/28 21:01:59  binli
// 1> BOOL->bool
// 2> Error Info
//
// Revision 1.2  2000/12/11 19:41:58  binli
// code cleaned
//
// Revision 1.1.1.1  2000/08/11 22:15:55  ingmar
// unit tests complete
//
// 
// 7     9/07/00 12:11p Binli
// fitted to viatronix standard
// 
// 6     8/29/00 1:04p Antonio
// updated footer
// 
// *****************  Version 5  *****************
// User: Binli           Date:  8/24/00  Time: 12:30p
// Checked in $/v2k/src/Triangles
// Comment:
//   Added comments to Class ViaStrip
// 
// *****************  Version 4  *****************
// User: Antonio         Date:  8/21/00  Time:  6:21p
// Checked in $/v2k/src/Triangles
// Comment:
//   added header and footer
// 
// *****************  Version 3  *****************
// User: Sarang          Date:  8/20/00  Time:  6:49p
// Checked in $/v2k/src/Triangles
// Comment:
//   The whole pipeline now works under linux+gcc. Removed tons of bugs
// and syntax errors found by gcc.
// 
// **********************
// Label: last_void_*_volume_version
// User: Jay             Date:  8/11/00  Time:  1:19a
// Labeled 'last_void_*_volume_version'
// Label comment:
//   last void * volume version
// about to replace with template volume
// 
// *****************  Version 2  *****************
// User: Binli           Date:  7/31/00  Time:  9:55p
// Checked in $/v2k/src/Triangles
// Comment:
//   Muiti-Seg preprocessing
// 
// *****************  Version 1  *****************
// User: Binli           Date:  7/31/00  Time:  5:29p
// Created ViaTies.C
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/ViaTies.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
// $Id: ViaTies.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
