// $Id: vxManipulatorColorMap.C,v 1.5 2006/01/18 20:39:26 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Ingmar Bitter (ingmar@viatronix.com)


// includes
#include "stdafx.h"
#include "vxManipulatorColormap.h"
#include "vxDataColormapEditor.h"
#include "vxEnvironment.h"
#include "vxMessageBox.h"
#include "vxUtils.h"
#include "vxIDs.h"
#include "Point2D.h"


// defines
#define FILE_REVISION "$Revision: 1.5 $"


/**
 * Constructor
 * @param environment
 */
vxManipulatorColormap::vxManipulatorColormap(vxEnvironment & environment): 
vxManipulator(environment),
m_bDragging(false)
//*******************************************************************
{
} // constructor


/**
 * Find location in image that has smallest euclidean distance in RGB space
 * @param color that needs to be found
 * @return position
 */
Point2D<float4> vxManipulatorColormap::FindLocationInImage(const PixelRGBA<uint1> & color)
//*******************************************************************
{
  vxShareableObject<vxDataColormapEditor> * pData = dynamic_cast< vxShareableObject<vxDataColormapEditor> *>(GetEnvironment().GetElement(vxIDs::DataColormapEditor));
  if (pData == NULL) 
  {
    return Point2D<float4>(0.0F, 0.0F);
  }
  else
  {
    Point2D<float4> imageLocation(0.0F, 0.0F);
    Point2D<float4> bestColorLocation;
    float4 fShortestDistance(9999.0), fDistance;

    for (int4 j(0); j<pData->GetImagePtr()->GetDim().Y(); j++)
    {
      for (int4 i(0); i<pData->GetImagePtr()->GetDim().X(); i++)
      {
        PixelRGBA<uint1> imageColor(pData->GetImagePtr()->GetPixelRaw(Point2D<uint2>(i,j)));
        const float4 fR(color.R() - imageColor.R());
        const float4 fG(color.G() - imageColor.G());
        const float4 fB(color.B() - imageColor.B());
        fDistance = sqrt(static_cast<float4>(fR*fR + fG*fG + fB*fB));
        if (fDistance < fShortestDistance)
        {
          fShortestDistance = fDistance;
          imageLocation = Point2D<float4>(i/(float4)pData->GetImagePtr()->GetDim().X(),
                                          1.0F-j/(float4)pData->GetImagePtr()->GetDim().Y());
        }
      } // for all pixels
    } // for all scanlines
    return imageLocation;
  } // image available?
} // FindLocationInImage()


/**
 * This event handler receives mouse down events for the viewer
 * @param mouse position
 * @param mouse button
 * @param keyboard state
 * @return handled?
 */
bool vxManipulatorColormap::OnButtonDown(const Point2D<int2> & pos, 
                                         const ButtonEnum eButton, 
                                         const uint4 uState)
//*******************************************************************
{
  bool bHandled(false);
  vxShareableObject<vxDataColormapEditor> * pData = dynamic_cast<vxShareableObject<vxDataColormapEditor> *>(GetEnvironment().GetElement(vxIDs::DataColormapEditor));
  if (pData == NULL) 
  {
    return bHandled;
  }
  
  if (eButton == BUTTON_LEFT)
  {
    // compute the closest point
    float4 fClosestDistance(0.1F);
    int4 iClosestPoint(GetClosestPoint(fClosestDistance, ToLocal(pos)));
		
		if( iClosestPoint == -1 )
		{ // no selection
			return true;
		}

    pData->SetSelectedColorPointIndex(iClosestPoint);
    PixelRGBA<uint1> color(0.0F, 0.0F, 0.0F, 0.0F);
    color.R() = 255 * GetEnvironment().GetColormap()->GetRedList()[iClosestPoint].Y();
    color.G() = 255 * GetEnvironment().GetColormap()->GetGreenList()[iClosestPoint].Y();
    color.B() = 255 * GetEnvironment().GetColormap()->GetBlueList()[iClosestPoint].Y();
    pData->SetColorLocation(FindLocationInImage(color));
    pData->Modified(vxModInfoID(vxElement, vxIDs::DataColormapEditor, vxUtils::DATACOLORMAPEDITOR_MODIFIED));
    GetEnvironment().GetColormap()->Modified(vxModInfo(vxColormap, vxUtils::COLORMAP_MODIFIED));
    bHandled = true;
    m_bDragging = true;
  } // if left button

  return bHandled;
} // OnButtonDown()


/**
 * Finds the closest point, closer than the closest distance, if any (-1 if none)
 * @param closest distance
 * @param position
 * @return index
 */
int4 vxManipulatorColormap::GetClosestPoint(float4 & fClosestDistance, 
                                            const Point2D<float4> & position)
//*******************************************************************
{
  // compute the closest point
  int4 iClosestPoint(-1);
  const std::vector< Point2D<ZeroOne> > & redList = GetEnvironment().GetColormap()->GetRedList();

  for (int i(0); i<redList.size(); i++)
  {
    const float4 fDistance(fabs(position.m_x - redList[i].m_x));
    if (fDistance < fClosestDistance)
    {
      iClosestPoint = i;
      fClosestDistance = fDistance;
    }
  }

  return iClosestPoint;
} // GetClosestPoint()


/**
 * This event handler can be reimplemented in a subclass to receive mouse down events for the viewer.
 * @param mouse position
 * @param mouse button
 * @param keyboard state
 * @return handled?
 */
bool vxManipulatorColormap::OnDoubleClick(const Point2D<int2> & position, 
                                          const ButtonEnum eButtonId, 
                                          const uint4 uKeyboardState)
//*******************************************************************
{
  bool bHandled(false);
  vxShareableObject<vxDataColormapEditor> * pData = dynamic_cast<vxShareableObject<vxDataColormapEditor> *>(GetEnvironment().GetElement(vxIDs::DataColormapEditor));
  if (pData == NULL) 
  {
    return bHandled;
  }

  // make (deep) copy of the original lists
  std::vector<Point2D<ZeroOne> > redList(GetEnvironment().GetColormap()->GetRedList());
  std::vector<Point2D<ZeroOne> > greenList(GetEnvironment().GetColormap()->GetGreenList());
  std::vector<Point2D<ZeroOne> > blueList(GetEnvironment().GetColormap()->GetBlueList());
  Point2D<float4> localPoint(ToLocal(position));
  
  if (eButtonId == BUTTON_LEFT)
  {
    // compute the closest point
    float4 fClosestDistance(0.05F);
    int4 iClosestPoint(GetClosestPoint(fClosestDistance, localPoint));

    if (iClosestPoint == -1)
    {
      // decide where to insert the new point
      int4 iInsertionPoint(-1);

      for (int i(0); i<redList.size()-1; i++)
      {
        if (localPoint.m_x > redList[i].m_x && localPoint.m_x < redList[i+1].m_x)
        {
          iInsertionPoint = i;
        }
      }

      if (iInsertionPoint != -1)
      {
        // pick a color ... that is not straight grey scale
        PixelRGBA<float4> color(0.0F, 0.0F, 0.0F, 0.0F);
        while (Max(Max(color.R(), color.G()), color.B()) - Min(Min(color.R(), color.G()), color.B()) < 0.1)
        {
           color = PixelRGBA<float4>(Drand48(0.25F, 1.0F), Drand48(0.25F, 1.0F), Drand48(0.25F, 1.0F), 1.0);
        }
        PixelRGBA<uint1> colorTmp(255*color.R(), 255*color.G(), 255*color.B(), 255);
        redList.insert(redList  .begin() + iInsertionPoint+1, Point2D<float4>(localPoint.m_x, color.R()));
        greenList.insert(greenList.begin() + iInsertionPoint+1, Point2D<float4>(localPoint.m_x, color.G()));
        blueList.insert(blueList .begin() + iInsertionPoint+1, Point2D<float4>(localPoint.m_x, color.B()));
        pData->SetColorLocation(FindLocationInImage(colorTmp));
        pData->SetSelectedColorPointIndex(iInsertionPoint + 1);
        m_bDragging = true;
        bHandled = true;
      } // still no closest point?
    } // no closest point?

    // see if we can safely delete this point
    else if (iClosestPoint > 0 && iClosestPoint < redList.size()-1)
    {
      //LogDbg("  erase point " + ToAscii(iClosestPoint), "vxManipulatorColormap", "OnButtonDown");
      redList  .erase(redList  .begin() + iClosestPoint);
      greenList.erase(greenList.begin() + iClosestPoint);
      blueList .erase(blueList .begin() + iClosestPoint);
      pData->SetColorLocation(Point2D<float4>(0.0F, 0.0F));
      pData->SetSelectedColorPointIndex(-1);
      m_bDragging = false;
      bHandled = true;
    }
  } // left mouse button?

  if (bHandled == true)
  {
    GetEnvironment().GetColormap()->SetRedList(redList);
    GetEnvironment().GetColormap()->SetGreenList(greenList);
    GetEnvironment().GetColormap()->SetBlueList(blueList);
    redList.clear();
    greenList.clear();
    blueList.clear();
    GetEnvironment().GetColormap()->Modified(vxModInfo(vxColormap, vxUtils::COLORMAP_MODIFIED));
    pData->Modified(vxModInfoID(vxElement, vxIDs::DataColormapEditor, vxUtils::DATACOLORMAPEDITOR_MODIFIED));
  }

  return bHandled;
} // OnDoubleClick()


/**
 * This event handler receives mouse up events for the viewer.     
 * @param mouse position
 * @param mouse button
 * @param keyboard state
 * @return handled?
 */
bool vxManipulatorColormap::OnButtonUp(const Point2D<int2> & position, 
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

  if (eButton == BUTTON_LEFT && m_bDragging == true)
  {
    m_bDragging = false;
    bHandled = true;
    GetEnvironment().GetColormap()->Modified(vxModInfo(vxColormap, vxUtils::COLORMAP_MODIFIED));
    pData->Modified(vxModInfoID(vxElement, vxIDs::DataColormapEditor, vxUtils::DATACOLORMAPEDITOR_MODIFIED));
  }
    
  return bHandled;
} // OnButtonUp


/**
 * This event handler receives mouse move events for the viewer
 * @param mouse position
 * @param keyboard state
 * @return handled?
 */
bool vxManipulatorColormap::OnMouseMove(const Point2D<int2> & position, 
                                        const uint4 uKeyboardState)
//*******************************************************************
{
  bool bHandled(false);
  vxShareableObject<vxDataColormapEditor> * pData = dynamic_cast< vxShareableObject<vxDataColormapEditor> *>(GetEnvironment().GetElement(vxIDs::DataColormapEditor));
  if (pData == NULL) 
  {
    return bHandled;
  }

  int4 iSelected(pData->GetSelectedColorPointIndex());

  if (uKeyboardState == STATE_LBUTTON && iSelected != -1)
  {
    Point2D<float4> newPos(ToLocal(position));
    std::vector<Point2D<ZeroOne> > redList  (GetEnvironment().GetColormap()->GetRedList());
    std::vector<Point2D<ZeroOne> > greenList(GetEnvironment().GetColormap()->GetGreenList());
    std::vector<Point2D<ZeroOne> > blueList (GetEnvironment().GetColormap()->GetBlueList());
    
    // apply restrictions
    if (iSelected == 0) 
      newPos.m_x = 0.0F;
    else if (iSelected == redList.size()-1)
      newPos.m_x = 1.0F;
    else
    {
      // between two other points, restrict x to neighbors
      if (newPos.m_x < redList[iSelected-1].m_x)
      {
        newPos.m_x = redList[iSelected-1].m_x;
        newPos.m_x = greenList[iSelected-1].m_x;
        newPos.m_x = blueList[iSelected-1].m_x;
      }
      if (newPos.m_x > redList[iSelected+1].m_x)
      {
        newPos.m_x = redList[iSelected+1].m_x;
        newPos.m_x = greenList[iSelected+1].m_x;
        newPos.m_x = blueList[iSelected+1].m_x;
      }
    } // not first nor last in list?

    redList[iSelected].m_x   = newPos.m_x;
    greenList[iSelected].m_x = newPos.m_x;
    blueList[iSelected].m_x  = newPos.m_x;
    GetEnvironment().GetColormap()->SetRedList(redList);
    GetEnvironment().GetColormap()->SetGreenList(greenList);
    GetEnvironment().GetColormap()->SetBlueList(blueList);
    bHandled = true;
  }
  if (bHandled == true)
  {
    GetEnvironment().GetColormap()->Modified(vxModInfo(vxColormap, vxUtils::COLORMAP_MODIFIED));
    pData->Modified(vxModInfoID(vxElement, vxIDs::DataColormapEditor, vxUtils::DATACOLORMAPEDITOR_MODIFIED));
  }

  return bHandled;
} // OnMouseMove()


