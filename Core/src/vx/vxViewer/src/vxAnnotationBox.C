// $Id: vxAnnotationBox.C,v 1.12 2007/02/12 14:23:56 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Author: Michael Meissner (mailto:meissner@viatronix.com)


// pragmas
#pragma warning (disable : 4786)


// includes
#include "stdafx.h"
#include "vxAnnotationBox.h"
#include "vxViewerType.h"
#include "GLFontGeorge.h"
#include "vxEnvironment.h"
#include "vxBlockVolume.h"
#include "vxIntensityConverter.h"
#include "vxIntensityStatistic.h"
#include "vxBlockVolumeRayIterator.h"
#include "vxSerializer.h"
#include "Matrix.h"
#include "ResourceImage.h"
#include "vxUtils.h"


// namespaces
using namespace std;


// defines
#define FILE_REVISION "$Revision: 1.12 $"


/**
 * Default constructor
 */
vxAnnotationBox::vxAnnotationBox()
//*******************************************************************
{
  CommonInit();
}


/**
 * CommonInit
 */
void vxAnnotationBox::CommonInit()
//*******************************************************************
{
  SetMouseCursor("CURSOR_SQUARE");
  SetImage(ResourceImage::FromResource("ANNOTATION_SQUARE"));
}

/**
 * copy constructor
 */
vxAnnotationBox::vxAnnotationBox( const vxAnnotationBox & other ) : vxAnnotation( other )
{
	CommonInit();
}//copy constructor


/**
 *Clone method
 */
vxAnnotationBox * vxAnnotationBox::Clone() const
{
	return new vxAnnotationBox( *this );
}//Clone method



/**
 * Reset
 */
void vxAnnotationBox::Reset()
//*******************************************************************
{
  // call base class
  vxAnnotation::Reset();

  // initialize all the local stuff
  CommonInit();
} // ~Reset()


/**
 * Get the position of a given handle.
 * @param index of handle
 * @param point of handle
 */
void vxAnnotationBox::SetHandle(const uint2 uHandleIndex, 
                                const vxHandle & handle)
