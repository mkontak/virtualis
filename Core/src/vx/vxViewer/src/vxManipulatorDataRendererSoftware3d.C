// $Id: vxManipulatorDataRendererSoftware3d.C,v 1.9 2007/01/16 17:04:18 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar & Soeren {ingmar|soeren}@viatronix.com


// includes
#include "stdafx.h"
#include "vxManipulatorDataRendererSoftware3d.h"
#include "vxEnvironment.h"
#include "vxUtils.h"
#include "vxDataRendererSoftware.h"
#include "vxRenderingMode.h"


// defines
#define FILE_REVISION "$Revision: 1.9 $"


/**
 * Constructor
 * @param environment
 */
vxManipulatorDataRendererSoftware3d::vxManipulatorDataRendererSoftware3d(vxEnvironment & environment)
:vxManipulator(environment)
//*******************************************************************
{
  m_bDraggingLightSource = false;
}


/**
 * Event handler if key is pressed
 * @param Key Id
 * @param state of keyboard
 */ 
bool vxManipulatorDataRendererSoftware3d::OnKeyPressed(const uint1 uKeyId,
                                                       const uint4 uKeyboardState)
//*******************************************************************
{
  bool bIfHandled(false);
  bool bModified(false);
  vxShareableObject<vxDataRendererSoftware> * pData = dynamic_cast<vxShareableObject<vxDataRendererSoftware> *>(GetEnvironment().GetDataRendererSoftware());

  if (pData != NULL)
  {
    // The following should not be exposed to the end user!
    switch (uKeyId)
    {
    // toggle shading
    case vxInput::KEY_S:
      // make sure the <ctrl> key is not pressed
      if ( ( uKeyboardState & STATE_CTRL ) == false )
      {
        //GetEnvironment().GetRenderingMode()->SetEnabled( vxRenderingMode::SHOW_SHADOWS, ! GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_SHADOWS ) );
        //GetEnvironment().GetRenderingMode()->Modified( vxModInfo( vxRenderingMode, 0 ) );
        LogDbg("Shading is " + std::string((GetEnvironment().GetRenderingMode()->IsEnabled( vxRenderingMode::SHOW_SHADOWS ) == true) ? "on" : "off"), "vxManipulatorDataRendererSoftware3d", "OnKeyPressed");
        //bModified = true;
      }
      break;

    //case vxInput::KEY_J: // toggle jittering of ray start position
    //  GetEnvironment().GetRendererCpu3dData()->SetJittering(!GetEnvironment().GetRendererCpu3dData()->IsJittering());
    //  LogDbg("Jittering is " + std::string((GetEnvironment().GetRendererCpu3dData()->IsJittering() == true) ? "on" : "off"), "vxManipulatorDataRendererSoftware3d", "OnKeyPressed");
    //  bModified = true;
    //  break;

    //case vxInput::KEY_I: // toggle pre-integrated rendering
    //  GetEnvironment().GetRendererCpu3dData()->SetPreIntegration(!GetEnvironment().GetRendererCpu3dData()->IsPreIntegration());
    //  LogDbg("Preintegration is " + std::string((GetEnvironment().GetRendererCpu3dData()->IsPreIntegration() == true) ? "on" : "off"), "vxManipulatorDataRendererSoftware3d", "OnKeyPressed");
    //  bModified = true;
    //  break;

    //case vxInput::KEY_Z: // increase sampling rate
    //  GetEnvironment().GetRendererCpu3dData()->SetSampleDistanceScale(2.0F * GetEnvironment().GetRendererCpu3dData()->GetSampleDistanceScale());
    //  LogDbg("Sampling rate scale is " + ToAscii(GetEnvironment().GetRendererCpu3dData()->GetSampleDistanceScale()), "vxManipulatorDataRendererSoftware3d", "OnKeyPressed");
    //  bModified = true;
    //  break;

    //case vxInput::KEY_X: // decrease sampling rate
    //  GetEnvironment().GetRendererCpu3dData()->SetSampleDistanceScale(0.5F * GetEnvironment().GetRendererCpu3dData()->GetSampleDistanceScale());
    //  LogDbg("Sampling rate scale is " + ToAscii(GetEnvironment().GetRendererCpu3dData()->GetSampleDistanceScale()), "vxManipulatorDataRendererSoftware3d", "OnKeyPressed");
    //  bModified = true;
    //  break;

    //case vxInput::KEY_F: // toggle frame rate high/low (fly/examine)
    //  GetEnvironment().GetRendererCpu3dData()->SetHighFramerateDemand(!GetEnvironment().GetRendererCpu3dData()->GetHighFramerateDemand());
    //  break;

    default:
      break; // nothing else is handled
    }
  }

  //if (bModified == true)
  //{
  //  pData->Modified(vxModInfo(vxDataRenderer, vxUtils::vxEventDataRendererEnum::DATARENDERER_MODIFIED));
  //}

  return bIfHandled;
} // OnKeyPressed()


/**
 * Event handler if mouse button goes down
 * @param position of mouse
 * @param delta of wheel
 * @param state of keyboard
 */ 
