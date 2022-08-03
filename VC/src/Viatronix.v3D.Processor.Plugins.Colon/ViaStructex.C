// $Id: ViaStructex.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// File: ViaStructex.C
// Description:  implemetation (16/17) of class ViaStrip
// Created: July 31, 2000
// Author:  Freeware
//          rewrited by : Bin Li (binli@viatronix.net)
//
// Complete History at bottom of file.


// appendix: implemetation (16/17) of class ViaStrip

#include "stdafx.h"
#include "ViaStrip.h"

static int4 out1Ex = -1;
static int4 out2Ex = -1;

#define FILE_REVISION "$Revision: 1.2 $"

/** 
 *	Put the edge that is adjacent to face_id into edge1
 *	and edge2. For each edge see if it is adjacent to face_id. 
 */
int4 ViaStrip::GetEdgeEx(int4 *edge1,int4 *edge2,int4 *index,int4 face_id,
	       int4 size, int4 id1, int4 id2)
{
	// Put the edge that is adjacent to face_id into edge1
	// and edge2. For each edge see if it is adjacent to
	// face_id. Id1 and id2 is the input edge, so see if 
	// the orientation is reversed, and save it in reversed.
	int4 x;
	int4 reversed = -1;
	bool set = false;
  
	for (x=0; x< size; x++) 
	{
		if (x == (size-1)) 
		{
			if ((*(index) == id1) && (*(index+size-1)==id2)) 
			{
				if (set) return 1;
				reversed = 1;
			} 
			else if ((*(index) == id2) && (*(index+size-1)==id1)) 
			{
				if (set) return 0;
				reversed = 0;
			}
			if (LookUp(*(index),*(index+size-1),face_id)) 
			{
				if ((out1Ex != -1) &&
					((out1Ex == *(index)) || (out1Ex == *(index+size-1)) ) &&
					((out2Ex == *(index)) || (out2Ex == *(index+size-1)) )) 
				{
					set = true;
					*edge1 = *(index);
					*edge2 = *(index+size-1);
				}
				else if (out1Ex == -1) 
				{
					set = true;
					*edge1 = *(index);
					*edge2 = *(index+size-1);
				}
				if ((reversed != -1) && (set))  
					return reversed;
			}
		} 
		else 
		{
			if ((*(index+x) == id1) && (*(index+x+1)==id2)) 
			{
				if (set) return 0;
				reversed = 0;
			} else if ((*(index+x) == id2) && (*(index+x+1)==id1)) 
			{
				if (set) return 1;
				reversed = 1;
			}	  
			if (LookUp(*(index+x),*(index+x+1),face_id)) 
			{
				if ( (out1Ex != -1) &&
					((out1Ex == *(index+x)) || (out1Ex == *(index+x+1)) ) &&
					((out2Ex == *(index+x)) || (out2Ex == *(index+x+1)))) 
				{
					set = true;
					*edge1 = *(index+x);
					*edge2 = *(index+x+1);
				} 
				else if (out1Ex == -1) 
				{
					set = true;
					*edge1 = *(index+x);
					*edge2 = *(index+x + 1);
				}
				if ((reversed != -1) && (set))
					return reversed;
			}
		} // end-else : (x == (size-1)) 
	}	// end-for
	if ((x == size) && (reversed != -1)) 
	{
		// Could not find the output edge
		int1 ErrorInfo[100];
		sprintf(ErrorInfo, "ViaStrip::Create strips: Error in the Lookup %d %d %d %d %d %d %d %d\n",
			face_id,id1,id2,reversed,*edge1,*edge2,out1Ex,out2Ex);
		throw ex::AbortDataProcessingException(LogRec( "ViaStrip", "GetEdgeEx", "Create strips: Error in the Lookup."));
      
	} // end-if
	return reversed;
} // Get_EdgeEx().

/** 
 *	decremente a face (id)
 */
