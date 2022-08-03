// $Id: vxManipulatorAlphaActiveRange.C,v 1.5 2006/01/18 20:39:26 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar Bitter (mailto:ingmar@viatronix.com)


// includes
#include "stdafx.h"
#include "vxManipulatorAlphaActiveRange.h"
#include "vxDataColormapEditor.h"
#include "vxEnvironment.h"
#include "vxMessageBox.h"
#include "vxUtils.h"
#include "Point2D.h"


// defines
#define FILE_REVISION "$Revision: 1.5 $"


/**
 * Constructor
 * @param environment Reference to the vxEnvironment object
 */
vxManipulatorAlphaActiveRange::vxManipulatorAlphaActiveRange(vxEnvironment & environment): 
vxManipulator(environment),
m_bDragging(false)
//*******************************************************************
{
} // constructor


/**
 * This event handler receives mouse down events for the viewer
 * @param position
 * @param button ID
 * @param keyboard state
 * @return succesful?
 */
bool vxManipulatorAlphaActiveRange::OnButtonDown(const Point2D<int2> & position, 
                                                 const ButtonEnum eButton, 
                                                 const uint4 uKeyboardState)
//*******************************************************************
{
  bool bHandled(false);
  vxShareableObject<vxDataColormapEditor> * pData = dynamic_cast< vxShareableObject<vxDataColormapEditor> *>(GetEnvironment().GetElement(vxIDs::DataColormapEditor));
  if (pData == NULL) 
  {
    return bHandled;
  }
  
  if (eButton == BUTTON_LEFT)
  {
    // compute the closest point
    int4 iClosestPoint(GetClosestPoint(ToLocal(position)));
    pData->SetSelectedAlphaPointIndex(iClosestPoint);
    pData->Modified(vxModInfoID(vxElement, vxIDs::DataColormapEditor, vxUtils::DATACOLORMAPEDITOR_MODIFIED));
    bHandled = true;
    m_bDragging = true;
    
  } // left button?
  
  return bHandled;
} // OnButtonDown


/**
 * This event handler receives mouse up events for the viewer    
 * @param position
 * @param button ID
 * @param keyboard state
 * @return succesful?
 */
bool vxManipulatorAlphaActiveRange::OnButtonUp(const Point2D<int2> & position,
                                               const ButtonEnum eButton, 
                                               const uint4 uState)
//*******************************************************************
{
  bool bHandled(false);
  
  vxShareableObject<vxDataColormapEditor> * pData = dynamic_cast< vxShareableObject<vxDataColormapEditor> *>(GetEnvironment().GetElement(vxIDs::DataColormapEditor));
  if (pData == NULL)
  {
    return bHandled;
  }
  
  if (eButton == BUTTON_LEFT && m_bDragging)
  {
    m_bDragging = false;
    bHandled = true;
    pData->Modified(vxModInfoID(vxElement, vxIDs::DataColormapEditor, vxUtils::DATACOLORMAPEDITOR_MODIFIED));
  }
  
  return bHandled;
} // OnButtonUp


/**
 * This event handler receives mouse move events for the viewer  
 * @param position
 * @param button ID
 * @param keyboard state
 * @return succesful?
 */
bool vxManipulatorAlphaActiveRange::OnMouseMove(const Point2D<int2> & position,
                                                const uint4 uState)
//*******************************************************************
{
  bool bHandled(false);
  vxShareableObject<vxDataColormapEditor> * pData = dynamic_cast< vxShareableObject<vxDataColormapEditor> *>(GetEnvironment().GetElement(vxIDs::DataColormapEditor));
  if (pData == NULL)
  {
    return bHandled;
  }

  int4 iSelected(pData->GetSelectedAlphaPointIndex());
  
  if (uState == STATE_LBUTTON && iSelected != -1)
  {
    Point2D<float4> newPos(ToLocal(position));
    std::vector< Point2D<ZeroOne> > alphaList(GetEnvironment().GetColormap()->GetAlphaList());
    
    // apply restrictions
    if (iSelected == 0) 
      newPos.m_x = 0.0F;
    else if (iSelected == alphaList.size()-1)
      newPos.m_x = 1.0F;
    else
    {
      // between two other points, restrict x to neighbors
      if (newPos.m_x < alphaList[iSelected-1].m_x)
        newPos.m_x = alphaList[iSelected-1].m_x;
      if (newPos.m_x > alphaList[iSelected+1].m_x)
        newPos.m_x = alphaList[iSelected+1].m_x;
    }
    
    // modify list
    alphaList[iSelected].m_x = newPos.m_x;
    alphaList[iSelected].m_y = newPos.m_y;
    GetEnvironment().GetColormap()->SetAlphaList(alphaList);
    bHandled = true;
  } // left button?

  if (bHandled == true)
  {
    GetEnvironment().GetColormap()->Modified(vxModInfo(vxColormap, vxUtils::COLORMAP_MODIFIED));
    pData->Modified(vxModInfoID(vxElement, vxIDs::DataColormapEditor, vxUtils::DATACOLORMAPEDITOR_MODIFIED));
  }
  
  return bHandled;
} // OnMouseMove


