// $Id: vxAnnotationEllipse.C,v 1.13 2007/02/12 14:23:56 geconomos Exp $
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
#include "vxAnnotationEllipse.h"
#include "vxViewerType.h"
#include "GLFontGeorge.h"
#include "vxEnvironment.h"
#include "vxBlockVolume.h"
#include "vxIntensityConverter.h"
#include "vxIntensityStatistic.h"
#include "vxBlockVolumeRayIterator.h"
#include "vxSerializer.h"
#include "vxUtils.h"
#include "Matrix.h"
#include "ResourceImage.h"
#include "vxAnnotationBox.h"


// namespaces
using namespace std;


// defines
#define FILE_REVISION "$Revision: 1.13 $"


/**
 * Default constructor
 */
vxAnnotationEllipse::vxAnnotationEllipse()
//*******************************************************************
{
  CommonInit();
}

/**
 * Copy Constructor
 */
vxAnnotationEllipse::vxAnnotationEllipse( const vxAnnotationEllipse & other ) : vxAnnotation( other )		
{
	CommonInit();
}//Copy Constructor


/**
 * Clone method
 */
vxAnnotationEllipse * vxAnnotationEllipse::Clone() const
{
	return new vxAnnotationEllipse( * this ); 
} //Clone method



/**
* CommonInit
*/
void vxAnnotationEllipse::CommonInit()
//*******************************************************************
{
  SetMouseCursor("CURSOR_ROI");
  SetImage(ResourceImage::FromResource("ANNOTATION_ELLIPSE"));
} // CommonInit()


/**
 * Reset
 */
void vxAnnotationEllipse::Reset()
//*******************************************************************
{
  // call base class
  vxAnnotation::Reset();

  // initialize all the local stuff
  CommonInit();
} // ~Reset()


/**
 * Get the position of a given handle
 * @param index of handle
 * @param position of handle
 */
void vxAnnotationEllipse::SetHandle(const uint2 uHandleIndex, 
                                    const vxHandle & handle)
//*******************************************************************
{
  static float4 fEpsilon(0.0001F);
  static int vEdgeMiddleHandleIndices[4] = { 1, 5, 7, 3 };
  static int vEdgeHandleIndices[4][3] = { {3,4,5}, {1,4,7}, {3,4,5}, {1,4,7} };

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

    vxAnnotation::SetHandle(uHandleIndex, handle);
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
    Vector<float4> vector(handle.GetPosition() - vHandlePos[uHandleIndex]);
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
      Vector3Df viewRight(vHandlePos[2] - vHandlePos[0]);
      if ( viewRight.GetLength() == 0.0F )
        viewRight = Vector3Df( 1, 0, 0 );
      viewRight.Normalize();
      Vector3Df viewUp(vHandlePos[6] - vHandlePos[0]);
      if ( viewUp.GetLength() == 0.0F )
        viewUp = Vector3Df( 0, 1, 0 );
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
            vHandlePos[vEdgeHandleIndices[j][i]] = vHandlePos[vEdgeHandleIndices[j][i]] + 0.5F * deltaVector;
            bAltered = true;
          }

          vHandlePos[uHandleIndex] = vHandlePos[uHandleIndex] + deltaVector;

          // update corner handles
          Point<float4> point;
          Point<float4> center(vHandlePos[4]);
          Vector<float4> horizontal(vHandlePos[5] - vHandlePos[4]);
          float4 fRadiusHorizontal(horizontal.GetLength());
          if (fRadiusHorizontal != 0.0F)
          {
            horizontal.Normalize();
          }
          Vector<float4> vertical(vHandlePos[1] - vHandlePos[4]);
          float4 fRadiusVertical(vertical.GetLength());
          if (fRadiusVertical != 0.0F)
          {
            vertical.Normalize();
          }

          int vAngles[4]  = { 315, 45, 135, 225 };
          int vIndices[4] = { 0, 2, 8, 6 };

          for (int i(0); i<4; i++)
          {
            point = CreatePointOnEllips(center, horizontal, vertical, fRadiusHorizontal, fRadiusVertical, vAngles[i]);
            vHandlePos[vIndices[i]] = point;
            bAltered = true;
          }
        }
      }
      static int vCornerHandleIndices[4] = { 0, 2, 8, 6 };
      static int vCornerHorizontalHandleIndices[4] = { 5, 3, 3, 5 }; // horizontal opposite
      static int vCornerVerticalHandleIndices[4] = { 7, 7, 1, 1 };   // vertical opposite

      // check for all corners
      //////////////////////////
      for (int j(0); j<4; j++)
      {
        if (uHandleIndex == vCornerHandleIndices[j])
        {
          // rotation desired
          /////////////////////
          Vector3Df previous(vHandlePos[uHandleIndex] - vHandlePos[4]);
          if ( previous.GetLength() == 0.0F )
            previous = Vector3Df( 1, 0, 0 );
          previous.Normalize();
          Vector3Df now(handle.GetPosition() - vHandlePos[4]);
          if ( now.GetLength() == 0.0F )
            now = Vector3Df( 0, 1, 0 );
          now.Normalize();
          float4 fAngle(acos(now.Dot(previous)));

          LogDbg("angle  : " + ToAscii(fAngle), "", "");

          if (fabs(fAngle) > fEpsilon)
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
                if ( vectorToHandle.GetLength() == 0.0F )
                  vectorToHandle = Vector3Df( 1, 0, 0 );
                vectorToHandle.Normalize();
                vectorToHandle = matrix * vectorToHandle;

                vHandlePos[i] = vHandlePos[4] + fLength * vectorToHandle;
              }
            }

            bAltered = true;
          }
        }
      }
    }
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
  }

  return;
} // SetHandle()


