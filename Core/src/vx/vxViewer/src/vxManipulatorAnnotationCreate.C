// $Id: vxManipulatorAnnotationCreate.C,v 1.9 2006/03/02 15:48:42 frank Exp $
//
// Copyright ©2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (mailto:meissner@viatronix.com)


// includes
#include "stdafx.h"
#include "vxManipulatorAnnotationCreate.h"
#include "vxEnvironment.h"
#include "vxViewerType.h"
#include "vxAnnotationPoint.h"
#include "vxUtils.h"
#include "vxDataAnnotationCreate.h"
#include "vxUndoActionAnnotation.h"


// defines
#define FILE_REVISION "$Revision: 1.9 $"


/**
 * Default constructor
 * @param environment
 */
vxManipulatorAnnotationCreate::vxManipulatorAnnotationCreate(vxEnvironment & environment)
:vxManipulator(environment),
m_bDragging(false)
//*******************************************************************
{
  m_pShareableObjectDataAC = dynamic_cast<vxShareableObject<vxDataAnnotationCreate> *>(GetEnvironment().GetElement(vxIDs::DataAnnotationCreate));
  m_pDataAC = static_cast<vxDataAnnotationCreate *>(m_pShareableObjectDataAC);

  if (m_pDataAC == NULL)
  {
    throw ex::InvalidDataException(LogRec("Data object not in environment", "vxManipulatorAnnotationCreate", "vxManipulatorAnnotationCreate"));
  }

  m_pDataAC->m_eCreationMode = vxDataAnnotationCreate::ON_MOUSE_DOWN_AND_UP;
  m_pDataAC->m_pAnnotation->SetEnvironment(&GetEnvironment());
  m_pDataAC->m_pAnnotation->SetStatus(vxAnnotation::CREATE);
} // constructor


/**
 * Destructor
 */
vxManipulatorAnnotationCreate::~vxManipulatorAnnotationCreate()
//*******************************************************************
{
} // destructor


/**
 * Event handler if the mouse is pressed
 * @param screen position of mouse
 * @param eButtonId mouse button
 * @param uKeyboardState state of the mouse
 * @return handled?
 */ 
bool vxManipulatorAnnotationCreate::OnButtonDown(const Point2D<int2> & mouseCoord, 
                                                 const ButtonEnum eButtonId, 
                                                 const uint4 uKeyboardState)
//*******************************************************************
{
  bool bIfHandled(false);

  if (IsAnnotationSupported() == false)
  {
    return bIfHandled;
  }

  if (eButtonId == BUTTON_LEFT)
  {
    Point<float4> worldPosition;
    if (GetEnvironment().GetViewerType()->ConvertMouseToWorld(mouseCoord, worldPosition) == false)
    {
      SetDragging(false);
      return bIfHandled;
    }

    bIfHandled = true;
    vxAnnotation * pAnnotation(m_pDataAC->m_pAnnotation);
    pAnnotation->SetEnvironment(&GetEnvironment());

    if (pAnnotation->GetNumHandles() == 0)
    {
      m_pShareableObjectDataAC->Modified(vxModInfoID(vxElement, vxIDs::DataAnnotationCreate, 0));
    }

    if (pAnnotation->GetNumCreationHandles() == pAnnotation->GetNumHandles())
    {
      vxHandle handle(pAnnotation->GetHandle(pAnnotation->GetNumHandles()-1));
      handle.SetPosition(worldPosition);
      pAnnotation->SetHandle(pAnnotation->GetNumHandles()-1, handle);
      pAnnotation->Morph(GetEnvironment());
      pAnnotation->Update(GetEnvironment());
      pAnnotation->SetSelectedHandleIndex(-1);

      GetEnvironment().GetAnnotations()->Add(m_pDataAC->m_pAnnotation);
      m_pDataAC->m_pAnnotation = NULL;

      // allow undo of creation
      vxEnvironment::GetUndoActions().PushAction( new vxUndoActionAnnotationAdd( GetEnvironment().GetAnnotations(), GetEnvironment().GetAnnotations()->GetActiveIndex() ) );

      GetEnvironment().GetAnnotations()->Modified(vxModIndInfo(vxAnnotations, GetEnvironment().GetAnnotations()->GetActiveIndex(), vxUtils::ANNOTATIONS_MODIFIED));
      GetEnvironment().GetAnnotations()->Modified(vxModIndInfo(vxAnnotations, GetEnvironment().GetAnnotations()->GetActiveIndex(), vxUtils::ANNOTATIONS_COMPLETED));
      GetEnvironment().GetInputFeedback().Add(vxFeedback(typeid(*this), vxFeedback::DONE));
      SetDragging(false);
    }
    else
    {
      // what type of creation mode is used?
      vxHandle handle;
      handle.SetPosition(worldPosition);
      pAnnotation->AddHandle(handle);
      pAnnotation->SetSelectedHandleIndex(pAnnotation->GetNumHandles()-1);

      if (m_pDataAC->m_eCreationMode == vxDataAnnotationCreate::ON_MOUSE_DOWN_AND_UP)
      {
        if (pAnnotation->GetNumCreationHandles() != pAnnotation->GetNumHandles())
        {
          vxHandle handle;
          handle.SetPosition(worldPosition);
          pAnnotation->AddHandle(handle);
          pAnnotation->SetSelectedHandleIndex(pAnnotation->GetNumHandles()-1);
        }
      }
      pAnnotation->Update(GetEnvironment());
      m_pShareableObjectDataAC->Modified(vxModInfoID(vxElement, vxIDs::DataAnnotationCreate, 0));
      SetDragging(true);
    } // not yet all handles set
  } // left button?

  return bIfHandled;
} // OnButtonDown()


