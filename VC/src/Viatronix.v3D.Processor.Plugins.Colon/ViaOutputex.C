// $Id: ViaOutputex.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ViaOutputex.c 
// Description:  Implementation (14/17) of Class ViaStrip 
// Created: July 31, 2000
// Author:  Freeware 
//			    rewriten by Bin Li (binli@viatronix.net)
//
// Complete History at bottom of file.

#include "stdafx.h"
#include "ViaStrip.h"


#define FILE_REVISION "$Revision: 1.2 $"

/**
 * save everything into a list, rather than output at once,
 */
void ViaStrip::OutputTriEx(int4 id1, int4 id2, int4 id3, int4 flag, bool where,
														std::ofstream & band)
{
	// We will save everything into a list, rather than output at once,
	// as was done in the old routine. This way for future modifications
	// we can change the strips later on if we want to.
  
	int4 swap,temp1,temp2,temp3;
	static int4 total=0;
	static int4 tri=0;
	static int4 strips = 0;
	static int4 cost = 0;
  
	if (flag == -20) 
	{
		cost = cost + where+total+tri+strips+strips;
		total = 0;
		tri = 0;
		strips = 0;
		return ;
	}
	if (flag == -10) 
	{
		// We are finished, now is time to output the triangle list
		// 'band' write:
		band << "\nt";
		tri = tri + Finished(&swap, 1, band);
		total = total + swap;
		strips++;
	} 
	else 
	{
		LastEdge(&temp1,&temp2,&temp3,0);
		AddIdStrips(id1, where);
		AddIdStrips(id2, where);
		AddIdStrips(id3, where);
		LastEdge(&id1,&id2,&id3,1);
	} // end-if
} // Output_TriEx().


/**
 * We just made a strip, now we are going to see if we can extend
 * backwards from the starting face, which had 2 or more adjacencies
 * to start with.
 */
void ViaStrip::ExtendBackwardsEx(int4 face_id, int4 *ties, 
			int4 tie, int4 triangulate, int4 swaps,int4 *next_id,
			std::ofstream & band)
{
	// We just made a strip, now we are going to see if we can extend
	// backwards from the starting face, which had 2 or more adjacencies
	// to start with.
	int4 bucket,next_face,num,x,y,z,c,max,f;              
	ListHead *pListFace;
	FFaces * face;
	ADJACENCIES * temp;
  
	// Get the first triangle that we have saved the the strip data 
    // structure, so we can see if there are any polygons adjacent
    // to this edge or a neighboring one
	FirstEdge(&x,&y,&z); 
	pListFace  = m_ppPolFaces[face_id];
	face = (FFaces *) PeekList(pListFace,LISTHEAD,0);
	num = face->nPolSize;
  
	// Go through the edges to see if there is an adjacency
	// with a vertex in common to the first triangle that was
    // outputted in the strip. (maybe edge was deleted....)
	for (c=0; c<num ; c++) 
	{ 
		if ( (c != (num-1)) && 
			(( (*(face->pPolygon+c) == x) && (*(face->pPolygon+c+1) == y)) ||
			(*(face->pPolygon+c) == y) && (*(face->pPolygon+c+1) == x))) 
		{
			// Input edge is still there see if there is an adjacency
			next_face = FindFace(face_id, x, y, &bucket);
			if (next_face == -1)
				// Could not find a face adjacent to the edge
				break;
			pListFace = m_pArray[bucket];
			max = NumOnList(pListFace);
			for (f=0;;f++) 
			{
				temp = (ADJACENCIES *) PeekList(pListFace,LISTHEAD,f);	
				if (temp->face_id == next_face) 
				{
					LastEdge(&z,&y,&x,1);
					PolygonOutputEx(temp,temp->face_id,bucket,pListFace,
						ties,tie,triangulate,swaps,next_id,0, band);
					return;
				}
				if (temp == NULL) 
        {
          int1 ErrorInfo[100];
          sprintf(ErrorInfo, "ViaStrip::Error in the new buckets%d %d %d\n",bucket,max,0);
          throw ex::AbortDataProcessingException(LogRec( "ViaStrip", "ExtendBackwardsEx", "ViaStrip::Error in the new buckets."));
        }
			} // endfor(f)
		} // endif (c! ... )
		else if ( (c == (num -1)) &&
				(((*(face->pPolygon) == x) && (*(face->pPolygon+num-1) == y)) ||
				(*(face->pPolygon) == y) && (*(face->pPolygon+num-1) == x))) 
		{
				next_face = FindFace(face_id,x,y,&bucket);
				if (next_face == -1)
					// Could not find a face adjacent to the edge
					break;
				pListFace = m_pArray[bucket];
				max = NumOnList(pListFace);
				for (f=0;;f++) 
				{
					temp = (ADJACENCIES *) PeekList(pListFace,LISTHEAD,f);
					if (temp->face_id == next_face) 
					{
						LastEdge(&z,&y,&x,1);
						PolygonOutputEx(temp,temp->face_id,bucket,pListFace,
								ties,tie,triangulate,swaps,next_id,0, band);
						return;
					}
					if (temp == NULL) 
          {
            int1 ErrorInfo[100];
            sprintf(ErrorInfo, "ViaStrip::Error in the new buckets%d %d %d\n",bucket,max,0);
            throw ex::AbortDataProcessingException(LogRec("ViaStrip", "ExtendBackwardsEx" , "ViaStrip::Error in the new buckets."));
          }
				}
		} // end-else if
	} // end-for (c-loop)
}

