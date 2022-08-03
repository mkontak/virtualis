// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: CadUtils.cpp
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#include "StdAfx.h"
#include "CadUtils.h"
#include "Dataset.h"
#include "ReaderGlobal.h"
#include "ObservableObject.h"
#include "ViatronStudy.h"
#include "ViatronViewWnd.h"
#include "RenderUtil.h"


using namespace vxCAD;
using namespace ReaderLib;
USING_UTILITIES_NS


/**
 * Jump the viewpoint to a finding
 *
 * @param     dataset           dataset containing the cad findings
 * @param     sID               cad finding id
 */
void CAD::JumpTo( Dataset & dataset, const std::string & sID )
{
  if ( !rdrGlobal.m_bDisplayCadFindings || dataset.m_cadFindings.GetCount() == 0 )
    return;

  // search through primary dataset
  for( uint4 i(0); i < dataset.m_cadFindings.GetCount(); i++ )
  {
    if( dataset.m_cadFindings.GetFinding(i).GetId() == sID )
    {
      vxCadFinding & rFinding( dataset.m_cadFindings.GetFinding(i) );
      dataset.m_cadFindings.SetSelectedIndex( i );
      Helper::JumpTo( dataset, rFinding );
      
      dataset.m_cadFindings.Notify( vxCadFindingsArgs( vxCadFindingsArgs::CAD_ACTION_SELECTED, &dataset ));
      return;
    }
  }
} // JumpTo( dataset, sID )


/**
 * Jump the viewpoint to a finding
 *
 * @param     dataset           dataset containing the cad findings
 * @param     finding           cad finding
 */
void CAD::Helper::JumpTo( ReaderLib::Dataset & dataset, const vxCAD::vxCadFinding & finding )
{

  // Set current 3D position to the closest centerline point
  Point<float4> centroid( finding.GetCentroidMM().GetVector() );
  Point<float4> skelPt = dataset.GetNearestSkeletonPosition( finding );
  Normal<float> viewDir = Normal<float4>( centroid - skelPt );

  Normal<float4> testUp(1,0,0);
  float4 fDot = Vector<float4>(viewDir).Dot(testUp);
  // TODO test for -1 and 1 dot products (unlikely case)
  Vector<float4> upDir = Vector<float4>(testUp) - Vector<float4>(viewDir) * fDot;

  if( ViatronStudy::GetCurrentViewModes() == ViewModes::AUTOFLY )
  {
    ColonViews::Layouts::ColonViatronixWnd::ToggleAutoflyMode();
  }
  else if( ViatronStudy::GetCurrentViewModes() & ViewModes::AUTOFLY2D )
  {
    ViatronStudy::GetCurrentViewModes() -= ViewModes::AUTOFLY2D;
    AfxGetMainWnd()->PostMessage( WM_APP_UPDATE_LUMEN_COVERAGE_MARKING, 0, 0 );
  }

  dataset.SelectSegment( skelPt );
  dataset.m_submarine.SetPositionWorld( skelPt );
  dataset.m_submarine.SetUpDirection( Normal<float4>( upDir ));
  dataset.m_submarine.SetOrientation( viewDir);
  dataset.m_submarine.UpdatePosition();
  dataset.m_fields.m_volumeRender.FlushAccumulationBuffer();

  if( &dataset == &rdrGlobal.m_supine )
    rdrGlobal.UpdateWorldPosition( dataset, rdrGlobal.m_prone );
  else if( &dataset == &rdrGlobal.m_prone )
    rdrGlobal.UpdateWorldPosition( dataset, rdrGlobal.m_supine );

  // scroll the orthogonal slices to the centroid
  dataset.m_vOrthogonalPtWorld = centroid;
  dataset.m_vLastUpdatedOrthoPtWorld = dataset.m_submarine.GetPositionWorld();

  // update the cross-section sliding bar position
  dataset.m_iXsectionScrollBarLocationIndex = dataset.m_joinedSkeleton.ConvertToJoinedIndex( dataset.m_segmentList.GetCurrentSegment(), 
                                              dataset.m_fields.GetClosestSkeletonIndex( dataset.m_submarine.GetPositionVolume()) );


  AfxGetMainWnd()->PostMessage( WM_APP_GLCHILDVIEW_VIEWTYPE_INVALIDATE, WPARAM( GLChildView::eGVT_ALL ), 0 );
} // JumpTo( dataset, finding )


/**
 * Set display setting on a CAD finding
 *
 * @param     dataset           dataset containing the cad findings
 * @param     sID               ID of the finding to which to set
 * @param     eDecision         New visibility of finding
 */
