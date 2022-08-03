// $Id: vxManipulatorMPROrthoPanZoom.C,v 1.5 2007/06/28 19:33:55 jmeade Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (mailto:michael@viatronix.com)


// include declarations
#include "stdafx.h"
#include "vxManipulatorMPROrthoPanZoom.h"
#include "vxEnvironment.h"
#include "vxMouseCursor.h"
#include "vxZoomFactor.h"


// define declarations
#define FILE_REVISION "$$"


/**
 * default constructor.
 * @param environment
 */
vxManipulatorMPROrthoPanZoom::vxManipulatorMPROrthoPanZoom(vxEnvironment & environment)
:vxManipulator(environment), 
m_bPan(false)
//*******************************************************************
{
}


/**
 * Handle the mouse scroll events
 * @param position mouse position
 * @param iDelta delta
 * @param uKeyboardState key state
 */
bool vxManipulatorMPROrthoPanZoom::OnMouseWheel(const Point2D<int2> & position, 
                                                const int2 iDelta, 
                                                const uint4 uKeyboardState)
//*******************************************************************
{
  bool bIfHandled(false);

  vxWinRect winRect(GetEnvironment().GetWinRect());
  uint2 uAxisX(GetEnvironment().GetCamera()->GetViewRight().GetMaxAbsComponentCoord());
  uint2 uAxisY(GetEnvironment().GetCamera()->GetViewUp().GetMaxAbsComponentCoord());
  float4 fWidthX(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim()[uAxisX]);
  float4 fHeightY(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim()[uAxisY]);
  float4 fAspectRatioVolume(fHeightY/fWidthX);
  float4 fAspectRatioWindow((float4)winRect.GetViewportHeight()/(float4)winRect.GetViewportWidth());
  float4 fWidth(GetEnvironment().GetZoomFactor()->GetSize());
  float4 fWidthMax(max(fWidthX, fHeightY));

  int4 iDirection(iDelta / vxInput::GetMouseWheelQuantum());
  float4 fDirection(5.0F*static_cast<float4>(iDirection));

  float4 fWidthCopy(fWidth);
  fWidth += fDirection * 0.01F * fWidthMax;

  if (fWidth > fWidthMax)
  {
    fWidth = fWidthMax;
  }
  if (fWidth <= 0.01F * fWidthMax)
  {
    fWidth = fWidthCopy;
  }

  GetEnvironment().GetZoomFactor()->SetSize(fWidth);
  GetEnvironment().GetZoomFactor()->Modified(vxModInfo(vxZoomFactor, vxUtils::ZOOMFACTOR_MODIFIED));

  return bIfHandled;
} // OnMouseWheel()

 
/**
 * Handle the button down event
 * @param position mouse position
 * @param eButtonId mouse button
 * @param uKeyboardState key state
 */
bool vxManipulatorMPROrthoPanZoom::OnButtonDown(const Point2D<int2> & position, 
                                                const ButtonEnum eButtonId, 
                                                const uint4 uKeyboardState)
//*******************************************************************
{
  bool bIfHandled(false);

  vxWinRect winRect(GetEnvironment().GetWinRect());
  uint2 uAxisX(GetEnvironment().GetCamera()->GetViewRight().GetMaxAbsComponentCoord());
  uint2 uAxisY(GetEnvironment().GetCamera()->GetViewUp().GetMaxAbsComponentCoord());
  float4 fWidthX(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim()[uAxisX]);
  float4 fHeightY(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim()[uAxisY]);
  float4 fAspectRatioVolume(fHeightY/fWidthX);
  float4 fAspectRatioWindow((float4)winRect.GetViewportHeight()/(float4)winRect.GetViewportWidth());
  float4 fWidth(GetEnvironment().GetZoomFactor()->GetSize());
  float4 fWidthMax(max(fWidthX, fHeightY));

  if (eButtonId == BUTTON_LEFT)
  {
    vxEnvironment::SetUserInteracting( true );
    SetLastMousePosition(position);
    m_bPan = true;

    Point3Df world;
    GetEnvironment().GetViewerType()->ConvertMouseToWorld(position, world);
    Triple<float4> volDim(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim());

    for (int i(0); i<3; i++)
    {
      world[i] = Bound(0.0F, world[i], volDim[i]);
    }

    GetEnvironment().GetZoomFactor()->SetCenter(world);
    GetEnvironment().GetZoomFactor()->Modified(vxModInfo(vxZoomFactor, vxUtils::ZOOMFACTOR_MODIFIED));
  }

  return bIfHandled;
}// OnButtonDown()


