// $Id: MCToObjTransfer.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: MCToObjTransfer.C  
// Description: Transfer data format from Marching Cubes' to Obj
// Created:  July 1, 2000
// Author: Bin Li (binli@viatronix.net)
//
// Complete History at bottom of file.



#include "stdafx.h"
#include "MCToObjTransfer.h"

#ifdef __linux
#endif

using namespace std;

const uint4 buffSize = 512;

/// Constructor : initialize.
MCToObjTransfer::MCToObjTransfer()
{
	// open an 512x512 buffer, each element is pointer to a sub-buffer
	// that vertices saved in it are of the same x and y coordinates
	// The comparison is done not linearly on this 2D buffer.
	for (int4 j=0; j<512; j++)
		for (int4 i=0; i<512; i++)
			m_pVBuf[j][i] = NULL;

	m_pPlgBuf = NULL;
	m_pPlgBufC = NULL;
	m_iVertexNum = m_iNormalNum = m_iFaceNum = 0;
} // MCToObjTransfer()


/// Destructor: free memory.
MCToObjTransfer::~MCToObjTransfer()
{
	for (int4 j=0; j<512; j++)
		for (int4 i=0; i<512; i++)
			if (m_pVBuf[j][i]) 
				delete m_pVBuf[j][i];
} // ~MCToObjTransfer()


/**
 *  transfer the 'Marching Cube' format of polygons to .obj format
 *
 *  @param filename		root of the file name;
 *	@param fileNum		serial number of the segment
 */
void MCToObjTransfer::DoMcToObjTransfer(int4 iFileNum, ColonWallPNG * pColonWallPNG)
{
	TransferFormat(iFileNum, pColonWallPNG);
} // DoMcToObjTransfer()


/** 
 *	set the pointer to the polygon buffer
 *
 *	@param plgPtr	pointer to the polygon buffer
 */
void MCToObjTransfer::SetPtrToPolygonBuffer(float4 *pfPlgPtr)
{
	m_pPlgBuf = pfPlgPtr;	
} // SetPtrToPolygonBuffer()


/**
 *	set the number of polygons in the buffer
 *
 *	@param	num		the number of polygons in this segment
 */
void MCToObjTransfer::SetPlgNoInBuffer(const int4 iNum)
{
	m_plgNumber = iNum;
} // SetPlgNoInBuffer()


/**
 *  get the number of vertices in the (character) polygon buffer
 *
 *	@return	the number of the verteices
 */
int4	MCToObjTransfer::GetNumberOfVertices() const
{
	return m_iVertexNum;
} // GetNumberOfVertices()


/**
 *  get the number of normals in the (character) polygon buffer
 *
 *	return	the number of the normals
 */
int4	MCToObjTransfer::GetNumberOfNormals() const
{	
	return m_iNormalNum;
} // GetNumberOfNormals()


/**
 *  get the number of faces in the (character) polygon buffer
 *
 *	@return	the number of the faces
 */
int4	MCToObjTransfer::GetNumberOfFaces() const
{	
	return m_iFaceNum;
} // GetNumberOfFaces()


/**
 *  get the pointer to (character) buffer;
 *
 *	@return	the pointer
 */
int1 * MCToObjTransfer::GetPlgBufPtr() const
{
	return m_pPlgBufC;
} // GetPlgBufPtr()


/** 
 *  A function that transfer the format from MarchingCubes (MC) 
 *  to '.obj' format (supported by wavefront).
 *
 *  @param	fileNo		serial number of the segment;
 *  @param	DepthBuf
 *	@param	SegIdBuf
 *  @param	NormalBuf
 *	@return	0 when success.
 */