/**
 * output a polygon, the id is face id, the number of adjacent polygons to it is bucket.
 */
void ViaStrip::PolygonOutputEx(ADJACENCIES *temp, int4 face_id, int4 bucket,
		      ListHead *pListHead, int4 *ties, int4 tie, 
		      int4 triangulate, int4 swaps, int4 *next_id, bool where,
					std::ofstream & band)
{
	ListHead *pListFace;
	FFaces * face;
	static bool begin = true;
	int4 old_face,next_face_id,next_bucket,e1,e2,e3,other1,other2,other3;
	ADJACENCIES * lpListInfo; 
  
	// We have a polygon to output, the id is face id, and the number
	// of adjacent polygons to it is bucket.
	LastEdge(&e1,&e2,&e3,0);
  
	// Get the polygon with id face_id
	pListFace  = m_ppPolFaces[face_id];
	face = (FFaces *) PeekList(pListFace,LISTHEAD,0);
  
	if (face->nPolSize == 3) 
	{
		// It is already a triangle
		if (bucket == 0) {
			// It is not adjacent to anything so we do not have to
			// worry about the order of the sides or updating adjacencies
			LastEdge(&e1,&e2,&e3,0);
			next_face_id = Different(*(face->pPolygon),*(face->pPolygon+1),
					*(face->pPolygon+2), e1,e2,e3,&other1,&other2);
			// No input edge, at the start
			if ((e2 ==0) && (e3 == 0)) {
				e2 = other1;
				e3 = other2;
			}
			OutputTriEx(e2,e3,next_face_id, begin,where, band);
			if (m_pFaceArray[temp->face_id].head == pListHead)
				m_pFaceArray[temp->face_id].pfNode = NULL;
			RemoveList(pListHead,(ListInfo *) temp);
			// We will be at the beginning of the next strip.
			begin = true;
		}
		// It is a triangle with adjacencies. This means that we have to:
		// 1. Update the adjacencies in the list, because we are
		//		using this polygon and it will be deleted.
		// 2. Get the next polygon.
		else 
		{
			// Return the face_id of the next polygon we will be using,
			// while updating the adjacency list by decrementing the
			// adjacencies of everything adjacent to the current triangle.
			next_face_id = UpdateAdjacenciesEx(face_id,&next_bucket, &e1,&e2,ties);
			old_face = next_face_id;
	  
			// Break the tie,  if there was one
			if (tie != _FIRST)
				old_face = Get_Next_Face(tie,face_id,triangulate);
			if (next_face_id == -1) 
			{
				PolygonOutputEx(temp,face_id,0,pListHead, ties,tie, 
					triangulate,swaps,next_id,where, band);
				return;
			}
			// We are using a different face
			if ((tie != _FIRST) && (old_face != next_face_id) && (swaps == ON))
			{
				next_face_id = old_face;
				// Get the new output edge, since e1 and e2 are for the
				// original next face that we got.
				e3 = GetEdgeEx(&e1,&e2,face->pPolygon,next_face_id,face->nPolSize,0,0);
			}
			// Find the other vertex to transmit in the triangle
			e3 = ReturnOther(face->pPolygon,e1,e2);
			LastEdge(&other1,&other2,&other3,0);
			if ((other1 != 0) && (other2 != 0)) 
			{
				// See which vertex in the output edge is not in the input edge
				if ((e1 != other2) && (e1 != other3))
					e3 = e1;
				else if ((e2 != other2) && (e2 != other3))
					e3 = e2;
			
				// can happen with > 2 polys on an edge but won't 
				// form a good strip so stop the strip here
				else 
				{
					PolygonOutputEx(temp,face_id,0,pListHead,
						ties,tie,triangulate,swaps,next_id,where, band);
					return;
				}
				// See which vertex of the input edge is not in the output edge
				if ((other2 != e1) && (other2 != e2)) {
					other1 = other2;
					other2 = other3;
				}
				else if ((other3 != e1) && (other3 != e2))
					other1 = other3;
				else 
				{
					// Degenerate triangle just return
					OutputTriEx(other1,other2,e3, begin,where, band);
					if (m_pFaceArray[temp->face_id].head == pListHead)
						m_pFaceArray[temp->face_id].pfNode = NULL;
					RemoveList(pListHead,(ListInfo *) temp);
					begin = false;
					return;
				}   
			} // endif ((other1 != 0) && (other2 != 0))
			else 
			{	// There was not an input edge, we are the first triangle in a strip
					// Find the correct order to transmit the triangle, what is
					// the output edge that we want ?
				other1 = e3;
				e3 = e2;
				other2 = e1;
			}
	  
			// At this point the adjacencies have been updated  and we
			// have the next polygon id 
	  
			OutputTriEx(other1,other2,e3, begin,where, band);
			if (m_pFaceArray[temp->face_id].head == pListHead)
				m_pFaceArray[temp->face_id].pfNode = NULL;
			RemoveList(pListHead,(ListInfo *) temp);
			begin = false;
	  
			if (Done(next_face_id,&next_bucket) == NULL)
				return;
	  
			pListHead = m_pArray[next_bucket];
			lpListInfo = m_pFaceArray[next_face_id].pfNode;
			if (lpListInfo == NULL) 
			{
        std::cout << "ViaStrip:: There is an error finding the next polygon3 ";   
        std::cout << "at ViaOutputex() in the PolygonOutputEx.C";   
        abort();
				
        /* int1 ErrorInfo[100];
				sprintf(ErrorInfo, "ViaStrip::There is an error finding the next polygon3 %d\n", next_face_id);       
        throw ex::AbortDataProcessingException(LogRecord(
          __FILE__, "$Revision: 1.2 $", __LINE__,"ViaStrip", "PolygonOutputEx",
          "ViaStrip::There is an error finding the next polygon3.")); */
			}
			PolygonOutputEx(lpListInfo,next_face_id,next_bucket,
					pListHead, ties,tie, triangulate,swaps,next_id,where, band);
		}
	} // end-if (face->nPolSize == 3) 
	else 
	{
		// It is not a triangle, we have to triangulate it.
	    // Since it is not adjacent to anything we can triangulate it blindly
		if (bucket == 0) {
			// Check to see if there is not an input edge
			LastEdge(&other1,&other2,&other3,0);
			if ((other1 == 0) && (other2 ==0))
				BlindTriangulateEx(face->nPolSize,face->pPolygon, true, where, band);
			else
				BlindTriangulateEx(face->nPolSize,face->pPolygon, false, where, band);
	  
			if (m_pFaceArray[temp->face_id].head == pListHead)        
				m_pFaceArray[temp->face_id].pfNode = NULL;
			RemoveList(pListHead,(ListInfo *) temp);
			// We will be at the beginning of the next strip.
			begin = true;
		}
		// If we have specified PARTIAL triangulation then
		// we will go to special routines that will break the
		// polygon and update the data structure. Else everything
		// below will simply triangulate the whole polygon 
		else if (triangulate == PARTIAL) 
		{
			// Return the face_id of the next polygon we will be using,
			next_face_id = MinFaceAdjEx(face_id,&next_bucket,ties);
	  
			// Don't do it partially, because we can go inside and get
			// less adjacencies, for a quad we can do the whole thing.
			if ((face_id == next_face_id) && (face->nPolSize == 4) && (swaps == ON))
			{
				next_face_id = UpdateAdjacenciesEx(face_id, &next_bucket,&e1,&e2,ties);
				if (next_face_id == -1) 
				{
					// There is no sequential face to go to, end the strip
					PolygonOutputEx(temp,face_id,0,pListHead, 
						ties,tie,triangulate,swaps,next_id,where, band);
					return;
				}
				// Break the tie,  if there was one
				if (tie != _FIRST)
					next_face_id = Get_Next_Face(tie,face_id,triangulate);
				NonBlindTriangulateEx(face->nPolSize,face->pPolygon,
						next_face_id,face_id,where, band);
	      
				if (m_pFaceArray[temp->face_id].head == pListHead)
					m_pFaceArray[temp->face_id].pfNode = NULL;
				RemoveList(pListHead,(ListInfo *) temp);
			}
			// Was not a quad but we still do not want to do it partially for
			// now, since we want to only do one triangle at a time
			else if ((face_id == next_face_id) && (swaps == ON))
				Inside_Polygon(face->nPolSize,face->pPolygon,face_id,pListHead,where, band);
			else 
			{
				if ((tie != _FIRST) && (swaps == ON))
					next_face_id = Get_Next_Face(tie,face_id,triangulate);
				Partial_Triangulate(face->nPolSize,face->pPolygon,
						next_face_id,face_id,next_id,pListHead,temp,where, band);
				// Check the next bucket again ,maybe it changed 
				// We calculated one less, but that might not be the case
			}
			if (Done(next_face_id,&next_bucket) == NULL) {
				// Check to see if there is not an input edge
				LastEdge(&other1,&other2,&other3,0);
				if ((other1 == 0) && (other2 ==0))
					BlindTriangulateEx(face->nPolSize,face->pPolygon, true, where, band);
				else
					BlindTriangulateEx(face->nPolSize,face->pPolygon, false, where, band);
	      
				if (Done(face_id,&bucket) != NULL) 
				{
					pListHead = m_pArray[bucket];
					lpListInfo = m_pFaceArray[face_id].pfNode;
					if (m_pFaceArray[temp->face_id].head == pListHead)
						m_pFaceArray[lpListInfo->face_id].pfNode = NULL;
					RemoveList(pListHead,(ListInfo *)lpListInfo);
				}
				begin = true;
				return;
			}
			begin = false;
			pListHead = m_pArray[next_bucket];
			lpListInfo = m_pFaceArray[next_face_id].pfNode;
			if (lpListInfo == NULL) 
      {
        std::cout << "ViaStrip:: There is an error finding the next polygon3 ";   
        std::cout << "at ViaOutputex() in the PolygonOutputEx.C";   
        abort();
				
        /* int1 ErrorInfo[100];
        sprintf(ErrorInfo, "There is an error finding the next polygon1 %d %d\n",next_face_id,next_bucket);
        throw ex::AbortDataProcessingException(LogRecord(
          __FILE__, "$Revision: 1.2 $", __LINE__,"ViaStrip", "PolygonOutputEx",
          "There is an error finding the next polygon1.")); */
      }
			PolygonOutputEx(lpListInfo,next_face_id,next_bucket,
					pListHead, ties,tie,triangulate,swaps,next_id,where, band);
      
		} // end-else if (triangulate == PARTIAL)
		else 
		{
			// WHOLE triangulation
			// It is not a triangle and has adjacencies. 
			// This means that we have to:
			// 1. TriangulateEx this polygon, not blindly because
			// we have an edge that we want to come out on, that
			// is the edge that is adjacent to a polygon with the
			// least number of adjacencies. Also we must come in
			// on the last seen edge.
			// 2. Update the adjacencies in the list, because we are using this polygon .
			// 3. Get the next polygon.
			// Return the face_id of the next polygon we will be using,
			// while updating the adjacency list by decrementing the
			// adjacencies of everything adjacent to the current polygon.
          
			next_face_id = UpdateAdjacenciesEx(face_id,&next_bucket, &e1,&e2,ties);
          
			if (Done(next_face_id,&next_bucket) == NULL) 
			{
				PolygonOutputEx(temp,face_id,0,pListHead, ties,tie, 
					triangulate,swaps,next_id,where, band);
				// Because maybe there was more than 2 polygons on the edge
				return;
			}
			// Break the tie,  if there was one
			else if (tie != _FIRST)
				next_face_id = Get_Next_Face(tie,face_id,triangulate);
          
			NonBlindTriangulateEx(face->nPolSize,face->pPolygon, 
				  next_face_id, face_id, where, band);
	  
			if (m_pFaceArray[temp->face_id].head == pListHead)
				m_pFaceArray[temp->face_id].pfNode = NULL;
			RemoveList(pListHead,(ListInfo *) temp);
			begin = false;
			pListHead = m_pArray[next_bucket];
			lpListInfo = m_pFaceArray[next_face_id].pfNode;
			if (lpListInfo == NULL) 
			{
        std::cout << "ViaStrip:: There is an error finding the next polygon3 ";   
        std::cout << "at ViaOutputex() in the PolygonOutputEx.C";   
        abort();
				
				/* int1 ErrorInfo[100];
				sprintf(ErrorInfo, "ViaStrip::There is an error finding the next polygon2 %d %d\n", next_face_id,next_bucket);
				throw ex::AbortDataProcessingException(LogRecord(
          __FILE__, "$Revision: 1.2 $", __LINE__,"ViaStrip", "PolygonOutputEx",
          "ViaStrip::There is an error finding the next polygon2.")); */
			}
			PolygonOutputEx(lpListInfo,next_face_id,next_bucket,
			   pListHead, ties,tie, triangulate,swaps,next_id,where, band);
		} // end-elseif-else
	} // end-else (face->nPolSize == 3) 
	LastEdge(&e1,&e2,&e3,0);
}

