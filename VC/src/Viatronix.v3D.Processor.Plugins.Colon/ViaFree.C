// $Id: ViaFree.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ViaFree.C  
// Description:  implementation (17a/18) of class ViaStrip
// Created: July 31, 2000
// Author:  Freeware
//			    Rewriten Bin Li (binli@viatronix.net)
//
// Complete History at bottom of file.

#include "stdafx.h"
#include "ViaStrip.h"

/**
 *	parse and free list
 *
 *	@param	pListHead	pointer to the list head
 */
void ViaStrip::ParseAndFreeList(ListHead *pListHead)
{
	ListInfo * value;
	register int4 c,num;
  
	// Freeing a linked list
	num = NumOnList(pListHead);
	for (c=0; c<num; c++)
		value = RemHead(pListHead);
}

/**
 *	free strips
 */
void ViaStrip::FreeStrips()
{
	// Free strips data structure
	if (m_pStrips == NULL)
		return;
	else
		ParseAndFreeList(m_pStrips);
}

/**
 *	free face table
 *
 *	@param nSize	the size of the table
 */
void ViaStrip::FreeFaceTable(int4 nSize)
{
	register int4 nIndex;
  
	for (nIndex=0; nIndex < nSize; nIndex++) 
	{ 
		if (m_ppPolFaces[nIndex] != NULL) 
			ParseAndFreeList( m_ppPolFaces[nIndex] );
	}
	delete [] m_ppPolFaces;
}

/**
 *	free enge table
 *
 *	@param nSize	the size of the table
 */
void ViaStrip::FreeEdgeTable(int4 nSize)
{
	register int4 nIndex;
  
	for (nIndex=0; nIndex < nSize; nIndex++ )
	{
		if (m_ppPolEdges[nIndex] != NULL)
			ParseAndFreeList(m_ppPolEdges[nIndex]);
	}
	delete m_ppPolEdges;
}

/**
 *	call it to free face table
 *
 *	@param numfaces		the number of the faces
 */
void ViaStrip::EndFaceStruct(int4 numfaces)
{
	FreeFaceTable(numfaces);
}

/**
 *	call it to free edge table
 *
 *	@param	numverts	the number of the vertices
 */
void ViaStrip::EndEdgeStruct(int4 numverts)
{
	FreeEdgeTable(numverts);
}


// $Log: ViaFree.C,v $
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
//    Rev 2.0   Sep 16 2001 23:39:04   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:25:36   ingmar
// Initial revision.
// Revision 1.8  2001/04/09 17:31:26  dmitry
// updated datatypes
//
// Revision 1.7  2001/03/27 19:49:51  binli
// code walkthrough: name convension
//
// Revision 1.6  2001/03/27 16:55:53  binli
// reflected new code standard.
//
// Revision 1.5  2001/03/23 20:41:19  binli
// code walkthrough
//
// Revision 1.4  2001/03/21 15:24:11  binli
// replaced all structs with classes.
//
// Revision 1.3  2001/03/07 19:57:21  binli
// malloc/free replaced by new/delete
//
// Revision 1.2  2001/02/28 21:01:59  binli
// 1> BOOL->bool
// 2> Error Info
//
// Revision 1.1.1.1  2000/08/11 22:15:54  ingmar
// unit tests complete
//
// 
// 6     9/06/00 10:22a Binli
// fitted to viatronix standard
// 
// 5     8/29/00 1:45p Antonio
// updated footer and header
// 
// *****************  Version 4  *****************
// User: Binli           Date:  8/24/00  Time: 12:30p
// Checked in $/v2k/src/Triangles
// Comment:
//   Added comments to Class ViaStrip
// 
// *****************  Version 3  *****************
// User: Antonio         Date:  8/21/00  Time:  6:00p
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
// Created ViaFree.C
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/ViaFree.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $
// $Id: ViaFree.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $
