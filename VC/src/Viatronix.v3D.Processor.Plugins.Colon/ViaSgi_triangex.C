// $Id: ViaSgi_triangex.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ViaSgi_triangex.C  
// Description: Implementation (8/17) of Class ViaStrip 
// Created: July 31, 2000 
// Author: Freeware 
//			rewrited bu by Bin Li (binli@viatronix.net) 
//
// Complete History at bottom of file.


#include "stdafx.h"
#include "ViaStrip.h"

#define FILE_REVISION "$Revision: 1.2 $"

/**
 * Return the vertex that is adjacent to id1,
 * but is not id2, in the list of integers.
 */
int4 ViaStrip::AdjacentEx(int4 id2,int4 id1, int4 *list, int4 size)
{
	register int4 x=0;
  
	while (x < size) 
	{
		if (*(list+x) == id1)
		{
			if ((x != (size -1)) && (x != 0)) 
			{
				if ( *(list+x+1) != id2)
					return *(list+x+1);
				else
					return *(list+x-1);
			} 
			else if (x == (size -1))
			{
				if (*(list) != id2)
					return *(list);
				else
					return *(list+x-1);
			} 
			else 
			{
				if (*(list+size-1) != id2)
					return *(list+size-1);
				else
					return *(list+x+1);
			}
		}
		x++;
	} // endwhile
  throw ex::AbortDataProcessingException(LogRec("ViaStrip", "AdjacentEx", "Error in the list." ));
	exit(0);
}

/**
 *	Delete the occurence of id in the list. (list has size size)
 */
void ViaStrip::Delete_From_ListEx(int4 id,int4 *list, int4 size)
{
	int4 *temp;
	register int4 x,y=0;
  
	temp = new int4 [size];
	for (x=0; x<size; x++) 
	{
		if (*(list+x) != id) 
		{
			*(temp+y) = *(list+x);
			y++;
		}
	}
	if(y != (size-1))
  {
    throw ex::AbortDataProcessingException(LogRec("ViaStrip", "Delete_From_ListEx", "There is an error in the delete." ));
  }
	*(temp+size-1) = -1;
	memcpy(list,temp,sizeof(int4)*size);
}

/**
 *	nonblindly triangulate a quad
 */
