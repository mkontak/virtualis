// $Id: vxManipulatorPlaneObliqueCreate.C,v 1.9 2007/02/05 15:29:25 geconomos Exp $
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
#include "vxManipulatorPlaneObliqueCreate.h"
#include "vxEnvironment.h"
#include "vxDataPlaneObliqueCreate.h"
#include "vxCursor3d.h"
#include "Matrix.h"


// defines
#define FILE_REVISION "$Revision: 1.9 $"


/**
 * Constructor.
 * @param environment
 */
vxManipulatorPlaneObliqueCreate::vxManipulatorPlaneObliqueCreate(vxEnvironment & environment)
:vxManipulator(environment),
m_bDragging(false)
//*******************************************************************
{
  m_pShareableObjectDataPOC = dynamic_cast<vxShareableObject<vxDataPlaneObliqueCreate> *>(GetEnvironment().GetElement(vxIDs::DataPlaneObliqueCreate));
  m_pDataPOC = static_cast<vxDataPlaneObliqueCreate *>(m_pShareableObjectDataPOC);

  if (m_pDataPOC == NULL)
  {
    throw ex::InvalidDataException(LogRec("Data object not in environment", "vxManipulatorPlaneObliqueCreate", "vxManipulatorPlaneObliqueCreate"));
  }
  else
  {
    if ( GetEnvironment().GetActiveEnvironment() == & GetEnvironment() )
    {
      // well, it is the right viewer, so store the data that is needed
      m_pDataPOC->m_pManipulationEnvironment = NULL;
      m_pDataPOC->m_pEnvironment = &environment;
      m_pDataPOC->m_cameraOriginal = *static_cast<vxCamera *>(environment.GetCamera());
      m_pDataPOC->m_planeObliqueOriginal = *static_cast<vxPlaneOblique<float4> *>(environment.GetPlaneOblique());
      m_pDataPOC->m_bCreating = true;
      m_pDataPOC->m_uNumPointsSet = 0;
    }
  }
} // constructor


/**
 * Destructor
 */
vxManipulatorPlaneObliqueCreate::~vxManipulatorPlaneObliqueCreate()
//*******************************************************************
{
  if ( GetEnvironment().GetActiveEnvironment() == & GetEnvironment() )
  {
    // well, clear out the drawing of the line since we are finished
    m_pDataPOC->m_uNumPointsSet = 0;
    m_pShareableObjectDataPOC->Modified(vxModInfoID(vxElement, vxIDs::DataPlaneObliqueCreate, 0));
  }
} // destructor

  
/**
 * Event handler if key is pressed
 * @param uKeyId key pressed
 * @param uKeyboardState state of key pressed
 */ 
bool vxManipulatorPlaneObliqueCreate::OnKeyPressed(const uint1 uKeyId, 
                                                   const uint4 uKeyboardState)
//*******************************************************************
{  
  bool bHandled(false);

  if (uKeyId == 27) // Esc
  {
    SetDragging(false);

    if (m_pDataPOC->m_iDraggingIndex != -1)
    {
      m_pDataPOC->m_vHandles[m_pDataPOC->m_iDraggingIndex].SetSelected(false);
    }
    m_pDataPOC->m_iDraggingIndex = -1;

    // restore plane
    *m_pDataPOC->m_pEnvironment->GetCamera()       = m_pShareableObjectDataPOC->m_cameraOriginal;
    *m_pDataPOC->m_pEnvironment->GetPlaneOblique() = m_pShareableObjectDataPOC->m_planeObliqueOriginal;

    m_pDataPOC->m_pManipulationEnvironment = NULL;
    m_pDataPOC->m_bCreating = true;
    m_pDataPOC->m_uNumPointsSet = 0;

    m_pDataPOC->m_pEnvironment->GetPlaneOblique()->Modified(vxModInfo(vxPlaneOblique<float4>, vxUtils::PLANE_MODIFIED));
    m_pDataPOC->m_pEnvironment->GetCamera()->Modified(vxModInfo(vxCamera, vxUtils::CAMERA_MODIFIED));
    m_pShareableObjectDataPOC->Modified(vxModInfoID(vxElement, vxIDs::DataPlaneObliqueCreate, 0));
  }

  return bHandled;
} // OnKeyPressed()


/**
 * Event handler if mouse button is held down.
 * @param mousePosition  const mouse position
 * @param eButtonId      const eButton the mouse button that is held down
 * @param uKeyboardState const uState state of mouse button
 */ 
