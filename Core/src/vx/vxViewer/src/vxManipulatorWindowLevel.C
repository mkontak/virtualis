// $Id: vxManipulatorWindowLevel.C,v 1.10 2006/08/03 14:01:35 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (michael@viatronix.com)


// includes
#include "stdafx.h"
#include "vxManipulatorWindowLevel.h"
#include "vxEnvironment.h"
#include "vxSerializer.h"
#include "vxViewerTypes.h"
#include "vxIntensityConverter.h"
#include "vxMathUtils.h"
#include "vxVisualizationPreset.h"
#include "ZeroOne.h"
#include "vxUndoActionPreset.h"

// defines
#define FILE_REVISION "$Revision: 1.10 $"


/**
 * Constructor
 * @param environment the environment
 */
vxManipulatorWindowLevel::vxManipulatorWindowLevel( vxEnvironment & environment )
:vxManipulator( environment ),
m_bAdjusting( false )
{
} // vxManipulatorWindowLevel()


/**
 * Event handler if key is pressed
 *
 * @param position of mouse
 * @param delta of wheel
 * @param keyboard state
 *
 * @return handled?
 */
bool vxManipulatorWindowLevel::OnButtonDown( const Point2D<int2> & position, 
                                             const ButtonEnum eButtonId, 
                                             const uint4 uKeyboardState )
{

  bool bIfHandled( false );

  if ( eButtonId == BUTTON_RIGHT )
  {
    vxEnvironment::GetUndoActions().PushAction( new vxUndoActionColormap( GetEnvironment().GetColormap() ) );
    bIfHandled = true;
    SetLastMousePosition( Point2D<uint2>( position ) );
    m_buttonDownPos = position;
    m_initialWindow = GetEnvironment().GetColormap()->GetWindowLevel().GetWindow();
    m_initialLevel = GetEnvironment().GetColormap()->GetWindowLevel().GetLevel();
    m_bAdjusting = true;
    GetEnvironment().SetUserInteracting( true );
  }

  return bIfHandled;

} // OnButtonDown()


/**
 * Event handler if key is pressed
 *
 * @param position of mouse
 * @param delta of wheel
 * @param state of keyboard
 *
 * @return handled?
 */ 
bool vxManipulatorWindowLevel::OnButtonUp( const Point2D<int2> & position, 
                                           const ButtonEnum eButtonId, 
                                           const uint4 uKeyboardState )
{

  bool bIfHandled( false );

  try
  {
    if ( eButtonId == BUTTON_RIGHT )
    {
      bIfHandled = true;
      GetEnvironment().GetColormap()->Modified( vxModInfo( vxColormap, vxUtils::COLORMAP_MODIFIED ) );
      GetEnvironment().SetUserInteracting( false );
      m_bAdjusting = false;
    }
  }
  catch( ... )
  {
    throw ex::InvalidDataException( LogRec("Unhandle exception on", "OnButtonUp", "vxManipulatorWindowLevel") );
  }

  return bIfHandled;

} // OnButtonUp()


/**
 * Event handler if key is pressed
 *
 * @param position of mouse
 * @param keyboard state
 *
 * @return handled?
 */ 