/**
 * This event handler can be reimplemented in a subclass to receive mouse down events for the viewer.
 * @param position
 * @param button ID
 * @param keyboard state
 * @return succesful?
 */
bool vxManipulatorAlphaActiveRange::OnDoubleClick(const Point2D<int2> & position, 
                                                  const ButtonEnum eButtonId, 
                                                  const uint4 uKeyboardState)
//*******************************************************************
{
  bool bHandled(false);
  vxShareableObject<vxDataColormapEditor> * pData = dynamic_cast< vxShareableObject<vxDataColormapEditor> *>(GetEnvironment().GetElement(vxIDs::DataColormapEditor));
  if (pData == NULL)
  {
    return bHandled;
  }

  std::vector<Point2D<ZeroOne> > alphaList(GetEnvironment().GetColormap()->GetAlphaList());
  Point2D<float4> localPoint(ToLocal(position));
  
  if (eButtonId == BUTTON_LEFT)
  {
    // compute the closest point
    int4 iClosestPoint(GetClosestPoint(localPoint));

    if (iClosestPoint == -1)
    {
      // create a new point and decide where to insert it
      int4 iInsertionPoint(-1);

      for (int i(0); i<alphaList.size()-1; i++)
      {
        if (localPoint.m_x > alphaList[i].m_x && localPoint.m_x < alphaList[i+1].m_x)
        {
          iInsertionPoint = i;
        }
      } // for all points

      if (iInsertionPoint != -1)
      {
        alphaList.insert(alphaList  .begin() + iInsertionPoint+1, localPoint);
        pData->SetSelectedAlphaPointIndex(iInsertionPoint + 1);
        m_bDragging = true;
        bHandled = true;
      } // to be inserted?
    } // create new point?

    // see if we can safely delete this point
    else if (iClosestPoint > 0 && iClosestPoint < alphaList.size()-1)
    {
      alphaList.erase(alphaList.begin() + iClosestPoint);
      pData->SetSelectedAlphaPointIndex(-1);
      m_bDragging = false;
      bHandled = true;
    }
  } // left button?

  if (bHandled == true)
  {
    GetEnvironment().GetColormap()->SetAlphaList(alphaList);
    GetEnvironment().GetColormap()->Modified(vxModInfo(vxColormap, vxUtils::COLORMAP_MODIFIED));
    pData->Modified(vxModInfoID(vxElement, vxIDs::DataColormapEditor, vxUtils::DATACOLORMAPEDITOR_MODIFIED));
  }

  return bHandled;
} // OnDoubleClick()


/**
 * This event handler can be reimplemented in a subclass to receive keyboard events for the viewer     
 * @param key ID
 * @param keyboard state
 * @return succesful?
 */
bool vxManipulatorAlphaActiveRange::OnKeyPressed(const uint1 uKeyId, 
                                                 const uint4 uKeyboardState)
