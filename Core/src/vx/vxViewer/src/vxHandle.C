// $Id: vxHandle.C,v 1.5.8.1 2009/05/01 21:42:12 gdavidson Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Michael Meissner (mailto:meissner@viatronix.com)


// includes
#include "stdafx.h"
#include "vxHandle.h"
#include "vxSerializer.h"
#include "vxEnvironment.h"
#include "vxViewerType.h"
#include "vxUtils.h"


// defines
#define FILE_REVISION "$Revision: 1.5.8.1 $"


// static member initialization
float4 vxHandle::m_fHandleScreenSize = 10;


/**
 * Default constructor
 */
vxHandle::vxHandle()
//*******************************************************************
{
  CommonInit();
} // constructor


/**
 * Copy contructor
 * @param other handle
 */
vxHandle::vxHandle(const vxHandle & other)
//*******************************************************************
{
  *this = other;
} // constructor


/** 
 * Destructor
 */
vxHandle::~vxHandle()
//*******************************************************************
{
} // destructor


/** 
 * Assignment operator
 */
const vxHandle & vxHandle::operator=(const vxHandle & other)
//*******************************************************************
{
  if (&other != this)
  {
    SetPosition(other.GetPosition());
    SetSelected(other.IsSelected());
    SetColorHandle(other.GetColorHandle());
    SetColorHandleSelected(other.GetColorHandleSelected());
    SetHandleWorldSize(other.GetHandleWorldSize());
    SetHandleScreenSize(other.GetHandleScreenSize());
  }

  return *this;
} // constructor


/**
 * Reset
 */
void vxHandle::Reset()
//*******************************************************************
{
  CommonInit();
} // Reset()


/**
 * CommonInit
 */
void vxHandle::CommonInit()
//*******************************************************************
{
  SetEnvironment(NULL);

  SetSelected(false);
  SetHandleScreenSize(7); // size in actual screen space pixels
  SetColorHandle(PixelRGBA<uint1>(128, 255, 192, 255));
  SetColorHandleSelected(PixelRGBA<uint1>(255, 128, 128, 255));
} // CommmonInit()


/**
 * Determine parameters needed to be computed once
 * @param environment
 */
void vxHandle::PreRender(vxEnvironment & environment)
//*******************************************************************
{
  try
  {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    Point2D<int2> screenMiddle(0.5*(viewport[0]+viewport[2]), 0.5*(viewport[1]+viewport[3]));
    Point2D<int2> screenMiddlePlus1(screenMiddle[0]+1, screenMiddle[1]);

    Point<float4> worldMiddle, worldMiddlePlus1;
    bool bSuccesful(true);
    bSuccesful |= environment.GetViewerType()->ConvertScreenToWorld(screenMiddle, worldMiddle, false);
    bSuccesful |= environment.GetViewerType()->ConvertScreenToWorld(screenMiddlePlus1, worldMiddlePlus1, false);
    if (bSuccesful == false)
    {
      LogWrn("Cannot get world coordinate of that position!!!", "vxHandle", "PreRender");
    }
    float4 fWorldDistance(worldMiddle.GetEuclideanDistance(worldMiddlePlus1));
    SetHandleWorldSize(GetHandleScreenSize()*fWorldDistance);
  }
  catch ( ... )
  {
    LogErr( "unhandled exception", "vxHandle", "PreRender" );
  }
} // PreRender()


/**
 * Update the annotation text
 * @param environment
 */