/**
 * Event handler if the mouse is moved
 * @param position Relative Mouse position
 * @param uKeyboardState mouse state
 * @return handled?
 */ 
bool vxManipulatorAnnotationCreate::OnMouseMove(const Point2D<int2> & mousePosition, 
                                                const uint4 uKeyboardState)
//*******************************************************************
{
  bool bIfHandled(false);

  if (IsAnnotationSupported() == false)
  {
    return bIfHandled;
  }

  Point<float4> worldPosition;
  if (IsDragging() == false || GetEnvironment().GetViewerType()->ConvertMouseToWorld(mousePosition, worldPosition) == false)
  {
    return bIfHandled;
  }

  // adjust the handle of the annotation currently being in progress
  vxAnnotation * pAnnotation(m_pDataAC->m_pAnnotation);
  pAnnotation->SetEnvironment(&GetEnvironment());

  // what type of creation mode is used?
  if (m_pDataAC->m_eCreationMode == vxDataAnnotationCreate::WHILE_MOUSE_DOWN)
  {
    vxHandle handle;
    handle.SetPosition(worldPosition);
    pAnnotation->AddHandle(handle);
    pAnnotation->SetSelectedHandleIndex(pAnnotation->GetNumHandles()-1);
    if (pAnnotation->PerformUpdateWhileDragging() == true)
      pAnnotation->Update(GetEnvironment());
    m_pShareableObjectDataAC->Modified(vxModInfoID(vxElement, vxIDs::DataAnnotationCreate, 0));
    bIfHandled = true;
  }
  else if (m_pDataAC->m_eCreationMode == vxDataAnnotationCreate::ON_MOUSE_DOWN 
           || m_pDataAC->m_eCreationMode == vxDataAnnotationCreate::ON_MOUSE_DOWN_AND_UP)
  {      
    vxHandle handle(pAnnotation->GetHandle(pAnnotation->GetNumHandles()-1));
    handle.SetPosition(worldPosition);
    pAnnotation->SetHandle(pAnnotation->GetNumHandles()-1, handle);
    if (pAnnotation->PerformUpdateWhileDragging() == true)
      pAnnotation->Update(GetEnvironment());
    m_pShareableObjectDataAC->Modified(vxModInfoID(vxElement, vxIDs::DataAnnotationCreate, 0));
    bIfHandled = true;
  }
  else // this annotation has no handle
  {
    bIfHandled = false;
  }

  return bIfHandled;
} // OnMouseMove()


/**
 * Event handler if the mouse is released
 * @param position mouse position
 * @param eButtonId mouse button
 * @param uKeyboardState mouse state
 * @return handled?
 */ 
bool vxManipulatorAnnotationCreate::OnButtonUp(const Point2D<int2> & mousePosition, 
                                               const ButtonEnum eButtonId,
                                               const uint4 uKeyboardState)
