// $Id: ViaLocal.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ViaLocal.C  
// Description: Implementation (10/17) of Class ViaStrip 
// Created: July 31, 2000 
// Author:  Freeware 
//			    Rewriten by Bin Li (binli@viatronix.net)
//
// Complete History at bottom of file.

#include "stdafx.h"
#include "ViaStrip.h"

#define FILE_REVISION "$Revision: 1.2 $"

/**
 *	This routine will peel off the strips from the model
 */
void ViaStrip::FindStripsEx(int4 *ties, int4 tie, int4 triangulate, int4 swaps,
                            int4 *next_id, std::ofstream & band)
{
	// This routine will peel off the strips from the model
	ListHead * pListHead;
	ADJACENCIES * temp = NULL;
	register int4 max,bucket=0;
	bool whole_flag = true;
	int4 dummy = 0;
  
	// Set the last known input edge to be null
	LastEdge(&dummy,&dummy,&dummy,1);
  
	// Search for lowest adjacency polygon and output strips
	while (whole_flag) 
	{
		bucket = -1;
		// Search for polygons in increasing number of adjacencies
		while (bucket < 59) 
		{
			bucket++;
			pListHead = m_pArray[bucket];
			max = NumOnList(pListHead);
			if (max > 0) 
			{
				temp = (ADJACENCIES *) PeekList(pListHead,LISTHEAD,0);
				if (temp == NULL) 
        {
          int1 buf[100];
          sprintf(buf, "Create strips:: Error in the buckets%d %d %d\n",bucket,max,0); 

          throw ex::AbortDataProcessingException(LogRec("ViaStrip", "FindStripsEx", "Error in the buckets." ));
        }

				//iRecursiveCallCounter = 0;

				PolygonOutputEx(temp,temp->face_id,bucket,pListHead,
						ties,tie,triangulate,swaps,next_id,1, band);
				  // Try to extend backwards, if the starting polygon in the
				  // strip had 2 or more adjacencies to begin with

				if (bucket >= 2)
					ExtendBackwardsEx(temp->face_id, ties,tie, triangulate,swaps,next_id, band);

				break;
			}
		}

		// Went through the whole structure, it is empty and we are done. 
		if ((bucket == 59) && (max == 0))
			whole_flag = false;
      
		// We just finished a strip, send dummy data to signal the end
		// of the strip so that we can output it. 
		else 
		{
			OutputTriEx(-1,-2,-3, -10,1, band);

			LastEdge(&dummy,&dummy,&dummy,1);
		}
	}
}


/**	
 *	Build a table that has all the polygons sorted by the number
 *	of polygons adjacent to it.
 */
void ViaStrip::SGIStrip(int4 num_faces, int4 ties, int4 triangulate,
												 std::ofstream & band)
{
	int4 next_id = -1,t=0;
  
	// We are going to output and find triangle strips
  // according the the method that SGI uses, ie always
  // choosing as the next triangle in our strip the triangle
  // that has the least number of adjacencies. We do not have
  // all triangles and will be triangulating on the fly those
  // polygons that have more than 3 sides.
  
	// Build a table that has all the polygons sorted by the number
  // of polygons adjacent to it.
		
	/// Initialize it
	InitTableSGI(num_faces);
	// Build it
	Build_SGI_Table(num_faces);
  
	// We will have a structure to hold all the strips, until outputted.
  
	InitStripTable();
	
	// Now we have the structure built to find the polygons according
  // to the number of adjacencies. Now use the SGI Method to find
  // strips according to the adjacencies
	FindStripsEx(&t,ties,triangulate, ON, &next_id, band);
}

// $Log: ViaLocal.C,v $
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
//    Rev 2.0   Sep 16 2001 23:39:06   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:25:38   ingmar
// Initial revision.
// Revision 1.20  2001/08/13 20:29:19  sachin
// changed exceptions ,warnings,debug and information
// according to the new logger class
//
// Revision 1.19  2001/07/25 16:08:39  lihong
// Defect Id: 599
// Added throw ex:: after the function name
//
// Revision 1.18  2001/07/24 23:06:51  lihong
// Defect ID: 599
// new format of exceptions
//
// Revision 1.17  2001/07/23 16:56:27  lihong
// Defect ID: 599
// replaced file version with "$Revision: 1.2 $"
//
// Revision 1.16  2001/07/20 23:07:10  lihong
// Defect ID: 599
// changed abort() to throw ex
//
// Revision 1.15  2001/05/21 00:22:53  dave
// uses & implement. of Error::DebugMsg, Warn, Abort, etc, gone
//
// Revision 1.14  2001/04/09 17:31:26  dmitry
// updated datatypes
//
// Revision 1.13  2001/04/02 19:18:20  binli
// code simplified!
//
// Revision 1.12  2001/03/27 19:49:51  binli
// code walkthrough: name convension
//
// Revision 1.11  2001/03/27 16:55:54  binli
// reflected new code standard.
//
// Revision 1.10  2001/03/23 20:41:19  binli
// code walkthrough
//
// Revision 1.9  2001/03/22 21:15:21  binli
// renamed some variables according to new code standard.
//
// Revision 1.8  2001/03/21 15:24:11  binli
// replaced all structs with classes.
//
// Revision 1.7  2001/02/28 21:01:59  binli
// 1> BOOL->bool
// 2> Error Info
//
// Revision 1.6  2000/12/08 21:16:41  binli
// removed code to write out 'text' triangle files.
//
// Revision 1.5  2000/12/08 16:56:18  binli
// changed the output of triangle files from text to binary.
// added header to triangle files.
//
// Revision 1.4  2000/11/06 23:57:49  binli
// copied the old version (from mixkit) back to replace 'polygon generation'  in lastest version.
// so as to make the triangle generation modular works.
//
// Revision 1.3  2000/10/21 21:19:41  ingmar
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
// 5     8/29/00 1:41p Antonio
// updated header footer
// 
// *****************  Version 4  *****************
// User: Binli           Date:  8/24/00  Time: 12:30p
// Checked in $/v2k/src/Triangles
// Comment:
//   Added comments to Class ViaStrip
// 
// *****************  Version 3  *****************
// User: Antonio         Date:  8/21/00  Time:  6:02p
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
// Created ViaLocal.C
// 
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/ViaLocal.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
// $Id: ViaLocal.C,v 1.2 2005/07/01 12:46:05 vxconfig Exp $