/**
 * Handle the mouse move events for rotation.
 * Rotation axis is a linear combination of the View Direction
 * and the Up vector/Right vector. The rotation is more about the
 * Up vector/Right vector near the object and is more about the
 * View Direction further away from the object.
 * @param position mouse position
 * @param uKeyboardState key state
 */
bool vxManipulatorMPROrthoPanZoom::OnMouseMove(const Point2D<int2> & position, 
                                               const uint4 uKeyboardState)
//*******************************************************************
{
  bool bIfHandled(false);

  if (m_bPan == true)
  {
    vxMouseCursor cursor("CURSOR_HAND");

    bIfHandled = true;

    Point3Df world;
    GetEnvironment().GetViewerType()->ConvertMouseToWorld(position, world);
    Triple<float4> volDim(GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim());

    for (int i(0); i<3; i++)
    {
      world[i] = Bound(0.0F, world[i], volDim[i]);
    }

    GetEnvironment().GetZoomFactor()->SetCenter(world);
    GetEnvironment().GetZoomFactor()->Modified(vxModInfo(vxZoomFactor, vxUtils::ZOOMFACTOR_MODIFIED));

    SetLastMousePosition(position);
  }

  return bIfHandled;
} // OnMouseMove()


/**
 * Handle button up event
 * @param position mouse position
 * @param eButtonId mouse button
 * @param uKeyboardState key state
 */
bool vxManipulatorMPROrthoPanZoom::OnButtonUp(const Point2D<int2> & position, 
                                              const ButtonEnum eButtonId, 
                                              const uint4 uKeyboardState)
//*******************************************************************
{  
  bool bIfHandled(false);
  //    bIfHandled = true;  

  if (eButtonId == BUTTON_LEFT)
  {
    vxEnvironment::SetUserInteracting( false );
    m_bPan = false;
  }  
  SetLastMousePosition(position);

  return bIfHandled;
} // OnButtonUp()


/**
 * Called when the mouse cursor is to be set
 * @return true if cursor was set, false otherwise
 */
bool vxManipulatorMPROrthoPanZoom::OnSetCursor() 
//*******************************************************************
{ 
  if (m_bPan == true)
  {
    vxMouseCursor("CURSOR_HAND");
    return true;
  }
  return false;
} // OnSetCursor() 


/**
 * Event handler if a key is pressed
 * @param uKeyId         the key pressed
 * @param uKeyboardState key state
 * @return if handled
 */ 
bool vxManipulatorMPROrthoPanZoom::OnKeyPressed(const uint1 uKeyId, 
                                                const uint4 uKeyboardState)
//*******************************************************************
{
  bool bIfHandled(false);

  try
  {
    switch (uKeyId)
    {
    case vxInput::KEY_EQUALS:
    case vxInput::KEY_PLUS_KEYPAD:
      OnMouseWheel(Point2D<int2>(0, 0), - vxInput::GetMouseWheelQuantum(), uKeyboardState);
      bIfHandled = true;
      break;

    case vxInput::KEY_MINUS:
    case vxInput::KEY_MINUS_KEYPAD:
      OnMouseWheel(Point2D<int2>(0, 0), vxInput::GetMouseWheelQuantum(), uKeyboardState);
      bIfHandled = true;
      break;

    case vxInput::KEY_A:
      {
        float4 fPreviousZoom(-1);
        const uint4 uAttempts(100);

        for (uint4 i(0); i<uAttempts && fPreviousZoom != GetEnvironment().GetZoomFactor()->GetSize(); ++i)
        {
          OnMouseWheel(Point2D<int2>(0, 0), vxInput::GetMouseWheelQuantum(), 0);
        }

        bIfHandled = true;

        GetEnvironment().GetZoomFactor()->SetCenter((Point3Df)(0.5F * GetEnvironment().GetIntensityVolume()->GetHeader().GetWorldDim()));
        GetEnvironment().GetViewerType()->InitializeView();

        break;
      }
    default:
      break;

    } // switch key
  } // try
  catch (...)
  {
    throw ex::VException(LogRec("OnKeyPressed failed.", "vxManipulatorMPROrthoPanZoom", "OnKeyPressed"));
  }

  return bIfHandled;
} // OnKeyPressed()


#undef FILE_REVISION