//*******************************************************************
{
  bool bIfHandled(false);

  if (IsAnnotationSupported() == false)
  {
    return bIfHandled;
  }

  Point<float4> worldPosition;
  if (IsDragging() == false || GetEnvironment().GetViewerType()->ConvertMouseToWorld(mousePosition, worldPosition) == false)
  {
    return bIfHandled;
  }

  bool bSkipDone(false);
  vxAnnotation * pAnnotation(m_pDataAC->m_pAnnotation);
  pAnnotation->SetEnvironment(&GetEnvironment());

  // what type of creation mode is used?
  if (m_pDataAC->m_eCreationMode == vxDataAnnotationCreate::WHILE_MOUSE_DOWN)
  {
    vxHandle handle(pAnnotation->GetHandle(pAnnotation->GetNumHandles()-1));
    handle.SetPosition(worldPosition);
    pAnnotation->SetHandle(pAnnotation->GetNumHandles()-1, handle);
    pAnnotation->Update(GetEnvironment());

    GetEnvironment().GetAnnotations()->Add(m_pDataAC->m_pAnnotation);
    m_pDataAC->m_pAnnotation = NULL;

    // allow undo of creation
    vxEnvironment::GetUndoActions().PushAction( new vxUndoActionAnnotationAdd( GetEnvironment().GetAnnotations(), GetEnvironment().GetAnnotations()->GetActiveIndex() ) );

    GetEnvironment().GetAnnotations()->Modified(vxModIndInfo(vxAnnotations, GetEnvironment().GetAnnotations()->GetActiveIndex(), vxUtils::ANNOTATIONS_COMPLETED));
    GetEnvironment().GetInputFeedback().Add(vxFeedback(typeid(*this), vxFeedback::DONE));
    bIfHandled = true;
  } // mode is WHILE_MOUSE_DOWN
  else if (m_pDataAC->m_eCreationMode == vxDataAnnotationCreate::ON_MOUSE_DOWN)
  {
    if (pAnnotation->GetNumCreationHandles() == pAnnotation->GetNumHandles())
    {
      vxHandle handle(pAnnotation->GetHandle(pAnnotation->GetNumHandles()-1));
      handle.SetPosition(worldPosition);
      pAnnotation->SetHandle(pAnnotation->GetNumHandles()-1, handle);
      pAnnotation->Update(GetEnvironment());

      GetEnvironment().GetAnnotations()->Add(m_pDataAC->m_pAnnotation);
      m_pDataAC->m_pAnnotation = NULL;

      // allow undo of creation
      vxEnvironment::GetUndoActions().PushAction( new vxUndoActionAnnotationAdd( GetEnvironment().GetAnnotations(), GetEnvironment().GetAnnotations()->GetActiveIndex() ) );

      GetEnvironment().GetAnnotations()->Modified(vxModIndInfo(vxAnnotations, GetEnvironment().GetAnnotations()->GetActiveIndex(), vxUtils::ANNOTATIONS_COMPLETED));
      GetEnvironment().GetInputFeedback().Add(vxFeedback(typeid(*this), vxFeedback::DONE));
      bIfHandled = true;
    } // all handles set
  } // mode is ON_MOUSE_DOWN
  else if (m_pDataAC->m_eCreationMode == vxDataAnnotationCreate::ON_MOUSE_DOWN_AND_UP)
  {
    if (pAnnotation->GetNumCreationHandles() == pAnnotation->GetNumHandles())
    {
      vxHandle handle(pAnnotation->GetHandle(pAnnotation->GetNumHandles()-1));
      handle.SetPosition(worldPosition);
      pAnnotation->SetHandle(pAnnotation->GetNumHandles()-1, handle);
      pAnnotation->Update(GetEnvironment());

      GetEnvironment().GetAnnotations()->Add(m_pDataAC->m_pAnnotation);
      m_pDataAC->m_pAnnotation = NULL;

      // allow undo of creation
      vxEnvironment::GetUndoActions().PushAction( new vxUndoActionAnnotationAdd( GetEnvironment().GetAnnotations(), GetEnvironment().GetAnnotations()->GetActiveIndex() ) );

      GetEnvironment().GetAnnotations()->Modified(vxModIndInfo(vxAnnotations, GetEnvironment().GetAnnotations()->GetActiveIndex(), vxUtils::ANNOTATIONS_MODIFIED));
      GetEnvironment().GetAnnotations()->Modified(vxModIndInfo(vxAnnotations, GetEnvironment().GetAnnotations()->GetActiveIndex(), vxUtils::ANNOTATIONS_COMPLETED));
      GetEnvironment().GetInputFeedback().Add(vxFeedback(typeid(*this), vxFeedback::DONE));
      bIfHandled = true;
    } // all handles set
    else
    {
      vxHandle handle;
      handle.SetPosition(worldPosition);
      pAnnotation->AddHandle(handle);
      pAnnotation->SetSelectedHandleIndex(pAnnotation->GetNumHandles()-1);
      pAnnotation->Update(GetEnvironment());
      m_pShareableObjectDataAC->Modified(vxModInfoID(vxElement, vxIDs::DataAnnotationCreate, 0));
      bIfHandled = true;
      bSkipDone = true;
    } // not all handles set
  } // mode is ON_MOUSE_DOWN_AND_UP
  else // this annotation has no handle
  {
    bIfHandled = false;
  }

  // did we set the last point? Could be true if we set all points or if it is a "dragging creation"
  if (bSkipDone == false && pAnnotation->GetNumCreationHandles() == pAnnotation->GetNumHandles() || pAnnotation->GetNumCreationHandles() < 0)
  {
    // we are done
    pAnnotation->Morph(GetEnvironment());
    pAnnotation->Update(GetEnvironment());

    // notify the feedback so that manipulator can be removed
    GetEnvironment().GetAnnotations()->Modified(vxModIndInfo(vxAnnotations, GetEnvironment().GetAnnotations()->GetActiveIndex(), vxUtils::ANNOTATIONS_COMPLETED));
    GetEnvironment().GetInputFeedback().Add(vxFeedback(typeid(*this), vxFeedback::DONE));

    pAnnotation->SetSelectedHandleIndex(-1);
    SetDragging(false);
  } // set last handle of annotation?

  return bIfHandled;
} // OnButtonUp()