//*******************************************************************
{

  bool bHandled(false);

  vxShareableObject<vxDataColormapEditor> * pData = dynamic_cast<vxShareableObject<vxDataColormapEditor> *>(GetEnvironment().GetElement(vxIDs::DataColormapEditor));
  if (pData == NULL) 
  {
    return bHandled;
  }

  std::vector<Point2D<ZeroOne> > alphaList(GetEnvironment().GetColormap()->GetAlphaList());  
  bHandled = true;

  switch (uKeyId)
  {
  case vxInput::KEY_1:
    // reset to ramp
    alphaList.clear();
    alphaList.push_back(Point2D< ZeroOne >(0.0F, 0.0F));
    alphaList.push_back(Point2D< ZeroOne >(1.0F, 1.0F));
    break;

  case vxInput::KEY_2:
    // reset to spike
    alphaList.clear();
    alphaList.push_back(Point2D< ZeroOne >(0.0F, 0.0F));
    alphaList.push_back(Point2D< ZeroOne >(0.5F, 1.0F));
    alphaList.push_back(Point2D< ZeroOne >(1.0F, 0.0F));
    break;

  case vxInput::KEY_3:
    // reset to uniform
    alphaList.clear();
    alphaList.push_back(Point2D< ZeroOne >(0.0F, 0.0F));
    alphaList.push_back(Point2D< ZeroOne >(0.0F, 1.0F));
    alphaList.push_back(Point2D< ZeroOne >(1.0F, 1.0F));
    alphaList.push_back(Point2D< ZeroOne >(1.0F, 0.0F));
    break;

  case vxInput::KEY_4:
    // reset to inverse window/level
    alphaList.clear();
    alphaList.push_back(Point2D< ZeroOne >(0.0F, 1.0F));
    alphaList.push_back(Point2D< ZeroOne >(1.0F, 0.0F));
    break;

  case vxInput::KEY_5:
    // reset to inverse spike (fly inside blood vessel with contrast)
    alphaList.clear();
    alphaList.push_back(Point2D< ZeroOne >(0.0F, 0.4F));
    alphaList.push_back(Point2D< ZeroOne >(0.25F,0.0F));
    alphaList.push_back(Point2D< ZeroOne >(0.90F,0.0F));
    alphaList.push_back(Point2D< ZeroOne >(1.0F, 1.0F));
    break;

  default:
    bHandled = false;
    break;

  } // switch key id

  if (bHandled == true)
  {
    GetEnvironment().GetColormap()->SetAlphaList(alphaList);
    GetEnvironment().GetColormap()->Modified(vxModInfo(vxColormap, vxUtils::COLORMAP_MODIFIED));
    pData->Modified(vxModInfo(vxDataColormapEditor, vxUtils::DATACOLORMAPEDITOR_MODIFIED));
  }

  return bHandled;

} // OnKeyPressed()


/**
 * Finds the closest point, closer than the closest distance, if any (-1 if none)
 * @param closest distance
 * @param point
 * @return point index
 */
int4 vxManipulatorAlphaActiveRange::GetClosestPoint(const Point2D<float4> & point)
/////////////////////////////////////////////////////////////////////
{
  float4 fClosestDistanceSquared( pow( static_cast< float4 >( m_selectionThresholdDistance ), 2.0F ) );
  int4 iClosestPoint( -1 );
  const std::vector<Point2D<ZeroOne> > & alphaList( GetEnvironment().GetColormap()->GetAlphaList() );
  const float4 selectionPixelX( point.m_x * GetEnvironment().GetWinRect().GetViewportWidth() );
  const float4 selectionPixelY( point.m_y * GetEnvironment().GetWinRect().GetViewportHeight() );

  for ( int i(0); i<alphaList.size(); ++i )
  {
    const float4 alphaPixelX( alphaList[i].m_x * GetEnvironment().GetWinRect().GetViewportWidth() );
    const float4 alphaPixelY( alphaList[i].m_y * GetEnvironment().GetWinRect().GetViewportHeight() );
    const float4 fDistanceSquared( pow( selectionPixelX - alphaPixelX, 2.0F ) + pow( selectionPixelY - alphaPixelY, 2.0F ) );
    if ( fDistanceSquared < fClosestDistanceSquared )
    {
      iClosestPoint = i;
      fClosestDistanceSquared = fDistanceSquared;
    }
  } // for all points

  return iClosestPoint;
} // GetClosestPoint()


/**
 * Converts a point in world coordinates to local coordinates
 * @param point in world coordinates
 * @return rranslated point to local coordinates
 */
