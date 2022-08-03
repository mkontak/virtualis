// $Id: vxManipulatorVesselLuminal.C,v 1.7 2007/08/23 18:47:05 romy Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)


// includes
#include "StdAfx.h"
#include "vxManipulatorVesselLuminal.h"
#include "vxIDs.h"
#include "vxRibbon.h"
#include "vxElementInt4.h"
#include "vxInput.h"
#include "vxElementFloat4.h"
#include "vxVessel.h"
#include "vxEnvironment.h"
#include "vxVesselUtilities.h"
#include "vxViewerTypes.h"


// pragmas
#pragma unmanaged


// defines
#define FILE_REVISION "$Revision: 1.7 $"


// static member declarations
bool vxManipulatorVesselLuminal::m_bDraggingForward( true );


/**
 * default constructor
 *
 * @param environment the environment
 */
vxManipulatorVesselLuminal::vxManipulatorVesselLuminal( vxEnvironment & environment )
: vxManipulatorVessel( environment )
{
  m_bDragging = false;
  m_bDraggingForward = true;
} // vxManipulatorVesselLuminal()


/**
 * Destructor
 */
vxManipulatorVesselLuminal::~vxManipulatorVesselLuminal()
{
  GetEnvironment().GetRenderingMode()->SetEnabled( vxRenderingMode::SHOW_CROP_BOX, false );
} // ~vxManipulatorVesselLuminal()


/**
 * Event handler if mouse button is pressed down
 *
 * @param position       the screen position
 * @param eButtonId      the button that was pressed
 * @param uKeyboardState the state of the keyboard
 *
 * @return true if handled
 */
bool vxManipulatorVesselLuminal::OnButtonDown( const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState )
{

  try
  {
    // only respond to left mouse button
    if ( eButtonId == BUTTON_LEFT )
    {
      GetEnvironment().SetUserInteracting( true );

      // get the current vessel
      vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
      if (pVessel == NULL)
        return false;

      // get the ribbon
      vxShareableObject< vxRibbon > * pRibbon = dynamic_cast<vxShareableObject< vxRibbon > * >( GetEnvironment().GetElement( vxIDs::Ribbon ) );
      if ( pRibbon == NULL )
      {
        LogDbg( "Ribbon pointer is NULL", "vxManipulatorVesselLuminal", "OnButtonDown" );
        return false;
      }

      // see if we have selected a valid vessel index
      vxManipulatorVessel::SelectedSection::SelectionEnum eSelection = GetNearbySelection( position );
      if ( eSelection != vxManipulatorVessel::SelectedSection::None )
      {
        m_selection.SetSelection( eSelection );
        if ( eSelection == vxManipulatorVessel::SelectedSection::End )
        {
          pVessel->SetStenosisValid( true );
        }
      }

      // check if using the ALT key for brute-force specification of start and end
      const float4 fVesselIndex( vxVesselUtilities::GetVesselIndex( position.m_y, pRibbon, GetEnvironment().GetWinRect().GetViewportHeight() ) );
      const bool bValidVesselIndex( fVesselIndex >= 0.0F && fVesselIndex <= pVessel->size() - 1 );
      if ( bValidVesselIndex && ( uKeyboardState & vxInput::STATE_ALT ) )
      {
        // ALT key lets you quickly set normal state
        pVessel->SetMeasurementDomain( vxVessel::MeasurePair( fVesselIndex, fVesselIndex ) );
        pVessel->SetStenosisValid( true );
        vxShareableObject< vxElementFloat4 > * pCurrentVesselSection = dynamic_cast< vxShareableObject< vxElementFloat4 > * >( GetEnvironment().GetElement( vxIDs::CurrentVesselSection ) );
        pCurrentVesselSection->Modified( vxModInfoID( vxElement, vxIDs::CurrentVesselSection, 0 ) );
        //GetEnvironment().GetPlaneOrtho()->Modified( vxModInfo( vxPlaneOrtho<float4>, 0 ) );
      } // if ALT key

      // update the current position in other views
      SetNavigationPars( GetEnvironment(), position );
      m_bDragging = true;

    } // if left mouse button

    SetMousePosition( position );
    OnSetCursor();
    return m_bDragging;
  }
  catch ( ... )
  {
    throw ex::VException( LogRec( "Unhandled exception", "vxManipulatorVesselLuminal", "OnButtonDown" ) );
  }

} // OnButtonDown()


