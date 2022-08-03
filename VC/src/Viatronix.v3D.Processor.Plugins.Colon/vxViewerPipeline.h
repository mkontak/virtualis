// $Id: vxViewerPipeline.h,v 1.0 2011/04/21  dongqing Exp $
//
// Copyright(c) 2000-2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Dongqing Chen  dongqing@viatornix.com
//
// Complete History at bottom of file.


#ifndef vxViewerPipeline_h
#define vxViewerPipeline_h

#include "Global.h"
#include "Thread.h"
#include "Volume.h"
#include "vxVolumeHeader.h"
#include "vxBlockVolume.h"
#include "SegmentList.h"


/**
* The virtual colonoscopy backend processing Pipeline.
* 
* The Pipeline connects all automatic preprocessing stages of the Viatron 2000 together
* and hands the data from one stage to the other.
* The Pipeline further enables running the complete preprocessing as well as
* just individual steps to allow a technician to track down unusual behavior.
*/
class vxViewerPipeline
{
  // member functions
public:

  /// Constructor with commadline parameters                         
  vxViewerPipeline();
  
	/// destructor
	~vxViewerPipeline();

	/// read volume
	void ReadVolume( vxBlockVolume<uint2> * pVol, const std::string & sFilename );

	/// get the image orientation fromt the DICOM header (orientation cosine)
	void ComputeImageOrientation();

  /// format the linear volume from new to old volume layout
  void PrepareLinearVolume( vxBlockVolume<uint2> * pVol, uint2 * pLVol );

	/// check if the intensity range is abnormal
	bool CheckIntensityRange( uint2 * pLVol );

	/// extract body region
	void ComputeBodyRegion( uint2 * pVol, uint1 * pMask );

	/// count tagged voxel and clamp HU value
	void CountTaggedVoxel( uint2 * pVol, uint1 * pMask );

	/// remove bone from body region
	void RemoveBone( uint2 * pVol, uint1 * pMask );

	/// apply electronic cleansing
	void ComputeCRES( uint2 * pVol, uint1 * pMask );

	/// reformate the linear and mask volume back into new block volume layout
	void ReformatBack2NewLayout( uint2 * pVol, uint1 * pMask );

	/// compute inside colon segments
	void ComputeInsideSegments( uint2 * pVol, uint1 * pMask, uint1 * pIns );

	/// get the updated block volume header
	vxVolumeHeader GetVolumeHeader() { return( m_vHeader ); }

	/// get the updated block volume header
	std::string GetImageOrientation() { return( m_sImageOrientation ); }

	/// compute the non-cleansed INS volume
	void ComputeUncleansedINS( vxBlockVolume<uint2> * pv, uint1 * pIns, uint1 * pInsNoec );

	/// compute SLD volume and TRI
	void ComputeSLDandTRI( uint1 * pMask, uint1 * pIns, uint1 * pInsNoec, 
		uint1 * pSld, uint1 * pUld, std::string & sTemporaryDirectory );

	/// compute PDF and CSI
	void ComputePDFandCSI( vxBlockVolume<uint1> * pIns, uint1 * pSld,
		vxBlockVolume<float4> * pPdr0, vxBlockVolume<float4> * pdr1, vxBlockVolume<uint2> * pCsi,
		std::string & sTemporaryDirectory );


  // member variables
private:
	
	/// series body position info containor
	std::string m_sImageOrientation;

  /// colon residues percent threshold for skip CRES
  float4 m_fResiduePercent;

	/// original input block volume header
  vxVolumeHeader m_vHeader;

	/// volume dimension constants
	int4 m_iX;
	int4 m_iY;
	int4 m_iZ;
	int4 m_iS;
	int4 m_iN;


  // member classes
private:

	/// thread implementation for volume copy, reformat, and clamping from block to linear
	class PrepareLinearVolThread : public Thread
	{
	public:
		/// construction
		PrepareLinearVolThread( vxBlockVolume<uint2> * pVol, uint2 * pLVol, const int4 & iZMin, 
			                      const int4 & iZMax, const std::string & sImageOrientaiton );
		
		/// member function
	public:
		/// Thread run
		virtual void Run();

		/// member variables
	private:
		/// original block volume
		vxBlockVolume<uint2> * m_pVol;

		/// new linear volume
		uint2 * m_pLVol;

		/// slice index bounds
		const int4 & m_iZMin;
		const int4 & m_iZMax;

		/// image orientation information
		const std::string & m_sImageOrientation;
	}; // class HandleVolumeThread


	///thread implementation for counting the number of tagged voxels
	class CountTaggedVoxelThread : public Thread
	{
	public:
		/// construction
		CountTaggedVoxelThread( vxVolumeHeader & vHeader, uint2 *pVol, uint1 * pMask, const int4 & iStart, const int4 & iEnd, int4 & iNumOfTaggedVoxel );
		
		/// member function
	public:
		/// Thread run
		virtual void Run();

		/// member variables
	private:
		/// header of the block volume
		vxVolumeHeader & m_vHeader;

		/// original linear volume
		uint2 * m_pVol;

		/// mask volume
		uint1 * m_pMask;

		/// counter of body voxels
		int4 & m_iCounter;

		/// volume offset bounds
		const int4 & m_iStart;
		const int4 & m_iEnd;

	}; // class CountTaggedVoxelThread


	/// thread implementation for reformating the linear volume into new block volume layout
	class ReformatBack2NewLayoutThread : public Thread
	{
	public:
		/// construction
		ReformatBack2NewLayoutThread( vxVolumeHeader & vHeader, uint2 * pVol, uint1 * pMask, const int4 & iZMin, 
			                      const int4 & iZMax, const std::string & sImageOrientaiton );
		
		/// member function
	public:
		/// Thread run
		virtual void Run();

		/// member variables
	private:
		/// the block volume header
		vxVolumeHeader & m_vHeader;

		/// pointer to the linear intensity volume
		uint2 * m_pVol;

		/// pointer to the mask volume
		uint1 * m_pMask;

		/// slice index bounds
		const int4 & m_iZMin;
		const int4 & m_iZMax;

		/// image orientation information
		const std::string & m_sImageOrientation;
	}; // class ReformatBack2NewLayoutThread

}; // end of Pipeline

#endif // vxViewerPipeline_h


// $Log: vxViewerPipeline.h,v $
// Revision 1.0 2011/04/21   dongqing
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.Colon/vxViewerPipeline.h,v 1.0 2011/04/21  dongqing Exp $
// $Id: vxViewerPipeline.h,v 1.0 2011/04/21  dongqing Exp $