void ViaStrip::UpdateFaceEx(int4 *next_bucket, int4 *min_face, int4 face_id, 
						  int4 *e1,int4 *e2,int4 temp1,int4 temp2,int4 *ties)
{
	// We have a face id that needs to be decremented.
	// We have to determine where it is in the structure,
	// so that we can decrement it.
  
	// The number of adjacencies may have changed, so to locate
	// it may be a little tricky. However we know that the number
	// of adjacencies is less than or equal to the original number
	// of adjacencies,
	int4 y,size; 
	ListHead *pListHead;
	FFaces * temp = NULL;
	ListInfo * lpListInfo;
	static int4 each_poly = 0;
	bool there = false;
  
	pListHead = m_ppPolFaces[face_id];
	temp = (FFaces *) PeekList( pListHead, LISTHEAD, 0 );
	// Check each edge of the face and tally the number of adjacent polygons to this face. 	      		
	if ( temp != NULL ) 
	{
		// Size of the polygon
		size = temp->nPolSize;
		for (y = 0; y< size; y++) 
		{
			// If we are doing partial triangulation, we must check
			// to see whether the edge is still there in the polygon,
			// since we might have done a portion of the polygon
			// and saved the rest for later.
			if (y != (size-1)) 
			{
				if( ((temp1 == *(temp->pPolygon+y)) && 
					(temp2 ==*(temp->pPolygon+y+1))) ||
					((temp2 == *(temp->pPolygon+y)) &&
					(temp1 ==*(temp->pPolygon+y+1))))
					// edge is still there we are ok
				there = true;
			} 
			else 
			{
				if (((temp1 == *(temp->pPolygon)) && 
					(temp2 == *(temp->pPolygon+size-1))) ||
					((temp2 == *(temp->pPolygon)) &&
					(temp1 ==*(temp->pPolygon+size-1))))
					// edge is still there we are ok
					there = true;
			}
		}
		if (!there)
			// Original edge was already used, we cannot use this polygon
			return;
      
		// We have a starting point to start our search to locate this polygon.
		// Check to see if this polygon was done
		lpListInfo = Done(face_id,&y);
		if (lpListInfo == NULL)
			return;
		// Was not done, but there is an error in the adjacency calculations
		// If more than one edge is adj to it then maybe it was not updated
		if (y == 0)
			return;
      
		// Now put the face in the proper bucket depending on tally.
		// First add it to the new bucket, then remove it from the old
		AddSgiAdj(y-1,face_id);
		RemoveList(m_pArray[y],lpListInfo);
      
		// Save it if it was the smallest seen so far since then
		// it will be the next face 
		// Here we will have different options depending on
		// what we want for resolving ties:
		// 1) First one we see we will use
		// 2) Random resolving
		// 3) Look ahead
		// 4) Alternating direction

		//	At a new strip
		if (*next_bucket == 60)
			*ties = *ties + each_poly;
		//	Have a tie
		if (*next_bucket == (y-1)) 
		{
			AddTies(face_id);
			each_poly++;
		}
		// At a new minimum
		if (*next_bucket > (y-1)) 
		{
			*next_bucket = y-1;
			*min_face = face_id;
			*e1 = temp1;
			*e2 = temp2;
			each_poly = 0;
			ClearTies();
			AddTies(face_id);
		}
	}
}

/**
 *	Find the face that is adjacent to the edge and is not the current face. 
 *	Delete one adjacency from it. Save the min adjacency seen so far.
 */
void ViaStrip::DeleteAdjEx(int4 id1, int4 id2,int4 *next_bucket,int4 *min_face, 
		  int4 current_face,int4 *e1,int4 *e2,int4 *ties)
{
	register int4 count=0;
	FEdges * temp = NULL;
	ListHead *pListHead;
	int4 next_face;
  
	// Always want smaller id first
	SwitchLower(&id1,&id2);
	pListHead = m_ppPolEdges[id1];
	temp = (FEdges *) PeekList(pListHead,LISTHEAD,count);
	if (temp == NULL)
		// It could be a new edge that we created. So we can
		// exit, since there is not a face adjacent to it.
		return;
	
	while (temp->edge[0] != id2) 
	{
		count++;
		temp = (FEdges *)GetNextNode(temp);                                      
		if (temp == NULL)
		// Was a new edge that was created and therefore
		// does not have anything adjacent to it
			return;
	}
	// Was not adjacent to anything else except itself
	if (temp->edge[2] == -1)
		return;
	else 
	{	// Was adjacent to something
		if (temp->edge[2] == current_face)
			next_face =  temp->edge[1];
		else 
			next_face = temp->edge[2];
	}
	// We have the other face adjacent to this edge, it is 
	// next_face. Now we need to decrement this faces' adjacencies.
	UpdateFaceEx(next_bucket, min_face, next_face,e1,e2,id1,id2,ties);
}

