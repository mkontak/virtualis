// $Id: vxManipulatorAnnotationAdjust.C,v 1.15 2007/06/28 19:33:55 jmeade Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Michael Meissner (mailto:michael@viatronix.com)


// includes
#include "stdafx.h"
#include "vxManipulatorAnnotationAdjust.h"
#include "vxEnvironment.h"
#include "vxAnnotations.h"
#include "vxViewerType.h"
#include "GLFontGeorge.h"
#include "vxHandle.h"
#include "vxViewerTypeMPROrtho.h"
#include "vxViewerTypeMPROblique.h"
#include "vxUndoActionAnnotation.h"


// defines
#define FILE_REVISION "$Revision: 1.15 $"


/**
 * Constructor
 * @param environment
 */
vxManipulatorAnnotationAdjust::vxManipulatorAnnotationAdjust(vxEnvironment & environment):
vxManipulator(environment),
m_bDragging(false)
//*******************************************************************
{
  SetLabelSelected(false);
  SetTextSelected(false);
  SetSelectedHandleIndex(-1);
  SetSelectedHandleObjIndex(-1);
} // constructor


/*
 * Event handler if the mouse is pressed
 * @param mouseCoord screen mouse position
 * @param eButtonId mouse button
 * @param uKeyboardState state of the mouse
 * @return handled?
 */ 
bool vxManipulatorAnnotationAdjust::OnButtonDown(const Point2D<int2> & mouseCoord, 
                                                 const ButtonEnum eButtonId, 
                                                 const uint4 uKeyboardState)
