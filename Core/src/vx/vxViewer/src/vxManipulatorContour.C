// $Id: vxManipulatorContour.C,v 1.17.8.1 2009/08/14 16:02:38 kchalupa Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Manjushree Nulkar (mailto:manju@viatronix.com)


// includes
#include "stdafx.h"
#include "vxManipulatorUtils.h"
#include "vxManipulatorContour.h"
#include "vxBlockVolume.h"
#include "vxContourList.h"
#include "vxMouseCursor.h"
#include "vxContour.h"
#include "vxViewerTypeMPROrtho.h"
#include "vxMessageBox.h"
#include "vxUndoActionSegmentation.h"
#include "vxComponentUtils.h"


// defines
#define FILE_REVISION "$Revision: 1.17.8.1 $"


/**
 * Constructor
 * @param environment
 */
vxManipulatorContour::vxManipulatorContour( vxEnvironment & environment )
: vxManipulator( environment ),
m_iLassoIndex( -1 ),
m_bActive( false ),
m_pCurrentLasso( NULL )
{
} // vxManipulatorContour()


/**
 * Destructor
 */
vxManipulatorContour::~vxManipulatorContour()
{
  Clear();
} // ~vxManipulatorContour()


/**
 * Clear out all the data and reset the state to defaults
 */
void vxManipulatorContour::Clear()
{
  GetEnvironment().GetContourList()->Clear();
  GetEnvironment().GetContourList()->Modified( vxModInfo( vxContourList, vxUtils::CONTOUR_MODIFIED ) );
} // Clear()


/**
 * Public method to process button down event
 *
 * @param position mouse position
 * @param eButtonId mouse button enum
 * @param uKeyboardState state of the mouse
 * @return handled
 */ 
bool vxManipulatorContour::OnButtonDown(const Point2D<int2> & mousePosition, 
                                        const ButtonEnum eButtonId, 
                                        const uint4 uKeyboardState )
{
  Point<float4> worldPos;
  const bool bOrthoMPRView(dynamic_cast<vxViewerTypeMPROrtho *>(GetEnvironment().GetViewerType()) != NULL);

  if (bOrthoMPRView == true && eButtonId == BUTTON_LEFT &&
      GetEnvironment().GetWinRect().IsInside(mousePosition) &&
      GetEnvironment().GetViewerType()->ConvertMouseToWorld(mousePosition, worldPos, false))
  {
    // get the contour element
    vxShareableObject<vxContourList> * pContourList( GetEnvironment().GetContourList() );
    
    // check if contour elements are not null.
    if ( pContourList != NULL )
    {
      uint1 uOrientation( GetEnvironment().GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord() );

      // return if the plane orientation has changed.
      if (pContourList->GetLassoList().empty() == false && uOrientation != pContourList->GetOrientation())
      {
        pContourList->GetLassoList().clear();
        pContourList->SetOrientation(uOrientation);
      }  

      m_pCurrentLasso = new vxContour;
      m_pCurrentLasso->SetOrientation(uOrientation);
      m_pCurrentLasso->AddPoint(worldPos);

      // check if a lasso was already created on this slice
      CheckForLassoOnSameSlice(worldPos, *pContourList);

      // put the lasso created at the front temporarily. 
      // will be put in the correct place in OnButtonUp event
      pContourList->GetLassoList().push_front( m_pCurrentLasso );
      pContourList->Modified( vxModInfo( vxContourList, vxUtils::CONTOUR_MODIFIED ) );

      m_bActive = true;
    } // if restricted lasso & cardiac plaques objects are not null
  } // if left button. 

  return m_bActive;
} // OnButtonDown()


/**
 * Event handler if the mouse is moved
 * @param position Relative Mouse position
 * @param uKeyboardState mouse state
 * @return handled?
 */