/**		
 *	When doing a partial triangulation and we need to
 *	put the new face of triangle into the correct bucket
 */
int4 ViaStrip::ChangeFaceEx(int4 face_id,int4 in1,int4 in2,
		  ListHead *pListHead, bool no_check)
{
	// We are doing a partial triangulation and we need to
	// put the new face of triangle into the correct bucket
	int4 input_adj,y;
	ADJACENCIES * lpListInfo;  
  
	// Find the old number of adjacencies to this face,
	// so we know where to delete it from
	y = Old_Adj(face_id);
	pListHead = m_pArray[y];
	lpListInfo = m_pFaceArray[face_id].pfNode;
  
	if (lpListInfo == NULL) 
	{
		int1 ErrorInfo[100];
		sprintf(ErrorInfo, "ViaStrip::Create strips error: there is an error finding the next polygon3 %d\n",face_id);
		throw ex::AbortDataProcessingException(LogRec("ViaStrip", "ChangeFaceEx", "Create strips error: there is an error finding the next polygon3."));
	}
  
	// Do we need to change the adjacency? Maybe the edge on the triangle
	// that was outputted was not adjacent to anything. We know if we
	// have to check by "check". We came out on the output edge
	// that we needed, then we know that the adjacencies will decrease
	// by exactly one.
	if (!no_check) 
	{
		input_adj = Number_Adj(in1,in2,face_id);
		// If there weren't any then don't do anything
		if (input_adj == 0)
			return y;
	}
  
	if (m_pFaceArray[lpListInfo->face_id].head == pListHead)
		m_pFaceArray[lpListInfo->face_id].pfNode = NULL;

	RemoveList(pListHead,(ListInfo *)lpListInfo);
	
	// RemoveList(pListHead,(PLISTINFO)/ *(temp* /lpListInfo);

	// Before we had a quad with y adjacencies. The in edge
	// did not have an adjacency, since it was just deleted,
	// since we came in on it. The outedge must have an adjacency
	// otherwise we would have a bucket 0, and would not be in this
	// routine. Therefore the new adjacency must be y-1
	AddSgiAdj(y-1,face_id);
	return (y-1);
}

/** 
 *	Give the face with id face_id, decrement all the faces that are adjacent to it.
 */
int4 ViaStrip::UpdateAdjacenciesEx(int4 face_id, int4 *next_bucket, 
								   int4 *e1, int4 *e2, int4 *ties)
{
	// Give the face with id face_id, we want to decrement
	// all the faces that are adjacent to it, since we will
	// be deleting face_id from the data structure.
	// We will return the face that has the least number of adjacencies.
	FFaces * temp = NULL;
	ListHead *pListHead;
	int4 size,y,min_face = -1;

	*next_bucket = 60;
	pListHead = m_ppPolFaces[face_id];
	temp = (FFaces *) PeekList( pListHead, LISTHEAD, 0);
	if ( temp == NULL) 
	{
		throw ex::AbortDataProcessingException(LogRec("ViaStrip", "UpdateAdjacenciesEx", "Create strips: the face was already deleted, there is an error."));
	}
	// Size of the polygon
	size = temp->nPolSize;
	for (y = 0; y< size; y++) 
	{
		if (y != (size-1))
			DeleteAdjEx(*(temp->pPolygon+y),*(temp->pPolygon+y+1),
				next_bucket,&min_face,face_id,e1,e2,ties);
		else
			DeleteAdjEx(*(temp->pPolygon),*(temp->pPolygon+(size-1)),
				next_bucket,&min_face,face_id,e1,e2,ties);
	}
	return (min_face);
}

/**
 *	Find the face that is adjacent to the edge and is not the
 *	current face. Save the min adjacency seen so far.
 */
