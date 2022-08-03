// $Id: vxManipulatorVesselDetailMPROblique.C,v 1.5 2007/07/30 19:23:25 romy Exp $
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
#include "vxManipulatorVesselDetailMPROblique.h"
#include "vxEnvironment.h"
#include "vxUtils.h"
#include "vxElementFloat4.h"
#include "timer.h"
#include "vxElementInt4.h"
#include "vxManipulatorPlaneOblique.h"
#include "vxVesselUtilities.h"
#include "vxVessel.h"

// Defines
#define FILE_REVISION "$Revision: 1.5 $"

bool vxManipulatorVesselDetailMPROblique::m_bZoomModified( false );
/**
 * default constructor
 *
 * @param environment the environment
 */
vxManipulatorVesselDetailMPROblique::vxManipulatorVesselDetailMPROblique(vxEnvironment & environment):
vxManipulator(environment)
{
  m_bDragging = false;
} // vxManipulatorVesselDetailMPROblique()


/**
 * Event handler if button is pressed down
 *
 * @param position       the mouse position
 * @param eButtonId      the mouse button that was pressed
 * @param uKeyboardState the keyboard state
 *
 * @return true if handled
 */ 
bool vxManipulatorVesselDetailMPROblique::OnButtonDown(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState)
{
  if ( eButtonId == BUTTON_MIDDLE )
  {
    GetEnvironment().SetUserInteracting( true );
    SetLastMousePosition( position );
    m_bDragging = true;
  } // if middle button

  return m_bDragging;
} // OnButtonDown()


/**
 * Event handler if mouse button is released
 *
 * @param position       the mouse position
 * @param eButtonId      the mouse button that was pressed
 * @param uKeyboardState the keyboard state
 *
 * @return true if handled
 */ 
bool vxManipulatorVesselDetailMPROblique::OnButtonUp(const Point2D<int2> & position, const ButtonEnum eButtonId, const uint4 uKeyboardState)
{

  bool bHandled( false );

  if( m_bDragging )
  {
    bHandled = OnMouseMove( position, uKeyboardState );
    GetEnvironment().SetUserInteracting( false );
  }

  m_bDragging = false;

  return bHandled;

} // OnButtonUp()


/**
 * Event handler if mouse is moved
 *
 * @param position       the screen position
 * @param uKeyboardState the keyboard state at the time of button press
 *
 * @return true if handled
 */ 
bool vxManipulatorVesselDetailMPROblique::OnMouseMove(const Point2D<int2> & position, const uint4 uKeyboardState)
{

  if ( m_bDragging == true )
  {

    int4 yDifference( position.m_y - GetLastMousePosition().m_y );

    // go faster with special keys
    yDifference *= GetKeyMultiplier(uKeyboardState);

    TranslateSection( yDifference );

    SetLastMousePosition( position );

  } // if dragging

  return m_bDragging;

} // OnMouseMove()


/**
 * Event handler if key is pressed
 *
 * @param uKeyId         the key that was pressed
 * @param uKeyboardState the keyboard state
 *
 * @return true if handled
 */ 
bool vxManipulatorVesselDetailMPROblique::OnKeyPressed(const uint1 uKeyId, const uint4 uKeyboardState)
{

  bool bHandled(false);

  switch ( uKeyId )
  {
  // PageUp key
  case 33:
    TranslateSection( GetKeyMultiplier( uKeyboardState ) );
    bHandled = true;
    break;

  // PageDown key
  case 34:
    TranslateSection( - int4( GetKeyMultiplier( uKeyboardState ) ) );
    bHandled = true;
    break;

  // Home key
  case 36:
    SetVesselSectionIndex( GetEnvironment(), 0 );
    bHandled = true;
    break;

  // End key
  case 35:
    TranslateSection( 100000 );
    bHandled = true;
    break;

  // +/- keys
  case vxInput::KEY_PLUS_KEYPAD:
  case vxInput::KEY_EQUALS:
  case vxInput::KEY_MINUS_KEYPAD:
  case vxInput::KEY_MINUS:
    {
    // maybe we need a non linear mapping at some point ...
    Triplef units(GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits());
    float fMin(Min(units[0], units[1], units[2]));
    float fFactor(10);
    const bool bIncrement( uKeyId == vxInput::KEY_PLUS_KEYPAD || uKeyId == vxInput::KEY_EQUALS );
    fFactor *= ( bIncrement ? 1 : -1 );
    fFactor *= fMin;
    vxManipulatorPlaneOblique::ZoomView( GetEnvironment(), int4( GetKeyMultiplier( uKeyboardState ) ) * fFactor );
    bHandled = true;
		m_bZoomModified = true;
    break;
    }

  case 'f':
  case 'F':
    {
      SaveAVIClip();
    }
    break;

  default:
    break;
  }

  return bHandled;

} // OnKeyPressed()