bool vxManipulatorPlaneObliqueCreate::OnButtonDown(const Point2D<int2> & mouseCoordinate, 
                                                   const ButtonEnum eButtonId,
                                                   const uint4 uKeyboardState)
//*******************************************************************
{
  bool bHandled(false);

  if (m_pDataPOC->m_pEnvironment == &GetEnvironment())
  {
    return bHandled;
  }

  if (eButtonId == BUTTON_LEFT)
  {
    // try dragging in case we are close to any handle
    if (m_pDataPOC->m_pManipulationEnvironment == &GetEnvironment() && m_pDataPOC->m_uNumPointsSet == 2)
    {
      // get mouse world position
      Point<float4> worldPosition;
      if (GetEnvironment().GetViewerType()->ConvertMouseToWorld(mouseCoordinate, worldPosition) == false)
        return bHandled;

      // find closest handle
      int iIndex(-1);
      float4 fMaxDistance(0.5*m_pDataPOC->m_vHandles[0].GetHandleWorldSize());
      float4 fDistance(2*fMaxDistance);

      for (int i(0); i<2; i++)
      {
        vxHandle * pHandle(&m_pDataPOC->m_vHandles[i]);
        //if (GetEnvironment().GetViewerType()->IsVisible(*pHandle))
        {
          const float4 fTmpDistance(worldPosition.GetEuclideanDistance(pHandle->GetPosition()));
          if (fTmpDistance < fDistance)
          {
            iIndex = i;
            fDistance = fTmpDistance;
          }
        } // annotation visible?
      } // for all annotations

      if (fDistance < fMaxDistance)
      {
        SetDragging(true);
        m_pDataPOC->m_iDraggingIndex = iIndex;
        m_pDataPOC->m_vHandles[iIndex].SetPosition(worldPosition);
        m_pDataPOC->m_vHandles[iIndex].SetSelected(true);
        m_pShareableObjectDataPOC->Modified(vxModInfoID(vxElement, vxIDs::DataPlaneObliqueCreate, 0));

        GetEnvironment().SetUserInteracting( true );
        bHandled = true;
      }
  
      return bHandled;
    } // line already there?

    // ok, we are not dragging so create new lines
    Point3Df worldPos0, worldPos1;
    Point2D<int2> mousePosClamped(GetEnvironment().GetWinRect().Clamp(mouseCoordinate));

    if (GetEnvironment().GetViewerType()->ConvertMouseToWorld(mousePosClamped, worldPos0) == false)
    {
      LogDbg("Weired, should be valid ....", "vxManipulatorPlaneObliqueCreate", "OnButtonDown()");
      return false;
    }

    Point2D<int2> mousePosNeighbour(GetEnvironment().GetWinRect().Clamp(mousePosClamped + Point2D<int2>(2,0)));

    // are we at the right border of the window rectangle?
    if (mousePosNeighbour == mousePosClamped)
    {
      mousePosNeighbour = GetEnvironment().GetWinRect().Clamp(mousePosClamped + Point2D<int2>(-2,0));
    }

    if (GetEnvironment().GetViewerType()->ConvertMouseToWorld(mousePosNeighbour, worldPos1) == false)
    {
      LogDbg("Weired, at least one neighbouring mouse position should be valid ....", "vxManipulatorPlaneObliqueCreate", "OnButtonDown()");
      return false;
    }

    SetDragging(true);
    m_pDataPOC->m_iDraggingIndex = 1;
    m_pDataPOC->m_vHandles[1].SetSelected(true);

    m_pDataPOC->m_pManipulationEnvironment = &GetEnvironment();
    m_pDataPOC->m_vHandles[0].SetPosition(worldPos0);
    m_pDataPOC->m_vHandles[1].SetPosition(worldPos1);
    m_pDataPOC->m_bCreating = true;
    m_pDataPOC->m_uNumPointsSet = 2;

    CreatePlaneObliqueAndCamera(*m_pDataPOC);

    // finally, call modified on the plane as well as the camera
    //////////////////////////////////////////////////////////////
    m_pDataPOC->m_pEnvironment->GetPlaneOblique()->Modified(vxModInfo(vxPlaneOblique<float4>, vxUtils::PLANE_MODIFIED));
    m_pDataPOC->m_pEnvironment->GetCamera()->Modified(vxModInfo(vxCamera, vxUtils::CAMERA_MODIFIED));
    m_pShareableObjectDataPOC->Modified(vxModInfoID(vxElement, vxIDs::DataPlaneObliqueCreate, 0));

    GetEnvironment().SetUserInteracting( true );
  }

  return bHandled;
} // OnButtonDown()


