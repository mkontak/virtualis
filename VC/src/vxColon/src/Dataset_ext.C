// $Id: Dataset_ext.C,v 1.7.2.8 2010/12/20 18:53:26 dongqing Exp $
//
// Copyright © 2000-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Dongqing Chen ( dongqing@viatronix.com )


/**
 * This file provide more efficient file reading via vxBlockVolumeFileReader 
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

// namespaces
using namespace std;
using namespace vx;
using namespace ReaderLib;
using namespace vxCAD;

// defines
#define FILE_REVISION "$Revision: 1.7.2.8 $"



/**
 * Load all data members from disk into memory with vxBlockVolumeFileReader.
 *  @param uStage               stage within loading
 *  @param loadCallback         load step notify callback method
 *  @param bLoadBoth            whether 1 or 2 datasets are being loaded
 *  @param uOtherLoadPercent    progress of opposite dataset load
 *  @param bSetZoomFactor       whether the overview zoom has already been calculated
 */
void Dataset::LoadStep3(const uint4 uStage, StudyLoad::LoadCallback & loadCallback, const bool bLoadBoth, const uint4 uOtherLoadPercent, bool & bSetZoomFactor)
{
  try
  {
    uint4 uLoadScale = bLoadBoth ? 2 : 1;
    std::string sOrientation = GetOrientation() == DatasetOrientations::eSUPINE? "Supine" : "Prone";

    switch(uStage)
    {
    case eLoadHeader: 
      {
        LogDbg( "Start Load Step( " + sOrientation + " ): eLoadHeader", "Dataset_ext", "LoadStep3" );

        m_bVolumeLoaded = false;
        m_bNeedVisualizationReinit = true;

				std::string sFilePath;
				if (!vx::File(sFilePath = m_sVolumePrefix + vx::VolumeExt).Exists())
        {
          if (!vx::File(sFilePath = m_sPreprocessedPrefix + vx::VolumeExt).Exists())
  					throw ex::AbortLoadException(LogRec( "Loading abort due to missing volume file", "Dataset_ext", "LoadStep3") );
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
        /*if (bSetZoomFactor)
        {
          rdrGlobal.SetupZoomFactorInOverview( m_vDim, m_vUnits );
          bSetZoomFactor = false;
        }*/
        m_uLoadPercent += 4/uLoadScale;
        LogDbg( "End Load Step( " + sOrientation + " ): eLoadHeader", "Dataset_ext", "LoadStep3" );
      }
      break;
    case ePreAllocLinearVols:
      {
        LogDbg( "Start Load Step( " + sOrientation + " ): ePreAllocLinearVols", "Dataset", "LoadStep" );
        // preallocate the 3 linear volumes to save memory (due to virtual address space
        // fragmentation since windows is limited to 2GB of virtual addressing) 
        m_fields.m_sldInterior.Resize(m_vDim);
        m_fields.m_sldInterior.SetUnits(m_vUnits);
				if( rdrGlobal.m_bUseULDvolume )
				{
          m_fields.m_sldExterior.Resize(m_vDim);       //:: Use this for un-cleansed SLD, i.e. the ULD volume
          m_fields.m_sldExterior.SetUnits(m_vUnits);   //:: Use this for un-cleansed SLD, i.e. the ULD volume
				}
        m_fields.m_volumeRender.Resize(m_vDim);
        m_fields.m_volumeRender.SetUnits(m_vUnits);
        m_fields.m_paintVol.Resize(m_vDim);
        m_fields.m_paintVol.SetUnits(m_vUnits);  
        m_fields.m_volumeRender.PreLaunchRenderThreads(this);
        LogDbg( "End Load Step( " + sOrientation + " ): ePreAllocLinearVols", "Dataset_ext", "LoadStep3" );
      }
      break;
    case eLoadOverview:
      {
        LogDbg( "Start Load Step( " + sOrientation + " ): eLoadOverview", "Dataset_ext", "LoadStep3" );
        // Load the segments
        loadCallback.NextStep(StudyLoad::eSEGMENTS, uOtherLoadPercent+m_uLoadPercent, GetOrientation());
        if (rdrGlobal.m_bAbortLoad == true) 
					throw ex::AbortLoadException(LogRec("Loading aborted by user at stage: reading segment list", "Dataset_ext", "LoadStep3"));
        LogDbg( "End Load Step( " + sOrientation + " ): eLoadOverview", "Dataset_ext", "LoadStep3" );
        m_segmentList.Read(m_sPreprocessedPrefix);
        m_uLoadPercent += 10/uLoadScale;

        // Read CAD Findings
        LogDbg( "Start Load Step( " + sOrientation + " ): CAD", "Dataset_ext", "LoadStep3" );
        vxDOMDocument::Initialize();
        vx::File cadfile( m_sCadPrefix + vx::XMLExt );
        if ( cadfile.Exists() )
        {
          m_cadFindings = vxCadFindings::FromFile( cadfile.ToString() );
          m_bCadFindingsLoaded = m_cadFindings.GetCount() > 0;
          if (m_cadFindings.m_iCadThresholdValue == -2)
            m_cadFindings.m_iCadThresholdValue = rdrGlobal.GetSettings()->GetIntValue( VCSettings::ViewerXML(), VCSettings::DefaultCadSliderValue() );

          // read original volume because that is the only one that contains the true voxel coordinate conversion
          // if not there, quit!
          vx::File originalVolumeFile( m_sVolumePrefix + vx::VolumeExt );
          if ( ! originalVolumeFile.Exists() )
          {
            LogErr( "Required volume file missing: " + originalVolumeFile.GetAbsolutePath(), "Dataset_ext", "LoadStep3" );
            throw ex::AbortLoadException( LogRec( "Required volume file missing: " + originalVolumeFile.GetAbsolutePath(), "Dataset_ext", "LoadStep3" ) );
          }
          vxVolumeHeader * pOriginalHeader = new vxVolumeHeader;
          pOriginalHeader->Read( originalVolumeFile.GetAbsolutePath() );

          m_cadFindings.ResolveUnits( pOriginalHeader->GetVoxelUnits() );
					delete pOriginalHeader;
        }
        LogDbg( "End Load Step( " + sOrientation + " ): CAD", "Dataset_ext", "LoadStep3" );

        LogDbg( "Start Load Step( " + sOrientation + " ): session", "Dataset_ext", "LoadStep3" );
        // Load session
        // TODO: Remove this initialization, it's not really necessary, but I'll leave it for now just in case
        for ( int4 i(0); i < m_cadFindings.GetCount(); i++ )
        {
          m_session.m_mapCadFindingInfo[ m_cadFindings.GetFinding( i ).GetId() ].SetDecision(ColonSession::FindingInfo::eDecisionUndecided);
        }
        m_session.Read(m_sSessionPrefix, *this);
        LogDbg( "End Load Step( " + sOrientation + " ): session", "Dataset_ext", "LoadStep3" );
      }
      break;

    case eLoadVolumeFields:
        LogDbg( "Start Load Step( " + sOrientation + " ): eLoadVolumeFields", "Dataset_ext", "LoadStep3" );
        loadCallback.NextStep(StudyLoad::eVOLFIELDS, uOtherLoadPercent+m_uLoadPercent, GetOrientation());
        m_fields.LoadVolumeFields3( m_sVolumePrefix, m_sPreprocessedPrefix );
				
				/// TODO: this assume that both series have the same Bias. This has to be fixed for loading 2 series from
				///       different scanner
				UpdateBiopsyColormap();

        LogDbg( "End Load Step( " + sOrientation + " ): eLoadVolumeFields", "Dataset_ext", "LoadStep3" );
        break;

    case eLoadThresholds: 
      { // There will be no THR file any more!!!
        // Set rendering thresholds. It is fixed for all cases now!!! Dongqing
        m_fColonWallThreshold = 280*255/2000; // intentional integer
        m_surfaceTransferFunction.SetColonWall( m_fColonWallThreshold );
        m_uLoadPercent += 2/uLoadScale;
        
        // Read the ins file from disk
        LogDbg( "Start Load Step( " + sOrientation + " ): eINS", "Dataset_ext", "LoadStep3" );
        loadCallback.NextStep(StudyLoad::eINS, uOtherLoadPercent+m_uLoadPercent, GetOrientation());
        LogDbg( "End Load Step( " + sOrientation + " ): eINS", "Dataset_ext", "LoadStep3" );
        if (rdrGlobal.m_bAbortLoad == true) 
					throw ex::AbortLoadException(LogRec("Loading aborted by user at stage: load .ins", "Dataset_ext", "LoadStep3"));
				vx::File fileChecker( m_sPreprocessedPrefix + vx::InsideLabelVolExt );
				if( !fileChecker.Exists() )
				{
					//rdrGlobal.m_bAbortLoad = true;
					throw ex::AbortLoadException(LogRec("Loading abort at stage 'load .ins", "Dataset_ext", "LoadStep3"));
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
        LogDbg( "Start Load Step( " + sOrientation + " ): eDFS", "Dataset_ext", "LoadStep3" );
        loadCallback.NextStep(StudyLoad::eDFS, uOtherLoadPercent+m_uLoadPercent, GetOrientation());
        LogDbg( "End Load Step( " + sOrientation + " ): eDFS", "Dataset_ext", "LoadStep3" );        
        if (rdrGlobal.m_bAbortLoad == true) 
					throw ex::AbortLoadException(LogRec("Loading aborted by user at stage: load .dfs", "Dataset_ext", "LoadStep3"));
				vxBlockVolumeFileReader< float4 >::Read( m_sPreprocessedPrefix + "_0" + vx::PotentialDistRootExt, m_fields.m_dfs );
				m_fields.m_dfs.Compact();
        m_uLoadPercent += 10/uLoadScale;

        // Read the dfe file from disk
        LogDbg( "Start Load Step( " + sOrientation + " ): eDFE", "Dataset_ext", "LoadStep3" );
        loadCallback.NextStep(StudyLoad::eDFE, uOtherLoadPercent+m_uLoadPercent, GetOrientation());
        LogDbg( "End Load Step( " + sOrientation + " ): eDFE", "Dataset_ext", "LoadStep3" );
        if (rdrGlobal.m_bAbortLoad == true) 
					throw ex::AbortLoadException(LogRec( "Loading aborted by user at stage: load .dfe", "Dataset_ext", "LoadStep3" ));
				vxBlockVolumeFileReader< float4 >::Read( m_sPreprocessedPrefix + "_1" + vx::PotentialDistRootExt, m_fields.m_dfe );
				m_fields.m_dfe.Compact();
        m_uLoadPercent += 10/uLoadScale;

        // read the csi file from disk
        LogDbg( "Start Load Step( " + sOrientation + " ): eCSI", "Dataset_ext", "LoadStep3" );
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
        LogDbg( "Start Load Step( " + sOrientation + " ): eSLD", "Dataset_ext", "LoadStep3" );
        loadCallback.NextStep(StudyLoad::eSLD, uOtherLoadPercent+m_uLoadPercent, GetOrientation());
        LogDbg( "End Load Step( " + sOrientation + " ): eSLD", "Dataset_ext", "LoadStep3" );
        if (rdrGlobal.m_bAbortLoad == true) 
					throw ex::AbortLoadException(LogRec("Loading aborted by user at stage: load .sld", "Dataset_ext", "LoadStep3"));
        
				m_fields.LoadSpaceLeapFields3( m_sPreprocessedPrefix );
        m_uLoadPercent += 22/uLoadScale;

        // Read the session files from disk
        LogDbg( "Start Load Step( " + sOrientation + " ): eVOLUME", "Dataset_ext", "LoadStep3" );
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
          LogDbg( "Start Load Step( " + sOrientation + " ): paint volume", "Dataset_ext", "LoadStep3" );
          m_fields.m_paintVol.ReadData(m_sSessionPrefix + vx::MarkedVolumeExt);
          LogDbg( "End Load Step( " + sOrientation + " ): paint volume", "Dataset_ext", "LoadStep3" );
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
        
				// compute volume for all segments
				ComputeVolume4Segments();

        // indicate completion
        m_bVolumeLoaded = true;
        m_bNeedVisualizationReinit = true;
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
} // LoadStep3


/** extract the associated skeleton indexes for a cad finding
 *
 * @param rFinding the input cad finding
 * @param iti the iterator for the INS volume
 * @param fMaximumColonDiameter the maximum diameter of colon lumen
 * @return true if the finding is in the same lumen as the nearest centerline node in
 */
bool Dataset::ExtractSkeletonIndexesForFinding( vxCAD::vxCadFinding & rFinding, vxBlockVolumeIterator<uint1> & iti, const float4 & fMaximumColonDiameter )
{
  bool bIsInLumen = true;

  Skeleton * pSkeleton = static_cast< Skeleton * >( & m_joinedSkeleton );

  int4 iN = pSkeleton->GetNumOfElements();

  // search for the point on the centerline nearest to the finding air voxel. The centerline index is the joined skeleton
  float4 fMinDistance = Vector3Df( pSkeleton->GetWorldPos( 0 ) - rFinding.GetCentroidMM().GetVector() ).GetLength();
  float4 fCurrentDistance;
  int4 iSelectedJoinedIndex = 0;

  register int4 j;

  for( j=1; j<iN; j++ )
  {
    fCurrentDistance = Vector3Df( pSkeleton->GetWorldPos(j) - rFinding.GetCentroidMM().GetVector() ).GetLength();
    if( fCurrentDistance < fMinDistance )
    {
      fMinDistance = fCurrentDistance;
      iSelectedJoinedIndex = j;
    } // if
  } // for j
 
  if( fMinDistance > fMaximumColonDiameter )
    bIsInLumen = false;

  Vector3D<float4> vfNearestNode = Vector3D<float4>( pSkeleton->GetWorldPos(iSelectedJoinedIndex) );

  iti.SetPos( Vector3D<int4>(pSkeleton->GetVolumePos(iSelectedJoinedIndex)) );
  rFinding.SetSegment( iti.GetVoxel() );  /// This is necessary for synchonization of display

  pSkeleton = & m_segmentList.GetUnsortedSegment( m_segmentList.GetUnsortedSegmentIndexFromIdentifier( rFinding.GetSegment() ) ).GetCenterline( 0 );
  
  /// determine the index for the selected segment
  iN = pSkeleton->GetNumOfElements();
  int4 iSelectedIndex = iSelectedJoinedIndex;

  for( j=0; j<iN; j++ )
  {
    if( vfNearestNode == Vector3D<float4>(pSkeleton->GetWorldPos(j)) )
    {
      iSelectedIndex = j;
      break;
    } // if
  } // for j
  
  m_session.m_mapCadFindingInfo[ rFinding.GetId() ].SetJoinedSkeletonIndex( iSelectedJoinedIndex );
  m_session.m_mapCadFindingInfo[ rFinding.GetId() ].SetSkeletonIndex( iSelectedIndex );        

  return bIsInLumen;
} // ExtractSkeletonIndexesForFinding()


/**
 *  compute air volume for all segments. This has to be called all way at the end of data loading.
 *  The average HU value for tagged region are calculated as well. 
 */
void Dataset::ComputeVolume4Segments()
{
	const int4 iNumberOfSegments = m_segmentList.GetNumUnsortedSegments();
	if( iNumberOfSegments < 1 || iNumberOfSegments > 16 )
	{
		LogErr("Incorrect number of colon segment was found while computing volumes.", "Dataset_ext.C", "ComputeVolume4Segments");
		return;
	} // if

	uint4 * iAirVoxelCounts   = new uint4[iNumberOfSegments];
	uint4 * iTotalVoxelCounts = new uint4[iNumberOfSegments];
	uint4 * iTaggedVoxelCounts = new uint4[iNumberOfSegments];
	double * fTaggedValue = new double[iNumberOfSegments];

	register int4 i, iSegIndex;

	for( i=0; i<iNumberOfSegments; i++ )
	{
		iAirVoxelCounts[i] = 0;
		iTotalVoxelCounts[i] = 0;
		fTaggedValue[i] = 0.0;
		iTaggedVoxelCounts[i] = 0;
	} // for i

	/// iterator for the volumes
	vxBlockVolumeIterator<uint1> iti( m_fields.m_ins );
	vxBlockVolumeIterator<uint2> itv( *m_fields.m_sliceRendererData.GetUncleansedVolume() );
	vxBlockVolumeIteratorNeighbors<uint1> itNB( m_fields.m_ins, Vector3D<int4>(0,0,0), vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors18 );

	const uint2 uAirBound = vxIntensityConverter::ModalityToRaw( -759, m_fields.m_sliceRendererData.GetUncleansedVolume()->GetHeader() );
	const uint2 uTaggedBound = vxIntensityConverter::ModalityToRaw( 200, m_fields.m_sliceRendererData.GetUncleansedVolume()->GetHeader() );

	register Vector3D<int4> vPos;

	/// copy the segmental mask to the temporary mask volume
	for( ; iti.IsNotAtEnd(); iti.NextBlockZYX() )
	{
		for( ; iti.IsNotAtEndOfBlock(); iti.NextZYXinsideBlock() )
		{
      /// check if it is inside a segment
      if( iti.GetVoxel() > 0 && iti.GetVoxel() < 17 )
			{
				vPos = iti.GetPos();

				itv.SetPos( vPos );

				iSegIndex = int4(iti.GetVoxel()-1);

				iTotalVoxelCounts[iSegIndex] = iTotalVoxelCounts[iSegIndex]+1;

				if( itv.GetVoxel() < uAirBound )
				{ /// set new label mask volume
				  iAirVoxelCounts[iSegIndex] = iAirVoxelCounts[iSegIndex]+1;
				}
				else if( itv.GetVoxel() > uTaggedBound )
				{
					iTaggedVoxelCounts[iSegIndex] = iTaggedVoxelCounts[iSegIndex] + 1;
					fTaggedValue[iSegIndex] = fTaggedValue[iSegIndex] + double( float4(int4(itv.GetVoxel())) );
				} // else if

				itNB.CenterAt( vPos );

				for( ; !itNB.IsAtEnd(); ++itNB )
				{
					if( itNB.GetVoxel() == 0x00 )
					{
						if( itNB.IsInsideVolume() )
						{
							itNB.SetVoxel(255);
						}
						iTotalVoxelCounts[iSegIndex] = iTotalVoxelCounts[iSegIndex]+1;

						if( itv.GetVoxel() < uAirBound )
						{ /// set new label mask volume
							iAirVoxelCounts[iSegIndex] = iAirVoxelCounts[iSegIndex]+1;
						}
					} // if 
				} // for check all neighbors
			} // if
		} // inside the block
	} // for all blocks

	/// reset the INS back to its original value
	for( iti.SetPos(0,0,0); iti.IsNotAtEnd(); iti.NextBlockZYX() )
	{
		for( ; iti.IsNotAtEndOfBlock(); iti.NextZYXinsideBlock() )
		{
			if( iti.GetVoxel() == 255 )
				iti.SetVoxel( 0x00 );
		} // for 
	} // for

	/// record the volume value in the segment data structure
	const float4 fUnitVolume = m_vUnits.m_x*m_vUnits.m_y*m_vUnits.m_z;

	/// iterator for the segment list
	SegmentList::UnsortedIterator itSeg( m_segmentList );

	for( ; !itSeg.IsAtEnd(); itSeg++ )
	{
		Skeleton & ske = (*itSeg).GetCurrentSkeleton();
		Point<int4> posVol = ske.GetSkelNode(1, true).m_vVolumePos;
		iti.SetPos( posVol.m_x, posVol.m_y, posVol.m_z );
		if( iti.GetVoxel() > 0 ) 
		{
      i = iti.GetVoxel()-1;
		}
		else
		{ 
			LogErr("Volume measurement for one segment is incorrect.","Dataset_ext.C", "ComputeVolume4Segments");
			break;
		} // else

		(*itSeg).SetAirVolume  ( float4(iAirVoxelCounts[i])/1000.0F*fUnitVolume );
		(*itSeg).SetTotalVolume( float4(iTotalVoxelCounts[i])/1000.0F*fUnitVolume );

		(*itSeg).SetAverageTaggedHU( fTaggedValue[i] );
		(*itSeg).SetTaggedVoxelCount( iTaggedVoxelCounts[i] );
	} // for

} // ComputeVolume4Segments()



//undefines
#undef FILE_REVISION


// $Log: Dataset_ext.C,v $
// Revision 1.7.2.8  2010/12/20 18:53:26  dongqing
// add VCSetting switch for using ULD volume
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.7.2.7  2010/12/16 16:34:39  dongqing
// Forgot to remove the initialization of temportary volume.
// Fixed now.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.7.2.6  2010/12/16 16:23:28  dongqing
// Change the calculation of segment volume. No new temporary mask volume
// is needed now. That saves the memory footprint of the appl
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.7.2.5  2010/11/15 20:37:29  dongqing
// add codes for measuring air lumen volume of selected segments
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.7.2.4  2010/04/05 20:09:34  dongqing
// Fix the bug that the Display Extracolonic CAD Finding toggle shown
// incorrect association between finding and selected colon segments.
// Note, the CAD Pane should always display the total number from the CAD
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.7.2.3  2010/01/25 22:24:20  dongqing
// add JoinedSkeletonIndex for correctly display cad mark in UDS view
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.7.2.2  2009/11/10 19:03:48  dongqing
// Improving fly speed for non-cleansed images by using ULD volume
//
// Revision 1.7.2.1  2008/11/11 23:45:42  jmeade
// spelling in comment.
//
// Revision 1.7  2007/03/13 02:12:46  jmeade
// code standards.
//
// Revision 1.6  2007/02/27 21:50:17  jmeade
// Issue 5424: ensure cad threshold default is read when necessary.
//
// Revision 1.5  2006/11/22 19:18:03  dongqing
// fix the pseudocolormap bug for studies having bias != 1000
//
// Revision 1.4  2006/09/13 15:17:05  frank
// Issue #4965: Unflipped prone images to make viewer orientation agnostic
//
// Revision 1.3  2006/07/24 19:04:57  jmeade
// Issue 4657: Bug: deleting needed volume header information after read.
//
// Revision 1.2  2006/06/22 15:20:24  dongqing
// Get rid of reading SLV file
//
// Revision 1.1  2006/06/22 14:00:37  dongqing
// First checkin using vxBlockVolumeFileReader
//
// Revision 1.16  2006/06/15 dongqing
//  
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/Dataset_ext.C,v 1.7.2.8 2010/12/20 18:53:26 dongqing Exp $
// $Id: Dataset_ext.C,v 1.7.2.8 2010/12/20 18:53:26 dongqing Exp $