void ViaStrip::Triangulate_QuadEx(int4 out_edge1,int4 out_edge2,int4 in_edge1,
																	int4 in_edge2,int4 size,int4 *index, int4 reversed, int4 where, std::ofstream & band)
{
	int4 vertex4,vertex5;
  
	/* This routine will nonblindly triangulate a quad, meaning
	that there is a definite input and a definite output
	edge that we must adhere to. Reversed will tell the orientation
	of the input edge. (Reversed is -1 is we do not have an input
	edge, in other words we are at the beginning of a strip.)
	Out_edge* is the output edge, and in_edge* is the input edge. 
	Index are the edges of the polygon
	and size is the size of the polygon. Begin is whether we are
	at the start of a new strip. */
  
	/*	If we do not have an input edge, then we can make our input
	edge whatever we like, therefore it will be easier to come
	out on the output edge. */
	
	if (reversed == -1) 
	{
		vertex4 = AdjacentEx(out_edge1,out_edge2,index,size);
		vertex5 = GetOtherVertex(vertex4,out_edge1,out_edge2,index);
		OutputTriEx(vertex5,vertex4,out_edge1, -1,(where)?true:false, band);
		OutputTriEx(vertex4,out_edge1,out_edge2, -1,(where)?true:false, band);
		return;
	}
  
	// These are the 5 cases that we can have for the output edge
	// Are they consecutive so that we form a triangle to
	// peel off, but cannot use the whole quad?
  
	if (in_edge2 == out_edge1) 
	{
		// Output the triangle that comes out the correct
		// edge last. First output the triangle that comes out the wrong edge.
		vertex4 = GetOtherVertex(in_edge1,in_edge2,out_edge2,index);
		OutputTriEx(in_edge1,in_edge2,vertex4, -1,(where)?true:false, band);
		OutputTriEx(vertex4,in_edge2,out_edge2, -1,(where)?true:false, band);
		return;  
	}
	// The next case is where it is impossible to come out the
	// edge that we want. So we will have to start a new strip to
	// come out on that edge. We will output the one triangle
	// that we can, and then start the new strip with the triangle
	// that comes out on the edge that we want to come out on.
	else if (in_edge1 == out_edge1) 
	{
		// We want to output the first triangle (whose output
		// edge is not the one that we want.
		// We have to find the vertex that we need, which is
		// the other vertex which we do not have.

		vertex4 = GetOtherVertex(in_edge2,in_edge1,out_edge2,index);
		OutputTriEx(in_edge2,in_edge1,vertex4, -1,(where)?true:false, band);
		OutputTriEx(vertex4,in_edge1,out_edge2, -1,(where)?true:false, band);
		return;
	}
  
	// Consecutive cases again, but with the output edge reversed
	else if (in_edge1 == out_edge2) 
	{
		vertex4 = GetOtherVertex(in_edge1,in_edge2,out_edge1,index);
		OutputTriEx(in_edge2,in_edge1,vertex4, -1,(where)?true:false, band);
		OutputTriEx(vertex4,in_edge1,out_edge1, -1,(where)?true:false, band);
		return;
	} 
	else if (in_edge2 == out_edge2) 
	{
		vertex4 = GetOtherVertex(in_edge1,in_edge2,out_edge1,index);
		OutputTriEx(in_edge1,in_edge2,vertex4, -1,(where)?true:false, band);
		OutputTriEx(vertex4,in_edge2,out_edge1, -1,(where)?true:false, band);
		return;
	} 
	else 
	{ 
		// The final case is where we want to come out the opposite edge.
		if( ((!reversed) &&
			(out_edge1 == (AdjacentEx(in_edge1,in_edge2,index,size)))) ||
			((reversed) &&
			(out_edge2 == (AdjacentEx(in_edge2,in_edge1,index,size))))) 
		{
			// We need to know the orientation of the input
			// edge, so we know which way to put the diagonal.
			// And also the output edge, so that we triangulate correctly.
			OutputTriEx(in_edge1,in_edge2,out_edge2, -1,(where)?true:false, band);
			OutputTriEx(in_edge2,out_edge2,out_edge1, -1,(where)?true:false, band);
		} 
		else 
		{
			// Input and output orientation was reversed, so diagonal will
			// be reversed from above.
			OutputTriEx(in_edge1,in_edge2,out_edge1, -1, (where)?true:false, band);
			OutputTriEx(in_edge2,out_edge1,out_edge2, -1, (where)?true:false, band);
		}
		return;
	} // endelse
}

/**
 *	nonblindly triangulate a polygon, recursively do that
 */