//*******************************************************************
{
  bool bHandled(false);
  
  if (CanAdjust() == false)
  {
    return false;
  }

  // do the modifications only on left mouse button.
  if (eButtonId == BUTTON_LEFT)
  {
    if (GetEnvironment().GetAnnotations() != NULL)
    {
      vxAnnotations * pAnnotations = static_cast<vxAnnotations *>(GetEnvironment().GetAnnotations());
      if (pAnnotations->GetCount() <= 0)
        return bHandled;

      Point<float4> worldPosition;
      if (GetEnvironment().GetViewerType()->ConvertMouseToWorld(mouseCoord, worldPosition) == false)
        return bHandled;

      int iAnnotation(-1), iHandle(-1);
      float4 fMaxDistance(0.5*pAnnotations->Get(0)->GetHandle(0).GetHandleWorldSize());
      float4 fDistance(2*fMaxDistance);

      // find closest handle
      ////////////////////////
      for (int i(0); i<pAnnotations->GetCount(); i++)
      {
        vxHandlesObj * pHandlesObj(pAnnotations->Get(i));
        if (GetEnvironment().GetViewerType()->IsAnnotationSupported(*pHandlesObj))
        {
          if (GetEnvironment().GetViewerType()->IsVisible(*pHandlesObj))
          {
            for (int j(0); j<pHandlesObj->GetNumHandles(); j++)
            {
              const float4 fTmpDistance(worldPosition.GetEuclideanDistance(pHandlesObj->GetHandle(j).GetPosition()));
              if (fTmpDistance < fDistance)
              {
                iAnnotation = i;
                iHandle = j;
                fDistance = fTmpDistance;
              }
            } // for all handles
          } // annotation visible?
        } // annotation supported?
      } // for all annotations

      if (fDistance < fMaxDistance)
      {
        SetDragging(true);
        SetSelectedHandleObjIndex(iAnnotation);
        SetSelectedHandleIndex(iHandle);

        vxAnnotations * pAnnotations = static_cast<vxAnnotations *>(GetEnvironment().GetAnnotations());
        vxAnnotation * pAnnotation(pAnnotations->Get(GetSelectedHandleObjIndex()));
        pAnnotation->SetEnvironment(&GetEnvironment());
        pAnnotation->SetSelectedHandleIndex(GetSelectedHandleIndex());
				
				//Set the Undo action
				vxEnvironment::GetUndoActions().PushAction( new vxUndoActionAnnotationAdjust( GetEnvironment().GetAnnotations(), pAnnotation, pAnnotations->GetActiveIndex() ) );

        vxHandle handle(pAnnotation->GetHandle(GetSelectedHandleIndex()));
        handle.SetPosition(worldPosition);
        pAnnotation->SetHandle(GetSelectedHandleIndex(), handle);

        if (pAnnotation->PerformUpdateWhileDragging() == true)
        {
          pAnnotation->Update(GetEnvironment());
        }

        if (pAnnotations->GetActiveIndex() >= 0)
        {
          pAnnotations->Get(pAnnotations->GetActiveIndex())->SetTextSelected(false);
          pAnnotations->Get(pAnnotations->GetActiveIndex())->SetLabelSelected(false);
        }

        // cache the plane values
        CachePlane();

        pAnnotations->SetActiveIndex(iAnnotation);
        GetEnvironment().GetAnnotations()->Modified(vxModIndInfo(vxAnnotations, iAnnotation, vxUtils::ANNOTATIONS_MODIFIED), true);
        GetEnvironment().GetAnnotations()->Modified(vxModIndInfo(vxAnnotations, iAnnotation, vxUtils::ANNOTATIONS_SELECTED), true);
        bHandled = true;
      } // handle selected?
      else
      {
        Point2D<int2> screenCoord(GetEnvironment().GetViewerType()->ConvertMouseToScreen(mouseCoord));

        // since we didn't find a closest handle, find closest text
        /////////////////////////////////////////////////////////////
        fMaxDistance = 0.5*pAnnotations->Get(0)->GetHandle(0).GetHandleScreenSize();
        fDistance = 2*fMaxDistance;
        bool bLabel(false), bText(false);

        for (int i(0); i<pAnnotations->GetCount(); i++)
        {
          vxAnnotation * pAnnotation(pAnnotations->Get(i));
          if (GetEnvironment().GetViewerType()->IsAnnotationSupported(*pAnnotation))
          {
            if (GetEnvironment().GetViewerType()->IsVisible(*pAnnotation))
            {
              pAnnotation->SetEnvironment(&GetEnvironment());

              // are we within text or label rectangle?
              ///////////////////////////////////////////
              if (pAnnotation->GetLabel().empty() != true)
              {
                Point2D<int2> labelLocation;
                if (GetEnvironment().GetViewerType()->ConvertWorldToScreen(pAnnotation->GetLabelPosition(), labelLocation) == true)
                {
                  const int4 iWidth(GetEnvironment().GetFont()->GetPixelWidth(pAnnotation->GetLabel()));
                  const int4 iHeight(GetEnvironment().GetFont()->GetPixelHeight());
                  const BoundingRect<int2> labelBoundingRect(labelLocation.m_x         , labelLocation.m_y - iHeight, 
                                                             labelLocation.m_x + iWidth, labelLocation.m_y);

                  if (labelBoundingRect.IsInside(screenCoord.X(), screenCoord.Y()))
                  {
                    iAnnotation = i;
                    bLabel = true; 
                    bText  = false;
                  }
                } // within viewport?
              } // label not empty?
              if (pAnnotation->GetText().empty() != true)
              {
                Point2D<int2> textLocation;
                if (GetEnvironment().GetViewerType()->ConvertWorldToScreen(pAnnotation->GetTextPosition(), textLocation) == true)
                {
                  const int4 iWidth(pAnnotation->GetTextLinesWidth());
                  const int4 iHeight(pAnnotation->GetTextLinesHeight());
                  const BoundingRect<int2> textBoundingRect(textLocation.m_x         , textLocation.m_y - iHeight, 
                                                            textLocation.m_x + iWidth, textLocation.m_y);

                  if (textBoundingRect.IsInside(screenCoord.X(), screenCoord.Y()))
                  {
                    iAnnotation = i;
                    bLabel = false; 
                    bText  = true;
                  }
                } // within viewport?
              } // text not empty?
            } // annotation visible?
          } // annotation supported?
        } // for all annotations

        if (bLabel == true || bText == true)
        {
          if (pAnnotations->GetActiveIndex() >= 0)
          {
            pAnnotations->Get(pAnnotations->GetActiveIndex())->SetTextSelected(false);
            pAnnotations->Get(pAnnotations->GetActiveIndex())->SetLabelSelected(false);
            pAnnotations->Get(pAnnotations->GetActiveIndex())->SetSelectedHandleIndex(-1);
          }

          pAnnotations->SetActiveIndex(iAnnotation);
          pAnnotations->Get(pAnnotations->GetActiveIndex())->SetTextSelected(bText);
          pAnnotations->Get(pAnnotations->GetActiveIndex())->SetLabelSelected(bLabel);

          SetLabelSelected(bLabel);
          SetTextSelected(bText);
          SetDragging(bLabel | bText);
          SetSelectedHandleObjIndex(iAnnotation);

          // cache the plane values
          CachePlane();

          GetEnvironment().GetAnnotations()->Modified(vxModIndInfo(vxAnnotations, iAnnotation, vxUtils::ANNOTATIONS_MODIFIED), true);
          GetEnvironment().GetAnnotations()->Modified(vxModIndInfo(vxAnnotations, iAnnotation, vxUtils::ANNOTATIONS_SELECTED), true);

          bHandled = true;
        } // any text selected?
      } // else no handle selected
    } // annotations pointer not NULL?
  } // left button?

  return bHandled;
} // OnButtonDown()


