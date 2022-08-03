// $Id: ViaInit.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ViaInit.C  
// Description: Implementation (3/17) of the class ViaStrip
// Created: July 31, 2000
// Author: Freeware,
//		     rewriten by Bin Li (binli@viatronix.net)
//
// Complete History at bottom of file.

#include "stdafx.h"
#include "ViaStrip.h"

#define FILE_REVISION "$Revision: 1.2 $"

/**
 *	Initialize vertex/normal array to have all zeros to start with.
 *
 *	@param	num_vert	the number of vertices
 */
void ViaStrip::InitVertNorms(int4 num_vert)
{
	for (int4 x=0; x<num_vert; x++) 
		*(m_pVertNorms + x) = 0;
}

/** 
 *	Initialize vertex/texture array to have all zeros to start with.
 *
 *	@param	num_vert	the number of vertices
 */
// void ViaStrip::InitVertTexture(int4 num_vert)
// {
//	for (int4 x=0; x<num_vert; x++) 
//		*(m_pVertTexture + x) = 0;
// }

/** 
 *	Initialize the vertex table
 *
 *	@param nSize	the size of the vertex table
 *	@return	True if vertex table is not empty
 */
bool ViaStrip::InitVertexTable(int4 nSize)
{
	m_plVertices = new ListHead * [nSize]; 
 	
	if (m_plVertices) 
	{
		for (int4 nIndex=0; nIndex < nSize; nIndex++) 
		{
			m_plVertices[nIndex] = NULL;
		}
		return true;
	}
	return false;
}

/** 
 *	Initialize the face table
 *
 *	@param nSize	the size(= the number of faces) of the face table
 *	@return	True if face table is not empty
 */
bool ViaStrip::InitFaceTable(int4 nSize)
{
	m_ppPolFaces = new ListHead * [nSize]; 
	if (m_ppPolFaces)
	{
		for (int4 nIndex=0; nIndex < nSize; nIndex++)
		{
			m_ppPolFaces[nIndex] = NULL;
		}
		return true;
	}
	return false;
}

/**	
 *	Initialize the edge table
 *
 *	@param nSize	the size of the edge table
 *	@return	true when edge table is not empty
 */
bool ViaStrip::InitEdgeTable(int4 nSize)
{
	m_ppPolEdges = new ListHead * [nSize];
	if (m_ppPolEdges)
	{
		for (int4 nIndex=0; nIndex < nSize; nIndex++)
		{
			m_ppPolEdges[nIndex] = NULL;
		}
		return true;
	}
	return false;
}

/** 
 *	Initialize the strip table
 */
void ViaStrip::InitStripTable() 
{
	ListHead * pListHead;

	pListHead = new ListHead;
	if (!pListHead) 
    throw ex::OutOfMemoryException(LogRec("ViaStrip", "InitStripTable", "Out of memory."));

  InitList(pListHead);
	m_pStrips = pListHead;
}

/**
 *	This routine will initialize the table that will
 *	have the faces sorted by the number of adjacent polygons to it.
 *
 *	@param numfaces	number of faces
 */
void ViaStrip::InitTableSGI(int4 numfaces) 
{
	ListHead * pListHead;
	int4 max_adj = 60;
	register int4 x;

	// This routine will initialize the table that will
    // have the faces sorted by the number of adjacent polygons to it.
	for (x=0; x< max_adj; x++) 
	{
		// We are allowing the max number of sides of a polygon to be max_adj.
		pListHead = new ListHead;
		if (!pListHead) 
      throw ex::OutOfMemoryException(LogRec("ViaStrip", "InitTableSGI" , "Out of memory."));

    InitList( pListHead );
	  m_pArray[x] = pListHead;
	} // endfor (x-loop)
	if (m_pFaceArray != NULL) // It seems this function is called more than
		delete [] m_pFaceArray;   // once so we'll free up the old stuff

	m_pFaceArray = new FACE_ADJACENCIES [numfaces]; 

	if (m_pFaceArray == NULL) 
		throw ex::OutOfMemoryException(LogRec( "ViaStrip", "InitTableSGI", "Out of memory."));
              
}

/**
 *	build vertex table
 *
 *	@param	nSize	size of the vertex table
 */
void ViaStrip::BuildVertexTable(int4 nSize)
{
	register int4 nIndex;
	ListHead * pListHead;

	// for each vertex, do ...
	for (nIndex=0; nIndex < nSize; nIndex++) 
	{
		pListHead = new ListHead;
		if (pListHead) 
		{
			InitList(pListHead);
			m_plVertices[nIndex] = pListHead;
		}
		else 
			return; 
	}
}

/**
 *	build face table
 *
 *	@param	nSize	size of the face table
 */