void ViaStrip::Triangulate_PolygonEx(int4 out_edge1,int4 out_edge2,int4 in_edge1,
			   int4 in_edge2,int4 size,int4 *index,
			   int4 reversed,int4 face_id, int4 where,
				 std::ofstream & band)
{
	/*	We have a polygon that we need to nonblindly triangulate.
	We will recursively try to triangulate it, until we are left
	with a polygon of size 4, which can use the quad routine
	from above. We will be taking off a triangle at a time
	and outputting it. We will have 3 cases similar to the
	cases for the quad above. The inputs to this routine
	are the same as for the quad routine. */
  
	int4 vertex4;
	int4 *temp;
  
	// Since we are calling this recursively, we have to check whether
	// we are down to the case of the quad.
	if (size == 4) 
	{
		Triangulate_QuadEx(out_edge1,out_edge2,in_edge1,in_edge2,size,
			 index,reversed,where, band);
		return;
	}
  
	// We do not have a specified input edge, and therefore we
	// can make it anything we like, as long as we still come out 
	// the output edge that we want.

	if (reversed  == -1) 
	{
		// Get the vertex for the last triangle, which is
		// the one coming out the output edge, before we do
		// any deletions to the list. We will be doing this	bottom up.
		vertex4 = AdjacentEx(out_edge1,out_edge2,index,size);
		temp = new int4 [size];
		memcpy(temp,index,sizeof(int4)*size);
		Delete_From_ListEx(out_edge2,index,size);
		Triangulate_PolygonEx(out_edge1,vertex4,in_edge2,
			    vertex4,size-1,index,
			    reversed,face_id,where, band);
		memcpy(index,temp,sizeof(int4)*size);
		// Lastly do the triangle that comes out the output edge.
      
		OutputTriEx(vertex4,out_edge1,out_edge2, -1,(where)?true:false, band);
		return;
	}
  
	// These are the 5 cases that we can have for the output edge
	// Are they consecutive so that we form a triangle to	
	// peel off that comes out the correct output edge, 
	// but we cannot use the whole polygon?
  
	if (in_edge2 == out_edge1) 
	{
		// Output the triangle that comes out the correct
		// edge last. First recursively do the rest of the polygon.
      
		// Do the rest of the polygon without the triangle. 
		// We will be doing a fan triangulation.
      
		// Get the vertex adjacent to in_edge1, but is not in_edge2.
		vertex4 = AdjacentEx(in_edge2,in_edge1,index,size);
		OutputTriEx(in_edge1,in_edge2,vertex4, -1,(where)?true:false, band);
      
		// Create a new edgelist without the triangle that was just outputted.
		temp = new int4 [size];
		memcpy(temp,index,sizeof(int4)*size);
		Delete_From_ListEx(in_edge1,index,size);
		Triangulate_PolygonEx(out_edge1,out_edge2,in_edge2,
			    vertex4,size-1,index,
			    !reversed,face_id,where, band);
		memcpy(index,temp,sizeof(int4)*size);
		return;
	}
  
  	/* Next case is where it is again consecutive, but the triangle
	formed by the consecutive edges do not come out of the
	correct output edge. For this case, we can not do much to
	keep it sequential. Try and do the fan. */
  
	else if (in_edge1 == out_edge1) 
	{
		// Get vertex adjacent to in_edge2, but is not in_edge1
		vertex4 = AdjacentEx(in_edge1,in_edge2,index,size);
		OutputTriEx(in_edge1,in_edge2,vertex4, -1,(where)?true:false, band);
		
		/*	Since that triangle goes out of the polygon (the
		output edge of it), we can make our new input edge
		anything we like, so we will try to make it good for
		the strip. (This will be like starting a new strip,
		all so that we can go out the correct output edge.) */
      
		temp = new int4 [size];
		memcpy(temp,index,sizeof(int4)*size);
		Delete_From_ListEx(in_edge2,index,size);
		Triangulate_PolygonEx(out_edge1,out_edge2,in_edge1,
			    vertex4,size-1,index, reversed,face_id,(where)?true:false, band);
		memcpy(index,temp,sizeof(int4)*size);
		return;
	}
	// 	Consecutive cases again, but with the output edge reversed
	else if (in_edge1 == out_edge2) 
	{
		// Get vertex adjacent to in_edge2, but is not in_edge1
		vertex4 = AdjacentEx(in_edge1,in_edge2,index,size);
		OutputTriEx(in_edge2,in_edge1,vertex4, -1,(where)?true:false, band);
		temp = new int4 [size];
		memcpy(temp,index,sizeof(int4)*size);
		Delete_From_ListEx(in_edge2,index,size);
		Triangulate_PolygonEx(out_edge1,out_edge2,in_edge1,
			    vertex4,size-1,index, reversed,face_id,where, band);
		memcpy(index,temp,sizeof(int4)*size);
		return;
	}
	else if (in_edge2 == out_edge2) 
	{
		// Get vertex adjacent to in_edge2, but is not in_edge1
		vertex4 = AdjacentEx(in_edge2,in_edge1,index,size);
		OutputTriEx(in_edge1,in_edge2,vertex4, -1,(where)?true:false, band);
		temp = new int4 [size];
		memcpy(temp,index,sizeof(int4)*size);
		Delete_From_ListEx(in_edge1,index,size);
		Triangulate_PolygonEx(out_edge1,out_edge2,vertex4,
			    in_edge2,size-1,index, reversed,face_id,where, band);
		memcpy(index,temp,sizeof(int4)*size);
		return;
	}
	/* Else the edge is not consecutive, and it is sufficiently
	far away, for us not to make a conclusion at this time.
	So we can take off a triangle and recursively call this function. */
	else 
	{
		vertex4 = AdjacentEx(in_edge2,in_edge1,index,size);
		OutputTriEx(in_edge1,in_edge2,vertex4, -1,(where)?true:false, band);
		temp = new int4 [size];
		memcpy(temp,index,sizeof(int4)*size);
		Delete_From_ListEx(in_edge1,index,size);
		Triangulate_PolygonEx(out_edge1,out_edge2,in_edge2,
			    vertex4,size-1,index, !reversed,face_id,(where)?true:false, band);
		memcpy(index,temp,sizeof(int4)*size);
		return;
	}
}

