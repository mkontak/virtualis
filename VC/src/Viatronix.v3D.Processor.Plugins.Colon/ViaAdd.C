// $Id: ViaAdd.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Created: July 31, 2000 
// Author: Freeware,
//		     rewriten by Bin Li (binli@viatronix.net)
//
// Complete History at bottom of file.

#include "stdafx.h"
#include "ViaStrip.h"

#define FILE_REVISION "$Revision: 1.2 $"


bool ViaStrip::NewVertex(float8 difference, int4 id1,int4 id2, Triple <float8> *n)
{
	Triple <float8> *pn1,*pn2;
	float8 dot_product, distance1, distance2, distance, rad;
	int1 arg1[100], arg2[100];
  
	pn1 = n + id1;
	pn2 = n + id2;
  
	dot_product = ((pn1->m_x)*(pn2->m_x)) + ((pn1->m_y)*(pn2->m_y)) + ((pn1->m_z)*(pn2->m_z));
	// Get the absolute value
	if (dot_product < 0)
		dot_product = dot_product * -1;
  
	distance1 = sqrt((pn1->m_x * pn1->m_x) + (pn1->m_y * pn1->m_y) + (pn1->m_z * pn1->m_z));
	distance2 = sqrt((pn2->m_x * pn2->m_x) + (pn2->m_y * pn2->m_y) + (pn2->m_z * pn2->m_z));
	distance = distance1 * distance2;

	rad = acos((float8)dot_product/(float8)distance);
	// convert to degrees
	rad = (180 * rad)/PI;

	if (rad <= difference)
		return false;
  
	// float8 checking because of imprecision with floating point acos function
	sprintf(arg1,"%.5f", rad );
	sprintf(arg2,"%.5f", difference );
	if (strcmp(arg1, arg2) <=0)
		return (false);
	if (rad <= difference)
		return false;
	else 
		return true;
}

/**
 *	Check to see if we have already added this vertex and normal
 */
bool ViaStrip::CheckVN(int4 vertex,int4 normal, vert_added *added)
{
	register int4 x, n;
  
	n = (added+vertex)->num;
	for (x = 0; x < n; x++) 
	{
		if (*((added+vertex)->normal+x) == normal)
			return true;
	}
	return false;
}

// Fill the pointer with the id of the normal
bool ViaStrip::NormArray(int4 id, int4 vertex, float8 normal_difference,
						  Triple <float8> *n, int4 num_vert) 
{
	static int4 last;
	static vert_added *added;
	register int4 x;
	static bool first = true;
  
	if (first) 
	{
		// This is the first time that we are in here, so we will allocate
		// a structure that will save the vertices that we added, so that we
		// do not add the same thing twice
		first = false;
		added = new vert_added [num_vert];
		// The number of vertices added for each vertex must be initialized to zero
		for (x = 0; x < num_vert; x++)
			(added+x)->num = 0;
	}
	if (vertex)
		// Set the pointer to the vertex, calling again with the normal to fill it with
		last = id;
	else 
	{
		// Fill the pointer with the id of the normal
		if (*(m_pVertNorms + last) == 0)
			*(m_pVertNorms + last) = id;
		else if ((*(m_pVertNorms + last) != id) && ((int4)normal_difference != 360)) 
		{
			// difference is big enough, we need to create a new vertex
			if (NewVertex(normal_difference,id,*(m_pVertNorms + last),n)) 
			{
				// First check to see if we added this vertex and normal already
				if (CheckVN(last,id,added))
					return false;
					// OK, create the new vertex, and have its id = the number of 
					// vertices and its normal what we have here
				m_pVertNorms = (int4 *)realloc(m_pVertNorms, sizeof(int4) * (num_vert+1));
				if (!m_pVertNorms) 
				{
				  throw ex::OutOfMemoryException(LogRec("ViaStrip", "NormArray", "memory allocation error" ));				
				}
				*(m_pVertNorms + num_vert) = id;
				// We created a new vertex, now put it in our added structure so
				// we do not add the same thing twice
				(added+last)->num = (added+last)->num + 1;
				if ((added+last)->num == 1) 
				{
					// First time 
					(added+last)->normal =  new int4 [1];
					*((added+last)->normal) =  id;
				} 
				else 
				{
					// Not the first time, reallocate space
					(added+last)->normal = (int4 *)realloc((added+last)->normal,
						sizeof(int4) * (added+last)->num);
					*((added+last)->normal+((added+last)->num-1)) = id;
				} 
				return true;
			}
		}
	}
	return false;
}

/**
 *	Save the texture with its vertex for future use when outputting 
 *
 *	@param id		texture id
 *	@param vertex	??	
 */
/* void ViaStrip::AddTexture(int id, bool vertex)
{
	static int last;
	
	if (vertex)
		last = id;
	else
		*(m_pVertTexture+last) = id;
} */

/**
 *	Add vertex id to 'ids' buffer
 *
 *	@param id	vert id (>=0 && < num_vert)
 *	@param index_count	the number of the vertex in this polygon
 *	@return		1 if added, 0 if already in the index buffer
 */