/**
 * Translate the section by a certain distance.
 *
 *  @param fDistance 
 */
void vxManipulatorVesselDetailMPROblique::TranslateSection( float4 fDistance )
{
  // get the current vessel
  vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( GetEnvironment().GetComponents() );
  if (pVessel == NULL)
  {
    //LogWrn( "Vessel pointer is NULL", "vxManipulatorVesselDetailMPROblique", "TranslateSection" );
    return;
  }

  // go in steps of 0.5mm or index of 0.25
  fDistance *= 0.25F;

  float4 fNewIndex( Bound( float4(0), float4( GetVesselSectionIndex( GetEnvironment() ) + fDistance ), float4( pVessel->size()-1 ) ) );

  SetVesselSectionIndex( GetEnvironment(), fNewIndex );

} // TranslateSection()


/**
 * Event handler if mouse wheel is scrolled
 *
 * @param uKeyId         the key that was pressed
 * @param iDelta         the amount the wheel was scrolled in either direction
 * @param uKeyboardState the keyboard state
 *
 * @return true if handled
 */
bool vxManipulatorVesselDetailMPROblique::OnMouseWheel(const Point2D<int2> & position, const int2 iDelta, const uint4 uKeyboardState)
{

  TranslateSection( float4( iDelta ) / float4( vxInput::GetMouseWheelQuantum() ) *
                    float4( GetKeyMultiplier(uKeyboardState) ) );

  return iDelta != 0;

} // OnMouseWheel()


/**
 * Get the current vessel section index
 *
 * @return the section index
 */
float4 vxManipulatorVesselDetailMPROblique::GetVesselSectionIndex( vxEnvironment & environment )
{

  // get the current index pointer
  vxShareableObject< vxElementFloat4 > * pCurrentVesselSection = dynamic_cast<vxShareableObject< vxElementFloat4 > * >( environment.GetElement( vxIDs::CurrentVesselSection ));
  if( pCurrentVesselSection == NULL )
  {
    LogDbg( "no current vessel section: ", "vxManipulatorVesselLuminal", "GetVesselSectionIndex" );
    return 0;
  }

  return pCurrentVesselSection->GetValue();

} // GetVesselSectionIndex()


/**
 * Set the current section index
 * 
 * @param environment   the environment
 * @param fSectionIndex the new desired section index
 */