/**
 * Cache Plane
 */ 
void vxManipulatorAnnotationAdjust::CachePlane()
//*******************************************************************
{
  if (dynamic_cast<vxViewerTypeMPROrtho *>(GetEnvironment().GetViewerType()) != NULL)
    SetPlaneOrtho(GetEnvironment().GetPlaneOrtho());

  if (dynamic_cast<vxViewerTypeMPROblique *>(GetEnvironment().GetViewerType()) != NULL)
    SetPlaneOblique(GetEnvironment().GetPlaneOblique());

  return;
}


/**
 * Discard Selection
 * @return discard?
 */ 
bool vxManipulatorAnnotationAdjust::DiscardSelection()
//*******************************************************************
{
  bool bDiscard(false);

  if (dynamic_cast<vxViewerTypeMPROrtho *>(GetEnvironment().GetViewerType()) != NULL)
    if (GetPlaneOrtho() != *static_cast<vxPlaneOrtho<float4> *>(GetEnvironment().GetPlaneOrtho()))
      bDiscard = true;

  if (dynamic_cast<vxViewerTypeMPROblique *>(GetEnvironment().GetViewerType()) != NULL)
    if (GetPlaneOblique() != *static_cast<vxPlaneOblique<float4> *>(GetEnvironment().GetPlaneOblique()))
      bDiscard = true;

  if (bDiscard == true)
  {
    vxAnnotations * pAnnotations = static_cast<vxAnnotations *>(GetEnvironment().GetAnnotations());
    vxAnnotation * pAnnotation(pAnnotations->Get(GetSelectedHandleObjIndex()));
    pAnnotation->SetEnvironment(&GetEnvironment());
    pAnnotation->SetSelectedHandleIndex(-1);
    pAnnotation->Update(GetEnvironment());
    GetEnvironment().GetAnnotations()->Modified(vxModIndInfo(vxAnnotations, GetSelectedHandleObjIndex(), vxUtils::ANNOTATIONS_MODIFIED));
    SetDragging(false);
    SetSelectedHandleIndex(-1);
    SetSelectedHandleObjIndex(-1);
  }

  return bDiscard;
}


/**
 * Event handler if the mouse is moved
 * @param position Relative Mouse position
 * @param uKeyboardState mouse state
 * @return handled?
 */ 
bool vxManipulatorAnnotationAdjust::OnMouseMove(const Point2D<int2> & mouseCoord, 
                                                const uint4 uKeyboardState)
