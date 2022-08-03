// $Id: ViaStrip.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: ViaStrip.C
// Description: implementation (1/17) of the ViaStrip class. 
// Created: July 31, 2000 
// Author: Freeware;
//		   rewrited by Bin Li --  binli@viatronix.net
//
// Complete History at bottom of file.


#include "stdafx.h"
#include "ViaStrip.h"

/// Constructor: set up varables to NULL
ViaStrip::ViaStrip()
{
	m_pVertices = NULL;
	m_pNVertices = NULL,
	m_pPVertices = NULL,
	m_pPNVertices = NULL;
}

/// Destructor
ViaStrip::~ViaStrip()
{
}


/**
 * set the number of vertices, normals, and faces (polygon).
 *
 * @param	num_vertex	the number of the vertices in the polygon buffer.
 * @param	num_normals	the number of the normals in the polygon buffer.
 * @param	num_faces		the number of the faces in the polygon buffer.
 */
void ViaStrip::SetupPara(const int4 num_vertex, const int4 num_normals, const int4 num_faces)
{
	m_iVertexNum = num_vertex;
	m_iNormalNum = num_normals;
	m_iFaceNum = num_faces + 1;
}

/**
 *  set the pointer to the polygon buffer);
 *
 *	@param	ptr		the pointer to the input polygon buffer
 */
void ViaStrip::SetPtrToPlgBuf(int1 *ptr)
{
	m_pPlgBuf = ptr;
}

// $Log: ViaStrip.C,v $
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
//    Rev 2.0   Sep 16 2001 23:39:10   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:25:42   ingmar
// Initial revision.
// Revision 1.8  2001/07/03 14:59:48  soeren
// removed stdafx.c and .h
//
// Revision 1.7  2001/04/09 17:31:25  dmitry
// updated datatypes
//
// Revision 1.6  2001/03/27 19:49:51  binli
// code walkthrough: name convension
//
// Revision 1.5  2001/03/22 21:15:22  binli
// renamed some variables according to new code standard.
//
// Revision 1.4  2001/02/09 18:24:42  binli
// // defect 000186: turn the temporal file 'triangle.smf' into in-memory buffer
//
// Revision 1.3  2000/09/20 15:29:47  binli
// Analysized the 'Strip' module to fix the memory leaking problem.
//
// Revision 1.2  2000/09/11 18:36:30  binli
// test
//
// Revision 1.1.1.1  2000/08/11 22:15:54  ingmar
// unit tests complete
//
// 
// 7     9/07/00 12:11p Binli
// fitted to viatronix standard
// 
// 6     9/05/00 4:21p Binli
// change the sub-pipeline of polygon generation (3) : gap the 'data
// format transfer' submodule and 'getting triangle strips' submodule, so
// no temporary file(s) '.obj' is (are) generated.
// 
// 5     8/29/00 1:13p Antonio
// updated footer
// 
// *****************  Version 4  *****************
// User: Binli           Date:  8/23/00  Time: 10:34a
// Checked in $/v2k/src/Triangles
// Comment:
//   Added comments
// 
// *****************  Version 3  *****************
// User: Antonio         Date:  8/21/00  Time:  6:15p
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
// User: Binli           Date:  7/31/00  Time:  5:20p
// Created ViaStrip.C
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/ViaStrip.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $
// $Id: ViaStrip.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $
