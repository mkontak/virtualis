// $Id: SliceChildView.Region.C,v 1.1.2.34 2009/08/24 14:13:47 dongqing Exp $
//
// Copyright © 2008, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Description: Extension for the Viewer of 2D slices
// Owners: Dongqing Chen (dongqing@viatronix.com)

// includes
#include "stdafx.h"
#include "SliceChildView.h"
#include <sstream>
#include "MainAppFrame.h"
#include "ReaderGlobal.h"
#include "resource.h"
#include "HUconverter.h"
#include "Rect.h"
#include "RenderUtil.h"
#include "VCRenderUtils.h"
#include "Viatron.h"
#include "NavigationView.h"
#include "vxPolypSegmentation.h"
#include "vxHistogramUpdaterLabel.h"
#include "VolumePaint.h"
#include "vxBlockVolumeIteratorNeighbors.h"
#include "ReportViewWnd.h"

#define FILE_REVISION "$Revision: 1.1.2.34 $"


// namespaces
using namespace std;
using namespace ColonViews::Layouts;
using namespace WinUtils;
using namespace ReaderLib;
using namespace vxCAD;


/**
 * Segments a (user-)selected region
 *
 * @param selectPoint    The (user-)selected point to begin region segmentation in 3D
 * @param mouseClickPt   Mouse click point in the window
 */
void SliceChildView::SegmentRegion(const Point<float4>& selectPoint, const Point2D<int4>& mouseClickPt)
{
  CWaitCursor crsr;

  Dataset& rDataset = GetDatasetRef();
  rdrGlobal.SetDatasetOrientation(rDataset.GetOrientation());

  vxShareableObject< vxComponentArray > * pComponentArray( rDataset.m_fields.m_sliceRendererData.GetComponents() );
  uint2 uHistoryIndex = pComponentArray->AddComponent("Suspicious ROI", PixelRGBA<uint1>(255, 0, 0, 255) );
  const uint2 uNewLabel = (*pComponentArray)[uHistoryIndex]->GetLabel();

  vxPolypSegmentation polypSeg( rDataset.m_fields.m_volumeRender,
    *rDataset.m_fields.m_sliceRendererData.GetCleansedVolume(),
    *rDataset.m_fields.m_sliceRendererData.GetLabelVolume(), uNewLabel,
     rDataset.m_fields.m_ins, rDataset.m_fields.m_paintVol, selectPoint );

  Point3D<float4> ptEnd1;
  Point3D<float4> ptEnd2;
  float4 fMaximumDiameter;
  float4 fPolypVolume;

  try
  {
    int4 iIndicator = polypSeg.Run();

    vxHistogramUpdaterLabel<uint2>::Update( *rDataset.m_fields.m_sliceRendererData.GetLabelVolume() );

    if ( iIndicator < 0 )
    { // ROI does not like a polyp
      AfxMessageBox(IDS_REGIONSEGMENT_NONEFOUND);
      pComponentArray->RemoveComponent( uHistoryIndex );
    }
    else if ( iIndicator == 1 )
    { // ROI seems like a small polyp!

      if (AfxMessageBox(IDS_REGIONSEGMENT_SMALLREGION, MB_ICONQUESTION | MB_YESNO) == IDYES)
      { // User selects to keep the ROI
        fMaximumDiameter = polypSeg.GetMaxDiameter( ptEnd1, ptEnd2 );
        fPolypVolume = polypSeg.GetVolume();

        vxCadVector pt0(      0.0F,       0.0F,       0.0F );
        vxCadVector pt1(ptEnd1.m_x, ptEnd1.m_y, ptEnd1.m_z );
        vxCadVector pt2(ptEnd2.m_x, ptEnd2.m_y, ptEnd2.m_z );

        vxCadDimension largest(pt1, pt2);
        vxCadDimension depth  (pt0, pt0);
        vxCadDimension width  (pt0, pt0);

        vxCadDimensions regionDim(largest, width, depth);

        vxCadFinding suspiciousRegion;
        suspiciousRegion.SetDimensions( regionDim );
        pt0.SetVector( Vector3D<float4>(selectPoint.X(), selectPoint.Y(), selectPoint.Z()) );
        suspiciousRegion.SetCentroidMM( pt0 );

        vxCadROI roi;
        roi.SetVolumeValue( fPolypVolume );
        roi.SetLabelIndex( uNewLabel ); 

        // record ROI voxels for session
        RecordROIVoxels( roi, uNewLabel, *rDataset.m_fields.m_sliceRendererData.GetLabelVolume() );

        suspiciousRegion.SetVolumeROI(roi);

        rDataset.m_suspiciousRegions.AddFinding( suspiciousRegion );

        rDataset.m_fields.m_volumeRender.FlushAccumulationBuffer();
        vxHistogramUpdaterLabel<uint2>::Update( *rDataset.m_fields.m_sliceRendererData.GetLabelVolume() );
        ForceRedraw( true );
        AfxGetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(IDM_REPORT_ADDSNAPSHOT, 0), 0);
      }
      else
      {
        polypSeg.ResetPaintVolPerUserRequest();
        pComponentArray->RemoveComponent( uHistoryIndex );
      }
    }
    else if( iIndicator == 0 )
    { // ROI does like a regular polyp
      fMaximumDiameter = polypSeg.GetMaxDiameter( ptEnd1, ptEnd2 );
      fPolypVolume = polypSeg.GetVolume();

      vxCadVector pt0(      0.0F,       0.0F,       0.0F );
      vxCadVector pt1(ptEnd1.m_x, ptEnd1.m_y, ptEnd1.m_z );
      vxCadVector pt2(ptEnd2.m_x, ptEnd2.m_y, ptEnd2.m_z );

      vxCadDimension largest(pt1, pt2);
      vxCadDimension depth  (pt0, pt0);
      vxCadDimension width  (pt0, pt0);

      vxCadDimensions regionDim(largest, width, depth);

      vxCadFinding suspiciousRegion;
      suspiciousRegion.SetDimensions( regionDim );

      pt0.SetVector( Vector3D<float4>(selectPoint.X(), selectPoint.Y(), selectPoint.Z()) );
      suspiciousRegion.SetCentroidMM( pt0 );

      vxCadROI roi;
      roi.SetVolumeValue( fPolypVolume );
      roi.SetLabelIndex( uNewLabel );

       // record ROI voxels for session
      RecordROIVoxels( roi, uNewLabel, *rDataset.m_fields.m_sliceRendererData.GetLabelVolume() );

      suspiciousRegion.SetVolumeROI(roi);

      rDataset.m_suspiciousRegions.AddFinding( suspiciousRegion );

      rDataset.m_fields.m_volumeRender.FlushAccumulationBuffer();
      vxHistogramUpdaterLabel<uint2>::Update( *rDataset.m_fields.m_sliceRendererData.GetLabelVolume() );
      ForceRedraw( true );
      AfxGetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(IDM_REPORT_ADDSNAPSHOT, 0), 0);

    }
    else if ( iIndicator == 3 )
    {
      AfxMessageBox("This ROI has already been segmented.");
      pComponentArray->RemoveComponent( uHistoryIndex );
    }
    else
    {
      AfxMessageBox(IDS_REGIONSEGMENT_ERROROCCURRED);
      pComponentArray->RemoveComponent( uHistoryIndex );
    }
  } // try
  catch( ... )
  {
    AfxMessageBox(IDS_REGIONSEGMENT_ERROROCCURRED);
    pComponentArray->RemoveComponent( uHistoryIndex );
    throw;
  }

} // SegmentRegion( selectPoint, mouseClickPt )


/**
 * Record a single point SROI. This is for clinical trial creating a truth
 *
 * @param selectPoint    The (user-)selected point to begin region segmentation in 3D
 */
void SliceChildView::RecordAPointOnly(const Point<float4>& selectPoint)
{
  CWaitCursor crsr;

  Dataset& rDataset = GetDatasetRef();

  vxShareableObject< vxComponentArray > * pComponentArray( rDataset.m_fields.m_sliceRendererData.GetComponents() );
  uint2 uHistoryIndex = pComponentArray->AddComponent("Suspicious ROI", PixelRGBA<uint1>(255, 0, 0, 255) );
  const uint2 uNewLabel = (*pComponentArray)[uHistoryIndex]->GetLabel();

  Point3D<float4> ptEnd1(selectPoint);
  Point3D<float4> ptEnd2(selectPoint);
  float4 fMaximumDiameter;
  float4 fPolypVolume;

  try
  {
    Triple<float4> vfUnits( rDataset.m_vUnits );
    Triple<int4>   viDim  ( rDataset.m_vDim );

    Vector3D<int4> posVol(selectPoint.m_x/vfUnits.m_x+0.5F, selectPoint.m_y/vfUnits.m_y+0.5F, selectPoint.m_z/vfUnits.m_z+0.5F);

    /// record one point in the label volume
    vxBlockVolumeIterator<uint2> itl(*rDataset.m_fields.m_sliceRendererData.GetLabelVolume());
    itl.SetPos(posVol);
    itl.SetVoxel(uNewLabel);
    vxHistogramUpdaterLabel<uint2>::Update( *rDataset.m_fields.m_sliceRendererData.GetLabelVolume() );

    /// paint the surface point in endoview
    uint1 * pPaintVol = rDataset.m_fields.m_paintVol.GetDataPtr();
    const int4 iS( viDim.m_x*viDim.m_y );
    const int4 ip( posVol.m_x + posVol.m_y*viDim.m_x + posVol.m_z*iS );
    //register int4 x, y, z;   /// this may paint too much!
    //for( z=-1; z<2; z++ )
    //{
    //  for( y=-1; y<2; y++ )
    //  {
    //    for( x=-1; x<2; x++ )
    //      pPaintVol[ip + x + z*iS + y*viDim.m_x] = SUSPICIOUSROI;
    //  } // for y
    //} // for z
    pPaintVol[ip] = SUSPICIOUSROI;

    fMaximumDiameter = 0;
    fPolypVolume = 0;

    vxCadVector pt0(      0.0F,       0.0F,       0.0F );
    vxCadVector pt1(ptEnd1.m_x, ptEnd1.m_y, ptEnd1.m_z );
    vxCadVector pt2(ptEnd2.m_x, ptEnd2.m_y, ptEnd2.m_z );

    vxCadDimension largest(pt1, pt2);
    vxCadDimension depth  (pt0, pt0);
    vxCadDimension width  (pt0, pt0);

    vxCadDimensions regionDim(largest, width, depth);

    vxCadFinding suspiciousRegion;
    suspiciousRegion.SetDimensions( regionDim );

    pt0.SetVector( Vector3D<float4>(selectPoint.X(), selectPoint.Y(), selectPoint.Z()) );
    suspiciousRegion.SetCentroidMM( pt0 );

    vxCadROI roi;
    roi.SetVolumeValue( fPolypVolume );
    roi.SetLabelIndex( uNewLabel );

    // record ROI voxels for session
    RecordROIVoxels( roi, uNewLabel, *rDataset.m_fields.m_sliceRendererData.GetLabelVolume() );

    suspiciousRegion.SetVolumeROI(roi);

    rDataset.m_suspiciousRegions.AddFinding( suspiciousRegion );

    rDataset.m_fields.m_volumeRender.FlushAccumulationBuffer();
    vxHistogramUpdaterLabel<uint2>::Update( *rDataset.m_fields.m_sliceRendererData.GetLabelVolume() );
    ForceRedraw( true );
    AfxGetMainWnd()->SendMessage(WM_COMMAND, MAKEWPARAM(IDM_REPORT_ADDSNAPSHOT, 0), 0);
  } // try
  catch( ... )
  {
    AfxMessageBox(IDS_REGIONSEGMENT_ERROROCCURRED);
    pComponentArray->RemoveComponent( uHistoryIndex );
    throw;
  }
} // RecordAPointOnly( selectPoint )


/**
 * storing SRT ROI voxels for session
 * 
 * @param roi         the working vxCadROI
 * @param uROIlabel   the label of the ROI in the label volume
 * @param labelVol    the label volume
 */
void SliceChildView::RecordROIVoxels( vxCadROI & roi, const uint2 & uROIlabel, vxBlockVolume<uint2> & labelVol )
{
  vxBlockVolumeIterator<uint2> it( labelVol );

  register vxCadVoxelPosition pos;
  for( it.SetPos(0,0,0); it.IsNotAtEnd(); it.NextBlockZYX() )
  {
    for( ; it.IsNotAtEndOfBlock(); it.NextZYXinsideBlock() )
    {
      if( it.GetVoxel() == uROIlabel )
      {
        pos = vxCadVoxelPosition(it.GetPos().m_x, it.GetPos().m_y, it.GetPos().m_z);
        
        roi.AddVoxel( pos );
      }
    } // for inside a block
  } // for all blocks
} // RecordROIVoxels( roi, uROIlabel, labelVol )


/**
 * finds the closest SRT ROI to the given point
 * @param ptClient   client location
 * @return           label of the closest proximal SRT ROI, or 0 if none
 */
uint2 SliceChildView::FindProximalROILabel(const CPoint & ptClient)
{
  Point<float4> fpt(ptClient.x, ptClient.y, 0.0F);

  // Get the volume coordinate
  Vector3D<float> vfCurrent( Calculate3DPoint(fpt) );

  Dataset& rDataset = GetDatasetRef();

  // get the label volume
  vxBlockVolume<uint2> *pLabelVolume( rDataset.m_fields.m_sliceRendererData.GetLabelVolume() );
  Triple<float> vfUnits( rDataset.m_fields.m_sliceRendererData.GetCleansedVolume()->GetHeader().GetVoxelUnits() );

  Vector3D<int2> viCurrent(8,8,8);

  // compute the volume coordinate for the center of subvolume
  viCurrent.m_x = int2(vfCurrent.m_x/vfUnits.m_x+0.5F);
  viCurrent.m_y = int2(vfCurrent.m_y/vfUnits.m_y+0.5F);
  viCurrent.m_z = int2(vfCurrent.m_z/vfUnits.m_z+0.5F);

  // check if the current point is close to a suspicious region
  // if yes, rendering the measurement of the current suspicious region
  vxBlockVolumeIterator<uint2> itl(*pLabelVolume);
  itl.SetPos( viCurrent );

  uint2 uComponentLabel(0);
  if( rDataset.m_suspiciousRegions.GetCount() > 0 )
  {
    if( itl.GetVoxel() > 0 )
    {
      uComponentLabel = itl.GetVoxel();
    }
    else if( itl.PeekRel(1,0,0) > 0 )
    {
      uComponentLabel = itl.PeekRel(1,0,0);
    }
    else if( itl.PeekRel(2,0,0) > 0 )
    {
      uComponentLabel = itl.PeekRel(2,0,0);
    }
    else if( itl.PeekRel(3,0,0) > 0 )
    {
      uComponentLabel = itl.PeekRel(3,0,0);
    }
    else if( itl.PeekRel(-1,0,0) > 0 )
    {
      uComponentLabel = itl.PeekRel(-1,0,0);
    }
    else if( itl.PeekRel(-2,0,0) > 0 )
    {
      uComponentLabel = itl.PeekRel(-2,0,0);
    }
    else if( itl.PeekRel(-3,0,0) > 0 )
    {
      uComponentLabel = itl.PeekRel(-3,0,0);
    }
    else if( itl.PeekRel(0,1,0) > 0 )
    {
      uComponentLabel = itl.PeekRel(0,1,0);
    }
    else if( itl.PeekRel(0,2,0) > 0 )
    {
      uComponentLabel = itl.PeekRel(0,2,0);
    }
    else if( itl.PeekRel(0,3,0) > 0 )
    {
      uComponentLabel = itl.PeekRel(0,3,0);
    }
    else if( itl.PeekRel(0,-3,0) > 0 )
    {
      uComponentLabel = itl.PeekRel(0,-3,0);
    }
    else if( itl.PeekRel(0,-2,0) > 0 )
    {
      uComponentLabel = itl.PeekRel(0,-2,0);
    }
    else if( itl.PeekRel(0,-1,0) > 0 )
    {
      uComponentLabel = itl.PeekRel(0,-1,0);
    }
    else if( itl.PeekRel(0,0,1) > 0 )
    {
      uComponentLabel = itl.PeekRel(0,0,1);
    }
    else if( itl.PeekRel(0,0,2) > 0 )
    {
      uComponentLabel = itl.PeekRel(0,0,2);
    }
    else if( itl.PeekRel(0,0,3) > 0 )
    {
      uComponentLabel = itl.PeekRel(0,0,3);
    }
    else if( itl.PeekRel(0,0,-1) > 0 )
    {
      uComponentLabel = itl.PeekRel(0,0,-1);
    }
    else if( itl.PeekRel(0,0,-2) > 0 )
    {
      uComponentLabel = itl.PeekRel(0,0,-2);
    }
    else if( itl.PeekRel(0,0,-3) > 0 )
    {
      uComponentLabel = itl.PeekRel(0,0,-3);
    }
  }

  return uComponentLabel;
} // FindProximalROILabel( ptClient )


/**
 * finds the closest SRT ROI to the given point
 * @param ptClient    point to test in client coordinates
 * @return            the closest proximal roi's position in roi list
 */
int4 SliceChildView::FindProximalROI(const CPoint & ptClient)
{
  uint2 uComponentLabel(FindProximalROILabel(ptClient));

  if ( uComponentLabel > 0 )
  {
    // locate the component that this point is close to
    const vxCadFindings suspiciousRois = GetDatasetRef().m_suspiciousRegions;

    for ( int4 i(0); i<suspiciousRois.GetCount(); i++ )
    {
      vxCadFinding finding( suspiciousRois.GetFinding(i) );
      vxCadROI     roi( finding.GetVolumeROI() );
      uint2 uLabel( roi.GetLabelIndex() );

      if ( uLabel == uComponentLabel )
      {
        return i;
      }
    } // for
  } // if

  return -1;
} // FindProximalROI( ptClient )


/**
 * edit an existing segmented roi
 *
 * @param eMode         type of editing to be done
 * @param selectPoint   edit will be done as per this user-selected point
 */
