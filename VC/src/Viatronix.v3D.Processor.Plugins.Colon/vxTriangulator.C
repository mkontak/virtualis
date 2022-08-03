// $Id: vxTriangulator.C,v 1.5 2007/03/06 16:34:01 dongqing Exp $
//
// Copyright© 2000-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// File: vxTriangulator.C
// Description: A class controls the generation of triangle strips
//
// Author: Dongqing Chen dongqing@viatronix.net
//
// Complete History at bottom of file.


#include "stdafx.h"
#include "Timer.h"
#include "ViaQSlim.h"
#include "vxTriangulator.h"
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolumeIteratorNeighbors.h"
#include "ColonWallPNG.h"

using namespace std;

#define FILE_REVISION "$Revision: 1.5 $"

/**
* Contructor : get reference to data volume.
*
* @param pIns
* @param destFilePath
*/
vxTriangulator::vxTriangulator( vxBlockVolume<uint1> *pIns, const std::string & destFilePath )
: m_pIns(pIns), m_sFileNameRoot(destFilePath)
{
  VarMap map(m_pIns->GetHeader().GetVariableVolumeHeader());
  m_numSegments = 0;
  map.GetHdrVar("numSegments", m_numSegments);
} // constructor


/** destructor
 */
vxTriangulator::~vxTriangulator()
{
} // destructor


/**
  *  This does all the works needed to create the triangle set
  *  the work includes: volume simplification, polygon generation, and
  *  getting triangle strips
  *
  *  @return true if success
  */