void ViaStrip::FindAdjTallyEx(int4 id1, int4 id2,int4 *next_bucket,int4 *min_face, 
			     int4 current_face,int4 *ties)
{
	int4 size,each_poly=0,y,count=0;           
	FEdges * temp = NULL;
	FFaces * temp2 = NULL;
	ListHead *pListHead;
	int4 next_face;
	bool there = false;
	
	// Always want smaller id first
	SwitchLower(&id1,&id2);
  
	pListHead = m_ppPolEdges[id1];
	temp = (FEdges *) PeekList(pListHead,LISTHEAD,count);
	if (temp == NULL)
		// This was a new edge that was created, so it is adjacent to nothing.
		return;
	
	while (temp->edge[0] != id2) 
	{
		count++;
		temp = (FEdges *)GetNextNode(temp);                                       
		if (temp == NULL)
			// This was a new edge that we created
			return;
	}
	// Was not adjacent to anything else except itself
	if (temp->edge[2] == -1)
		return;
	else 
	{
		if (temp->edge[2] == current_face)
			next_face =  temp->edge[1];
		else 
			next_face = temp->edge[2];
	}
	// We have the other face adjacent to this edge, it is 
	// next_face. Find how many faces it is adjacent to.
	pListHead = m_ppPolFaces[next_face];
	temp2 = (FFaces *) PeekList( pListHead, LISTHEAD, 0);
	// Check each edge of the face and tally the number of adjacent 
	// polygons to this face. This will be the original number of
	// polygons adjacent to this polygon, we must then see if this
	// number has been decremented      		
	if ( temp2 != NULL ) 
	{
		// Size of the polygon
		size = temp2->nPolSize;
		for (y = 0; y< size; y++) 
		{
			// Make sure that the edge is still in the
			// polygon and was not deleted, because if the edge was
			// deleted, then we used it already.
			if (y != (size-1)) 
			{
				if( ((id1 == *(temp2->pPolygon+y)) &&
					(id2 ==*(temp2->pPolygon+y+1))) ||
					((id2 == *(temp2->pPolygon+y)) &&
					(id1 ==*(temp2->pPolygon+y+1))))
					// edge is still there we are ok
					there = true;
			} 
			else 
			{		
				if( ((id1 == *(temp2->pPolygon)) &&
					(id2 ==*(temp2->pPolygon+size-1))) ||
					((id2 == *(temp2->pPolygon)) &&
					(id1 ==*(temp2->pPolygon+size-1))))
					// edge is still there we are ok
					there = true;
			}
		}
		if (!there)
			// Edge already used and deleted from the polygon
			return;
			
		//	See if the face was already deleted, and where it is if it was not
		if (Done(next_face,&y) == NULL)
			return;
      
		// Save it if it was the smallest seen so far since then
		// it will be the next face 
		// Here we will have different options depending on
		// what we want for resolving ties:
		// 1) First one we see we will use
		// 2) Random resolving
		// 3) Look ahead
		// 4) Alternating direction
      
		// At a new strip
		if (*next_bucket == 60)
			*ties = *ties + each_poly;
		// Have a tie
		if (*next_bucket == (y-1)) 
		{
			AddTies(next_face);
			each_poly++;
		}
		// At a new minimum
		if (*next_bucket > (y-1)) 
		{
			*next_bucket = y-1;
			*min_face = next_face;
			each_poly = 0;
			ClearTies();
			AddTies(next_face);
		}
	} // end-if (temp2 != NULL)
}

/**	
 *	Used for the Partial triangulation to find the next face. 
 *	It will return the minimum adjacency face id found at this face.
 */
