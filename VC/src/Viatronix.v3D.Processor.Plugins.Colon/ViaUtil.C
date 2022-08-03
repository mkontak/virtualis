// $Id: ViaUtil.C,v 1.2 2006/06/02 19:01:16 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ViaUtil.C  
// Description: Implementation (8/17) of Class ViaStrip  
// Created: July 31, 2000 
// Author:	FreeWaree-mail
//			    Rewriten by Bin Li (binli@viatronix.net)
//
// Complete History at bottom of file.


#include "stdafx.h"
#include "ViaStrip.h"

/**
 *	get a lower value and put it in x
 *
 *	@param	x, y	input values
 *	@return	the lower of input values
 */
void ViaStrip::SwitchLower (int4 *x, int4 *y)
{
	register int4 temp;
  
	// Put lower value in x
	if (*y < *x) 
	{
		temp = *x;
		*x = *y;
		*y = temp;
	}
}

/**
 *	determine if x is in the triangle specified by id1,id2,id3
 *
 *	@param	x				int value
 *	@param	id1,id2,id3		int values
 *	@return	true when find, false otherwise
 */
bool ViaStrip::member(int4 x , int4 id1, int4 id2, int4 id3)
{
	if ((x != id1) && (x != id2) && (x != id3))
		return false;
		  
	return true;
}

/**
 *	Does the edge specified by id1 and id2 exist in this
 *	face currently? Maybe we deleted in partial triangulation
 *
 *	@param	face_id		face id
 *	@param	id1, id2	edge ids
 *	@return	true when found the edge, false otherwise
 */
bool ViaStrip::Exist(int4 face_id, int4 id1, int4 id2)
{
	ListHead *pListHead;
	FFaces * temp;
	register int4 x,size;
	bool a=false, b=false; 
  
	pListHead = m_ppPolFaces[face_id];
	temp = (FFaces *) PeekList( pListHead, LISTHEAD, 0 );
	size = temp->nPolSize;
	for (x=0; x<size; x++) 
	{
		if (*(temp->pPolygon+x) == id1) a = true;
		if (*(temp->pPolygon+x) == id2) b = true;
		if (a && b)
			return true;
	}
	return false;
}

/**
 *	Find the vertex in the first 3 numbers that does not exist in the last three numbers
 *
 *	@param	id1,id2,id3		vertex ids
 *	@param	id4,id5,id6		vertex ids
 *	@param	x,y				vertex ids
 *	@return	vertex ids
 */
int4 ViaStrip::Different(int4 id1,int4 id2,int4 id3,int4 id4,int4 id5, int4 id6, int4 *x, int4 *y)
{
	// Find the vertex in the first 3 numbers that does not exist in 
	// the last three numbers
	if ((id1 != id4) && (id1 != id5) && (id1 != id6)) 
	{
		*x = id2;
		*y = id3;
		return id1;
	}
	if ((id2 != id4) && (id2 != id5) && (id2 != id6)) 
	{
		*x = id1;
		*y = id3;
		return id2;
	}
	if ((id3 != id4) && (id3 != id5) && (id3 != id6)) 
	{
		*x = id1;
		*y = id2;
		return id3;
	}
  
	// Because there are degeneracies in the data, this might occur
	*x = id5;
	*y = id6;
	return id4;
}

/**	
 *	We have a triangle and want to know the third vertex of it
 */
int4 ViaStrip::ReturnOther(int4 *index,int4 e1,int4 e2)
{
	// We have a triangle and want to know the third vertex of it
	for (int4 x=0; x<3; x++) 
	{
		if ((*(index+x) != e1) && (*(index+x) != e2))
			return *(index+x);
	}
	// If there is a degenerate triangle return arbitrary
	return e1;
}

/**
 *	We have a list index of 4 numbers and we wish to return the number that is not id1,id2 or id3
 */