bool vxManipulatorContour::OnMouseMove(const Point2D<int2> & mousePosition, 
                                       const uint4 uKeyboardState )
{

  bool bHandled( false );

  // if applicable, display warning that this operation cannot be restricted in oblique views
  vxManipulatorUtils::WarnShiftOblique(GetEnvironment(), uKeyboardState);

  // check for a valid point
  Point<float4> worldPos;
  if ( m_bActive == true &&
       GetEnvironment().GetWinRect().IsInside( mousePosition ) &&
       GetEnvironment().GetViewerType()->ConvertMouseToWorld( mousePosition, worldPos, false ) )
  {

    vxShareableObject<vxContourList> * pContourList( GetEnvironment().GetContourList() );

    // make the iterator point to the beginning of the list, to the temporary lasso.
    vxContourList::LassoList::iterator lassoIter( pContourList->GetLassoList().begin() );

    // get the temporarily stored current lasso.
    m_pCurrentLasso = * lassoIter;

    // keep track of the mouse position
    SetMousePosition( mousePosition );

    // add the point to the contour.
    m_pCurrentLasso->AddPoint( worldPos );   

    // Refresh all the views so as to incorporate the changes occurred in the move event.
    pContourList->Modified( vxModInfo( vxContourList, vxUtils::CONTOUR_MODIFIED ) );

    bHandled = true;

  }

  return bHandled;

} // OnMouseMove()


/**
 * Public method to process button down event
 *
 * @param position mouse position
 * @param eButtonId mouse button enum
 * @param uKeyboardState state of the mouse
 * @return handled?
 */ 
bool vxManipulatorContour::OnButtonUp(const Point2D<int2> & mousePosition, 
                                      const ButtonEnum eButtonId, 
                                      const uint4 uKeyboardState )
{
  bool bHandled( false );
  Point<float4> worldPos;

  if ( eButtonId == BUTTON_LEFT && m_bActive &&
       GetEnvironment().GetWinRect().IsInside( mousePosition ) &&
       GetEnvironment().GetViewerType()->ConvertMouseToWorld( mousePosition, worldPos, false ) )
  {

    vxShareableObject<vxContourList> * pContourList( GetEnvironment().GetContourList() );

    // get the temporary lasso stored in the front of the list.
    vxContourList::LassoList::iterator lassoIter( pContourList->GetLassoList().begin() );
    m_pCurrentLasso = * lassoIter;
    
    // add the mouse point to the vector of contour points for this lasso.
    m_pCurrentLasso->AddPoint( worldPos );

    // close the contour  
    m_pCurrentLasso->AddPoint( m_pCurrentLasso->GetPoint(0) );        

    // insert the lasso in the correct position and delete the temporary lasso.
    InsertionSortList( m_pCurrentLasso, * pContourList );

    // refresh all the views so as to incorporate the changes occurred in the move event.
    pContourList->Modified( vxModInfo( vxContourList, vxUtils::CONTOUR_MODIFIED ) );
    
    m_bActive = false;
    bHandled = true;  

  } // if left button

  return bHandled;

} // OnButtonUp()


/**
 * Called when the mouse cursor is to be set
 *
 * @return true if cursor was set, false otherwise
 */
bool vxManipulatorContour::OnSetCursor()
{
  const bool bOrthoMPRView(dynamic_cast< vxViewerTypeMPROrtho * >(GetEnvironment().GetViewerType()) != NULL);

  if (bOrthoMPRView == true)
    vxMouseCursor::SetCursor("CURSOR_RESTRICTIVE_LASSO");
  else
    vxMouseCursor::SetCursor("CURSOR_NO");

  return true;
} // OnSetCursor()


/**
 * Check if a lasso was created on the current slice
 *
 * @param mouseWorldPos mouse position in world coordinates
 * @param pLasso the restricted lasso element
 */