/**
 * Event handler if a key is pressed
 * @param uKeyId the key pressed
 * @param uKeyboardState key state
 * @return handled?
 */ 
bool vxManipulatorAnnotationCreate::OnKeyPressed(const uint1 uKeyId, 
                                                 const uint4 uKeyboardState)
//*******************************************************************
{
  bool bIfHandled(false);

  if (IsAnnotationSupported() == false)
  {
    return bIfHandled;
  }

  switch(uKeyId)
  {
  case vxInput::KEY_ESCAPE:
    {
      bIfHandled = true;

      // reset manipulator
      m_bDragging = false;

      // reset status of annotation
      m_pDataAC->m_pAnnotation->Reset();
      if (GetEnvironment().GetAnnotations() != NULL)
      {
        vxAnnotation * pAnnotation(m_pDataAC->m_pAnnotation);
        if (pAnnotation != NULL)
        {
          pAnnotation->SetEnvironment(&GetEnvironment());
          m_pDataAC->m_eCreationMode = vxDataAnnotationCreate::ON_MOUSE_DOWN_AND_UP;
          pAnnotation->SetStatus(vxAnnotation::CREATE);
        }
      }
      m_pShareableObjectDataAC->Modified(vxModInfoID(vxElement, vxIDs::DataAnnotationCreate, 0));
    }
    break;

  default:
    break;
  }

  return bIfHandled;

} // OnKeyPressed()


/**
 * Called when the mouse cursor is to be set
 * @return true if cursor was set, false otherwise
 */
bool vxManipulatorAnnotationCreate::OnSetCursor()
//*******************************************************************
{
  if (IsAnnotationSupported())
  {
    m_pDataAC->m_pAnnotation->SetCursor();
  }
  else
  {
    vxMouseCursor::SetCursor("CURSOR_NO");
  }

  return true;
} // OnSetCursor()


/**
 * Checks if the active annotation is supported for the viewer type.
 * @return true if annotation is supported, false otherwise
 */
bool vxManipulatorAnnotationCreate::IsAnnotationSupported() const
//*******************************************************************
{
  // get the rendering mode object
  vxRenderingMode * pMode(GetEnvironment().GetRenderingMode());
  
  // does the current visualization mode support annotations?
  if (GetEnvironment().GetRenderingMode()->GetRenderingAlgorithm() !=  vxRenderingMode::DEFAULT)
  {
    return false;
  }      
  
  // ask the viewer type if the annotation is supported
  return GetEnvironment().GetViewerType()->IsAnnotationSupported(*m_pDataAC->m_pAnnotation);
} // IsAnnotationSupported()


#undef FILE_REVISION