//*******************************************************************
{
  static float4 fEpsilon(0.0001F);
  static int vEdgeMiddleHandleIndices[4] = { 1, 5, 7, 3 };
  static int vEdgeHandleIndices[4][3] = { {0,1,2}, {2,5,8}, {8,7,6}, {6,3,0} };
  static int vCornerHandleIndices[4]  = { 0, 2, 8, 6 };
  static int vCornerHorizontalHandleIndices[4] = { 6, 8, 2, 0 };
  static int vCornerVerticalHandleIndices[4]   = { 2, 0, 6, 8 };

  if (GetStatus() == vxAnnotation::CREATE)
  {
    if (GetNumHandles() == 2)
    {
      vxAnnotation::SetHandle(uHandleIndex, handle);

      Point<float4> vCorners[4];
      vxUtils::ComputeROIcorners(*GetEnvironment(), GetHandle(0).GetPosition(), GetHandle(1).GetPosition(), vCorners);

      if ((vCorners[1] - vCorners[0]).GetLength() < fEpsilon || (vCorners[3] - vCorners[0]).GetLength() < fEpsilon)
      {
        vxHandle handle0(GetHandle(0));

        Normal3Df normalRight(GetEnvironment()->GetCamera()->GetViewRight()); 
        Vector3Df vectorRight(normalRight); 
        Normal3Df normalUp(GetEnvironment()->GetCamera()->GetViewUp()); 
        Vector3Df vectorUp(normalUp); 
        Point3Df position(handle0.GetPosition() + handle0.GetHandleWorldSize() / handle0.GetHandleScreenSize() * vectorRight 
                                                + handle0.GetHandleWorldSize() / handle0.GetHandleScreenSize() * vectorUp);
        vxHandle tmpHandle(GetHandle(uHandleIndex));
        tmpHandle.SetPosition(position);
        vxAnnotation::SetHandle(uHandleIndex, tmpHandle);

        return;
      }
    }
  }
  else if (GetStatus() == vxAnnotation::DONE)
  {
    bool bAltered(false);

    // make a copy of handles
    ///////////////////////////
    vxHandle vHandle[9];
    Point3Df vHandlePos[9];
    for (int i(0); i<GetNumHandles(); i++)
    {
      vHandle[i]    = GetHandle(i);
      vHandlePos[i] = vHandle[i].GetPosition();
    }

    // check for the selected edge handle
    ///////////////////////////////////////
    Vector3Df vector(handle.GetPosition() - vHandlePos[uHandleIndex]);
    if (vector.GetLength() < fEpsilon)
    {
      return;
    }

    if (uHandleIndex == 4)
    {
      for (int i(0); i<GetNumHandles(); i++)
      {
        vHandlePos[i] = vHandlePos[i] + vector;
      }
      bAltered = true;
    }
    else
    {
      Vector3Df viewRight( vHandlePos[2] == vHandlePos[0] ? Vector3Df( 1, 0, 0 ) : vHandlePos[2] - vHandlePos[0]);
      Vector3Df viewUp( vHandlePos[6] == vHandlePos[0] ? Vector3Df( 0, 1, 0 ) : vHandlePos[6] - vHandlePos[0]);
      viewRight.Normalize();
      viewUp.Normalize();
      const float4 fDotRight(vector.Dot(viewRight));
      const float4 fDotUp(vector.Dot(viewUp));
      Vector<float4> right(fDotRight * viewRight);
      Vector<float4> up(fDotUp * viewUp);

      // check for all edges
      ////////////////////////
      for (int j(0); j<4; j++)
      {
        if (uHandleIndex == vEdgeMiddleHandleIndices[j])
        {
          Vector<float4> deltaVector(handle.GetPosition() - vHandlePos[uHandleIndex]);
          if (j & 1) // is it left or right edge
            deltaVector = right;
          else
            deltaVector = up;

          for (int i(0); i<3; i++)
          {
            vHandlePos[vEdgeHandleIndices[j][i]] = vHandlePos[vEdgeHandleIndices[j][i]] + deltaVector;
            bAltered = true;
          }

          // adjust the other center handles to move proportionally
          vHandlePos[1] =  0.5 * (vHandlePos[0] + Vector3Df(vHandlePos[2]));
          vHandlePos[3] =  0.5 * (vHandlePos[0] + Vector3Df(vHandlePos[6]));
          vHandlePos[5] =  0.5 * (vHandlePos[2] + Vector3Df(vHandlePos[8]));
          vHandlePos[7] =  0.5 * (vHandlePos[6] + Vector3Df(vHandlePos[8]));
          vHandlePos[4] = 0.25 * (vHandlePos[0] + Vector3Df(vHandlePos[2]) + Vector3Df(vHandlePos[6]) + Vector3Df(vHandlePos[8]));

          bAltered = true;
        } // if handle is middle handle of edge
      } // for all handles

      // check for all corners
      //////////////////////////
      for (int j(0); j<4; j++)
      {
        if (uHandleIndex == vCornerHandleIndices[j])
        {
          // rotation desired
          /////////////////////
          Vector3Df previous(vHandlePos[uHandleIndex] == vHandlePos[4] ? Vector3Df( 1, 0, 0 ) : vHandlePos[uHandleIndex] - vHandlePos[4]);
          Vector3Df now(handle.GetPosition() == vHandlePos[4] ? Vector3Df( 1, 0, 0 ) : handle.GetPosition() - vHandlePos[4]);
          previous.Normalize();
          now.Normalize();
          float4 fAngle(acos(now.Dot(previous)));

          LogDbg("angle  : " + ToAscii(fAngle), "", "");

          if (fabs(fAngle) > fEpsilon)
          {
            if ( now != previous )
            {
              Normal3Df normal(now.Cross(previous));
              Normal3Df upToPrevious(normal.Cross(previous));
              float4 fAngle90(acos(upToPrevious.Dot(now)));
              LogDbg("angle90: " + ToAscii(fAngle90), "", "");

              for (int i(0); i<9; i++)
              {
                if (i != 4)
                {
                  Matrix<float4> matrix;
                  matrix.Rotate(normal , fAngle);
                  Vector3Df vectorToHandle(vHandlePos[i] - vHandlePos[4]);
                  float4 fLength(vectorToHandle.GetLength());
                  vectorToHandle.Normalize();
                  vectorToHandle = matrix * vectorToHandle;

                  vHandlePos[i] = vHandlePos[4] + fLength * vectorToHandle;
                }
              }

              bAltered = true;
            }
          }

          bAltered = true;
        } // if handle is corner handle
      } // for all handles
    } // not four handles?

    float4 fLengthX((vHandlePos[2] - vHandlePos[0]).GetLength());
    float4 fLengthY((vHandlePos[6] - vHandlePos[0]).GetLength());

    if (bAltered == true && fLengthX > fEpsilon && fLengthY > fEpsilon)
    {
      for (int i(0); i<GetNumHandles(); i++)
      {
        vHandle[i].SetPosition(vHandlePos[i]);
        vxAnnotation::SetHandle(i, vHandle[i]);
      }
    }
  } // status == DONE?

  return;
} // SetHandle()