/**
 * Local helper function
 * @param list of red points
 * @param list of green points
 * @param list of blue points
 * @param position
 * @param color
 */
void AddColorToList(std::vector<Point2D<ZeroOne> > & redList,
                    std::vector<Point2D<ZeroOne> > & greenList,
                    std::vector<Point2D<ZeroOne> > & blueList,
                    const float4 & position, 
                    const PixelRGB<float4> & color)
//*******************************************************************
{
  redList  .push_back( Point2D<ZeroOne>( position, color.R() ) );
  greenList.push_back( Point2D<ZeroOne>( position, color.G() ) );
  blueList .push_back( Point2D<ZeroOne>( position, color.B() ) );
} // AddColorToList()


/**
 * Handling when key is pressed
 * @param key id
 * @param keyboard state
 * @return handled?
 */
bool vxManipulatorColormap::OnKeyPressed(const uint1 uKeyId, 
                                         const uint4 uKeyboardState)
//*******************************************************************
{
  vxShareableObject<vxDataColormapEditor> * pData = dynamic_cast<vxShareableObject<vxDataColormapEditor> *>(GetEnvironment().GetElement(vxIDs::DataColormapEditor));
  if (pData == NULL)
  {
    return false;
  }
  std::vector<Point2D<ZeroOne> > redList, greenList, blueList;
  bool bHandled(true);

  switch (uKeyId)
  {
  case vxInput::KEY_1:
    // reset to pure white
    AddColorToList(redList, greenList, blueList, 0.0F, PixelRGB< float4 >(1.0F, 1.0F, 1.0F));
    AddColorToList(redList, greenList, blueList, 1.0F, PixelRGB< float4 >(1.0F, 1.0F, 1.0F));
    break;
  case vxInput::KEY_2:
    // reset to heat scale
    AddColorToList(redList, greenList, blueList, 0.00F, PixelRGB< float4 >(1.00F, 0.00F, 0.00F));
    AddColorToList(redList, greenList, blueList, 0.33F, PixelRGB< float4 >(1.00F, 0.66F, 0.00F));
    AddColorToList(redList, greenList, blueList, 0.66F, PixelRGB< float4 >(1.00F, 1.00F, 0.00F));
    AddColorToList(redList, greenList, blueList, 1.00F, PixelRGB< float4 >(1.00F, 1.00F, 1.00F));
    break;
  case vxInput::KEY_3:
    // reset to rainbow
    AddColorToList(redList, greenList, blueList, 0.0F,  PixelRGB< float4 >(0, 0, 1));
    AddColorToList(redList, greenList, blueList, 0.25F, PixelRGB< float4 >(0, 1, 1));
    AddColorToList(redList, greenList, blueList, 0.5F,  PixelRGB< float4 >(0, 1, 0));
    AddColorToList(redList, greenList, blueList, 0.75F, PixelRGB< float4 >(1, 1, 0));
    AddColorToList(redList, greenList, blueList, 1.0F,  PixelRGB< float4 >(1, 0, 0));
    break;
  case vxInput::KEY_4:
    // reset to heat
    AddColorToList(redList, greenList, blueList, 0.0F, PixelRGB< float4 >(0, 0, 1));
    AddColorToList(redList, greenList, blueList, 0.5F, PixelRGB< float4 >(1, 1, 0));
    AddColorToList(redList, greenList, blueList, 1.0F, PixelRGB< float4 >(1, 0, 0));
    break;
  case vxInput::KEY_5:
    // reset to contrast
    AddColorToList(redList, greenList, blueList, 0.0F, PixelRGB< float4 >(0.831F, 0.153F, 0.153F));
    AddColorToList(redList, greenList, blueList, 0.5F, PixelRGB< float4 >(0.831F, 0.882F, 0.439F));
    AddColorToList(redList, greenList, blueList, 1.0F, PixelRGB< float4 >(1.000F, 1.000F, 1.000F));
    break;
  case vxInput::KEY_6:
    // reset to heat scale
    AddColorToList(redList, greenList, blueList, 0.0F, PixelRGB< float4 >(0.83F , 0.15F , 0.15F));
    AddColorToList(redList, greenList, blueList, 1.0F, PixelRGB< float4 >(0.914F, 0.792F, 0.533F));
    break;    
  default:
    bHandled = false;
    break;
  }

  if (bHandled == true)
  {
    pData->SetSelectedColorPointIndex(-1);
    GetEnvironment().GetColormap()->SetRedList(redList);
    GetEnvironment().GetColormap()->SetGreenList(greenList);
    GetEnvironment().GetColormap()->SetBlueList(blueList);
    GetEnvironment().GetColormap()->Modified(vxModInfo(vxColormap, vxUtils::COLORMAP_MODIFIED));
    pData->Modified(vxModInfoID(vxElement, vxIDs::DataColormapEditor, vxUtils::DATACOLORMAPEDITOR_MODIFIED));
  }

  return bHandled;
} // OnKeyPressed()


