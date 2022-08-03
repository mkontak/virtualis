// $Id: ViaStripe.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// File: ViaStripe.C
// Description: implementation (2/17) of the ViaStrip class.
// Created: July 31, 2000
// Author: Freeware
//         Rewrited by Bin Li (binli@viatronix.net), who change its C version to C++ version
//
// Complete History at bottom of file.


#include "stdafx.h"
#include "ViaStrip.h"

using namespace std;

#define FILE_REVISION "$Revision: 1.2 $"

/**
 *  Allocate structures for the storage of vertices, normals, and faces
 *
 *  @param num_faces    the number of faces in the polygon buffer
 *  @param num_vert     the number of vertices in the polygon buffer
 *  @param num_nvert    the number of normals in the polgyon buffer
 *  @param num_texture  the number of texture coordinatates in the polygon buffer
 */
void ViaStrip::AllocateStruct(int4 num_faces, int4 num_vert, int4 num_nvert)
{
	StartFaceStruct(num_faces);
  StartVertexStruct(num_vert);
	m_pVertices = new Triple<float8> [num_vert];
  if (num_nvert > 0) 
	{
		m_pNVertices = new Triple<float8> [num_nvert];
    m_pVertNorms = new int4 [num_vert];

		// Initialize entries to zero, in case there are 2 hits
		// to the same vertex we will know it - used for determining
		// the normal difference
		InitVertNorms(num_vert);
	} 
	else 
	{
 		m_pNVertices = NULL;
	} // end-if

	// Set up the temporary 'p' pointers
	m_pPVertices = m_pVertices;
	m_pPNVertices = m_pNVertices;
} // AllocateStruct().


/**
 *  get polygon descriptions from buffer and transfer it to triangle strips format
 *  kernel of this class
 */
void ViaStrip::ReadFromBuf(int4 num_vert, int4 num_nvert, int4 num_faces, 
                           int4 *cost, std::ofstream & band)
{
  int1 *ptr, *ptr2;
  int4 num2, face_id=0;
  int4 vertex, vert_count, loop, temp[MAX1];
  float4 vertexBuf[3], normalBuf[3];

  int1 * PtrToBuf = GetPtrToBuf();
  ptr = PtrToBuf;

  // for each line of polygon info, do strips transfer
  do {
    if (ptr[0]=='#' && ptr[1]=='#') break;
	
    // Load in vertices/normals
    if (*ptr == 'v' || *ptr == 'n') 
    {
      if (*(ptr)=='n') 
      { 
				// at normal
				sscanf(ptr+2,"%lf%lf%lf", &(m_pPNVertices->m_x), &(m_pPNVertices->m_y), &(m_pPNVertices->m_z));
				// 'band' write:
				band << "n";
				normalBuf[0] = m_pPNVertices->m_x;
				normalBuf[1] = m_pPNVertices->m_y;
				normalBuf[2] = m_pPNVertices->m_z;
				band.write((int1 *)(& normalBuf[0]), sizeof(float4)*3);
				band << endl;
				++ m_pPNVertices;
			} 
			else 
			{
				// at vertex: scale it to world coordinate system.
				sscanf(ptr+2,"%lf%lf%lf", &(m_pPVertices->m_x), &(m_pPVertices->m_y), &(m_pPVertices->m_z));
				// 'band' write:
				band << "v";
				vertexBuf[0] = (float4)(m_pPVertices->m_x * m_units.m_x * 2.0);
				vertexBuf[1] = (float4)(m_pPVertices->m_y * m_units.m_y * 2.0);
				vertexBuf[2] = (float4)(m_pPVertices->m_z * m_units.m_z * 2.0);
				band.write((int1 *) (& vertexBuf[0]), sizeof(float4)*3);
				
        band << endl;
        ++ m_pPVertices;
      }
    } // endif (processsing vertex(v) and normal (vn)
    else if (*ptr == 'f') 
    { 
      // at a face (polygon): the number, 'num2', of vertices in a polygon is always 3 in this application.
			num2 = 3;
      face_id ++;
      ptr2 = ptr+1;
      
			// loop on the number of numbers in this line of face data
      vert_count = 0;
			for (loop=0; loop<num2; loop++) 
			{ // skip the whitespace
				while (*ptr2<'0' || *ptr2>'9') 
				{
					if (*ptr2 == '-') break;
					ptr2++;
				}
				vertex = atoi(ptr2)-1;
				if (vertex < 0) 
				{
					vertex = num_vert + vertex;
					*ptr2 = ' ';
					ptr2++;
				}
                
        // no nvertices
        temp[vert_count] = vertex ;
        vert_count++;

        if (!(AddVertId(vertex,vert_count)))
          vert_count--;
        
        while (*ptr2>='0' && *ptr2<='9') ptr2++;
      } // endfor (loop)
			// add it to face structure
			if (vert_count >= 3)
				AddNewFace(m_IDs, vert_count, face_id, m_iNorms);
			else
				face_id--;
		} // endif (faces)
		ptr += 64;
	} while (true);
	// endwhile (each line of the polygon file)
	// Done reading in all the information into data structures
	num_faces = face_id;
	delete [] m_pVertices;
  delete [] m_pNVertices;
} // ReadFromBuf().


