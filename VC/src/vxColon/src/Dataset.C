// $Id: Dataset.C,v 1.37.2.30 2010/12/20 18:53:26 dongqing Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Frank Dachille( frank@viatronix.com )


/**
 * This class encapsulates the data required for a study. 
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
#include "vxBlockVolumeUtilities.h"
#include "VolumeUtilities.h"
#include "SeriesVersion.h"
#include "vxIntensityConverter.h"
#include "vxHistogramUpdaterLabel.h"
#include "FilePtrFactory.h"

// namespaces
using namespace std;
using namespace vx;
using namespace ReaderLib;
using namespace vxCAD;

// defines
#define FILE_REVISION "$Revision: 1.37.2.30 $"


// static declarations
uint4 Dataset::vLoadStage1GB[Dataset::eLoadStageCount] = {Dataset::eLoadHeader,Dataset::ePreAllocLinearVols,Dataset::eLoadOverview,Dataset::eLoadVolumeFields,Dataset::eLoadThresholds,Dataset::eLoadNavigationFields,Dataset::eLoadRenderFields,Dataset::ePostLoadInit};
uint4 Dataset::vLoadStage2GB[Dataset::eLoadStageCount] = {Dataset::eLoadHeader,Dataset::ePreAllocLinearVols,Dataset::eLoadOverview,Dataset::eLoadVolumeFields,Dataset::eLoadThresholds,Dataset::eLoadRenderFields,Dataset::eLoadNavigationFields,Dataset::ePostLoadInit};

/// current dataset for (global) sort routine callback
Dataset* Dataset::m_pCurrentSortDataset = (Dataset *) NULL;

/// global unique ID for datasets
uint4 Dataset::uGlobalUniqueID(1);

//////////////////////////////////////////////////////////////////////////


/**
 * removes a finding
 * @param rDataset   dataset for the finding
 * @param iIndex     index of the finding
 *
 */
void SuspiciousRegions::RemoveFinding( Dataset & rDataset, const int4 iIndex )
{
	const Triple<int4>     viDim = rDataset.m_fields.m_sliceRendererData.GetLabelVolume()->GetHeader().GetVolDim();
	const Triple<float4> vfUnits = rDataset.m_fields.m_sliceRendererData.GetLabelVolume()->GetHeader().GetVoxelUnits();
	const float4       fBoxWidth = 20.0F; // in millimeter

	uint2 uLabelIndex = vxCAD::vxCadFindings::GetFinding(iIndex).GetVolumeROI().GetLabelIndex();
  vxShareableObject< vxComponentArray > * pComponentArray( rDataset.m_fields.m_sliceRendererData.GetComponents() );

  // check that the component exists before removing it
  uint2 uComponentIndex = pComponentArray->GetHistoryFromLabel(uLabelIndex);
  if( uComponentIndex != 0 )
    pComponentArray->RemoveComponent( pComponentArray->GetHistoryFromLabel(uLabelIndex) );

	Vector3D<float4> vfCtr = rDataset.m_suspiciousRegions.GetFinding(iIndex).GetCentroidMM().GetVector();
	Vector3D<int2>   viCtr = Vector3D<int2>( vfCtr.m_x/vfUnits.m_x+0.5F, vfCtr.m_y/vfUnits.m_y+0.5F, vfCtr.m_z/vfUnits.m_z+0.5F );
	const int4 ixl = max(viCtr.m_x-int4(fBoxWidth/vfUnits.m_x), 0);
	const int4 ixu = min(viCtr.m_x+int4(fBoxWidth/vfUnits.m_x), viDim.m_x-1);
	const int4 iyl = max(viCtr.m_y-int4(fBoxWidth/vfUnits.m_y), 0);
	const int4 iyu = min(viCtr.m_y+int4(fBoxWidth/vfUnits.m_y), viDim.m_y-1);
	const int4 izl = max(viCtr.m_z-int4(fBoxWidth/vfUnits.m_z), 0);
	const int4 izu = min(viCtr.m_z+int4(fBoxWidth/vfUnits.m_z), viDim.m_z-1);

	uint1 * pPaintVol = rDataset.m_fields.m_paintVol.GetDataPtr();

	vxBlockVolumeIterator<uint2> itl( *rDataset.m_fields.m_sliceRendererData.GetLabelVolume() );
	register int4 x, y, z, j, k, s=viDim.m_x*viDim.m_y;
	register uint1 uSurfaceLabel = SUSPICIOUSROI;
	j = izl*s;

	for( z=izl, itl.SetPosZ(izl); z<izu; z++, itl.IncZ() )
	{
		k = j+iyl*viDim.m_x;

		for( y=iyl, itl.SetPosY(iyl); y<iyu; y++, itl.IncY() )
		{
			for( x=ixl, itl.SetPosX(ixl); x<ixu; x++, itl.IncX() )
			{
				if( itl.GetVoxel() == uLabelIndex )
					itl.SetVoxel( 0 );
				if( pPaintVol[k+x] == uSurfaceLabel )
					pPaintVol[k+x] = 2;
			} // for x

			k = k+viDim.m_x;
		} // for y

		j = j+s;
	} // for z

	vxCAD::vxCadFindings::RemoveFinding(iIndex); 
} // RemoveFinding()


//////////////////////////////////////////////////////////////////////////



/**
 * Constructor.
 */
Dataset::Dataset() :
m_seriesVersion( "-1.0" ),
m_uUniqueID(uGlobalUniqueID++)
{
	Reset();
} // Dataset()


/**
 * Destructor.
 */
Dataset::~Dataset()
{
} // ~Dataset()


/**
 * Reset all data members.
 */
void Dataset::Reset()
{
	m_submarine.SetParentDataset(this);
	m_fColonWallThreshold = 0.0F;
	m_surfaceTransferFunction.SetColonWall(30);
	m_biopsyTransferFunction.SetBiopsy();
	m_TTPTransferFunction.SetTransparentTissueProjection();
	
	m_vUnits = Triple<float4>(1.0F, 1.0F, 1.0F);
	m_vDim = Triple<uint4>(0.0F, 0.0F, 0.0F);
	m_submarine.Reset();
	m_session.Reset();

	m_fields.Reset();
	
	m_vOrthogonalPtWorld = Point<float4>(0.0F, 0.0F, 0.0F);
	m_bVolumeLoaded = false;  // whether or not the Load() method has been called

	m_bDistanceValid = false;
	m_volumeHeader = vxVolumeHeader();
	m_eOrientation = DatasetOrientations::eDEFAULT;
	
	// initialize virtual trackball (that controls Supine/Prone overview).
	m_trackballVerify.Reset();
	m_trackballVerify.Init(MatchOrientation(m_eOrientation));
	m_trackballStandard.Reset();
	m_trackballStandard.Init(MatchOrientation(m_eOrientation));
	m_pTrackball = & m_trackballStandard;

	m_uCsi = 0;
	m_fCrossSecDistance = 20.0F;

	m_vPatchList.clear();

	m_bNeedVisualizationReinit = false;

	m_vObliqueLocation    =  Point<float4>(0.0F, 0.0F, 0.0F);
	m_vObliqueOrientation = Normal<float4>(0.0F, 0.0F, 1.0F);
	m_vObliqueUp          = Normal<float4>(0.0F, 1.0F, 0.0F);
	m_vObliqueBottomLeft  =  Point<float4>(0.0F, 0.0F, 0.0F);
	m_vObliqueBottomRight =  Point<float4>(0.0F, 0.0F, 0.0F);
	m_vObliqueTopLeft     =  Point<float4>(0.0F, 0.0F, 0.0F);
	m_vObliqueTopRight    =  Point<float4>(0.0F, 0.0F, 0.0F);
	m_uObliqueViewIdentifier = 0;
	m_uCrossSecIndex = 20;
	m_iXsectionScrollBarLocationIndex = 0;

	m_joinedSkeleton.Resize(0,0);
	m_straightSegmentNodes.resize(0);
	m_currentSegment = 0;

	m_joinedFlippedSkeleton.Resize(0,0);
	m_ivNominatedFeatures.resize(0);  
	m_ivFeatures.resize(0);
	m_iFeatureToDisplayInfo=0;
	
	m_fSegmentErrorThreshold = 12.0;
	
	//UpdateBiopsyColormap();

	m_eDataModality = Dataset::CT;
	m_bUserInteracting = false;

	m_cadFindings = vxCadFindings();
	m_bCadFindingsLoaded = false;

	m_suspiciousRegions = SuspiciousRegions();

	m_seriesVersion = SeriesVersion( "-1.0" );
	m_bRotateProneToSupine = false;

} // Reset()


/**
 * FreeAllLargeVolumesButPaint.
 */
void Dataset::FreeAllLargeVolumesButPaint()
{
	Triple<uint4> vDim(0.0F, 0.0F, 0.0F);
	m_fields.m_sldInterior.Resize(vDim);
	if( rdrGlobal.m_bUseULDvolume )
	{
		m_fields.m_sldExterior.Resize(vDim); //:: Use this for un-cleansed SLD, i.e. the ULD volume
	}
	m_fields.m_volumeRender.Resize(vDim);
} // FreeAllLargeVolumesButPaint()


/**
 * Smooths out the centerlines by box filtering.
 */
void Dataset::SmoothCenterlines()
{
	for (uint4 uSegmentIndex=0; uSegmentIndex<m_segmentList.GetNumUnsortedSegments(); uSegmentIndex++)
	{
		Segment & segment = m_segmentList.GetUnsortedSegment(uSegmentIndex);
		// loop over all centerlines
		for (uint4 uCenterlineIndex=0; uCenterlineIndex<segment.GetNumCenterlines(); uCenterlineIndex++)
		{
			NibbleCenterlineEnds(segment.GetCenterline(uCenterlineIndex));
			const float smoothingRadius = 9.0F;
			ConstrainedSkeletonSmooth(segment.GetCenterline(uCenterlineIndex), smoothingRadius);
		} // loop over all centerlines
	} // loop over all segments
}

	
/**
 * Perform initialization after both volumes are loaded and verified.
 * Some things require that both the entire volume is loaded and the segments have been verified by the user before they
 * can be calculated. This function is where to put these and then it can be called once both of the conditions are true.
 * The reason it is difficult is that the order in which these two conditions become true is arbitrary.
 */
bool Dataset::InitializeVisualization()
{
	// mark surfaces
	if (m_fields.m_paintVol.m_bSurfacesNeedReinit)
	{
		m_fields.m_paintVol.InitializeMarkedSurfaces(m_segmentList, m_fields.m_ins);
		// Visualization initialized
		m_bNeedVisualizationReinit = false;

		// Initialize the viewpoint at the start of the first segment
		GoToFirstSegment();

		// flush the SRT ROI list
		if(  m_suspiciousRegions.GetCount() > 0 )
		{
			m_suspiciousRegions.ClearFindings(*this);
		}
	}

	// initialize the label volume for 2D MPR rendering
	m_fields.m_sliceRendererData.GetLabelVolume()->Clear();

	int4 iIndexNode = m_submarine.GetClosestSkeletonIndex();
	uint4 uJoinedSkelIndex = m_joinedSkeleton.ConvertToJoinedIndex(m_segmentList.GetCurrentSegment(),iIndexNode);
	for(uint4 s=0;s<m_straightSegmentNodes.size();s++)
	{
		if(iIndexNode > m_straightSegmentNodes[s]) m_currentSegment = s;
	}

	// Reset (clear) list of missed patches
	m_vPatchList.clear();

	// initialize SRT ROI
	InitializeSRTroi();

	/// Note, the SRTroi must initialize before the CADFindings for the 
	/// current code setting. If the sliceRenderer label volume could be
	/// sessioned out, this limitation could be dropped. Dongqing 06242008

	// initialize cad findings
	InitializeCadFindings();

	// clear out the component array in case the user has changed the selected segments
	m_componentArray = vxComponentArray();

	// check for a short colon
	const float4 fMinimumColonLengthMM(100.0F);
	return (m_segmentList.GetTotalLengthMM() >= fMinimumColonLengthMM);
} // InitializeVisualization()


/**
 * Prepare CAD findings for visualization
 */