void SliceChildView::EditROIRegion(const SEGMENTROIMODE eMode, const Point<float4> & selectPoint)
{
  Dataset& rDataset = GetDatasetRef();
  if( rDataset.m_suspiciousRegions.GetCount() < 1 )
  {
    AfxMessageBox("No suspicious region exists.\nPlease select a suspicious region first.");
    return;
  }

  const Triple<float4> vfUnits = rDataset.m_fields.m_sliceRendererData.GetLabelVolume()->GetHeader().GetVoxelUnits();
  const Vector3D<int4> viSelectPosInVol = Vector3D<int4>( int4(selectPoint.X()/vfUnits.m_x+0.5F), 
    int4(selectPoint.Y()/vfUnits.m_y+0.5F), int4(selectPoint.Z()/vfUnits.m_z+0.5F) );

  vxBlockVolumeIterator<uint2> itl( *rDataset.m_fields.m_sliceRendererData.GetLabelVolume() );

  uint2 uCurrentROIlabel = 0;

  itl.SetPos(viSelectPosInVol);

  if( itl.GetVoxel() > 0 ) 
  {
    uCurrentROIlabel = itl.GetVoxel();
  }
  else if( itl.Peek1px0py0pz() > 0 )
  {
    uCurrentROIlabel = itl.Peek1px0py0pz();
  }
  else if( itl.Peek1nx0py0pz() > 0 )
  {
    uCurrentROIlabel = itl.Peek1nx0py0pz();
  }
  else if( itl.Peek0px1py0pz() > 0 )
  {
    uCurrentROIlabel = itl.Peek0px1py0pz(); 
  }
  else if( itl.Peek0px1ny0pz() > 0 )
  {
    uCurrentROIlabel = itl.Peek0px1ny0pz();  
  }
  else if( itl.Peek0px0py1pz() > 0 )
  {
    uCurrentROIlabel = itl.Peek0px0py1pz(); 
  }
  else if( itl.Peek0px0py1nz() > 0 )
  {
    uCurrentROIlabel = itl.Peek0px0py1nz();  
  }

  /// check if the current ROI is a CAD ROI
  register int4 i, j=-1;
  const int4 iN = rDataset.m_suspiciousRegions.GetCount();

  if( uCurrentROIlabel > 0 )
  {
    for( i=0; i<iN; i++ )
    {
      if( rDataset.m_suspiciousRegions.GetFinding( i ).GetVolumeROI().GetLabelIndex() == uCurrentROIlabel )
      {
        j = i;
        break;
      } // it
    } // for i
  } // if
    
  if( j < 0 && uCurrentROIlabel > 0 )
  {
    AfxMessageBox("This is a CAD ROI and cannot be edited.");
    return;
  }
  else if( j < 0 )
  { // No matched ROI finding was found in nearby. Promptly return!
    return;
  } // else if j<0

  switch ( eMode )
  {
    case ROI_VOXEL_ADD:
    { /// add voxel to the nearest suspicious ROI region
      if( uCurrentROIlabel > 0 )
      {
        vxCadVoxelPosition pos = vxCadVoxelPosition(viSelectPosInVol.m_x, viSelectPosInVol.m_y, viSelectPosInVol.m_z);
        vxCadFinding & rFinding = rDataset.m_suspiciousRegions.GetFinding( j );

        if( itl.GetVoxel() == 0 )
        {
          itl.SetVoxel( uCurrentROIlabel );
          rFinding.GetVolumeROI().AddVoxel( pos );
        } // if

        if( GetSegmentROIVoxelSize() == 2 )
        { // this uses bigger brush size.
          AddVoxelIn2ndOrderNeighbors( rFinding, itl, uCurrentROIlabel, pos );
        } // if

        vxHistogramUpdaterLabel<uint2>::Update( *rDataset.m_fields.m_sliceRendererData.GetLabelVolume() );

        UpdateSuspiciousROI( j, uCurrentROIlabel );
      } // if( uCurrentROIlabel > 0 )

    } // case roi voxel add
    break;

    case ROI_VOXEL_DEL:
    { // delete voxel from the ROI
      if( uCurrentROIlabel > 0 ) 
      {
        vxCadVoxelPosition pos = vxCadVoxelPosition(viSelectPosInVol.m_x, viSelectPosInVol.m_y, viSelectPosInVol.m_z);
        vxCadFinding & rFinding = rDataset.m_suspiciousRegions.GetFinding( j );

        if( itl.GetVoxel() > 0 )
        {
          itl.SetVoxel( 0 );
          RemoveVoxelFromVolumeROI( rFinding, pos );
        } // if

        if( GetSegmentROIVoxelSize() == 2 )
        { // this uses bigger brush size.
          DeleteVoxelIn2ndOrderNeighbors( rFinding, itl, uCurrentROIlabel, pos );
        } // if

        vxHistogramUpdaterLabel<uint2>::Update( *rDataset.m_fields.m_sliceRendererData.GetLabelVolume() );

        if( rFinding.GetVolumeROI().GetCount() < 1 )
        { /// All voxels of this ROI have been deleted by user. The ROI needs to be removed
          AfxMessageBox("All voxels have been deleted.");
          RemoveSingleSROIFromList(uCurrentROIlabel, j);
        } // if

        UpdateSuspiciousROI( j, uCurrentROIlabel );
      } // if itl.GetVoxel() > 0
    } // case voxel delete
    break;

    case ROI_REGION_REM:
    { // remove the current ROI
      if( itl.GetVoxel() > 0 )
      { // get current component/ROI index label
        const Triple<int4> viDim = rDataset.m_fields.m_sliceRendererData.GetLabelVolume()->GetHeader().GetVolDim();
        const float4   fBoxWidth = 25.0F; // in millimeter

        int4 iN = rDataset.m_suspiciousRegions.GetCount();

        int4 jj = -1;
        for( int4 i=0; i<iN; i++ )
        {
          if( rDataset.m_suspiciousRegions.GetFinding( i ).GetVolumeROI().GetLabelIndex() == uCurrentROIlabel )
          {    
            RemoveSingleSROIFromList(uCurrentROIlabel, i);

            jj = i;
            break;
          } // if
        } // for i
      } // if
    } // remove the current ROI
    break;
  }
} // EditROIRegion( eMode, selectPoint )


/** Update the suspicious ROI in the label and paint volume
 *  after user editing.
 *
 *  @param iROIindex the index for the current Finding in the SRT list
 *  @param uROIlabel the label index for the current ROI in the label volume
 */