/**
 *	triangulate a polygon
 */
void ViaStrip::TriangulateEx(int4 out_edge1,int4 out_edge2,int4 in_edge1,
					int4 in_edge2,int4 size,int4 *index,
					int4 reversed,int4 face_id, int4 where,
					std::ofstream & band)
{
	// We have the info we need to triangulate a polygon
	if (size == 4)
		Triangulate_QuadEx(out_edge1,out_edge2,in_edge1,in_edge2,size,
		       index,reversed,where, band);
	else
		Triangulate_PolygonEx(out_edge1,out_edge2,in_edge1,in_edge2,size,
			  index, reversed, face_id, where, band);
}

/**
 *	When triangulate a polygon but cannot do it blindly, try to come out on 
 *  the edge that has the least number of adjacencies
 */
void ViaStrip::NonBlindTriangulateEx(int4 size,int4 *index, 
			     int4 next_face_id,int4 face_id,int4 where,
					 std::ofstream & band)
{
	int4 id1,id2,id3;
	int4 nedge1,nedge2;
	int4 reversed;
	// We have a polygon that has to be triangulated and we cannot
	// do it blindly, ie we will try to come out on the edge that
	// has the least number of adjacencies
  
	LastEdge(&id1,&id2,&id3,0);
	// Find the edge that is adjacent to the new face ,
	// also return whether the orientation is reversed in the
	// face of the input edge, which is id2 and id3.
	if (next_face_id == -1) 
	{
		int1 ErrorInfo[100];
		sprintf(ErrorInfo, "ViaStrip::The face is -1 and the size is %d\n",size);
		throw ex::AbortDataProcessingException(LogRec("ViaStrip", "NonBlindTriangulateEx", "The face is -1."));
      
	}
	reversed = GetEdgeEx(&nedge1,&nedge2,index,next_face_id,size,id2,id3);
	// Do the triangulation
  
	// If reversed is -1, the input edge is not in the polygon, 
	// therefore we can have the input edge to be anything we like,
	// since we are at the beginning of a strip
	TriangulateEx(nedge1,nedge2,id2,id3,size,index, reversed,face_id, where, band);
}

/**
 *	When in the middle of a strip, someone must find the edge to start on, 
 *	which is the last edge that had benn transmitted.
 */