int4 ViaStrip::GetOtherVertex(int4 id1,int4 id2,int4 id3,int4 *index)
{
	// We have a list index of 4 numbers and we wish to return the number that is not id1,id2 or id3
	int4 x=0;
	for ( ;x<4; x++) 
	{
		if ((*(index+x) != id1) && (*(index+x) != id2) && (*(index+x) != id3))
			return *(index+x);
	}
	// If there is some sort of degeneracy this might occur, return arbitrary 
	if (x==4)
		return id1;

	return NULL;
}

/**
 *	Check to see whether the polygon with face_id was used
 *	already, return NULL if it was, otherwise return a pointer to the face.
 */
ViaStrip::ListInfo * ViaStrip::Done(int4 face_id, int4 *bucket)
{
	// Check to see whether the polygon with face_id was used
	// already, return NULL if it was, otherwise return a pointer to the face.
	ListInfo * lpListInfo;
  
	lpListInfo = (ListInfo *) m_pFaceArray[face_id].pfNode;
	if (lpListInfo != NULL) 
	{
		*bucket = m_pFaceArray[face_id].bucket;
		return lpListInfo;
	} 
	else
		return lpListInfo;
}

/**
 *	Get the first triangle in the strip we just found, we will use this to
 *	try to extend backwards in the strip
 */
void ViaStrip::FirstEdge(int4 *id1,int4 *id2, int4 *id3)
{
	// Get the first triangle in the strip we just found, we will use this to
	// try to extend backwards in the strip
	register int4 num;
	ListHead *pListHead;
	Strips *temp1, *temp2, *temp3;
  
	pListHead = m_pStrips;
	num = NumOnList(pListHead);
  
	// Did not have a strip
	if (num < 3)
		return;
  
	temp1 = (Strips *) PeekList( pListHead, LISTHEAD, 0);
	temp2 = (Strips *) PeekList( pListHead, LISTHEAD, 1);
	temp3 = (Strips *) PeekList( pListHead, LISTHEAD, 2);
	*id1 = temp1->face_id;
	*id2 = temp2->face_id;
	*id3 = temp3->face_id;
}

/**
 *	need the last edge that we had
 */
void ViaStrip::LastEdge(int4 *id1, int4 *id2, int4 *id3, bool save)
{
	// We need the last edge that we had
	static int4 v1, v2, v3;
  
	if (save) 
	{
		v1 = *id1;
		v2 = *id2;
		v3 = *id3;
	} 
	else 
	{
		*id1 = v1;
		*id2 = v2;
		*id3 = v3;
	}
}

/**
 *	Search through face to match orignal vertices
 *	Start with vertex1's Vertices struct
 */
void ViaStrip::FindTriangleOrientation(int4 vertex1, int4 vertex2, int4 vertex3, int4 *original_vertex)
{
	int4 vertices,index;
	FVertices * verts;
  
	// Search through face to match orignal vertices
	// Start with vertex1's Vertices struct
	verts = (FVertices * ) PeekList(m_plVertices[vertex1-1],LISTHEAD,0);
	do {  
		index = 0;
		for (vertices = 0; vertices < verts->face->nOrgSize;vertices++) 
		{
			if (vertex1 == verts->face->pPolygon[vertices]+1 || 
				vertex2 == verts->face->pPolygon[vertices]+1 ||
				vertex3 == verts->face->pPolygon[vertices]+1 )
			{
				original_vertex[index++] = verts->face->pPolygon[vertices]+1;
			}
			if (index == 3)
				break;
		}
		if (index == 3)
			break;
	} while ((verts = (FVertices *)GetNextNode(verts)) != NULL);
  
	if (index != 3) 
	{
		// Search vertex2's Vertices struct
		verts = (FVertices *) PeekList(m_plVertices[vertex2-1],LISTHEAD,0);
		do {
			index = 0;
			for (vertices = 0; vertices < verts->face->nOrgSize;vertices++) 
			{
				if (vertex1 == verts->face->pPolygon[vertices]+1 || 
					vertex2 == verts->face->pPolygon[vertices]+1 ||
					vertex3 == verts->face->pPolygon[vertices]+1 )
				{
					original_vertex[index++] = verts->face->pPolygon[vertices]+1;
				}
				if (index == 3)
				break;
			}
			if (index == 3)
				break;
		} while ((verts = (FVertices *)GetNextNode(verts)) != NULL);
	}
  
	if (index != 3) 
	{
		// Search vertex3's Vertices struct
		verts = (FVertices *) PeekList(m_plVertices[vertex3-1],LISTHEAD,0);
		do {    
			index = 0;
			for (vertices = 0; vertices < verts->face->nOrgSize;vertices++) 
			{
				if (vertex1 == verts->face->pPolygon[vertices]+1 || 
					vertex2 == verts->face->pPolygon[vertices]+1 ||
					vertex3 == verts->face->pPolygon[vertices]+1 )
					original_vertex[index++] = verts->face->pPolygon[vertices]+1;      
				if (index == 3)
				break;
			}
			if (index == 3)
				break;
		} while ((verts = (FVertices *)GetNextNode(verts)) != NULL);
	}
}