void vxManipulatorContour::CheckForLassoOnSameSlice( Point3D<float4> & mouseWorldPos, vxContourList & pLassoElement )
{
  vxContourList::LassoList::iterator lassoIter = pLassoElement.GetLassoList().begin();

  if ( pLassoElement.GetLassoList().empty() == false )
  {

    for ( uint4 i(0); lassoIter != pLassoElement.GetLassoList().end(); i++, lassoIter++ )
    {
      vxContour * pContour( * lassoIter );
      uint1 uOrientation( pContour->GetOrientation() );
      const Triple<float4> & units( GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits() );

      if ( fabs( pContour->GetPoint(0)[ uOrientation ] - mouseWorldPos[ uOrientation ] ) < units[ uOrientation ] / 2 )
      { 
        pLassoElement.GetLassoList().erase( lassoIter );
        return;
      }
    } // for all lasso elements
  }
} // CheckForLassoOnSameSlice()


/**
 * Insert the newly created lasso in the correct location in the list
 *
 * @param pRoi newly created lasso
 * @param pLassoElement restricted lasso element
 */ 
void vxManipulatorContour::InsertionSortList( vxContour * pContour, vxContourList & pLassoElement )
{
  Point<float4> fPoint1( pContour->GetPoint(0) );
  int2 iOrientation( pContour->GetOrientation() );
  int2 uPos( -1 );
  
  std::list< vxContour * >::iterator iterLasso = pLassoElement.GetLassoList().begin();

  // skip the first lasso, it is the temporary stored one..
  iterLasso++;
  
  uint2 i(1);

  // get the correct position in the list for the newly created lasso.
  while (iterLasso != pLassoElement.GetLassoList().end())
  {
    vxContour *pContour = *(iterLasso);
    if (iOrientation == pContour->GetOrientation())
    { 
      if (fPoint1[iOrientation] < pContour->GetPoint(0)[iOrientation])
      {
        uPos = i;
        break;
      }
    }  
    i++;
    iterLasso++;
  }

  if (uPos == -1)
  {
    // list is empty --> insert at the first position
    pLassoElement.GetLassoList().push_back( pContour );
    pLassoElement.SetOrientation(iOrientation);
  }
  else
  {
    // list is not empty --> insert in the right place
    iterLasso = pLassoElement.GetLassoList().begin();
    std::advance(iterLasso, uPos);
    pLassoElement.GetLassoList().insert(iterLasso, pContour);
  }    

  iterLasso = pLassoElement.GetLassoList().begin();
  pLassoElement.GetLassoList().erase(iterLasso);
  m_pCurrentLasso = NULL;
} // InsertionSortList()


/**
 * Event handler if a key is pressed.
 *
 * @param uKeyId the key pressed.
 * @param uKeyboardState key state.
 * @return handled?
 */ 
bool vxManipulatorContour::OnKeyPressed( const uint1 uKeyId, const uint4 uKeyboardState )
{
  bool bHandled(false);
  vxShareableObject<vxContourList> * pContourList( GetEnvironment().GetContourList() );
  
  switch(uKeyId)
  {
  case vxInput::KEY_ESCAPE:
    // if the button has not been toggled, i.e. the lassos have not been processed yet
    if (pContourList != NULL)
    {
      // clear everything
      Clear();

      // notify the feedback so that manipulator could be removed
      GetEnvironment().GetInputFeedback().Add(vxFeedback(typeid(*this), vxFeedback::DONE));    

      bHandled = true;
    } // if there are any lassos
    break;

  case vxInput::KEY_ENTER:
    // Start processing the list of lassos and create a new plaque.
    CreateNewComponent( uKeyboardState );
    pContourList->Modified( vxModInfo( vxContourList, vxUtils::CONTOUR_MODIFIED ) );
    bHandled = true;
    break;

  default:
    break;
  } // switch

  return bHandled;
} // OnKeyPressed()


/**
 * Get current lasso
 */