// Revision History:
// $Log: vxManipulatorAnnotationCreate.C,v $
// Revision 1.9  2006/03/02 15:48:42  frank
// properly called static function
//
// Revision 1.8  2006/02/21 21:21:35  frank
// added more undo for annotations
//
// Revision 1.7  2006/02/21 19:45:23  frank
// added undo for annotations
//
// Revision 1.6  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.5  2005/11/18 21:15:22  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.4.2.1  2005/06/17 22:55:43  michael
// Changed code to be calling modified on the actual vxID::vxDataAnnotationCreate)
// and not the vxAnnotations which isn't modified.
//
// Revision 1.4  2005/05/24 13:50:02  michael
// Introduced concept of data objects to start with v3D_Data..../vxData...
// Has been done for DataColormapEditor, DataAnnotationCreate, and
// DataPlaneObliqueCreate
//
// Revision 1.3  2004/10/05 14:30:09  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.2  2004/04/06 16:24:46  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.36  2004/02/07 02:17:35  michael
// fixed issue #3728
//
// Revision 1.35  2004/02/07 01:12:39  michael
// fixed issue #3721
//
// Revision 1.34  2004/02/05 21:37:25  michael
// Annotation is now created even if it is not supported in the current viewer or
// due to the rendering mode. This ensures proper handling in case the creating
// becomes possible due to changes in e.g. the rendering mode.
//
// Revision 1.33  2004/01/23 04:00:18  michael
// adjusted code to reflect changes made in refactoring annotations
//
// Revision 1.32  2004/01/21 16:33:51  michael
// fixed update problem of pane (on creation)
//
// Revision 1.31  2004/01/16 16:52:15  michael
// annotations only get redrawn/updated in the view they are created/adjusted
// it. Only when releasing the mouse (completing the action), the update happens
// to the other viewers, too.
//
// Revision 1.30  2004/01/14 14:18:18  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.29  2004/01/05 19:05:57  michael
// added "mailto:"
//
// Revision 1.28  2003/12/16 19:06:19  frank
// Added additional key definitions
//
// Revision 1.27  2003/11/06 20:44:04  michael
// Fixed rendering mode related issues, renamed the "algorithm" to be the mode,
// and the display options to be the options so that things are consistent. Also
// fixed the progressive refinement that broke because George forgot to add
// the assignment of the mode in vxRenderingMode::operator=()
//
// Revision 1.26  2003/11/05 01:18:46  geconomos
// Updated to use new vxRenderingAlgorithmEnum
//
// Revision 1.25  2003/06/19 20:21:21  michael
// adjusted for proper use of vxUtils::vxEvent...
//
// Revision 1.24  2003/06/04 13:27:30  geconomos
// Disabled annotation support when in MIP, thin slab and X-Ray rendering modes.
//
// Revision 1.23  2003/05/16 13:21:33  frank
// formatting
//
// Revision 1.22  2003/05/16 11:45:25  michael
// code review
//
// Revision 1.21  2003/05/07 11:44:01  michael
// initial code review
//
// Revision 1.20  2003/05/05 14:10:20  michael
// fixed issue #3158
//
// Revision 1.19  2003/04/25 12:09:01  michael
// more code review
//
// Revision 1.18  2003/04/24 20:08:12  michael
// Added SetEnvironment() on the annotation before doing any operation on it
//
// Revision 1.17  2003/03/31 13:50:57  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.16  2003/03/25 15:35:24  geconomos
// Issue # 2949: Call to vxViewerType::IsAnnotationSupported on every OnXXX function.
//
// Revision 1.15  2003/03/04 01:43:24  michael
// fixed 'ESC' on creating annotations
//
// Revision 1.14  2003/02/28 05:44:27  michael
// fixed issue #2860
//
// Revision 1.13  2003/02/26 22:08:23  geconomos
// Added active annotation index to all Modified calls.
//
// Revision 1.12  2003/02/26 19:01:06  michael
// added "EventAdded" when creating the a
//
// Revision 1.11  2003/02/13 17:16:55  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.10  2003/02/07 01:03:53  michael
// added annotation for components
//
// Revision 1.9  2003/02/07 00:29:58  michael
// added annotation for components
//
// Revision 1.8.2.1  2003/02/12 22:51:02  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.8  2003/01/20 19:37:42  michael
// added ellipse annotation
//
// Revision 1.7  2003/01/20 04:52:14  michael
// added first version of box annotation. Not yet completed ...
//
// Revision 1.6  2003/01/15 23:53:49  michael
// added annotation manipulation and handle highlighting
//
// Revision 1.5  2003/01/14 23:36:27  michael
// furter adjustments to support point, arrow, distance and angle
//
// Revision 1.4  2003/01/13 23:20:33  michael
// made first annotation to work in new structure (vxAnnotations)
//
// Revision 1.3  2003/01/13 20:00:20  michael
// fixed bugs in creation of annotations, need to adjust the rendering
//
// Revision 1.2  2003/01/13 14:44:15  geconomos
// Generic OnSetCursor handling.
//
// Revision 1.1  2003/01/09 14:57:53  michael
// added new manipulator to creat the new annotation objects
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorAnnotationCreate.C,v 1.9 2006/03/02 15:48:42 frank Exp $
// $Id: vxManipulatorAnnotationCreate.C,v 1.9 2006/03/02 15:48:42 frank Exp $