// Revision history:
// $Log: vxManipulatorMPROrthoPanZoom.C,v $
// Revision 1.5  2007/06/28 19:33:55  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.4.6.1  2007/05/29 13:51:09  gdavidson
// Issue #5565: Set the user interacting flag when manipulator is active
//
// Revision 1.4  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.3  2005/03/08 19:09:13  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.2.6.1  2005/02/08 15:34:51  frank
// added plus/minus key on standard keyboard in addition to numeric keypad
//
// Revision 1.2  2004/04/27 14:27:51  vxconfig
// fixed modified message to pass in the right event
//
// Revision 1.1  2004/04/26 21:25:07  vxconfig
// added renamed classes
//
// Revision 1.3  2004/04/26 19:00:16  michael
// moved BoundingBox() / GetLeft() / GetRight() / etc. from vxCamera to
// vxViewerType and also reworked the MPR pan/zoom behaviour so that
// we are able to share a vxZoomFactor
//
// Revision 1.2  2004/04/06 16:25:11  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.35  2004/01/20 21:29:51  frank
// added zooming on plus and minus keys
//
// Revision 1.34  2004/01/20 20:13:01  frank
// added ability to reset the zoom of the image
//
// Revision 1.33  2003/09/10 20:16:12  dongqing
// Add try-catch on OnMouseWheel
//
// Revision 1.32  2003/07/11 19:49:14  michael
// final code reviews
//
// Revision 1.31  2003/06/19 20:21:22  michael
// adjusted for proper use of vxUtils::vxEvent...
//
// Revision 1.30  2003/06/18 15:19:56  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.29  2003/05/16 13:21:34  frank
// formatting
//
// Revision 1.28  2003/04/29 21:17:35  michael
// removed vxManipulatorCamera from inheritance
//
// Revision 1.27  2003/03/12 15:19:47  frank
// Properly interpreted scroll wheel messages
//
// Revision 1.26  2003/02/28 22:09:34  michael
// removed vxWinRect from vxManipulatorr. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.25  2003/01/22 21:57:26  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.24  2002/11/25 13:44:56  geconomos
// New mouse cursor handling.
//
// Revision 1.23  2002/10/13 23:04:26  michael
// accounting for the const Box<T> returned from the camera object
//
// Revision 1.22  2002/10/01 19:34:53  michael
// Fixed pan problem in zoomed coronal view, removed redundant code, and
// "release" pan mode when moving outside the window (issue #2722)
//
// Revision 1.21  2002/08/20 22:37:20  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.20  2002/06/27 19:27:34  michael
// completed transition from CameraNew to vxCamera
//
// Revision 1.19  2002/06/20 17:17:29  uday
// RecalculateLocalBox() update.
//
// Revision 1.18  2002/06/20 02:15:41  dmitry
// OnMouseEnter and OnMouseLeave implemented to setup the mouse cursor.
//
// Revision 1.17  2002/06/14 15:55:12  geconomos
// Reimplemented mouse cursors.
//
// Revision 1.16  2002/05/10 20:36:04  uday
// Corrected coronal orientation.
//
// Revision 1.15  2002/05/04 10:42:44  michael
// cosmetics
//
// Revision 1.14  2002/05/03 17:38:59  jenny
// zoom factor
//
// Revision 1.13  2002/05/02 15:52:10  michael
// completed transition from MouseCursor to vxMouseCursor
//
// Revision 1.12  2002/05/02 15:29:40  michael
// completed transition from old MouseCursor to vxMouseCursor
//
// Revision 1.11  2002/04/24 14:58:31  jenny
// vxBlockVolume stuff
//
// Revision 1.10  2002/04/23 20:51:22  dmitry
// New mouse cursor interface used.
//
// Revision 1.9  2002/04/23 20:12:23  michael
// replaced Density by Intensity
//
// Revision 1.8  2002/04/23 15:33:47  jenny
// switched to vxBlockVolume<uint2>
//
// Revision 1.7  2002/04/18 19:01:59  dmitry
// initializing default direction.
//
// Revision 1.6  2002/04/17 18:17:55  frank
// Cleaned up header & footer.
//
// Revision 1.5  2002/04/16 16:41:18  uday
// recalculate box instead of resize
//
// Revision 1.4  2002/04/10 17:55:28  uday
// PanZoom in windows with arbitrary aspect ratio.
//
// Revision 1.3  2002/04/04 17:01:00  uday
// Corrected Panning (Michael)
//
// Revision 1.2  2002/03/29 00:13:11  uday
// OnMouseWheel() update
//
// Revision 1.1  2002/03/26 21:14:49  uday
// Initial Version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorMPROrthoPanZoom.C,v 1.5 2007/06/28 19:33:55 jmeade Exp $
// $Id: vxManipulatorMPROrthoPanZoom.C,v 1.5 2007/06/28 19:33:55 jmeade Exp $
