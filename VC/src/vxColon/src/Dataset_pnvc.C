// $Id: Dataset_Processor.C,v 1.0  2011/04/21 dongqing Exp $
//
// Copyright © 2000-2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Dongqing Chen ( dongqing@viatronix.com )


/**
 * This file provide entire vc processing steps 
 */


// includes
#include "StdAfx.h"
#include "Dataset.h"
#include "FileExtensions.h"
#include "StudyLoad.h"
#include "File.h"
#include "ReaderGlobal.h"
#include "Thresholds.h"
#include "Typedef.h"
#include "HUConverter.h"
#include "vxVolumeHeader.h"
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolumeIteratorNeighborsConsts.h"
#include "vxBlockVolumeIteratorNeighbors.h"
#include "vxBlockVolumeFileReader.h"
#include "vxHistogramUpdaterIntensity.h"
#include "vxIntensityConverter.h"

#include "Timer.h"

// namespaces
using namespace std;
using namespace vx;
using namespace ReaderLib;

// defines
#define FILE_REVISION "$Revision: 1.0 $"


/**
 *  Load input volume file for 2D display first
 *
 *  @param pv                   the pointer to the block volume
 *  @param pipeline             the pipeline instance for preprocessing
 *  @param loadCallback         load step notify callback method
 *  @param bSetZoomFactor       whether the overview zoom has already been calculated
 */
void Dataset::LoadVolumeOnly( vxBlockVolume<uint2> * pv, vxViewerPipeline & pipeline, StudyLoad::LoadCallback & loadCallback, bool & bSetZoomFactor )
{
  try
  { 
		//Timer stageTime;

		// ========= Step 1: Load input volume ====================				
    m_bVolumeLoaded = false;
    m_bNeedVisualizationReinit = true;

		std::string sFilename = m_sVolumePrefix + vx::VolumeExt;
		
		if( !vx::File(sFilename).Exists() )
    {
  		throw ex::AbortLoadException(LogRec( "    Volume file is missing", "Dataset_Preprocessing", "LoadVolumeOnly") );
    }
		
		pipeline.ReadVolume( pv, sFilename );

		pipeline.ComputeImageOrientation();

		// Load the volume header information
		m_volumeHeader = pv->GetHeader();

    std::string sModality;
    m_volumeHeader.GetHeaderEntry( vxVolumeHeader::DataModality, sModality );
    if (sModality == "CT" )
    {
			m_eDataModality = Dataset::CT;
		} // if
		else
		{
			throw ex::AbortLoadException(LogRec( "    Image modality is not CT", "Dataset_Preprocessing", "LoadVolumeOnly") );
		}

		m_vUnits = m_volumeHeader.GetVoxelUnits();
    m_vDim   = m_volumeHeader.GetVolDim();
    m_vOrthogonalPtWorld = Point<float4>( m_vDim.m_x * m_vDim.m_x, m_vDim.m_y * m_vDim.m_y, m_vDim.m_z * m_vDim.m_z) * 0.5f;

    if (bSetZoomFactor)
    {
      rdrGlobal.SetupZoomFactorInOverview( m_vDim, m_vUnits );
      bSetZoomFactor = false;
    }
		
    if (rdrGlobal.m_bAbortLoad == true) 
			throw ex::AbortLoadException(LogRec("    Loading aborted by user", "Dataset_Processor.C", "LoadVolumeOnly"));

		m_fields.CreateVolumeOnly( pv, pipeline.GetImageOrientation() ); 

		//LogFyi( "    Load volume took " + stageTime.AsString(), "Dataset_Processor.C", "LoadVolumeOnly" );
  } // endtry

  catch (ex::AbortLoadException &) 
	{ 
		throw; 
	}
  catch (ex::VException &) 
	{ 
		throw; 
	}
  catch (...) 
	{ 
		throw ex::VException(LogRec("    Error occurred in loading volume", "Dataset_Processor.C", "LoadVolumeOnly")); 
	}
} // LoadVolumeOnly


/**
 *  start the viewer processor to create 3D model
 *
 *  @param pv                   the pointer to the block volume
 *  @param pipeline             the pipeline instance for preprocessing
 *  @param loadCallback         load step notify callback method
 *  @param series               string information on series loaded
 */