int4 ViaStrip::MinFaceAdjEx(int4 face_id, int4 *next_bucket, int4 *ties)
{ 
	FFaces * temp = NULL;
	ListHead *pListHead;
	int4 size,y,min_face,test_face;
  
	*next_bucket = 60;
	pListHead = m_ppPolFaces[face_id];
	temp = (FFaces *) PeekList( pListHead, LISTHEAD, 0);
  
	if ( temp == NULL ) 
	{
		throw ex::AbortDataProcessingException(LogRec("ViaStrip", "MinFaceAdjEx", "Create strips: the face was already deleted, there is an error."));
      
	}
  
	// Size of the polygon
	size = temp->nPolSize;
	for (y = 0; y< size; y++) 
	{
		if (y != (size-1))
			FindAdjTallyEx(*(temp->pPolygon+y),*(temp->pPolygon+y+1),
				next_bucket,&min_face,face_id,ties);
		else
			FindAdjTallyEx(*(temp->pPolygon),*(temp->pPolygon+(size-1)),
				next_bucket,&min_face,face_id,ties);
	}
	// Maybe we can do better by triangulating the face, because
	// by triangulating the face we will go to a polygon of lesser
	// adjacencies
	if (size == 4) 
	{
		// Checking for a quad whether to do the whole polygon will
		// result in better performance because the triangles in the 
		// polygon have less adjacencies
		CheckInQuad(face_id,&test_face);
		if (*next_bucket > test_face)
			// We can do better by going through the polygon
			min_face = face_id;
	}
	// We have a polygon with greater than 4 sides, check to see if going
	// inside is better than going outside the polygon for the output edge.
	else 
	{
		CheckInPolygon(face_id,&test_face,size);
		if (*next_bucket > test_face)
			// We can do better by going through the polygon
			min_face = face_id;
	}
	return (min_face);
}


// $Log: ViaStructex.C,v $
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
//    Rev 2.0   Sep 16 2001 23:39:12   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:25:46   ingmar
// Initial revision.
// Revision 1.19  2001/08/13 20:29:19  sachin
// changed exceptions ,warnings,debug and information
// according to the new logger class
//
// Revision 1.18  2001/07/25 16:08:38  lihong
// Defect Id: 599
// Added throw ex:: after the function name
//
// Revision 1.17  2001/07/24 23:06:50  lihong
// Defect ID: 599
// new format of exceptions
//
// Revision 1.16  2001/07/23 16:56:26  lihong
// Defect ID: 599
// replaced file version with "$Revision: 1.2 $"
//
// Revision 1.15  2001/07/20 23:07:09  lihong
// Defect ID: 599
// changed abort() to throw ex
//
// Revision 1.14  2001/07/03 14:59:48  soeren
// removed stdafx.c and .h
//
// Revision 1.13  2001/05/21 00:22:55  dave
// uses & implement. of Error::DebugMsg, Warn, Abort, etc, gone
//
// Revision 1.12  2001/04/09 17:31:25  dmitry
// updated datatypes
//
// Revision 1.11  2001/03/27 19:49:51  binli
// code walkthrough: name convension
//
// Revision 1.10  2001/03/27 16:55:54  binli
// reflected new code standard.
//
// Revision 1.9  2001/03/23 20:41:20  binli
// code walkthrough
//
// Revision 1.8  2001/03/22 21:15:22  binli
// renamed some variables according to new code standard.
//
// Revision 1.7  2001/03/21 15:24:11  binli
// replaced all structs with classes.
//
// Revision 1.6  2001/02/28 21:01:59  binli
// 1> BOOL->bool
// 2> Error Info
//
// Revision 1.5  2000/12/08 16:56:18  binli
// changed the output of triangle files from text to binary.
// added header to triangle files.
//
// Revision 1.4  2000/11/06 23:57:48  binli
// copied the old version (from mixkit) back to replace 'polygon generation'  in lastest version.
// so as to make the triangle generation modular works.
//
// Revision 1.3  2000/10/21 22:24:25  ingmar
// code cleanup
//
// Revision 1.2  2000/10/21 21:19:42  ingmar
// BOOL -> bool and other code cleanup
//
// Revision 1.1.1.1  2000/08/11 22:15:55  ingmar
// unit tests complete
//
//
// 6     9/07/00 12:11p Binli
// fitted to viatronix standard
//
// 5     8/29/00 1:07p Antonio
// updated footer
//
// *****************  Version 4  *****************
// User: Binli           Date:  8/24/00  Time: 12:30p
// Checked in $/v2k/src/Triangles
// Comment:
//   Added comments to Class ViaStrip
//
// *****************  Version 3  *****************
// User: Antonio         Date:  8/21/00  Time:  6:20p
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
// User: Binli           Date:  7/31/00  Time:  5:29p
// Created ViaStructex.C
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/ViaStructex.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
// $Id: ViaStructex.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
