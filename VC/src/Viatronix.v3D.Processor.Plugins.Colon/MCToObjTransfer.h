// $Id: MCToObjTransfer.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: MCToObjTransfer.h  
// Description: Transfer data format from Marching Cubes' to Obj
// Created: July 31, 2000  
// Author: Bin Li (binli@viatronix.net)
//
//
// Complete History at bottom of file.


#ifndef MC_TO_OBJ_TRANSFER_h
#define MC_TO_OBJ_TRANSFER_h

#include "Triple.h"
#include "ColonWallPNG.h"

class MCToObjTransfer
{
	// member functions
	//------------------

	// Constructor/Destructor/Access functions
	//----------------------------------------
public:
	/// Constructor
	MCToObjTransfer();

	/// Destructor
	virtual ~MCToObjTransfer();

	/// set pointer to the polygon buffer1.
	void SetPtrToPolygonBuffer(float4 *plgPtr);

	/// get the pointer to (character) buffer;
	int1 * GetPlgBufPtr() const;

	/// set the number of polygons in this buffer.
	void SetPlgNoInBuffer(const int4 num);

	/// get the number of vertices, normals and faces (polygons).
	int4 GetNumberOfVertices() const;
	int4 GetNumberOfNormals() const;
	int4 GetNumberOfFaces() const;

	// Primary functions
	//------------------

	/// transfer polygon file from MarchinCubes format to .obj format. 
	void DoMcToObjTransfer(int4 fileNum, ColonWallPNG * pColonWallPNG);

private:
	/// responsible for transfer
	int4 TransferFormat(int4 fileNo, ColonWallPNG * pColonWallPNG);

	// Data members
	//-------------

private:
	//  this class defines a cell of buffer for transfer.
	
	class VBufStruct  
	{
	public:
		float4 x,y,z;	// coordinates : vertex or normal.
		int4	vertNum;	// serial number of vertex or normal.
	};

	// buffer used for transfer. Use this kind of 2D buffer that the
	// the comparison can be done not linearly.
	VBufStruct *m_pVBuf[512][512];

	// number of polygons in the buffer
	int4 m_plgNumber;

	// pointer to the polygon buffer1: generated in last step ('MarchinCubes').
	float4 * m_pPlgBuf;

	// pointer to the polygon buffer2 (character buffer):
	// generated in this submodule and will be used by next step: 'Strip'.
	int1 * m_pPlgBufC;

	// the number of vertices, normals, and faces (polygons).
	int4	m_iVertexNum;
	int4	m_iNormalNum;
	int4	m_iFaceNum;

}; // end Class MCToObjTransfer

#endif // MC_TO_OBJ_TRANSFER_h


// $Log: MCToObjTransfer.h,v $
// Revision 1.1  2005/04/14 12:50:05  geconomos
// initial revision
//
// Revision 3.0  2001/10/14 23:01:49  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:45  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:39:02   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:25:32   ingmar
// Initial revision.
// Revision 1.16  2001/04/12 18:07:35  dmitry
// modified base class for documentation
//
// Revision 1.15  2001/04/12 14:38:47  dmitry
// Sec_j_tri base class added
//
// Revision 1.14  2001/04/09 17:31:27  dmitry
// updated datatypes
//
// Revision 1.13  2001/04/02 19:18:20  binli
// code simplified!
//
// Revision 1.12  2001/03/27 15:25:31  binli
// 'const' functions for safety.
//
// Revision 1.11  2001/03/23 20:42:15  binli
// reflected the add of new class ColonWallPNG
//
// Revision 1.10  2001/03/22 21:15:21  binli
// renamed some variables according to new code standard.
//
// Revision 1.9  2001/03/15 18:30:21  binli
// code cleaned.
//
// Revision 1.8  2001/01/19 15:48:58  binli
// moved temproal files: triangle.tri.and triangle.smf, from common work directory to their own directory.
//
// Revision 1.7  2001/01/02 22:32:40  binli
// used 2-level pointers to improve the efficiency of data format transfer.
//
// Revision 1.6  2000/12/13 21:59:18  binli
// created PNG header of triangle files.
//
// Revision 1.5  2000/10/21 06:06:04  ingmar
// merged mixkit branch back into main trunk
//
// Revision 1.3.10.1  2000/10/19 20:37:01  binli
// code optimization
//
// Revision 1.3  2000/09/20 15:12:36  binli
// Viatronix C++ coding walk through
//
// Revision 1.2  2000/09/14 20:15:44  binli
// fitted to Viatronix C++ code convention and Standard
//
// Revision 1.1.1.1  2000/08/11 22:15:54  ingmar
// unit tests complete
//
// 
// 8     9/05/00 4:21p Binli
// change the sub-pipeline of polygon generation (3) : gap the 'data
// format transfer' submodule and 'getting triangle strips' submodule, so
// no temporary file(s) '.obj' is (are) generated.
// 
// 7     9/01/00 12:49p Binli
// change the sub-pipeline of polygon generation (2) : gap the 'polygon
// generation'
// sub-module and 'data format transfer' sub-module, so no temporary
// file(s)
// '.plg' is(are) generated.
// 
// 6     8/29/00 12:50p Antonio
// updated footer
// 
// *****************  Version 5  *****************
// User: Binli           Date:  8/22/00  Time:  2:36p
// Checked in $/v2k/src/Triangles
// Comment:
//   Added Comments
// 
// *****************  Version 4  *****************
// User: Antonio         Date:  8/21/00  Time:  6:29p
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
// User: Binli           Date:  8/09/00  Time:  2:12p
// Checked in $/v2k/src/Triangles
// 
// *****************  Version 1  *****************
// User: Binli           Date:  7/31/00  Time:  5:00p
// Created MCToObjTransfer.h
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/MCToObjTransfer.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $
// $Id: MCToObjTransfer.h,v 1.1 2005/04/14 12:50:05 geconomos Exp $