/**	
 *	preserve strip orientation with normal
 */
void ViaStrip::PreserveStripOrientationWithNormal(int4 vertex1, int4 normal1, int4 vertex2, int4 normal2,
					    int4 vertex3, int4 normal3, std::ofstream & band)
{
	int4 original_vertex[3];
	int4 bandBuf;
  
	FindTriangleOrientation(vertex1,vertex2,vertex3, original_vertex);
  
	if ((original_vertex[0] == vertex3 &&
		original_vertex[1] == vertex2 &&
		original_vertex[2] == vertex1) ||
       (original_vertex[0] == vertex2 &&
		original_vertex[1] == vertex1 &&
		original_vertex[2] == vertex3) ||
       (original_vertex[0] == vertex1 &&
		original_vertex[1] == vertex3 &&
		original_vertex[2] == vertex2))  
	{
		// New Triangle is in an opposite orientation
		// Add vertex2 to correct it 
		// 'band' write:
		band << ' ';
		bandBuf = vertex2;
		band.write((int1 *) &bandBuf, sizeof(int4));
		band << "//";
		bandBuf = normal2;
		band.write((int1 *) &bandBuf, sizeof(int4));
	} // end-if
} // preserve_strip_orientation_with_normal()

/**
 *	preserve_strip_orientation_with_texture
 */
void ViaStrip::PreserveStripOrientationWithTexture(
					     int4 vertex1, int4 texture1,
					     int4 vertex2, int4 texture2,
					     int4 vertex3, int4 texture3,
							 std::ofstream & band)
{
	int4 original_vertex[3];
	int4 bandBuf;
  
	FindTriangleOrientation(vertex1,vertex2,vertex3,original_vertex);
  
	if ((original_vertex[0] == vertex3 &&
         original_vertex[1] == vertex2 &&
         original_vertex[2] == vertex1 ) ||
       ( original_vertex[0] == vertex2 &&
         original_vertex[1] == vertex1 &&
         original_vertex[2] == vertex3 ) ||
       ( original_vertex[0] == vertex1 &&
         original_vertex[1] == vertex3 &&
         original_vertex[2] == vertex2 )) 
	{
		// New Triangle is in an opposite orientation
		// Add vertex2 to correct it
		// 'band' write:
		band << ' ';
		bandBuf = vertex2;
		band.write((int1 *) &bandBuf, sizeof(int4));
		band << "/";
		bandBuf = texture2;
		band.write((int1 *) &bandBuf, sizeof(int4));
	}
}

/**
 *	preserve strip orientation with texture/normal
 */