void Dataset::InitializeCadFindings()
{
	if (m_joinedSkeleton.GetNumOfElements() <= 0 || m_cadFindings.GetCount() < 1 )
	{
		return;
	} // if

	const int iCount =  m_cadFindings.GetCount();

	/// get the iterator of the INS volume ready
	vxBlockVolumeIterator<uint1> iti( m_fields.m_ins );

	register int4 i, j;

	float4 fMaximumColonDiameter = 40.0F;  // in millimeter. This is for determination if a finding is in the lumen where the centerline node is in
	bool bIsColonicFinding = true;

	/// going through all findings in the CAD results XML
	for( i=0; i<iCount; i++ )
	{
		bIsColonicFinding = true;

		vxCadFinding & rFinding = m_cadFindings.GetFinding( i );

		/// This is the skeleton for all selected segments together
		Skeleton * pSkeleton = static_cast< Skeleton * >( & m_joinedSkeleton );

		/// There are three situations for the CAD finding input. They are associated to 3
		/// different CAD XML file version. It deals with different ways.
		///======================================================================
		if( rFinding.GetNormalMM().GetUnits() == vxCAD::vxCadVector::MILLIMETER && 
				rFinding.GetNormalMM().GetVector().X() != 0 && rFinding.GetNormalMM().GetVector().Y() != 0 &&
				rFinding.GetNormalMM().GetVector().Z() != 0)
		{ // The Directional Vector is available for the current finding

			const Vector3D<float4> vfNormal( rFinding.GetNormalMM().GetVector() ); 
			Vector3D<float4> vfCentroid( rFinding.GetCentroidMM().GetVector() );
			Vector3D<int4> viAirVoxelLocation(0,0,0);

			bool bFoundAir = false;
			/// search away from the centroid along the vfNormal direction and 
			/// stop at the first air voxel in the INS.
			for( j=0; j<100; j++ )
			{
				vfCentroid = vfCentroid + vfNormal;

				viAirVoxelLocation.m_x = int4(vfCentroid.m_x/m_fields.m_ins.GetHeader().GetVoxelUnits().m_x + 0.5F);
				viAirVoxelLocation.m_y = int4(vfCentroid.m_y/m_fields.m_ins.GetHeader().GetVoxelUnits().m_y + 0.5F);
				viAirVoxelLocation.m_z = int4(vfCentroid.m_z/m_fields.m_ins.GetHeader().GetVoxelUnits().m_z + 0.5F);

				iti.SetPos(viAirVoxelLocation);

				if( iti.GetVoxel() > 0 )
				{ /// encounter an air voxel in the INS
					vfCentroid.m_x = viAirVoxelLocation.m_x*m_fields.m_ins.GetHeader().GetVoxelUnits().m_x;
					vfCentroid.m_y = viAirVoxelLocation.m_y*m_fields.m_ins.GetHeader().GetVoxelUnits().m_y;
					vfCentroid.m_z = viAirVoxelLocation.m_z*m_fields.m_ins.GetHeader().GetVoxelUnits().m_z;

					bFoundAir = true;
					rFinding.SetSegment( iti.GetVoxel() ); /// The reset is needed for synchronization display
					break;
				} // if
			} // for j

			if( bFoundAir )
			{ /// reaching a lumen
				int4 iN = pSkeleton->GetNumOfElements();

				// search for the point on the centerline nearest to the finding air voxel
				float4 fMinDistance = Vector3Df( pSkeleton->GetWorldPos( 0 ) - vfCentroid ).GetLength();
				float4 fCurrentDistance;
				int4 iSelectedJoinedIndex = 0;

				for( j=1; j<iN; j++ )
				{
					fCurrentDistance = Vector3Df( pSkeleton->GetWorldPos(j) - vfCentroid ).GetLength();
					if( fCurrentDistance < fMinDistance )
					{
						fMinDistance = fCurrentDistance;
						iSelectedJoinedIndex = j;
					} // if
				} // for j

				if( fMinDistance > fMaximumColonDiameter )
				{
					bIsColonicFinding = false;
				} // if

				Vector3D<float4> vfNearestNode = Vector3D<float4>( pSkeleton->GetWorldPos(iSelectedJoinedIndex) );

				iti.SetPos( Vector3D<int4>(pSkeleton->GetVolumePos(iSelectedJoinedIndex)) );
				rFinding.SetSegment( iti.GetVoxel() );  /// This is necessary for synchonization of display

				pSkeleton = & m_segmentList.GetUnsortedSegment( m_segmentList.GetUnsortedSegmentIndexFromIdentifier( rFinding.GetSegment() ) ).GetCenterline( 0 );
			
				/// determine the index for the jointed skeleton
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

			} // if bFoundAir == TRUE
			else
			{ /// could not reach lumen ROI near by
				bIsColonicFinding = ExtractSkeletonIndexesForFinding( rFinding, iti, fMaximumColonDiameter );
			} // else bFoundAir == TRUE
		} // if Directional Vector is available for the current finding

		/////================================== The following codes block relys on correct CAD inputs.
		///// That requires the cad partner having good understanding of V3D-Colon segments structure. 
		///// Unfortunately, it is not always the case. DQ 25 January 2010.
		//else if( rFinding.GetSegment() > 0 )
		//{ // The Segment ID is available for the current finding
		//  
		//  pSkeleton = & m_segmentList.GetUnsortedSegment( m_segmentList.GetUnsortedSegmentIndexFromIdentifier( rFinding.GetSegment() ) ).GetCenterline( 0 );

		//  int4 iN = pSkeleton->GetNumOfElements();

		//  // search for the point on the centerline nearest to the finding air voxel. The centerline index is the CAD vender provided.
		//  float4 fMinDistance = Vector3Df( pSkeleton->GetWorldPos( 0 ) - rFinding.GetCentroidMM().GetVector() ).GetLength();
		//  float4 fCurrentDistance;
		//  int4 iSelectedIndex = 0;

		//  for( j=1; j<iN; j++ )
		//  {
		//    fCurrentDistance = Vector3Df( pSkeleton->GetWorldPos(j) - rFinding.GetCentroidMM().GetVector() ).GetLength();
		//    if( fCurrentDistance < fMinDistance )
		//    {
		//      fMinDistance = fCurrentDistance;
		//      iSelectedIndex = j;
		//    } // if
		//  } // for j
		// 
		//  Vector3D<float4> vfNearestNode = Vector3D<float4>( pSkeleton->GetWorldPos(iSelectedIndex) );

		//  pSkeleton = static_cast< Skeleton * >( & m_joinedSkeleton );
		//  
		//  /// determine the index for the jointed skeleton
		//  iN = pSkeleton->GetNumOfElements();
		//  int4 iSelectedJoinedIndex = iSelectedIndex;
		//  for( j=0; j<iN; j++ )
		//  {
		//    if( vfNearestNode == Vector3D<float4>(pSkeleton->GetWorldPos(j)) )
		//    {
		//      iSelectedJoinedIndex = j;
		//      break;
		//    } // if
		//  } // for j
		//  
		//  m_session.m_mapCadFindingInfo[ rFinding.GetId() ].SetJoinedSkeletonIndex( iSelectedJoinedIndex );
		//  m_session.m_mapCadFindingInfo[ rFinding.GetId() ].SetSkeletonIndex( iSelectedIndex );        

		//} // else if the Segment ID is available for the current finding

		///===============================================================
		else
		{ // the Segment ID is not available for the current finding
			bIsColonicFinding = ExtractSkeletonIndexesForFinding( rFinding, iti, fMaximumColonDiameter );
		} // else the Segment ID is not available for the current finding

		m_session.m_mapCadFindingInfo[ rFinding.GetId() ].SetInLumen( bIsColonicFinding ); 

	} // for all findings available


	//:::::::::: check the cad ROI and label it in both label and paint vol  
	{       
		vxShareableObject< vxComponentArray > * pComponentArray( m_fields.m_sliceRendererData.GetComponents() );

		vxBlockVolumeIterator<uint2> it( *m_fields.m_sliceRendererData.GetLabelVolume() );

		uint1 * pPaintVol = m_fields.m_paintVol.GetDataPtr();
		
		if( m_fields.m_paintVol.GetDim() != m_fields.m_sliceRendererData.GetLabelVolume()->GetHeader().GetVolDim() )
		{
			LogErr("The compressed 3D switch is on. That does not support the CAD ROI display", "Dataset.C", "InitializeCadFindings");
		}
		else 
		{
			const int iX  = m_fields.m_paintVol.GetDim().m_x;
			const int iS  = iX*m_fields.m_paintVol.GetDim().m_y;
			const int iNN = iS*m_fields.m_paintVol.GetDim().m_z;

			register int j;
			register vxCadVoxelPosition pos;
			register uint2 uLabel;

			for( register uint4 iDim=0; iDim<iCount; iDim++ )
			{
				uint2 uHistoryIndex = pComponentArray->AddComponent("CAD ROI", PixelRGBA<uint1>(128, 197, 255, 255) );
				uLabel = (*pComponentArray)[uHistoryIndex]->GetLabel();

				/// We need to reset the component label since we have never sessioned them out!!! Dongqing 05/28/2008
				m_cadFindings.GetFinding(iDim).GetVolumeROI().SetLabelIndex(uLabel);

				vxCadROI roi = m_cadFindings.GetFinding( iDim ).GetVolumeROI();

				if( roi.GetCount() > 0 )
				{
					/// label the volume ROI
					for( j=roi.GetCount()-1; j>=0; j-- )
					{
						pos = roi.GetVoxel( j );

						it.SetPos(pos.GetX(), pos.GetY(), pos.GetZ());
						it.SetVoxel( uLabel );

						pPaintVol[pos.GetX() + iX*pos.GetY() + iS*pos.GetZ()] = CADROI;

					} // for j
				} // if ROI not empty

				roi = m_cadFindings.GetFinding( iDim ).GetSurfaceROI();

				/// label the surface ROI
				if( roi.GetCount() > 0 )
				{
					for( j=roi.GetCount()-1; j>=0; j-- )
					{
						pos = roi.GetVoxel( j );
						pPaintVol[pos.GetX() + iX*pos.GetY() + iS*pos.GetZ()] = CADROI;
					} // for j
				} // if ROI not empty

			} // for iDim
		} // if dimension matched

		vxHistogramUpdaterLabel<uint2>::Update( *m_fields.m_sliceRendererData.GetLabelVolume() );
	} // end check the cad ROI and label it in both label and paint vol

	// sort the findings
	Dataset::m_pCurrentSortDataset = this;
	m_cadFindings.SortFindings( SortBySkeletonIndex );
	Dataset::m_pCurrentSortDataset = (Dataset *) NULL;

} // InitializeCadFindings()


/** 
 *   initialize SRT ROI 
 */
void Dataset::InitializeSRTroi()
{
	int N = m_suspiciousRegions.GetCount();

	if( N > 0 )
	{ // there is SRT findings

		vxShareableObject< vxComponentArray > * pComponentArray( m_fields.m_sliceRendererData.GetComponents() );

		vxBlockVolumeIterator<uint2> it( *m_fields.m_sliceRendererData.GetLabelVolume() );

		register int i, j, k;
		vxCadVoxelPosition pos;
		vxCadROI roi;
		uint2 uNewLabel, uHistoryIndex;

		// initialize the ROI in the label volume
		for( i=0; i<N; i++ )
		{
			uHistoryIndex = pComponentArray->AddComponent("Suspicious ROI", PixelRGBA<uint1>(255, 0, 0, 255) );
			uNewLabel = (*pComponentArray)[uHistoryIndex]->GetLabel();
						
			/// We need to reset the component label since we have never sessioned them out!!! Dongqing 05/28/2008
			m_suspiciousRegions.GetFinding(i).GetVolumeROI().SetLabelIndex(uNewLabel);

			roi = m_suspiciousRegions.GetFinding(i).GetVolumeROI();
			
			j = 0;
			j = roi.GetCount();

			if( j > 0 )
			{
				for( k=0; k<j; k++ )
				{
					pos = roi.GetVoxel(k);

					it.SetPos( pos.GetX(), pos.GetY(), pos.GetZ() );
					it.SetVoxel( uNewLabel );  // This will gurantee the new generate component label index 
																		 // will not larger than the actual number of components

					//pPaintVol[pos.GetX() + iX*pos.GetY() + iS*pos.GetZ()] = CADROI;
				} // for k

			} // if there is voxel in the ROI
		} // all findings i
	} // if there is SRT ROI

	/// this is necessary. Otherwise, it wouldn't displays correctly
	vxHistogramUpdaterLabel<uint2>::Update( *m_fields.m_sliceRendererData.GetLabelVolume() );
} // initializeSRTroi()




/**
 * retrieve the world position of the nearest skeleton to the finding
 *
 * @param finding the CAD finding in question
 *
 * @return the world position of the nearest skeleton to the finding
 */
Point3D<float4> Dataset::GetNearestSkeletonPosition( const vxCadFinding & finding )
{

	// get the proper skeleton
	Skeleton * pSkeleton = static_cast< Skeleton * >( & m_joinedSkeleton );
	if ( finding.GetSegment() != 0 )
		pSkeleton = & m_segmentList.GetUnsortedSegment( m_segmentList.GetUnsortedSegmentIndexFromIdentifier( finding.GetSegment() ) ).GetCenterline( 0 );

	// return the world position
	const ColonSession::FindingInfo & info = m_session.m_mapCadFindingInfo[ finding.GetId() ];
	return pSkeleton->GetWorldPos( info.GetSkeletonIndex() );

} // GetNearestSkeletonPosition()


/**
 * retrieve the DFS value of the nearest skeleton to the finding
 *
 * @param finding the CAD finding in question
 *
 * @return the DFS value of the nearest skeleton to the finding
 */
float4 Dataset::GetNearestSkeletonDFS( const vxCadFinding & finding )
{

	// get the proper skeleton
	Skeleton * pSkeleton = static_cast< Skeleton * >( & m_joinedSkeleton );
	if ( finding.GetSegment() != 0 )
		pSkeleton = & m_segmentList.GetUnsortedSegment( m_segmentList.GetUnsortedSegmentIndexFromIdentifier( finding.GetSegment() ) ).GetCenterline( 0 );

	// return the world position
	const ColonSession::FindingInfo & info = m_session.m_mapCadFindingInfo[ finding.GetId() ];

	const Segment & segment = m_segmentList.GetSegment( m_segmentList.GetSortedSegmentIndex( m_segmentList.GetUnsortedSegmentIndexFromIdentifier( finding.GetSegment() )) );
	float4 fDistFromRectumMM = segment.GetOffsetDistanceFromRectumMM();
	fDistFromRectumMM += segment.IsRegularDirection()? pSkeleton->GetWorldDfs(info.GetSkeletonIndex()) : pSkeleton->GetWorldDfe(info.GetSkeletonIndex());

	return fDistFromRectumMM;
} // GetNearestSkeletonDFS()


/**
 * Perform initialization after both volumes are loaded and verified.
 * Some things require that both the entire volume is loaded and the segments have been verified by the user before they
 * can be calculated. This function is where to put these and then it can be called once both of the conditions are true.
 * The reason it is difficult is that the order in which these two conditions become true is arbitrary.
 */
void Dataset::UpdateSegmentOrder()
{
	Timer regTimer; regTimer.Reset();     
	// recalculate the segment offsets
	m_segmentList.UpdateTotalLength();
		
	CreateJoinedSkeleton();
	LocateFeatures();
	CreateSkeletonSegments();
	
} // OnBothVolumeLoadedAndDoneVerification()


/**
 * Jumps the view position to the start or end of the current segment centerline.
 */
bool Dataset::MoveViewpointToSegmentEnd(const bool bStartOrEnd)
{
	Segment & segment = m_segmentList.GetCurrentSegment();
	if (bStartOrEnd)
		m_submarine.SetViewSkeleton(segment.GetBeginningSkelNode(), !segment.IsRegularDirection());
	else
		m_submarine.SetViewSkeleton(segment.GetEndingSkelNode(), segment.IsRegularDirection());

	return true;
} // MoveViewpointToSegmentEnd()


/**
 * Jumps the viewpoint to the first segment.
 */
void Dataset::GoToFirstSegment()
{
	m_segmentList.GoToFirstSegment();
	MoveViewpointToSegmentEnd(true);
} // GoToFirstSegment()


/**
 * Jumps the viewpoint to the last segment.
 */
void Dataset::GoToLastSegment()
{
	m_segmentList.GoToLastSegment();
	MoveViewpointToSegmentEnd(false);
} // GoToLastSegment()


/**
 * Jumps the viewpoint one segment toward the cecum.
 */
void Dataset::GoToNextSegmentToCecum()
{
	// check if there are any more segments to switch to
	if (m_segmentList.GoToNextSegmentToCecum())
	{
		MoveViewpointToSegmentEnd(true);
	}
	else
	{
		// last segment, jump to opposite end 
		MoveViewpointToSegmentEnd(false);
	} // if not last segment
} // GoToNextSegmentToCecum()


/**
 * Jumps the viewpoint one segment toward the rectum.
 */
void Dataset::GoToNextSegmentToRectum()
{
	// check if there are any more segments to switch to
	if (m_segmentList.GoToNextSegmentToRectum())
	{
		MoveViewpointToSegmentEnd(false);
	}
	else
	{
		// last segment, jump to opposite end
		MoveViewpointToSegmentEnd(true);
	} // if not last segment
} // GoToNextSegmentToRectum()


/**
* Gets the sorted segment index corresponding to a particular point in space.
*
*  @param Point<float> & positionWorld
*  @return  the sorted segment index corresponding to a particular point in space.
*/
int4 Dataset::GetSortedSegmentIndex(const Point<float> & positionWorld)
{
	// determine the volume coordinates
	Point<float> positionVolumeFloat( RenderUtil::GetWorldToVolumeMatrix( m_fields.m_ins.GetHeader() ) * positionWorld);
	Point<unsigned short> positionVolumeUS;
	positionVolumeUS = positionVolumeFloat;

	// determine the segment identifier
	vxBlockVolumeIterator<uint1> insIter( m_fields.m_ins );
	const uint1 iSegmentIdentifier = insIter.Peek( positionVolumeUS.m_x, positionVolumeUS.m_y, positionVolumeUS.m_z );

	// return the sorted segment index (0=rectum...n-1=cecum)
	return m_segmentList.GetSortedSegmentIndexFromIdentifier(iSegmentIdentifier);
} // GetSortedSegmentIndex()


/**
 * Gets the sorted segment index corresponding to a particular point in space.
 *
 *  @param Point<int2> & positionVol
 *  @return  the sorted segment index corresponding to a particular point in space.
 */
int4 Dataset::GetSortedSegmentIndex(const Point<uint2> & positionVol)
{
	// determine the volume coordinates
	Point<float> positionVolumeFloat( positionVol );
	Point<unsigned short> positionVolumeUS;
	positionVolumeUS = positionVolumeFloat;

	// determine the segment identifier
	vxBlockVolumeIterator<uint1> insIter( m_fields.m_ins );
	const uint1 iSegmentIdentifier = insIter.Peek( positionVolumeUS.m_x, positionVolumeUS.m_y, positionVolumeUS.m_z );

	// return the sorted segment index (0=rectum...n-1=cecum)
	return m_segmentList.GetSortedSegmentIndexFromIdentifier(iSegmentIdentifier);
} // GetSortedSegmentIndex()


/**
* Select the segment corresponding to a particular point in space.
*
*  @param Point<float> & positionWorld
*/
void Dataset::SelectSegment(const Point<float> & positionWorld)
{
	const int4 iSortedIndex = GetSortedSegmentIndex(positionWorld);
	// set the current segment in order to display the current centerline
	if (iSortedIndex > -1)
	{
		m_segmentList.SetCurrentSegmentIndex(iSortedIndex);
	}
} // SelectSegment()

/**
 * Retrieves the colon segment enumeration from the given offset.
 *
 *  @param float4 fOffsetMM
 */
ColonSegmentEnum Dataset::GetColonSegmentFromOffset(const float4 fOffsetMM) const
{
	const float fPercent = (fOffsetMM * 100.0F) / m_segmentList.GetTotalLengthMM();

	if (fPercent > 95.0F)
		return eCECUM;
	
	else if (fPercent > 70.0F)
		return eASCENDING;
	
	else if (fPercent > 50.0F)
		return eTRANSVERSE;
	
	else if (fPercent > 30.0F)
		return eDESCENDING;
	
	else if (fPercent > 5.0F)
		return eSIGMOID;
	
	return eRECTUM;
} // GetColonSegmentFromOffset()


/**
 * Retrieves the current colon segment enumeration.
 */
ColonSegmentEnum Dataset::GetCurrentColonSegment()
{
	return GetColonSegmentFromOffset(GetCurrentDistanceFromRectumMM());
} // GetCurrentColonSegment()


/**
 * Smooths out the skeleton, but keeps it within the same segment.
 * It smooths out the world positions and tangents only, it doesn't
 * modify the volume positions since they are constrained to grid
 * positions.  It also recomputes the skeleton lengths.
 *
 * @param skeleton
 * @param fRadius
 */