/**
 * Render the annotation
 * @param viewer type
 */
void vxAnnotationEllipse::Render(vxEnvironment & environment)
//*******************************************************************
{
  vxViewerType & viewerType(*environment.GetViewerType());

  if (viewerType.IsAnnotationSupported(*this))
  {
    if (viewerType.IsVisible(*this))
    {
      Point<float4> center;
      Vector<float4> horizontal;
      Vector<float4> vertical;
      float4 fRadiusHorizontal, fRadiusVertical;

      if (GetNumHandles() == 2)
      {
        ComputeEllipseParams(environment, GetHandle(0).GetPosition(), GetHandle(1).GetPosition(), 
                             center, horizontal, vertical, fRadiusHorizontal, fRadiusVertical);

      }
      else if (GetNumHandles() == 9)
      {
        center = GetHandle(4).GetPosition();
        horizontal = GetHandle(5).GetPosition() - GetHandle(4).GetPosition();
        fRadiusHorizontal = horizontal.GetLength();
        if (fRadiusHorizontal != 0.0F)
          horizontal.Normalize();
        vertical = GetHandle(1).GetPosition() - GetHandle(4).GetPosition();
        fRadiusVertical = vertical.GetLength();
        if (fRadiusVertical != 0.0F)
          vertical.Normalize();
      }
      else
      {
        LogWrn("Box annotation needs either 2 or 9 handles", "vxAnnotationBox", "Render");
      }

      DrawEllipse(center, horizontal, vertical, fRadiusHorizontal, fRadiusVertical);

      // call parent class
      vxAnnotation::Render(environment);

    } // annotation visible
  } // annotation supported

  return;
} // Render()


/**
 * Write object to XML node
 * @param node
 */
void vxAnnotationEllipse::ToXml(vxDOMElement & element) const
//*******************************************************************
{
  // call parent class
  vxAnnotation::ToXml(vxSerializer::AddElement(element, vxAnnotation::GetElementName(), "# parent class #"));
}


/**
 * Initialize from XML node
 * @param viewer type
 * @return successful?
 */
bool vxAnnotationEllipse::FromXml(vxDOMElement & element)
//*******************************************************************
{
  bool bSuccess(true);

  // call parent class
  bSuccess &= vxAnnotation::FromXml(vxSerializer::FindElement(element, vxAnnotation::GetElementName(), "# parent class #"));

  return bSuccess;
} // FromXml()


/**
 * Compute the four corner points out of the two given ones
 * @param environment
 * @param topleft
 * @param bottomleft
 * @param center
 * @param horizontal
 * @param vertical
 * @param radius horizontal
 * @param radius vertical
 */
void vxAnnotationEllipse::ComputeEllipseParams(vxEnvironment & environment, 
                                               const Point<float4> & topleft,
                                               const Point<float4> & bottomright,
                                               Point<float4> & center,
                                               Vector<float4> & horizontal,
                                               Vector<float4> & vertical,
                                               float4 & fRadiusHorizontal,
                                               float4 & fRadiusVertical) const