/**
 * Event handler if key is pressed
 *
 * @param uKeyId         the key that was pressed
 * @param uKeyboardState the keyboard state
 *
 * @return true if handled
 */ 
bool vxManipulatorVesselLuminal::OnKeyPressed( const uint1 uKeyId, const uint4 uKeyboardState )
{

  try
  {
    bool bHandled( false );

    switch ( uKeyId )
    {
    case vxInput::KEY_A:
      GetEnvironment().GetViewerType()->InitializeView();
      break;

    default:
      break;
    }

    return bHandled;
  }
  catch ( ... )
  {
    throw ex::VException( LogRec( "Unhandled exception", "vxManipulatorVesselLuminal", "OnKeyPressed" ) );
  }
  
} // OnKeyPressed


/**
 * Check if any selection lines are nearby and if so, which is closer
 *
 * @param position the mouse position
 *
 * @return the nearby selection
 */
vxManipulatorVessel::SelectedSection::SelectionEnum vxManipulatorVesselLuminal::GetNearbySelection( const Point2D<int2> & position ) const
{
  // get the current vessel
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if ( pVessel == NULL )
  {
    //LogWrn( "Vessel is NULL", "vxManipulatorVesselLuminal", "GetNearbySelection" );
    return vxManipulatorVessel::SelectedSection::None;
  }

  // get the ribbon
  vxShareableObject< vxRibbon > * pRibbon = dynamic_cast<vxShareableObject< vxRibbon > * >( GetEnvironment().GetElement( vxIDs::Ribbon ) );
  if ( pRibbon == NULL )
  {
    LogWrn( "Ribbon pointer is NULL", "vxManipulatorVesselLuminal", "GetNearbySelection" );
    return vxManipulatorVessel::SelectedSection::None;
  }

  // check to see if we are over an endpoint
  const float4 fScanlineImageStart( vxVesselUtilities::GetScanlinePosition( pVessel->GetMeasurementDomain().first , pRibbon ) );
  const float4 fScanlineImageEnd  ( vxVesselUtilities::GetScanlinePosition( pVessel->GetMeasurementDomain().second, pRibbon ) );
  const float4 fScanlineScreenStart( Interpolate( 0.0F, fScanlineImageStart, pRibbon->GetScanlineData().size()-1, 0.0F, GetEnvironment().GetWinRect().GetViewportHeight() - 1.0F ) );
  const float4 fScanlineScreenEnd  ( Interpolate( 0.0F, fScanlineImageEnd  , pRibbon->GetScanlineData().size()-1, 0.0F, GetEnvironment().GetWinRect().GetViewportHeight() - 1.0F ) );
  const float4 fDistanceStart( fabs( GetEnvironment().GetWinRect().GetViewportHeight() - 1 - position.m_y - fScanlineScreenStart ) );
  const float4 fDistanceEnd  ( fabs( GetEnvironment().GetWinRect().GetViewportHeight() - 1 - position.m_y - fScanlineScreenEnd   ) );
  const float4 fNearbyPixels( 5.0F );

  // pick the closest, nearby selection
  vxManipulatorVessel::SelectedSection::SelectionEnum eSelection = vxManipulatorVessel::SelectedSection::None;
  if ( fDistanceStart < fNearbyPixels && fDistanceStart < fDistanceEnd   ) eSelection = SelectedSection::Start;
  if ( fDistanceEnd   < fNearbyPixels && fDistanceEnd   < fDistanceStart ) eSelection = SelectedSection::End;

  return eSelection;

} // GetNearbySelection()


/** 
 * This event handler can be reimplemented in a subclass to receive set cursor events for the viewer.
 *
 * @return true if the cursor was set and nobody else should set theirs
 */
bool vxManipulatorVesselLuminal::OnSetCursor()
{
  try
  {
    // if editing a selection line, then show up-down arrow, otherwise leave it along
    if ( m_bDragging || GetNearbySelection( GetMousePosition() ) != vxManipulatorVessel::SelectedSection::None )
    {
      vxMouseCursor::SetCursor( "CURSOR_NS" );
      return true;
    } // if near line
    else
    {
      return false;
    } // not near line
  }
  catch ( ... )
  {
    throw ex::VException( LogRec( "Unhandled exception", "vxManipulatorVesselLuminal", "OnSetCursor" ) );
  }

} // OnSetCursor()