void ViaStrip::Rearrange_IndexEx(int4 *index, int4 size)
{
	// If we are in the middle of a strip we must find the
	// edge to start on, which is the last edge that we had transmitted.
	int4 x,f,y,e1,e2,e3;
	int4 increment = 1;
	int4 *temp;
  
	// Find where the input edge is in the input list
	LastEdge(&e1,&e2,&e3,0);
	for (y = 0; y < size; y++) 
	{
		if (*(index+y) == e2) 
		{
			if ((y != (size - 1)) && (*(index+y+1) == e3))
				break;
			else if ((y == (size - 1)) && (*(index) == e3))
				break;
			else if ((y != 0) && (*(index+y-1) == e3)) 
			{
				increment = -1;
				break;
			} 
			else if ((y==0) && (*(index+size-1) == e3)) 
			{
				increment = -1;
				break;
			}
		}
		if (*(index+y) == e3) 
		{
			if ((y != (size - 1)) && (*(index+y+1) == e2))
				break;
			else if ((y == (size - 1)) && (*(index) == e2))
				break;
			else if ((y != 0) && (*(index+y-1) == e2)) 
			{
				increment = -1;
				break;
			} 
			else if ((y==0) && (*(index+size-1) == e2)) 
			{
				increment = -1;
				break;
			}
		}
		// Edge is not here, we are at the beginning
		if ((y == (size-1)) && (increment != -1)) 
			return;
	}
  
	// Now put the list into a new list, starting with the
	// input edge. Increment tells us whether we have to go forward or backward.
	// Was in good position already
	if ((y == 0) && (increment == 1)) 
		return;
  
	temp = new int4 [size];
	memcpy(temp,index,sizeof(int4)*size);
	if (increment == 1) 
	{
		x=0;
		for (f = y ; f< size; f++) 
		{
			*(index+x) = *(temp+f);
			x++;
		}
		// Finish the rest of the list
		for(f = 0; f < y ; f++) 
		{
			*(index+x) = *(temp+f);
			x++;
		}
	} 
	else 
	{
		x=0;
		for (f = y ; f >= 0; f--) 
		{
			*(index+x) = *(temp+f);
			x++;
		}
		// Finish the rest of the list
		for(f = (size - 1); f > y ; f--) 
		{
			*(index+x) = *(temp+f);
			x++;
		}
	}
}

/**
 *	save sides in temp array, we need it so we know about swaps.
 */
void ViaStrip::BlindTriangulateEx(int4 size, int4 *index,
                                  bool begin, int4 where, std::ofstream & band)
{
  // save sides in temp array, we need it so we know about swaps.
  int4 mode, decreasing,increasing,e1,e2,e3;
  
  //	Rearrange the index list so that the input edge is first
  if (!begin)
    Rearrange_IndexEx(index,size);
  
  // We are given a polygon of more than 3 sides and want to 
  // triangulate it. We will output the triangles to the output file.
  // Find where the input edge is in the input list
  LastEdge(&e1,&e2,&e3,0);
  if (((!begin) && (*(index) == e2) ) || (begin)) 
  {
    OutputTriEx(*(index+0),*(index+1),*(index+size-1), -1,(where)?true:false, band);
    // If we have a quad, (chances are yes), then we know that
    // we can just add one diagonal and be done. 
    // (divide the quad into 2 triangles
    if (size == 4) 
    {
      OutputTriEx(*(index+1),*(index+size-1),*(index+2), -1,(where)?true:false, band);
      return;
    }
		increasing = 1;
		mode = 1;
	}
	else if (!begin) 
	{
		OutputTriEx(*(index+1),*(index+0),*(index+size-1), -1,(where)?true:false, band);
		if (size == 4) 
		{
			OutputTriEx(*(index+0),*(index+size-1),*(index+2), -1,(where)?true:false, band);
			return;
		}
		OutputTriEx(*(index+0),*(index+size-1),*(index+2), -1,(where)?true:false, band);
		increasing = 2;
		mode = 0;
	}
	if (size != 4) 
	{
		// We do not have a quad, we have something bigger.
		decreasing = size - 1;
		do 
		{
			// Will be alternating diagonals, so we will be increasing and 
			// decreasing around the polygon
			if (mode) 
			{
				OutputTriEx(*(index+increasing),*(index+decreasing),
						*(index+increasing+1), -1,(where)?true:false, band);
				increasing++;
			} 
			else 
			{
				OutputTriEx(*(index+decreasing),*(index+increasing),
						*(index+decreasing-1), -1,(where)?true:false, band);
				decreasing--;
			}
			mode = !mode;
		} while ((decreasing - increasing) >= 2);
	}
}