void SliceChildView::UpdateSuspiciousROI( const int4 & iROIindex, const uint2 & uROIlabel )
{
  Dataset& rDataset = GetDatasetRef();
  const int4 iN = rDataset.m_suspiciousRegions.GetCount();
  if( iN < 1 ||
      rDataset.m_suspiciousRegions.GetFinding( iROIindex ).GetVolumeROI().GetLabelIndex() != uROIlabel )
  {
    AfxMessageBox("No suspicious region exists or is nearby.");
    return;
  } // if

  const Triple<float4> vfUnits = rDataset.m_fields.m_sliceRendererData.GetLabelVolume()->GetHeader().GetVoxelUnits();
  const Vector3D<int4> viPos   = Vector3D<int4>( int4(rDataset.m_suspiciousRegions.GetFinding( iROIindex ).GetCentroidMM().GetVector().m_x/vfUnits.m_x+0.5F), 
                                   int4(rDataset.m_suspiciousRegions.GetFinding( iROIindex ).GetCentroidMM().GetVector().m_y/vfUnits.m_y+0.5F), 
                                   int4(rDataset.m_suspiciousRegions.GetFinding( iROIindex ).GetCentroidMM().GetVector().m_z/vfUnits.m_z+0.5F) );

  vxBlockVolumeIterator<uint2> itl(*rDataset.m_fields.m_sliceRendererData.GetLabelVolume());
  itl.SetPos(viPos);

  const Triple<int4> viDim = rDataset.m_fields.m_sliceRendererData.GetLabelVolume()->GetHeader().GetVolDim();
  const float4   fBoxWidth = 30.0F; // in millimeter

  Vector3D<float4> vfCtr(0.0F,0.0F,0.0F);
  Vector3D<int2>   viCtr(0,0,0);
  int4 ixl, ixu, iyl, iyu, izl, izu;

  /// determine which suspicious ROI is the current selected one
  vfCtr = rDataset.m_suspiciousRegions.GetFinding(iROIindex).GetCentroidMM().GetVector();
  viCtr = Vector3D<int2>( vfCtr.m_x/vfUnits.m_x+0.5F, vfCtr.m_y/vfUnits.m_y+0.5F, vfCtr.m_z/vfUnits.m_z+0.5F );
  
  ixl = max(viCtr.m_x-int4(fBoxWidth/vfUnits.m_x), 0);
  ixu = min(viCtr.m_x+int4(fBoxWidth/vfUnits.m_x), viDim.m_x-1);
  iyl = max(viCtr.m_y-int4(fBoxWidth/vfUnits.m_y), 0);
  iyu = min(viCtr.m_y+int4(fBoxWidth/vfUnits.m_y), viDim.m_y-1);
  izl = max(viCtr.m_z-int4(fBoxWidth/vfUnits.m_z), 0);
  izu = min(viCtr.m_z+int4(fBoxWidth/vfUnits.m_z), viDim.m_z-1);
        
  const Triple<int4> viDimPolyp = Triple<int4>(ixu-ixl, iyu-iyl, izu-izl);

  /// allocation memory for the temporary mask 
  vxBlockVolume<uint1> smallMask( vxVolumeHeader(viDimPolyp, vfUnits, 8*sizeof(uint1), 0) );
  vxBlockVolumeIterator<uint1> its( smallMask );

  const uint1 u01 = 0x01;
  register int4 x, y, z;
  float4 fVolume = 0.0F;
  for( z=izl, itl.SetPosZ(izl), its.SetPosZ(0); z<izu; z++, itl.IncZ(), its.IncZ() )
  {
    for( y=iyl, itl.SetPosY(iyl), its.SetPosY(0); y<iyu; y++, itl.IncY(), its.IncY() )
    {
      for( x=ixl, itl.SetPosX(ixl), its.SetPosX(0); x<ixu; x++, itl.IncX(), its.IncX() )
      {
        if( itl.GetVoxel() == uROIlabel )
        {
          its.SetVoxel(u01);
          fVolume = fVolume+1.0F;
        } // if
      } // for x
    } // for y
  } // for z

  if( fVolume > 0.0F )
  { /// This ROI needs to be updated.
    const uint2 u02 = u01+1;

    vxBlockVolumeIteratorNeighbors<uint1> it18( smallMask, Vector3D<int4>(1,1,1), vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors18 );

    /// dilating the ROI in one 18-neighbor layer
    for( its.SetPos(0,0,0); its.IsNotAtEnd(); its.NextBlockZYX() )
    {
      for( ; its.IsNotAtEndOfBlock(); its.NextZYXinsideBlock() )
      {
        if( its.GetVoxel() == u01 )
        {
				  for( it18.CenterAt(its.GetPos()); !it18.IsAtEnd(); ++it18 )
				  {
            if( it18.GetVoxel() == 0x00 && it18.IsInsideVolume() )
              it18.SetVoxel( u02 );
          } // for 18 neighbors
        } // if

      } // for inside the block
    } // for all blocks

    std::vector< Vector3D<int2> > vvList;

    vxBlockVolumeIterator<uint2>          itv  ( *rDataset.m_fields.m_sliceRendererData.GetCleansedVolume() );
    vxBlockVolumeIteratorNeighbors<uint2> it18v( *rDataset.m_fields.m_sliceRendererData.GetCleansedVolume(), 
                                       Vector3D<int4>(1,1,1), vxBlockVolumeIteratorNeighborsConsts::e3Dneighbors18 );

    const uint2 uTissueLow = vxIntensityConverter::ModalityToRaw(-500, rDataset.m_fields.m_sliceRendererData.GetCleansedVolume()->GetHeader());
    const int4  iS         = viDim.m_x*viDim.m_y;  // number of voxel in the slice

    /// get pointer to the paint volume
    uint1 * pPaintVol = rDataset.m_fields.m_paintVol.GetDataPtr();

    register int4 j, i = izl*iS;
    register bool bIsSurface = false;
    /// determine the surface point
    for( z=izl, itv.SetPosZ(izl), its.SetPosZ(0); z<izu; z++, itv.IncZ(), its.IncZ() )
    {
      j = i+iyl*viDim.m_x;

      for( y=iyl, itv.SetPosY(iyl), its.SetPosY(0); y<iyu; y++, itv.IncY(), its.IncY() )
      {
        for( x=ixl, itv.SetPosX(ixl), its.SetPosX(0); x<ixu; x++, itv.IncX(), its.IncX() )
        {
          if( pPaintVol[j+x] & SUSPICIOUSROI )
            pPaintVol[j+x] = 1;

          bIsSurface = false;

          if( its.GetVoxel() == u02 && itv.GetVoxel() <= uTissueLow )
          {
            bIsSurface = true;
          } // if

          //This block of code is not necessary. It appears too agressive on surface voxel! Dongqing 12-23-2008
          else if( its.GetVoxel() == u01 )
          {
			      for( it18v.CenterAt(itv.GetPos()); !it18v.IsAtEnd(); ++it18v )
			      {
              if( it18v.GetVoxel() <= uTissueLow )
              {
                vvList.push_back( Vector3D<int2>(x, y, z) );
                bIsSurface = true;
                break;
              } // if
            } // for neighbors
          } // else 

          if( bIsSurface )
          {
            pPaintVol[j+x] = SUSPICIOUSROI;
          } // if

        } // for x

        j = j+viDim.m_x;
      } // for y

      i = i+iS;
    } // for z

    if( vvList.size() < 1 )
    {
      AfxMessageBox("The updated ROI is far away from air lumen.\nPlease remove the ROI and re-segment the ROI.");
      return;
    }

    /// update ROI volume value
    fVolume = fVolume*vfUnits.m_x*vfUnits.m_y*vfUnits.m_z;
    rDataset.m_suspiciousRegions.GetFinding( iROIindex ).GetVolumeROI().SetVolumeValue( fVolume );

    register Vector3D<int2> vip;
    register Vector3D<float4> vfTmp;
    register float4 fDistance, fMaxDistance(0.0F);
    const float4 fxx = vfUnits.m_x*vfUnits.m_x;
    const float4 fyy = vfUnits.m_y*vfUnits.m_y;
    const float4 fzz = vfUnits.m_z*vfUnits.m_z;

    /// determine the maximum diameter
    for( i=vvList.size()-1; i>0; i-- )
    {
      for( j=i-1; j>=0; j-- )
      {
        vfTmp = Vector3D<float4>( vvList[i].m_x-vvList[j].m_x, vvList[i].m_y-vvList[j].m_y, vvList[i].m_z-vvList[j].m_z );
        fDistance = vfTmp.m_x*vfTmp.m_x*fxx + vfTmp.m_y*vfTmp.m_y*fyy + vfTmp.m_z*vfTmp.m_z*fzz;

        if( fDistance > fMaxDistance )
        {
          fMaxDistance = fDistance;
          x = i;  // record the end point index
          y = j;  // record the end point index
        } // if
      } // for j
    } // for i

    vxCadVector pt0(      0.0F,       0.0F,       0.0F );
    vxCadVector pt1( float4(vvList[x].m_x)*vfUnits.m_x, float4(vvList[x].m_y)*vfUnits.m_y, 
                     float4(vvList[x].m_z)*vfUnits.m_z );
    vxCadVector pt2( float4(vvList[y].m_x)*vfUnits.m_x, float4(vvList[y].m_y)*vfUnits.m_y, 
                     float4(vvList[y].m_z)*vfUnits.m_z );

    vxCadDimension largest(pt1, pt2);
    vxCadDimension depth  (pt0, pt0);
    vxCadDimension width  (pt0, pt0);

    vxCadDimensions regionDim(largest, width, depth);
    rDataset.m_suspiciousRegions.GetFinding( iROIindex ).SetDimensions(regionDim);
  } // if

  else
  {
    AfxMessageBox("No voxels exist in the ROI.");
  }
} // UpdateSuspiciousROI( iROIindex, uROIlabel )


/**
 * handles toggling mode on or off as necessary
 *
 * @Param eMode the segmentation mode enum
 */
void SliceChildView::ToggleROISegmentMode(const SEGMENTROIMODE eMode)
{
  const bool bRptWriteable = ( rdrGlobal.m_pCurrDataset->IsLoadingComplete() && ReportLayout::HasPermissions() && ReportLayout::CanModifyReport() );
  if ( !bRptWriteable || !rdrGlobal.GetSettings()->GetBoolValue(VCSettings::ViewerXML(), VCSettings::EnableSRT()) )
  {
    return;
  }

  if (!(GetViewModes() & ViewModes::SEGMENTPOLYP))
  {
    // if the ROI tool is off, turn it on
    GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_ENDOSCOPICVIEW_CURRENTMODE_SEGMENTREGION, 0), 0);

		/// When the 2D SRT tool menu enables, the 2D ROI color code should display. Otherwise, don't display 2D ROI color code
	  rdrGlobal.m_bDisplaySRTroi2D = true;
	  rdrGlobal.m_bDisplaySRTroi   = true;
  }
  else // (GetViewModes() == ViewModes::SEGMENTPOLYP)
  {
    // turn the tool off if the submode is already selected
    if (m_eROIEditMode == eMode )
    {
      GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(ID_ENDOSCOPICVIEW_CURRENTMODE_SEGMENTREGION, 0), 0);

			rdrGlobal.m_bDisplaySRTroi2D = false;
			rdrGlobal.m_bDisplaySRTroi   = false;
    }
  }

  m_eROIEditMode = (GetDatasetRef().m_suspiciousRegions.GetCount() > 0) ? eMode : ROI_REGION_NEW; // actually you only need set this for new eMode
  OnSetCursor(this, 0,0);

} // ToggleROISegmentMode( eMode )


/** convert volume coordinate to DICOM RCS world coordinate
 *
 *  @param viVolCoordinate
 *  @param vfWorldCoordinate the input RCS offset and the output RCS world coordinate
 *  @param viDim the dimension of the volume
 *  @param vfUnits the voxel units
 */