/**
 * Event handler if mouse is moved.
 * @param const mousePosition mouse position.
 * @param const uState state of mouse button.
 */ 
bool vxManipulatorPlaneObliqueCreate::OnMouseMove(const Point2D<int2> & mousePosition, 
                                                  const uint4 uKeyboardState)
//*******************************************************************
{
  bool bHandled(false);

  if (IsDragging() == true)
  {
    if (GetEnvironment().GetWinRect().IsInside(mousePosition) == true)
    {
      Point3Df worldPos;

      if (GetEnvironment().GetViewerType()->ConvertMouseToWorld(mousePosition, worldPos) == false)
        return false;

      if (worldPos != m_pDataPOC->m_vHandles[(m_pDataPOC->m_iDraggingIndex+1)%2].GetPosition())
      {
        m_pDataPOC->m_vHandles[m_pDataPOC->m_iDraggingIndex].SetPosition(worldPos);
        CreatePlaneObliqueAndCamera(*m_pDataPOC);

        // finally, call modified on the plane as well as the camera
        //////////////////////////////////////////////////////////////
        m_pDataPOC->m_pEnvironment->GetPlaneOblique()->Modified(vxModInfo(vxPlaneOblique<float4>, vxUtils::PLANE_MODIFIED));
        m_pDataPOC->m_pEnvironment->GetCamera()->Modified(vxModInfo(vxCamera, vxUtils::CAMERA_MODIFIED));
        m_pShareableObjectDataPOC->Modified(vxModInfoID(vxElement, vxIDs::DataPlaneObliqueCreate, 0));
      }
    }
  }

  return bHandled;
} // OnMouseMove()


/**
 * Event handler if mouse button is released.
 * @param mousePosition  const position mouse position.
 * @param eButtonId      const eButtonId the mouse button that is released.
 * @param uKeyboardState const uKeyboardState state of mouse button.
 */ 
bool vxManipulatorPlaneObliqueCreate::OnButtonUp(const Point2D<int2> & mousePosition, 
                                                 const ButtonEnum eButtonId,
                                                 const uint4 uKeyboardState)
//*******************************************************************
{
  bool bHandled(false);

  if (eButtonId == BUTTON_LEFT)
  {
    if (IsDragging() == true)
    {
      m_pDataPOC->m_vHandles[0].SetSelected(false);
      m_pDataPOC->m_vHandles[1].SetSelected(false);
      m_pDataPOC->m_iDraggingIndex = -1;
      SetDragging(false);

      // need to call modified on the other environment so that the selected handle gets redrawn as unselected
      m_pShareableObjectDataPOC->Modified(vxModInfoID(vxElement, vxIDs::DataPlaneObliqueCreate, 0));

      GetEnvironment().SetUserInteracting( false );
    }
  }

  return bHandled;
} // OnButtonUp()


/**
 * Create a new oblique view based on two points.
 * @param data plane oblique creation data
 */