void vxManipulatorVesselDetailMPROblique::SetVesselSectionIndex( vxEnvironment & environment, const float4 fSectionIndex )
{
  try
  {
    // get the current index pointer
    vxShareableObject< vxElementFloat4 > * pCurrentVesselSection = dynamic_cast<vxShareableObject< vxElementFloat4 > * >( environment.GetElement( vxIDs::CurrentVesselSection ));
    if( pCurrentVesselSection == NULL )
    {
      LogWrn( "no current vessel section: ", "vxManipulatorVesselLuminal", "SetVesselSectionIndex" );
      return;
    }

    // set the current vessel section index
    pCurrentVesselSection->SetValue( fSectionIndex );
    pCurrentVesselSection->Modified( vxModInfoID( vxElement, vxIDs::CurrentVesselSection, 0 ) );

    // get the current vessel
    vxVessel * pVessel = vxVesselUtilities::GetCurrentVessel( environment.GetComponents() );
    if ( pVessel == NULL )
    {
      //LogDbg( "Vessel pointer is NULL", "vxManipulatorVesselDetailMPROblique", "SetVesselSectionIndex" );
      return;
    }

    if ( fSectionIndex < 0.0F || fSectionIndex > pVessel->size()-1 )
      return;

    Point<float4> position( pVessel->GetResampledPosition( fSectionIndex ) );
    Normal<float4> tangent( pVessel->GetResampledTangent( fSectionIndex ) );
    Normal<float4> upDirection( pVessel->GetResampledUpDirection( fSectionIndex ) );
    Normal<float4> leftDirection( tangent.Cross( upDirection ) );
    Normal<float4> vYAxis(0.0f,-1.0f,0.0f);
    float4 fYOnUp( vYAxis.Dot(upDirection) );
    float4 fYOnLeft( vYAxis.Dot(leftDirection) );
    Normal<float4> vUpdatedUpDirection( fYOnUp*upDirection + fYOnLeft*leftDirection );

  	Normal<float4> vZAxis( 0.0F, 0.0F, 1.0F );
  	Normal<float4> vView( tangent.Dot(vZAxis) < 0.0F ? -tangent : tangent );
  
    vxShareableObject< vxCamera> * pCamera = environment.GetCamera();
    vxShareableObject< vxPlaneOblique<float4> > * pPlaneOblique = environment.GetPlaneOblique();
    vxVesselUtilities::RelocateNewObliqueView( pCamera, pPlaneOblique, position, vView, vUpdatedUpDirection );

  }
  catch ( ex::VException & e )
  {
    throw ex::VException( LogRec( "Caught an exception: " + e.GetLogRecord().GetTextMessage(), "vxManipulatorVesselDetailMPROblique", "SetVesselSectionIndex" ) );
  }
  catch (...)
  {
    throw ex::VException( LogRec( "Caught an unhandled exception", "vxManipulatorVesselDetailMPROblique", "SetVesselSectionIndex" ) );
  }


} // SetVesselSectionIndex()


/**
 * called when the mouse enters the view
 */
void vxManipulatorVesselDetailMPROblique::OnMouseEnter()
{

} // OnMouseEnter()

/**
 * Indicates whether the Zooming was modified or not
 */
bool vxManipulatorVesselDetailMPROblique::IsZoomFactorModified()
{
	return m_bZoomModified;
}// IsZoomFactorModified()


/**
 * save the avi clip along the centerline
 */
void vxManipulatorVesselDetailMPROblique::SaveAVIClip()
{
  //std::string srcVolFullFileName, srcVolRootFileName, centerlineFile;
  //srcVolFullFileName = GetEnvironment().GetIntensityVolume()->GetHeader().GetFileName();   
  //srcVolRootFileName = srcVolFullFileName.substr(0, srcVolFullFileName.length()-6);

  //Timer timer;

  //vxShareableObject< vxVesselArray > * pVesselArray = dynamic_cast< vxShareableObject< vxVesselArray > * >(GetEnvironment().GetElement( vxIDs::VesselArray ));
  //if( pVesselArray == NULL )
  //  throw ex::NullPointerException( LogRec( "Vessel array pointer is NULL", "vxManipulatorVesselCreate", "OnKeyPressed" ) );

  //vxVessel * pCurrVessel = pVesselArray->GetCurrentVessel();
  //if ( pCurrVessel == NULL )
  //{
  //  LogFyi( " no vessel is selected.", "vxManipulatorVesselCreate", "UpdateVesselContour");
  //  return;    
  //}

  //timer.Reset();
 
  //bool res = pCurrVessel->Save2AVIClip( srcVolRootFileName+pCurrVessel->GetName()+".avi", GetEnvironment() );

  //if ( res )
  //{
  //  LogFyi( timer.AsString() + " --- avi clip is finished.", "vxManipulatorVesselCreate", "OnKeyPressed");
  //}
  //else
  //{
  //  LogFyi( " failed to save the centerline avi clip", "vxManipulatorVesselCreate", "OnKeyPressed");
  //}

} // SaveAVIClip()


// undefines
#undef FILE_REVISION