void SliceChildView::ConvertVol2DicomWorldCoordinate( const Vector3D<int4> & viVolCoordinate, Vector3D<float4> & vfWorldCoordinate,
       const Triple<int4> & viDim, const Triple<float4> & vfUnits, const std::string & sPatientOrientation )
{
  const Vector3D<float4> vfWorldRCSdicom = vfWorldCoordinate;

  vfWorldCoordinate.m_z = vfWorldRCSdicom.m_z + viVolCoordinate.m_z * vfUnits.m_z;

  if( sPatientOrientation == "HFS" )
  {
    vfWorldCoordinate.m_x = vfWorldRCSdicom.m_x + float4(viVolCoordinate.m_x) * vfUnits.m_x;
    vfWorldCoordinate.m_y = vfWorldRCSdicom.m_y + float4(viVolCoordinate.m_y) * vfUnits.m_y;
  }
  else if( sPatientOrientation == "FFS" )
  {
    vfWorldCoordinate.m_x = vfWorldRCSdicom.m_x - (float4(viDim.m_x)-viVolCoordinate.m_x) * vfUnits.m_x;
    vfWorldCoordinate.m_y = vfWorldRCSdicom.m_y + float4(viVolCoordinate.m_y) * vfUnits.m_y;
  }
  else if( sPatientOrientation == "HFP" )
  {
    vfWorldCoordinate.m_x = vfWorldRCSdicom.m_x - (float4(viDim.m_x)-viVolCoordinate.m_x) * vfUnits.m_x;
    vfWorldCoordinate.m_y = vfWorldRCSdicom.m_y - (float4(viDim.m_y)-viVolCoordinate.m_y) * vfUnits.m_y;
  }
  else if( sPatientOrientation == "FFP" )
  {
    vfWorldCoordinate.m_x = vfWorldRCSdicom.m_x + viVolCoordinate.m_x * vfUnits.m_x;
    vfWorldCoordinate.m_y = vfWorldRCSdicom.m_y - (float4(viDim.m_y)-viVolCoordinate.m_y) * vfUnits.m_y;
  }
  else if( sPatientOrientation == "HFDR" )
  {
    vfWorldCoordinate.m_x = vfWorldRCSdicom.m_x - (float4(viDim.m_x)-viVolCoordinate.m_x) * vfUnits.m_x;
    vfWorldCoordinate.m_y = vfWorldRCSdicom.m_y + viVolCoordinate.m_y * vfUnits.m_y;
  }
  else if( sPatientOrientation == "FFDR" )
  {
    vfWorldCoordinate.m_x = vfWorldRCSdicom.m_x - (float4(viDim.m_x)-viVolCoordinate.m_x) * vfUnits.m_x;
    vfWorldCoordinate.m_y = vfWorldRCSdicom.m_y - (float4(viDim.m_y)-viVolCoordinate.m_y) * vfUnits.m_y;
  }
  else if( sPatientOrientation == "HFDL" )
  {
    vfWorldCoordinate.m_x = vfWorldRCSdicom.m_x + viVolCoordinate.m_x * vfUnits.m_x;
    vfWorldCoordinate.m_y = vfWorldRCSdicom.m_y - (float4(viDim.m_y)-viVolCoordinate.m_y) * vfUnits.m_y;
  }
  else if( sPatientOrientation == "FFDL" )
  {
    vfWorldCoordinate.m_x = vfWorldRCSdicom.m_x + viVolCoordinate.m_x * vfUnits.m_x;
    vfWorldCoordinate.m_y = vfWorldRCSdicom.m_y + viVolCoordinate.m_y * vfUnits.m_y;
  }
  else
  {
    vfWorldCoordinate.m_x = vfWorldRCSdicom.m_x + viVolCoordinate.m_x * vfUnits.m_x;
    vfWorldCoordinate.m_y = vfWorldRCSdicom.m_y + viVolCoordinate.m_y * vfUnits.m_y;
  }
} // ConvertVol2DicomWorldCoordinate( viVolCoordinate, vfWorldCoordinate, viDim, vfUnits, sPatientOrientation )


/** 
 * Remove a suspicious ROI from the list 
 *
 * @param uLabelIndex is the label index for the ROI in label volume
 * @param iIndex is the ROI index in the suspicious ROI list
 */
void SliceChildView::RemoveSingleSROIFromList( const uint2 & uLabelIndex, const int4 & iIndex )
{
  Dataset& rDataset = GetDatasetRef();
  rDataset.m_suspiciousRegions.RemoveFinding(rDataset, iIndex); 

  // if, after removal, there are no more existing regions, set the region-edit mode to new region
  if ( rDataset.m_suspiciousRegions.GetCount() <= 0 )
  {
    m_eROIEditMode = ROI_REGION_NEW;
  }
} // RemoveSingleSROIFromList( uLabelIndex, iIndex )


/** add voxel to selected ROI with brush size of the 1st order neighborhood
 *
 *  @param rFinding the current finding
 *  @param itl the label volume iterator
 *  @param uROIlable the label index for the selected ROI
 *  @param pos the current volume coordinate for the mouse cursor
 */