//*******************************************************************
{
  // assume that the camera (viewing coordinate system) defines
  // the top/bottom (right vector) left/right (up vector) edges
  // of the rectangle
  ////////////////////////////////////////////////////////////////
  Normal<float4> normal(environment.GetCamera()->GetViewRight()); 
  horizontal = (Vector<float4>)normal; 
  vxPlaneOblique<float4> plane1(normal, topleft);
  fRadiusHorizontal = 0.5F * plane1.GetDistance(bottomright);

  Point<float4> topright(topleft + 2.0F * fRadiusHorizontal * horizontal);
  vertical = topright - bottomright;
  fRadiusVertical  = 0.5F * vertical.GetLength();
  if (fRadiusVertical != 0.0F)
    vertical.Normalize();

  center = topleft + fRadiusHorizontal * horizontal - fRadiusVertical * vertical;

  return;
} // ComputeEllipseParams()


/**
 * Draw the ellipse
 * @param center
 * @param horizontal
 * @param vertical
 * @param radius horizontal
 * @param radius veritcal
 */
void vxAnnotationEllipse::DrawEllipse(const Point<float4> & center,
                                      const Vector<float4> & horizontal,
                                      const Vector<float4> & vertical,
                                      float4 fRadiusHorizontal,
                                      float4 fRadiusVertical) const
//*******************************************************************
{
  static int vEdgeCornerHandleIndices[4][2] = { {0,2}, {2,8}, {8,6}, {6,0} };
  static int vEdgeHandleIndices[4][3] = { {0,1,2}, {2,5,8}, {8,7,6}, {6,3,0} };

  glPushAttrib(GL_ALL_ATTRIB_BITS);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

  for (int j(0); j<2; j++)
  {
    // determine color and line width
    PixelRGBA<uint1> color;

    glLineWidth( m_fLineWidth );
    if (IsLabelSelected() == true || IsTextSelected() == true || GetSelectedHandleIndex() != -1)
      color = GetColorSelected();
    else
      color = GetColor();

    if (j==0)
    {
      glLineWidth( m_fLineShadowWidth );
      color.R() = color.G() = color.B() = 0;
    }

    glColor4f(color.R()/255.0, color.G()/255.0, color.B()/255.0, color.A()/255.0);

    bool bScreenSpace = GetEnvironment()->GetViewerType()->GetRenderCoordinateSystem() == vxViewerType::eScreenSpace;
    // draw the lines
    glBegin(GL_LINE_STRIP);
    for (int iAngle(0); iAngle<=360; iAngle++)
    {
      // convert to radians and modulo back to complete the circle (GL_LINE_LOOP is screwed up on some graphics cards)
      float4 fAngle( M_PI * iAngle / 180.0F );
      float4 fX( sin( fAngle ) );
      float4 fY( cos( fAngle ) );
      Point3D<float4> point( center + fX * fRadiusHorizontal * horizontal + fY * fRadiusVertical * vertical );
      
      if( bScreenSpace )
      {
        Point2D< int2 > pos;
        GetEnvironment()->GetViewerType()->ConvertWorldToScreen(  point, pos );
        glVertex2i( pos.m_x, pos.m_y );
      }
      else
      {
        glVertex3fv( & point[0] );
      }
    } // for all angles
    glEnd();
  } // for shadow and line drawing
  glPopAttrib();
} // DrawEllipse()


/**
 * Helper function to create point on ellipse
 * @param center
 * @param horizontal
 * @param vertical
 * @param radius horizontal
 * @param radius vertical
 * @return position
 */
Point<float4> vxAnnotationEllipse::CreatePointOnEllips(const Point<float4> & center,
                                                       const Vector<float4> & horizontal,
                                                       const Vector<float4> & vertical,
                                                       float4 fRadiusHorizontal,
                                                       float4 fRadiusVertical,
                                                       float4 fAngle)
//*******************************************************************
{
  const float4 fX(sin(M_PI*fAngle/180.0));
  const float4 fY(cos(M_PI*fAngle/180.0));
  return center + fX * fRadiusHorizontal * horizontal + fY * fRadiusVertical * vertical;
}


/**
 * Morph
 * @param environment
 */