/**
 * Render the annotation
 * @param environment
 */
void vxAnnotationBox::Render(vxEnvironment & environment)
//*******************************************************************
{
  static int vEdgeCornerHandleIndices[4][2] = { {0,2}, {2,8}, {8,6}, {6,0} };
  static int vEdgeHandleIndices[4][3] = { {0,1,2}, {2,5,8}, {8,7,6}, {6,3,0} };

  vxViewerType & viewerType(*environment.GetViewerType());

  if (viewerType.IsAnnotationSupported(*this))
  {
    if (viewerType.IsVisible(*this))
    {
      if (GetNumHandles() == 2)
      {
        DrawBox(environment);
      }
      else if (GetNumHandles() == 9)
      {
        for (int j(0); j<2; j++)
        {
          for (int i(0); i<4; i++)
          {
            // determine color and line width
            PixelRGBA<uint1> color;

            glLineWidth( m_fLineWidth );
            if (IsLabelSelected() == true || IsTextSelected() == true 
                                          || GetSelectedHandleIndex() == vEdgeHandleIndices[i][0]
                                          || GetSelectedHandleIndex() == vEdgeHandleIndices[i][1]
                                          || GetSelectedHandleIndex() == vEdgeHandleIndices[i][2]
                                          || GetSelectedHandleIndex() == 4)
              color = GetColorSelected();
            else
              color = GetColor();

            if (j==0)
            {
              glLineWidth( m_fLineShadowWidth );
              color.R() = color.G() = color.B() = 0;
            }

            bool bScreenSpace = GetEnvironment()->GetViewerType()->GetRenderCoordinateSystem() == vxViewerType::eScreenSpace;

            glColor4f(color.R()/255.0, color.G()/255.0, color.B()/255.0, color.A()/255.0);

            glEnable(GL_LINE_SMOOTH);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

            Point3Df point0(GetHandle(vEdgeCornerHandleIndices[i][0]).GetPosition());
            Point3Df point1(GetHandle(vEdgeCornerHandleIndices[i][1]).GetPosition());

            if( bScreenSpace )
            {
              Point2D< int2 > p1;
              GetEnvironment()->GetViewerType()->ConvertWorldToScreen( point0, p1 );
              
              Point2D< int2 > p2;
              GetEnvironment()->GetViewerType()->ConvertWorldToScreen( point1, p2 );

              glBegin(GL_LINES);
                glVertex2i( p1.m_x, p1.m_y );
                glVertex2i( p2.m_x, p2.m_y );
              glEnd();

            }
            else
            {
              glBegin(GL_LINES);
                glVertex3f(point0[0], point0[1], point0[2]);
                glVertex3f(point1[0], point1[1], point1[2]);
              glEnd();
            }

          } // for all four edges
        } // for shadow and real line drawing
      } // status == DONE and nine handles?
      else
      {
        LogWrn("Box annotation needs either 2 or 9 handles", "vxAnnotationBox", "Render");
      }

      // call parent class
      vxAnnotation::Render(environment);
    } // annotation visible?
  } // annotation supported?

  return;
} // Render()


/**
 * Draw the arc representing the angle
 * @param environment
 */