void CAD::SetDisplayDecision( ReaderLib::Dataset & dataset, const std::string & sID, ColonSession::FindingInfo::DecisionEnum eDecision )
{
  if( !rdrGlobal.m_bDisplayCadFindings )
    return;

  dataset.m_session.m_mapCadFindingInfo[ sID ].SetDecision( eDecision );
  dataset.m_cadFindings.Notify( vxCadFindingsArgs( vxCadFindingsArgs::CAD_ACTION_UPDATED, &dataset ));
} // SetDisplayDecision( dataset, sID, eDecision )


/**
 * Selects the next cad finding
 *
 * @param     dataset           dataset containing the cad findings
 */
void CAD::NextFinding( ReaderLib::Dataset & dataset )
{
  const int4 iFindingCount( dataset.m_cadFindings.GetCount() );
  if( !rdrGlobal.m_bDisplayCadFindings || iFindingCount == 0 )
    return;

  int4 iSelectedIndex( dataset.m_cadFindings.GetSelectedIndex() );
  for( int4 i = 0; i < iFindingCount; ++i )
  {
    iSelectedIndex = ++iSelectedIndex % iFindingCount;
    if( Helper::JumpToVisibleFinding( dataset, iSelectedIndex ))
      break;
  }
} // NextFinding( dataset )


/**
 * Selects the previous cad finding
 *
 * @param     dataset           dataset containing the cad findings
 */
void CAD::PreviousFinding( ReaderLib::Dataset & dataset )
{
  const int4 iFindingCount( dataset.m_cadFindings.GetCount() );
  if( !rdrGlobal.m_bDisplayCadFindings || iFindingCount == 0 )
    return;

  int4 iSelectedIndex( dataset.m_cadFindings.GetSelectedIndex() );
  for( int4 i = 0; i < iFindingCount; ++i )
  {
    iSelectedIndex = ( iFindingCount + --iSelectedIndex ) % iFindingCount;
    if( Helper::JumpToVisibleFinding( dataset, iSelectedIndex ))
      break;
  }
} // PreviousFinding( dataset )


/**
 * Toggles the visibility for all cad findings
 */
void CAD::ToggleVisibility()
{
  rdrGlobal.m_bDisplayCadFindings = !rdrGlobal.m_bDisplayCadFindings;

  rdrGlobal.m_supine.m_cadFindings.Notify( vxCadFindingsArgs( vxCadFindingsArgs::CAD_ACTION_VISIBILITY, &rdrGlobal.m_supine ));
  rdrGlobal.m_prone.m_cadFindings.Notify( vxCadFindingsArgs( vxCadFindingsArgs::CAD_ACTION_VISIBILITY, &rdrGlobal.m_prone ));

  AfxGetMainWnd()->PostMessage( WM_APP_GLCHILDVIEW_VIEWTYPE_INVALIDATE, WPARAM( GLChildView::eGVT_ALL ), 0 );
} // ToggleVisibility()


/**
 * Calls for a refresh of all cad findings
 */
void CAD::Refresh()
{
  rdrGlobal.m_supine.m_cadFindings.Notify( vxCadFindingsArgs( vxCadFindingsArgs::CAD_ACTION_REFRESH, &rdrGlobal.m_supine ));
  rdrGlobal.m_prone.m_cadFindings.Notify( vxCadFindingsArgs( vxCadFindingsArgs::CAD_ACTION_REFRESH, &rdrGlobal.m_prone ));
} // Refresh()


/**
 * Jumps to the next visible cad finding
 *
 * @param       dataset               Dataset containing the cad findings
 * @param       index                 index of the finding
 * @return      true if a finding was found; otherwise false
 */
bool CAD::Helper::JumpToVisibleFinding( ReaderLib::Dataset & dataset, int4 index )
{
  vxCadFinding & finding( dataset.m_cadFindings.GetFinding( index ));
  if( RenderUtil::IsFindingVisible( finding, dataset.m_session.m_mapCadFindingInfo, dataset.m_cadFindings.m_iCadThresholdValue, rdrGlobal.m_bShowExternalCadFindings ))
  {
    if( dataset.m_session.m_mapCadFindingInfo[ finding.GetId() ].GetDecision() != ColonSession::FindingInfo::eDecisionDenied )
    {
      dataset.m_cadFindings.SetSelectedIndex( index );

      Helper::JumpTo( dataset, finding );
      dataset.m_cadFindings.Notify( vxCadFindingsArgs( vxCadFindingsArgs::CAD_ACTION_SELECTED, &dataset ));
      return true;
    }
  }

  return false;
} // JumpToVisibleFinding( dataset, index )