void vxManipulatorContour::GetCurrentLasso()
{
  if (GetEnvironment().GetContourList() != NULL)
  {
    vxContourList * pContourList( GetEnvironment().GetContourList() );

    if (pContourList->GetLassoList().empty() == false)
    {
      // get the slice orientation.
      uint4 uIndex(GetEnvironment().GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord());
      float4 fCurPlaneDistance(GetCurrentPlaneDistance());
      
      vxContourList::LassoList::iterator lassoIter = pContourList->GetLassoList().begin();

      for ( int2 i(0); lassoIter != pContourList->GetLassoList().end(); lassoIter++, i++ )
      {
        vxContour * pContour( * lassoIter );
        Point3D<float4> contourPoint( pContour->GetPoint( 0 ) );
        if ( fabs( contourPoint[ uIndex ] - fCurPlaneDistance ) < 0.5F )
        {
          m_iLassoIndex = i;
          break;
        }
      } // for all lasso points
    } // list not empty?
  } // restricted lasso element not null?

} // GetCurrentLasso()


/**
 * Get the current plane distance
 *
 * @return plane distance
 */
float4 vxManipulatorContour::GetCurrentPlaneDistance()
{
  uint4 uIndex(GetEnvironment().GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord());
  Triple<float4> units(static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolume())->GetHeader().GetVoxelUnits());
  Triple<int4> volDim(static_cast<vxBlockVolume<uint2> *>(GetEnvironment().GetIntensityVolume())->GetHeader().GetVolDim());
  uint2 uNumberOfSlices(volDim[uIndex]); 
  uint4 iCurrentSliceIndex(vxUtils::CalculateOffsetDistance(0.5 + GetEnvironment().GetPlaneOrtho()->GetDistance()/units[uIndex],
                                                            uNumberOfSlices, 
                                                            GetEnvironment().GetOffset(), 
                                                            GetEnvironment().GetMaximumOffset()));
  float4 fCurrentSliceIndex(iCurrentSliceIndex * units[uIndex]);
  fCurrentSliceIndex *= units[uIndex];      
  
  return fCurrentSliceIndex;
} // GetCurrentSliceNumber()


/**
 * Copy the selected lasso on the desired slice
 */
void vxManipulatorContour::CopyLasso()
{
  if (m_iLassoIndex != -1)
  {
    if (GetEnvironment().GetContourList() != NULL)
    {
      vxContourList * pContourList = GetEnvironment().GetContourList();
      
      vxContourList::LassoList::iterator lassoIter = pContourList->GetLassoList().begin();
      std::advance( lassoIter, m_iLassoIndex );
      vxContour * pSelectedContour = * lassoIter;
      vxContour * pNewContour = new vxContour;

      // get the slice orientation.
      uint4 uIndex(GetEnvironment().GetPlaneOrtho()->GetNormal().GetMaxAbsComponentCoord());
      // store the slice orientation.
      pNewContour->SetOrientation(uIndex);

      std::list<Point<float4> >::iterator contourIter = pSelectedContour->GetContourPtVector().begin();

      float4 fCurPlaneDistance(GetCurrentPlaneDistance());

      for ( ; contourIter != pSelectedContour->GetContourPtVector().end(); contourIter++ )
      {
        Point<float4> contourPoint(*contourIter);
        contourPoint[uIndex] = fCurPlaneDistance;
        pNewContour->GetContourPtVector().push_back(contourPoint);
      } // end for     
    
      // insert the newly created lasso in the correct position in the list of lassos.
      InsertionSortList( pNewContour, *pContourList );
    } // contour is not null?
  } // stored index is not -1?
} // CopyLasso()


/**
 * Create a new component from the list of lassos
 */