void Dataset::ConstrainedSkeletonSmooth(Skeleton & skeleton, const float4 fRadius)
{
	Point<uint2> nodePos;
	nodePos = skeleton.GetVolumePos(0);
	vxBlockVolumeIterator<uint1> insIter( m_fields.m_ins );
	uint2 uThisSegment = insIter.Peek( nodePos.m_x, nodePos.m_y, nodePos.m_z );
	
	// copy the skeleton in order to resample easily
	Skeleton copy(skeleton);
	
	// loop over all the skeleton nodes
	for (int4 i=0; i<skeleton.GetNumOfElements(); i++)
	{
		// loop over all other nodes which can influence this one
		int4 iStartIndex = int4(floor(i-fRadius));
		int4 iEndIndex = int4(ceil(i+fRadius));
		Point<float> vectorSum;
		float4 fWeightSum = 0.0F;
		for (int4 j=iStartIndex; j<=iEndIndex; j++)
		{
			if (j >= 0 && j < skeleton.GetNumOfElements())
			{
				// weigh the samples
				//const float distance = Bound(0.0F, float(fabs(i - j)), radius);
				//const float weight = Bound(0.0F, 1.0F - distance / radius, 1.0F);
				const float4 fWeight = 1.0F;
				fWeightSum += fWeight;
				vectorSum += Vector<float>(copy.GetWorldPos(j));
			}
		} // end loop over other nodes
		
		// normalize by total weight
		if (fWeightSum != 0)
		{
			vectorSum /= fWeightSum;
		} // valid denominator
		
		// decide if it is still within the volume
		// convert to volume coordinates
		Point<float4> newPositionVolume( RenderUtil::GetWorldToVolumeMatrix( m_fields.m_ins.GetHeader() ) * vectorSum);
		// round to nearest point
		Point<uint2> newPositionVolumeUS(newPositionVolume.m_x + 0.5,
																		 newPositionVolume.m_y + 0.5,
																		 newPositionVolume.m_z + 0.5);
		uint1 uSegmentValue = insIter.Peek( newPositionVolumeUS.m_x, newPositionVolumeUS.m_y, newPositionVolumeUS.m_z );
		if (uSegmentValue == uThisSegment)
		{
			skeleton.SetWorldPos(i, vectorSum);
		}
	} // end loop over all skeleton nodes
	
	// central difference tangents
	skeleton.ComputeTangents(fRadius);
	skeleton.ComputeUpVectors();
	
	// Recompute the distances as well
	skeleton.RecalculateDistances();
} // ConstrainedSkeletonSmooth()


/**
 * Nibble off the ends of the centerline that are in a narrow opening.
 *
 *  @param skeleton
 */
void Dataset::NibbleCenterlineEnds(Skeleton & skeleton)
{
	// eat nodes from the start of the centerline if it is too close to the sld
	vxBlockVolumeIterator<uint1> insIter( m_fields.m_ins );
	while (skeleton.GetNumOfElements() > 1)
	{
		Point<uint2> vNodePosition(skeleton.GetVolumePos(0));
		const uint1 uInsValue = insIter.Peek( vNodePosition.m_x, vNodePosition.m_y, vNodePosition.m_z );
		bool bTooCloseToEnd = false;
		for(int4 iDeltaZ = -1; iDeltaZ <= 1; iDeltaZ++)
		{
			for(int4 iDeltaY = -1; iDeltaY <= 1; iDeltaY++)
			{
				for(int4 iDeltaX = -1; iDeltaX <=1; iDeltaX++)
				{
					Point<uint2> vDeltaPosition(vNodePosition+Vector<uint2>(iDeltaZ, iDeltaY, iDeltaX));
					if (vDeltaPosition.m_x < 2 || vDeltaPosition.m_x > (m_vDim.m_x-2) ||
							vDeltaPosition.m_y < 2 || vDeltaPosition.m_y > (m_vDim.m_y-2) ||
							vDeltaPosition.m_z < 2 || vDeltaPosition.m_z > (m_vDim.m_z-2) ||
							insIter.Peek( vDeltaPosition.m_x, vDeltaPosition.m_y, vDeltaPosition.m_z ) != uInsValue )
					{
						bTooCloseToEnd = true;
					}
				}
			}
		}
		if (bTooCloseToEnd)
		{
			skeleton.Erase(0);
		} 
		else 
		{
			break;
		} // endif less than minimum
	} // endwhile still eating nodes
	
	// eat nodes from the end of the centerline if it is too close to the sld
	while (skeleton.GetNumOfElements() > 1)
	{
		const uint2 uLastIndex = skeleton.GetNumOfElements()-1;
		Point<uint2> vNodePosition(skeleton.GetVolumePos(uLastIndex));
		const uint1 uInsValue = insIter.Peek( vNodePosition.m_x, vNodePosition.m_y, vNodePosition.m_z );
		bool bTooCloseToEnd = false;
		for(int4 iDeltaZ = -1; iDeltaZ <= 1; iDeltaZ++)
		{
			for(int4 iDeltaY = -1; iDeltaY <= 1; iDeltaY++)
			{
				for(int4 iDeltaX = -1; iDeltaX <=1; iDeltaX++)
				{
					Point<uint2> vDeltaPosition(vNodePosition+Vector<uint2>(iDeltaZ, iDeltaY, iDeltaX));
					if (vDeltaPosition.m_x < 2 || vDeltaPosition.m_x > (m_vDim.m_x-2) ||
							vDeltaPosition.m_y < 2 || vDeltaPosition.m_y > (m_vDim.m_y-2) ||
							vDeltaPosition.m_z < 2 || vDeltaPosition.m_z > (m_vDim.m_z-2) ||
							insIter.Peek( vDeltaPosition.m_x, vDeltaPosition.m_y, vDeltaPosition.m_z ) != uInsValue )
					{
						bTooCloseToEnd = true;
					}
				}
			}
		}
		if (bTooCloseToEnd)
		{
			skeleton.Erase(uLastIndex);
		} 
		else 
		{
			break;
		} // endif less than minimum
	} // endwhile still eating nodes
	
	// Recompute the distances as well
	skeleton.RecalculateDistances();

} // NibbleCenterlineEnds()


/**
 * Returns the number of mm we are from the rectum.
 */
float4 Dataset::GetCurrentDistanceFromRectumMM()
{

	return GetDistanceFromRectumMM(
		m_segmentList.GetCurrentSegment(),
		m_segmentList.GetCurrentSkeleton(),
		m_submarine.GetPositionVolume() );

} // GetCurrentDistanceFromRectum()


/**
 * Returns the number of mm this is from the rectum.
 *
 * @param segment        the segment the point is in
 * @param skeleton       the skeleton we are in
 * @param positionVolume the position (in volume coordinates) to compute the distance
 *
 * @return the distance from the rectum in MM
 */
float4 Dataset::GetDistanceFromRectumMM( Segment & segment, Skeleton & skeleton, const Point3D<float4> & positionVolume )
{

	// don't try to access data that is not yet ready
	if ( ! rdrGlobal.IsLoadingComplete() ) return 0;

	float4 fDistanceFromRectumMM = segment.GetOffsetDistanceFromRectumMM();
	uint2 uNumOfNode = skeleton.GetNumOfElements();

	// it is the current dataset, not rdrGloabl.m_pCurrDataset, that will be used.
	// uint2 uCsi = m_fields.GetClosestSkeletonIndex();  <--- 'dataset'
	uint2 uCsi = m_fields.GetClosestSkeletonIndex( positionVolume );

	// make sure the uCsi should be not out of bound (bug 000567).
	if (uCsi <= 0)
		uCsi = m_uCsi;
	else
		m_uCsi = uCsi;

	if (uCsi >= uNumOfNode)
	{
		uCsi = uNumOfNode - 1;
		m_uCsi = uCsi;
	}

	if (segment.IsRegularDirection() == true) 
	{ 
		fDistanceFromRectumMM += skeleton.GetSkelNode(uCsi).m_worldDfs;
	} 
	else 
	{
		fDistanceFromRectumMM += skeleton.GetSkelNode(uCsi).m_worldDfe;
	}// endif direction
	
	return fDistanceFromRectumMM;

} // GetDistanceFromRectumMM()


/**
 * Writes all dataset session data to disk.
 *
 * @param bSavePaintVol   Whether or not to also save paint volume data
 * @return                Success/failure
 */
bool Dataset::SaveSessionData(const bool bSavePaintVol)
{
	try
	{
		if (IsLoadingComplete() == false)
		{ /// this tells the dataset is not loaded. It is not for error indication. 
			return true;
		}

		if( m_session.GetVersion() < 3.0F || rdrGlobal.m_bReadOnly )
		{ 
			return true;
		}
		
		bool bResult(true);

		Timer sessionTimer; sessionTimer.Reset();

		bResult &= m_session.Write(m_sSessionPrefix, *this);

		LogFyi(std::string("Writing Session File took ") + ToAscii(sessionTimer.AsString().c_str()) + std::string(" seconds to complete.") ,"Dataset","SaveSessionData"); sessionTimer.Reset();  

		if (bSavePaintVol)
		{
			// Write the current paint volume to disk (i.e. the only session data not located in ColonSession class for now)     
			m_fields.m_paintVol.WriteData(m_sSessionPrefix + vx::MarkedVolumeExt,false);

			LogFyi(std::string("Writing PaintVol File took ") + ToAscii(sessionTimer.AsString().c_str()) + std::string(" seconds to complete.") ,"Dataset","SaveSessionData"); sessionTimer.Reset();  
		}

		return bResult;
	}
	catch (...)
	{
	}

	return false;
} // SaveSessionData()

	
/**
 * Calculate the percent of the colon which has been examined.
 *
 */
float4 Dataset::GetPercentExamined() const
{
	return m_fields.m_paintVol.GetPercentExamined();
} // GetPercentExamined()


// Constants used to convert gradient magnitude to [0, 1]
const float4 ONE_OVER_MAX_GRADIENT = 1.0F/441.673F; // sqrt(255*255 *3)

/**
 * Build the preclassified and gradient-modulated volume for TTP
 *
 */
void Dataset::BuildOpacityVol()
{
	
	LinVolUC * pVol = &(m_fields.m_volumeRender);
	LinVolUC * pVolOpa = &(m_fields.m_opacityVol);

	const Triple<uint4> dimSmall(256,256,256);
	*pVolOpa = LinVolUC(dimSmall, 8*sizeof(uint1));
	uint1 * const inVol  = (unsigned char *)pVol->GetDataPtr();
	uint1 * const outVol = (unsigned char *)pVolOpa->GetDataPtr();

	const uint4 flc_xStepSmall = 1;
	const uint4 flc_yStepSmall = dimSmall.m_x;
	const uint4 flc_zStepSmall = dimSmall.m_x*dimSmall.m_y;
	const uint4 flc_xStepLarge = 1;
	const uint4 flc_yStepLarge = pVol->GetDim().m_x;
	const uint4 flc_zStepLarge = pVol->GetDim().m_x*pVol->GetDim().m_y;
	
	float4 xScale = float4(pVol->GetDim().m_x) / float4(dimSmall.m_x);
	float4 yScale = float4(pVol->GetDim().m_y) / float4(dimSmall.m_y);
	float4 zScale = float4(pVol->GetDim().m_z) / float4(dimSmall.m_z);
	
	for (uint4 voxel_z=1; voxel_z < dimSmall.m_z-1; voxel_z++)
	{
		for (uint4 voxel_y=1; voxel_y < dimSmall.m_y-1; voxel_y++)
		{
			for (uint4 voxel_x=1; voxel_x < dimSmall.m_x-1; voxel_x++)
			{
				uint4 voxelLarge_x = (float4(voxel_x) * xScale) + 0.5F;
				uint4 voxelLarge_y = (float4(voxel_y) * yScale) + 0.5F;
				uint4 voxelLarge_z = (float4(voxel_z) * zScale) + 0.5F;
				uint4 indexOffsetSmall = voxel_x * flc_xStepSmall + voxel_y * flc_yStepSmall + voxel_z * flc_zStepSmall;
				uint4 indexOffsetLarge = voxelLarge_x * flc_xStepLarge + voxelLarge_y * flc_yStepLarge + voxelLarge_z * flc_zStepLarge;
				uint1 * dptr = inVol + indexOffsetLarge;

				float4 gradx, grady, gradz;
				CentralDifferenceGradient(dptr, flc_xStepLarge, flc_yStepLarge, flc_zStepLarge, gradx, grady, gradz);
				float4 fGradMagnitude = (sqrt(gradx*gradx + grady*grady + gradz*gradz)*ONE_OVER_MAX_GRADIENT);
				if(fGradMagnitude>1.0)fGradMagnitude=1.0;
				*(outVol + indexOffsetSmall) = (unsigned char)(fGradMagnitude*255);
			}
		}
	}

	m_fields.SetOpacityVolumeCreated(true);
} // BuildOpacityVol()


void Dataset::BuildHomogenityVolume()
{
	// Try to create the testVolume
	if(!m_fields.m_bTestVolValid)
	{
		rdrGlobal.m_bHomogenityCreated = m_fields.LazyCreateTestVolume();
	}
	if(rdrGlobal.m_bHomogenityCreated)
	{
		uint1 * pTestVol = m_fields.m_testVolumeUC.GetDataPtr();
		uint1 * pVol = m_fields.m_volumeRender.GetDataPtr();
		vxBlockVolume<uint1> & insVol = m_fields.m_ins;
		vxBlockVolumeIterator<uint1> insIter( insVol );
		uint4 yStep = m_vDim.m_x;
		uint4 zStep = m_vDim.m_y*m_vDim.m_x;
		memset(pTestVol,0,m_vDim.m_x*m_vDim.m_y*m_vDim.m_z);

		Triple<int4> dims = m_vDim;    
		for(int4 z=4; z<dims.m_z-4; z++)
		{
			for(int4 y=4; y<dims.m_y-4; y++)
			{
				for(int4 x=4; x<dims.m_x-4; x++)
				{
					int4 startX, stopX, startY, stopY, startZ, stopZ;
					// search for the extent in each direction of the "passing" voxels
					for(startX=0; startX>=-4 && pVol[z*zStep+y*yStep+(x+startX)] < HUconverter::HU2uchar(-400) && insIter.Peek(x+startX,y,z) == 0; startX--); // do nothing
					for(stopX =0; stopX <= 4 && pVol[z*zStep+y*yStep+(x+stopX )] < HUconverter::HU2uchar(-400) && insIter.Peek(x+stopX ,y,z) == 0; stopX++ ); // do nothing
					for(startY=0; startY>=-4 && pVol[z*zStep+(y+startY)*yStep+x] < HUconverter::HU2uchar(-400) && insIter.Peek(x,y+startY,z) == 0; startY--); // do nothing
					for(stopY =0; stopY <= 4 && pVol[z*zStep+(y+stopY )*yStep+x] < HUconverter::HU2uchar(-400) && insIter.Peek(x,y+stopY ,z) == 0; stopY++ ); // do nothing
					for(startZ=0; startZ>=-4 && pVol[(z+startZ)*zStep+y*yStep+x] < HUconverter::HU2uchar(-400) && insIter.Peek(x,y,z+startZ) == 0; startZ--); // do nothing
					for(stopZ =0; stopZ <= 4 && pVol[(z+stopZ )*zStep+y*yStep+x] < HUconverter::HU2uchar(-400) && insIter.Peek(x,y,z+stopZ ) == 0; stopZ++ ); // do nothing
					// 4 voxels in a row means start-stop = 3, but the way the loops work above we go the the first "non-Passing" voxel
					if( ((stopX-startX)>=5) && ((stopY-startY)>=5) && ((stopZ-startZ)>=5) ) pTestVol[z*zStep+y*yStep+x] = 255;
				} // endfor x
			} // endfor y
			//AfxGetMainWnd()->SendMessage(FunctionProgress::WM_REG_SET_FUNCTION_PROGRESS, int4(float4(z)/dims.m_z*100), 0);
		} // endfor z
		//AfxGetMainWnd()->SendMessage(FunctionProgress::WM_REG_SET_FUNCTION_PROGRESS, 0, 0);
		rdrGlobal.m_bShowHomogenity = true;
	} // endif create worked
} // BuildHomogenityVolume


/*
 * Calculate Cross-Section Plane.
 */