bool vxTriangulator::Run() 
{ 
  if( m_numSegments == 0 )
  {
    LogErr("No colon segment was found", "Run", "vxTriangulator.C");
	  return(false);
  }
  
  // pre-stage : shrinking volume...
  const Triple<int4>   viDim   ( m_pIns->GetHeader().GetVolDim() );
  Triple<float4>       vfUnits ( m_pIns->GetHeader().GetVoxelUnits() );
  const Triple<int4>   viNewDim(Min(256, viDim.m_x/2), Min(256, viDim.m_y/2),
		                            Min(256, viDim.m_z/2));                                                   
  
  const int4 iX(viNewDim.m_x);
  const int4 iY(viNewDim.m_y);
  const int4 iZ(viNewDim.m_z);
  const int4 iS(iX*viNewDim.m_y);
  const int4 iN(iS*viNewDim.m_z);

  // shrunk INS data buffer
  uint1 * pVolumeData = new uint1[iN];
  if( pVolumeData == NULL )
  {
    LogErr("Out of memory", "Run", "vxTriangulator.C");
	  return(false);
  }

  vxBlockVolumeIterator<uint1> it(*m_pIns);
  vxBlockVolumeIteratorNeighbors<uint1> it6(*m_pIns, (Vector3D<int4>(1,1,1)), vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors06);
  // shrinking the INS volume into the pInsData buffer
  const Triple<float4> vfScale( static_cast<float4>(viNewDim.m_x)/static_cast<float4>(viDim.m_x),
                                static_cast<float4>(viNewDim.m_y)/static_cast<float4>(viDim.m_y),
                                static_cast<float4>(viNewDim.m_z)/static_cast<float4>(viDim.m_z));
  register Vector3D<uint4> vPos;
  register uint1 u;
  register int4  bIsLumen;
  for( it.SetPos(0,0,0); it.IsNotAtEnd(); it.NextBlockZYX() )
  {
    for( ; it.IsNotAtEndOfBlock(); it.NextZYXinsideBlock() )
	  {
	    if( it.GetVoxel() > 0x00 )
	    {
	      u = it.GetVoxel();
	      vPos = it.GetPos();
		    bIsLumen = true;
	    
				for( it6.CenterAt(vPos); !it6.IsAtEnd(); ++it6 )
		      if( it6.GetVoxel() != u )
		      {
		        bIsLumen = false;
			      break;
		      }

	      if(bIsLumen)
		    {
	         vPos.m_x = static_cast<uint4>(vfScale.m_x*(static_cast<float4>(vPos.m_x)+0.5F));
	         vPos.m_y = static_cast<uint4>(vfScale.m_y*(static_cast<float4>(vPos.m_y)+0.5F));
	         vPos.m_z = static_cast<uint4>(vfScale.m_z*(static_cast<float4>(vPos.m_z)+0.5F));

		      pVolumeData[vPos.m_x + vPos.m_y*iX + vPos.m_z*iS] = u;
		    } // if(bIsLumen)
	    } // if( it.GetVoxel() > 0x00 )
	  } // for in the block
  } // for all block
    
  register int4 i;
  for( i=iN-1; i>0; i-- ) 
  {
    pVolumeData[i] = (pVolumeData[i] << 4);	// now the high 5 bits are used
  }
  m_size.m_x = iX;
  m_size.m_y = iY;
  m_size.m_z = iZ;

  // adjust units here in case that the size exceeds 512x512x512.
  if (viDim.m_z > 512)
  {
    vfUnits.m_z = viDim.m_z / 512.0 * vfUnits.m_z;
  }
  if (viDim.m_y > 512)
  {
    vfUnits.m_y = viDim.m_y / 512.0 * vfUnits.m_y;
  }
  if (viDim.m_x > 512)
  {
    vfUnits.m_x = viDim.m_x / 512.0 * vfUnits.m_x;
  }

  float4 * pfPtrToPolygonBuffer;
  int1   * piPtrToPolygonBuffer_C;
  int1   * piSMFBuf;
  
  BSPTreePolygon  * pPolyBSPTreePolygon;
  MCToObjTransfer * pPolyMcToObjTransfer;
  ViaStrip        * pViaStrip;
  ColonWallPNG    * pColonWallPNG = new ColonWallPNG(vfUnits, m_sFileNameRoot);
  stringstream      ss;

  try
  {
    // for each segment, run a sub-pipeline to generate the triangle strips.
    for( i=0; i<m_numSegments; i++ ) 
    {
      ss << "    ("<<i+1<<") triangle extraction     ";
      LogFyi(ss.str(), "vxTriangulator", "Run");  ss.str("");
    
      // form a segment file name.
      string sFileName(m_sFileNameRoot);
      int1 vBuff[10];
      sprintf(vBuff, "_%i.tri", i+1);
      sFileName += vBuff;
    
      // stage 1: generating polygons in MarchingCubes ...
      pPolyBSPTreePolygon = new BSPTreePolygon;
      // set up this value to ensure there is only one branch of BSP-tree generated.
      // the value is the estimated maximum of polygons in a branch.
      const int4 iMaxPolygonInOneBranchOfBSPTree = 999999;
      pPolyBSPTreePolygon->SetPolyNumPerFile(iMaxPolygonInOneBranchOfBSPTree);
    
      // generate polygons.
      pPolyBSPTreePolygon->PolygonGen(pVolumeData, m_size, vfUnits, i+1);
    
      // get the number of polygons in a segment.
      int4 iPolygonNumber = pPolyBSPTreePolygon->GetPolygonNumber();
    
      // if not polygons in this segment, jump to next polygon.
      if (iPolygonNumber <=0) 
      {
        // write out and just write out the head of the .tri file for this segment.
        std::ofstream band(sFileName.c_str(), std::ios::binary | std::ios::out);
        std::string patientId = sFileName.substr(
          sFileName.find_last_of("\\")+1, sFileName.find_last_of("_") - sFileName.find_last_of("\\")-1);
        std::string SegId = sFileName.substr(
          sFileName.find_last_of("_")+1, sFileName.find_last_of(".tri") - sFileName.find_last_of("_")-4);
        uint4 uVerts = 0;
        uint4 uFaces = 0;
        band << "#Patient Id: " << patientId << endl;
        band << "#Segment Id: " << SegId << endl;
        band << "#Vertex Num: " << uVerts << endl;
        band << "#Triangles Num: " << uFaces << endl;
        band.close();
        ss << "No triangle extracted from this segment." << endl;
        LogFyi(ss.str(), "vxTriangulator", "Run" );  ss.str("");
        continue;
      } // end-if (no triangle in this seg.)
    
      // get pointer to the polygon buffer.
      pfPtrToPolygonBuffer = pPolyBSPTreePolygon->GetPtrToPolygonBuffer();
      delete pPolyBSPTreePolygon;
    
      // stage2: transfer data format to .obj that can be processed by 'stripe'.
      pPolyMcToObjTransfer = new MCToObjTransfer;
      // set 'polygonNumber' got in last step (polygon generation).
      pPolyMcToObjTransfer->SetPlgNoInBuffer(iPolygonNumber);
      // set pointer to the polygon buffer got in last step.
      pPolyMcToObjTransfer->SetPtrToPolygonBuffer(pfPtrToPolygonBuffer);
      // do transfer : 2nd parameter indicates how many files will be transferred.
      // for current implementation, just set it to '1'.
      pPolyMcToObjTransfer->DoMcToObjTransfer(i+1, pColonWallPNG);
      // get the pointer to the polygon buffer (after transfer).
      piPtrToPolygonBuffer_C = pPolyMcToObjTransfer->GetPlgBufPtr();
    
      // get the number of vertices, normals and faces in the polygon buffer (after transfer).
      int4 iFaceNum = pPolyMcToObjTransfer->GetNumberOfFaces();
    
      delete pPolyMcToObjTransfer;
      if ( pfPtrToPolygonBuffer ) delete [] pfPtrToPolygonBuffer;
    
      ss << "    ("<<i+1<<") triangle simplification ";
      LogFyi(ss.str(), "vxTriangulator", "Run" );  ss.str("");
      // stage 3: polygon simplification.
      m_pQSlim = new ViaQSlim;
      int4 iPreferredPlgNum;
      if ( iFaceNum > 200000 ) iPreferredPlgNum = (int4)(iFaceNum / 4);
      else if ( iFaceNum > 90000 ) iPreferredPlgNum = iFaceNum / 3;
      else if ( iFaceNum > 8000 )  iPreferredPlgNum = iFaceNum / 2;
      else iPreferredPlgNum = iFaceNum;
    
      // Process command line and read input model(s).
      m_pQSlim->m_sInputPtr = piPtrToPolygonBuffer_C;
      m_pQSlim->ReplacedStartupAndInput(iPreferredPlgNum);
      // Initial simplification process.  Collect contractions and build heap.
      m_pQSlim->SlimInit();
      // Decimate model until target is reached.
      slim->decimate(m_pQSlim->m_iFaceTarget);
      // Output the result.
      piSMFBuf = m_pQSlim->OutputFinalModelSimplified();
      if (piSMFBuf==NULL) 
        throw ex::OutOfMemoryException(LogRec( "vxTriangulator", "Run" ,"memory allocation error" ));
        
      if (piPtrToPolygonBuffer_C) delete [] piPtrToPolygonBuffer_C;
    
      // stage 4: creating triangle strips ...
      pViaStrip = new ViaStrip;
      // set up number of vertices, normals, and faces in the polygon buffer.
      pViaStrip->SetupPara(m_pQSlim->m_iNumVertex, m_pQSlim->m_iNumNormals, m_pQSlim->m_iNumFaces);
      // set pointer to polygon buffer
      pViaStrip->SetPtrToPlgBuf(piSMFBuf);
    
      // clean up.
      m_pQSlim->SlimCleanup();
      delete m_pQSlim;
    
      // create strips.
      pViaStrip->CreateStrips(vfUnits, sFileName);
      delete pViaStrip;
      if ( piSMFBuf ) delete [] piSMFBuf;
    } // end-for the generation procedure

	  delete pVolumeData;
  } // try
  catch( ... )
  {
    LogErr("Fail TRI generation", "Run", "vxTriangulator.C");
	  return(false);
  }
     
  delete [] pColonWallPNG;

  ss << " End of Triangle generation" << endl;
  LogFyi(ss.str(), "vxTriangulator", "Run" );  ss.str(""); 
  
  return(true);
} // Run()

#undef FILE_REVISION

// $Log: vxTriangulator.C,v $
// Revision 1.5  2007/03/06 16:34:01  dongqing
// code review
//
// Revision 1.4  2006/06/02 19:01:16  geconomos
// updated to vs 2005
//
// Revision 1.3  2006/04/26 18:13:35  dongqing
// fix the overview centerline shift problem for studies more than 512 slices
//
// Revision 1.2  2006/04/06 15:08:57  dongqing
// rewrite the iterator
//
// Revision 1.1  2006/03/07 13:30:18  dongqing
// initial check in the TRI stage codes
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/vxTriangulator.C,v 1.5 2007/03/06 16:34:01 dongqing Exp $
// $Id: vxTriangulator.C,v 1.5 2007/03/06 16:34:01 dongqing Exp $