bool vxManipulatorDataRendererSoftware3d::OnButtonDown(const Point2D<int2> & position, 
                                                       const ButtonEnum eButtonId, 
                                                       const uint4 uKeyboardState)
//*******************************************************************
{
  bool bIfHandled(false);
  vxShareableObject<vxDataRendererSoftware> * pData = dynamic_cast<vxShareableObject<vxDataRendererSoftware> *>(GetEnvironment().GetDataRendererSoftware());

  if (eButtonId == BUTTON_MIDDLE)
  {
    m_bDraggingLightSource = true;
    vxWinRect winRect(GetEnvironment().GetWinRect());
    float4 fX((position.X() - winRect.GetReferencePoint().first)/(float4)winRect.GetViewportSize().first);
    float4 fY((position.Y() - winRect.GetReferencePoint().second)/(float4)winRect.GetViewportSize().second);
    GetEnvironment().GetRenderingMode()->SetRelativeMousePosition( std::pair< float4, float4 >( fX, fY ) );
    GetEnvironment().GetRenderingMode()->Modified( vxModInfo( vxRenderingMode, 0 ) );
    GetEnvironment().SetUserInteracting( true );
  }

  return bIfHandled;
} // OnButtonDown()


/**
 * Event handler if mouse button goes up
 * @param position of mouse
 * @param delta of wheel
 * @param state of keyboard
 */ 
bool vxManipulatorDataRendererSoftware3d::OnButtonUp(const Point2D<int2> & position, 
                                                     const ButtonEnum eButtonId, 
                                                     const uint4 uKeyboardState)
//*******************************************************************
{
  bool bIfHandled(false);

  if (eButtonId == BUTTON_MIDDLE)
  {
    m_bDraggingLightSource = false;
    GetEnvironment().SetUserInteracting( false );
  }

  return bIfHandled;
} // OnButtonUp()


/**
 * Event handler if mouse moves
 * @param position of mouse
 * @param state of keyboard
 */ 
bool vxManipulatorDataRendererSoftware3d::OnMouseMove(const Point2D<int2> & position, 
                                                      const uint4 uKeyboardState)
//*******************************************************************
{
  bool bIfHandled(false);

  if (m_bDraggingLightSource == true)
  {
    vxShareableObject<vxDataRendererSoftware> * pData = dynamic_cast<vxShareableObject<vxDataRendererSoftware> *>(GetEnvironment().GetDataRendererSoftware());

    vxWinRect winRect(GetEnvironment().GetWinRect());
    float4 fX((position.X() - winRect.GetReferencePoint().first)/(float4)winRect.GetViewportSize().first);
    float4 fY((position.Y() - winRect.GetReferencePoint().second)/(float4)winRect.GetViewportSize().second);
    GetEnvironment().GetRenderingMode()->SetRelativeMousePosition( std::pair< float4 , float4>( fX, fY ) );
    GetEnvironment().GetRenderingMode()->Modified( vxModInfo( vxRenderingMode, 0 ) );
  }

  return bIfHandled;
} // OnMouseMove()


#undef FILE_REVISION