void vxManipulatorPlaneObliqueCreate::CreatePlaneObliqueAndCamera(vxDataPlaneObliqueCreate & data)
//*******************************************************************
{
  // compute screen space angle
  ///////////////////////////////
  Point2D<int2> screen0, screen1;
  GetEnvironment().GetViewerType()->ConvertWorldToScreen(data.m_vHandles[0].GetPosition(), screen0);
  GetEnvironment().GetViewerType()->ConvertWorldToScreen(data.m_vHandles[1].GetPosition(), screen1);

  // invert screen Y to fix wrong assumption that mouse and screen are same coordinate system (HACK!!!)
  screen0.m_y = GetEnvironment().GetWinRect().GetViewportHeight() - 1 - screen0.m_y;
  screen1.m_y = GetEnvironment().GetWinRect().GetViewportHeight() - 1 - screen1.m_y;

  float4 fAngleScreen(0.0F);
  Vector3Df vectorScreen(screen1.X() - screen0.X(), screen1.Y() - screen0.Y(), 0);
  if (vectorScreen.GetLength() != 0.0)
  {
    Normal3Df normalScreen(vectorScreen);
    Normal3Df normalScreenX(1, 0, 0);
    fAngleScreen = acos(normalScreen.Dot(normalScreenX)) * 180.0F/M_PI;
    if (normalScreen.Y() < 0)
    {
      fAngleScreen *= -1;
    }
  }

  // compute angle in world space
  /////////////////////////////////
  const vxPlaneOblique<float4> parallelPlane(GetEnvironment().GetCamera()->GetViewDirection(), data.m_vHandles[0].GetPosition());
  const Point3Df pointOnPlane(data.m_vHandles[1].GetPosition() - parallelPlane.GetDistance(data.m_vHandles[1].GetPosition()) * parallelPlane.GetNormal());
  const Vector3Df vector0to1(data.m_vHandles[1].GetPosition() - data.m_vHandles[0].GetPosition());
  const Normal3Df normal0to1(vector0to1);
  const Normal3Df normal0to1projected(pointOnPlane - data.m_vHandles[0].GetPosition());
  float4 fAngleWorld;
  if (fabs(parallelPlane.GetDistance(data.m_vHandles[1].GetPosition())) < 0.001)
    fAngleWorld = 0.0F;
  else
    fAngleWorld = acos(normal0to1.Dot(normal0to1projected)) * 180.0F/M_PI;
  if (parallelPlane.GetDistance(data.m_vHandles[1].GetPosition()) < 0.0F)
    fAngleWorld *= -1;

  // initialize camera
  //////////////////////
  vxCamera camera(*data.m_pEnvironment->GetCamera());
  camera.SetEye(Point3Df(0,0,0));
  camera.SetViewReference(camera.GetEye() + (Vector3Df)GetEnvironment().GetCamera()->GetViewDirection());
  camera.SetViewUp(GetEnvironment().GetCamera()->GetViewUp());
  Normal3Df viewUp(camera.GetViewUp());
  Normal3Df viewRight(camera.GetViewRight());
  Normal3Df viewDir(camera.GetViewDirection());

  // rotate camera around view direction (screen space angle) 
  // and along view right by -90 (face new oblique plane)
  /////////////////////////////////////////////////////////////
  Matrix<float4> matrixAll, matrixRotateDir, matrixRotateUp, matrixRotateRight;
  matrixRotateDir.Identity();
  matrixRotateDir.Rotate(viewDir, -fAngleScreen * M_PI/180.0F);
  matrixRotateRight.Identity();
  matrixRotateRight.Rotate(viewRight, static_cast< float4 >( -90.0F * M_PI / 180.0F ) );
  matrixAll.Identity();
  matrixAll = matrixRotateDir * matrixRotateRight;
  viewUp    = matrixAll * viewUp;
  viewDir   = matrixAll * viewDir;
  viewRight = matrixAll * viewRight;

  // rotate camera around view direction to account for 
  // possibly varying depth of the two selected points 
  /////////////////////////////////////////////////////////////
  matrixRotateDir.Identity();
  matrixRotateDir.Rotate(viewDir, -fAngleWorld * M_PI/180.0F);
  viewUp    = matrixRotateDir * viewUp;
  viewDir   = matrixRotateDir * viewDir;
  viewRight = matrixRotateDir * viewRight;

  // set camera parameters accordingly
  //////////////////////////////////////
  Point3Df center(data.m_vHandles[0].GetPosition() + 0.5 * vector0to1);
  camera.SetViewReference(center);
  camera.SetEye(center - (Vector3Df)viewDir);
  camera.SetViewUp(viewUp);

  // move camera back to see it all
  ///////////////////////////////////
  float4 fWidthDesired(1.1F * vector0to1.GetLength());
  const float4 fFovRadians(camera.GetFovDegrees() * M_PI / 180.0F);
  const float4 fDistanceDesired(fWidthDesired / (2.0 * tan(0.5f * fFovRadians)));
  Vector3Df translate(fDistanceDesired * camera.GetViewDirection());
  camera.SetEye(center + translate);

  // set camera
  ///////////////
  vxCamera * pCamera = static_cast<vxCamera *>(data.m_pEnvironment->GetCamera());
  *pCamera = camera;

  // set oblique plane accordingly
  //////////////////////////////////
  vxPlaneOblique<float4> plane(camera.GetViewDirection(), center);
  vxPlaneOblique<float4> * pPlaneOblique = static_cast<vxPlaneOblique<float4> *>(data.m_pEnvironment->GetPlaneOblique());
  *pPlaneOblique = plane;

  return;
} // CreatePlaneObliqueAndCamera()