void vxAnnotationEllipse::Morph(vxEnvironment & environment)
//*******************************************************************
{
  // call parent class
  vxAnnotation::Morph(environment);

  // create out of the few created handles the full set of handles
  Point<float4> point;
  Point<float4> center;
  Vector<float4> horizontal;
  Vector<float4> vertical;
  float4 fRadiusHorizontal, fRadiusVertical;
  ComputeEllipseParams(environment, GetHandle(0).GetPosition(), GetHandle(1).GetPosition(),
                        center, horizontal, vertical, fRadiusHorizontal, fRadiusVertical);


  // set/add the handles
  vxHandle handle(GetHandle(0));
  handle.SetPosition(center + fRadiusVertical * vertical);
  vxAnnotation::SetHandle(1, handle);

  handle.SetPosition(center);
  AddHandle(handle);

  handle.SetPosition(center - fRadiusHorizontal * horizontal);
  AddHandle(handle);

  handle.SetPosition(center);
  AddHandle(handle);

  handle.SetPosition(center + fRadiusHorizontal * horizontal);
  AddHandle(handle);

  handle.SetPosition(center);
  AddHandle(handle);

  handle.SetPosition(center - fRadiusVertical * vertical);
  AddHandle(handle);

  handle.SetPosition(center);
  AddHandle(handle);

  // update corner handles
  int vCornerHandleIndices[4]       = { 0, 2, 8, 6 };
  int vCornerNeighbourVertical[4]   = { 1, 1, 7, 7 };
  int vCornerNeighbourHorizontal[4] = { 3, 5, 5, 3 };
  
  for (int i(0); i<4; i++)
  {
    Point<float4> center(GetHandle(4).GetPosition());
    Vector<float4> horizontal(GetHandle(vCornerNeighbourHorizontal[i]).GetPosition() - center);
    Vector<float4> vertical(GetHandle(vCornerNeighbourVertical[i]).GetPosition() - center);
    Point<float4> point(GetHandle(4).GetPosition() + cos(M_PI/4.0F) * horizontal + sin(M_PI/4.0F) * vertical);

    handle.SetPosition(point);
    handle.SetColorHandle(PixelRGBA<uint1>(255, 255, 0, 255));
    vxAnnotation::SetHandle(vCornerHandleIndices[i], handle);
  } // for all four corner handles

  // update the label and text position
  vxAnnotation::SetHandle(GetLabelHandleIndex(), GetHandle(GetLabelHandleIndex()));
  vxAnnotation::SetHandle(GetTextHandleIndex(), GetHandle(GetTextHandleIndex()));
} // Morph()


/**
 * Update the annotation text
 * @param environment
 */
void vxAnnotationEllipse::Update(vxEnvironment & environment)
//*******************************************************************
{
  if (GetNumHandles() == 9)
  {

    Vector3Df horizontalDiameter( GetHandle(5).GetPosition() - GetHandle(3).GetPosition() );
    Vector3Df verticalDiameter( GetHandle(1).GetPosition() - GetHandle(7).GetPosition() );
    if ( horizontalDiameter.GetLength() == 0.0F ||
         verticalDiameter  .GetLength() == 0.0F )
    {
      SetText("");
      return;
    }

    // compute the statistics
    vxIntensityStatistic statistics;
    Point3Df firstPoint(GetHandle(0).GetPosition());
    vxUtils::ComputeEllipseStatistics( statistics, firstPoint, verticalDiameter, horizontalDiameter, * GetEnvironment()->GetIntensityVolume() );

    // compute the area
    Triple<float4> units( GetEnvironment()->GetIntensityVolume()->GetHeader().GetVoxelUnits() );
    float4 fMinUnit = Min(units[0], Min(units[1], units[2]));
    float4 fLengthHorizontal(horizontalDiameter.GetLength());
    float4 fLengthVertical(verticalDiameter.GetLength());
    float4 fArea(fLengthHorizontal*0.5F*fLengthVertical*0.5F*M_PI);

    string sAreaOrVolume("Area");
    string sAreaOrVolumeUnits("mm^2");
    FormatAndSetStatisticText( statistics, sAreaOrVolume, fArea, sAreaOrVolumeUnits );

  } // nine handles?

  return;
} // Update()


#undef FILE_REVISION