Point2D<float4> vxManipulatorAlphaActiveRange::ToLocal(const Point2D<int2> & point)
//*******************************************************************
{
  return Point2D<float4>(Interpolate(0.0F, float4(point.m_x), float4(GetEnvironment().GetWinRect().GetViewportWidth ()-1), 0.0F, 1.0F),
                         Interpolate(0.0F, float4(point.m_y), float4(GetEnvironment().GetWinRect().GetViewportHeight()-1), 1.0F, 0.0F)
 );
} // ToLocal()


/**
 * Returns check if a click in world coordinates was next to a point in local coordinates
 * @param click selected point
 * @param point compared point
 * @return are the poionts next to each other
 */
bool vxManipulatorAlphaActiveRange::NextTo(const Point2D<float4>& click, 
                                           const Point2D<float4>& point) const
//*******************************************************************
{
  return(Distance(click, point) < 0.035);
} // NextTo()


#undef FILE_REVISION


// Revision History:
// $Log: vxManipulatorAlphaActiveRange.C,v $
// Revision 1.5  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.4  2005/05/24 15:32:18  frank
// made it easier to select and adjust points in the alpha transfer function
//
// Revision 1.3  2005/05/24 13:50:02  michael
// Introduced concept of data objects to start with v3D_Data..../vxData...
// Has been done for DataColormapEditor, DataAnnotationCreate, and
// DataPlaneObliqueCreate
//
// Revision 1.2  2004/04/06 16:24:32  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.26  2004/01/15 18:50:49  michael
// added "mailto"
//
// Revision 1.25  2004/01/09 17:21:58  frank
// fixed hardcoded key values - replaced with enumeration
//
// Revision 1.24  2003/12/16 19:36:15  michael
// added missing modified that previously resulted in a missing update.
//
// Revision 1.23  2003/08/20 14:22:34  michael
// added "mailto:"
//
// Revision 1.22  2003/07/30 17:57:39  geconomos
// Fixed modified calls for colormap editor data.
//
// Revision 1.21  2003/06/19 20:21:21  michael
// adjusted for proper use of vxUtils::vxEvent...
//
// Revision 1.20  2003/04/25 12:09:01  michael
// more code review
//
// Revision 1.19  2003/04/08 16:06:36  michael
// cosmetics
//
// Revision 1.18  2003/02/28 22:09:34  michael
// removed vxWinRect from vxManipulatorr. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.17  2002/11/27 23:58:09  ingmar
// added inverse spike
//
// Revision 1.16  2002/11/27 22:44:00  michael
// Took out curves that could be infringing on patents
//
// Revision 1.15  2002/10/14 15:31:28  michael
// accounting for const methods in vxColormap
//
// Revision 1.14  2002/10/04 18:11:06  michael
// renaming vxColormapEditorSharedData --> vxColormapEditorData
//
// Revision 1.13  2002/09/26 19:23:26  michael
// Fixed color selection to always update the lcoation in the color chooser
// corresponding to the currently selected point in the color map
//
// Revision 1.12  2002/09/25 22:13:40  ingmar
// changed order of preset assignments and added very wispy button
//
// Revision 1.11  2002/09/25 20:36:41  michael
// Completed vxTransferFunction to vxColormap transition
//
// Revision 1.10  2002/09/20 16:41:35  geconomos
// Changed implemetation of class vxID
//
// Revision 1.9  2002/09/19 14:26:01  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.8  2002/09/17 13:01:02  michael
// adjustments for changes in transfer function
//
// Revision 1.7  2002/09/16 19:59:03  dmitry
// New vxID (s) used.
//
// Revision 1.6  2002/09/12 04:59:44  frank
// Improved wispy and hard shapes
//
// Revision 1.5  2002/09/11 21:56:35  frank
// Added extra modified events to make it more live.
//
// Revision 1.4  2002/09/11 20:43:36  frank
// Working on transfer function event mechanism
//
// Revision 1.3  2002/09/10 23:19:52  frank
// Working on modified flags.
//
// Revision 1.2  2002/09/10 22:59:13  frank
// Made alpha curve manipulation possible.
//
// Revision 1.1  2002/09/06 23:05:50  ingmar
// new TFE without sampling effect renderer
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorAlphaActiveRange.C,v 1.5 2006/01/18 20:39:26 geconomos Exp $
// $Id: vxManipulatorAlphaActiveRange.C,v 1.5 2006/01/18 20:39:26 geconomos Exp $