/**
 * Called when the mouse cursor is to be set
 * @return true if cursor was set, false otherwise
 */
bool vxManipulatorPlaneObliqueCreate::OnSetCursor()
//*******************************************************************
{
  bool bHandled(false);

  if (m_pDataPOC->m_pEnvironment != NULL)
  {
    bHandled = true;

    if (GetEnvironment().GetIntensityVolume() == m_pDataPOC->m_pEnvironment->GetIntensityVolume())
    {
      if (m_pDataPOC->m_pEnvironment != &GetEnvironment())
        vxMouseCursor::SetCursor("CURSOR_PLANE_OBLIQUE_CREATE");
      else
        vxMouseCursor::SetCursor("CURSOR_PLANE_OBLIQUE_CREATE_DISABLED");
    }
    else
      vxMouseCursor::SetCursor("CURSOR_NO");
  }
  else
    vxMouseCursor::SetCursor("CURSOR_NO");

  return bHandled;
} // OnSetCursor()


#undef FILE_REVISION


// Revision History:
// $Log: vxManipulatorPlaneObliqueCreate.C,v $
// Revision 1.9  2007/02/05 15:29:25  geconomos
// reworked "user interacting" mechanism
//
// Revision 1.8  2007/01/29 20:03:46  gdavidson
// Issue #5340: Set the user interacting flag
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
// Revision 1.5  2005/05/24 13:50:02  michael
// Introduced concept of data objects to start with v3D_Data..../vxData...
// Has been done for DataColormapEditor, DataAnnotationCreate, and
// DataPlaneObliqueCreate
//
// Revision 1.4  2005/05/20 19:09:39  frank
// fixed 3D cursor on 3x3 view, fixed active environment and oblique 'self-shadowing' problem
//
// Revision 1.3  2001/01/03 19:30:23  michael
// "Replaced" ActiveViewerType by ActiveEnvironment.
//
// Revision 1.2  2004/04/06 16:25:23  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.21  2004/01/23 04:00:18  michael
// adjusted code to reflect changes made in refactoring annotations
//
// Revision 1.20  2004/01/14 14:18:19  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.19  2003/12/09 21:31:14  frank
// Fixed compile warning
//
// Revision 1.18  2003/11/11 02:19:58  michael
// took out debug messages
//
// Revision 1.17  2003/11/11 01:56:16  michael
// added initialization for dragging
//
// Revision 1.16  2003/09/29 17:53:58  michael
// plane is not initialized but modified!!!
//
// Revision 1.15  2003/09/29 17:21:54  michael
// accounting for the new event types when initializing the plane
//
// Revision 1.14  2003/09/23 22:10:38  michael
// took out redundant code and added ability to drag handles
//
// Revision 1.13  2003/09/22 21:14:44  michael
// changed points to two handles that can be rendered
//
// Revision 1.12  2003/08/27 12:21:16  michael
// using 2 pixels distance to avoid zero vector normalization due to forward and
// backward projection. Also initialized plane to show 110% of area of interest
//
// Revision 1.11  2003/08/21 18:25:27  michael
// fixed rotations for all kind of view types
//
// Revision 1.10  2003/08/21 13:14:50  michael
// adjusted code to set camera such that the orientation is identical as in the
// viewer where the line is created.
//
// Revision 1.9  2003/08/20 15:40:40  michael
// made scaling factor for zoom to be dependent on the volume units
//
// Revision 1.8  2003/08/20 14:21:55  michael
// added dragging and OnMouseEnter() and OnMouseLeave()
//
// Revision 1.7  2003/08/13 19:24:54  michael
// plane oblique creating, added environment to dynamic environment data
//
// Revision 1.6  2003/05/15 20:07:52  frank
// Moving away from old point class
//
// Revision 1.5  2003/04/29 14:12:57  frank
// Moved projection functionality to the viewer type
//
// Revision 1.4  2003/02/28 22:09:34  michael
// removed vxWinRect from vxManipulatorr. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.3  2002/12/17 14:38:56  michael
// OpenGL back projection now returns bool to indicate success or not
//
// Revision 1.2  2002/11/18 16:36:56  michael
// completed plane object renaming
//
// Revision 1.1  2002/10/17 15:27:03  frank
// Initial versions.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorPlaneObliqueCreate.C,v 1.9 2007/02/05 15:29:25 geconomos Exp $
// $Id: vxManipulatorPlaneObliqueCreate.C,v 1.9 2007/02/05 15:29:25 geconomos Exp $