/**
 * Event handler if mouse button is released
 *
 * @param position       the screen position
 * @param eButtonId      the button that was pressed
 * @param uKeyboardState the state of the keyboard
 *
 * @return true if handled
 */ 
bool vxManipulatorVesselLuminal::OnButtonUp( const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState )
{
  try
  {
    OnMouseMove( position, uKeyboardState );
    m_bDragging = false;
    m_selection.SetSelection( SelectedSection::None );
    OnSetCursor();
    GetEnvironment().SetUserInteracting( false );

    return m_bDragging;
  }
  catch ( ... )
  {
    throw ex::VException( LogRec( "Unhandled exception", "vxManipulatorVesselLuminal", "OnButtonUp" ) );
  }
} // OnButtonUp()


/**
 * Event handler if mouse was moved
 *
 * @param position       the screen position
 * @param uKeyboardState the state of the keyboard
 *
 * @return true if handled
 */ 
bool vxManipulatorVesselLuminal::OnMouseMove( const Point2D<int2> & position, const uint4 uKeyboardState )
{
  try
  {
    // make sure we are dragging
    if ( m_bDragging == true )
    {
      // get the current vessel
      vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
      if( pVessel == NULL )
        return false;

      // get the ribbon
      vxShareableObject< vxRibbon > * pRibbon = dynamic_cast<vxShareableObject< vxRibbon > * >( GetEnvironment().GetElement( vxIDs::Ribbon ) );
      if ( pRibbon == NULL )
      {
        LogDbg( "Ribbon pointer is NULL", "vxManipulatorVesselLuminal", "OnMouseMove" );
        return false;
      }

      // see if we are over a valid section or if we have dragged it past either end
      const float4 fVesselIndex( vxVesselUtilities::GetVesselIndex( position.m_y, pRibbon, GetEnvironment().GetWinRect().GetViewportHeight() ) );
      const bool bValidVesselIndex( fVesselIndex >= 0.0F && fVesselIndex <= pVessel->size() - 1 );
      bool bUpdateMeasurement( false );
      if ( bValidVesselIndex )
      {
        // ALT-drag changes stenotic
        if ( uKeyboardState & vxInput::STATE_ALT )
        {
          pVessel->SetMeasurementDomain( vxVessel::MeasurePair( pVessel->GetMeasurementDomain().first, fVesselIndex ) );
          pVessel->SetStenosisValid( true );
          bUpdateMeasurement = true;
        }

        // dragging the start
        if ( m_selection.GetSelection() == SelectedSection::Start )
        {
          pVessel->SetMeasurementDomain( vxVessel::MeasurePair( fVesselIndex, pVessel->GetMeasurementDomain().second ) );
          bUpdateMeasurement = true;
        }

        // dragging the end
        if ( m_selection.GetSelection() == SelectedSection::End )
        {
          pVessel->SetMeasurementDomain( vxVessel::MeasurePair( pVessel->GetMeasurementDomain().first, fVesselIndex ) );
          bUpdateMeasurement = true;
        }
      }

      else if ( fVesselIndex == -1 )
      {
        // dragged past the start of the vessel - clamp to start
        
        if ( m_selection.GetSelection() == SelectedSection::Start )
        {
          // was dragging start point
          pVessel->SetMeasurementDomain( vxVessel::MeasurePair( 0, pVessel->GetMeasurementDomain().second ) );
          bUpdateMeasurement = true;
        }

        if ( m_selection.GetSelection() == SelectedSection::End )
        {
          // was dragging end point
          pVessel->SetMeasurementDomain( vxVessel::MeasurePair( pVessel->GetMeasurementDomain().first, 0 ) );
          pVessel->SetStenosisValid( true );
          bUpdateMeasurement = true;
        }
      } // if selected before start of vessel

      else if ( fVesselIndex == -2 )
      {
        // dragged past the end of the vessel - clamp to the end

        if ( m_selection.GetSelection() == SelectedSection::Start )
        {
          // was dragging the start point
          pVessel->SetMeasurementDomain( vxVessel::MeasurePair( pVessel->size()-1, pVessel->GetMeasurementDomain().second ) );
          bUpdateMeasurement = true;
        }

        if ( m_selection.GetSelection() == SelectedSection::End )
        {
          // was dragging the end point
          pVessel->SetMeasurementDomain( vxVessel::MeasurePair( pVessel->GetMeasurementDomain().first, pVessel->size()-1 ) );
          pVessel->SetStenosisValid( true );
          bUpdateMeasurement = true;
        }
      } // if selected past end of vessel

      // see if we have actually changed anything and need to update the GUI
      if ( bUpdateMeasurement )
      {
        vxShareableObject< vxElementFloat4 > * pCurrentVesselSection = dynamic_cast< vxShareableObject< vxElementFloat4 > * >( GetEnvironment().GetElement( vxIDs::CurrentVesselSection ) );
        pCurrentVesselSection->Modified( vxModInfoID( vxElement, vxIDs::CurrentVesselSection, 0 ) );
        //GetEnvironment().GetPlaneOrtho()->Modified( vxModInfo( vxPlaneOrtho<float4>, 0 ) );
      }

      // update the current position in other views
      SetNavigationPars( GetEnvironment(), position );

    } // if dragging

    SetMousePosition( position );
    OnSetCursor();
    return m_bDragging;
  }
  catch ( ... )
  {
    throw ex::VException( LogRec( "Unhandled exception", "vxManipulatorVesselLuminal", "OnMouseMove" ) );
  }

} // OnMouseMove()