void Dataset::CrossSectionCalc()
{
	Point<float4> positionWorld;
	Normal<float4> nOrientation;
	Normal<float4> nUp;
	Point<float4> positionVolume;

	// Get the image configuration
	int2 iObliqueIndex1 = m_fields.GetClosestSkeletonIndex( m_submarine.GetPositionVolume() ) + m_fCrossSecDistance;
	int2 iObliqueIndex2 = m_fields.GetClosestSkeletonIndex( m_submarine.GetPositionVolume() ) - m_fCrossSecDistance;
	if (iObliqueIndex1 < 0) iObliqueIndex1 = 0;
	if (iObliqueIndex2 < 0) iObliqueIndex2 = 0;

	Skeleton & skeleton = m_segmentList.GetCurrentSkeleton();
	SkeletonNode const & skeletonNode1 = skeleton.GetSkelNode(iObliqueIndex1, true);
	SkeletonNode const & skeletonNode2 = skeleton.GetSkelNode(iObliqueIndex2, true);
		
	// Choose the node that is closer to the view direction
	Point<float4> positionWorld1 = skeletonNode1.m_vWorldPos;
	Point<float4> positionWorld2 = skeletonNode2.m_vWorldPos;
	Normal<float4> nDirection1(positionWorld1 - m_submarine.GetPositionWorld());
	Normal<float4> nDirection2(positionWorld2 - m_submarine.GetPositionWorld());
	float4 dot1 = nDirection1.Dot(m_submarine.GetOrientation());
	float4 dot2 = nDirection2.Dot(m_submarine.GetOrientation());
		
	// Get the oblique slice position and orientation and save the closer one
	nUp = m_submarine.GetUpDirection();
	if (dot1 > dot2) 
	{
		m_uCrossSecIndex = iObliqueIndex1;
		positionWorld = skeletonNode1.m_vWorldPos;
		nOrientation = skeletonNode1.m_vWorldTan;
	} 
	else 
	{
		m_uCrossSecIndex = iObliqueIndex2;
		positionWorld = skeletonNode2.m_vWorldPos;
		nOrientation = -(skeletonNode2.m_vWorldTan);
	}
	positionVolume = m_fields.m_volumeRender.GetWorldToVolumeMatrix() * positionWorld;
		
	// Calculate the approximate line of intersection between the vertical view plane
	// and the perpendicular centerline plane
	Normal<float4> nRight = Normal<float4>(m_submarine.GetOrientation().Cross(m_submarine.GetUpDirection()));
	for (int4 i=0; i<10; i++) 
	{
		nUp = Normal<float4>(GetProjectionParallelToPlane(Vector<float4>(nUp), nOrientation));
		nUp = Normal<float4>(GetProjectionParallelToPlane(Vector<float4>(nUp), nRight));
	}
		
	// Compute 4 corners in world coordinates
	Point<float4> bottomLeft, bottomRight, topLeft, topRight;
	Vector<float4> vRight = nOrientation.Cross(nUp);
	Vector<float4> vUp(nUp);

	// Point<float4> m_bottomLeft, m_bottomRight, m_topLeft, m_topRight;
	float4 m_fSizeWorldUnits = 50.0F; 

	// save the slice definition for later volume rendering of the shadow
	m_vObliqueLocation = positionWorld;
	m_vObliqueOrientation = nOrientation;
	m_vObliqueUp = nUp;
		
	m_vObliqueBottomLeft  = positionWorld + vUp *  m_fSizeWorldUnits + vRight * -m_fSizeWorldUnits;
	m_vObliqueBottomRight = positionWorld + vUp *  m_fSizeWorldUnits + vRight *  m_fSizeWorldUnits;
	m_vObliqueTopLeft     = positionWorld + vUp * -m_fSizeWorldUnits + vRight * -m_fSizeWorldUnits;
	m_vObliqueTopRight    = positionWorld + vUp * -m_fSizeWorldUnits + vRight *  m_fSizeWorldUnits;
	m_uObliqueViewIdentifier = m_submarine.GetViewIdentifier();

} // CrossSectionCalc().


/**
 * Dicom patient orientation value
 *
 * @return  dicom patient orientation
 */
SerializedDicom::PatientOrientation Dataset::GetPatientOrientation() const
{
	SerializedDicom::PatientOrientation patientOrientation = SerializedDicom::UNKNOWN;

	/// get the image orientation cosine
	std::string sImageOrientation("");
  sImageOrientation = m_volumeHeader.GetDicomFieldValue("0020", "0037", sImageOrientation);
	
	if( sImageOrientation == "" )
	{
		LogWrn("Could not retrieve patient orientation information", "Dataset.C", "GetPatientOrientation");
		return patientOrientation;
	}

  /// determine the body position based on the cosine

  std::string sBreaker = "/\\,:";
  int4 i, n = sImageOrientation.size();
  int4 j[5], k = 0;
  for( i=0; i<5; i++ )
    j[i] = -1;

  for( i=0; i<n; i++ )
  {
    if( sImageOrientation[i] == sBreaker[0] || sImageOrientation[i] == sBreaker[1] || 
        sImageOrientation[i] == sBreaker[2] || sImageOrientation[i] == sBreaker[3] )
    {
      j[k] = i;
      k++;
    } // if
  } // for i

  if( j[0] < 0 || j[1] < 0 || j[2] < 0 || j[3] < 0 || j[4] < 0 )
  {
    LogWrn("Could not retrieve image orientation patient info correctly",  "Fields_ext", "LoadVolumeFields3");
  } // if
  else
  { /// get the orientation cosine
    std::string srx, sry, srz, scx, scy, scz;
    float4      frx, fry, frz, fcx, fcy, fcz;
    for( i=0; i<j[0]; i++ )
      srx.push_back(sImageOrientation[i]);

    ToType(srx, frx);

    for( i=j[0]+1; i<j[1]; i++ )
      sry.push_back(sImageOrientation[i]);

    ToType(sry, fry);

    for( i=j[1]+1; i<j[2]; i++ )
      srz.push_back(sImageOrientation[i]);

    ToType(srz, frz);

    for( i=j[2]+1; i<j[3]; i++ )
      scx.push_back(sImageOrientation[i]);

    ToType(scx, fcx);

    for( i=j[3]+1; i<j[4]; i++ )
      scy.push_back(sImageOrientation[i]);

    ToType(scy, fcy);

    for( i=j[4]+1; i<n; i++ )
      scz.push_back(sImageOrientation[i]);

    ToType(scz, fcz);

    if( (fcy == 1.0F && frx == 1.0F) || (fcy == 1.0F && frx == -1.0F) )
    {
			patientOrientation = SerializedDicom::SUPINE;
    }
    else if( (fcy == -1.0F && frx == -1.0F) || (fcy == -1.0F && frx == 1.0F) )
    {
			patientOrientation = SerializedDicom::PRONE;
    }
    else if( (fry == 1.0F && fcx == -1.0F) || (fry == -1.0F && fcx == -1.0F) )
    {
      patientOrientation = SerializedDicom::DECUBITUS_RIGHT;
    }
    else if( (fry == -1.0F && fcx == 1.0F) || (fry == 1.0F && fcx == 1.0F) )
    {
			patientOrientation = SerializedDicom::DECUBITUS_LEFT;
    } // else
  } // else

	return patientOrientation;

} // GetPatientOrientation()


/**
 * The joined skeleton for segmental view. This is a singleton skeleton from rectum to 
 * cecum with nodes placed right through any collapse regions by linear interpolation
 */
void Dataset::CreateJoinedSkeleton()
{
	if (m_segmentList.GetNumSelectedSegments() <= 0)
	{
		return;
	}

	// Calculate the average length of a skeleton node (to fill in Gaps)
	float4 fTotalLength(0);
	uint4 i;
	for (i=0; i<m_segmentList.GetNumSelectedSegments(); i++)
	{
		Skeleton & skeleton = m_segmentList.GetSegment(i).GetCurrentSkeleton();
		fTotalLength += (skeleton.GetPiecewiseEuclideanLength() / skeleton.GetNumOfElements());
	} // end for all segments
	float4 fAverageLength = fTotalLength / m_segmentList.GetNumSelectedSegments();

	// Calculate the total number of nodes
	uint2 uTotalNodes(0);
	for (i=0; i<m_segmentList.GetNumSelectedSegments(); i++)
	{
		Skeleton & skeleton = m_segmentList.GetSegment(i).GetCurrentSkeleton();
		uTotalNodes += skeleton.GetNumOfElements();
		if(i<m_segmentList.GetNumSelectedSegments()-1)
		{
			Skeleton & skeleton2 = m_segmentList.GetSegment(i+1).GetCurrentSkeleton();
			uint2 uEndCurrentIndex = m_segmentList.GetSegment(i  ).IsRegularDirection() ? skeleton.GetNumOfElements()-1 : 0;
			uint2 uStartNextIndex  = m_segmentList.GetSegment(i+1).IsRegularDirection() ? 0 : skeleton2.GetNumOfElements()-1;
			Vector<float4> gap = skeleton2.GetWorldPos(uStartNextIndex)-skeleton.GetWorldPos(uEndCurrentIndex);
			uTotalNodes += uint2(gap.GetLength()/fAverageLength)-1;
		}
	} // end for all segments

	// Fill in the joined skeleton and the segment pointer indexes
	m_joinedSkeleton.Resize(uTotalNodes,m_segmentList.GetNumUnsortedSegments()+1);
	uint2 uCurrentNode(0);
	for (i=0; i<m_segmentList.GetNumSelectedSegments(); i++)
	{
		Segment & segment = m_segmentList.GetSegment(i);
		Skeleton & skeleton = segment.GetCurrentSkeleton();

		bool bReverseDirection(!segment.IsRegularDirection());

		// Copy the Skel nodes for this segment
		uint4 uSegStartNode = uCurrentNode;
		for (uint4 j=0; j<skeleton.GetNumOfElements(); j++)
		{
			uint2 uSkelNodeIndex = bReverseDirection ? skeleton.GetNumOfElements()-1-j : j;
			SkeletonNodeVCExt skeVCExtNode;
			skeVCExtNode.m_iSegmentIndex = i; // the sorted segment index (starts at 0, Note: does NOT match ins file)
			skeVCExtNode.m_uSkelIndexWithinSegment = uSkelNodeIndex;
			m_joinedSkeleton.SetSkelNode(uCurrentNode++,skeVCExtNode,skeleton.GetSkelNode(uSkelNodeIndex));  
		} // end for each element of the segment 
		m_joinedSkeleton.CalculateAcceleration(uSegStartNode,uCurrentNode);

		// setup the segment pointer index
		m_joinedSkeleton.SetFirstNodeInSegment(segment.GetIdentifier(), (segment.IsRegularDirection() ? uSegStartNode : uCurrentNode));
		
		// fill in the GAP between this segment and the next
		if ((m_segmentList.GetNumSelectedSegments() > 0) && (i < m_segmentList.GetNumSelectedSegments() - 1))
		{
			Skeleton & skeleton2 = m_segmentList.GetSegment(i+1).GetCurrentSkeleton();
			uint2 uEndCurrentIndex = m_segmentList.GetSegment(i  ).IsRegularDirection() ? skeleton.GetNumOfElements()-1 : 0;
			uint2 uStartNextIndex  = m_segmentList.GetSegment(i+1).IsRegularDirection() ? 0 : skeleton2.GetNumOfElements()-1;
			Vector<float4> gap = skeleton2.GetWorldPos(uStartNextIndex)-skeleton.GetWorldPos(uEndCurrentIndex);
			uint4 uGapNodes = uint2(gap.GetLength()/fAverageLength);
			Normal<float4> gapDirection(gap);
			float4 fGapDelta = gap.GetLength()/uGapNodes;
			for (uint4 k=1; k<uGapNodes; k++)
			{
				SkeletonNode node;
				node.m_branchIdx = 0;
				node.m_vWorldPos = skeleton.GetWorldPos(uEndCurrentIndex) + Vector<float4>(gapDirection)*(fGapDelta*k);
				node.m_vWorldTan = gapDirection;
				node.m_vWorldUp = Normal<float4>(0,0,1);
				node.m_worldDfe = 0;
				node.m_worldDfs = 0;
				node.m_vVolumePos = m_fields.m_volumeRender.GetWorldToVolumeMatrix() * node.m_vWorldPos;
				SkeletonNodeVCExt skeVCExtNode;
				skeVCExtNode.m_iSegmentIndex = -1;
				m_joinedSkeleton.SetSkelNode(uCurrentNode++,skeVCExtNode,node);  
			} // endfor nodes in GAP
		} // endif GAP to fill in
		
	} // end for each segment
	
	// Re-compute information about this new joined skeleton
	m_joinedSkeleton.ComputeTangents(9.0F);
	m_joinedSkeleton.ComputeUpVectors();
	m_joinedSkeleton.RecalculateDistances();
	m_joinedSkeleton.LocateCenterOfGravity();
} // CreateJoinedSkeleton


/**
 * The flipped skeleton for Supine/Prone Registration NOTE: THIS IS ONLY FILLED IN FOR THE PRONE STUDY.
 * 
 * Update September 2006:
 *   We did away with the supine/prone/decubitus differences in the volumes. The volumes are now always
 *   in supine position. So the flipping is now disabled. We still fill in the joined flipped skeleton,
 *   but it is not actually flipped anymore.
 */
void Dataset::CreateFlippedSkeleton()
{
	m_joinedFlippedSkeleton.Resize(m_joinedSkeleton.GetNumOfElements(),m_segmentList.GetNumUnsortedSegments()+1);
	for (uint4 i=0; i<m_joinedSkeleton.GetNumOfElements(); i++)
	{
		m_joinedFlippedSkeleton.FlipSkelNode(i,m_joinedSkeleton.GetExtSkelNode(i),m_joinedSkeleton.GetSkelNode(i),m_vUnits,m_vDim);  
	} // end for each element of the segment 
	m_joinedFlippedSkeleton.LocateCenterOfGravity();
	//m_joinedFlippedSkeleton.PrintJoinedData("Joined Flipped (Should only be Prone)");
} // CreateFlippedSkeleton


/**
 * Locates the features in a colon. Features are merely points located at the peaks
 * in the acceleration curve, that is, each feature is placed exactly where the colon bends.
 * The value that goes into the vFeatures vector is the centerline joined index.
 * @paran eType dataset type
 */
void Dataset::LocateFeatures()
{
	uint4 uFeatureResolution = 20;
	m_ivNominatedFeatures.clear();

	for (int4 i = uFeatureResolution; i < int4(m_joinedSkeleton.GetNumOfElements()) - int4(uFeatureResolution); i++)
	{
		if(m_joinedSkeleton.GetAcceleration(i) > m_joinedSkeleton.GetAcceleration(i-uFeatureResolution) &&
			 m_joinedSkeleton.GetAcceleration(i) > m_joinedSkeleton.GetAcceleration(i+uFeatureResolution) &&
			 m_joinedSkeleton.GetAcceleration(i-uFeatureResolution) > 0.0F &&
			 m_joinedSkeleton.GetAcceleration(i+uFeatureResolution) > 0.0F)
		{
			m_ivNominatedFeatures.push_back(i);
		}
	} // end of locating features
	
	// In order to not have features too close to each other, we take the average of 
	// a bunch of features with the distance of m_MinDistanceBtwFeatures or less.
	AverageFeatures();
	//AverageFeaturesPickSmallerAngle();
	//PrintFeatures();
} // LocateFeatures


/**
 * Prints the features to a file
 */
void Dataset::PrintFeatures()
{
	std::ofstream outStream("C:/V-System/Log/Features.txt", std::ios::app);

	outStream << m_ivFeatures.size() << " features in " << (m_eOrientation == DatasetOrientations::ePRONE ? "Prone" : "Supine") << std::endl;  
	for (uint4 i=0; i<m_ivFeatures.size(); i++)
	{
		outStream << i << ": " << m_ivFeatures[i] << std::endl;
	}
	outStream << std::endl;  
}

/**
 * Makes an average of the features that have the distance m_iMinDistanceBtwFeatures
 * or less from each other.
 *
 * @param vFeatures the features generated
 * @param bType dataset type
 */
void Dataset::AverageFeatures()
{
	uint4 uMinDistBtwFeatures = 10;
	m_ivFeatures.clear();

	if (m_ivNominatedFeatures.size() <= 0)
	{
		return;
	}

	for (uint4 i=1; i<m_ivNominatedFeatures.size()-1; i++)
	{
		// if the distance between features is less or equal to distBtwFeatures
		// lets take the average
		if ( ((m_ivNominatedFeatures[i+1] - m_ivNominatedFeatures[i]) < uMinDistBtwFeatures) )
		{
			uint4 uFirstFeature = i;
			while ( ( i < m_ivNominatedFeatures.size() - 1 ) &&
							( ( m_ivNominatedFeatures[i+1] - m_ivNominatedFeatures[i] ) < uMinDistBtwFeatures ) )
				i++;
			m_ivFeatures.push_back(((m_ivNominatedFeatures[i-1] - m_ivNominatedFeatures[uFirstFeature]) / 2) + m_ivNominatedFeatures[uFirstFeature]);
		} // end if take the average
		else
		{
			m_ivFeatures.push_back(m_ivNominatedFeatures[i]);
		} // end else the two features are not too near to each other
		
	} // end while traverse the features
	
} // AverageFeatures