int4 MCToObjTransfer::TransferFormat(int4 fileNo, ColonWallPNG * pColonWallPNG)
{
  int4	i, j, k;
  float4 * DepthBuf[6]; 
  uint1 * SegIdBuf[6];
  Triple<float4> * NormalBuf[6];

  for (i=0; i<6; i++) 
  {
    NormalBuf[i] = pColonWallPNG->m_pXYZNormalBuf[i];
    SegIdBuf [i] = pColonWallPNG->m_puXYZSegIdBuf[i];
    DepthBuf [i] = pColonWallPNG->m_pfXYZPlaneDepthBuf[i];
  }

  // a 2D buffer used to increase the efficiency of comparison.
  VBufStruct *VBufPtr;

  // a temporary pointer to the polygon buffer
  float4 *plgPtr = m_pPlgBuf;

  // temporary polygon (face) buffer;
  int4 * fptr = new int4 [m_plgNumber * 3];	
  int4 * face = fptr;

  // temporary vertex buffer.
  float4 ** vpptr = new float4* [m_plgNumber * 3];
  float4 ** vpp = vpptr;
	
  // temporary normal buffer.
  float4 ** npptr = new float4* [m_plgNumber * 3];
  float4 ** npp = npptr;

  int4 vCount = 0;
  int4 fCount = 0;
  int4 serialNum = 1;

  for (int4 nn=0; nn<m_plgNumber; nn++) 
  { 
    // for each polygon, do ...
    for (int4 mm=0; mm<3; mm++) 
    { 
      // there are 3 vertices for each polygon (triangle)
			i = (int4)plgPtr[mm*6];
			j = (int4)plgPtr[mm*6+1];

			if ( m_pVBuf[i][j] == NULL ) 
			{	
				// not allocated, allocate an element in a 2D-array 
				m_pVBuf[i][j] = new VBufStruct [buffSize];
				VBufPtr = m_pVBuf[i][j];
				for (k=0; k<buffSize; k++) 
				{	
					// initialize VBuf
					(VBufPtr + k)->vertNum = -99;
				} // end-for

				// put the first vertex into the V-buffer ...
				VBufPtr->x = plgPtr[mm*6];
				VBufPtr->y = plgPtr[mm*6+1];
				VBufPtr->z = plgPtr[mm*6+2];
				VBufPtr->vertNum = serialNum;

				* vpp = & plgPtr[mm*6];		vpp ++;
				* npp = & plgPtr[mm*6+3];	npp ++;
				vCount ++;
				*fptr = serialNum;
        fptr ++;
        serialNum ++;
      }
      else
      {		
        // comparing ... buffering ...
        int4	found = 0;
        VBufStruct *VBufPtr = m_pVBuf[i][j];
        while (VBufPtr->vertNum != -99) 
        {
          // comparing...
          if ( VBufPtr->x == plgPtr[mm*6]
            && VBufPtr->y == plgPtr[mm*6+1]
            && VBufPtr->z == plgPtr[mm*6+2]) 
          {
            // already buffered (have found the same vertex)
            *fptr = VBufPtr->vertNum;
            fptr ++;
						found = 1;
						break;
					} 
					else 
					{ // not buffered (not found)
						VBufPtr ++;
					} // end-else
				} // endwhile : comparing
				if (!found) 
				{ 
					// buffering ... 
					VBufPtr->x = plgPtr[mm*6];
					VBufPtr->y = plgPtr[mm*6+1];
					VBufPtr->z = plgPtr[mm*6+2];
					VBufPtr->vertNum = serialNum;

					* vpp = & plgPtr[mm*6];		vpp ++;
					* npp = & plgPtr[mm*6+3];	npp ++;
					vCount ++;
					
					// save indexed vertex/normal information into polygon buffer ... 
					*fptr = serialNum;
					fptr ++;
					serialNum ++;
				} // endif: buffering...
			} // endelse comparing .. buffering
		} // end-for (mm)
	  plgPtr += 18;
	  fCount ++;
	} // endfor polygon number

	// buffering DepthBuf, SegIdBuf and NormalBuf for previewable header creation.
	vpp = vpptr;
	npp = npptr;
	for (i=0; i<vCount; i++, vpp++, npp ++) 
	{
		float4 * vptr = * vpp;
		float4 * nptr = * npp;
		
		int4 xx = (int4)(*(vptr) + 0.5);
		int4 yy = (int4)(*(vptr+1) + 0.5);
		int4 zz = (int4)(*(vptr+2) + 0.5);

		// overview from positive Z-direction.
		if ((*(DepthBuf[2] + yy*256 + xx)) < (*(vptr+2))) 
		{
			*(DepthBuf[2] + yy*256 + xx) = *(vptr+2);
			*(SegIdBuf[2] + yy*256 + xx) = static_cast<uint1>(fileNo);
			(NormalBuf[2] + yy*256 + xx)->m_x = *(nptr);
			(NormalBuf[2] + yy*256 + xx)->m_y = *(nptr+1);
			(NormalBuf[2] + yy*256 + xx)->m_z = *(nptr+2);
		}

		// overview from positive Y-direction.
		if ((*(DepthBuf[1] + zz*256 + xx)) < (*(vptr+1))) 
		{
			*(DepthBuf[1] + zz*256 + xx) = *(vptr+1);
			*(SegIdBuf[1] + zz*256 + xx) = static_cast<uint1>(fileNo);
			(NormalBuf[1] + zz*256 + xx)->m_x = *(nptr);
			(NormalBuf[1] + zz*256 + xx)->m_y = *(nptr+1);
			(NormalBuf[1] + zz*256 + xx)->m_z = *(nptr+2);
		}

		// overview from positive X-direction.
		if ((*(DepthBuf[0] + zz*256 + yy)) < (*vptr)) 
		{
			*(DepthBuf[0] + zz*256 + yy) = *vptr;
			*(SegIdBuf[0] + zz*256 + yy) = static_cast<uint1>(fileNo);
			(NormalBuf[0] + zz*256 + yy)->m_x = *(nptr);
			(NormalBuf[0] + zz*256 + yy)->m_y = *(nptr+1);
			(NormalBuf[0] + zz*256 + yy)->m_z = *(nptr+2);
		}

		// overview from negative Z-direction.
		if ((*(DepthBuf[5] + yy*256 + xx)) > (*(vptr+2))) 
		{
			*(DepthBuf[5] + yy*256 + xx) = *(vptr+2);
			*(SegIdBuf[5] + yy*256 + xx) = static_cast<uint1>(fileNo);
			(NormalBuf[5] + yy*256 + xx)->m_x = *(nptr);
			(NormalBuf[5] + yy*256 + xx)->m_y = *(nptr+1);
			(NormalBuf[5] + yy*256 + xx)->m_z = *(nptr+2);
		}
		
		// overview from negative Y-direction.
		if ((*(DepthBuf[4] + zz*256 + xx)) > (*(vptr+1))) 
		{
			*(DepthBuf[4] + zz*256 + xx) = *(vptr+1);
			*(SegIdBuf[4] + zz*256 + xx) = static_cast<uint1>(fileNo);
			(NormalBuf[4] + zz*256 + xx)->m_x = *(nptr);
			(NormalBuf[4] + zz*256 + xx)->m_y = *(nptr+1);
			(NormalBuf[4] + zz*256 + xx)->m_z = *(nptr+2);
		}

		// overview from negative X-direction.
		if ((*(DepthBuf[3] + zz*256 + yy)) > (*vptr)) 
		{
			*(DepthBuf[3] + zz*256 + yy) = *vptr;
			*(SegIdBuf[3] + zz*256 + yy) = static_cast<uint1>(fileNo);
			(NormalBuf[3] + zz*256 + yy)->m_x = *(nptr);
			(NormalBuf[3] + zz*256 + yy)->m_y = *(nptr+1);
			(NormalBuf[3] + zz*256 + yy)->m_z = *(nptr+2);
		}

	} // end-for.

	// saving contents in vertex buffer ...
	// write the results to a temporary file.
	// std::string fileName = rFileNameRoot + ".tri";
	// ofstream tri(fileName.c_str());
  // ofstream tri("triangle.tmp");
	m_pPlgBufC = new int1 [(vCount + vCount + fCount + 10) * 28];	// there are 16 bytes in each line.
	int1 * Ptr = m_pPlgBufC;

	vpp = vpptr;
	for (i=0; i<vCount; i++, vpp++) 
	{
		float4 * vptr = * vpp;
		sprintf(Ptr, "%c %.2f %.2f %.2f", 'v', *vptr, *(vptr+1), *(vptr+2));
		Ptr += 28;
	} // end-for
	delete [] vpptr;

	// save contents in polygon (face) buffer ...
	fptr = face;
	for (i=0; i<fCount; i++) 
	{
  	sprintf(Ptr, "%c %d %d %d", 'f', *fptr, *(fptr+1), *(fptr+2));
		Ptr += 28;
		fptr += 3;
	} // end-for
	delete [] face;

	// saving contents in normal buffer ...
  // tri<<"bind n vertex\n";
	sprintf(Ptr, "%s", "bind n vertex\n");
	Ptr += 28;
	npp = npptr;
	for (i=0; i<vCount; i++, npp++) 
	{
		float4 * nptr = * npp;
  	sprintf(Ptr, "%c %.2f %.2f %.2f", 'n', *nptr, *(nptr+1), *(nptr+2));
		Ptr += 28;
	} // end-for
	delete [] npptr;

	sprintf(Ptr,"%c%c",'#','#');
  
	m_iVertexNum = vCount;
	m_iNormalNum = vCount;
	m_iFaceNum = fCount;

	return 0;
} // TransferFormat()