//*******************************************************************
{
  bool bHandled(false);
  
  if (CanAdjust() == false)
  {
    return false;
  }

	 vxAnnotations * pAnnotations = static_cast<vxAnnotations *>(GetEnvironment().GetAnnotations());
	 if ( pAnnotations == NULL || pAnnotations->GetCount() <= 0 )
			return false;

   Point<float4> worldPosition;
   if (GetEnvironment().GetViewerType()->ConvertMouseToWorld(mouseCoord, worldPosition) == false)
      return bHandled;

  if (IsDragging() == true)
  {
    if (DiscardSelection() == true)
      return bHandled;
   
    vxAnnotation * pAnnotation(pAnnotations->Get(GetSelectedHandleObjIndex()));
    pAnnotation->SetEnvironment(&GetEnvironment());

    if (IsLabelSelected() == true)
    {
      pAnnotation->SetLabelPosition(worldPosition);
    }
    else if (IsTextSelected() == true)
    {
      pAnnotation->SetTextPosition(worldPosition);
    }
    else
    {

      pAnnotation->SetSelectedHandleIndex(GetSelectedHandleIndex());
      vxHandle handle(pAnnotation->GetHandle(GetSelectedHandleIndex()));
      handle.SetPosition(worldPosition);
      pAnnotation->SetHandle(GetSelectedHandleIndex(), handle);
      if (pAnnotation->PerformUpdateWhileDragging() == true)
        pAnnotation->Update(GetEnvironment());
      GetEnvironment().GetAnnotations()->Modified(vxModIndInfo(vxAnnotations, GetSelectedHandleObjIndex(), vxUtils::ANNOTATIONS_MODIFIED), true);
    }
  } // dragging mouse?
	else  //Else if the mouse selection is on annotation Display the Handles
	{
		vxAnnotation * pAnnotation = NULL;
		for ( int i =0; i < pAnnotations->GetCount(); i++ )
  	{
			pAnnotation = (vxAnnotation*)pAnnotations->Get(i);
			if ( pAnnotation->GetBoundingBox().IsInside( static_cast<Triplef>(worldPosition) ))
			{
				pAnnotations->SetActiveIndex( i );
     		pAnnotation->SetDrawHandles( true );
				break;
			}
			else
			{
				pAnnotation->SetDrawHandles( false );
			}
		}

  // TODO: keep on eye on this. Is it needed?
	/*	if ( pAnnotation != NULL )
		{
			pAnnotation->SetEnvironment(&GetEnvironment());
			pAnnotation->Update(GetEnvironment());
		}*/
	}

  return bHandled;
} // OnMouseMove()


/**
 * Event handler if the mouse is released
 * @param position mouse position
 * @param eButtonId mouse button
 * @param uKeyboardState mouse state
 * @return handled?
 */ 
bool vxManipulatorAnnotationAdjust::OnButtonUp(const Point2D<int2> & mouseCoord, 
                                               const ButtonEnum eButtonId, 
                                               const uint4 uKeyboardState)
//*******************************************************************
{
  bool bHandled(false);
  
  if (CanAdjust() == false)
  {
    return false;
  }


  if (eButtonId == BUTTON_LEFT)
  {
    if (IsDragging() == true)
    {
      if (DiscardSelection() == true)
        return bHandled;

      vxAnnotations * pAnnotations = static_cast<vxAnnotations *>(GetEnvironment().GetAnnotations());
      vxAnnotation * pAnnotation(pAnnotations->Get(GetSelectedHandleObjIndex()));
      pAnnotation->SetEnvironment(&GetEnvironment());

      if (IsDragging() == true && IsLabelSelected() == false && IsTextSelected() == false)
      {
        pAnnotation->SetSelectedHandleIndex(-1);
        pAnnotation->Update(GetEnvironment());
        GetEnvironment().GetAnnotations()->Modified(vxModIndInfo(vxAnnotations, GetSelectedHandleObjIndex(), vxUtils::ANNOTATIONS_MODIFIED));
        SetDragging(false);
        SetSelectedHandleIndex(-1);
        SetSelectedHandleObjIndex(-1);
      }
      else if (IsLabelSelected() == true)
      {
        pAnnotation->SetLabelSelected(false);
        SetLabelSelected(false);
        SetDragging(false);
      }
      else if (IsTextSelected() == true)
      {
        pAnnotation->SetTextSelected(false);
        SetTextSelected(false);
        SetDragging(false);
      }
    } // is dragging?

  } // left mouse?

  return bHandled;
} // OnButtonUp()