/**
 * The straight sections of the joined skeleton
 */
void Dataset::CreateSkeletonSegments()
{
	// Clear the list
	if(m_straightSegmentNodes.size()!=0) m_straightSegmentNodes.resize(0);
	m_currentSegment = 0;

	//  place the first and last nodes in the list
	m_straightSegmentNodes.push_back(0);
	m_straightSegmentNodes.push_back(m_joinedSkeleton.GetNumOfElements()-1);

	// Place the nodes in the list
	RecursiveDouglasPeucker(0,m_joinedSkeleton.GetNumOfElements()-1);

	// Sort the output list
	uint4 uSegmentCount = m_straightSegmentNodes.size();
	if (uSegmentCount > 0)
	{
		for (uint4 i=0;i<uSegmentCount-1;i++)
		{
			for (uint4 j=i+1;j<uSegmentCount;j++)
			{
				if (m_straightSegmentNodes[i] > m_straightSegmentNodes[j])
				{
					uint4 uTmpIndex = m_straightSegmentNodes[i];
					m_straightSegmentNodes[i] = m_straightSegmentNodes[j];
					m_straightSegmentNodes[j] = uTmpIndex;
				}
			}
		}
	}
} // CreateSkeletonSegments


/**
*  @param point
*  @param lineEndPoint1
*  @param lineEndPoint2
*  @return
*/
float4 DistancePoint2Line(const Point<float4> point, const Point<float4> lineEndPoint1, const Point<float4> lineEndPoint2)
{
	Vector<float4> line(lineEndPoint2-lineEndPoint1);
	Vector<float4> diag(lineEndPoint1-point);
	Vector<float4> tmp1(line.Cross(diag));
	float4 num = tmp1.GetLength();
	float4 den = line.GetLength();
	if(den != 0.0) return num/den;
	return 0;
} // DistancePoint2Line


/**
*  @param viewPointWorld
*  @param centerWorld
*  @param BlockVolUC & insVol
*  @return
*/
bool Visible(Point<float4> viewPointWorld, Point<float4> centerWorld, const BlockVolUC & insVol);


/**
 * The straight sections of the joined skeleton
 *
 *  @param uStartIndex
 *  @param uStopIndex
 */
void Dataset::RecursiveDouglasPeucker(const uint4 uStartIndex,const uint4 uStopIndex)
{
	if(uStopIndex <= uStartIndex+1) return;

	float4 fMaxDistance = 0.0;
	uint4 uMaxIndex = uStartIndex;
	Point<float4> startPoint = m_joinedSkeleton.GetWorldPos(uStartIndex);
	Point<float4> stopPoint  = m_joinedSkeleton.GetWorldPos(uStopIndex );

	for(uint4 i=uStartIndex+1; i<uStopIndex; i++)
	{
		Point<float4> currentPoint  = m_joinedSkeleton.GetWorldPos(i);
		float4 fCurrentDistance = DistancePoint2Line(currentPoint,startPoint,stopPoint);
		if(fCurrentDistance > fMaxDistance)
		{
			fMaxDistance = fCurrentDistance;
			uMaxIndex = i;
		} // endif new max distance
	} // endfor all vertices in this current range

	if(fMaxDistance>m_fSegmentErrorThreshold)// || !Visible(m_joinedSkeleton.GetWorldPos(uStartIndex),m_joinedSkeleton.GetWorldPos(uStopIndex ),m_fields.m_ins))
	{
		m_straightSegmentNodes.push_back(uMaxIndex);
		RecursiveDouglasPeucker(uStartIndex, uMaxIndex);
		RecursiveDouglasPeucker(uMaxIndex, uStopIndex);
	} //endif found a vertex beyonf error threshold
	
} //RecursiveDouglasPeucker


/**
 * Updates the biopsy colormap in the vxSlcieRendererData class.
 */
void Dataset::UpdateBiopsyColormap()
{
	std::vector< std::pair< float4, float4 > > vRed, vGreen, vBlue, vAlpha;

	float4 fScale(2000.0F/4096.0F);

	const int iBias( vxIntensityConverter::GetBias    (m_fields.m_sliceRendererData.GetUncleansedVolume()->GetHeader(),  true) );
	const int iMax ( vxIntensityConverter::GetMaxValue(m_fields.m_sliceRendererData.GetUncleansedVolume()->GetHeader(), false) );
		
	float ff( iBias-1000);
	if( ff != 0.0F )
	{
		ff = ff/float(iMax);
		fScale = 2000.0F/float(iMax); 
	}

	for( int i( 0 ); i < m_biopsyTransferFunction.m_vfBiopsyThresholds.size()-1; ++i )
	{
		TransferFunctionVC::LinearTFParams params = m_biopsyTransferFunction.m_vfBiopsyThresholds[ i ];
		TransferFunctionVC::LinearTFParams params2 = m_biopsyTransferFunction.m_vfBiopsyThresholds[ i+1 ];
		vRed.push_back  ( std::make_pair( fScale * params.m_fThreshold / 255.0F + ff, params.m_prevColor.R()    / 255.0F ) );
		vRed.push_back  ( std::make_pair( fScale * params2.m_fThreshold / 255.0F + ff, params.m_nextColor.R()   / 255.0F ) );
		vGreen.push_back( std::make_pair( fScale * params.m_fThreshold / 255.0F + ff, params.m_prevColor.G()  / 255.0F ) );
		vGreen.push_back( std::make_pair( fScale * params2.m_fThreshold / 255.0F + ff, params.m_nextColor.G() / 255.0F ) );
		vBlue.push_back ( std::make_pair( fScale * params.m_fThreshold / 255.0F + ff, params.m_prevColor.B()   / 255.0F ) );
		vBlue.push_back ( std::make_pair( fScale * params2.m_fThreshold / 255.0F + ff, params.m_nextColor.B()  / 255.0F ) );
	}
	vRed.push_back  ( std::make_pair( 1.0, vRed.back().second) );
	vGreen.push_back( std::make_pair( 1.0, vGreen.back().second) );
	vBlue.push_back ( std::make_pair( 1.0, vBlue.back().second) );

	vAlpha.push_back( std::make_pair( 0.0F, 1.0F) );
	vAlpha.push_back( std::make_pair( 1.0F, 1.0F) );

	m_fields.m_sliceRendererData.SetBiopsyColormap( vRed, vGreen, vBlue, vAlpha );

} // UpdateBiopsyColormap()


/**
 * Load all data members from disk into memory.
 *  @param uStage
 *  @param loadCallback
 *  @param bLoadBoth
 *  @param uOtherLoadPercent
 *  @param bSetZoomFactor
 */
void Dataset::LoadStep2(const uint4 uStage, StudyLoad::LoadCallback & loadCallback, const bool bLoadBoth, const uint4 uOtherLoadPercent, bool & bSetZoomFactor)
{ 
	try
	{
		uint4 uLoadScale = bLoadBoth ? 2 : 1;
		std::string sOrientation = GetOrientation() == DatasetOrientations::eSUPINE? "Supine" : "Prone";

		switch(uStage)
		{
		case eLoadHeader:
			{
				LogDbg( "Start Load Step( " + sOrientation + " ): eLoadHeader", "Dataset", "LoadStep" );
				
				m_bVolumeLoaded = false;
				m_bNeedVisualizationReinit = true;

				std::string sFilePath = (m_sVolumePrefix + vx::ElecCleansedVolExt);
				if (!io::FilePtrFactory::Exists(sFilePath ))
					sFilePath = m_sPreprocessedPrefix + vx::ElecCleansedVolExt;

				loadCallback.NextStep(StudyLoad::eHEADER, uOtherLoadPercent+m_uLoadPercent, GetOrientation());
				// Load the volume header information
				m_volumeHeader.Read(sFilePath);

				// set the correct dataset orientation
				std::string sOrientation;
				m_volumeHeader.GetHeaderEntry( vxVolumeHeader::PatientPosition, sOrientation );
				if ( sOrientation[2] == 'P' )
					SetOrientation( DatasetOrientations::ePRONE );
				else
					SetOrientation( DatasetOrientations::eSUPINE );
				// this second half of the logic may be redundant (ie we may only call LoadStep2 for these versions anyways)
				m_bRotateProneToSupine = GetOrientation() == DatasetOrientations::ePRONE &&
					atof( m_seriesVersion.GetVersion().c_str() ) < 6.0F;
				if ( m_bRotateProneToSupine )
					LogDbg( "We are going to flip this prone dataset to supine", "", "" )

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
				m_vDim = m_volumeHeader.GetVolDim();
				m_vOrthogonalPtWorld = Point<float4>(m_vDim.m_x * m_fields.m_units2D.m_x,
																					 m_vDim.m_y * m_fields.m_units2D.m_y,
																					 m_vDim.m_z * m_fields.m_units2D.m_z) * 0.5f;

				// setup the value once: this function should be called right after the m_vDim&m_vUnits are set for the first time
				// if the loading order would be changed in the future (as said by Kevin in Revision 3.13.2.3), 
				// the place to call this function should follow the the change.
				if (bSetZoomFactor)
				{
					rdrGlobal.SetupZoomFactorInOverview(m_vDim, m_vUnits);
					bSetZoomFactor = false;
				}
				m_uLoadPercent += 4/uLoadScale;
				LogDbg( "End Load Step( " + sOrientation + " ): eLoadHeader", "Dataset", "LoadStep" );
			}
			break;
		case ePreAllocLinearVols:
			{
				LogDbg( "Start Load Step( " + sOrientation + " ): ePreAllocLinearVols", "Dataset", "LoadStep" );
				// preallocate the 3 linear volumes to save memory (due to virtual address space
				// fragmentation since windows is limited to 2GB of virtual addressing) 
				m_fields.m_sldInterior.Resize(m_vDim);
				if( rdrGlobal.m_bUseULDvolume )
				{
					m_fields.m_sldExterior.Resize(m_vDim); //:: Use this for un-cleansed SLD, i.e. the ULD volume
				}
				m_fields.m_volumeRender.Resize(m_vDim);
				m_fields.m_paintVol.Resize(m_vDim);
				m_fields.m_paintVol.SetUnits(m_vUnits);  
				m_fields.m_volumeRender.PreLaunchRenderThreads(this);
				LogDbg( "End Load Step( " + sOrientation + " ): ePreAllocLinearVols", "Dataset", "LoadStep" );
			}
			break;
		case eLoadOverview:
			{
				LogDbg( "Start Load Step( " + sOrientation + " ): eLoadOverview", "Dataset", "LoadStep" );
				// Load the segments
				loadCallback.NextStep(StudyLoad::eSEGMENTS, uOtherLoadPercent+m_uLoadPercent, GetOrientation());
				if (rdrGlobal.m_bAbortLoad == true) throw ex::AbortLoadException(LogRec("Loading abort by customer at stage 'reading segment list","Dataset", "LoadStep"));
				LogDbg( "End Load Step( " + sOrientation + " ): eLoadOverview", "Dataset", "LoadStep" );
				m_segmentList.Read(m_sPreprocessedPrefix);
				// flip around old prone segment list
				if ( m_bRotateProneToSupine )
				{
					LogDbg( "Flipping an old segment list around into supine orientation", "Dataset", "LoadStep2" );
					m_segmentList.RotateProneToSupine( m_volumeHeader.GetWorldDim() );
				}
				m_uLoadPercent += 10/uLoadScale;

				// Read CAD Findings
				LogDbg( "Start Load Step( " + sOrientation + " ): CAD", "Dataset", "LoadStep" );
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
  						LogErr( "Required volume file missing: " + sOriginalColumeFilePath, "Dataset", "LoadStep" );
						throw ex::AbortLoadException( LogRec( "Required volume file missing: " + sOriginalColumeFilePath, "Dataset", "LoadStep" ) );
					}
					vxVolumeHeader originalHeader;
					originalHeader.Read( sOriginalColumeFilePath );

					m_cadFindings.ResolveUnits( originalHeader.GetVoxelUnits() );
				}
				LogDbg( "End Load Step( " + sOrientation + " ): CAD", "Dataset", "LoadStep2" );

				LogDbg( "Start Load Step( " + sOrientation + " ): session", "Dataset", "LoadStep2" );
				// Load session
				// TODO: Remove this initialization, it's not really necessary, but I'll leave it for now just in case
				for ( int4 i(0); i < m_cadFindings.GetCount(); i++ )
				{
					m_session.m_mapCadFindingInfo[ m_cadFindings.GetFinding( i ).GetId() ].SetDecision(ColonSession::FindingInfo::eDecisionUndecided);
				}
				m_session.Read(m_sSessionPrefix, *this);
				if ( m_bRotateProneToSupine )
				{
					LogDbg( "Flipping an old prone session around into supine orientation", "Dataset", "LoadStep2" );
					m_session.RotateProneToSupine(m_volumeHeader.GetWorldDim());
				}
				LogDbg( "End Load Step( " + sOrientation + " ): session", "Dataset", "LoadStep2" );
			}
			break;

		case eLoadVolumeFields:
				LogDbg( "Start Load Step( " + sOrientation + " ): eLoadVolumeFields", "Dataset", "LoadStep" );
				loadCallback.NextStep(StudyLoad::eVOLFIELDS, uOtherLoadPercent+m_uLoadPercent, GetOrientation());
				m_fields.LoadVolumeFields2( m_sVolumePrefix, m_sPreprocessedPrefix, m_bRotateProneToSupine );
				LogDbg( "End Load Step( " + sOrientation + " ): eLoadVolumeFields", "Dataset", "LoadStep" );        
				break;

		case eLoadThresholds:
			{
				// Read the threshold information file
				LogDbg( "Start Load Step( " + sOrientation + " ): eLoadThresholds", "Dataset", "LoadStep" );
				loadCallback.NextStep(StudyLoad::eTHRESHOLD, uOtherLoadPercent+m_uLoadPercent, GetOrientation());
				LogDbg( "End Load Step( " + sOrientation + " ): eLoadThresholds", "Dataset", "LoadStep" );
				if (rdrGlobal.m_bAbortLoad == true) throw ex::AbortLoadException(LogRec("Loading abort by customer at stage 'read threshold'","Dataset", "LoadStep"));
				Thresholds thresh;
				thresh.Read(m_sPreprocessedPrefix + vx::ThresholdExt);
				m_fColonWallThreshold = thresh.m_vThresholds[2];
				// your momma int THIS_SHOULD_PROBABLY_CALL_HU_CONVERTER;
				// the code below should be better, but the compiler complains for some strange reason
				//int1 eightBitThreshold(0);  
				//HU::Convert((int2) thresh.m_vThresholds[2], eightBitThreshold);
				//m_fColonWallThreshold = eightBitThreshold;

				m_fColonWallThreshold = m_fColonWallThreshold*255/2000; // intentional integer
				m_surfaceTransferFunction.SetColonWall(m_fColonWallThreshold);
				m_uLoadPercent += 2/uLoadScale;
				
				// Read the ins file from disk
				LogDbg( "Start Load Step( " + sOrientation + " ): eINS", "Dataset", "LoadStep" );
				loadCallback.NextStep(StudyLoad::eINS, uOtherLoadPercent+m_uLoadPercent, GetOrientation());
				LogDbg( "End Load Step( " + sOrientation + " ): eINS", "Dataset", "LoadStep" );
				if (rdrGlobal.m_bAbortLoad == true) throw ex::AbortLoadException(LogRec("Loading abort by customer at stage 'load .ins","Dataset","LoadStep"));
				m_fields.m_ins.Read(m_sPreprocessedPrefix + vx::InsideLabelVolExt);
				if ( m_bRotateProneToSupine )
					vxBlockVolumeUtilities::ReformatVolumeToSupine( m_fields.m_ins, "FFP" );
				m_uLoadPercent += 10/uLoadScale;

				// smooth the centerlines
				SmoothCenterlines();
				m_bDistanceValid = true;
			}
			break;

		case eLoadNavigationFields:
			{
				// Read the dfs file from disk
				LogDbg( "Start Load Step( " + sOrientation + " ): eDFS", "Dataset", "LoadStep" );
				loadCallback.NextStep(StudyLoad::eDFS, uOtherLoadPercent+m_uLoadPercent, GetOrientation());
				LogDbg( "End Load Step( " + sOrientation + " ): eDFS", "Dataset", "LoadStep" );        
				if (rdrGlobal.m_bAbortLoad == true) throw ex::AbortLoadException(LogRec("Loading abort by customer at stage 'load .dfs","Dataset","LoadStep"));
				m_fields.m_dfs.Read(m_sPreprocessedPrefix + "_0" + vx::PotentialDistRootExt);
				if ( m_bRotateProneToSupine )
					vxBlockVolumeUtilities::ReformatVolumeToSupine( m_fields.m_dfs, "FFP" );
				m_uLoadPercent += 10/uLoadScale;

				{vxVolumeHeader &hdr = m_fields.m_dfs.GetHeader();
				LogDbg("DFS Dim: " + ToAscii(hdr.GetVolDim()) + "; Units: " + ToAscii(hdr.GetVoxelUnits()), "Fields", "LoadStep2");}

				// Read the dfe file from disk
				LogDbg( "Start Load Step( " + sOrientation + " ): eDFE", "Dataset", "LoadStep" );
				loadCallback.NextStep(StudyLoad::eDFE, uOtherLoadPercent+m_uLoadPercent, GetOrientation());
				LogDbg( "End Load Step( " + sOrientation + " ): eDFE", "Dataset", "LoadStep" );
				if (rdrGlobal.m_bAbortLoad == true) throw ex::AbortLoadException(LogRec( "Loading abort by customer at stage 'load .dfe" ,"Dataset","LoadStep"));
				m_fields.m_dfe.Read(m_sPreprocessedPrefix + "_1" + vx::PotentialDistRootExt);
				if ( m_bRotateProneToSupine )
					vxBlockVolumeUtilities::ReformatVolumeToSupine( m_fields.m_dfe, "FFP" );
				m_uLoadPercent += 10/uLoadScale;

				{vxVolumeHeader &hdr = m_fields.m_dfe.GetHeader();
				LogDbg("DFE Dim: " + ToAscii(hdr.GetVolDim()) + "; Units: " + ToAscii(hdr.GetVoxelUnits()), "Fields", "LoadStep2");}

				// read the csi file from disk
				LogDbg( "Start Load Step( " + sOrientation + " ): eCSI", "Dataset", "LoadStep" );
				loadCallback.NextStep(StudyLoad::eCSI, uOtherLoadPercent+m_uLoadPercent, GetOrientation());
				LogDbg( "End Load Step( " + sOrientation + " ): eCSI", "Dataset", "LoadStep" );
				if (rdrGlobal.m_bAbortLoad == true) throw ex::AbortLoadException(LogRec("Loading abort by customer at stage 'load .csi","Dataset","LoadStep"));
				m_fields.m_csi.Read(m_sPreprocessedPrefix + "_0" + vx::ClosestSkelIndexExt);
				if ( m_bRotateProneToSupine )
					vxBlockVolumeUtilities::ReformatVolumeToSupine( m_fields.m_csi, "FFP" );
				m_uLoadPercent += 10/uLoadScale;

				{vxVolumeHeader &hdr = m_fields.m_csi.GetHeader();
				LogDbg("CSI Dim: " + ToAscii(hdr.GetVolDim()) + "; Units: " + ToAscii(hdr.GetVoxelUnits()), "Fields", "LoadStep2");}
			}
			break;
		case eLoadRenderFields:
			{
				// Read the space leap files from disk
				LogDbg( "Start Load Step( " + sOrientation + " ): eSLD", "Dataset", "LoadStep" );
				loadCallback.NextStep(StudyLoad::eSLD, uOtherLoadPercent+m_uLoadPercent, GetOrientation());
				LogDbg( "End Load Step( " + sOrientation + " ): eSLD", "Dataset", "LoadStep" );
				if (rdrGlobal.m_bAbortLoad == true) throw ex::AbortLoadException(LogRec("Loading abort by customer at stage 'load .sld","Dataset","LoadStep"));
				m_fields.LoadSpaceLeapFields(m_sPreprocessedPrefix);
				if ( m_bRotateProneToSupine )
				{
					VolumeUtilities::ReformatVolumeProneToSupine( m_fields.m_sldInterior );
					if( rdrGlobal.m_bUseULDvolume )
					{
						VolumeUtilities::ReformatVolumeProneToSupine( m_fields.m_sldExterior ); //:: Use this for un-cleansed SLD, i.e. the ULD volume
					}
				} // if

				m_uLoadPercent += 22/uLoadScale;

				// ?????????????? WHAT DOES THE FOLLOWING CODE DO ?????????????????????
				// Read the volume files from disk
				LogDbg( "Start Load Step( " + sOrientation + " ): eVOLUME", "Dataset", "LoadStep" );
				loadCallback.NextStep(StudyLoad::eVOLUME, uOtherLoadPercent+m_uLoadPercent, GetOrientation());
				LogDbg( "End Load Step( " + sOrientation + " ): eVOLUME", "Dataset", "LoadStep" );
				if (rdrGlobal.m_bAbortLoad == true) throw ex::AbortLoadException(LogRec("Loading abort by customer at stage 'load .ecv","Dataset","LoadStep"));
				m_uLoadPercent += 22/uLoadScale;

				// Set the orthogonal point to the center of the volume
				m_vOrthogonalPtWorld = Point<float4>(m_vDim.m_x * m_fields.m_units2D.m_x,
																					 m_vDim.m_y * m_fields.m_units2D.m_y,
																					 m_vDim.m_z * m_fields.m_units2D.m_z) * 0.5f;

				try
				{
					LogDbg( "Start Load Step( " + sOrientation + " ): paint volume", "Dataset", "LoadStep" );
					m_fields.m_paintVol.ReadData(m_sSessionPrefix + vx::MarkedVolumeExt);
					if ( m_bRotateProneToSupine )
						VolumeUtilities::ReformatVolumeProneToSupine( m_fields.m_paintVol );
					LogDbg( "End Load Step( " + sOrientation + " ): paint volume", "Dataset", "LoadStep" );
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
			}
			break;
		default:
			break;
		}

	} // endtry

	catch (ex::AbortLoadException &) { throw;}
	catch (ex::VException &) { throw;}
	catch (...) { throw ex::VException(LogRec("An unspecified error in stage" + uStage ,"Dataset", "LoadStep"));}

	return;
} // LoadStep2