bool vxManipulatorWindowLevel::OnMouseMove( const Point2D<int2> & position, 
                                            const uint4 uKeyboardState )
{

  bool bIfHandled(false);

  if (GetEnvironment().GetColormap() != NULL)
  {
    if (uKeyboardState & STATE_RBUTTON && m_bAdjusting == true)
    {
      bIfHandled = true;
      // old code scaled delta by window size which allowed to browse 
      // through the full range in one go. However, adjustments were 
      // too coarse and we now use adjustments relative to per pixel movement
      ////////////////////////////////////////////////////////////////////////
      float4 fUnitDelta(1.0F/(float4)vxIntensityConverter::GetMaxValue(GetEnvironment().GetIntensityVolume()->GetHeader(), false));
      float4 fDeltaX((position.m_x - m_buttonDownPos.m_x) * fUnitDelta);
      float4 fDeltaY((position.m_y - m_buttonDownPos.m_y) * fUnitDelta);

      int4 iMax(vxIntensityConverter::GetMaxValue(GetEnvironment().GetIntensityVolume()->GetHeader()));
      int4 iShift(log(static_cast<float4>(GetNextPowerOfTwo(iMax+1)))/log(2.0F) - 8.0F);
      int4 iAccelerate(pow(2.0F, 0.5F*iShift));
      if (GetKeyMultiplier(uKeyboardState) == 1)
      {
        iAccelerate = 1;
      }

      // go faster with special keys
      fDeltaX *= GetKeyMultiplier(uKeyboardState) * iAccelerate;
      fDeltaY *= GetKeyMultiplier(uKeyboardState) * iAccelerate;

      // go faster depending on the distance from the initial mouse button down location
      fDeltaX *= GetDistanceMultiplier(position, 0);
      fDeltaY *= GetDistanceMultiplier(position, 1);

      // window/level manipulator
      ZeroOne window(m_initialWindow);
      ZeroOne level(m_initialLevel);
      window += fDeltaX;
      level  += fDeltaY;
  
      window = DnClamp(1.0f, window);
      window = UpClamp(0, window);
      level  = DnClamp(1.0f, level);
      level  = UpClamp(0, level);
      GetEnvironment().GetColormap()->SetWindowLevel(vxWindowLevel(window, level));
      GetEnvironment().GetColormap()->SetDirty();
      GetEnvironment().GetColormap()->Modified(vxModInfo(vxColormap, vxUtils::COLORMAP_MODIFIED), true);
     }
  }
  else
  {
    throw ex::IllegalArgumentException(LogRec("GetEnvironment().GetColormap() is NULL", 
                                              "vxManipulatorWindowLevel", 
                                              "OnMouseMove()"));
  }
  if (m_bAdjusting == true)
  {
    vxMouseCursor::SetCursor("CURSOR_TFUNC");
  }
  
  // set last position to this one
  SetLastMousePosition(Point2D<uint2>(position));

  return bIfHandled;

} // OnMouseMove()


/**
 * Gets a multiplier based on the distance from the initial mouse button down position
 * @param position the current mouse position
 * @param uDimension 0 or 1 for x or y dimension respectively
 * @return the multiplier
 */
float4 vxManipulatorWindowLevel::GetDistanceMultiplier( const Point2D<int2> & position, 
                                                        const uint4 uDimension ) const
{

  // get the distance from the initial point where the button was pressed down
  const float4 fDistance(fabs(static_cast<float4>(position[ uDimension ] - m_buttonDownPos[ uDimension ])));

  // start with 0.5 per pixel then increase to 1.0 for first several pixels
  const float4 fMinimumMultiplier(0.5F);
  const float4 fMinimumThreshold(40);

  // increase the speed linearly up to a certain distance where the multiplier reaches a maximum
  const float4 fMaximumThreshold(500);
  const float4 fMaximumMultiplier(4);

  // compute the multiplier
  float4 fMulitiplier(1.0F);
  if (fDistance <= fMinimumThreshold)
  {
    fMulitiplier = Interpolate(0.0F, fDistance, fMinimumThreshold, fMinimumMultiplier, 1.0F);
  }
  else if (fDistance > fMinimumThreshold && fDistance < fMaximumThreshold)
  {
    fMulitiplier = Interpolate(fMinimumThreshold, fDistance, fMaximumThreshold, 1.0F, fMaximumMultiplier);
  }
  else if (fDistance >= fMaximumThreshold)
  {
    fMulitiplier = fMaximumMultiplier;
  }

  return fMulitiplier;

} // GetDistanceMultiplier()


// undefines
#undef FILE_REVISION