void Dataset::Preprocessing( vxBlockVolume<uint2> * pv, vxViewerPipeline & pipeline, StudyLoad::LoadCallback & loadCallback, std::string & series )
{
  try
  { 
		Timer totalTime, stageTime;

		std::string ss = m_sVolumePrefix + vx::VolumeExt;
		
    if (rdrGlobal.m_bAbortLoad == true) 
			throw ex::AbortLoadException(LogRec("    Loading aborted by user", "Dataset_Processor.C", "Preprocessing"));

		const int4 iX( m_vDim.m_x );
		const int4 iY( m_vDim.m_y );
		const int4 iZ( m_vDim.m_z );
		const int4 iS( iX*iY );
		const int4 iN( iS*iZ );

    const Triple<float4> vUnits( pv->GetHeader().GetVoxelUnits() );
    if( (vUnits.m_z > 1.5)||(vUnits.m_y > 1.0)||(vUnits.m_x > 1.0) )
		{ /// provide warning 
			LogWrn("    Slice thickness is greater than nomal.", "Dataset_Processor.C", "Processing" );
		}

		// ========= Step 2: make an linear volume =================
		/// alocate memory for the working linear volume
		uint2 * plv = new uint2[iN];
		if( plv == NULL )
		{
			throw ex::OutOfMemoryException( LogRec("    Out of memory", "Dataset_Processor.C", "Processing") );
		}

		pipeline.PrepareLinearVolume( pv, plv );

		if( !pipeline.CheckIntensityRange( plv ) )
		{ /// This may be caused by error background removal, imaging noise, or inproper FOV setup
		  LogErr( "    Abnormal intensity range.", "Dataset_Processor.C", "Processing" );
		}

		//LogFyi( "    Preparing the linear volume took " + stageTime.AsString(), "Dataset_Processor.C", "Processing" );
		//stageTime.Reset(); //--------------------------------------------

    if (rdrGlobal.m_bAbortLoad == true) 
			throw ex::AbortLoadException(LogRec("    Loading aborted by user", "Dataset_Processor.C", "Preprocessing"));

		// ========= Step 3: remove background and extract body region ===========
		uint1 * pMask = new uint1[iN];
		if( pMask == NULL )
		{
			throw ex::AbortLoadException( LogRec("    Fail to allocate memory for mask volume", "Dataset_Processor.C", "Processing") );
		}

		pipeline.ComputeBodyRegion( plv, pMask );

		//LogFyi( "    Remove background took " + stageTime.AsString(), "Dataset_Processor.C", "Processing" );
		//stageTime.Reset(); //------------------------------------------------------

    if (rdrGlobal.m_bAbortLoad == true) 
			throw ex::AbortLoadException(LogRec("    Loading aborted by user", "Dataset_Processor.C", "Preprocessing"));

		// ========= Step 4: Count tagged voxel and clamp HU value ================
		pipeline.CountTaggedVoxel( plv, pMask );

		//LogFyi( "    Count tagged voxel took " + stageTime.AsString(), "Dataset_Processor.C", "Processing" );
		//stageTime.Reset(); //------------------------------------------------------

    if (rdrGlobal.m_bAbortLoad == true) 
			throw ex::AbortLoadException(LogRec("    Loading aborted by user", "Dataset_Processor.C", "Processing"));

		// ========= Step 5: Remove bone ==========================
		pipeline.RemoveBone( plv, pMask );

		//LogFyi( "    Removal of bone took " + stageTime.AsString(), "Dataset_Processor.C", "Processing" );
		stageTime.Reset(); //--------------------------------------

    if (rdrGlobal.m_bAbortLoad == true) 
			throw ex::AbortLoadException(LogRec("    Loading aborted by user", "Dataset_Processor.C", "Processing"));

		// ========= Step 6: apply electronic cleansing ==========================
		pipeline.ComputeCRES( plv, pMask );

		LogFyi( "    Compute CRES took " + stageTime.AsString(), "Dataset_Processor.C", "Processing" );
		//stageTime.Reset(); //-----------------------------------------------------

    if (rdrGlobal.m_bAbortLoad == true) 
			throw ex::AbortLoadException(LogRec("    Loading aborted by user", "Dataset_Processor.C", "Preprocessing"));

		// ========= Step 7: Reformat linear volumes ==========================
		pipeline.ReformatBack2NewLayout( plv, pMask );

		//LogFyi( "    Reformat volumes back took " + stageTime.AsString(), "Dataset_Processor.C", "Processing" );
		stageTime.Reset(); //-----------------------------------------------------

    if (rdrGlobal.m_bAbortLoad == true) 
			throw ex::AbortLoadException(LogRec("    Loading aborted by user", "Dataset_Processor.C", "Processing"));

		// ========= Step 8: compute inside colon segments ==========================
		uint1 *pIns = new uint1[iN];
		if( pIns == NULL )
		{
			throw ex::OutOfMemoryException( LogRec("    Out of memory", "Dataset_Processor.C", "Processing") );
		} // if

		pipeline.ComputeInsideSegments( plv, pMask, pIns );

    // Set rendering thresholds. It is fixed for all cases now!!! Dongqing
    m_fColonWallThreshold = 280*255/2000; // intentional integer
    m_surfaceTransferFunction.SetColonWall( m_fColonWallThreshold );

		LogFyi( "    Compute inside segments took " + stageTime.AsString(), "Dataset_Processor.C", "Processing" );
		//stageTime.Reset(); //-----------------------------------------------------

    if (rdrGlobal.m_bAbortLoad == true) 
			throw ex::AbortLoadException(LogRec("    Loading aborted by user", "Dataset_Processor.C", "Preprocessing"));

		// ========= Step 9: Prepare volumes for rendering ==========================
		m_fields.m_volumeRender.Resize( m_vDim );
		m_fields.m_volumeRender.SetUnits( m_vUnits );
    m_fields.m_paintVol.Resize( m_vDim );
    m_fields.m_paintVol.SetUnits( m_vUnits );  
    m_fields.m_volumeRender.PreLaunchRenderThreads(this);

		m_fields.CreateCDV( pv, plv, pIns, pipeline.GetVolumeHeader(), pipeline.GetImageOrientation() );
		delete plv;  

		/// TODO: this assume that both series have the same bias. This has to be fixed for loading 2 series from
		///       different scanner
		UpdateBiopsyColormap();

		//LogFyi( "    Preparing volumes for rendering took " + stageTime.AsString(), "Dataset_Processor.C", "Processing" );
		//stageTime.Reset(); //-------------------------------------------------------

    if (rdrGlobal.m_bAbortLoad == true) 
			throw ex::AbortLoadException(LogRec("    Loading aborted by user", "Dataset_Processor.C", "Preprocessing"));

		// ========= Step 10: Prepare non-cleansed INS volume ==========================
		uint1 *pInsNoec = new uint1[iN];
		if( pInsNoec == NULL )
		{
			throw ex::OutOfMemoryException( LogRec("    Out of memory", "Dataset_Processor.C", "Processing") );
		} // if

		pipeline.ComputeUncleansedINS( pv, pIns, pInsNoec );

		//LogFyi( "    Prepare uncleansed INS took " + stageTime.AsString(), "Dataset_Processor.C", "Processing" );
		stageTime.Reset(); //----------------------------------------------------------

    if (rdrGlobal.m_bAbortLoad == true) 
			throw ex::AbortLoadException(LogRec("    Loading aborted by user", "Dataset_Processor.C", "Preprocessing"));

		// ========= Step 11: Compute SLD volumes and TRI =============================
    m_fields.m_sldInterior.Resize  ( m_vDim   );
    m_fields.m_sldInterior.SetUnits( m_vUnits );
    m_fields.m_sldExterior.Resize  ( m_vDim   );   //:: Use this for un-cleansed SLD, i.e. the ULD volume
    m_fields.m_sldExterior.SetUnits( m_vUnits );   //:: Use this for un-cleansed SLD, i.e. the ULD volume

		std::string sTemporaryDirectory = "c:/temp/" + series;
		pipeline.ComputeSLDandTRI( pMask, pIns, pInsNoec, m_fields.m_sldInterior.GetDataPtr(),
			m_fields.m_sldExterior.GetDataPtr(), sTemporaryDirectory );

		delete pInsNoec;
		delete pMask;
		delete pIns;
		
		m_fields.SetScaleFacter4SLD( pipeline.GetVolumeHeader() );

		LogFyi( "    Compute SLD volumes and TRI took " + stageTime.AsString(), "Dataset_Processor.C", "Processing" );
		stageTime.Reset(); //----------------------------------------------------------

    if (rdrGlobal.m_bAbortLoad == true) 
			throw ex::AbortLoadException(LogRec("    Loading aborted by user", "Dataset_Processor.C", "Preprocessing"));

		// ========= Step 12: Compute PDF and CSI =============================
		m_fields.m_dfs.SetHeader( vxVolumeHeader(m_vDim, m_vUnits, 32, 0.0) );
		m_fields.m_dfe.SetHeader( vxVolumeHeader(m_vDim, m_vUnits, 32, 0.0) );
		m_fields.m_csi.SetHeader( vxVolumeHeader(m_vDim, m_vUnits, 16,   0) );

		pipeline.ComputePDFandCSI( &m_fields.m_ins, m_fields.m_sldInterior.GetDataPtr(),
			&m_fields.m_dfs, &m_fields.m_dfe, &m_fields.m_csi, sTemporaryDirectory );

		m_fields.m_dfs.Compact();
		m_fields.m_dfe.Compact();
		m_fields.m_csi.Compact();

		m_segmentList.Read( sTemporaryDirectory );

		LogFyi( "    Compute PDF and CSI took " + stageTime.AsString(), "Dataset_Processor.C", "Processing" );
		stageTime.Reset(); //---------------------------------------------------------- 

    if (rdrGlobal.m_bAbortLoad == true) 
			throw ex::AbortLoadException(LogRec("    Loading aborted by user", "Dataset_Processor.C", "Preprocessing"));

		// ========= Step 13: Post load initialization =============================
		sTemporaryDirectory = sTemporaryDirectory + ".seg";
		vx::File rfile(sTemporaryDirectory); 
	  rfile.Delete();

    m_bCadFindingsLoaded = false;
		m_session.Read(m_sSessionPrefix, *this);
    
		// smooth the centerlines
    SmoothCenterlines();
    m_bDistanceValid = true;  //no sure if we need this?

    try
    {
      m_fields.m_paintVol.ReadData(m_sSessionPrefix + vx::MarkedVolumeExt);
    }
    catch (ex::FileNotFoundException)
    {
      m_fields.m_paintVol.m_bSurfacesNeedReinit = true;
    }
    catch (...)
    {
      m_fields.m_paintVol.Reset();
      m_fields.m_paintVol.Resize(m_vDim);
      m_fields.m_paintVol.SetUnits( m_vUnits );  
    }

    // Initialize the submarine
    m_submarine.SetParentDataset(this);        
    SelectSegment(m_session.m_worldCamera.GetVrp());
    m_submarine.SetView(m_session.m_worldCamera.GetVrp(), m_session.m_worldCamera.GetVpn(), m_session.m_worldCamera.GetVup());
        
    // indicate completion
    m_bVolumeLoaded = true;
    m_bNeedVisualizationReinit = true;

		// compute volume for all segments. Note, this is not essential for diagnois. It had better remove out from preprocessing step.
		ComputeVolume4Segments();

		LogFyi( "    Finish entire preprocessing took " + totalTime.AsString(), "Dataset_Processor.C", "Processing" );

  } // endtry

  catch (ex::AbortLoadException &) 
	{ 
		throw; 
	}
  catch (ex::VException &) 
	{ 
		throw; 
	}
  catch (...) 
	{ 
		throw ex::VException(LogRec("    An unspecified error occurred in preprocessing", "Dataset_Processor.C", "Processing")); 
	}
} // Preprocessing



//undefines
#undef FILE_REVISION


// $Log: Dataset_Processor.C,v $
// Revision 1.0  2011/04/21  dongqing
//  
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/Dataset_Processor.C,v 1.0  2011/04/21 dongqing Exp $
// $Id: Dataset_Processor.C,v 1.0  2011/04/21 dongqing Exp $