int4	ViaStrip::AddVertId(int4 id, int4 index_count)
{
	register int4 x;
  
	for (x=1; x<index_count; x++) 
	{
		// Test if degenerate, if so do not add degenerate vertex
		if (m_IDs[x] == id)
		return 0;
	}
	m_IDs[index_count] = id;
	return 1;
}

/**
 *	add normal id to the structure norms
 *
 *	@param	id			normal id
 *	@param	index_count	the number of the normals in theis polygon
 */
void ViaStrip::AddNormId(int4 id, int4 index_count)
{
	m_iNorms[index_count] = id;
}

/**
 * Add a new face into our face data structure
 *
 * @param ids			a buffer that stores numbered vertices in a polygon
 * @param vret_count	number of vertices in a polygon
 * @param face_id		face id
 * @param norms			a buffer that stores numbered normals in a polygon
 */
void ViaStrip::AddNewFace(int4 ids[MAX1], int4 vert_count, int4 face_id, int4 norms[MAX1])
{
	FFaces * pfNode;
	FVertices * pfVertNode;
	int4	*pTempInt;
	int4 *pnorms;
	FEdges **pTempVertptr;
	int4	*pTempmarked, *pTempwalked;
	register int4 y, count = 0; 

	// Add a new face into our face data structure
	pfNode = new FFaces;
	if (pfNode) 
	{
		pfNode->pPolygon = new int4 [vert_count];
		pfNode->pNorms = new int4 [vert_count];
		pfNode->VertandId = new FEdges * [vert_count]; 
		pfNode->marked = new int4 [vert_count];
		pfNode->walked = new int4 [vert_count];
	}
	pTempInt = pfNode->pPolygon;
	pnorms = pfNode->pNorms;
	pTempmarked = pfNode->marked;
	pTempwalked = pfNode->walked;
	pTempVertptr = pfNode->VertandId;
	pfNode->nPolSize = vert_count;
	pfNode->nOrgSize = vert_count;
	pfNode->seen = -1;
	pfNode->seen2 = -1;
	for (y=1; y<=vert_count; y++) 
	{
		*(pTempInt    + count) = ids[y];
		*(pnorms      + count) = norms[y];
		*(pTempmarked + count) = false;
		*(pTempwalked + count) = -1;
		*(pTempVertptr+ count) = NULL;
		count++;
		// Add this FaceNode to the Vertices Structure
		pfVertNode = new FVertices;
		pfVertNode->face = pfNode;
		AddHead(m_plVertices[ids[y]],(ListInfo *)pfVertNode);
	}
	AddHead(m_ppPolFaces[face_id-1],(ListInfo *)pfNode);
}

// Copy a face node into a new node, used after the global algorithm is run
void ViaStrip::CopyFace(int4 ids[MAX1], int4 vert_count, int4 face_id, int4 norms[MAX1])
{
	FFaces * pfNode;
	int4	*pTempInt;
	int4 *pnorms;
	FEdges **pTempVertptr;
	int4	*pTempmarked, *pTempwalked;
	register int4 y, count = 0;
  
	// Copy a face node into a new node, used after the global algorithm
	// is run, so that we can save whatever is left into a new structure
	pfNode = new FFaces;
	if (pfNode) 
	{
		pfNode->pPolygon = new int4 [vert_count];
		pfNode->pNorms = new int4 [vert_count];
		pfNode->VertandId = new FEdges * [vert_count]; 
		pfNode->marked  = new int4 [vert_count];
		pfNode->walked = new int4 [vert_count];
	}
	pTempInt =pfNode->pPolygon;
	pnorms = pfNode->pNorms;
	pTempmarked = pfNode->marked;
	pTempwalked = pfNode->walked;
	pTempVertptr = pfNode->VertandId;
	pfNode->nPolSize = vert_count;
	pfNode->nOrgSize = vert_count;
	pfNode->seen = -1;
	pfNode->seen2 = -1;
	for (y=0;y<vert_count;y++) 
	{
		*(pTempInt + count) = ids[y];
		*(pnorms + count) = norms[y];
		*(pTempmarked + count) = false;
		*(pTempwalked + count) =  -1;
		*(pTempVertptr+count) = NULL;
		count++;
	}
	AddHead(m_ppPolFaces[face_id-1],(ListInfo *) pfNode);
}