void ViaStrip::PreserveStripOrientationWithTextureAndNormal(
							 int4 vertex1, int4 texture1, int4 normal1,
							 int4 vertex2, int4 texture2, int4 normal2,
							 int4 vertex3, int4 texture3, int4 normal3,
							 std::ofstream & band)
{
	int4 original_vertex[3];
	int4 bandBuf;

	FindTriangleOrientation(vertex1,vertex2,vertex3,original_vertex);

	if (( original_vertex[0] == vertex3 &&
         original_vertex[1] == vertex2 &&
         original_vertex[2] == vertex1) ||
       ( original_vertex[0] == vertex2 &&
         original_vertex[1] == vertex1 &&
         original_vertex[2] == vertex3) ||
       ( original_vertex[0] == vertex1 &&
         original_vertex[1] == vertex3 &&
         original_vertex[2] == vertex2)) 
	{
		// New Triangle is in an opposite orientation
		// Add vertex2 to correct it
		// 'band' write:
		band << ' ';
		bandBuf = vertex2;
		band.write((int1 *) &bandBuf, sizeof(int4));
		band << "/";
		bandBuf = texture2;
		band.write((int1 *) &bandBuf, sizeof(int4));
		band << "/";
		bandBuf = normal2;
		band.write((int1 *) &bandBuf, sizeof(int4));
	} // end-if
} // preserve_strip_orientation_with_texture_and_normal()

/**
 *	preserve strip orientation
 */
void ViaStrip::PreserveStripOrientation(int4 vertex1, int4 vertex2, int4 vertex3, std::ofstream & band)
{ 
	int4 original_vertex[3];
	int4 bandBuf;
  
	FindTriangleOrientation(vertex1,vertex2,vertex3,original_vertex);

	if ((original_vertex[0] == vertex3 &&
         original_vertex[1] == vertex2 &&
         original_vertex[2] == vertex1) ||
       ( original_vertex[0] == vertex2 &&
         original_vertex[1] == vertex1 &&
         original_vertex[2] == vertex3) ||
       ( original_vertex[0] == vertex1 &&
         original_vertex[1] == vertex3 &&
         original_vertex[2] == vertex2 ))  
	{
		// New Triangle is in an opposite orientation
		// Add vertex2 to correct it
		// 'band' write:
		band << ' ';
		bandBuf = vertex2;
		band.write((int1 *) &bandBuf, sizeof(int4));
	}
}


// $Log: ViaUtil.C,v $
// Revision 1.2  2006/06/02 19:01:16  geconomos
// updated to vs 2005
//
// Revision 1.1  2005/04/14 12:50:05  geconomos
// initial revision
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
// Revision 1.14  2001/07/03 14:59:48  soeren
// removed stdafx.c and .h
//
// Revision 1.13  2001/04/09 17:31:25  dmitry
// updated datatypes
//
// Revision 1.12  2001/03/27 19:49:51  binli
// code walkthrough: name convension
//
// Revision 1.11  2001/03/27 16:55:54  binli
// reflected new code standard.
//
// Revision 1.10  2001/03/22 21:15:22  binli
// renamed some variables according to new code standard.
//
// Revision 1.9  2001/03/21 17:15:55  binli
// reflected changes made below:
//   1> replaced triple variable with standard Triple.
//   2> changed struct Strips to class Strips.
//
// Revision 1.8  2001/03/21 16:36:39  binli
// reflected the movement of classes.
//
// Revision 1.7  2001/03/21 15:24:11  binli
// replaced all structs with classes.
//
// Revision 1.6  2001/02/28 21:01:59  binli
// 1> BOOL->bool
// 2> Error Info
//
// Revision 1.5  2000/12/08 21:16:41  binli
// removed code to write out 'text' triangle files.
//
// Revision 1.4  2000/12/08 16:56:18  binli
// changed the output of triangle files from text to binary.
// added header to triangle files.
//
// Revision 1.3  2000/11/06 23:57:48  binli
// copied the old version (from mixkit) back to replace 'polygon generation'  in lastest version.
// so as to make the triangle generation modular works.
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
// 5     8/29/00 1:02p Antonio
// updated footer
// 
// *****************  Version 4  *****************
// User: Binli           Date:  8/24/00  Time: 12:30p
// Checked in $/v2k/src/Triangles
// Comment:
//   Added comments to Class ViaStrip
// 
// *****************  Version 3  *****************
// User: Antonio         Date:  8/21/00  Time:  6:22p
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
// Created ViaUtil.C
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/ViaUtil.C,v 1.2 2006/06/02 19:01:16 geconomos Exp $
// $Id: ViaUtil.C,v 1.2 2006/06/02 19:01:16 geconomos Exp $