void vxAnnotationBox::DrawBox(vxEnvironment & environment) const
//*******************************************************************
{
  Point<float4> vCorners[4];
  vxUtils::ComputeROIcorners(environment, GetHandle(0).GetPosition(), GetHandle(1).GetPosition(), vCorners);

  for (int j(0); j<2; j++)
  {
    // determine color and line width
    PixelRGBA<uint1> color;

    if (j==0)
    {
			glLineWidth( m_fLineShadowWidth );
      color = PixelRGBA<uint1>(0, 0, 0, GetColorSelected().A());
    }
    else
    {
      glLineWidth( m_fLineWidth );
      color = GetColorSelected();
    }

    glColor4f(color.R()/255.0, color.G()/255.0, color.B()/255.0, color.A()/255.0);

    bool bScreenSpace = environment.GetViewerType()->GetRenderCoordinateSystem() == vxViewerType::eScreenSpace;

    glBegin(GL_LINE_LOOP);
    for (int i(0); i<4; i++)
    {
      if( bScreenSpace )
      {
        Point2D< int2 > pos;
        environment.GetViewerType()->ConvertWorldToScreen( vCorners[i], pos );
        glVertex2i( pos.m_x, pos.m_y );
      }
      else
      {
        glVertex3f(vCorners[i][0], vCorners[i][1], vCorners[i][2]);
      }      
    }
    glEnd();
  } // for shadow and line drawing
} // DrawBox()


/**
 * Morph
 * @param environment
 */
void vxAnnotationBox::Morph(vxEnvironment & environment)
//*******************************************************************
{
  // call parent class
  vxAnnotation::Morph(environment);

  // create out of the few created handles the full set of handles
  Point<float4> vCorners[4];
  vxUtils::ComputeROIcorners(environment, GetHandle(0).GetPosition(), GetHandle(1).GetPosition(), vCorners);

  vxHandle handle(GetHandle(0));
  handle.SetPosition(0.5F * (vCorners[0] + Vector3Df(vCorners[1]))); 
  vxAnnotation::SetHandle(1, handle);

  handle.SetPosition(vCorners[1]);
  AddHandle(handle);

  handle.SetPosition(0.5F * (vCorners[0] + Vector3Df(vCorners[3])));
  AddHandle(handle);

  handle.SetPosition((vCorners[0] + Vector3Df(vCorners[1]) + Vector3Df(vCorners[2]) + Vector3Df(vCorners[3])) * 0.25F);
  AddHandle(handle);

  handle.SetPosition(0.5F * (vCorners[1] + Vector3Df(vCorners[2])));
  AddHandle(handle);

  handle.SetPosition(vCorners[3]);
  AddHandle(handle);

  handle.SetPosition(0.5F * (vCorners[3] + Vector3Df(vCorners[2])));
  AddHandle(handle);

  handle.SetPosition(vCorners[2]);
  AddHandle(handle);

  // update corner handles
  int vCornerHandleIndices[4]       = { 0, 2, 8, 6 };

  for (int i(0); i<4; i++)
  {
    handle = GetHandle(vCornerHandleIndices[i]);
    handle.SetColorHandle(PixelRGBA<uint1>(255, 255, 0, 255));
    vxHandlesObj::SetHandle(vCornerHandleIndices[i], handle);
  } // for all four corner handles

  // update the label and text position
  vxAnnotation::SetHandle(GetLabelHandleIndex(), GetHandle(GetLabelHandleIndex()));
  vxAnnotation::SetHandle(GetTextHandleIndex(), GetHandle(GetTextHandleIndex()));
} // Morph()


/**
 * Update the annotation text
 * @param environment
 */