void vxHandle::Render(vxEnvironment & environment)
//*******************************************************************
{
  vxViewerType & viewerType(*environment.GetViewerType());

  if (viewerType.IsVisible(GetPosition()) == true)
  {
    glLineWidth(3.0);

    // determine color
    PixelRGBA<uint1> color(GetColorHandle());

    if (IsSelected() == true)
    {
      color = GetColorHandleSelected();
    }

    glColor4f(color.R()/255.0, color.G()/255.0, color.B()/255.0, color.A()/255.0);

    bool bScreenSpace = viewerType.GetRenderCoordinateSystem() == vxViewerType::eScreenSpace;

    // get the location and size of the handle
    Point<float4> position;
    float4 fHalfCubeSize;
    Point2D<int2> screenPos;
    switch (viewerType.GetRenderCoordinateSystem())
    {
    case vxViewerType::eScreenSpace:
      viewerType.ConvertWorldToScreen(GetPosition(), screenPos);
      position.m_x = screenPos.m_x;
      position.m_y = screenPos.m_y;
      position.m_z = 0.0F;
      fHalfCubeSize = GetHandleScreenSize() / 4.0F;
      break;
    case vxViewerType::eWorldSpace:
      position = GetPosition();
      fHalfCubeSize = GetHandleWorldSize() / 4.0F;
      break;
    default:
      throw ex::IllegalArgumentException(LogRec("", "vxHandle", "Render"));
      break;
    }

    if( bScreenSpace )
    {
      glBegin(GL_QUADS);
      glVertex2i(position[0] - fHalfCubeSize, position[1] - fHalfCubeSize );
      glVertex2i(position[0] - fHalfCubeSize, position[1] + fHalfCubeSize );
      glVertex2i(position[0] + fHalfCubeSize, position[1] + fHalfCubeSize );
      glVertex2i(position[0] + fHalfCubeSize, position[1] - fHalfCubeSize );
      glEnd();
    }
    else
    {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glBegin(GL_POLYGON);
      glVertex3f(position[0] - fHalfCubeSize, position[1] - fHalfCubeSize, position[2] - fHalfCubeSize);
      glVertex3f(position[0] - fHalfCubeSize, position[1] + fHalfCubeSize, position[2] - fHalfCubeSize);
      glVertex3f(position[0] + fHalfCubeSize, position[1] + fHalfCubeSize, position[2] - fHalfCubeSize);
      glVertex3f(position[0] + fHalfCubeSize, position[1] - fHalfCubeSize, position[2] - fHalfCubeSize);
      glEnd();

      glBegin(GL_POLYGON);
      glVertex3f(position[0] + fHalfCubeSize, position[1] - fHalfCubeSize, position[2] + fHalfCubeSize);
      glVertex3f(position[0] + fHalfCubeSize, position[1] + fHalfCubeSize, position[2] + fHalfCubeSize);
      glVertex3f(position[0] + fHalfCubeSize, position[1] + fHalfCubeSize, position[2] - fHalfCubeSize);
      glVertex3f(position[0] + fHalfCubeSize, position[1] - fHalfCubeSize, position[2] - fHalfCubeSize);	
      glEnd();

      glBegin(GL_POLYGON);
      glVertex3f(position[0] + fHalfCubeSize, position[1] + fHalfCubeSize, position[2] + fHalfCubeSize);
      glVertex3f(position[0] + fHalfCubeSize, position[1] - fHalfCubeSize, position[2] + fHalfCubeSize);
      glVertex3f(position[0] - fHalfCubeSize, position[1] - fHalfCubeSize, position[2] + fHalfCubeSize);
      glVertex3f(position[0] - fHalfCubeSize, position[1] + fHalfCubeSize, position[2] + fHalfCubeSize);
      glEnd();

      glBegin(GL_POLYGON);
      glVertex3f(position[0] - fHalfCubeSize, position[1] - fHalfCubeSize, position[2] - fHalfCubeSize);
      glVertex3f(position[0] - fHalfCubeSize, position[1] + fHalfCubeSize, position[2] - fHalfCubeSize);
      glVertex3f(position[0] - fHalfCubeSize, position[1] + fHalfCubeSize, position[2] + fHalfCubeSize);
      glVertex3f(position[0] - fHalfCubeSize, position[1] - fHalfCubeSize, position[2] + fHalfCubeSize);
      glEnd();

      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

  }

} // Render()


#undef FILE_REVISION


// $Log: vxHandle.C,v $
// Revision 1.5.8.1  2009/05/01 21:42:12  gdavidson
// Issue# 5593: Do not use rendering mode when converting from screen to world
//
// Revision 1.5  2007/02/12 14:23:56  geconomos
// added support for screen space rendering for oblique views
//
// Revision 1.4  2006/08/02 21:40:19  romy
// adjusted the cube size
//
// Revision 1.3  2005/11/18 21:15:22  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.2.12.1  2005/08/01 19:12:07  frank
// changed function name according to coding standards
//
// Revision 1.2  2004/03/11 18:21:02  frank
// reworked crop box to eliminate handles set and annotation ROI
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.1  2004/01/23 03:51:32  michael
// reworked annotation to have a vxHandle class and a vxHandlesObj contains
// a vector of handles and access functions. The vxAnnotation derives from this
// one and overloads needed SetHandle() calls to make needed adjustments.
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxHandle.C,v 1.5.8.1 2009/05/01 21:42:12 gdavidson Exp $
// $Id: vxHandle.C,v 1.5.8.1 2009/05/01 21:42:12 gdavidson Exp $