// $Log: vxManipulatorVesselDetailMPROblique.C,v $
// Revision 1.5  2007/07/30 19:23:25  romy
// added a flag to indicate that the Zoom level was modified
//
// Revision 1.4  2007/01/10 20:06:17  geconomos
// Issue# 5276: zoom not working as expected
//
// Revision 1.3  2006/10/24 12:30:53  geconomos
// removed old slice shadows mechanism
//
// Revision 1.2  2006/10/03 20:10:36  geconomos
// setting the "user interacting" flag as needed
//
// Revision 1.1  2006/09/25 13:17:05  geconomos
// moved from vascular
//
// Revision 1.5  2006/08/17 15:52:30  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.4  2005/06/08 15:00:29  vxconfig
// formatting
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
// Revision 1.2  2004/04/06 16:31:40  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.20  2004/03/02 01:10:32  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.19.2.1  2004/02/25 14:59:26  frank
// formatting
//
// Revision 1.19  2004/01/14 17:14:55  frank
// removed old code
//
// Revision 1.18  2004/01/07 18:26:21  frank
// removed old warnings
//
// Revision 1.17  2003/12/23 14:40:19  frank
// fixed warning messages
//
// Revision 1.16  2003/12/22 21:35:50  wenli
// obliqueCut is viewed from the Inferior direction.
//
// Revision 1.15  2003/12/22 20:01:26  frank
// updated slice shadow data
//
// Revision 1.14  2003/12/11 20:08:18  frank
// Issue #3569: Showed the annotation on the oblique view when you click on them in the pane
//
// Revision 1.13  2003/10/22 20:38:03  frank
// Issue #3448: Fixed so that the 3D view does not re-render unnecessarily when you enter the window
//
// Revision 1.12  2003/09/25 03:08:03  frank
// Merged oblique views into one
//
// Revision 1.11  2003/08/12 20:20:24  frank
// Moved the vessel data structure and tools to vxBase
//
// Revision 1.10  2003/07/21 18:38:48  frank
// Issue #3351: Fixed zooming in/out
//
// Revision 1.9  2003/06/30 15:23:35  wenli
// #  3279  : Oblique view is oriented like the axial with the spine at the bottom
//
// Revision 1.8  2003/06/09 13:37:24  frank
// Added individual slice shadow renderer
//
// Revision 1.7  2003/06/03 19:33:45  frank
// code cleanup
//
// Revision 1.6  2003/06/02 18:16:56  frank
// Overrode zoom in vxBase with +/- key handling here.  Workaround for problem of extended element camera modification initiating 3D re-render.
//
// Revision 1.5  2003/05/23 15:47:13  frank
// parameter comments added
//
// Revision 1.4  2003/05/21 18:27:40  frank
// fixed oblique view
//
// Revision 1.3  2003/05/19 14:24:25  frank
// working on oblique view
//
// Revision 1.2  2003/05/12 20:06:53  frank
// Moved to alternate oblique viewer
//
// Revision 1.1  2003/05/08 19:59:26  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.17  2003/05/01 18:49:45  dongqing
// code review
//
// Revision 1.16  2003/04/30 20:05:37  frank
// typo
//
// Revision 1.15  2003/04/30 17:34:12  frank
// code review
//
// Revision 1.14  2003/04/30 16:56:32  frank
// code reviews
//
// Revision 1.13  2003/04/09 14:00:28  frank
// Fixed crash if there is no vessel data
//
// Revision 1.12  2003/03/13 14:13:33  frank
// Enabled sub-millimeter precision vessel centerline measurement, oblique plane location, and endoluminal flight position.
//
// Revision 1.11  2003/02/18 21:51:29  frank
// Error handling for empty vessels.
//
// Revision 1.10  2003/02/18 17:58:49  frank
// Cleaned up debug messages
//
// Revision 1.9  2003/02/14 22:34:25  frank
// fixed problem wth unsigned math
//
// Revision 1.8  2003/02/14 19:24:48  frank
// removed dead code
//
// Revision 1.7  2003/02/13 17:16:55  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.6  2003/02/12 17:46:26  frank
// Added Home/End/PageUp/PageDown navigation
//
// Revision 1.5  2003/02/06 23:44:49  frank
// bounds checking
//
// Revision 1.4  2003/02/05 23:19:55  frank
// Fixed mouse wheel on oblique.
//
// Revision 1.3.2.1  2003/02/12 23:10:18  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.3  2003/02/03 19:57:16  frank
// Fixed mouse wheel translation
//
// Revision 1.2  2003/01/22 22:17:56  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.1  2002/12/18 18:24:21  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorVesselDetailMPROblique.C,v 1.5 2007/07/30 19:23:25 romy Exp $
// $Id: vxManipulatorVesselDetailMPROblique.C,v 1.5 2007/07/30 19:23:25 romy Exp $
