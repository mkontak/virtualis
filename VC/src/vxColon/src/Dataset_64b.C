// $Id: Dataset_64b.C,v 1.0 2011/07/15 dongqing Exp $
//
// Copyright © 2000-2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Dongqing Chen ( dongqing@viatronix.com )


/**
 * This file provide viewer loading process based on new 64bits parallel implemented 
 * preprocessing, but still keep the CON_3-0 style
 */


// includes
#include "StdAfx.h"
#include "Dataset.h"
#include "FileExtensions.h"
#include "StudyLoad.h"
#include "File.h"
#include "ReaderGlobal.h"
#include "Typedef.h"
#include "VolumeRender.inl"
#include "HUConverter.h"
#include "Plane.h"
#include "vxVolumeHeader.h"
#include "FileExtensions.h"
#include "vxDOMDocument.h"
#include "RenderUtil.h"
#include "vxBlockVolumeIterator.h"
#include "vxBlockVolumeIteratorNeighborsConsts.h"
#include "vxBlockVolumeIteratorNeighbors.h"
#include "vxBlockVolumeFileReader.h"
#include "vxHistogramUpdaterIntensity.h"
#include "vxIntensityConverter.h"
#include "FilePtrFactory.h"

// namespaces
using namespace std;
using namespace vx;
using namespace ReaderLib;
using namespace vxCAD;

// defines
#define FILE_REVISION "$Revision: 1.7.2.8 $"



/**
 * Load all data members from disk into memory for 64bit parallel implemented preprocessing
 *  @param uStage               stage within loading
 *  @param loadCallback         load step notify callback method
 *  @param bLoadBoth            whether 1 or 2 datasets are being loaded
 *  @param uOtherLoadPercent    progress of opposite dataset load
 *  @param bSetZoomFactor       whether the overview zoom has already been calculated
 */