/**
 * gets the component array
 *
 * @return a component array that matches the selected segments
 */
vxComponentArray & Dataset::GetComponentArray()
{

	// lazily create the component array
	if ( m_componentArray.ArrayByLabel().size() == 1 )
	{

		m_componentArray[ 0 ]->SetIntensityVisible( false );

		// go through the segments and turn them on and off as required
		const int4 iMaxSegments = 256;
		for ( int i=1; i<iMaxSegments; i++ )
		{
			m_componentArray.AddComponent( ToAscii( i ), PixelRGBA<uint1>( 255, 255, 255, 255 ) );
			assert( m_componentArray[ i ]->GetLabel() == i );
			const bool bSelected = ( m_segmentList.GetSortedSegmentIndexFromIdentifier( i ) != -1 );
			m_componentArray[ i ]->SetIntensityVisible( bSelected );
		}
	}

	return m_componentArray;

} // GetComponentArray()


/**
 * Determines if a given point is within the colon lumen
 * 
 * @param pt   point to test
 * @return     whether pt is inside or proximal to the selected segments of the lumen
 */
bool Dataset::IsPointNearSelectedLumen( const Point3Df & pt )
{

	// maximum distance from a selected segment lumen to allow to be 'near' lumen
	const float4 fWithinThresholdMM( 20.0F );

	// select the maximum radius to search in x, y, and z
	const Triple<float4> & units = m_fields.m_ins.GetHeader().GetVoxelUnits();
	int4 iRadiusX = int4( ceil( fWithinThresholdMM / units.m_x ) );
	int4 iRadiusY = int4( ceil( fWithinThresholdMM / units.m_y ) );
	int4 iRadiusZ = int4( ceil( fWithinThresholdMM / units.m_z ) );

	// bound to the visible volume
	int4 iStartX = max( 0, pt.m_x - iRadiusX );
	int4 iStartY = max( 0, pt.m_y - iRadiusY );
	int4 iStartZ = max( 0, pt.m_z - iRadiusZ );
	int4 iEndX = min( m_fields.m_ins.GetHeader().GetVolDim().m_x - 1, pt.m_x + iRadiusX );
	int4 iEndY = min( m_fields.m_ins.GetHeader().GetVolDim().m_y - 1, pt.m_y + iRadiusY );
	int4 iEndZ = min( m_fields.m_ins.GetHeader().GetVolDim().m_z - 1, pt.m_z + iRadiusZ );

	// predetermine which INS values are part of the selected (sorted) segments
	std::vector<bool> m_vbSegmentSelected;
	for ( int i=0; i < m_segmentList.GetNumUnsortedSegments() + 1; i++ )
		m_vbSegmentSelected.push_back( m_segmentList.GetSortedSegmentIndexFromIdentifier( i ) > -1 );

	// search nearby for an 'inside' voxel
	vxBlockVolumeIterator<uint1> insIter( m_fields.m_ins );
	Vector3Di position( iStartX, iStartY, iStartZ );
	const float4 fSquaredThresholdDistanceMM( pow( fWithinThresholdMM, 2 ) );
	for( position.m_z = iStartZ; position.m_z <= iEndZ; position.m_z++ )
	{
		for( position.m_y = iStartY; position.m_y <= iEndY; position.m_y++ )
		{
			position.m_x = iStartX;
			insIter.SetPos( position );
			for( position.m_x = iStartX; position.m_x <= iEndX; position.m_x++, insIter.IncX() )
			{
				if ( m_vbSegmentSelected[ insIter.GetVoxel() ] )
				{
					const float4 fSquaredDistanceMM =
						pow( ( position.m_x - pt.m_x ) * units.m_x, 2 ) +
						pow( ( position.m_y - pt.m_y ) * units.m_y, 2 ) +
						pow( ( position.m_z - pt.m_z ) * units.m_z, 2 );
					if ( fSquaredDistanceMM < fSquaredThresholdDistanceMM )
					{
						return true;
					}
				}
			}
		}
	}

	return false;

} // IsPointNearSelectedLumen()


/**
 * Is the joined skeleton computed?
 *
 * @return  True if the joined skeleton is computed?
 */
bool Dataset::IsJoined() const
{
	return m_joinedSkeleton.GetNumOfElements() > 0;
} // IsJoined()


/**
 * Comparison function
 *
 * @param x   First cad finding to compare
 * @param y   Second cad finding to compare
 * @return    true if first finding is "less-than" the second, false otherwise
 */
bool CALLBACK Dataset::SortByVisibility( vxCadFinding & x, vxCadFinding & y )
{
	Dataset& rDataset = (Dataset::GetCurrentSortDataset() != NULL) ? *Dataset::GetCurrentSortDataset() : *rdrGlobal.m_pCurrDataset;
	return rDataset.m_session.m_mapCadFindingInfo[x.GetId()].GetDecision() <
				 rDataset.m_session.m_mapCadFindingInfo[y.GetId()].GetDecision();
} // SortByVisibility()


/**
 * finding comparison function that sorts by skeleton index
 *
 * @param x the first finding
 * @param y the second finding
 *
 * @return if x skeleton index is less than y skeleton index
 */
bool CALLBACK Dataset::SortBySkeletonIndex( vxCadFinding & x, vxCadFinding & y )
{
	Dataset& rDataset = (Dataset::m_pCurrentSortDataset != NULL) ? *Dataset::m_pCurrentSortDataset : *rdrGlobal.m_pCurrDataset;
	return rDataset.GetNearestSkeletonDFS( x ) < rDataset.GetNearestSkeletonDFS( y );
} // SortBySkeletonIndex()


//undefines
#undef FILE_REVISION