/**
 * Synchronize all the other views to the position clicked on the screen
 *
 * @param position the screen position clicked
 */
void vxManipulatorVesselLuminal::SetNavigationPars( vxEnvironment & environment, const Point2D<int2> & position )
{
  try
  {

    // make sure there is a current vessel
    vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( environment.GetComponents() );
    if ( pVessel == NULL )
      return;

    
    // get the curved plane - to get the width
    vxPlaneCurved * pCurvedPlane = dynamic_cast<vxPlaneCurved *>(environment.GetPlaneCurved());
    if (pCurvedPlane == NULL)
    {
      LogDbg( "Curved Plane pointer is NULL", "vxManipulatorVesselLuminal", "SetNavigationPars" );
      return;
    }

    // get the ribbon
    vxShareableObject< vxRibbon > * pRibbon = dynamic_cast<vxShareableObject< vxRibbon > * >( environment.GetElement( vxIDs::Ribbon ) );
    if ( pRibbon == NULL )
    {
      LogDbg( "Ribbon pointer is NULL", "vxManipulatorVesselLuminal", "SetNavigationPars" );
      return;
    }

    // make sure the current section is valid
    const float4 fSectionIndex( vxVesselUtilities::GetVesselIndex( position.m_y, pRibbon, environment.GetWinRect().GetViewportHeight() ) );
    if ( fSectionIndex >= 0 && fSectionIndex <= pVessel->size()-1 )
    {
      vxManipulatorVessel::SetVesselSectionIndex( environment, fSectionIndex );

    } // if the current section is valid

  } // try
  catch ( ... )
  {
    throw ex::VException( LogRec( "Unhandled exception", "vxManipulatorVesselLuminal", "SetNavigationPars" ) );
  }

} // SetNavigationPars()


/**
 * Set the view parameters for the 3D detail view.
 *
 * @param cutBox the detail box of interest
 */
