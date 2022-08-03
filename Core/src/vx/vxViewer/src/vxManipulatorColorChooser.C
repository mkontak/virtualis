// $Id: vxManipulatorColorChooser.C,v 1.4 2006/01/18 20:39:26 geconomos Exp $
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
#include "vxManipulatorColorChooser.h"
#include "vxDataColormapEditor.h"
#include "vxEnvironment.h"
#include "vxMessageBox.h"
#include "vxUtils.h"
#include "vxIDs.h"
#include "Point2D.h"
#include "ResourceImage.h"


// defines
#define FILE_REVISION "$Revision: 1.4 $"


/**
 * Constructor
 * @param environment
 */
vxManipulatorColorChooser::vxManipulatorColorChooser(vxEnvironment & environment): 
vxManipulator(environment),
m_pColormapImage(NULL),
m_bDragging(false)
//*******************************************************************
{
} // constructor


/**
 * Handle the button down event
 * @param mouse position
 * @param mouse button
 * @param key state
 * @return handled?
 */
bool vxManipulatorColorChooser::OnButtonDown(const Point2D<int2> & position, 
                                             const ButtonEnum eButton, 
                                             const uint4 uKeyboardState)
//*******************************************************************
{
  bool bHandled(false);

  if (eButton == BUTTON_LEFT)
    bHandled = SetColor(position);

  if (bHandled == true)
    m_bDragging = true;

  return bHandled;
} // OnButtonDown()


/**
 * Set the color from the colormap image to the transfer function
 * @param position
 * @return success?
 */
bool vxManipulatorColorChooser::SetColor(const Point2D<int2> & position)
//*******************************************************************
{
  bool bHandled(false);

  vxShareableObject<vxDataColormapEditor> * pData = dynamic_cast<vxShareableObject<vxDataColormapEditor> *>(GetEnvironment().GetElement(vxIDs::DataColormapEditor));
  if (pData == NULL)
  {
    return bHandled;
  }

  const int4 iSelectedPoint(pData->GetSelectedColorPointIndex());
  
  if (iSelectedPoint != -1)
  {
    if (m_pColormapImage == NULL)
    {
      m_pColormapImage = ResourceImage::FromResource("COLORMAP");
      if (m_pColormapImage == NULL)
      {
        LogDbg("Colormap image is NULL", "vxManipulatorColorChooser", "OnButtonDown");
        return false;
      }
    }

    // scale to image space
    float4 fX(Interpolate(0.0F, float4(position.m_x), float4(GetEnvironment().GetWinRect().GetViewportWidth ()), 0.0F, float4(m_pColormapImage->GetDim().m_x-1)));
    float4 fY(Interpolate(0.0F, float4(position.m_y), float4(GetEnvironment().GetWinRect().GetViewportHeight()), 0.0F, float4(m_pColormapImage->GetDim().m_y-1)));
    
    // bound to image dimensions
    int4 iX(Bound(int4(0), int4(fX + 0.5F), int4(m_pColormapImage->GetDim().m_x-1)));
    int4 iY(Bound(int4(0), int4(fY + 0.5F), int4(m_pColormapImage->GetDim().m_y-1)));
    PixelRGBA<uint1> pixel(m_pColormapImage->GetPixel(Point2D<uint2>(iX, iY)));

    // convert to normalized image coordinates
    const Point2D<float4> imagePos(float(iX) / float4(m_pColormapImage->GetDim().m_x-1), float4(m_pColormapImage->GetDim().m_y - 1 - iY) / float4(m_pColormapImage->GetDim().m_y-1));

    // set the color
    std::vector<Point2D<ZeroOne> > redList(GetEnvironment().GetColormap()->GetRedList());
    std::vector<Point2D<ZeroOne> > greenList(GetEnvironment().GetColormap()->GetGreenList());
    std::vector<Point2D<ZeroOne> > blueList(GetEnvironment().GetColormap()->GetBlueList());
    redList  [iSelectedPoint].m_y = pixel.R() / 255.0F;
    greenList[iSelectedPoint].m_y = pixel.G() / 255.0F;
    blueList [iSelectedPoint].m_y = pixel.B() / 255.0F;
    GetEnvironment().GetColormap()->SetRedList  (redList);
    GetEnvironment().GetColormap()->SetGreenList(greenList);
    GetEnvironment().GetColormap()->SetBlueList (blueList);
    pData->SetColorLocation(imagePos);
    bHandled = true;
  } // point selected?
  
  if (bHandled)
  {
    GetEnvironment().GetColormap()->Modified(vxModInfo(vxColormap, vxUtils::COLORMAP_MODIFIED));
    pData->Modified(vxModInfo(vxDataColormapEditor, vxUtils::DATACOLORMAPEDITOR_MODIFIED));
  }

  return bHandled;
} // SetColor()