void SliceChildView::AddVoxelIn2ndOrderNeighbors( vxCAD::vxCadFinding & rFinding, vxBlockVolumeIterator<uint2> & itl, 
                                 const uint2 & uROIlabel, vxCAD::vxCadVoxelPosition & pos )
{
  if( m_sliceViewer.GetOrientation() == SliceOrientations::eAXIAL )
  {
    if( itl.PeekRel(1,0,0) == 0 )
    {
      itl.IncX();
      itl.SetVoxel( uROIlabel );
      itl.DecX();
      pos.SetX( pos.GetX()+1 );
      rFinding.GetVolumeROI().AddVoxel( pos );
      pos.SetX( pos.GetX()-1 );
    }
    if( itl.PeekRel(-1,0,0) == 0 )
    {
      itl.DecX();
      itl.SetVoxel( uROIlabel );
      itl.IncX();
      pos.SetX( pos.GetX()-1 );
      rFinding.GetVolumeROI().AddVoxel( pos );
      pos.SetX( pos.GetX()+1 );
    }
    if( itl.PeekRel(0,1,0) == 0 )
    {
      itl.IncY();
      itl.SetVoxel( uROIlabel );
      itl.DecY();
      pos.SetY( pos.GetY()+1 );
      rFinding.GetVolumeROI().AddVoxel( pos );
      pos.SetY( pos.GetY()-1 );
    }
    if( itl.PeekRel(0,-1,0) == 0 )
    {
      itl.DecY();
      itl.SetVoxel( uROIlabel );
      itl.IncY();
      pos.SetY( pos.GetY()-1 );
      rFinding.GetVolumeROI().AddVoxel( pos );
      pos.SetY( pos.GetY()+1 );
    }
    if( itl.PeekRel(1,1,0) == 0 )
    {
      itl.IncX();
      itl.IncY();
      itl.SetVoxel( uROIlabel );
      itl.DecX();
      itl.DecY();
      pos.SetX( pos.GetX()+1 );
      pos.SetY( pos.GetY()+1 );
      rFinding.GetVolumeROI().AddVoxel( pos );
      pos.SetX( pos.GetX()-1 );
      pos.SetY( pos.GetY()-1 );
    }
    if( itl.PeekRel(1,-1,0) == 0 )
    {
      itl.IncX();
      itl.DecY();
      itl.SetVoxel( uROIlabel );
      itl.DecX();
      itl.IncY();
      pos.SetX( pos.GetX()+1 );
      pos.SetY( pos.GetY()-1 );
      rFinding.GetVolumeROI().AddVoxel( pos );
      pos.SetX( pos.GetX()-1 );
      pos.SetY( pos.GetY()+1 );
    }
    if( itl.PeekRel(-1,1,0) == 0 )
    {
      itl.DecX();
      itl.IncY();
      itl.SetVoxel( uROIlabel );
      itl.IncX();
      itl.DecY();
      pos.SetX( pos.GetX()-1 );
      pos.SetY( pos.GetY()+1 );
      rFinding.GetVolumeROI().AddVoxel( pos );
      pos.SetX( pos.GetX()+1 );
      pos.SetY( pos.GetY()-1 );
    }
    if( itl.PeekRel(-1,-1,0) == 0 )
    {
      itl.DecX();
      itl.DecY();
      itl.SetVoxel( uROIlabel );
      itl.IncX();
      itl.IncY();
      pos.SetX( pos.GetX()-1 );
      pos.SetY( pos.GetY()-1 );
      rFinding.GetVolumeROI().AddVoxel( pos );
      pos.SetX( pos.GetX()+1 );
      pos.SetY( pos.GetY()+1 );
    }
  }
  else if( m_sliceViewer.GetOrientation() == SliceOrientations::eSAGITTAL )
  {
    if( itl.PeekRel(0,0,1) == 0 )
    {
      itl.IncZ();
      itl.SetVoxel( uROIlabel );
      itl.DecZ();
      pos.SetZ( pos.GetZ()+1 );
      rFinding.GetVolumeROI().AddVoxel( pos );
      pos.SetZ( pos.GetZ()-1 );
    }
    if( itl.PeekRel(0,0,-1) == 0 )
    {
      itl.DecZ();
      itl.SetVoxel( uROIlabel );
      itl.IncZ();
      pos.SetZ( pos.GetZ()-1 );
      rFinding.GetVolumeROI().AddVoxel( pos );
      pos.SetZ( pos.GetZ()+1 );
    }
    if( itl.PeekRel(0,1,0) == 0 )
    {
      itl.IncY();
      itl.SetVoxel( uROIlabel );
      itl.DecY();
      pos.SetY( pos.GetY()+1 );
      rFinding.GetVolumeROI().AddVoxel( pos );
      pos.SetY( pos.GetY()-1 );
    }
    if( itl.PeekRel(0,-1,0) == 0 )
    {
      itl.DecY();
      itl.SetVoxel( uROIlabel );
      itl.IncY();
      pos.SetY( pos.GetY()-1 );
      rFinding.GetVolumeROI().AddVoxel( pos );
      pos.SetY( pos.GetY()+1 );
    }
    if( itl.PeekRel(0,1,1) == 0 )
    {
      itl.IncZ();
      itl.IncY();
      itl.SetVoxel( uROIlabel );
      itl.DecZ();
      itl.DecY();
      pos.SetZ( pos.GetZ()+1 );
      pos.SetY( pos.GetY()+1 );
      rFinding.GetVolumeROI().AddVoxel( pos );
      pos.SetZ( pos.GetZ()-1 );
      pos.SetY( pos.GetY()-1 );
    }
    if( itl.PeekRel(0,-1,1) == 0 )
    {
      itl.IncZ();
      itl.DecY();
      itl.SetVoxel( uROIlabel );
      itl.DecZ();
      itl.IncY();
      pos.SetZ( pos.GetZ()+1 );
      pos.SetY( pos.GetY()-1 );
      rFinding.GetVolumeROI().AddVoxel( pos );
      pos.SetZ( pos.GetZ()-1 );
      pos.SetY( pos.GetY()+1 );
    }
    if( itl.PeekRel(0,1,-1) == 0 )
    {
      itl.DecZ();
      itl.IncY();
      itl.SetVoxel( uROIlabel );
      itl.IncZ();
      itl.DecY();
      pos.SetZ( pos.GetZ()-1 );
      pos.SetY( pos.GetY()+1 );
      rFinding.GetVolumeROI().AddVoxel( pos );
      pos.SetZ( pos.GetZ()+1 );
      pos.SetY( pos.GetY()-1 );
    }
    if( itl.PeekRel(0,-1,-1) == 0 )
    {
      itl.DecZ();
      itl.DecY();
      itl.SetVoxel( uROIlabel );
      itl.IncZ();
      itl.IncY();
      pos.SetZ( pos.GetZ()-1 );
      pos.SetY( pos.GetY()-1 );
      rFinding.GetVolumeROI().AddVoxel( pos );
      pos.SetZ( pos.GetZ()+1 );
      pos.SetY( pos.GetY()+1 );
    }
  }
  else if( m_sliceViewer.GetOrientation() == SliceOrientations::eCORONAL )
  {
    if( itl.PeekRel(1,0,0) == 0 )
    {
      itl.IncX();
      itl.SetVoxel( uROIlabel );
      itl.DecX();
      pos.SetX( pos.GetX()+1 );
      rFinding.GetVolumeROI().AddVoxel( pos );
      pos.SetX( pos.GetX()-1 );
    }
    if( itl.PeekRel(-1,0,0) == 0 )
    {
      itl.DecX();
      itl.SetVoxel( uROIlabel );
      itl.IncX();
      pos.SetX( pos.GetX()-1 );
      rFinding.GetVolumeROI().AddVoxel( pos );
      pos.SetX( pos.GetX()+1 );
    }
    if( itl.PeekRel(0,0,1) == 0 )
    {
      itl.IncZ();
      itl.SetVoxel( uROIlabel );
      itl.DecZ();
      pos.SetZ( pos.GetZ()+1 );
      rFinding.GetVolumeROI().AddVoxel( pos );
      pos.SetZ( pos.GetZ()-1 );
    }
    if( itl.PeekRel(0,0,-1) == 0 )
    {
      itl.DecZ();
      itl.SetVoxel( uROIlabel );
      itl.IncZ();
      pos.SetZ( pos.GetZ()-1 );
      rFinding.GetVolumeROI().AddVoxel( pos );
      pos.SetZ( pos.GetZ()+1 );
    }
    if( itl.PeekRel(1,0,1) == 0 )
    {
      itl.IncX();
      itl.IncZ();
      itl.SetVoxel( uROIlabel );
      itl.DecX();
      itl.DecZ();
      pos.SetX( pos.GetX()+1 );
      pos.SetZ( pos.GetZ()+1 );
      rFinding.GetVolumeROI().AddVoxel( pos );
      pos.SetX( pos.GetX()-1 );
      pos.SetZ( pos.GetZ()-1 );
    }
    if( itl.PeekRel(1,0,-1) == 0 )
    {
      itl.IncX();
      itl.DecZ();
      itl.SetVoxel( uROIlabel );
      itl.DecX();
      itl.IncZ();
      pos.SetX( pos.GetX()+1 );
      pos.SetZ( pos.GetZ()-1 );
      rFinding.GetVolumeROI().AddVoxel( pos );
      pos.SetX( pos.GetX()-1 );
      pos.SetZ( pos.GetZ()+1 );
    }
    if( itl.PeekRel(-1,0,1) == 0 )
    {
      itl.DecX();
      itl.IncZ();
      itl.SetVoxel( uROIlabel );
      itl.IncX();
      itl.DecZ();
      pos.SetX( pos.GetX()-1 );
      pos.SetZ( pos.GetZ()+1 );
      rFinding.GetVolumeROI().AddVoxel( pos );
      pos.SetX( pos.GetX()+1 );
      pos.SetZ( pos.GetZ()-1 );
    }
    if( itl.PeekRel(-1,0,-1) == 0 )
    {
      itl.DecX();
      itl.DecZ();
      itl.SetVoxel( uROIlabel );
      itl.IncX();
      itl.IncZ();
      pos.SetX( pos.GetX()-1 );
      pos.SetZ( pos.GetZ()-1 );
      rFinding.GetVolumeROI().AddVoxel( pos );
      pos.SetX( pos.GetX()+1 );
      pos.SetZ( pos.GetZ()+1 );
    }
  }  // else if
  else /// this is for X-sectional and view-direction orientations
  {
    if( itl.PeekRel(1,0,0) == 0 )
    {
      itl.IncX();
      itl.SetVoxel( uROIlabel );
      itl.DecX();
      pos.SetX( pos.GetX()+1 );
      rFinding.GetVolumeROI().AddVoxel( pos );
      pos.SetX( pos.GetX()-1 );
    }
    if( itl.PeekRel(-1,0,0) == 0 )
    {
      itl.DecX();
      itl.SetVoxel( uROIlabel );
      itl.IncX();
      pos.SetX( pos.GetX()-1 );
      rFinding.GetVolumeROI().AddVoxel( pos );
      pos.SetX( pos.GetX()+1 );
    }
    if( itl.PeekRel(0,1,0) == 0 )
    {
      itl.IncY();
      itl.SetVoxel( uROIlabel );
      itl.DecY();
      pos.SetY( pos.GetY()+1 );
      rFinding.GetVolumeROI().AddVoxel( pos );
      pos.SetY( pos.GetY()-1 );
    }
    if( itl.PeekRel(0,-1,0) == 0 )
    {
      itl.DecY();
      itl.SetVoxel( uROIlabel );
      itl.IncY();
      pos.SetY( pos.GetY()-1 );
      rFinding.GetVolumeROI().AddVoxel( pos );
      pos.SetY( pos.GetY()+1 );
    }
    if( itl.PeekRel(0,0,1) == 0 )
    {
      itl.IncZ();
      itl.SetVoxel( uROIlabel );
      itl.DecZ();
      pos.SetZ( pos.GetZ()+1 );
      rFinding.GetVolumeROI().AddVoxel( pos );
      pos.SetZ( pos.GetZ()-1 );
    }
    if( itl.PeekRel(0,0,-1) == 0 )
    {
      itl.DecZ();
      itl.SetVoxel( uROIlabel );
      itl.IncZ();
      pos.SetZ( pos.GetZ()-1 );
      rFinding.GetVolumeROI().AddVoxel( pos );
      pos.SetZ( pos.GetZ()+1 );
    }
  } // else 
} // AddVoxelInNearestNeighbor( rFinding, itl, uROIlabel, pos )


/** remove a single voxel from the VolumeROI list of a CAD finding
 *
 *  @param rFinding the current CAD finding data
 *  @param pos the coordinates of the voxel that should be removed
 */
void SliceChildView::RemoveVoxelFromVolumeROI( vxCAD::vxCadFinding & rFinding, const vxCAD::vxCadVoxelPosition & pos )
{
  register int4 i;

  if( rFinding.GetVolumeROI().GetCount() > 0 )
  {
    for( i=rFinding.GetVolumeROI().GetCount()-1; i>=0; i-- )
    {
      if( rFinding.GetVolumeROI().GetVoxel(i).GetX() == pos.GetX() && 
          rFinding.GetVolumeROI().GetVoxel(i).GetY() == pos.GetY() && 
          rFinding.GetVolumeROI().GetVoxel(i).GetZ() == pos.GetZ() )
      {
        rFinding.GetVolumeROI().RemoveVoxel(i);
        break;
      } // if
    } // for i
  } // if
} // RemoveVoxelFromVolumeROI( rFinding, pos )