void vxManipulatorVesselLuminal::SetDetail3DView( vxEnvironment & environment, const Box< float4 > cutBox )
{

  //try
  //{
  //  // early return if we are not in this mode
  //  if ( !dynamic_cast< vxViewerTypeVesselDetail3D * >( environment.GetViewerType() ) )
  //    return;

  //  //// get the crop box
  //  //vxShareableObject< vxCropBox > * pCropBox = static_cast< vxShareableObject< vxCropBox > * > ( environment.GetElement( vxIDs::VesselDetailCropBox ) );
  //  //if ( pCropBox == NULL )
  //  //{
  //  //  LogDbg( "No crop box found", "vxManipulatorVesselLuminal", "SetDetail3DView" );
  //  //  return;
  //  //}

  //  //// set the crop box
  //  //pCropBox->SetBox( cutBox );
  //  //if ( environment.GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_CROP_BOX ) == false )
  //  //  environment.GetRenderingMode()->SetEnabled( vxRenderingMode::SHOW_CROP_BOX, true );
  //  //pCropBox->Modified( vxModInfo( vxCropBox, vxUtils::CROPBOX_MODIFIED ) );       

  //  //// get the box-of-interest
  //  //vxShareableObject< vxCropBox > * pInterestBox = static_cast< vxShareableObject< vxCropBox > * > ( environment.GetElement( vxIDs::BoxOfInterest ) );
  //  //if ( pInterestBox == NULL )
  //  //{
  //  //  LogDbg( "No interest box found", "vxManipulatorVesselLuminal", "SetDetail3DView" );
  //  //  return;
  //  //}

  //  //// set the box-of-interest
  //  //pInterestBox->SetBox( cutBox );
  //  //pInterestBox->Modified( vxModInfoID( vxElement, vxIDs::BoxOfInterest, vxUtils::CROPBOX_MODIFIED ) );

  //  // Set the camera
  //  vxShareableObject< vxCamera > * pDetailCamera = dynamic_cast< vxShareableObject< vxCamera > * >( environment.GetElement( vxIDs::VesselDetailCamera ) );
  //  Vector3Df viewDir = Vector3Df( pDetailCamera->GetViewDirection() );
  //  if ( viewDir.GetLength() == 0.0F )
  //    viewDir = Vector3Df( 1, 0, 0 );
  //  pDetailCamera->SetViewReference( cutBox.GetMidPoint() );
  //  pDetailCamera->SetEye( cutBox.GetMidPoint() - viewDir * 2.5 * cutBox.GetDimX() ); 
  //  vxUtils::CameraViewAll( * pDetailCamera, cutBox );
  //  pDetailCamera->Modified( vxModInfo( vxCamera,vxUtils::CAMERA_MODIFIED ) );

  //} // try
  //catch ( ... )
  //{
  //  throw ex::VException( LogRec( "Unhandled exception", "vxManipulatorVesselLuminal", "SetDetail3DView" ) );
  //}

} // SetDetail3DView()


/**
 * Set the endoluminal camera to the proper position and orientation
 *
 * @param fVesselSection the location along the centerline
 * @param bLookForward   look forward or backward
 */
void vxManipulatorVesselLuminal::SetEndoluminalCamera( vxEnvironment & environment, const float4 fVesselSection,
                                                       const bool bLookForward, const Vector3Df & positionTranslation )
{
  //// determine if we are even in detail 3D mode - check the shared variable
  ////vxElementInt4 * pDetailViewMode = dynamic_cast< vxElementInt4 * >( environment.GetElement( vxIDs::DetailViewMode ) );
  ////if ( pDetailViewMode == NULL ) return;

  //// get the endoluminal camera
  //vxShareableObject< vxCamera > * pCamera = dynamic_cast<vxShareableObject< vxCamera > * >( environment.GetElement( vxIDs::VesselDetailEndoluminalCamera ) );
  //if( pCamera == NULL )
  //{
  //  LogWrn( "no endoluminal camera", "vxManipulatorVesselLuminal", "SetEndoluminalCamera" );
  //  return;
  //}

  //// get the current vessel
  //vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( environment.GetComponents() );

  //// set the camera
  //const Point3Df position( pVessel->GetResampledPosition( fVesselSection ) + positionTranslation );
  //Normal3Df viewDirection( pVessel->GetResampledTangent( fVesselSection ) );
  //if ( ! bLookForward ) viewDirection = - viewDirection;
  //Normal3Df upDirection( pVessel->GetResampledUpDirection( fVesselSection ) );

  //environment.GetCursor3d()->SetPosition(position);
  //environment.GetCursor3d()->SetViewDirection(viewDirection);
  //environment.GetCursor3d()->Modified( vxModInfo( vxCursor3d, 0 ) );

  //pCamera->SetEye( position );
  //pCamera->SetViewDirection( viewDirection );
  //pCamera->SetViewUp( upDirection );
  
} // SetEndoluminalCamera()


/**
 * called when the mouse enters the view
 */
void vxManipulatorVesselLuminal::OnMouseEnter()
{
  
} // OnMouseEnter()


// undefines
#undef FILE_REVISION