/**
 * add header to the triangle file
 *
 * @param filename	triangle file name
 * @param band			output stream
 */
void ViaStrip::AddHeader(const std::string & filename, std::ofstream & band, int4 verts, int4 faces)
{
  std::string patientId = filename.substr(
    filename.find_last_of("\\")+1, filename.find_last_of("_") - filename.find_last_of("\\")-1);
	
  std::string SegId = filename.substr(
    filename.find_last_of("_")+1, filename.find_last_of(".tri")-filename.find_last_of("_")-4);

  band << "#Patient Id: " << patientId << endl;
  band << "#Segment Id: " << SegId << endl;
  band << "#Vertex Num: " << verts << endl;
  band << "#Triangles Num: " << faces << endl;
} // AddHeader()


/**
 * do transfer here to get triangle strips
 *
 * @param fname     the root of the output file name
 */
void ViaStrip::CreateStrips(Triple<float4> & rUnits, const std::string & fileName)
{
  // retrieve the units of this data volume.
  m_units = rUnits;

  int4 num_vert, num_nvert, num_faces;
  bool costPositive = false;

	// Interpret the options specified
  int4 tie = 0;
  int4 triangle = 0;
  int4 cost = 0;
	num_vert  = m_iVertexNum;
	num_nvert = m_iNormalNum;
	num_faces = m_iFaceNum;

  std::ofstream band(fileName.c_str(), std::ios::binary | std::ios::out);
  AddHeader(fileName, band, num_vert, num_faces);
  if ( !band )
  {
    std::string ErrorInfo("Error create strips: Unable to open strips file \"");
    ErrorInfo += fileName;
    ErrorInfo += "\" for writing.";
    throw ex::FileNotFoundException(LogRec( "ViaStrip", "CreateStrips", "Error create strips: Unable to open strips file." ));
  } // bad file.

  // allocate data structure for the storage of verts,normals and faces (polygons)
  AllocateStruct(num_faces,num_vert,num_nvert);
  // Load the object into memory
  ReadFromBuf(num_vert, num_nvert, num_faces, &cost, band);
  StartEdgeStruct(num_vert);
  FindAdjacencies(num_faces);
  // Initialize it
  m_pFaceArray = NULL;
  InitTableSGI(num_faces);
  // Build it
  Build_SGI_Table(num_faces);
  InitStripTable();
  SGIStrip(num_faces, tie, triangle, band);

  // Get the total cost
  if (cost > 0) 
    costPositive = true;
  else 
    costPositive = false;
	
  OutputTriEx(-1,-2,-3, -20, costPositive, band);
  EndFaceStruct(num_faces);
  EndEdgeStruct(num_vert);

  band << "\n#";
  band.close();

  return;
} // CreateStrips()