/**
 * Called when the mouse cursor is to be set
 * @return true if cursor was set, false otherwise
 */
bool vxManipulatorAnnotationAdjust::OnSetCursor()
//*******************************************************************
{
 /* if( CanAdjust() )
    vxMouseCursor::SetCursor("CURSOR_ADJUST");
  else
    vxMouseCursor::SetCursor("CURSOR_NO");*/

  return false;
} // OnSetCursor()


/**
 * Indicates if adjustment is allowed.
 * @return true if adjustment is allowed; false otherwise.
 */
bool vxManipulatorAnnotationAdjust::CanAdjust() const
//*******************************************************************
{
  // get the rendering mode object
  vxRenderingMode * pMode(GetEnvironment().GetRenderingMode());

  // does the current visualization mode support annotations?
  if (GetEnvironment().GetRenderingMode()->GetRenderingAlgorithm() !=  vxRenderingMode::DEFAULT)
  {
    return false;
  }      
  
  return true;
} // CanAdjust()


/**
 * Event handler if key is released
 * @param uKeyId         the key that was pressed
 * @param uKeyboardState the keyboard state
 * @return true if handled; false otherwise
 */ 
bool vxManipulatorAnnotationAdjust::OnKeyReleased(const uint1 uKeyId, const uint4 uKeyboardState)
{
	if ( uKeyId == VK_DELETE )
  {
  	//delete the selected annotation.
		vxAnnotations * pAnnotations = static_cast<vxAnnotations *>(GetEnvironment().GetAnnotations());
		if ( pAnnotations == NULL || pAnnotations->GetCount() <= 0 || pAnnotations->GetActiveIndex() < 0 )
			return false;
	
		//Set the Undo action
		vxEnvironment::GetUndoActions().PushAction( new vxUndoActionAnnotationRemove( GetEnvironment().GetAnnotations(), pAnnotations->GetActive( ) ) );

		GetEnvironment().GetAnnotations()->Modified(vxModIndInfo(vxAnnotations, GetSelectedHandleObjIndex(), vxUtils::ANNOTATIONS_REMOVED ));
		pAnnotations->Remove( pAnnotations->GetActiveIndex() );
  }

  return false;
} // OnKeyReleased()


#undef FILE_REVISION