void ViaStrip::BuildFaceTable(int4 nSize)
{
	register int4 nIndex;
	ListHead * pListHead;

	// for each polygon, do ...
	for (nIndex=0; nIndex < nSize; nIndex++) 
	{
		pListHead = new ListHead;
		if (pListHead) 
		{
			InitList(pListHead);
			m_ppPolFaces[nIndex] = pListHead;
		}
		else 
			return; 
	} // endfor
}

/** 
 *	build edge table
 *
 *	@param	nSize	the size of edge table
 */
void ViaStrip::BuildEdgeTable(int4 nSize)
{
	register int4 nIndex;
	ListHead * pListHead;

	// for each edge, do ...
	for (nIndex=0; nIndex < nSize; nIndex++) 
	{
		pListHead = new ListHead;
		if ( pListHead ) 
		{
			InitList(pListHead);
			m_ppPolEdges[nIndex] = pListHead;
		}
		else 
			return;
	} // endfor
}

/**
 *	initialize and build vertex table
 *
 *	@param	numverts	the number of vertices
 */
void ViaStrip::StartVertexStruct(int4 numverts)
{
	if (InitVertexTable(numverts)) 
	{
		BuildVertexTable(numverts);
	}
}

/** 
 *	initialize and build Face table
 *
 *	@param	numverts	the number of faces \
 */
void ViaStrip::StartFaceStruct(int4 numfaces)
{
	if (InitFaceTable(numfaces)) 
	{
		BuildFaceTable(numfaces);
	}
}

/** 
 *	initialize and build Edge table
 *
 *	@param	numverts	the number of vertices
 */
void ViaStrip::StartEdgeStruct(int4 numverts)
{
	if (InitEdgeTable(numverts)) 
	{
		BuildEdgeTable(numverts);
	}
}


// $Log: ViaInit.C,v $
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
//    Rev 2.0   Sep 16 2001 23:39:04   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:25:38   ingmar
// Initial revision.
// Revision 1.19  2001/08/13 20:29:19  sachin
// changed exceptions ,warnings,debug and information
// according to the new logger class
//
// Revision 1.18  2001/07/25 16:08:39  lihong
// Defect Id: 599
// Added throw ex:: after the function name
//
// Revision 1.17  2001/07/24 23:06:51  lihong
// Defect ID: 599
// new format of exceptions
//
// Revision 1.16  2001/07/23 20:03:11  lihong
// Defect ID: 599
// Changed previous exception with detailed LogRecord
//
// Revision 1.15  2001/04/13 14:33:42  dave
// changed namespaces for exceptions
//
// Revision 1.14  2001/04/12 15:50:49  dave
// added namespace vx for exceptions
//
// Revision 1.13  2001/04/09 22:27:33  dave
// changing from Abort to exceptions
//
// Revision 1.12  2001/04/09 17:31:26  dmitry
// updated datatypes
//
// Revision 1.11  2001/04/02 19:18:20  binli
// code simplified!
//
// Revision 1.10  2001/03/27 19:49:51  binli
// code walkthrough: name convension
//
// Revision 1.9  2001/03/23 20:41:19  binli
// code walkthrough
//
// Revision 1.8  2001/03/22 21:15:21  binli
// renamed some variables according to new code standard.
//
// Revision 1.7  2001/03/21 15:24:11  binli
// replaced all structs with classes.
//
// Revision 1.6  2001/03/07 19:57:21  binli
// malloc/free replaced by new/delete
//
// Revision 1.5  2001/02/28 21:01:59  binli
// 1> BOOL->bool
// 2> Error Info
//
// Revision 1.4  2000/11/06 23:57:57  binli
// copied the old version (from mixkit) back to replace 'polygon generation'  in lastest version.
// so as to make the triangle generation modular works.
//
// Revision 1.3  2000/10/21 21:06:02  ingmar
// BOOL -> bool and other code cleanup
//
// Revision 1.2  2000/09/20 15:29:48  binli
// Analysized the 'Strip' module to fix the memory leaking problem.
//
// Revision 1.1.1.1  2000/08/11 22:15:54  ingmar
// unit tests complete
//
// 
// 6     9/07/00 12:11p Binli
// fitted to viatronix standard
// 
// 5     8/29/00 1:43p Antonio
// updatd header and footer
// 
// *****************  Version 4  *****************
// User: Binli           Date:  8/23/00  Time: 10:34a
// Checked in $/v2k/src/Triangles
// Comment:
//   Added comments
// 
// *****************  Version 3  *****************
// User: Antonio         Date:  8/21/00  Time:  6:01p
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
// User: Binli           Date:  7/31/00  Time:  5:21p
// Created ViaInit.C
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/ViaInit.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
// $Id: ViaInit.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