// $Log: vxManipulatorVesselLuminal.C,v $
// Revision 1.7  2007/08/23 18:47:05  romy
// fixed the issue with slow Rendering of Vessel detail view in full screen. Unnecessary update of PLANE_MPR was the cause of performance
//
// Revision 1.6  2006/10/24 12:30:53  geconomos
// removed old slice shadows mechanism
//
// Revision 1.5  2006/09/25 20:11:39  geconomos
// moved initialization of 3d detail and endoluminal to vxControllerVesselDetail
//
// Revision 1.4  2006/09/25 13:22:06  geconomos
// + commented out unneeded  log debugging
// + removed "DetailViewMode" extended element
//
// Revision 1.3  2006/09/20 14:33:03  geconomos
// removed unused detail volume code in SetNavigatonPars()
//
// Revision 1.2  2006/09/14 20:13:40  geconomos
// + added "user interacting" flag support
// + moved initialization of oblique plane to controller
// + proper modification of camera
//
// Revision 1.1  2006/08/17 14:01:12  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.6  2006/06/05 15:07:46  frank
// updated for visual studio 2005
//
// Revision 1.5  2005/11/18 21:12:06  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.4.2.5  2005/09/19 19:18:53  frank
// Issue #4407: Showed arrow position in all detail modes
//
// Revision 1.4.2.4  2005/09/07 14:01:08  frank
// dead code removed
//
// Revision 1.4.2.3  2005/08/31 20:04:53  frank
// Issue #4407: Initialized the 3D camera arrow when new vessel is selected.
//
// Revision 1.4.2.2  2005/08/19 19:45:25  frank
// Issue #4358: Fixed problem with 3D detail on small vessels
//
// Revision 1.4.2.1  2005/07/13 18:25:21  frank
// Issue #4269: Fixed linking of 3D detail camera with curved MPR view
//
// Revision 1.4  2005/06/08 15:00:14  vxconfig
// added easy select object
//
// Revision 1.3  2005/06/07 18:10:42  vxconfig
// removed the vessel array
//
// Revision 1.2  2005/05/18 19:28:41  vxconfig
// slice shadows were moved into their own directory
//
// Revision 1.1  2004/08/09 19:29:34  frank
// first version in new V3D Viewer
//
// Revision 1.4  2004/04/26 19:04:42  michael
// Adjusted code to reflect the changes in zomm factor, roi not being part of
// camera anymore, etc.
//
// Revision 1.3  2004/04/06 16:31:40  frank
// unified environment access naming
//
// Revision 1.2  2004/03/12 16:10:10  frank
// Added auto-zoom on A key press
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.22  2004/03/02 01:10:32  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.21.2.2  2004/02/25 22:48:36  frank
// fixed problem with dragging the centerline handles
//
// Revision 1.21.2.1  2004/02/25 14:59:06  frank
// formatting
//
// Revision 1.21  2004/01/14 17:14:55  frank
// removed old code
//
// Revision 1.20  2003/12/23 14:39:34  frank
// fixed 3D overview slice shadows
//
// Revision 1.19  2003/12/22 21:36:07  wenli
// obliqueCut is viewed from the Inferior direction.
//
// Revision 1.18  2003/12/22 20:02:09  frank
// updated slice shadow data exchange
//
// Revision 1.17  2003/11/19 15:02:49  dongqing
// comments out the
// if ( pDetailViewMode->GetValue() != vxIDs::DetailModeEnum::Endoluminal )
//   return;
// This fixes the bug that the 3D Overview does not coorelated to the curve mouse movement.
//
// Revision 1.16  2003/11/04 18:31:49  frank
// Issue #3558: fixed endoluminal flight direction on mouse up
//
// Revision 1.15  2003/10/22 20:38:03  frank
// Issue #3448: Fixed so that the 3D view does not re-render unnecessarily when you enter the window
//
// Revision 1.14  2003/08/20 18:23:02  frank
// Flipped the arrow depending on the direction of movement.
//
// Revision 1.13  2003/08/19 12:27:08  frank
// Added an arrow at the current vessel section
//
// Revision 1.12  2003/06/30 15:00:37  wenli
// #  3279  : Oblique view is oriented like the axial with the spine at the bottom
//
// Revision 1.11  2003/06/18 15:19:45  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.10  2003/06/09 14:30:46  frank
// Prevented stenosis text from dislaying unless the stenosis line is manipulated
//
// Revision 1.9  2003/06/09 13:37:10  frank
// Added individual slice shadow renderers
//
// Revision 1.8  2003/06/05 14:23:36  frank
// Fixed axial view
//
// Revision 1.7  2003/05/29 14:04:58  frank
// Fixed debug messages
//
// Revision 1.6  2003/05/27 13:20:01  frank
// code review
//
// Revision 1.5  2003/05/20 17:40:36  frank
// code review
//
// Revision 1.4  2003/05/19 14:24:25  frank
// working on oblique view
//
// Revision 1.3  2003/05/16 19:36:08  frank
// Issue #3200: Show message if vascular dataset is not loaded
//
// Revision 1.2  2003/05/12 20:06:38  frank
// Moved to alternate oblique viewer
//
// Revision 1.1  2003/05/08 19:59:26  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.47  2003/05/02 18:11:44  frank
// Issue #3167: Added up/down arrow when dragging or near selection lines
//
// Revision 1.46  2003/05/01 15:10:38  frank
// Added function to relocate the axial plane
//
// Revision 1.45  2003/04/30 16:56:32  frank
// code reviews
//
// Revision 1.44  2003/04/30 15:50:09  frank
// code review
//
// Revision 1.43  2003/04/24 15:04:56  michael
// accounted for new Cursor3d handline (real class, not just triple)
//
// Revision 1.42  2003/04/24 14:03:02  michael
// accounted for new Cursor3d handline (real class, not just triple)
//
// Revision 1.41  2003/04/16 14:51:03  frank
// Separated out function for the new oblique plane
//
// Revision 1.40  2003/04/09 14:00:43  frank
// Cleaned up includes
//
// Revision 1.39  2003/03/24 15:46:47  frank
// Made the selection of handles in screen space rather than world space
//
// Revision 1.38  2003/03/24 14:29:33  frank
// Moved some common code to a base class
//
// Revision 1.37  2003/03/19 19:35:48  frank
// Fixed updating of overview when selecting on curved MPR.
//
// Revision 1.36  2003/03/19 13:37:59  frank
// Removed crash when no vessel is selected
//
// Revision 1.35  2003/03/18 21:38:48  frank
// removed debug
//
// Revision 1.34  2003/03/14 17:28:43  frank
// Refactored code
//
// Revision 1.33  2003/03/14 16:06:16  frank
// Organized code better
//
// Revision 1.32  2003/03/13 14:29:13  frank
// Enabled sub-millimeter precision vessel centerline measurement, oblique plane location, and endoluminal flight position.
//
// Revision 1.31  2003/03/07 20:41:12  geconomos
// Implementation of destructor move to .C file.
//
// Revision 1.30  2003/03/01 13:20:26  frank
// GetWinRect() was removed from vxManipulator
//
// Revision 1.29  2003/02/28 19:26:27  frank
// Fixed handle selection past the ends of the centerline
//
// Revision 1.28  2003/02/18 18:50:17  frank
// Cleaned up selection logic
//
// Revision 1.27  2003/02/18 18:00:03  frank
// Refactored code
//
// Revision 1.26  2003/02/18 15:26:40  frank
// removed smoothing
//
// Revision 1.25  2003/02/14 19:29:14  frank
// Fixed up initial curved zoom and made it possible to grab the endpoints of selection without using the ALT key
//
// Revision 1.24  2003/02/13 17:16:55  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.23  2003/02/12 16:35:54  frank
// Enabled selection of the cross-section directly.
//
// Revision 1.22  2003/02/11 21:48:30  frank
// Cleaned up vessel index selection
//
// Revision 1.21  2003/02/05 23:33:37  wenli
// update
//
// Revision 1.19.2.1  2003/02/12 23:10:18  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.19  2003/02/03 19:28:45  frank
// Fixed oblique slice relocating on mouse drag
//
// Revision 1.18  2003/02/03 16:15:51  frank
// moved box of interest again
//
// Revision 1.17  2003/01/28 15:08:44  frank
// Added vxRendererVesselCurved
//
// Revision 1.16  2003/01/24 18:32:27  frank
// synchronized the new oblique plane, well at least tried to...
//
// Revision 1.15  2003/01/22 22:17:44  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.14  2003/01/20 19:34:40  wenli
// New Version with updated centerline selection and Hessian Filter
//
// Revision 1.13  2002/12/17 00:08:33  frank
// Updated focus point during meausurement dragging.
//
// Revision 1.12  2002/11/27 21:53:17  frank
// trying to speed up 3D
//
// Revision 1.11  2002/11/27 21:41:29  wenli
// fix the size of the volumeDim-1
//
// Revision 1.10  2002/11/22 20:16:22  frank
// Replaced some exception throws with LogDbg
//
// Revision 1.9  2002/11/19 20:20:26  frank
// removed debug
//
// Revision 1.8  2002/11/18 16:55:41  michael
// made changes to account for changes to vx and v3D plane objects
//
// Revision 1.7  2002/11/15 02:08:46  frank
// Smoothed the vessel with the 'm' key
//
// Revision 1.6  2002/11/13 22:07:20  frank
// Renamed vxPlaneOblique to vxViewObliqueData,
// Demoted the data to a second class citizen in the environment.
//
// Revision 1.5  2002/11/13 20:11:55  frank
// enabled oblique view renderer and manipulator
//
// Revision 1.4  2002/11/12 16:14:10  frank
// Added synchronized oblique view with vessel section.
//
// Revision 1.3  2002/11/08 00:19:55  frank
// removed warning
//
// Revision 1.2  2002/11/07 22:57:51  frank
// removed toggle of component visibility
//
// Revision 1.1  2002/10/31 12:46:52  geconomos
// Moved from Viatronix.Visualization.Ext.Vascular
//
// Revision 1.18  2002/10/30 00:04:00  frank
// Changed interface.
//
// Revision 1.17  2002/10/29 18:28:07  frank
// went from std::pair to MeasurePair
//
// Revision 1.16  2002/10/29 16:51:39  frank
// revamped measurement
//
// Revision 1.15  2002/10/25 01:51:49  frank
// Moved working vessel into the VesselArray
//
// Revision 1.14  2002/10/24 17:32:18  frank
// formatting
//
// Revision 1.13  2002/10/23 23:07:15  frank
// cleaned up debug
//
// Revision 1.12  2002/10/23 22:56:03  frank
// oblique now using full volume
//
// Revision 1.11  2002/10/21 22:33:22  frank
// Working on linking up the oblique view
//
// Revision 1.10  2002/10/07 21:38:27  wenli
// view label in MPR
//
// Revision 1.9  2002/09/25 22:10:24  wenli
// Add BoxOfInterest
//
// Revision 1.8  2002/09/25 15:25:24  wenli
// v3D vessel component list
//
// Revision 1.7  2002/09/16 20:22:14  dmitry
// New vxID (s) used.
//
// Revision 1.6  2002/08/24 00:57:05  wenli
// initial version - Vascular
//
// Revision 1.5  2002/08/23 23:31:54  frank
// Cut and write to disk.
//
// Revision 1.4  2002/08/23 17:34:53  frank
// Working on middle button copy
//
// Revision 1.3  2002/08/22 17:18:17  frank
// working on live updates
//
// Revision 1.2  2002/08/22 03:34:33  frank
// working on modified
//
// Revision 1.1  2002/08/22 02:58:44  frank
// Intial version.
//
// Revision 1.8  2002/08/21 23:28:18  wenli
// initial version - Vascular
//
// Revision 1.7  2002/08/20 18:06:07  ana
// When creating the certerline, create the vxVessel file.
//
// Revision 1.6  2002/08/19 23:15:38  wenli
// initial version - Vascular
//
// Revision 1.5  2002/08/15 21:45:27  frank
// Wrote out vesselness volume on 'v' key.
//
// Revision 1.4  2002/08/15 18:20:36  wenli
// initial version - Vascular
//
// Revision 1.3  2002/08/14 22:46:01  wenli
// initial version - Vascular
//
// Revision 1.2  2002/08/14 16:07:41  wenli
// initial version - Vascular
//
// Revision 1.1  2002/08/12 15:52:58  wenli
// initial version - Vascular
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorVesselLuminal.C,v 1.7 2007/08/23 18:47:05 romy Exp $
// $Id: vxManipulatorVesselLuminal.C,v 1.7 2007/08/23 18:47:05 romy Exp $