/**
 * Converts a point in world coordinates to local coordinates
 * @param point in world coordinates
 * @return translated point to local coordinates
 */ 
Point2D<float4> vxManipulatorColormap::ToLocal(const Point2D<int2> & point)
//*******************************************************************
{
  vxDataColormapEditor * pData = dynamic_cast<vxDataColormapEditor *>(GetEnvironment().GetElement(vxIDs::DataColormapEditor));
  if (pData == NULL) 
  {
    return Point2D<float4>(0.0F, 0.0F);
  }

  return Point2D<float4>(Interpolate(0.0F, float4(point.m_x), float4(GetEnvironment().GetWinRect().GetViewportWidth ()-1), 0.0F, 1.0F),
                         Interpolate(0.0F, float4(point.m_y), float4(GetEnvironment().GetWinRect().GetViewportHeight()-1), 0.0F, 1.0F));
} // ToLocal()


#undef FILE_REVISION


// Revision History:
// $Log: vxManipulatorColorMap.C,v $
// Revision 1.5  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.4  2005/05/24 13:50:02  michael
// Introduced concept of data objects to start with v3D_Data..../vxData...
// Has been done for DataColormapEditor, DataAnnotationCreate, and
// DataPlaneObliqueCreate
//
// Revision 1.3  2004/04/06 16:25:11  frank
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
// Revision 1.35  2004/01/09 17:21:44  frank
// fixed hardcoded key values - replaced with enumeration
//
// Revision 1.34  2003/12/18 14:14:17  frank
// Renamed the element ID filename to match class name
//
// Revision 1.33  2003/07/30 17:57:40  geconomos
// Fixed modified calls for colormap editor data.
//
// Revision 1.32  2003/06/19 20:21:21  michael
// adjusted for proper use of vxUtils::vxEvent...
//
// Revision 1.31  2003/06/10 12:06:41  frank
// Code review of pixel classes
//
// Revision 1.30  2003/05/07 11:44:01  michael
// initial code review
//
// Revision 1.29  2003/04/24 14:03:02  michael
// accounted for new Cursor3d handline (real class, not just triple)
//
// Revision 1.28  2003/04/08 16:06:36  michael
// cosmetics
//
// Revision 1.27  2003/02/28 22:09:34  michael
// removed vxWinRect from vxManipulatorr. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.26  2003/01/22 21:56:02  ingmar
// synchronized to new Point2D access to Image pixels
//
// Revision 1.25  2002/12/20 15:57:43  ingmar
// new PixelRGBA interface functions R() G() B() A()
//
// Revision 1.24  2002/10/16 22:23:15  michael
// cosmetics
//
// Revision 1.23  2002/10/14 18:43:20  michael
// fixed type bug that ocurred during the const-transition of vxCamera and vxColormap
//
// Revision 1.22  2002/10/14 15:31:28  michael
// accounting for const methods in vxColormap
//
// Revision 1.21  2002/10/08 15:20:05  michael
// fixed bug: Reset active color point to -1 when selecting preset (OnKeyPressed)
//
// Revision 1.20  2002/10/07 20:43:27  michael
// changed default color ranges ...
//
// Revision 1.19  2002/10/04 18:11:07  michael
// renaming vxColormapEditorSharedData --> vxColormapEditorData
//
// Revision 1.18  2002/10/04 16:34:19  michael
// colormap renderer/manipulator ID fix
//
// Revision 1.17  2002/10/04 14:37:07  michael
// fixed colormap
//
// Revision 1.16  2002/10/01 15:46:50  michael
// Added modified call, so that on mouse release there is a redraw of the 2D and
// 3D views and not only when picking a color in the colorchoser.
//
// Revision 1.15  2002/09/26 19:23:26  michael
// Fixed color selection to always update the lcoation in the color chooser
// corresponding to the currently selected point in the color map
//
// Revision 1.14  2002/09/25 20:36:41  michael
// Completed vxTransferFunction to vxColormap transition
//
// Revision 1.13  2002/09/20 16:41:35  geconomos
// Changed implemetation of class vxID
//
// Revision 1.12  2002/09/19 14:26:01  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.11  2002/09/17 13:01:02  michael
// adjustments for changes in transfer function
//
// Revision 1.10  2002/09/16 20:50:25  dmitry
// vxID (s) used properly
//
// Revision 1.9  2002/09/16 16:50:43  frank
// Added new colormaps.
//
// Revision 1.8  2002/09/11 23:04:22  frank
// Added more colormap presets.
//
// Revision 1.7  2002/09/11 21:56:35  frank
// Added extra modified events to make it more live.
//
// Revision 1.6  2002/09/11 20:43:36  frank
// Working on transfer function event mechanism
//
// Revision 1.5  2002/09/10 22:59:59  frank
// Cleaned out dead code.
//
// Revision 1.4  2002/09/10 17:18:46  frank
// Added colormap point rendering and manipulation.
//
// Revision 1.3  2002/09/09 22:28:40  frank
// Improved selection and creation of colormap points.
//
// Revision 1.2  2002/09/09 21:31:58  frank
// Working on color map editor
//
// Revision 1.1  2002/09/06 23:05:50  ingmar
// new TFE without sampling effect renderer
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorColorMap.C,v 1.5 2006/01/18 20:39:26 geconomos Exp $
// $Id: vxManipulatorColorMap.C,v 1.5 2006/01/18 20:39:26 geconomos Exp $