// $Log: vxManipulatorWindowLevel.C,v $
// Revision 1.10  2006/08/03 14:01:35  geconomos
// removed inclusion of "vxRenderEngineData.h"
//
// Revision 1.9  2006/07/07 13:34:37  geconomos
// set "UserInteracting" flag in associated environment as needed
//
// Revision 1.8  2006/06/27 20:13:23  geconomos
// setting "user interacting" flag in environment accordingly
//
// Revision 1.7  2006/02/24 20:13:36  frank
// added undo for rendering mode and colormap
//
// Revision 1.6  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.5  2004/04/15 16:05:16  frank
// split out the preset manipulation from the window/level manipulator
//
// Revision 1.4  2004/04/14 18:48:53  frank
// Extended presets to set the rendering mode and other visualization settings
//
// Revision 1.3  2004/04/06 16:26:11  frank
// unified environment access naming
//
// Revision 1.2  2004/03/05 16:37:30  geconomos
// Updated to compile with 7.1 compiler.
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.63  2003/12/24 15:37:33  dongqing
// add try-catch for OnButtonUp. This is for tracking dangling
// pointer bug
//
// Revision 1.62  2003/12/23 23:58:57  michael
// added adjustment function for colormap initialized from Preset for CT
//
// Revision 1.61  2003/12/22 16:22:12  michael
// worked on the DeciderSphere to take a radius that is float since it needs to
// be scaled anisotropically with the spacing of the volume which cannot be
// handled properlin on an integer basis.
//
// Revision 1.60  2003/11/07 13:07:43  frank
// Spacebar is already used for something else :(
//
// Revision 1.59  2003/11/06 16:26:45  frank
// Added the ability to flip through presets with the space bar.
//
// Revision 1.58  2003/10/31 17:12:14  frank
// Issue #3375: Inverted the level and gave the adjustment some tweaking so that you can get fine detail at first, then acceleration as you move away from the initial click point
//
// Revision 1.57  2003/06/19 20:21:22  michael
// adjusted for proper use of vxUtils::vxEvent...
//
// Revision 1.56  2003/06/11 02:07:21  michael
// fixed self-introduced bug ...
//
// Revision 1.55  2003/06/10 13:29:48  michael
// made increment more sensitive to used bits in volume
//
// Revision 1.54  2003/06/09 12:29:55  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.53  2003/06/04 20:02:34  michael
// fixed scaling to be dependent on modality and bits used instead of assuming 12 bits
//
// Revision 1.52  2003/05/16 13:21:35  frank
// formatting
//
// Revision 1.51  2003/05/07 15:01:31  dongqing
// code walkthrough
//
// Revision 1.50  2003/05/07 11:44:02  michael
// initial code review
//
// Revision 1.49  2003/03/21 15:40:39  geconomos
// Updated code for the removal of the viewer type enumeration.
//
// Revision 1.48  2003/03/19 19:33:47  frank
// Removed obsolete VIEW_CHANGING mode
//
// Revision 1.47  2002/11/27 23:02:29  michael
// brought code back in since it breaks too much :-(
//
// Revision 1.46  2002/11/27 22:44:45  michael
// commented out the relative camera adjustment
//
// Revision 1.45  2002/11/25 13:44:57  geconomos
// New mouse cursor handling.
//
// Revision 1.44  2002/10/24 19:54:10  michael
// added initial camera and colormap indices
//
// Revision 1.43  2002/10/24 15:35:14  michael
// proportionally align a camera to the new box when chosing new camera preset
//
// Revision 1.42  2002/10/17 15:33:53  frank
// Centralized key multiplier functionality.
//
// Revision 1.41  2002/10/16 15:56:36  michael
// took out redundant include declarations
//
// Revision 1.40  2002/10/14 15:37:32  michael
// accounting for const methods in vxColormap/vxCamera
//
// Revision 1.39  2002/10/07 19:23:46  michael
// fixed bug, camera presets only "active" in 3D views
//
// Revision 1.38  2002/10/03 17:36:13  michael
// Extended possible presets (Colormap and Camera)
//
// Revision 1.37  2002/10/01 13:56:57  michael
// Fixed preset initialization (alpha curve)
//
// Revision 1.36  2002/10/01 13:04:02  michael
// Temporarily added presets in case there is no database ...
//
// Revision 1.35  2002/09/26 23:38:21  dmitry
// Assignment operator called on Colormap.
//
// Revision 1.34  2002/09/26 18:11:45  dmitry
// Colormp presets are now an instance instead of a pointer.
//
// Revision 1.33  2002/09/25 20:36:41  michael
// Completed vxTransferFunction to vxColormap transition
//
// Revision 1.32  2002/09/19 19:34:28  michael
// Initial version for std::vec of colormap presets in environment
//
// Revision 1.31  2002/09/19 14:26:01  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.30  2002/09/17 13:01:02  michael
// adjustments for changes in transfer function
//
// Revision 1.29  2002/08/20 22:46:13  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.28  2002/07/30 20:22:18  dmitry
// bunch of stuff changed to vx
//
// Revision 1.27  2002/06/25 20:24:21  michael
// adjusted code following tracker issue 2462
//
// Revision 1.26  2002/06/25 18:01:20  michael
// adjusted code following tracker issue 2462
//
// Revision 1.25  2002/06/21 21:59:05  michael
// switch to view_changing while sculpting and changing window/level
// ... to render faster
//
// Revision 1.24  2002/06/21 19:24:54  dmitry
// Removed unnecessary cursor call OnMouseMove.
//
// Revision 1.23  2002/06/20 02:18:22  dmitry
// Using MouseCursor from the Environment.
//
// Revision 1.22  2002/06/14 15:55:12  geconomos
// Reimplemented mouse cursors.
//
// Revision 1.21  2002/06/12 20:55:19  michael
// added funtionality so that on mouse drag window/level only gets updated in
// current viewer, update on all others OnButtonUp()
//
// Revision 1.20  2002/06/04 22:49:57  ingmar
// fixed comment
//
// Revision 1.19  2002/06/04 14:21:22  dmitry
// Adjusting state toggle added.
//
// Revision 1.18  2002/05/09 14:44:56  dmitry
// TransferFunction enums moved to vxUtils.
//
// Revision 1.17  2002/05/03 22:25:48  jaddonisio
// First attempt to remove references to old block volume. God help us!
//
// Revision 1.16  2002/05/02 15:52:10  michael
// completed transition from MouseCursor to vxMouseCursor
//
// Revision 1.15  2002/05/02 15:29:40  michael
// completed transition from old MouseCursor to vxMouseCursor
//
// Revision 1.14  2002/04/23 20:51:21  dmitry
// New mouse cursor interface used.
//
// Revision 1.13  2002/04/22 14:18:50  dmitry
// using existing window level object instead of recreating it
//
// Revision 1.12  2002/04/21 21:59:08  michael
// Re-fixed window level AGAIN since checked in code does not work
//
// Revision 1.11  2002/04/16 19:05:21  michael
// introduced vxWindowLevel class and use it throughout the code
//
// Revision 1.10  2002/03/27 15:07:56  michael
// adjusted Get.SetSize() to Get/SetViewportSize()
//
// Revision 1.9  2002/03/27 14:25:17  michael
// fixed window size problem by using correct call in winRect
//
// Revision 1.8  2002/03/26 19:35:15  michael
// cosmetics
//
// Revision 1.7  2002/03/26 19:29:35  michael
// removed duplicated update mechanism
//
// Revision 1.6  2002/03/24 16:36:06  michael
// adjusted to new dsp files with less dependencies and introduced more forward declarations ... to be continued
//
// Revision 1.5  2002/03/22 19:24:56  manju
// Added WinRect and resize method. --- Dmitry
//
// Revision 1.4  2002/03/21 18:24:14  michael
// removed unused includes
//
// Revision 1.3  2002/03/12 00:46:16  michael
// SetModified() is only called if value is different than previous value
//
// Revision 1.2  2002/03/11 22:25:18  michael
// fixed update problem
//
// Revision 1.1  2002/03/11 19:42:43  michael
// moved xvManipulators, vxInput, and vxEnvironment to 75_vxManipulators
//
// Revision 1.3  2002/03/07 22:31:09  dmitry
// Modified to fit new architecture.
//
// Revision 1.2  2002/02/28 15:02:59  michael
// made code (alomst) coding guidelines conform
//
// Revision 1.1  2002/02/22 15:28:58  dmitry
// Initial Versions Replacing same classes w/o vx prefix.
//
// Revision 1.7  2002/01/29 20:09:59  michael
// comments, restructuring, and misc
//
// Revision 1.6  2002/01/28 22:37:29  dmitry
// If WindowLevel stste is not set then set it.
//
// Revision 1.5  2002/01/22 21:12:56  michael
// cleaned the manipulators ... no more protected member data
//
// Revision 1.4  2001/12/19 13:50:08  geconomos
// Changed signature of OnMouseMove and added mouse cursor changing
//
// Revision 1.3  2001/12/12 18:37:20  michael
// made actual pointer to object private such that derived classes are forced
// to use the Set/Get-methods.
//
// Revision 1.2  2001/12/12 16:30:50  dmitry
// using GetTransferfuntion instead of member directly
//
// Revision 1.1  2001/12/06 17:24:24  michael
// Added Inputs class and the first manipulator classes. One is an implementation
// class for manipulating a TransferFunction (presets and right mouse).
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorWindowLevel.C,v 1.10 2006/08/03 14:01:35 geconomos Exp $
// $Id: vxManipulatorWindowLevel.C,v 1.10 2006/08/03 14:01:35 geconomos Exp $