// $Log: vxManipulatorDataRendererSoftware3d.C,v $
// Revision 1.9  2007/01/16 17:04:18  gdavidson
// Removed OnTimer
//
// Revision 1.8  2006/08/03 14:01:35  geconomos
// removed inclusion of "vxRenderEngineData.h"
//
// Revision 1.7  2006/07/07 13:34:37  geconomos
// set "UserInteracting" flag in associated environment as needed
//
// Revision 1.6  2006/06/27 20:13:23  geconomos
// setting "user interacting" flag in environment accordingly
//
// Revision 1.5  2006/01/27 15:47:45  geconomos
// still migrating to visual studio 2005
//
// Revision 1.4  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.3  2005/11/18 21:15:22  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.2.6.1  2005/07/19 11:49:31  frank
// missing include
//
// Revision 1.2  2004/10/18 14:47:57  frank
// moved rendering mode data out of software 3D data renderer
//
// Revision 1.1  2004/08/30 17:15:51  michael
// Data used by renderers is now polymorph as well. Adjusted manipulator, too.
//
// Revision 1.4  2004/04/06 16:25:36  frank
// unified environment access naming
//
// Revision 1.3  2004/03/26 21:16:04  frank
// fixed rendering timers so that two renderers can co-exist in a single environment without stepping on each other with the timers
//
// Revision 1.2  2004/03/12 19:51:02  frank
// removed annotation ROI and readjusted sculpting to remove handle set
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.11  2004/03/02 01:10:39  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.10.2.1  2004/02/18 20:02:03  vxconfig
// removed the Shading short cut key
//
// Revision 1.10  2004/02/07 03:43:29  michael
// fixed light source/direction issue in 3d rendering by making the mouse position
// relative to the viewport size instead of absolut. Now the change in vxWinRect
// has correctly no impact on shading.
//
// Revision 1.9  2004/01/09 17:21:44  frank
// fixed hardcoded key values - replaced with enumeration
//
// Revision 1.8  2003/11/11 16:11:26  michael
// commented out the ability to change jittering, pre-integration, or sampling rate.
//
// Revision 1.7  2003/10/21 17:09:10  frank
// Fixed shading toggle on <ctrl>-S - now just on plain S
//
// Revision 1.6  2003/09/29 17:21:00  michael
// accounting for the new event flags in vxEnvironment
//
// Revision 1.5  2003/09/22 15:56:15  michael
// added manipulation for jittering, shading, preintegration, and sampling rate
//
// Revision 1.4  2003/08/21 13:14:07  michael
// cometics
//
// Revision 1.3  2003/06/19 20:24:17  michael
// removed logged warning
//
// Revision 1.2  2003/06/19 20:21:22  michael
// adjusted for proper use of vxUtils::vxEvent...
//
// Revision 1.1  2003/04/29 21:54:17  michael
// initial version (renamed RenderQualitySpeedTradeoff to vxRendererCpu3dData)
//
// Revision 1.32  2002/11/13 00:09:49  michael
// took out experimental stuff
//
// Revision 1.31  2002/10/23 23:13:08  ingmar
// RendererQualitySpeedTradeoff now only keeps if high or low framrerate is desired (fly vs examine) the Cpu3D rederer keeps internally what the curent image size is and decides no its own in PstRender() how to change the resolution. the "f" key toggles between the two frame rate modes
// I also cleand out the too general sliders of the RendererQualitySpeedTradeoff. The class now only contains concrete elements that are actually used
//
// Revision 1.30  2002/09/30 14:51:20  ingmar
// removed vector of bool toggle modes
//
// Revision 1.29  2002/09/05 22:37:48  jaddonisio
// New vxComponent.
//
// Revision 1.28  2002/08/20 22:43:49  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.27  2002/08/15 14:28:37  ingmar
// disabled manual speed tuning features
//
// Revision 1.26  2002/08/12 14:34:55  ingmar
// added 'l' key to write out labels
//
// Revision 1.25  2002/08/06 15:04:13  ingmar
// use of new vxEventTimer
// now only initiating resolution changes inside the renderer
// now also only increasing resolution up to the resolution of the current viewport
//
// Revision 1.24  2002/08/06 14:49:15  ingmar
// use of new vxEventTimer
//
// Revision 1.23  2002/08/06 14:07:02  ingmar
// use of new vxEventTimer
//
// Revision 1.22  2002/08/05 16:20:36  dmitry
// Event name added to OnTimer.
//
// Revision 1.21  2002/07/31 19:49:11  ingmar
// now sending a different event such that no more hack is needed in v3D_Environment.cpp
//
// Revision 1.20  2002/07/16 20:48:33  ingmar
// added toggle button 'o'
//
// Revision 1.19  2002/07/12 21:21:00  ingmar
// fixed space leaoing, current framerate 512^3 256^2 => 5.3
//
// Revision 1.18  2002/07/12 16:09:26  ingmar
// removed obsolete SetResolutionIncreaseTimerEnabled() call
//
// Revision 1.17  2002/07/11 22:58:24  ingmar
// added event for modification of light
//
// Revision 1.16  2002/07/11 20:56:54  ingmar
// worked on progressive refinement
//
// Revision 1.15  2002/07/11 16:34:45  ingmar
// added timer for QualitySpeedTradeoff
//
// Revision 1.14  2002/06/25 19:33:02  soeren
// added initial version of moveable light
//
// Revision 1.13  2002/06/12 20:36:37  ingmar
// made slider control twice as fine
//
// Revision 1.12  2002/05/28 21:53:41  soeren
// removed transferfunction modification
//
// Revision 1.11  2002/05/09 21:07:55  jenny
// removed CreateHistogram. It is now in v3D_Volume class.
//
// Revision 1.10  2002/05/09 14:44:56  dmitry
// TransferFunction enums moved to vxUtils.
//
// Revision 1.9  2002/05/01 21:17:10  jenny
// commented out GetEnvironment().GetVolumeSetPtr()->CreateHistogram(); since it still uses volumeset
//
// Revision 1.8  2002/04/16 19:05:32  michael
// introduced vxWindowLevel class and use it throughout the code
//
// Revision 1.7  2002/04/09 23:45:06  ingmar
// added check if Transferfunction actually changed
//
// Revision 1.6  2002/04/08 21:48:41  ingmar
// added update Hsitogramm
//
// Revision 1.5  2002/04/05 22:02:50  ingmar
// set new default W/L for ankle dataset
//
// Revision 1.4  2002/04/01 20:55:37  ingmar
// fixed code according to compiler warning: now using float constant, not double
//
// Revision 1.3  2002/04/01 19:12:47  ingmar
// made sure only one update event is send
//
// Revision 1.2  2002/04/01 19:10:13  ingmar
// added space bar press for start of spin 360 degrees
//
// Revision 1.1  2002/03/29 01:39:56  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorDataRendererSoftware3d.C,v 1.9 2007/01/16 17:04:18 gdavidson Exp $
// $Id: vxManipulatorDataRendererSoftware3d.C,v 1.9 2007/01/16 17:04:18 gdavidson Exp $