void vxManipulatorContour::CreateNewComponent( const uint4 uKeyboardState )
{
  if (GetEnvironment().GetContourList() != NULL)
  {
    vxContourList * pContourList( GetEnvironment().GetContourList() );
    pContourList->Initialize( GetEnvironment().GetIntensityVolume()->GetHeader().GetVolDim(),
                              GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits() );

    // If the lasso list is not empty
    if (pContourList->GetLassoList().empty() == false)
    {
      // create new component
      vxMouseCursor cursor("CURSOR_WAIT");

      // Process the lasso list to create a bool mask volume used to create a new plaque
      pContourList->ProcessLassoList();

      // save the current state for undo purposes and relinquish control of memory
      vxEnvironment::GetUndoActions().PushAction( new vxUndoActionSegmentation( GetEnvironment().GetLabelVolume(),
        GetEnvironment().GetComponents() ) );

      // restrict the operation to the visible volume if shift is held down
      if ( uKeyboardState & vxInput::STATE_SHIFT )
        vxComponentUtils::CullOutside( * pContourList->GetMaskVolume(), vxUtils::GetActiveBoundingBox( true,
        GetEnvironment().GetIntensityVolume(), GetEnvironment().GetViewerType(), GetEnvironment().GetPlaneOrtho(),
        GetEnvironment().GetRenderingMode(), GetEnvironment().GetOffset(),
        GetEnvironment().GetMaximumOffset(), GetEnvironment().GetCropBox() ) );

      int4 iCount = GetEnvironment().GetComponents()->GetCount();

      // make the component
      vxComponentUtils::CreateNewComponent( * pContourList->GetMaskVolume(),
        vxComponentUtils::GetUniqueComponentName( * GetEnvironment().GetComponents(), "Contour" ), GetEnvironment(), true );

      // let everyone know that the volume has changed
      GetEnvironment().GetLabelVolume()->Modified(vxModInfo(vxBlockVolume<uint2>, vxUtils::VOLUME_LABEL_MODIFIED));

      if(iCount == GetEnvironment().GetComponents()->GetCount())
      {
        GetEnvironment().GetComponents()->Modified(vxModIndInfo(vxComponentArray, 0, vxUtils::COMPONENTARRAY_MODIFIED));
      }
      else
      {
        GetEnvironment().GetComponents()->Modified(vxModIndInfo(vxComponentArray, 0, vxUtils::COMPONENTARRAY_ADDED));
      }

      // notify the feedback so that manipulator could be removed
      GetEnvironment().GetInputFeedback().Add(vxFeedback(typeid(*this), vxFeedback::DONE));    

      // clear the mask volume for next use
      pContourList->GetMaskVolume()->Clear();

    } // lasso is not empty?
  } // contour is not null?
} // CreateNewComponent()


// undefines
#undef FILE_REVISION