// $Log: ViaSgi_triangex.C,v $
// Revision 1.2  2005/07/01 12:46:05  vxconfig
// Added new Logger.
//
// Revision 1.1  2005/04/14 12:50:05  geconomos
// initial revision
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
//    Rev 2.0   Sep 16 2001 23:39:10   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:25:42   ingmar
// Initial revision.
// Revision 1.21  2001/08/13 20:29:19  sachin
// changed exceptions ,warnings,debug and information
// according to the new logger class
//
// Revision 1.20  2001/07/25 16:08:38  lihong
// Defect Id: 599
// Added throw ex:: after the function name
//
// Revision 1.19  2001/07/24 23:06:50  lihong
// Defect ID: 599
// new format of exceptions
//
// Revision 1.18  2001/07/23 16:56:27  lihong
// Defect ID: 599
// replaced file version with "$Revision: 1.2 $"
//
// Revision 1.17  2001/07/20 23:07:09  lihong
// Defect ID: 599
// changed abort() to throw ex
//
// Revision 1.16  2001/07/03 14:59:48  soeren
// removed stdafx.c and .h
//
// Revision 1.15  2001/05/21 00:22:54  dave
// uses & implement. of Error::DebugMsg, Warn, Abort, etc, gone
//
// Revision 1.14  2001/04/09 17:31:25  dmitry
// updated datatypes
//
// Revision 1.13  2001/04/02 19:18:21  binli
// code simplified!
//
// Revision 1.12  2001/03/27 16:55:54  binli
// reflected new code standard.
//
// Revision 1.11  2001/03/23 20:41:20  binli
// code walkthrough
//
// Revision 1.10  2001/03/22 21:15:22  binli
// renamed some variables according to new code standard.
//
// Revision 1.9  2001/03/07 19:57:21  binli
// malloc/free replaced by new/delete
//
// Revision 1.8  2001/03/01 17:49:52  binli
// changed 'int' to 'bool' in Output_Tri() and Output_TriEx().
//
// Revision 1.7  2001/02/28 21:01:59  binli
// 1> BOOL->bool
// 2> Error Info
//
// Revision 1.6  2000/12/08 21:16:41  binli
// removed code to write out 'text' triangle files.
//
// Revision 1.5  2000/12/08 16:56:18  binli
// changed the output of triangle files from text to binary.
// added header to triangle files.
//
// Revision 1.4  2000/11/06 23:57:41  binli
// copied the old version (from mixkit) back to replace 'polygon generation'  in lastest version.
// so as to make the triangle generation modular works.
//
// Revision 1.3  2000/11/02 04:54:47  antonio
// changed int where -> bool end for functions in ViaStrip.h
//
// Revision 1.2  2000/10/21 21:19:41  ingmar
// BOOL -> bool and other code cleanup
//
// Revision 1.1.1.1  2000/08/11 22:15:54  ingmar
// unit tests complete
//
// 
// 6     9/07/00 12:11p Binli
// fitted to viatronix standard
// 
// 5     8/29/00 1:14p Antonio
// updated footer
// 
// *****************  Version 4  *****************
// User: Binli           Date:  8/24/00  Time: 12:30p
// Checked in $/v2k/src/Triangles
// Comment:
//   Added comments to Class ViaStrip
// 
// *****************  Version 3  *****************
// User: Antonio         Date:  8/21/00  Time:  6:14p
// Checked in $/v2k/src/Triangles
// Comment:
//   added header and footer
// 
// *****************  Version 2  *****************
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
// *****************  Version 1  *****************
// User: Binli           Date:  7/31/00  Time:  5:28p
// Created ViaSgi_triangex.C
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/ViaSgi_triangex.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
// $Id: ViaSgi_triangex.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