void Dataset::LoadStep64b(const uint4 uStage, StudyLoad::LoadCallback & loadCallback, const bool bLoadBoth, const uint4 uOtherLoadPercent, bool & bSetZoomFactor)
{
  try
  {
    uint4 uLoadScale = bLoadBoth ? 2 : 1;
    std::string sOrientation = GetOrientation() == DatasetOrientations::eSUPINE? "Supine" : "Prone";

    switch(uStage)
    {
    case eLoadHeader: 
      {
        LogDbg( "Start Load Step( " + sOrientation + " ): eLoadHeader", "Dataset_64b", "LoadStep64b" );

        m_bVolumeLoaded = false;
        m_bNeedVisualizationReinit = true;

				std::string sFilePath;
				if (! io::FilePtrFactory::Exists(sFilePath = m_sVolumePrefix + vx::VolumeExt))
        {
          if (!io::FilePtrFactory::Exists(sFilePath = m_sPreprocessedPrefix + vx::VolumeExt))
  					throw ex::AbortLoadException(LogRec( "Loading abort due to missing volume file", "Dataset_64b", "LoadStep64b" ) );
        }

        loadCallback.NextStep( StudyLoad::eHEADER, uOtherLoadPercent+m_uLoadPercent, GetOrientation() );
				// Load the volume header information
        m_volumeHeader.Read( sFilePath );

        // Set dataset modality
        std::string sModality;
        m_volumeHeader.GetHeaderEntry( vxVolumeHeader::DataModality, sModality );
        if (sModality == "MR")
        {
          m_eDataModality = Dataset::MR;
        }
        // TODO: Check the DICOM header value of PET, Ultrasound, and other modalities
        else if (sModality == "PET")
        {
          m_eDataModality = Dataset::PET;
        }
        else if (sModality == "US")
        {
          m_eDataModality = Dataset::US;
        }
        else // if (sModality == "CT")
        {
          m_eDataModality = Dataset::CT;
        }

        // Set the orthogonal point to the center of the volume
        m_vUnits = m_volumeHeader.GetVoxelUnits();
        m_vDim   = m_volumeHeader.GetVolDim();
        m_vOrthogonalPtWorld = Point<float4>( m_vDim.m_x * m_fields.m_units2D.m_x,
                                              m_vDim.m_y * m_fields.m_units2D.m_y,
                                              m_vDim.m_z * m_fields.m_units2D.m_z) * 0.5f;

        // setup the value once: this function should be called right after the m_vDim&m_vUnits are set for the first time
        // if the loading order would be changed in the future (as said by Kevin in Revision 3.13.2.3), 
        // the place to call this function should follow the the change.
        if (bSetZoomFactor)
        {
          rdrGlobal.SetupZoomFactorInOverview( m_vDim, m_vUnits );
          bSetZoomFactor = false;
        }
        m_uLoadPercent += 4/uLoadScale;
        LogDbg( "End Load Step( " + sOrientation + " ): eLoadHeader", "Dataset_64b", "LoadStep64b" );
      }
      break;
    case ePreAllocLinearVols:
      {
        LogDbg( "Start Load Step( " + sOrientation + " ): ePreAllocLinearVols", "Dataset_64b", "LoadStep64b" );
        // preallocate the 3 linear volumes to save memory (due to virtual address space
        // fragmentation since windows is limited to 2GB of virtual addressing) 
				m_fields.m_sldInterior.Read( m_sPreprocessedPrefix + vx::SpaceLeapDistExt );
				if( rdrGlobal.m_bUseULDvolume )
				{
          m_fields.m_sldExterior.Read( m_sPreprocessedPrefix + vx::UncleansedSpaceLeapDistExt );       //:: Use this for un-cleansed SLD, i.e. the ULD volume
				}
        m_fields.m_volumeRender.Resize(m_vDim);
        m_fields.m_volumeRender.SetUnits(m_vUnits);
        m_fields.m_paintVol.Resize(m_vDim);
        m_fields.m_paintVol.SetUnits(m_vUnits);  
        m_fields.m_volumeRender.PreLaunchRenderThreads(this);
        LogDbg( "End Load Step( " + sOrientation + " ): ePreAllocLinearVols", "Dataset_64b", "LoadStep64b" );
      }
      break;
    case eLoadOverview:
      {
        LogDbg( "Start Load Step( " + sOrientation + " ): eLoadOverview", "Dataset_64b", "LoadStep64b" );
        // Load the segments
        loadCallback.NextStep(StudyLoad::eSEGMENTS, uOtherLoadPercent+m_uLoadPercent, GetOrientation());
        if (rdrGlobal.m_bAbortLoad == true) 
					throw ex::AbortLoadException(LogRec("Loading aborted by user at stage: reading segment list", "Dataset_64b", "LoadStep64b" ));
        LogDbg( "End Load Step( " + sOrientation + " ): eLoadOverview", "Dataset_64b", "LoadStep64b" );
        m_segmentList.Read(m_sPreprocessedPrefix);
        m_uLoadPercent += 10/uLoadScale;

        // Read CAD Findings
        LogDbg( "Start Load Step( " + sOrientation + " ): CAD", "Dataset_64b", "LoadStep64b" );
        vxDOMDocument::Initialize();
        std::string sCadFilePath( m_sCadPrefix + vx::XMLExt );
        if ( io::FilePtrFactory::Exists(sCadFilePath) )
        {

          // TODO: REST - Need File Server Implementation
          m_cadFindings = vxCadFindings::FromFile( sCadFilePath );
          m_bCadFindingsLoaded = m_cadFindings.GetCount() > 0;
          if (m_cadFindings.m_iCadThresholdValue == -2)
            m_cadFindings.m_iCadThresholdValue = rdrGlobal.GetSettings()->GetIntValue( VCSettings::ViewerXML(), VCSettings::DefaultCadSliderValue() );

          // read original volume because that is the only one that contains the true voxel coordinate conversion
          // if not there, quit!
          std::string sOriginalColumeFilePath(m_sVolumePrefix + vx::VolumeExt );
          if ( ! io::FilePtrFactory::Exists(sOriginalColumeFilePath) )
          {
           LogErr( "Required volume file missing: " + sOriginalColumeFilePath, "Dataset_64b", "LoadStep64b" );
            throw ex::AbortLoadException( LogRec( "Required volume file missing: " + sOriginalColumeFilePath, "Dataset_64b", "LoadStep64b" ) );
          }
          vxVolumeHeader * pOriginalHeader = new vxVolumeHeader;
          pOriginalHeader->Read( sOriginalColumeFilePath );

          m_cadFindings.ResolveUnits( pOriginalHeader->GetVoxelUnits() );
					delete pOriginalHeader;
        }
        LogDbg( "End Load Step( " + sOrientation + " ): CAD", "Dataset_64b", "LoadStep64b" );

        LogDbg( "Start Load Step( " + sOrientation + " ): session", "Dataset_64b", "LoadStep64b" );
        // Load session
        // TODO: Remove this initialization, it's not really necessary, but I'll leave it for now just in case
        for ( int4 i(0); i < m_cadFindings.GetCount(); i++ )
        {
          m_session.m_mapCadFindingInfo[ m_cadFindings.GetFinding( i ).GetId() ].SetDecision(ColonSession::FindingInfo::eDecisionUndecided);
        }
        m_session.Read(m_sSessionPrefix, *this);
        LogDbg( "End Load Step( " + sOrientation + " ): session", "Dataset_64b", "LoadStep64b" );
      }
      break;

    case eLoadVolumeFields:
        LogDbg( "Start Load Step( " + sOrientation + " ): eLoadVolumeFields", "Dataset_64b", "LoadStep64b" );
        loadCallback.NextStep(StudyLoad::eVOLFIELDS, uOtherLoadPercent+m_uLoadPercent, GetOrientation());
        m_fields.LoadVolumeFields3( m_sVolumePrefix, m_sPreprocessedPrefix );
				
				/// TODO: this assume that both series have the same Bias. This has to be fixed for loading 2 series from
				///       different scanner
				UpdateBiopsyColormap();

        LogDbg( "End Load Step( " + sOrientation + " ): eLoadVolumeFields", "Dataset_64b", "LoadStep64b" );
        break;

    case eLoadThresholds: 
      { // There will be no THR file any more!!!
        // Set rendering thresholds. It is fixed for all cases now!!! Dongqing
        m_fColonWallThreshold = 280*255/2000; // intentional integer
        m_surfaceTransferFunction.SetColonWall( m_fColonWallThreshold );
        m_uLoadPercent += 2/uLoadScale;
        
        // Read the ins file from disk
        LogDbg( "Start Load Step( " + sOrientation + " ): eINS", "Dataset_64b", "LoadStep64b" );
        loadCallback.NextStep(StudyLoad::eINS, uOtherLoadPercent+m_uLoadPercent, GetOrientation());
        LogDbg( "End Load Step( " + sOrientation + " ): eINS", "Dataset_64b", "LoadStep64b" );
        if (rdrGlobal.m_bAbortLoad == true) 
					throw ex::AbortLoadException(LogRec("Loading aborted by user at stage: load .ins", "Dataset_64b", "LoadStep64b" ));
        
        std::shared_ptr<io::FilePtr> fileChecker = io::FilePtrFactory::CreateFilePtr( m_sPreprocessedPrefix + vx::InsideLabelVolExt );
				if( !fileChecker->Exists() )
				{
					rdrGlobal.m_bAbortLoad = true;
					throw ex::AbortLoadException(LogRec("Loading abort at stage 'load .ins", "Dataset_64b", "LoadStep64b" ));
				}
				
				// read INS volume
	      vxBlockVolumeFileReader< uint1 >::Read( m_sPreprocessedPrefix + vx::InsideLabelVolExt, m_fields.m_ins );
				m_fields.m_ins.Compact();
        m_uLoadPercent += 10/uLoadScale;

        // smooth the centerlines
        SmoothCenterlines();
        m_bDistanceValid = true;
      }
      break;

    case eLoadNavigationFields:
      {
        // Read the dfs file from disk
        LogDbg( "Start Load Step( " + sOrientation + " ): eDFS", "Dataset_64b", "LoadStep64b" );
        loadCallback.NextStep(StudyLoad::eDFS, uOtherLoadPercent+m_uLoadPercent, GetOrientation());
        LogDbg( "End Load Step( " + sOrientation + " ): eDFS", "Dataset_64b", "LoadStep64b" );        
        if (rdrGlobal.m_bAbortLoad == true) 
					throw ex::AbortLoadException(LogRec("Loading aborted by user at stage: load .dfs", "Dataset_ext", "LoadStep3"));
				vxBlockVolumeFileReader< float4 >::Read( m_sPreprocessedPrefix + "_0" + vx::PotentialDistRootExt, m_fields.m_dfs );
				m_fields.m_dfs.Compact();
        m_uLoadPercent += 10/uLoadScale;

        // Read the dfe file from disk
        LogDbg( "Start Load Step( " + sOrientation + " ): eDFE", "Dataset_64b", "LoadStep64b" );
        loadCallback.NextStep(StudyLoad::eDFE, uOtherLoadPercent+m_uLoadPercent, GetOrientation());
        LogDbg( "End Load Step( " + sOrientation + " ): eDFE", "Dataset_64b", "LoadStep64b" );
        if (rdrGlobal.m_bAbortLoad == true) 
					throw ex::AbortLoadException(LogRec( "Loading aborted by user at stage: load .dfe", "Dataset_64b", "LoadStep64b" ));
				vxBlockVolumeFileReader< float4 >::Read( m_sPreprocessedPrefix + "_1" + vx::PotentialDistRootExt, m_fields.m_dfe );
				m_fields.m_dfe.Compact();
        m_uLoadPercent += 10/uLoadScale;

        // read the csi file from disk
        LogDbg( "Start Load Step( " + sOrientation + " ): eCSI", "Dataset_64b", "LoadStep64b" );
        loadCallback.NextStep(StudyLoad::eCSI, uOtherLoadPercent+m_uLoadPercent, GetOrientation());
        LogDbg( "End Load Step( " + sOrientation + " ): eCSI", "Dataset", "LoadStep" );
        if (rdrGlobal.m_bAbortLoad == true) 
					throw ex::AbortLoadException(LogRec("Loading aborted by user at stage: load .csi", "Dataset_ext", "LoadStep3"));
				vxBlockVolumeFileReader< uint2 >::Read( m_sPreprocessedPrefix + "_0" + vx::ClosestSkelIndexExt, m_fields.m_csi );
				m_fields.m_csi.Compact();
        m_uLoadPercent += 10/uLoadScale;
      }
      break;
    case eLoadRenderFields:
      {
        // Read the space leap files from disk
        LogDbg( "Start Load Step( " + sOrientation + " ): eSLD", "Dataset_64b", "LoadStep64b" );
        loadCallback.NextStep(StudyLoad::eSLD, uOtherLoadPercent+m_uLoadPercent, GetOrientation());
        LogDbg( "End Load Step( " + sOrientation + " ): eSLD", "Dataset_64b", "LoadStep64b" );
        if (rdrGlobal.m_bAbortLoad == true) 
					throw ex::AbortLoadException(LogRec("Loading aborted by user at stage: load .sld", "Dataset_ext", "LoadStep3"));
        
				m_fields.LoadSpaceLeapFields3( m_sPreprocessedPrefix );
        m_uLoadPercent += 22/uLoadScale;

        // Read the session files from disk
        LogDbg( "Start Load Step( " + sOrientation + " ): eVOLUME", "Dataset_64b", "LoadStep64b" );
        loadCallback.NextStep(StudyLoad::eVOLUME, uOtherLoadPercent+m_uLoadPercent, GetOrientation());
        LogDbg( "End Load Step( " + sOrientation + " ): eVOLUME", "Dataset", "LoadStep" );
        if (rdrGlobal.m_bAbortLoad == true) 
					throw ex::AbortLoadException(LogRec("Loading aborted by user at stage: load .ecv", "Dataset_ext", "LoadStep3"));
        m_uLoadPercent += 22/uLoadScale;

        // Set the orthogonal point to the center of the volume
        m_vOrthogonalPtWorld = Point<float4>( m_vDim.m_x * m_fields.m_units2D.m_x,
                                              m_vDim.m_y * m_fields.m_units2D.m_y,
                                              m_vDim.m_z * m_fields.m_units2D.m_z) * 0.5f;

        try
        {
          LogDbg( "Start Load Step( " + sOrientation + " ): paint volume", "Dataset_64b", "LoadStep64b" );
          m_fields.m_paintVol.ReadData(m_sSessionPrefix + vx::MarkedVolumeExt);
          LogDbg( "End Load Step( " + sOrientation + " ): paint volume", "Dataset_64b", "LoadStep64b" );
        }
        catch (ex::FileNotFoundException)
        {
          m_fields.m_paintVol.m_bSurfacesNeedReinit = true;
        }
        catch (...)
        {
          m_fields.m_paintVol.Reset();
          m_fields.m_paintVol.Resize(m_vDim);
          m_fields.m_paintVol.SetUnits(m_vUnits);  
        }
      }
      break;
    case ePostLoadInit:
      {
        // Initialize the submarine
        m_submarine.SetParentDataset(this);        
        SelectSegment(m_session.m_worldCamera.GetVrp());
        m_submarine.SetView(m_session.m_worldCamera.GetVrp(), m_session.m_worldCamera.GetVpn(), m_session.m_worldCamera.GetVup());
        
        // indicate completion
        m_bVolumeLoaded = true;
        m_bNeedVisualizationReinit = true;

				// compute volume for all segments
				ComputeVolume4Segments();
      }
      break;
    default:
      break;
    }

  } // endtry

  catch (ex::AbortLoadException &) { throw;}
  catch (ex::VException &) { throw;}
  catch (...) { throw ex::VException(LogRec("An unspecified error in stage" + uStage ,"Dataset_ext", "LoadStep3"));}

  return;
} // LoadStep64b



//undefines
#undef FILE_REVISION


// $Log: Dataset_64b.C,v $
// Revision 1.0 2011/07/12  dongqing
//  
// $Header: /vxColon/src/Dataset_64b.C,v 1.0 2011/07/15 dongqing Exp $
// $Id: Dataset_64b.C,v 1.0 2011/07/15 dongqing Exp $