void vxAnnotationBox::Update(vxEnvironment & environment)
//*******************************************************************
{
  if (GetNumHandles() == 9)
  {
    // since we are sampling the area, we need to make sure that we are starting at the edge 
    // that is inside. Otherwise, the values can change while dragging the edge that is outside.
    // Ideally, we would scan convert for MPR ortho views and sample for MPR oblique ...
    /////////////////////////////////////////////////////////////////////////////////////////////
    Triple<float4> worldSize(GetEnvironment()->GetIntensityVolume()->GetHeader().GetWorldDim());
    Box<float4> boxWorld(0, 0, 0, worldSize[0], worldSize[1], worldSize[2]);
    float4 fSignX(1), fSignY(1);
    uint4 iIndexOrigin(0);
    if (boxWorld.IsInside((Triplef)GetHandle(8).GetPosition()) == true && boxWorld.IsInside((Triplef)GetHandle(6).GetPosition()) == false || boxWorld.IsInside((Triplef)GetHandle(2).GetPosition()) == true && boxWorld.IsInside((Triplef)GetHandle(0).GetPosition()) == false)
    {
      fSignX = -1;
      iIndexOrigin += 2;
    }
    if (boxWorld.IsInside((Triplef)GetHandle(6).GetPosition()) == true && boxWorld.IsInside((Triplef)GetHandle(0).GetPosition()) == false || boxWorld.IsInside((Triplef)GetHandle(8).GetPosition()) == true && boxWorld.IsInside((Triplef)GetHandle(2).GetPosition()) == false)
    {
      fSignY = -1;
      iIndexOrigin += 6;
    }
    Vector3Df vectorHorizontal(fSignX * (GetHandle(5).GetPosition() - GetHandle(3).GetPosition()));
    Vector3Df vectorVertical(fSignY * (GetHandle(7).GetPosition() - GetHandle(1).GetPosition()));

    if (vectorHorizontal.GetLength() == 0.0F || vectorVertical.GetLength() == 0.0F)
    {
      SetText("");
      return;
    }

    Triple<float4> units(GetEnvironment()->GetIntensityVolume()->GetHeader().GetVoxelUnits());
    float4 fMinUnit(Min(units[0], Min(units[1], units[2])));
    Normal3Df normalHorizontal(vectorHorizontal);
    Normal3Df normalVertical(vectorVertical);
    Vector3Df incrementHorizontal(0.5F * fMinUnit * (Vector3Df)normalHorizontal);
    Vector3Df incrementVertical(0.5F * fMinUnit * (Vector3Df)normalVertical);
    int iStepsHorizontal(vectorHorizontal.GetLength() / incrementHorizontal.GetLength());
    int iStepsVertical(vectorVertical.GetLength() / incrementVertical.GetLength());

    vxBlockVolumeIterator<uint2> iter(*GetEnvironment()->GetIntensityVolume());
    Point3Df rayStartWorld, rayEndWorld;
    Vector3Df rayDirWorld;
    Point3Df intersectionWorld;
    Triple<float4> locationWorld;

    vxIntensityStatistic statistic;

    // go through the rectangle and compute max and average voxel values
    //////////////////////////////////////////////////////////////////////
    for (int j(0); j<=iStepsVertical; j++)
    {
      // get world coordinates of start and end
      ///////////////////////////////////////////
      rayStartWorld = GetHandle(iIndexOrigin).GetPosition() + (float4)j * incrementVertical;
      rayEndWorld   = rayStartWorld + vectorHorizontal;
      rayDirWorld   = rayEndWorld - rayStartWorld;

      // need to clamp it to volume boundary ...
      ////////////////////////////////////////////
      locationWorld.SetXYZ(rayStartWorld.X(), rayStartWorld.Y(), rayStartWorld.Z());
      if (boxWorld.IsInside(locationWorld) == false)
      {
        if (vxUtils::HitBoundingBox(boxWorld, rayStartWorld, rayDirWorld, intersectionWorld) == true)
        {
          rayStartWorld = intersectionWorld;
        }
        else
        {
          continue;
        }
      }
      locationWorld.SetXYZ(rayEndWorld.X(), rayEndWorld.Y(), rayEndWorld.Z());
      if (boxWorld.IsInside(locationWorld) == false)
      {
        if (vxUtils::HitBoundingBox(boxWorld, rayEndWorld, -rayDirWorld, intersectionWorld) == true)
        {
          rayEndWorld = intersectionWorld;
        }
        else
        {
          continue;
        }
      }
      float4 fNumSteps((rayEndWorld-rayStartWorld).GetLength() / (0.5*fMinUnit));
      Vector4Df rayStart(rayStartWorld[0]/units[0], rayStartWorld[1]/units[1], rayStartWorld[2]/units[2], 0);
      Vector4Df rayEnd(rayEndWorld[0]/units[0], rayEndWorld[1]/units[1], rayEndWorld[2]/units[2], 1);
      RaySegment raySegment(rayStart, rayEnd);
      Vector4Df rayIncrement(raySegment.GetDirection()/fNumSteps);
      vxBlockVolumeRayIterator<uint2> iterRay(*environment.GetIntensityVolume(), raySegment, rayIncrement);

      for (; !(iterRay.IsAtEnd()) ; ++iterRay)
      {
        if (iterRay.IsInsideVolume())
        {
          const int2 iVoxel(iterRay.GetVoxel());
          statistic.Add(iVoxel);
        }
      } // for all positions in scanline
    } // for all scanlines

    // compute results
    statistic.Compute();

    float4 fArea(vectorHorizontal.GetLength() * vectorVertical.GetLength());
    string sAreaOrVolume("Area");
    string sAreaOrVolumeUnits("mm^2");
    FormatAndSetStatisticText(statistic, sAreaOrVolume, fArea, sAreaOrVolumeUnits);
  } // if nine handles

  return;
} // Update