/**
 * Handle the button up event
 * @param mouse position
 * @param mouse button
 * @param key state
 * @return handled?
 */
bool vxManipulatorColorChooser::OnButtonUp(const Point2D<int2> & position, 
                                           const ButtonEnum eButton, 
                                           const uint4 uKeyboardState)
//*******************************************************************
{
  bool bHandled(false);

  m_bDragging = false;

  return bHandled;
} // OnButtonUp()


/**
 * This event handler receives mouse move events for the viewer.     
 * @param mouse position
 * @param key state
 * @return handled?
 */
bool vxManipulatorColorChooser::OnMouseMove(const Point2D<int2> & position, 
                                            const uint4 uKeyboardState)
//*******************************************************************
{
  bool bHandled(false);

  if (m_bDragging == true)
    bHandled = SetColor(position);
  
  return bHandled;
} // OnMouseMove()


/**
 * This event handler receives mouse move events for the viewer    
 */
void vxManipulatorColorChooser::OnMouseLeave()
//*******************************************************************
{
  m_bDragging = false;
} // OnMouseLeave()


#undef FILE_REVISION


// Revision History:
// $Log: vxManipulatorColorChooser.C,v $
// Revision 1.4  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.3  2005/05/24 13:50:02  michael
// Introduced concept of data objects to start with v3D_Data..../vxData...
// Has been done for DataColormapEditor, DataAnnotationCreate, and
// DataPlaneObliqueCreate
//
// Revision 1.2  2004/04/06 16:24:58  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.21  2003/12/18 14:14:17  frank
// Renamed the element ID filename to match class name
//
// Revision 1.20  2003/07/30 17:57:39  geconomos
// Fixed modified calls for colormap editor data.
//
// Revision 1.19  2003/06/19 20:21:21  michael
// adjusted for proper use of vxUtils::vxEvent...
//
// Revision 1.18  2003/05/07 11:44:01  michael
// initial code review
//
// Revision 1.17  2003/04/08 16:06:36  michael
// cosmetics
//
// Revision 1.16  2003/02/28 22:09:34  michael
// removed vxWinRect from vxManipulatorr. Is now only in vxViewer and is set in
// vxEnvironment when needed (during rendering or manipulation).
//
// Revision 1.15  2003/01/22 21:55:51  ingmar
// synchronized to new Point2D access to Image pixels
//
// Revision 1.14  2002/12/20 15:57:43  ingmar
// new PixelRGBA interface functions R() G() B() A()
//
// Revision 1.13  2002/10/14 18:43:20  michael
// fixed type bug that ocurred during the const-transition of vxCamera and vxColormap
//
// Revision 1.12  2002/10/14 15:31:28  michael
// accounting for const methods in vxColormap
//
// Revision 1.11  2002/10/04 18:11:07  michael
// renaming vxColormapEditorSharedData --> vxColormapEditorData
//
// Revision 1.10  2002/09/26 19:23:26  michael
// Fixed color selection to always update the lcoation in the color chooser
// corresponding to the currently selected point in the color map
//
// Revision 1.9  2002/09/25 20:36:41  michael
// Completed vxTransferFunction to vxColormap transition
//
// Revision 1.8  2002/09/20 16:41:35  geconomos
// Changed implemetation of class vxID
//
// Revision 1.7  2002/09/19 14:26:01  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.6  2002/09/17 13:01:02  michael
// adjustments for changes in transfer function
//
// Revision 1.5  2002/09/16 20:50:25  dmitry
// vxID (s) used properly
//
// Revision 1.4  2002/09/11 17:03:06  frank
// Flipped image
//
// Revision 1.3  2002/09/10 17:19:52  frank
// Removed debug
//
// Revision 1.2  2002/09/10 17:18:46  frank
// Added colormap point rendering and manipulation.
//
// Revision 1.1  2002/09/06 23:05:50  ingmar
// new TFE without sampling effect renderer
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxManipulatorColorChooser.C,v 1.4 2006/01/18 20:39:26 geconomos Exp $
// $Id: vxManipulatorColorChooser.C,v 1.4 2006/01/18 20:39:26 geconomos Exp $