// Revision History:
// $Log: vxAnnotationEllipse.C,v $
// Revision 1.13  2007/02/12 14:23:56  geconomos
// added support for screen space rendering for oblique views
//
// Revision 1.12  2006/11/24 19:17:49  romy
// Put back Convariant virtual Clone methods back
//
// Revision 1.11  2006/11/23 06:23:15  romy
// cloning modified
//
// Revision 1.10  2006/08/07 20:26:23  romy
// added vxUndoActionAnnotationAdjust
//
// Revision 1.9  2006/08/02 21:37:32  romy
// adjusted the line & line shadow width
//
// Revision 1.8  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.7  2005/11/18 21:14:53  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.6.2.1  2005/08/17 12:18:08  frank
// Issue #4369: Prevented exception when normalizing zero-length vectors. Apparently revision 1.31.2.1 did not catch all cases
//
// Revision 1.6  2005/05/24 13:50:02  michael
// Introduced concept of data objects to start with v3D_Data..../vxData...
// Has been done for DataColormapEditor, DataAnnotationCreate, and
// DataPlaneObliqueCreate
//
// Revision 1.5  2005/05/17 16:30:36  frank
// fixed the extra lines in the ellipse on some machines
//
// Revision 1.4  2005/05/12 16:54:52  frank
// added opacity to the ellipse rendering
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
// Revision 1.32  2004/03/02 01:10:39  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.31.2.1  2004/02/27 21:32:48  michael
// fixed ellipse annotation creation resulting in an empty structure that would cause
// an exception when normalizing its diameter vectors. Two initial points are now
// guaranteed to be different
//
// Revision 1.31  2004/02/10 13:33:32  michael
// cosmetics
//
// Revision 1.30  2004/01/23 17:20:16  michael
// fixed bug in computation of area
//
// Revision 1.29  2004/01/23 16:32:06  frank
// computed and displayed Find Region with sampling for oblique & 3D views
//
// Revision 1.28  2004/01/23 03:51:31  michael
// reworked annotation to have a vxHandle class and a vxHandlesObj contains
// a vector of handles and access functions. The vxAnnotation derives from this
// one and overloads needed SetHandle() calls to make needed adjustments.
//
// Revision 1.27  2004/01/21 16:32:45  michael
// allows rotation now (corner handles) and scaling on edge handles
//
// Revision 1.26  2004/01/05 23:19:43  michael
// made ellipse and box to work properly for all handle adjustments
//
// Revision 1.25  2004/01/05 18:59:36  michael
// made it work for oblique planes, too.
//
// Revision 1.24  2003/09/22 21:11:50  michael
// added "mailto:"
//
// Revision 1.23  2003/06/18 15:19:51  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.22  2003/06/04 18:27:36  michael
// added namespace declaration
//
// Revision 1.21  2003/05/22 12:58:27  michael
// fixed bug in statistics: use GetSample(), not GetVoxel()!!!
//
// Revision 1.20  2003/05/16 13:17:40  frank
// formatting
//
// Revision 1.19  2003/04/24 13:46:29  michael
// initial code reviews
//
// Revision 1.18  2003/04/02 21:01:13  michael
// fixed issue #2581
//
// Revision 1.17  2003/04/01 17:53:05  michael
// cosmetics
//
// Revision 1.16  2003/03/18 21:23:01  frank
// Distinguished between supporting specific annotations and supporting specific manipulators for viewer types. IsSupported() has become IsAnnotationSupported().
//
// Revision 1.15  2003/03/04 01:43:06  michael
// renaming of methods removing the double occurence of "Annotation", e.g.
// vxAnnotations::GetActiveAnnotation() --> vxAnnotations::GetActive()
//
// Revision 1.14  2003/03/01 22:42:05  michael
// fixed label and text position after creation
//
// Revision 1.13  2003/02/28 05:44:26  michael
// fixed issue #2860
//
// Revision 1.12  2003/02/27 23:12:27  michael
// made Render() non const since some annotations (e.g. arrow) need to adjust
// their size (handle locations) depending on the screen/world size which again
// depends on the actual viewport size.
//
// Revision 1.11  2003/02/26 19:26:58  geconomos
// Implementation of associated image.
//
// Revision 1.10  2003/02/22 00:22:56  michael
// fixed annotations to serialize and deserialize
//
// Revision 1.9  2003/02/13 16:56:47  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.8  2003/02/07 00:29:59  michael
// added annotation for components
//
// Revision 1.7.2.1  2003/02/12 22:49:48  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
//
// Revision 1.7  2003/02/04 15:36:34  michael
// fixed ellipse annotation
//
// Revision 1.6  2003/01/30 12:46:42  michael
// adjusted code to use ray iterator to traverse the box/ellipse
//
// Revision 1.5  2003/01/30 00:36:29  michael
// using rayiterator and samples rather than voxels (in vxAnnotationBox)
//
// Revision 1.4  2003/01/28 23:41:38  michael
// fixed ellipse scaling ... dual scaling (corner handles) still to be done ...
//
// Revision 1.3  2003/01/22 21:41:56  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 1.2  2003/01/21 20:41:30  michael
// fixed angle annotation, improved arrow annotaion, adjusted lines to 3 pixel
// width and consistently using glColor4f() instead of glColor3f()
//
// Revision 1.1  2003/01/20 19:38:24  michael
// added ellipse annotation
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxAnnotationEllipse.C,v 1.13 2007/02/12 14:23:56 geconomos Exp $
// $Id: vxAnnotationEllipse.C,v 1.13 2007/02/12 14:23:56 geconomos Exp $