/**
 * Write object to XML node
 * @param node
 */
void vxAnnotationBox::ToXml(vxDOMElement & element) const
//*******************************************************************
{
  // call parent class
  vxAnnotation::ToXml(vxSerializer::AddElement(element, vxAnnotation::GetElementName(), "# parent class #"));
} // ToXml()


/**
 * Initialize from XML node
 * @param viewer type
 * @return successful?
 */
bool vxAnnotationBox::FromXml(vxDOMElement & element)
//*******************************************************************
{
  bool bSuccess(true);

  // call parent class
  bSuccess &= vxAnnotation::FromXml(vxSerializer::FindElement(element, vxAnnotation::GetElementName(), "# parent class #"));

  return bSuccess;
} // FormXml()


#undef FILE_REVISION


// Revision History:
// $Log: vxAnnotationBox.C,v $
// Revision 1.12  2007/02/12 14:23:56  geconomos
// added support for screen space rendering for oblique views
//
// Revision 1.11  2006/11/24 19:17:49  romy
// Put back Convariant virtual Clone methods back
//
// Revision 1.10  2006/11/23 06:23:15  romy
// cloning modified
//
// Revision 1.9  2006/10/20 20:55:00  romy
// Fixed issue 4612 . Now taking the voxel values instead of the average values
//
// Revision 1.8  2006/08/07 20:26:23  romy
// added vxUndoActionAnnotationAdjust
//
// Revision 1.7  2006/08/02 21:36:36  romy
// adjusted the line & line shadow width
//
// Revision 1.6  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.5  2005/11/18 21:14:53  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.4.2.2  2005/10/05 13:58:31  frank
// typo fixed
//
// Revision 1.4.2.1  2005/10/05 12:28:06  frank
// Issue #4458: Trying to prevent problems when normalizing vectors
//
// Revision 1.4  2005/05/24 13:50:02  michael
// Introduced concept of data objects to start with v3D_Data..../vxData...
// Has been done for DataColormapEditor, DataAnnotationCreate, and
// DataPlaneObliqueCreate
//
// Revision 1.3  2005/04/12 15:23:32  geconomos
// changed xml library to the vxDOM classes
//
// Revision 1.2  2004/04/06 16:24:17  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.39  2004/03/02 01:10:39  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.38.2.1  2004/02/27 21:32:47  michael
// fixed ellipse annotation creation resulting in an empty structure that would cause
// an exception when normalizing its diameter vectors. Two initial points are now
// guaranteed to be different
//
// Revision 1.38  2004/02/03 20:06:31  michael
// took out redundant code (was there for debugging only)
//
// Revision 1.37  2004/02/03 12:26:38  michael
// changed order of scan conversion so that it starts along edge that is inside
// the volume so that the values do not change due to dragging the outside
// edge, in case the box is partially outside the volume.
//
// Revision 1.36  2004/01/23 17:16:42  michael
// fixed bug in computation of area
//
// Revision 1.35  2004/01/23 03:51:31  michael
// reworked annotation to have a vxHandle class and a vxHandlesObj contains
// a vector of handles and access functions. The vxAnnotation derives from this
// one and overloads needed SetHandle() calls to make needed adjustments.
//
// Revision 1.34  2004/01/21 16:32:45  michael
// allows rotation now (corner handles) and scaling on edge handles
//
// Revision 1.33  2004/01/05 23:19:43  michael
// made ellipse and box to work properly for all handle adjustments
//
// Revision 1.32  2004/01/05 18:59:36  michael
// made it work for oblique planes, too.
//
// Revision 1.31  2003/09/22 21:11:50  michael
// added "mailto:"
//
// Revision 1.30  2003/06/18 15:19:51  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.29  2003/06/04 18:27:36  michael
// added namespace declaration
//
// Revision 1.28  2003/05/22 12:58:27  michael
// fixed bug in statistics: use GetSample(), not GetVoxel()!!!
//
// Revision 1.27  2003/05/16 13:17:39  frank
// formatting
//
// Revision 1.26  2003/05/05 14:12:45  michael
// fixed wrong comment
//
// Revision 1.25  2003/04/24 13:46:29  michael
// initial code reviews
//
// Revision 1.24  2003/04/02 21:01:13  michael
// fixed issue #2581
//
// Revision 1.23  2003/04/01 17:53:05  michael
// cosmetics
//
// Revision 1.22  2003/03/18 21:23:01  frank
// Distinguished between supporting specific annotations and supporting specific manipulators for viewer types. IsSupported() has become IsAnnotationSupported().
//
// Revision 1.21  2003/03/04 01:43:05  michael
// renaming of methods removing the double occurence of "Annotation", e.g.
// vxAnnotations::GetActiveAnnotation() --> vxAnnotations::GetActive()
//
// Revision 1.20  2003/03/01 22:42:05  michael
// fixed label and text position after creation
//
// Revision 1.19  2003/02/28 05:44:26  michael
// fixed issue #2860
//
// Revision 1.18  2003/02/27 23:12:27  michael
// made Render() non const since some annotations (e.g. arrow) need to adjust
// their size (handle locations) depending on the screen/world size which again
// depends on the actual viewport size.
//
// Revision 1.17  2003/02/26 19:26:58  geconomos
// Implementation of associated image.
//
// Revision 1.16  2003/02/25 22:16:25  michael
// fixed crash (member call of unitialized pointer)
//
// Revision 1.15  2003/02/22 00:22:56  michael
// fixed annotations to serialize and deserialize
//
// Revision 1.14  2003/02/13 16:56:47  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.13  2003/02/07 00:29:59  michael
// added annotation for components
//
// Revision 1.12.2.1  2003/02/12 22:49:48  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.12  2003/02/04 15:36:34  michael
// fixed ellipse annotation
//
// Revision 1.11  2003/01/30 12:46:42  michael
// adjusted code to use ray iterator to traverse the box/ellipse
//
// Revision 1.10  2003/01/30 00:36:29  michael
// using rayiterator and samples rather than voxels (in vxAnnotationBox)
//
// Revision 1.9  2003/01/29 12:59:28  michael
// added new statistic class (not ideal where it is now) and used it in avxAnnotationBox
//
// Revision 1.8  2003/01/28 23:41:38  michael
// fixed ellipse scaling ... dual scaling (corner handles) still to be done ...
//
// Revision 1.7  2003/01/28 17:02:05  michael
// fixed statistics of box annotation
//
// Revision 1.6  2003/01/27 23:18:49  michael
// added annotation stuff: flexible text location (handle index), multiple text lines etc.
//
// Revision 1.5  2003/01/22 21:41:56  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.4  2003/01/21 20:41:30  michael
// fixed angle annotation, improved arrow annotaion, adjusted lines to 3 pixel
// width and consistently using glColor4f() instead of glColor3f()
//
// Revision 1.3  2003/01/20 19:37:42  michael
// added ellipse annotation
//
// Revision 1.2  2003/01/20 04:52:14  michael
// added first version of box annotation. Not yet completed ...
//
// Revision 1.1  2003/01/19 22:26:24  michael
// Added new annotation
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
// Revision 1.2  2003/01/15 16:30:55  michael
// added defines for FILE_VERSION
//
// Revision 1.1  2003/01/14 23:25:51  michael
// added initial version of angle annotation
//
// Revision 1.4  2003/01/13 14:43:47  geconomos
// Set the active annotation in AddAnnotation().
//
// Revision 1.3  2003/01/10 17:14:46  michael
// fixed (compile) bug
//
// Revision 1.2  2003/01/10 14:20:58  michael
// fixed compilation
//
// Revision 1.1  2003/01/10 13:39:40  michael
// added initial version of distance annotation
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxAnnotationBox.C,v 1.12 2007/02/12 14:23:56 geconomos Exp $
// $Id: vxAnnotationBox.C,v 1.12 2007/02/12 14:23:56 geconomos Exp $