/** delete voxel to selected ROI with brush size of the 1st order neighborhood
 *
 *  @param rFinding the current finding
 *  @param itl the label volume iterator
 *  @param uROIlable the lable index for the selected ROI
 *  @param pos the current volume coordinate for the mouse cursor
 */
void SliceChildView::DeleteVoxelIn2ndOrderNeighbors( vxCAD::vxCadFinding & rFinding, vxBlockVolumeIterator<uint2> & itl, 
                                 const uint2 & uROIlabel, vxCAD::vxCadVoxelPosition & pos )
{
  if( m_sliceViewer.GetOrientation() == SliceOrientations::eAXIAL )
  {
    if( itl.PeekRel(1,0,0) == uROIlabel )
    {
      itl.IncX();
      itl.SetVoxel( 0 );
      itl.DecX();
      pos.SetX( pos.GetX()+1 );
      RemoveVoxelFromVolumeROI( rFinding, pos );
      pos.SetX( pos.GetX()-1 );
    } // if
    if( itl.PeekRel(-1,0,0) == uROIlabel )
    {
      itl.DecX();
      itl.SetVoxel( 0 );
      itl.IncX();
      pos.SetX( pos.GetX()-1 );
      RemoveVoxelFromVolumeROI( rFinding, pos );
      pos.SetX( pos.GetX()+1 );
    } // if
    if( itl.PeekRel(0,1,0) == uROIlabel )
    {
      itl.IncY();
      itl.SetVoxel( 0 );
      itl.DecY();
      pos.SetY( pos.GetY()+1 );
      RemoveVoxelFromVolumeROI( rFinding, pos );
      pos.SetY( pos.GetY()-1 );
    } // if
    if( itl.PeekRel(0,-1,0) == uROIlabel )
    {
      itl.DecY();
      itl.SetVoxel( 0 );
      itl.IncY();
      pos.SetY( pos.GetY()-1 );
      RemoveVoxelFromVolumeROI( rFinding, pos );
      pos.SetY( pos.GetY()+1 );
    } // if
    if( itl.PeekRel(1,1,0) == uROIlabel )
    {
      itl.IncX();
      itl.IncY();
      itl.SetVoxel( 0 );
      itl.DecX();
      itl.DecY();
      pos.SetX( pos.GetX()+1 );
      pos.SetY( pos.GetY()+1 );
      RemoveVoxelFromVolumeROI( rFinding, pos );
      pos.SetX( pos.GetX()-1 );
      pos.SetY( pos.GetY()-1 );
    } // if
    if( itl.PeekRel(1,-1,0) == uROIlabel )
    {
      itl.IncX();
      itl.DecY();
      itl.SetVoxel( 0 );
      itl.DecX();
      itl.IncY();
      pos.SetX( pos.GetX()+1 );
      pos.SetY( pos.GetY()-1 );
      RemoveVoxelFromVolumeROI( rFinding, pos );
      pos.SetX( pos.GetX()-1 );
      pos.SetY( pos.GetY()+1 );
    } // if
    if( itl.PeekRel(-1,1,0) == uROIlabel )
    {
      itl.DecX();
      itl.IncY();
      itl.SetVoxel( 0 );
      itl.IncX();
      itl.DecY();
      pos.SetX( pos.GetX()-1 );
      pos.SetY( pos.GetY()+1 );
      RemoveVoxelFromVolumeROI( rFinding, pos );
      pos.SetX( pos.GetX()+1 );
      pos.SetY( pos.GetY()-1 );
    } // if
    if( itl.PeekRel(-1,-1,0) == uROIlabel )
    {
      itl.DecX();
      itl.DecY();
      itl.SetVoxel( 0 );
      itl.IncX();
      itl.IncY();
      pos.SetX( pos.GetX()-1 );
      pos.SetY( pos.GetY()-1 );
      RemoveVoxelFromVolumeROI( rFinding, pos );
      pos.SetX( pos.GetX()+1 );
      pos.SetY( pos.GetY()+1 );
    } // if
  } // if eAxial
  else if( m_sliceViewer.GetOrientation() == SliceOrientations::eSAGITTAL )
  {
    if( itl.PeekRel(0,0,1) == uROIlabel )
    {
      itl.IncZ();
      itl.SetVoxel( 0 );
      itl.DecZ();
      pos.SetZ( pos.GetZ()+1 );
      RemoveVoxelFromVolumeROI( rFinding, pos );
      pos.SetZ( pos.GetZ()-1 );
    } // if
    if( itl.PeekRel(0,0,-1) == uROIlabel )
    {
      itl.DecZ();
      itl.SetVoxel( 0 );
      itl.IncZ();
      pos.SetZ( pos.GetZ()-1 );
      RemoveVoxelFromVolumeROI( rFinding, pos );
      pos.SetZ( pos.GetZ()+1 );
    } // if
    if( itl.PeekRel(0,1,0) == uROIlabel )
    {
      itl.IncY();
      itl.SetVoxel( 0 );
      itl.DecY();
      pos.SetY( pos.GetY()+1 );
      RemoveVoxelFromVolumeROI( rFinding, pos );
      pos.SetY( pos.GetY()-1 );
    } // if
    if( itl.PeekRel(0,-1,0) == uROIlabel )
    {
      itl.DecY();
      itl.SetVoxel( 0 );
      itl.IncY();
      pos.SetY( pos.GetY()-1 );
      RemoveVoxelFromVolumeROI( rFinding, pos );
      pos.SetY( pos.GetY()+1 );
    } // if
    if( itl.PeekRel(0,1,1) == uROIlabel )
    {
      itl.IncZ();
      itl.IncY();
      itl.SetVoxel( 0 );
      itl.DecZ();
      itl.DecY();
      pos.SetZ( pos.GetZ()+1 );
      pos.SetY( pos.GetY()+1 );
      RemoveVoxelFromVolumeROI( rFinding, pos );
      pos.SetZ( pos.GetZ()-1 );
      pos.SetY( pos.GetY()-1 );
    } // if
    if( itl.PeekRel(0,-1,1) == uROIlabel )
    {
      itl.IncZ();
      itl.DecY();
      itl.SetVoxel( 0 );
      itl.DecZ();
      itl.IncY();
      pos.SetZ( pos.GetZ()+1 );
      pos.SetY( pos.GetY()-1 );
      RemoveVoxelFromVolumeROI( rFinding, pos );
      pos.SetZ( pos.GetZ()-1 );
      pos.SetY( pos.GetY()+1 );
    } // if
    if( itl.PeekRel(0,1,-1) == uROIlabel )
    {
      itl.DecZ();
      itl.IncY();
      itl.SetVoxel( 0 );
      itl.IncZ();
      itl.DecY();
      pos.SetZ( pos.GetZ()-1 );
      pos.SetY( pos.GetY()+1 );
      RemoveVoxelFromVolumeROI( rFinding, pos );
      pos.SetZ( pos.GetZ()+1 );
      pos.SetY( pos.GetY()-1 );
    } // if
    if( itl.PeekRel(0,-1,-1) == uROIlabel )
    {
      itl.DecZ();
      itl.DecY();
      itl.SetVoxel( 0 );
      itl.IncZ();
      itl.IncY();
      pos.SetZ( pos.GetZ()-1 );
      pos.SetY( pos.GetY()-1 );
      RemoveVoxelFromVolumeROI( rFinding, pos );
      pos.SetZ( pos.GetZ()+1 );
      pos.SetY( pos.GetY()+1 );
    } // if
  } // else if eSagittal
  else if( m_sliceViewer.GetOrientation() == SliceOrientations::eCORONAL )
  {
    if( itl.PeekRel(1,0,0) == uROIlabel )
    {
      itl.IncX();
      itl.SetVoxel( 0 );
      itl.DecX();
      pos.SetX( pos.GetX()+1 );
      RemoveVoxelFromVolumeROI( rFinding, pos );
      pos.SetX( pos.GetX()-1 );
    } // if
    if( itl.PeekRel(-1,0,0) == uROIlabel )
    {
      itl.DecX();
      itl.SetVoxel( 0 );
      itl.IncX();
      pos.SetX( pos.GetX()-1 );
      RemoveVoxelFromVolumeROI( rFinding, pos );
      pos.SetX( pos.GetX()+1 );
    } // if
    if( itl.PeekRel(0,0,1) == uROIlabel )
    {
      itl.IncZ();
      itl.SetVoxel( 0 );
      itl.DecZ();
      pos.SetZ( pos.GetZ()+1 );
      RemoveVoxelFromVolumeROI( rFinding, pos );
      pos.SetZ( pos.GetZ()-1 );
    } // if
    if( itl.PeekRel(0,0,-1) == uROIlabel )
    {
      itl.DecZ();
      itl.SetVoxel( 0 );
      itl.IncZ();
      pos.SetZ( pos.GetZ()-1 );
      RemoveVoxelFromVolumeROI( rFinding, pos );
      pos.SetZ( pos.GetZ()+1 );
    } // if
    if( itl.PeekRel(1,0,1) == uROIlabel )
    {
      itl.IncX();
      itl.IncZ();
      itl.SetVoxel( 0 );
      itl.DecX();
      itl.DecZ();
      pos.SetX( pos.GetX()+1 );
      pos.SetZ( pos.GetZ()+1 );
      RemoveVoxelFromVolumeROI( rFinding, pos );
      pos.SetX( pos.GetX()-1 );
      pos.SetZ( pos.GetZ()-1 );
    } // if
    if( itl.PeekRel(1,0,-1) == uROIlabel )
    {
      itl.IncX();
      itl.DecZ();
      itl.SetVoxel( 0 );
      itl.DecX();
      itl.IncZ();
      pos.SetX( pos.GetX()+1 );
      pos.SetZ( pos.GetZ()-1 );
      RemoveVoxelFromVolumeROI( rFinding, pos );
      pos.SetX( pos.GetX()-1 );
      pos.SetZ( pos.GetZ()+1 );
    } // if
    if( itl.PeekRel(-1,0,1) == uROIlabel )
    {
      itl.DecX();
      itl.IncZ();
      itl.SetVoxel( 0 );
      itl.IncX();
      itl.DecZ();
      pos.SetX( pos.GetX()-1 );
      pos.SetZ( pos.GetZ()+1 );
      RemoveVoxelFromVolumeROI( rFinding, pos );
      pos.SetX( pos.GetX()+1 );
      pos.SetZ( pos.GetZ()-1 );
    } // if
    if( itl.PeekRel(-1,0,-1) == uROIlabel )
    {
      itl.DecX();
      itl.DecZ();
      itl.SetVoxel( 0 );
      itl.IncX();
      itl.IncZ();
      pos.SetX( pos.GetX()-1 );
      pos.SetZ( pos.GetZ()-1 );
      RemoveVoxelFromVolumeROI( rFinding, pos );
      pos.SetX( pos.GetX()+1 );
      pos.SetZ( pos.GetZ()+1 );
    } // if
  } // else if coronal
  else // for orientations: eCROSS_SECTION and eVIEWDIR 
  {
    if( itl.PeekRel(1,0,0) == uROIlabel )
    {
      itl.IncX();
      itl.SetVoxel( 0 );
      itl.DecX();
      pos.SetX( pos.GetX()+1 );
      RemoveVoxelFromVolumeROI( rFinding, pos );
      pos.SetX( pos.GetX()-1 );
    } // if
    if( itl.PeekRel(-1,0,0) == uROIlabel )
    {
      itl.DecX();
      itl.SetVoxel( 0 );
      itl.IncX();
      pos.SetX( pos.GetX()-1 );
      RemoveVoxelFromVolumeROI( rFinding, pos );
      pos.SetX( pos.GetX()+1 );
    } // if
    if( itl.PeekRel(0,1,0) == uROIlabel )
    {
      itl.IncY();
      itl.SetVoxel( 0 );
      itl.DecY();
      pos.SetY( pos.GetY()+1 );
      RemoveVoxelFromVolumeROI( rFinding, pos );
      pos.SetY( pos.GetY()-1 );
    } // if
    if( itl.PeekRel(0,-1,0) == uROIlabel )
    {
      itl.DecY();
      itl.SetVoxel( 0 );
      itl.IncY();
      pos.SetY( pos.GetY()-1 );
      RemoveVoxelFromVolumeROI( rFinding, pos );
      pos.SetY( pos.GetY()+1 );
    } // if
    if( itl.PeekRel(0,0,1) == uROIlabel )
    {
      itl.IncZ();
      itl.SetVoxel( 0 );
      itl.DecZ();
      pos.SetZ( pos.GetZ()+1 );
      RemoveVoxelFromVolumeROI( rFinding, pos );
      pos.SetZ( pos.GetZ()-1 );
    } // if
    if( itl.PeekRel(0,0,-1) == uROIlabel )
    {
      itl.DecZ();
      itl.SetVoxel( 0 );
      itl.IncZ();
      pos.SetZ( pos.GetZ()-1 );
      RemoveVoxelFromVolumeROI( rFinding, pos );
      pos.SetZ( pos.GetZ()+1 );
    } // if
  } // else
  
} // DeleteVoxelIn2ndOrderNeighbors( rFinding, itl, uROIlabel, pos )