// $Log: ViaStripe.C,v $
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
//    Rev 1.0   Sep 16 2001 17:25:44   ingmar
// Initial revision.
// Revision 1.40  2001/08/13 20:29:19  sachin
// changed exceptions ,warnings,debug and information
// according to the new logger class
//
// Revision 1.39  2001/07/25 16:16:42  lihong
// Defect Id: 599
// Added throw ex:: after the function name
//
// Revision 1.38  2001/07/24 23:06:50  lihong
// Defect ID: 599
// new format of exceptions
//
// Revision 1.37  2001/07/23 16:56:27  lihong
// Defect ID: 599
// replaced file version with "$Revision: 1.2 $"
//
// Revision 1.36  2001/07/20 23:07:09  lihong
// Defect ID: 599
// changed abort() to throw ex
//
// Revision 1.35  2001/07/03 14:59:48  soeren
// removed stdafx.c and .h
//
// Revision 1.34  2001/05/21 00:22:54  dave
// uses & implement. of Error::DebugMsg, Warn, Abort, etc, gone
//
// Revision 1.33  2001/04/09 17:31:25  dmitry
// updated datatypes
//
// Revision 1.32  2001/04/02 19:18:21  binli
// code simplified!
//
// Revision 1.31  2001/03/27 19:49:51  binli
// code walkthrough: name convension
//
// Revision 1.30  2001/03/27 16:55:54  binli
// reflected new code standard.
//
// Revision 1.29  2001/03/23 20:41:20  binli
// code walkthrough
//
// Revision 1.28  2001/03/22 21:15:22  binli
// renamed some variables according to new code standard.
//
// Revision 1.27  2001/03/21 17:15:55  binli
// reflected changes made below:
//   1> replaced triple variable with standard Triple.
//   2> changed struct Strips to class Strips.
//
// Revision 1.26  2001/03/07 19:57:21  binli
// malloc/free replaced by new/delete
//
// Revision 1.25  2001/02/28 21:01:59  binli
// 1> BOOL->bool
// 2> Error Info
//
// Revision 1.24  2001/02/27 15:33:35  binli
// code compression again.
//
// Revision 1.23  2001/02/09 18:24:43  binli
// // defect 000186: turn the temporal file 'triangle.smf' into in-memory buffer
//
// Revision 1.22  2001/01/19 15:48:59  binli
// moved temproal files: triangle.tri.and triangle.smf, from common work directory to their own directory.
//
// Revision 1.21  2001/01/02 18:00:01  binli
// fixed bug of 'zero' triangle in a segment
//
// Revision 1.20  2000/12/11 19:41:33  binli
// applied double operation to the output  of 'world coordinate' described triangle files.
// --- no twice scale is needed in v1k.
//
// Revision 1.19  2000/12/08 21:16:41  binli
// removed code to write out 'text' triangle files.
//
// Revision 1.18  2000/12/08 16:56:18  binli
// changed the output of triangle files from text to binary.
// added header to triangle files.
//
// Revision 1.17  2000/12/04 18:42:12  binli
// write out triangles in World coordinates.
//
// Revision 1.16  2000/11/15 04:41:43  ingmar
// put back BOOL defined as int
//
// Revision 1.15  2000/11/10 16:51:25  dongqing
// Ingmar checking in on Dongqing's machine the close file fix that dave made and Bin Li approved
//
// Revision 1.14  2000/11/09 22:07:55  binli
// tried to fix the bug in 'strips creation'. 4 data sets tested. No bugs found!
//
// Revision 1.13  2000/11/06 23:57:10  binli
// copied the old version (from mixkit) back to replace 'polygon generation'  in lastest version.
// so as to make the triangle generation modular works.
//
// Revision 1.12  2000/11/06 03:59:19  dave
// convert back to real bool
//
// Revision 1.11  2000/11/05 00:05:52  dave
// fixed one segfault, need to remove or re-write code entirely
//
// Revision 1.10  2000/11/02 07:35:27  antonio
// added boolean variable costPositive to ViaStrip::DoStripe,and made g into bool.
//
// Revision 1.9  2000/10/21 21:00:24  ingmar
// BOOL -> bool and other code cleanup
//
// Revision 1.8  2000/10/21 20:29:30  ingmar
// filenaming cleanup
//
// Revision 1.7  2000/10/21 06:06:04  ingmar
// merged mixkit branch back into main trunk
//
// Revision 1.4.10.3  2000/10/20 15:29:56  binli
// fixed bug of 'hole' in surface.
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
// 11    9/07/00 12:11p Binli
// fitted to viatronix standard
//
// 10    9/05/00 4:21p Binli
// change the sub-pipeline of polygon generation (3) : gap the 'data
// format transfer' submodule and 'getting triangle strips' submodule, so
// no temporary file(s) '.obj' is (are) generated.
//
// 9     8/31/00 4:42p Binli
// change the sub-pipeline of polygon generation (1) : gap the 'shrink'
// sub-module and 'polygon generation' sub-module, so no temporary file(s)
// '.shr' is(are) generated.
//
// 8     8/29/00 1:10p Antonio
// updated footer
//
// *****************  Version 7  *****************
// User: Binli           Date:  8/23/00  Time: 10:34a
// Checked in $/v2k/src/Triangles
// Comment:
//   Added comments
//
// *****************  Version 6  *****************
// User: Binli           Date:  8/22/00  Time:  3:11p
// Checked in $/v2k/src/Triangles
//
// *****************  Version 5  *****************
// User: Binli           Date:  8/22/00  Time:  3:03p
// Checked in $/v2k/src/Triangles
// Comment:
//   Added comments
//
// *****************  Version 4  *****************
// User: Antonio         Date:  8/21/00  Time:  6:17p
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
// User: Binli           Date:  8/01/00  Time: 10:21a
// Checked in $/v2k/src/Triangles
//
// *****************  Version 1  *****************
// User: Binli           Date:  7/31/00  Time:  5:13p
// Created ViaStripe.C
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/ViaStripe.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
// $Id: ViaStripe.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