// $Log: vxManipulatorContour.C,v $
// Revision 1.17.8.1  2009/08/14 16:02:38  kchalupa
// When append to component is selected, the selected component should not jump to the last one in the list.
//
// Revision 1.17  2006/07/06 12:56:21  frank
// Issue #4840: Refactored component creation and naming method
//
// Revision 1.16  2006/04/14 18:24:32  frank
// Issue #4711: Fixed the unchecked growing into locked regions
//
// Revision 1.15  2006/03/29 20:03:08  geconomos
// binary histogram is now contained within the block info structure, updated accordingly
//
// Revision 1.14  2006/03/13 13:53:44  frank
// formatting
//
// Revision 1.13  2006/03/02 15:48:18  frank
// pulled the environment out of one of the utility functions
//
// Revision 1.12  2006/02/27 15:23:42  frank
// split out component utilities
// added undo for component operations
//
// Revision 1.11  2006/02/17 15:13:58  frank
// working on undoing all segmentation actions
//
// Revision 1.10  2006/02/01 21:22:34  frank
// added undo actions for some segmentation operations
//
// Revision 1.9  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.8  2005/11/18 21:15:22  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.7.2.2  2005/08/26 13:28:24  frank
// bug when appending to background component fixed
//
// Revision 1.7.2.1  2005/07/25 20:10:41  frank
// Issue #4304: Fixed problem when sculpting or contouring locked voxels
//
// Revision 1.7  2001/01/03 19:30:08  michael
// Moved function WarnShiftOblique out of the base class and into vxManipulatorUtils
// where it can be used by the (few) manipulators that need it.
//
// Revision 1.6  2005/03/08 19:09:13  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.5.6.2  2005/02/15 19:50:04  frank
// Issue #3948: Showed proper tooltip for editable and not editable component names
//
// Revision 1.5.6.1  2005/02/15 19:45:28  frank
// Issue #3948: Showed proper tooltip for editable and not editable component names
//
// Revision 1.5  2004/04/06 16:25:11  frank
// unified environment access naming
//
// Revision 1.4  2004/03/31 17:35:03  frank
// unused include files
//
// Revision 1.3  2004/03/12 19:50:34  frank
// removed annotation ROI and readjusted sculpting to remove handle set
//
// Revision 1.2  2004/03/12 18:03:58  frank
// working on removing annotation ROI - first renaming contour to contour list
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.35  2004/01/19 20:31:54  frank
// fixed sculpting in fullscreen mode
//
// Revision 1.34  2004/01/16 21:22:43  frank
// Prevented the oblique-shift warning from flashing as you move the mouse
//
// Revision 1.33  2004/01/14 17:15:51  frank
// cleared the old contour in the destructor
//
// Revision 1.32  2004/01/14 14:18:18  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.31  2004/01/13 13:13:40  frank
// Contouring only works in ortho views
//
// Revision 1.30  2004/01/12 14:19:24  frank
// fixed problem with manipulating on oblique with shift held down
//
// Revision 1.29  2003/12/16 20:22:24  frank
// Issue #3593: Restricted the segmentation operations to the visible volume including crop box and thin slab when using the shift key
//
// Revision 1.28  2003/11/20 18:27:00  frank
// Fixed new component creation procedure
//
// Revision 1.27  2003/07/31 14:02:20  frank
// Issue #3407: Fixed contour drawing on small-unit datasets
//
// Revision 1.26  2003/06/24 19:20:50  frank
// formatting
//
// Revision 1.25  2003/06/19 20:21:22  michael
// adjusted for proper use of vxUtils::vxEvent...
//
// Revision 1.24  2003/06/18 15:19:55  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.23  2003/05/15 20:07:52  frank
// Moving away from old point class
//
// Revision 1.22  2003/05/13 17:42:05  frank
// Issue #3215: Contour broken in some slices
//
// Revision 1.21  2003/05/07 11:44:02  michael
// initial code review
//
// Revision 1.20  2003/04/29 14:13:12  frank
// Moved projection functionality to the viewer type
//
// Revision 1.19  2003/04/08 20:34:23  frank
// Issue #3062: Trying to reproduce problem with buggy contour drawing
//
// Revision 1.18  2003/04/08 18:17:24  frank
// Issued #3060: Preventing operation on oblique views
//
// Revision 1.17  2003/03/31 13:50:57  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.16  2003/02/28 22:09:34  michael
// removed vxWinRect from vxManipulatorr. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.15  2003/02/25 22:11:48  frank
// Fixed problem with pan/zoom window killing selection points
//
// Revision 1.14  2003/02/25 20:40:00  frank
// Fixed mouse jumping to (0,0) when you go off the edge problem.
//
// Revision 1.13  2003/02/25 16:38:18  frank
// Believe to have fixed the contour missing slices
//
// Revision 1.12  2003/01/22 21:56:15  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.11  2002/12/17 14:38:56  michael
// OpenGL back projection now returns bool to indicate success or not
//
// Revision 1.10  2002/12/16 23:47:26  frank
// Removed old comment
//
// Revision 1.9  2002/11/29 21:01:22  geconomos
// Added manipulator to vxInputFeedback list on ESC.
//
// Revision 1.8  2002/11/27 17:37:30  frank
// removed comment
//
// Revision 1.7  2002/11/25 21:06:57  michael
// update mask volume and take this into account
//
// Revision 1.6  2002/11/25 13:44:56  geconomos
// New mouse cursor handling.
//
// Revision 1.5  2002/11/12 21:29:22  geconomos
// Made vxContour  a first class citizen of vxEnvironment (frank really did it).
//
// Revision 1.4  2002/11/12 00:11:54  michael
// accounted for rounding error when converting world plane distance to volume index coordinates
//
// Revision 1.3  2002/11/08 23:58:03  frank
// cleaned out name "restricted lasso"
//
// Revision 1.2  2002/11/08 18:02:58  michael
// fixed world/index coordinates
//
// Revision 1.1  2002/11/06 18:29:38  frank
// Copied RestrictedLasso from CS and renamed Contour
//
// Revision 1.25  2002/10/08 16:42:51  frank
// Issue #2658: Fixed when you click outside the menu
//
// Revision 1.24  2002/10/05 02:37:12  frank
// Issue #2658: Can no longer unassign with the restricted lasso.
//
// Revision 1.23  2002/10/04 19:10:55  frank
// Fixed up the scoring mechanism.  Cleaned up code significantly.
//
// Revision 1.22  2002/10/03 19:12:25  dmitry
// Rendering Mode not concidered when calc world coord.
//
// Revision 1.21  2002/09/30 14:36:59  dmitry
// EVENT_COMPLETED called
//
// Revision 1.20  2002/09/27 21:06:15  frank
// Issue #2699: Added wait cursor during assignment.
//
// Revision 1.19  2002/09/26 18:08:57  dmitry
// Cleaned up mouse input handlers and orientation change handled.
//
// Revision 1.18  2002/09/26 14:51:52  frank
// Moved InitializeData into C++.
//
// Revision 1.17  2002/09/24 22:58:54  geconomos
// Prevented popup menu during marking of bones and noise.
//
// Revision 1.16  2002/09/18 20:17:33  dmitry
// Cursors added.
//
// Revision 1.15  2002/09/16 20:20:39  dmitry
// New vxID (s) used.
//
// Revision 1.14  2002/09/12 03:26:42  frank
// Toggled plaque label display with F12.
//
// Revision 1.13  2002/09/09 21:10:40  dmitry
// point vector changed to list
//
// Revision 1.12  2002/09/05 22:37:48  jaddonisio
// New vxComponent.
//
// Revision 1.11  2002/08/27 16:29:13  dmitry
// SetColor method used
//
// Revision 1.10  2002/08/26 19:30:34  frank
// Positioned pop-up window under mouse.
//
// Revision 1.9  2002/08/26 17:01:15  frank
// Cleaned up dead code.
//
// Revision 1.8  2002/08/22 22:34:25  uday
// update.
//
// Revision 1.7  2002/08/21 23:27:19  uday
// update.
//
// Revision 1.6  2002/08/20 22:44:25  uday
// Manipulator returns true/false  if the event has been handled or not.
//
// Revision 1.5  2002/08/13 15:53:02  dmitry
// Took out unnecessary param from CalculateScoresFreeForm.
//
// Revision 1.4  2002/07/15 15:20:52  geconomos
// no message
//
// Revision 1.3  2002/07/11 21:09:11  manju
// Speed up.
//
// Revision 1.2  2002/07/10 15:13:31  manju
// Added methods to implement copy-paste of a lasso and creation of
// new plaque.
//
// Revision 1.1  2002/07/10 13:49:01  manju
// Initial revision.
//
// Revision 1.3  2002/06/27 12:07:49  manju
// Added OnKeyPressed method.
//
// Revision 1.2  2002/06/26 17:24:58  manju
// Modified the code to get it working,
//
// Revision 1.1  2002/06/19 14:24:20  manju
// Manipulator for restricted lasso.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorContour.C,v 1.17.8.1 2009/08/14 16:02:38 kchalupa Exp $
// $Id: vxManipulatorContour.C,v 1.17.8.1 2009/08/14 16:02:38 kchalupa Exp $