#undef FILE_REVISION


// $Log: ViaOutputex.C,v $
// Revision 1.2  2005/07/01 12:46:05  vxconfig
// Added new Logger.
//
// Revision 1.1  2005/04/14 12:50:05  geconomos
// initial revision
//
// Revision 3.2  2003/01/08 17:08:10  dongqing
// Fix stack overflow bug in the function PolygonOutputEx().
// The vxException throw was replaced by abort().
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
//    Rev 2.0   Sep 16 2001 23:39:06   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:25:40   ingmar
// Initial revision.
// Revision 1.23  2001/08/13 20:29:19  sachin
// changed exceptions ,warnings,debug and information
// according to the new logger class
//
// Revision 1.22  2001/07/25 16:08:38  lihong
// Defect Id: 599
// Added throw ex:: after the function name
//
// Revision 1.21  2001/07/24 23:06:51  lihong
// Defect ID: 599
// new format of exceptions
//
// Revision 1.20  2001/07/23 16:56:27  lihong
// Defect ID: 599
// replaced file version with "$Revision: 1.2 $"
//
// Revision 1.19  2001/07/20 23:07:10  lihong
// Defect ID: 599
// changed abort() to throw ex
//
// Revision 1.18  2001/05/21 00:22:54  dave
// uses & implement. of Error::DebugMsg, Warn, Abort, etc, gone
//
// Revision 1.17  2001/04/09 17:31:26  dmitry
// updated datatypes
//
// Revision 1.16  2001/04/02 19:18:20  binli
// code simplified!
//
// Revision 1.15  2001/03/27 19:49:51  binli
// code walkthrough: name convension
//
// Revision 1.14  2001/03/27 16:55:54  binli
// reflected new code standard.
//
// Revision 1.13  2001/03/23 20:41:20  binli
// code walkthrough
//
// Revision 1.12  2001/03/22 21:15:21  binli
// renamed some variables according to new code standard.
//
// Revision 1.11  2001/03/21 15:24:11  binli
// replaced all structs with classes.
//
// Revision 1.10  2001/03/01 17:49:52  binli
// changed 'int' to 'bool' in Output_Tri() and Output_TriEx().
//
// Revision 1.9  2001/02/28 21:01:59  binli
// 1> BOOL->bool
// 2> Error Info
//
// Revision 1.8  2000/12/08 21:16:41  binli
// removed code to write out 'text' triangle files.
//
// Revision 1.7  2000/12/08 16:56:18  binli
// changed the output of triangle files from text to binary.
// added header to triangle files.
//
// Revision 1.6  2000/11/07 18:37:32  sarang
// Removed _errors_ which gcc found.
// 1. BOOL was never defined
// 2. function definition and call mismatch (bool and int were not distinguished by VC++!!!)
//
// Revision 1.5  2000/11/06 23:57:33  binli
// copied the old version (from mixkit) back to replace 'polygon generation'  in lastest version.
// so as to make the triangle generation modular works.
//
// Revision 1.4  2000/11/02 07:35:27  antonio
// added boolean variable costPositive to ViaStrip::DoStripe,and made g into bool.
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
// 8     9/07/00 12:11p Binli
// fitted to viatronix standard
// 
// 7     8/29/00 1:34p Antonio
// updated footer
// 
// *****************  Version 6  *****************
// User: Binli           Date:  8/24/00  Time: 12:30p
// Checked in $/v2k/src/Triangles
// Comment:
//   Added comments to Class ViaStrip
// 
// *****************  Version 5  *****************
// User: Antonio         Date:  8/21/00  Time:  6:09p
// Checked in $/v2k/src/Triangles
// Comment:
//   added header and footer
// 
// *****************  Version 4  *****************
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
// *****************  Version 3  *****************
// User: Binli           Date:  8/01/00  Time: 10:21a
// Checked in $/v2k/src/Triangles
// 
// *****************  Version 2  *****************
// User: Binli           Date:  7/31/00  Time:  9:55p
// Checked in $/v2k/src/Triangles
// Comment:
//   Muiti-Seg preprocessing
// 
// *****************  Version 1  *****************
// User: Binli           Date:  7/31/00  Time:  5:28p
// Created ViaOutputex.C
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/ViaOutputex.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
// $Id: ViaOutputex.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