// $Log: Dataset.C,v $
// Revision 1.37.2.30  2010/12/20 18:53:26  dongqing
// add VCSetting switch for using ULD volume
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.37.2.29  2010/04/05 20:09:34  dongqing
// Fix the bug that the Display Extracolonic CAD Finding toggle shown
// incorrect association between finding and selected colon segments.
// Note, the CAD Pane should always display the total number from the CAD
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.37.2.28  2010/01/25 22:24:19  dongqing
// add JoinedSkeletonIndex for correctly display cad mark in UDS view
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.37.2.27  2009/11/16 23:09:08  dongqing
// Issue Cad09-02: giving overview cad marks colours to indicate visibility setting.
//
// Revision 1.37.2.26  2009/11/10 19:03:48  dongqing
// Improving fly speed for non-cleansed images by using ULD volume
//
// Revision 1.37.2.25  2009/09/11 15:38:20  dongqing
// Per iCAD request, iCAD plugin no longer provide segment index.
// They shall only provide the viewing normal for each finding.
//
// Revision 1.37.2.24  2009/03/31 18:52:07  dongqing
// the iCAD version check must be > 1.1 rather than >= 1.2
//
// Revision 1.37.2.23  2009/03/30 19:15:47  dongqing
// change ICAD to iCAD
//
// Revision 1.37.2.22  2009/03/27 16:51:56  dongqing
// add codes to handle iCAD finding normal information correctly
//
// Revision 1.37.2.21  2009/03/17 17:20:54  dongqing
// enhance CadFindingInitialization to check normal direction for better
// camera position looking into a finding on fold.
//
// Revision 1.37.2.20  2009/01/23 19:21:53  jmeade
// Issue 6224: proper deletion of single/all volume measurement
//
// Revision 1.37.2.19  2009/01/15 04:15:11  jmeade
// GetSortedSegmentIndex convenience method
//
// Revision 1.37.2.18  2008/11/12 00:09:31  jmeade
// Issue 6128: update old prone session data for new volume orientation.
//
// Revision 1.37.2.17  2008/10/11 00:15:17  jmeade
// remove unneeded member variable.
//
// Revision 1.37.2.16  2008/10/06 19:57:35  dongqing
// add bool flag to indicate if user interacts with 2D x-sectional or 3D endo view
// This will tell the SliceViewer selecting proper oblique 2D view renderer setting.
//
// Revision 1.37.2.15  2008/07/09 14:10:11  dongqing
// CAD session reload has to set the label  index for the ROI
// Otherwise, the component control for 2D view does not work
//
// Revision 1.37.2.14  2008/07/02 19:53:33  dongqing
// add VCSetting.xml file EnableSRT status check for SRT ROI
// display activation
//
// Revision 1.37.2.13  2008/06/24 20:04:18  dongqing
// change to use the history index to track components for initialization
// CAD and SRT rois
//
// Revision 1.37.2.12  2008/06/24 18:12:32  dongqing
// Fix the bug that SRAT ROI shows same color as
// that of CAD ROI when load back session
//
// Revision 1.37.2.11  2008/05/28 22:10:52  dongqing
// Fix the SRT delete session reload crash by initializing componentArray
// in fresh way
//
// Revision 1.37.2.10  2008/03/04 15:02:56  dongqing
// change the color for CAD ROI in 2D to match the 3D color code
//
// Revision 1.37.2.9  2008/03/03 21:57:34  dongqing
// fix the bug of incorrect label index for session SROI
//
// Revision 1.37.2.8  2008/01/21 13:46:22  dongqing
// change the algorithm for creating label volume
//
// Revision 1.37.2.7  2007/06/25 20:33:44  jmeade
// Issue 5695: attempt to determine patient position from patientPosition header field if dicom field not present.
//
// Revision 1.37.2.6  2007/06/22 00:08:18  jmeade
// spacing, typos.
//
// Revision 1.37.2.5  2007/06/20 21:40:03  dongqing
// continuse the fix of SRT session by reseting the component index
// in the SRT Findings of the Dataset
//
// Revision 1.37.2.4  2007/06/20 20:39:16  dongqing
// fix the session crash caused by SRT session
//
// Revision 1.37.2.3  2007/06/14 18:10:22  jmeade
// comments.
//
// Revision 1.37.2.2  2007/06/05 23:36:16  jmeade
// don't output session file or marking volume for studies older than session version 3.0.
//
// Revision 1.37.2.1  2007/03/22 20:41:51  dongqing
// change the m_uXsectionScrollBarLocation to m_iXsectionScrollBarLocation
// The type is changed from uint4 to int4
//
// Revision 1.37  2007/03/13 02:12:46  jmeade
// code standards.
//
// Revision 1.36  2007/03/05 23:17:53  dongqing
// add new m_iXsectionScrollBarLocationIndex
//
// Revision 1.35  2007/02/27 21:50:17  jmeade
// Issue 5424: ensure cad threshold default is read when necessary.
//
// Revision 1.34  2007/02/16 22:00:17  jmeade
// Issue 5400: CAD: Add checks to ensure vendor-specific code not used by other vendors.
//
// Revision 1.33  2007/02/15 00:08:23  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.32  2007/01/30 22:16:06  dongqing
// fix the incorrect computation of distance to rectum for CAD findings
//
// Revision 1.31  2007/01/26 22:32:49  dongqing
// add CAD ROI display
//
// Revision 1.30  2007/01/23 20:18:22  dongqing
// add functionto handle the SRT session save and load
//
// Revision 1.29  2007/01/09 19:15:10  jmeade
// method to determine if a point in/near lumen.
//
// Revision 1.28  2006/12/05 19:55:44  jmeade
// Issue 5047: better organization of translucency settings structure.
//
// Revision 1.27  2006/11/22 19:18:03  dongqing
// fix the pseudocolormap bug for studies having bias != 1000
//
// Revision 1.26  2006/11/10 00:19:22  jmeade
// Issue 5097: Ensure cad list sorting will use intended dataset.
//
// Revision 1.25  2006/09/27 15:43:29  dongqing
// allow the InitializeVisualization refresh the labelVolume in the sliceRenderData
//
// Revision 1.24  2006/09/26 20:04:55  frank
// Fixed up 3D segment view
//
// Revision 1.23  2006/09/14 17:46:01  dongqing
// add m_suspiciousRegions
//
// Revision 1.22  2006/09/13 15:15:48  frank
// Issue #4965: Unflipped prone images to make viewer orientation agnostic
//
// Revision 1.21  2006/08/25 21:18:59  jmeade
// Issue 4946: serialized dicom patient orientation info.
//
// Revision 1.20  2006/08/24 01:32:44  jmeade
// cvs string.
//
// Revision 1.19  2006/07/05 15:30:30  frank
// Issue #4823: Updated the translucent view segmentation when neccessary
//
// Revision 1.18  2006/06/27 17:11:14  jmeade
// Read patient position from volume header.
//
// Revision 1.17  2006/06/26 13:03:08  frank
// creates a component array that matches the selected segments
//
// Revision 1.16  2006/06/01 20:26:13  frank
// updated to visual studio 2005
// bad buffer overrun detected!
//
// Revision 1.15  2006/05/12 20:29:20  jmeade
// Issue 4795: use proper prefix for cad xml file.
//
// Revision 1.14  2006/04/25 17:16:36  jmeade
// new study directory structure.
//
// Revision 1.13  2006/04/24 15:17:42  jmeade
// no hard-coded extensions/text.
//
// Revision 1.12  2006/04/12 23:16:51  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.11  2006/02/07 21:40:55  dongqing
// code walk through
//
// Revision 1.10  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.9  2005/11/19 12:04:45  vxconfig
// coding standards
//
// Revision 1.8  2005/11/16 04:51:39  jmeade
// Merge from Merge_VC_2-0_051115.
//
// Revision 1.7  2005/11/15 16:14:58  geconomos
// modified loading process to use new multi-directory structure
//
// Revision 1.6.2.9  2007/02/12 20:48:13  jmeade
// Merge from VC_2-0-2-0-RC50_Branch on 070212
//
// Revision 1.6.2.8  2007/01/30 22:42:28  dongqing
// fix the incorrect computation of the distance to rectum for CAD findings
//
// Revision 1.6.2.7.2.5  2007/02/01 20:03:50  jmeade
// Issue 4933: use correct dataset for finding sort.
//
// Revision 1.6.2.7.2.4  2007/01/31 22:34:07  jmeade
// Issue 5327: skeleton-list is 1-based, segment list is 0-based.
//
// Revision 1.6.2.7.2.3  2007/01/31 22:14:33  jmeade
// Issue 5327: skeleton-list is 1-based, segment list is 0-based.
//
// Revision 1.6.2.7.2.2  2007/01/31 21:51:00  jmeade
// Issue 5327: used 1-based cad segment index to determine in-lumen status of findings within (0-based) segments.
//
// Revision 1.6.2.7.2.1  2007/01/30 22:02:50  dongqing
// fix the CAD finding distance to rectum incorrect bug
//
// Revision 1.6.2.7  2006/03/29 20:52:23  frank
// read the segment number from the cad findings file
//
// Revision 1.6.2.6  2006/03/29 14:17:16  frank
// name change for finding info property
//
// Revision 1.6.2.5  2006/03/28 20:43:57  jmeade
// Issue 4642: clearer delineation of cad findings info.
//
// Revision 1.6.2.4  2006/03/24 23:37:23  jmeade
// Issue 4642: Allow hiding of extra-colonic cad findings.
//
// Revision 1.6.2.3  2006/03/22 20:53:10  frank
// moved cad finding sorting functions out of cad finding
//
// Revision 1.6.2.2  2006/01/10 16:46:18  frank
// Fixed display of CAD vendor image when only 1 of 2 cad files exists
//
// Revision 1.6.2.1  2005/11/15 23:29:38  frank
// removed unused debug messages
//
// Revision 1.6  2005/11/01 21:22:00  frank
// added more error messages
//
// Revision 1.5  2005/11/01 14:49:18  frank
// Issue #4503: Read new .cdv and .vol files instead of _hq files
//
// Revision 1.4  2005/10/06 18:05:27  vxconfig
// converted volume to world units using hq volume for correctness
//
// Revision 1.3  2005/10/06 17:42:45  jmeade
// initialize cad vis map *before* reading from file.
//
// Revision 1.2  2005/09/29 18:32:21  vxconfig
// moved cad finding visibility tester to utilities
//
// Revision 1.1  2005/09/28 20:24:11  vxconfig
// renamed study files to dataset
//
// Revision 1.8  2005/09/28 20:08:38  vxconfig
// Pulling weeds - replaced old block volume with new for inside volume
//
// Revision 1.7  2005/09/28 17:00:59  vxconfig
// added sorting, needs more work
//
// Revision 1.6  2005/09/27 01:19:32  jmeade
// enums for cad display constants.
//
// Revision 1.5  2005/09/27 01:12:54  jmeade
// context menu for cad findings.
//
// Revision 1.4  2005/09/26 14:11:21  vxconfig
// added function to determine visibility of a cad finding
//
// Revision 1.3  2005/09/23 12:02:28  vxconfig
// further changes to show cad marks behind folds
//
// Revision 1.2  2005/09/22 16:59:43  jmeade
// store cad finding locations along centerline.
//
// Revision 1.1  2005/09/13 12:59:56  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.5  2005/08/30 15:54:11  geconomos
// Issue# 4345: Added debugging message for load stages
//
// Revision 1.4  2005/08/23 16:31:45  geconomos
// Corrected merge from vx repository.
//
// Revision 1.3  2005/08/19 13:00:09  vxconfig
// merged incremental changes from vx VC_1-6 to v3d
//
// Revision 1.2  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.49.2.13.2.8.2.2  2005/07/13 17:22:23  geconomos
// Added a method to indicate if the joined skeleton has been computed
//
// Revision 3.49.2.13.2.8.2.1  2005/06/27 17:36:38  geconomos
// Issue #4173: (Frank) Fixed distance from anal verge when the viewpoint is adjusted for better path visibility which changes the distance from rectum
//
// Revision 3.49.2.13.2.8  2005/06/14 22:13:59  jmeade
// Merge from VC_1-5_Full-Oblique
//
// Revision 3.49.2.13.2.7  2005/06/14 19:13:26  geconomos
// Un-did changes to high resolution 3d rendering.
//
// Revision 3.49.2.13.2.6.2.1  2005/06/14 17:46:55  jmeade
// removed code that was previously moved.
//
// Revision 3.49.2.13.2.6  2005/04/22 16:49:05  frank
// cleaned up field class interface
//
// Revision 3.49.2.13.2.5  2005/04/13 14:50:29  frank
// Issue #4112: Working on the crash during study load
//
// Revision 3.49.2.13.2.4  2001/01/02 11:17:23  frank
// Issue #3984: Fixing update of oblique slice shadow when stopping flight
//
// Revision 3.49.2.13.2.3  2001/01/02 09:24:31  jmeade
// Create separate stage for loading volume fields (w/ 2d slice loading).
//
// Revision 3.49.2.13.2.2  2005/03/14 22:57:02  jmeade
// Loading and displaying slice data as early as possible during study load.
//
// Revision 3.49.2.13.2.1  2005/03/03 19:22:39  jmeade
// Issue 4003: Removed unnecessary current patch placeholder.
//
// Revision 3.49.2.12.2.2  2005/02/04 23:57:09  jmeade
// Allowing for variances in 2D and 3D volume coordinate units.
//
// Revision 3.49.2.12.2.1  2004/11/16 22:36:07  jmeade
// Identify dataset modality.
//
// Revision 3.49.2.12  2004/05/21 21:10:07  jmeade
// Use start and stop values in color setting. (only stop values were used b4).
//
// Revision 3.49.2.11  2004/05/19 13:07:51  geconomos
// Added a methods to update the colormap in vxSliceRendererData from the biopsy transfer function.
//
// Revision 3.49.2.10  2003/05/23 19:34:11  kevin
// Code Walkthrough cosmetic changes
//
// Revision 3.49.2.9  2003/04/28 17:53:34  jmeade
// Issue 2870: Ensure array access will not be overrun with 1-based segment identifier.
//
// Revision 3.49.2.8  2003/04/14 22:27:25  kevin
// Be careful not to nibble the entire centerline away
//
// Revision 3.49.2.7  2003/04/04 20:13:26  kevin
// Issue 2835: The threads were crashing on creatiion. 2 things were
// done to fix this:
// (1) Handle rendierng if the threads are non-existant by switching
// to sequential rendering (this should have been in there nayway)
// (2) Create the threads early when there is plenty of memory blocks
//
// Revision 3.49.2.6  2003/03/26 23:15:37  jmeade
// Issue 2849:  Const equivalents to a few access methods.
//
// Revision 3.49.2.5  2003/03/21 01:41:18  jmeade
// Issue 2883:  Round floating point numbers in voxel calculations.
//
// Revision 3.49.2.4  2003/03/07 06:02:35  jmeade
// Issue 2835:  Plugging up careless subtractions from uint's and unchecked comparisons b/w signed and unsigned vars.
//
// Revision 3.49.2.3  2003/03/04 05:55:14  jmeade
// Issue 2835: Invalid normal vector.
//
// Revision 3.49.2.2  2003/02/27 02:20:44  jmeade
// Issue 2856:  More concise method for triggering child view updates.
//
// Revision 3.49.2.1  2003/02/12 23:22:47  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 3.50  2003/02/10 18:49:46  kevin
// Registration Debug Info
//
// Revision 3.49  2003/01/22 22:28:31  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 3.48  2001/01/02 19:15:31  ingmar
// moved ProjectOntoPlane() to class Plane
//
// Revision 3.47  2001/01/02 19:09:24  ingmar
// moved ProjectOntoPlane() to class Plane
//
// Revision 3.46  2002/11/26 00:40:29  kevin
// Changed surface initialization logic
//
// Revision 3.45  2002/11/24 17:46:51  kevin
// Added Dr. Brewington's "Homogenity" measure and HU text on Highly Zoomed BLock Volumes
//
// Revision 3.44  2002/11/22 19:35:09  jmeade
// Identify dataset orientation when loading dataset.
//
// Revision 3.43  2002/11/21 23:55:04  jmeade
// Code standard on m_trackball* variables.
//
// Revision 3.42  2002/11/15 16:47:07  kevin
// Recompute UpVectors after smoothing skeleton
//
// Revision 3.41  2002/11/14 19:35:16  kevin
// Hmm problems with trying to increase quality of straight segments
//
// Revision 3.40  2002/11/11 21:17:43  kevin
// A little bit better straight segment estimation
//
// Revision 3.39  2002/11/11 14:28:21  kevin
// Initialize current segment on load (e.g. sessions)
//
// Revision 3.38  2002/10/31 19:59:26  kevin
// 1) Free up memroy on close to allow paint vol to be written to linear-vol file
// 2) Add load step so post-load init is always at the end even for both memory configurations
//
// Revision 3.37  2002/10/21 18:51:53  kevin
// Kevin's refactored S/P Registration with new heirarchical algorithm
//
// Revision 3.36  2002/09/30 20:46:33  ingmar
// moved ToAscii() to 03_utils/utilities.h
//
// Revision 3.35  2002/09/30 19:20:28  kevin
// Don't know why this got checked in different than SkeletonVCExt
//
// Revision 3.34  2002/09/29 21:44:15  kevin
// Re-Factor of SUpine/Prone registration ot place data structures
// in more meaningful places and remove so much copying of
// large data strucutres (there was a lot of creating local copies of
// refernces passed into toomany classes).
//
// Suzi and Ana's original registration is still presetn. In fact the registration
// still runs off their data structures. I jsut added new ones. From here on, I will
// have to break theres. So I wanted a version I could always go back
// to in case I broke anything.
//
// Revision 3.33  2002/09/27 19:04:50  jmeade
// Save/set saved current world location; Function returning static member data should be static; Removed unused function/variable.
//
// Revision 3.32  2002/09/13 17:22:45  jmeade
// Function name change; Paint volume handles all volume and value serialization.
//
// Revision 3.31  2002/09/13 00:59:31  jmeade
// Comments on paint volume save code.
//
// Revision 3.30  2002/09/13 00:51:15  jmeade
// ColonSession class for appropriate data; Save to disk/reset paint volume data as appropriate.
//
// Revision 3.29  2002/09/11 19:18:33  kevin
// Updates to when and how registration is run during the users traversal
// of the verification screen
//
// Revision 3.28  2002/09/02 20:38:14  kevin
// Render a specific sized segment and mark which part the user should focus on
//
// Revision 3.27  2002/08/30 18:59:52  kevin
// Adjust default Segment Error Threshold
//
// Revision 3.26  2002/08/28 20:00:52  jmeade
// Registration is getting ready for prime-time.
//
// Revision 3.25  2002/08/22 14:58:49  jmeade
// Removed debug code.
//
// Revision 3.24  2002/08/20 19:23:41  kevin
// 2D and 3D segment views.
//
// Revision 3.23  2002/08/15 15:37:30  kevin
// Added Segmental Visualization prototype/test code.
//
// Revision 3.22  2002/08/08 16:28:37  jmeade
// Like girls moving furniture: Logger --> vxLogger.
//
// Revision 3.21  2002/07/11 23:25:52  jmeade
// Implementing user-defined registration point specification.
//
// Revision 3.20  2002/06/26 17:49:55  jmeade
// Moving more hard-coded strings to the string table; Code standards.
//
// Revision 3.19  2002/06/10 22:39:24  jmeade
// Merged with VC_1-2_CS-1 branch, tag:  Joined_On_06-10-2002
//
// Revision 3.18  2002/06/04 18:37:10  jmeade
// Merged with VC_1-2_CS-1 Branch, tag: Joined_On_06-03-2002
//
// Revision 3.17  2002/04/12 02:15:19  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.16  2002/03/13 21:56:40  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.15  2002/02/06 15:43:20  binli
// issue 1699: correct setting of fFactor (refer revision 3.14)
//
// Revision 3.14  2002/02/05 22:13:36  binli
// Proper setting of default zoom factor in trackball according to the volume size.
//
// Revision 3.13.2.8  2002/06/06 01:36:36  kevin
// Added check for 2GB of memeory. If this is present we can load the linear volumes
// first since they will nto swap out. This helps with Address space fragmentation
// since the large temporary "uncompres" buffers are allocated earlier. This is
// required now that we use a graphics card which takes 128MB of address space
// for it's on baord texture memory that is mapped into the processes address sapce.
//
// Revision 3.13.2.7  2002/06/04 19:22:29  jmeade
// Issue 2270:  Allow class user to specify file extension.
//
// Revision 3.13.2.6  2002/05/23 16:00:56  jmeade
// Issue 2270:  List of 2D annotations.
//
// Revision 3.13.2.5  2002/03/07 16:26:57  jmeade
// Just rethrow caught errors.
//
// Revision 3.13.2.4  2002/02/27 15:27:43  binli
// issue 1699: refixed (since the change in Revision 3.13.2.3)
//                    (if the loading order would chang in the future, be sure to call function ReaderGlobal::SetupZoomFactorInOverview() right after the m_vDim&m_vUnits are set FOR the FIRST time)
//
// Revision 3.13.2.3  2002/02/25 16:04:32  kevin
// Dramatically changed the patient loading mechanism. This is required to fix
// memory erros both in physicial memory and in adress space fragmentation.
// Moved the control of the loading order to Study class. Fields now only
// handles mutiple volumes in one load. ReaderGlobal now only calls Study
// in a loop which allows Supine and Prone loads to be interleaved (the
// part in Readglobal was simplified alot and the logic placed in study so that
// all the logic is in one place in case we want to play with the order of
// loading in the future)
//
// Also added pre-allocation of all the linear volumes before address is fragmented.
//
// Also added timing info into the LogFyi log file so that we can track the time it
// takes to laod even in the field.
//
// Revision 3.13.2.2  2002/02/11 15:14:37  binli
// issue 1699 in branch: setting default zoom factor setting in trackball according to volume size
//
// Revision 3.13.2.1  2002/02/10 20:22:28  kevin
// Major fixes for TTP rendering algorithm
//
// Revision 3.13  2002/01/10 18:04:09  kevin
// Made diff volumes lazy-load since they will not be used in the common case
//
// Revision 3.12  2002/01/08 17:14:42  kevin
// Only create registration data if not in Fianl Release for this version
//
// Revision 3.11  2002/01/04 16:33:19  kevin
// Some cleanup after the hacking to get TTP rendering without ses,set,sec
//
// Revision 3.10  2002/01/03 02:25:00  kevin
// Removed ses, set,sec and added build opacity volume
//
// Revision 3.9  2001/12/29 00:13:22  jmeade
// Merge from 1.1 branch.
//
// Revision 3.8  2001/11/14 19:38:44  ana
// In GoThroughSegments added the update to the segment direction flag.
//
// Revision 3.7  2001/11/14 01:34:16  kevin
// Cleaned up alot of Supine/Proneregistration stuff, fixed some
// of the multi-rendering and added feature match pair rendering
//
// Revision 3.6  2001/11/06 15:27:57  kevin
// Removed ses, set, and sec volumes
//
// Revision 3.5  2001/11/05 18:48:34  ana
// GoThroughSegments moved to Study.
//
// Revision 3.4  2001/11/05 17:52:18  ana
// Registration view added.
//
// Revision 3.3  2001/11/01 19:52:59  kevin
// Added 3D Uncleansed DIff-Block_volume rendering
//
// Revision 3.2  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.1.2.1  2001/11/15 19:51:47  jmeade
// InitializeVisualization() returns false when total segment length less than 100 cm.
//
// Revision 3.1  2001/10/23 22:35:36  jmeade
// Corrected a mispelling
//
// Revision 3.0  2001/10/14 23:01:54  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Oct 03 2001 20:06:20   jmeade
// User load abort should throw AbortLoadException, not AbortOperationEx; spelling errors in exception comment
// 
//    Rev 2.0   Sep 16 2001 23:40:26   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:18   ingmar
// Initial revision.
// Revision 1.121  2001/09/14 18:12:09  kevin
// TR ID # 631.
// Fixed Black spots in colon by not derefernceing the current
// study to rdrGLobal.m_pStudy (whihc is always the currently
// visible study and thus alsways supine in the begining when
// the datset is loaded and the trandfer functiosn generated)
//
// Revision 1.120  2001/08/31 18:57:23  soeren
// removed the whole stream classes and disabled fyi,dbg,wrn,tmp stream support.
//
// Revision 1.119  2001/08/21 20:23:46  binli
// bug 612: fixed
//
// Revision 1.118  2001/08/20 20:08:08  tushar
// removed some errors in vc2000src due to logger class
//
// Revision 1.117  2001/08/16 14:24:55  binli
// bug 000486:
//
// Revision 1.116  2001/08/15 15:39:14  jmeade
// Bug 652:  Clear list of missed patches when initializing study visualization
//
// Revision 1.115  2001/08/09 22:27:36  binli
// bug 000567: make sure no out of bound
//
// Revision 1.114  2001/07/26 20:29:50  binli
// ID 89: exception handling.
//
// Revision 1.113  2001/07/25 22:52:17  binli
// ID 89: new exception handling.
//
// Revision 1.112  2001/07/19 15:07:09  binli
// caught 'Abort' exception
//
// Revision 1.111  2001/07/16 23:48:08  jmeade
// VException::GetMessage --> GetMessageString
//
// Revision 1.110  2001/07/13 19:52:38  binli
// defect 547: the error message (to user) has simplified.
//
// Revision 1.109  2001/07/12 21:12:40  binli
// defect ID: 547.
// (the error message shold be simplified.)
//
// Revision 1.108  2001/06/26 16:20:49  ana
// Results of code review for Fields and Jitter classes
//
// Revision 1.107  2001/06/21 21:15:19  binli
// speed up overview/slice render in StandardView/Endo-OrthogonalView.
// (defect 263, partial)
//
// Revision 1.106  2001/05/31 21:57:11  binli
// bug 000484 (step1: distance): use current study, not rdrGlobal.m_pStudy.
//
// Revision 1.105  2001/05/25 20:28:59  kevin
// Various coding conventions related to ReaderGlobal and VolumeRender
//
// Revision 1.104  2001/05/24 20:40:40  binli
// reflected movement of trackball class from readerlib to vxBase/9_render
//
// Revision 1.103  2001/05/19 00:41:05  jmeade
// Keeping all exceptions localized in Exception.h
//
// Revision 1.102  2001/05/17 20:42:20  binli
// VerifyView vs. StandardVIew/S&PView: controled under different trackball.
//
// Revision 1.101  2001/05/15 17:49:37  kevin
// Finished coding compliance for ReaderGlobal for wlakthrough
//
// Revision 1.100  2001/05/07 00:51:27  kevin
// Added a paintVol and cleaned up ReaderGLobal some more
// (Got more to go, wanted to finish this weekend, but had
// problems chciking in CVS like Jeff Meade did last week)
//
// Revision 1.99  2001/05/02 20:12:13  ingmar
// BookmarkList.C
//
// Revision 1.98  2001/04/30 12:30:30  frank
// Added exception handling.
//
// Revision 1.96.2.1  2001/05/01 23:52:40  jmeade
// m_volume changed to m_volumeRender (April 26 - jmeade)
//
// Revision 1.97  2001/04/26 15:50:56  jmeade
// m_volume changed to m_volumeRender
//
// Revision 1.96  2001/04/18 15:45:52  frank
// Reflected change in Submarine class.
//
// Revision 1.95  2001/04/13 13:28:56  frank
// Started using the streams interface.
//
// Revision 1.94  2001/04/13 13:01:39  frank
// Moved slice loading to Fields.
//
// Revision 1.93  2001/04/13 11:53:39  frank
// Protected some members of Fields.
//
// Revision 1.92  2001/04/12 19:20:49  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.91  2001/04/12 18:01:18  jmeade
// file extension class
//
// Revision 1.90  2001/04/12 15:58:20  dave
// added namespace vx for exceptions
//
// Revision 1.89  2001/04/11 14:07:27  frank
// Renamed class members of Fields.
//
// Revision 1.88  2001/04/11 13:27:31  frank
// Moved volume data to Fields class.
//
// Revision 1.87  2001/04/07 04:45:30  jmeade
// Bug: Interface reporting error on user abort.  Fix:  Better throw and catch
// of abort exception in ReaderGlobal, Study, Fields
//
// Revision 1.86  2001/04/05 20:43:38  binli
// reflected change in TrackBall: private matrix.
//
// Revision 1.85  2001/04/05 16:27:06  binli
// variable name converntion/made m_qCurrQuat private
//
// Revision 1.84  2001/03/27 23:19:30  binli
// extended the function of class 'Quaternion'.
//
// Revision 1.83  2001/03/26 16:04:07  frank
// Reflected changes in Segment class.
//
// Revision 1.82  2001/03/21 14:59:49  frank
// Updated transfer function class for coding guideline compliance.
//
// Revision 1.81  2001/03/19 18:06:22  frank
// Reflected a variable name change in Segment class.
//
// Revision 1.80  2001/03/19 15:28:46  frank
// Continued to update to reflect new coding standards.
//
// Revision 1.79  2001/03/19 11:59:04  frank
// Went from register variables (I2 U4 F8) to small letters (int2 uint4 float8).
//
// Revision 1.78  2001/03/16 16:24:33  frank
// Updated variable names in Study class according to new coding standards.
//
// Revision 1.77  2001/03/16 14:38:03  frank
// Updated ReaderGlobal variables for new coding standards.
//
// Revision 1.76  2001/03/09 01:14:57  jmeade
// check validity (IsVolumeLoaded) of study prior to saving bookmarks;
// write correct bookmark file by study-specific title
//
// Revision 1.75  2001/03/09 01:07:08  jmeade
// protect access to some more Study members
//
// Revision 1.74  2001/03/01 01:58:28  jmeade
// m_bNeedVisualizationReinit member variable; set true in volume load (LoadStep...),
// set false in MarkSurfaces()
//
// Revision 1.73  2001/02/27 17:59:17  jmeade
// Missed regions list (m_patchList) a member of Study class
//
// Revision 1.72  2001/02/22 01:35:19  jeff
// code without coding conventions is gettting harder and harder to comprehend
//
// Revision 1.71  2001/02/13 12:57:23  frank
// Added a function to sample the original volume data instead of the e-cleansed data.
//
// Revision 1.70  2001/02/09 14:35:47  kevin
// Added "edge of volume" check in surface coverage calculation
//
// Revision 1.69  2001/02/08 19:57:21  frank
// Used PeekNExt iterators in MarkSurfaces.
//
// Revision 1.68  2001/02/08 19:10:36  frank
// Don't use PeekNext XYZ or PeekPrevXYZ for Block Volumes yet...
//
// Revision 1.67  2001/02/07 13:46:23  frank
// Improved the speed of initialization.
//
// Revision 1.66  2001/02/06 14:13:48  frank
// Revamped study load progress display.
//
// Revision 1.65  2001/02/01 13:41:17  frank
// Revamped study initialization procedure.
//
// Revision 1.64  2001/02/01 12:55:27  frank
// Initialized parent pointer another way.
//
// Revision 1.63  2001/01/31 19:04:18  kevin
// Moved transferfunction to datastruct so triangle rendering could get
// correct colors
//
// Revision 1.62  2001/01/31 14:09:14  frank
// Changed to reflect updated submarine class.
//
// Revision 1.61  2001/01/29 22:15:50  liwei
// Attempted to move the opacity volume to study or fields, not finished yet
//
// Revision 1.60  2001/01/29 18:18:44  frank
// Synchronization.
//
// Revision 1.59  2001/01/29 18:16:46  frank
// Removed error suppression.
//
// Revision 1.58  2001/01/25 17:41:25  frank
// Indicated volume load completion.
//
// Revision 1.57  2001/01/25 17:28:57  frank
// Removed old functions.
//
// Revision 1.56  2001/01/24 20:16:26  frank
// Cleaned up history at top of file.
//
// Revision 1.55  2001/01/24 19:18:20  liwei
// Software volume rendering for overview
//
// Revision 1.54  2001/01/24 17:46:42  frank
// Indicated the completion of volume load.
//
// Revision 1.53  2001/01/24 16:41:41  frank
// Upgraded study load functions for supine/prone.
//
// Revision 1.52  2001/01/23 19:35:38  frank
// Removed excess warning boxes.
//
// Revision 1.51  2001/01/23 17:43:23  jeff
// enums should be capitalized
//
// Revision 1.50  2001/01/19 21:07:09  kevin
// Added OnStudyLoadedAndDone Verification function to perform
// tasks once both of these conditions are true. The function currently
// contins code to calculate the percent coverage values and to
// calculate segmnet offsets from the rectum once the skeletons have
// been smoothed and the segmnet order defined.
//
// Revision 1.49  2001/01/11 15:29:07  frank
// Caught null memory exception.
//
// Revision 1.48  2001/01/11 13:08:45  frank
// Improved load progress metering, better handled dual studies.
//
// Revision 1.47  2001/01/09 15:20:48  frank
// Cleaned up study orientations in slice view.
//
// Revision 1.46  2001/01/08 18:42:59  frank
// Clarified Supine-Prone dataset distinction.
//
// Revision 1.45  2001/01/05 19:27:39  frank
// Moved marking functions into Study class.
//
// Revision 1.44  2001/01/05 14:59:22  frank
// Coding guidelines.
//
// Revision 1.43  2001/01/03 16:46:57  binli
// removed self-defined matrix (replaced with 3_math::matrix)
//
// Revision 1.42  2001/01/02 20:01:05  frank
// Changed the behavior of study aborting.
//
// Revision 1.41  2000/12/27 23:04:01  jeff
// TRANSVERSE ==> AXIAL, CROSSECT ==> CROSS_SECTION
//
// Revision 1.40  2000/12/27 20:22:08  binli
// zooming (6):make sure the arrow is visible  when double click in Overview.
//
// Revision 1.39  2000/12/22 15:52:21  frank
// Cleared out header map information.
//
// Revision 1.38  2000/12/22 14:55:55  frank
// Removed spurious warnings
//
// Revision 1.37  2000/12/19 14:49:55  frank
// dead code cleanup
//
// Revision 1.36  2000/12/19 02:31:11  kevin
// True/false on segment load checking whether the secondary study can be loaded
//
// Revision 1.35  2000/12/18 19:40:21  frank
// Changed segment filename to filename prefix
//
// Revision 1.34  2000/12/12 18:01:14  frank
// Removed spurious warning upon loading secondary volume header
//
// Revision 1.33  2000/12/11 21:39:35  dave
// slight changes in Study.C,Fields.C,VolumeRender.C in try catch
//
// Revision 1.32  2000/12/09 15:39:18  frank
// Removed spurious warnings
//
// Revision 1.31  2000/11/22 19:46:01  frank
// Moved study completion to after marking surfaces
//
// Revision 1.30  2000/11/22 16:55:50  frank
// Updated progress meter with all data fields
//
// Revision 1.29  2000/11/22 14:18:56  frank
// Showed axial images during loading
//
// Revision 1.28  2000/11/22 12:38:02  frank
// Added study load aborting
//
// Revision 1.27  2000/11/20 19:05:04  kevin
// Fixed the lnegth display
//
// Revision 1.26  2000/11/20 00:14:43  jeff
// Inserted a couple of load update callbacks (prior to appropriate steps)
//
// Revision 1.25  2000/11/17 21:18:58  kevin
// FIxed some errors from changing the rendersize to be non-power-of-two
//
// Revision 1.24  2000/11/17 18:27:36  frank
// Added Reset functions
//
// Revision 1.23  2000/11/17 16:51:26  binli
// Made sure if the second study (segments and volume head at present) can't be opened,
// the second window in VerifyView would not be blank. ---- temporal version.
//
// Revision 1.22  2000/11/17 04:26:19  liwei
// Bugs fixed for VolumePro
//
// Revision 1.21  2000/11/16 22:41:33  binli
// Simply registation of Supine  and Prone.
//
// Revision 1.20  2000/11/16 15:23:21  jeff
// LoadCallback class to keep track of study load state information
//
// Revision 1.19  2000/11/15 23:31:57  liwei
// Minor changes for VolumePro
//
// Revision 1.18  2000/11/15 21:51:56  liwei
// The errors in scaling vli volume corrected
// Axial slices loaded earlier so that vli volume can be created earlier
//
// Revision 1.17  2000/11/15 19:13:45  liwei
// Add features to read transfer funciton from file
//
// Revision 1.16  2000/11/14 23:53:04  liwei
// Add support for VolumePro
//
// Revision 1.15  2000/11/14 20:32:58  jeff
// Read header info from .ecv instead of .ins
//
// Revision 1.14  2000/11/14 19:24:37  frank
// Flushed accumulation buffer during jumps
//
// Revision 1.13  2000/11/14 18:56:20  frank
// Changed study reference to pointer
//
// Revision 1.12  2000/11/14 18:32:29  binli
// added * assignment function
//
// Revision 1.11  2000/11/14 17:32:47  frank
// Increased smoothing of centerline
//
// Revision 1.10  2000/11/13 20:01:34  frank
// Added centerline smoothing
//
// Revision 1.9  2000/11/13 19:15:12  binli
// 1. moved some (Supine/Prone related) stuffs from 'ReaderGlobal' to 'Study'.
// 2. adjusted lock/unlock controls in VerifyView.
//
// Revision 1.8  2000/11/10 20:19:21  frank
// Removed unnecessary warning
//
// Revision 1.7  2000/11/09 20:11:08  frank
// Moved Study-specific information to study class from ReaderGlobal
//
// Revision 1.6  2000/11/09 17:41:59  frank
// Added reading for the secondary study
//
// Revision 1.5  2000/11/09 15:42:44  frank
// Cleaned up
//
// Revision 1.4  2000/11/08 17:38:56  frank
// Added m_study to ReaderGlobal
//
// Revision 1.3  2000/11/08 04:39:59  frank
// Moved ecv into Study class
//
// Revision 1.2  2000/11/07 15:52:07  frank
// Added copy constructor and assignment operator
//
// Revision 1.1  2000/11/07 14:44:17  frank
// Added Study class
//  
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/Dataset.C,v 1.37.2.30 2010/12/20 18:53:26 dongqing Exp $
// $Id: Dataset.C,v 1.37.2.30 2010/12/20 18:53:26 dongqing Exp $