// add adjacent edges
void ViaStrip::AddAdjEdge(int4 v1,int4 v2,int4 fnum,int4 index1) 
{
	FEdges * temp = NULL;  
	FFaces * temp2 = NULL;  
	FEdges * pfNode;
	ListHead * pListHead;  
	ListHead * pListFace;
	bool flag = true;
	register int4 count = 0;
	register int4 t,v3 = -1;  
  
	if (v1 > v2) 
	{
		t  = v1;
		v1 = v2;
		v2 = t;
	}
  
	pListFace  = m_ppPolFaces[fnum];
	temp2 = (FFaces *)PeekList(pListFace,LISTHEAD,0);
	pListHead = m_ppPolEdges[v1];
	temp = (FEdges *)PeekList(pListHead,LISTHEAD,count);
	if (temp == NULL)
		flag = false;

	count++;
	while (flag) 
	{
		if (v2 == temp->edge[0]) 
		{
			if (temp->edge[2] == -1)
				temp->edge[2] = fnum;
			else
				v3 = temp->edge[2];
			flag = false;
		} 
		else 
		{
			temp = (FEdges *)GetNextNode(temp);
			count++;
			if (temp == NULL) 
				flag = false;
		}
	}

	// Did not find it
	if (temp == NULL) 
	{
		pfNode = new FEdges;
		if (!pfNode) 
      throw ex::OutOfMemoryException(LogRec( "ViaStrip", "AddAdjEdge", "memory allocation error"));
	
    pfNode->edge[0] = v2;
		pfNode->edge[1] = fnum;
		pfNode->edge[2] = v3;
		AddTail(m_ppPolEdges[v1], (ListInfo *)pfNode);
		*(temp2->VertandId+index1) = pfNode;
	} 
	else
		*(temp2->VertandId+index1) = temp;
}


// $Log: ViaAdd.C,v $
// Revision 1.2  2005/07/01 12:46:05  vxconfig
// Added new Logger.
//
// Revision 1.1  2005/04/14 12:50:05  geconomos
// initial revision
//
// Revision 3.1  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0  2001/10/14 23:01:49  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:45  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:39:04   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:25:36   ingmar
// Initial revision.
// Revision 1.26  2001/08/13 20:29:19  sachin
// changed exceptions ,warnings,debug and information
// according to the new logger class
//
// Revision 1.25  2001/07/25 16:16:42  lihong
// Defect Id: 599
// Added throw ex:: after the function name
//
// Revision 1.24  2001/07/24 23:06:51  lihong
// Defect ID: 599
// new format of exceptions
//
// Revision 1.23  2001/07/23 20:03:11  lihong
// Defect ID: 599
// Changed previous exception with detailed LogRecord
//
// Revision 1.22  2001/07/23 16:56:27  lihong
// Defect ID: 599
// replaced file version with "$Revision: 1.2 $"
//
// Revision 1.21  2001/07/20 23:07:10  lihong
// Defect ID: 599
// changed abort() to throw ex
//
// Revision 1.20  2001/05/21 00:22:53  dave
// uses & implement. of Error::DebugMsg, Warn, Abort, etc, gone
//
// Revision 1.19  2001/04/13 14:33:42  dave
// changed namespaces for exceptions
//
// Revision 1.18  2001/04/12 15:50:49  dave
// added namespace vx for exceptions
//
// Revision 1.17  2001/04/09 22:27:33  dave
// changing from Abort to exceptions
//
// Revision 1.16  2001/04/09 17:31:26  dmitry
// updated datatypes
//
// Revision 1.15  2001/04/08 21:38:58  dave
// updated doc headers
//
// Revision 1.14  2001/04/02 19:18:20  binli
// code simplified!
//
// Revision 1.13  2001/03/27 19:49:51  binli
// code walkthrough: name convension
//
// Revision 1.12  2001/03/23 20:41:19  binli
// code walkthrough
//
// Revision 1.11  2001/03/22 21:15:21  binli
// renamed some variables according to new code standard.
//
// Revision 1.10  2001/03/21 17:15:55  binli
// reflected changes made below:
//   1> replaced triple variable with standard Triple.
//   2> changed struct Strips to class Strips.
//
// Revision 1.9  2001/03/21 15:24:11  binli
// replaced all structs with classes.
//
// Revision 1.8  2001/03/07 19:57:21  binli
// malloc/free replaced by new/delete
//
// Revision 1.7  2001/02/28 21:01:59  binli
// 1> BOOL->bool
// 2> Error Info
//
// Revision 1.6  2001/02/28 18:19:38  binli
// tried to solve 'BOOL' problem in ViaStrip: changed parameter from BOOL to int in Output_Tri().
//
// Revision 1.5  2000/11/06 23:57:57  binli
// copied the old version (from mixkit) back to replace 'polygon generation'  in lastest version.
// so as to make the triangle generation modular works.
//
// Revision 1.4  2000/10/21 21:00:24  ingmar
// BOOL -> bool and other code cleanup
//
// Revision 1.3  2000/09/20 15:29:48  binli
// Analysized the 'Strip' module to fix the memory leaking problem.
//
// Revision 1.2  2000/09/12 13:24:34  binli
// Analysis for memory leak, as well as binary output
//
// Revision 1.1.1.1  2000/08/11 22:15:54  ingmar
// unit tests complete
//
// 
// 7     9/07/00 12:11p Binli
// fitted to viatronix standard
// 
// 6     9/06/00 10:22a Binli
// fitted to viatronix standard
// 
// 5     8/29/00 1:48p Antonio
// updated footer and header
// 
// *****************  Version 4  *****************
// User: Binli           Date:  8/23/00  Time: 12:57p
// Checked in $/v2k/src/Triangles
// Comment:
//   Added comments
// 
// *****************  Version 3  *****************
// User: Antonio         Date:  8/21/00  Time:  5:57p
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
// Created ViaAdd.C
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/ViaAdd.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
// $Id: ViaAdd.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