// undefines
#undef FILE_REVISION


// $Log: SliceChildView.Region.C,v $
// Revision 1.1.2.34  2009/08/24 14:13:47  dongqing
// code review
//
// Revision 1.1.2.33  2009/07/30 17:58:21  kchalupa
// Code Walkthrough
//
// Revision 1.1.2.32  2009/01/23 23:42:38  jmeade
// Issue 6224: proper deletion of single/all volume measurement
//
// Revision 1.1.2.31  2009/01/23 19:21:54  jmeade
// Issue 6224: proper deletion of single/all volume measurement
//
// Revision 1.1.2.30  2009/01/07 01:39:00  jmeade
// volume measure interface changes.
//
// Revision 1.1.2.29  2008/12/23 20:46:02  dongqing
// fix the bug that 3D view ROI did not refresh after 2D ROI editing
//
// Revision 1.1.2.28  2008/12/23 17:00:13  dongqing
// add bigger brush size option for the SRT ROI region editing tool
//
// Revision 1.1.2.27  2008/07/02 19:52:50  dongqing
// add VCSetting.xml file EnableSRT status check for SRT hot key
// activation.
//
// Revision 1.1.2.26  2008/06/25 19:26:08  dongqing
// make the display SRT ROI mode right back after hitting SRAT hot keys
//
// Revision 1.1.2.25  2008/05/28 22:11:40  dongqing
// comments out redundant code block
//
// Revision 1.1.2.24  2008/05/27 20:43:42  dongqing
// Fix the SRT new ROI getting the wrong label index bug
// The componoent mechanism works based on HistoryLabelList
//
// Revision 1.1.2.23  2008/04/28 20:39:00  dongqing
// fix the incorrect transfer matrix for RCS in DL/DR cases
//
// Revision 1.1.2.22  2008/04/02 21:11:39  jmeade
// Issue 5963: disable srt if report is locked.
//
// Revision 1.1.2.21  2008/04/02 18:50:39  dongqing
// fix world coordinate display bug again
//
// Revision 1.1.2.20  2008/03/06 20:18:18  dongqing
// change ! to . in AfxMessagBox
//
// Revision 1.1.2.19  2008/03/06 05:55:21  jmeade
// Issue 6035: set current dataset for the subsequent snapshot taken when adding an ROI.
//
// Revision 1.1.2.18  2008/03/05 19:45:01  dongqing
// change the wording for popup message box
//
// Revision 1.1.2.17  2008/03/05 17:53:02  dongqing
// change the display english terms
//
// Revision 1.1.2.16  2008/03/03 23:23:14  dongqing
// fix the bug of Remove CAD ROI as suspicious ROI
//
// Revision 1.1.2.15  2008/03/03 20:40:17  dongqing
// add function for record single point user seleced in the SROI list
//
// Revision 1.1.2.14  2008/03/03 17:21:41  dongqing
// fix the orientation bug in the ConvertVol2WorldCoordinate
//
// Revision 1.1.2.13  2008/03/03 15:32:33  dongqing
// fix typo in the ConvertVol2DicomWorldCoordinate
//
// Revision 1.1.2.12  2008/02/29 15:52:54  dongqing
// Create a new function for converting Vol Coordinate to Dicom RCS world
// coordinate
//
// Revision 1.1.2.11  2008/02/01 19:48:03  dongqing
// synchronize the bounding box in EditROI and UpdateROI
//
// Revision 1.1.2.10  2008/02/01 15:55:37  dongqing
// decomments
//
// Revision 1.1.2.9  2008/01/22 19:25:42  dongqing
// fix the bug in the UpdateSRI could not get ROI label correct
//
// Revision 1.1.2.8  2008/01/17 19:35:31  dongqing
// complete the manual tool for suspicious ROI editing
//
// Revision 1.1.2.7  2008/01/16 01:07:15  jmeade
// spelling.
//
// Revision 1.1.2.6  2008/01/11 17:28:43  jmeade
// roi editing.
//
// Revision 1.1.2.5  2008/01/11 02:45:50  jmeade
// roi editing.
//
// Revision 1.1.2.4  2008/01/10 18:34:14  jmeade
// moving methods btw files.
//
// Revision 1.1.2.3  2008/01/10 18:30:14  jmeade
// moving methods btw files.
//
// Revision 1.1.2.2  2008/01/09 22:03:02  dongqing
// comments
//
// Revision 1.1.2.1  2008/01/09 20:54:31  jmeade
// more descriptive name for file.
//
// Revision 1.1.2.1  2008/01/09 17:03:09  dongqing
// move polyp segmentation functions to SliceChildView_Ext.C
//
// Revision 1.0  2008/01/07 20:42:08  dongqing
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/SliceChildView.Region.C,v 1.1.2.34 2009/08/24 14:13:47 dongqing Exp $
// $Id: SliceChildView.Region.C,v 1.1.2.34 2009/08/24 14:13:47 dongqing Exp $