// Revision History:
// $Log: vxManipulatorAnnotationAdjust.C,v $
// Revision 1.15  2007/06/28 19:33:55  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.14.2.1  2007/03/19 14:47:59  geconomos
// removed unnecessary call to annotation.update during mouse move (I hope it is unnecessary! )
//
// Revision 1.14  2006/08/08 04:33:20  romy
// Undo action added for key press delete
//
// Revision 1.13  2006/08/08 04:06:56  romy
// undo code cleaned
//
// Revision 1.12  2006/08/07 20:26:23  romy
// added vxUndoActionAnnotationAdjust
//
// Revision 1.11  2006/08/07 18:48:54  romy
// Undo added for adjust
//
// Revision 1.10  2006/08/04 15:53:28  romy
// added the delete Key Press
//
// Revision 1.9  2006/08/03 22:18:48  romy
// added the delete Key Press - testing
//
// Revision 1.8  2006/08/03 21:33:30  romy
// removed the AnnotationAdjust tool
//
// Revision 1.7  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.6  2005/11/18 21:15:22  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.5.2.1  2005/08/01 19:12:07  frank
// changed function name according to coding standards
//
// Revision 1.5  2005/03/08 19:09:13  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.4.2.1  2005/02/16 20:28:16  michael
// Fixed issue #3964 by checking the plane object while dragging so that in case
// the plane changes in the meantime, the dragging of a handle/text/label is
// suspended.
//
// Revision 1.4  2004/10/05 14:30:09  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.3  2004/04/06 16:24:32  frank
// unified environment access naming
//
// Revision 1.2  2004/03/31 17:45:47  frank
// unused include files
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.35  2004/02/07 02:17:35  michael
// fixed issue #3728
//
// Revision 1.34  2004/01/23 04:00:18  michael
// adjusted code to reflect changes made in refactoring annotations
//
// Revision 1.33  2004/01/21 16:34:15  michael
// function coments adjustments
//
// Revision 1.32  2004/01/16 16:52:15  michael
// annotations only get redrawn/updated in the view they are created/adjusted
// it. Only when releasing the mouse (completing the action), the update happens
// to the other viewers, too.
//
// Revision 1.31  2004/01/14 15:43:20  michael
// renamed variables to better represent the different coordinates
//
// Revision 1.30  2004/01/14 14:18:18  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.29  2004/01/05 15:54:22  wenli
// Add GetBoundingRect() to slove the ssue #3167: missing ablility to move the annotation text in curved
//
// Revision 1.28  2003/11/11 01:55:55  michael
// added to return true in case we select an annotation handle or text
//
// Revision 1.27  2003/11/06 20:44:03  michael
// Fixed rendering mode related issues, renamed the "algorithm" to be the mode,
// and the display options to be the options so that things are consistent. Also
// fixed the progressive refinement that broke because George forgot to add
// the assignment of the mode in vxRenderingMode::operator=()
//
// Revision 1.26  2003/11/05 01:18:45  geconomos
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
// Revision 1.22  2003/05/07 11:44:01  michael
// initial code review
//
// Revision 1.21  2003/05/05 14:10:20  michael
// fixed issue #3158
//
// Revision 1.20  2003/04/25 12:09:01  michael
// more code review
//
// Revision 1.19  2003/04/24 20:08:13  michael
// Added SetEnvironment() on the annotation before doing any operation on it
//
// Revision 1.18  2003/03/31 13:50:57  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.17  2003/03/18 21:26:23  frank
// Distinguished between supporting specific annotations and supporting specific manipulators for viewer types. IsSupported() has become IsAnnotationSupported().
//
// Revision 1.16  2003/03/04 01:43:06  michael
// renaming of methods removing the double occurence of "Annotation", e.g.
// vxAnnotations::GetActiveAnnotation() --> vxAnnotations::GetActive()
//
// Revision 1.15  2003/02/26 22:08:23  geconomos
// Added active annotation index to all Modified calls.
//
// Revision 1.14  2003/02/22 02:02:18  michael
// fixed crash after loading session when adjusting text: manipulator needs to
// set the current environment on annotation so that the GLFontGeorge can
// be accessed.
//
// Revision 1.13  2003/02/13 17:16:54  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.12  2003/02/07 01:03:53  michael
// added annotation for components
//
// Revision 1.11.2.1  2003/02/12 22:51:02  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.11  2003/01/28 17:02:05  michael
// fixed statistics of box annotation
//
// Revision 1.10  2003/01/27 23:18:49  michael
// added annotation stuff: flexible text location (handle index), multiple text lines etc.
//
// Revision 1.9  2003/01/22 18:00:08  michael
// fixed text selection of annotations
//
// Revision 1.8  2003/01/22 15:08:08  michael
// fixed selection of annotations ... still to be continued when 'ESC' is hit
//
// Revision 1.7  2003/01/20 19:37:42  michael
// added ellipse annotation
//
// Revision 1.6  2003/01/17 23:22:58  michael
// fixed basic annotation, minor issue in angle still to be done
//
// Revision 1.5  2003/01/17 17:39:40  michael
// text of annotations can now be selected and moved
//
// Revision 1.4  2003/01/17 00:33:22  michael
// further iteration on new annotations. Text now shows up and there is a utility
// in the viewerType that allows to pass in the OpenGL parameters in case the
// current one should not be used, e.g. as in render text of annotations. Also extended
// the glFontGeorge class to return width and height of the given string
//
// Revision 1.3  2003/01/15 23:53:49  michael
// added annotation manipulation and handle highlighting
//
// Revision 1.2  2003/01/14 23:36:27  michael
// furter adjustments to support point, arrow, distance and angle
//
// Revision 1.1  2003/01/14 23:25:07  michael
// added initial version of annotation adjust manipulator
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorAnnotationAdjust.C,v 1.15 2007/06/28 19:33:55 jmeade Exp $
// $Id: vxManipulatorAnnotationAdjust.C,v 1.15 2007/06/28 19:33:55 jmeade Exp $