// $Log: MCToObjTransfer.C,v $
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
//    Rev 2.1   Oct 01 2001 16:10:08   soeren
// removed error class
// Resolution for 1022: Remove Error class
// 
//    Rev 2.0   Sep 16 2001 23:39:02   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:25:32   ingmar
// Initial revision.
// Revision 1.27  2001/07/03 14:59:48  soeren
// removed stdafx.c and .h
//
// Revision 1.26  2001/05/30 15:43:31  binli
// code conventions
//
// Revision 1.25  2001/04/09 17:31:27  dmitry
// updated datatypes
//
// Revision 1.24  2001/04/02 19:18:20  binli
// code simplified!
//
// Revision 1.23  2001/03/30 16:07:10  binli
// fixed bug reported from CA. ---> quadrilateral polygon
//
// Revision 1.22  2001/03/27 19:49:51  binli
// code walkthrough: name convension
//
// Revision 1.21  2001/03/27 15:25:30  binli
// 'const' functions for safety.
//
// Revision 1.20  2001/03/23 20:42:15  binli
// reflected the add of new class ColonWallPNG
//
// Revision 1.19  2001/03/22 21:15:21  binli
// renamed some variables according to new code standard.
//
// Revision 1.18  2001/03/21 20:15:03  binli
// code cleaned.
//
// Revision 1.17  2001/03/15 18:30:21  binli
// code cleaned.
//
// Revision 1.16  2001/02/08 18:43:44  binli
// defect 000186: turn the temporal file triangle.tmp into in-memory buffer
//
// Revision 1.15  2001/01/19 15:48:58  binli
// moved temproal files: triangle.tri.and triangle.smf, from common work directory to their own directory.
//
// Revision 1.14  2001/01/02 22:32:46  binli
// used 2-level pointers to improve the efficiency of data format transfer.
//
// Revision 1.13  2000/12/13 21:59:18  binli
// created PNG header of triangle files.
//
// Revision 1.12  2000/11/15 17:42:54  binli
// cleaning
//
// Revision 1.11  2000/11/15 17:39:50  binli
// enlarged the size of buffer (in some cases, the buffered polygon vertex along a
// stick are possibly more than that estimated before, so more buffer are needed.)
//
// Revision 1.10  2000/11/07 00:14:34  binli
// copied the old version (from mixkit) back to replace 'polygon generation'  in lastest version.
// so as to make the triangle generation modular works.
//
// Revision 1.9  2000/11/06 21:38:54  dave
// added more print stmts, put delete[] back
//
// Revision 1.8  2000/11/05 00:05:52  dave
// fixed one segfault, need to remove or re-write code entirely
//
// Revision 1.7  2000/10/21 21:00:24  ingmar
// BOOL -> bool and other code cleanup
//
// Revision 1.6  2000/10/21 06:06:04  ingmar
// merged mixkit branch back into main trunk
//
// Revision 1.4.10.2  2000/10/19 20:37:01  binli
// code optimization
//
// Revision 1.4.10.1  2000/10/16 18:10:00  binli
// added MixKit.lib
//
// Revision 1.4  2000/09/22 14:42:10  binli
// changed some variables name for walk through
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
// 10    9/06/00 10:17a Binli
// fitted to viatronix standard
// 
// 9     9/05/00 4:21p Binli
// change the sub-pipeline of polygon generation (3) : gap the 'data
// format transfer' submodule and 'getting triangle strips' submodule, so
// no temporary file(s) '.obj' is (are) generated.
// 
// 8     9/01/00 12:49p Binli
// change the sub-pipeline of polygon generation (2) : gap the 'polygon
// generation'
// sub-module and 'data format transfer' sub-module, so no temporary
// file(s)
// '.plg' is(are) generated.
// 
// 7     8/29/00 1:52p Antonio
// updated footer and header
// 
// *****************  Version 6  *****************
// User: Binli           Date:  8/22/00  Time:  2:36p
// Checked in $/v2k/src/Triangles
// Comment:
//   Added Comments
// 
// *****************  Version 5  *****************
// User: Antonio         Date:  8/21/00  Time:  5:51p
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
// User: Binli           Date:  8/09/00  Time:  2:13p
// Checked in $/v2k/src/Triangles
// 
// *****************  Version 2  *****************
// User: Binli           Date:  7/31/00  Time:  9:55p
// Checked in $/v2k/src/Triangles
// Comment:
//   Muiti-Seg preprocessing
// 
// *****************  Version 1  *****************
// User: Binli           Date:  7/31/00  Time:  5:00p
// Created MCToObjTransfer.C
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/MCToObjTransfer.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $
// $Id: MCToObjTransfer.C,v 1.1 2005/04/14 12:50:05 geconomos Exp $
