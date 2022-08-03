// $Id: vxViewerType.C,v 1.13 2007/03/05 15:23:28 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Michael Meissner, (mailto:michael@viatronix.com)


// includes
#include "stdafx.h"
#include "vxViewerType.h"
#include "vxUtils.h"
#include "vxEnvironment.h"
#include "Point.h"
#include "vxBlockVolumeRayIterator.h"
#include "vxIntensityConverter.h"

/**
 * Constructor
 */
vxViewerType::vxViewerType(vxEnvironment & environment)
:m_environment(environment),
m_eRenderCoordinateSystem(eWorldSpace)
//*******************************************************************
{
	m_pRenderingEnginePlugin = NULL;
} // vxViewerType()

vxViewerType::~vxViewerType()
{
	delete m_pRenderingEnginePlugin;
	m_pRenderingEnginePlugin = NULL;
}


/**
 * Helper function for manipulators
 * @param mouseCoord mouse coordinate
 * @param worldCoord world coordinate
 * @return succesful
 */
bool vxViewerType::ConvertMouseToWorld(const Point2D<int2> & mouseCoord, 
                                       Point<float4> & worldCoord, 
                                       bool bConsiderRenderingMode) const
//*******************************************************************
{
  return ConvertScreenToWorld(ConvertMouseToScreen(mouseCoord), worldCoord, bConsiderRenderingMode);
}


/**
 * Get the screen coordinates from mouse coordinates
 * @param mouseCoord  mouse coordinates
 * @param screenCoord screen coordinates
 * @return succesful
 */
Point2D<int2> vxViewerType::ConvertMouseToScreen(const Point2D<int2> & mouseCoord) const
//*******************************************************************
{
  const Point2D<int2> reference(GetEnvironment().GetWinRect().GetReferencePoint().first, GetEnvironment().GetWinRect().GetReferencePoint().second);
  Point2D<int2> screenCoord(Bound((int2)0, (int2)mouseCoord.X(), (int2)(GetEnvironment().GetWinRect().GetViewportSize().first-1)),
                            Bound((int2)0, (int2)mouseCoord.Y(), (int2)(GetEnvironment().GetWinRect().GetViewportSize().second-1)));

  screenCoord += Point2D<int2>(reference.m_x, reference.m_y);

  return screenCoord;
}


/**
 * Get world coordinates from screen position using openGL frame buffer.
 * @param screenCoord screen coordinates
 * @param worldCoord  world coordinates
 * @return success
 */ 
bool vxViewerType::ConvertScreenToWorld(const Point2D<int2> & screenCoord, 
                                        Point<float4> & worldCoord, 
                                        bool bConsiderRenderingMode) const 
//*******************************************************************
{
  return ConvertScreenToWorldUsingOpenGL(screenCoord, worldCoord);
} // ConvertScreenToWorld()


/**
 * Get the world position of the given screen position
 * @param world position
 * @param screen position
 * @return succesful
 */
bool vxViewerType::ConvertWorldToScreen(const Point<float4> & worldCoord, 
                                        Point2D<int2> & screenCoord) const
//*******************************************************************
{
  return ConvertWorldToScreenUsingOpenGL(worldCoord, screenCoord);
} // ConvertWorldToScreen()


/**
* Get the world position of the given screen position.
* Call this version when you already have the OpenGL matrices and you need efficiency,
* or if you do not want to read it from the current OpenGL settings, but from another.
* @param world position
* @param screen position
* @param vieweport parameters
* @param model view matrix
* @param projection matrix
* @return succesful
*/
bool vxViewerType::ConvertWorldToScreen(const Point<float4> & worldCoord, 
                                        Point2D<int2> & screenCoord,
                                        const GLint * piViewport,
                                        const GLdouble * pfModelviewMatrix,
                                        const GLdouble * pfProjectionMatrix) const 
//*******************************************************************
{
  GLdouble screenX(0);
  GLdouble screenY(0);
  GLdouble screenZ(0);

  while (glGetError() != GL_NO_ERROR) // clear out error queue
    ;

  gluProject(worldCoord[0], worldCoord[1], worldCoord[2],
             pfModelviewMatrix, pfProjectionMatrix, piViewport,
             &screenX, &screenY, &screenZ);

  if (glGetError() != GL_NO_ERROR)
    return false;

  screenCoord = Point2D<int2>(screenX, screenY);

  return true;
} // ConvertWorldToScreen()


/**
 * Get world coordinates from screen position using openGL frame buffer.
 * @param screenCoord screen coordinates
 * @param worldCoord  world coordinates
 * @return success
 */ 
bool vxViewerType::ConvertScreenToWorldUsingOpenGL(const Point2D<int2> & screenCoord, 
                                                   Point<float4> & worldCoord) const 
//*******************************************************************
{
  // values returned.
  float4 fRed(0.0F);
  float4 fGreen(0.0F);
  float4 fBlue(0.0F);
  float4 fAlpha(0.0F);
  float vfCoord[3] = { 0.0F, 0.0F, 0.0F };

  // do actual backprojection
  glReadBuffer(GL_BACK);
  if (vxUtils::ReadFramebuffer(Point2D<uint4>(screenCoord.m_x, screenCoord.m_y),
                               fRed, fGreen, fBlue, fAlpha, vfCoord) == false)
  {
    return false;
  }

  worldCoord = Point<float4>(vfCoord[0], vfCoord[1], vfCoord[2]);

  return true;
} // ConvertScreenToWorldUsingOpenGL()


/**
 * Get the world position of the given screen position
 * @param world position
 * @param screen position
 * @return succesful
 */
bool vxViewerType::ConvertWorldToScreenUsingOpenGL(const Point<float4> & worldCoord, 
                                                   Point2D<int2> & screenCoord) const 
//*******************************************************************
{
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  // What is this supposed to do?
  // It doesn't make sense to compare world coordinates with the OpenGL viewport.
  //
  //const uint4 uSizeX(viewport[2]);
  //const uint4 uSizeY(viewport[3]);

  // is this really needed? I don't think this makes sense at all.
  // revisit after the February 2004 relases and take it out after testing.
  //if (worldCoord.X() < viewport[0] || worldCoord.X() >= viewport[2] || 
  //    worldCoord.Y() < viewport[1] || worldCoord.Y() >= viewport[3])
  //{
  //  return false;
  //}

  GLdouble mvmatrix[16];
  GLdouble projmatrix[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
  glGetDoublev(GL_PROJECTION_MATRIX, projmatrix); 

  GLdouble screenX(0);
  GLdouble screenY(0);
  GLdouble screenZ(0);

  while (glGetError() != GL_NO_ERROR) // clear out error queue
    ; 

  gluProject(worldCoord[0], worldCoord[1], worldCoord[2],
             mvmatrix, projmatrix, viewport,
             &screenX, &screenY, &screenZ);
  if (glGetError() != GL_NO_ERROR)
    return false;

  screenCoord = Point2D<int2>(screenX, screenY);

  return true;
} // ConvertWorldToScreenUsingOpenGL()


/**
 * Get the world position given a set of ray iterators
 *
 * @param dataIter  the intensity volume ray iterator
 * @param labelIter the label volume ray iterator
 * @return true if found a voxel
 */
bool vxViewerType::ConvertRayToWorld(vxBlockVolumeRayIterator<uint2> & dataIter,
                                     vxBlockVolumeRayIterator<uint2> & labelIter,
                                     Point3Df & worldPos) const
//******************************************************
{
  // check rendering mode
  const bool bMIPRendering(GetEnvironment().GetRenderingMode()->GetRenderingAlgorithm() ==  vxRenderingMode::MIP);
  const bool bXRay        (GetEnvironment().GetRenderingMode()->GetRenderingAlgorithm() ==  vxRenderingMode::XRAY);
  const Vector3Df units(GetEnvironment().GetIntensityVolume()->GetHeader().GetVoxelUnits());

  // the components in convenient sorted order
  const std::vector<vxComponent *> & vpComponentInfo(GetEnvironment().GetComponents()->ArrayByLabel());

  if (bMIPRendering || bXRay)
  {
    // If MIP, find the voxel with max intensity along the ray
    Vector4d<float4> maxLoc(dataIter.GetPosFloat());
    vxBlockVolume<uint2> * pVolume(static_cast< vxBlockVolume<uint2> * >(GetEnvironment().GetIntensityVolume()));
    vxVolumeHeader header(pVolume->GetHeader());
    int4 maxIntensity(vxIntensityConverter::GetMinValue(header, true));

    // loop over all samples
    for (; ! dataIter.IsAtEnd(); ++dataIter, ++labelIter)
    {
      // check label visiblity and if reached maximum
      if (vpComponentInfo[ labelIter.GetVoxel() ]->IsIntensityVisible() && maxIntensity < dataIter.GetVoxel())
      {
        maxIntensity = dataIter.GetVoxel();
        maxLoc = dataIter.GetPosFloat();
      }
    } // for all samples

    // convert from volume to world coordinates
    worldPos = Point3Df(maxLoc.X() * units.X(), maxLoc.Y() * units.Y(), maxLoc.Z() * units.Z());

    return true;
  } // if MIP || XRay
  else
  {
    // In normal mode go until 0.5 opacity is reached.
    Vector4d<float4> curLoc;
    float4 fAccumulatedOpacity(0.0F);
    float * pfRGBA(GetEnvironment().GetColormap()->GetTablePointerRGBA(GetEnvironment().GetIntensityVolume()->GetHeader()));

		int4 iMaxVoxelValue4Clamp(4094);
    std::string sModality("");
    sModality = GetEnvironment().GetIntensityVolume()->GetHeader().GetDicomFieldValue("0008", "0060", sModality);
    if( sModality != "CT" )
		{
	    int4 iHeaderVersion(0);
	    GetEnvironment().GetIntensityVolume()->GetHeader().GetVariableVolumeHeader().GetHdrVar("version", iHeaderVersion);

	    if( iHeaderVersion >= 5 )
	    {
	      GetEnvironment().GetIntensityVolume()->GetHeader().GetVariableVolumeHeader().GetHdrVar("maxValueRange", iMaxVoxelValue4Clamp);
		    if( iMaxVoxelValue4Clamp > 4095 ) iMaxVoxelValue4Clamp = 65534;
	    }
	    else
	    {
        iMaxVoxelValue4Clamp = 65534;
	    }
		}

    for (; !dataIter.IsAtEnd(); ++dataIter, ++labelIter)
    {
      // check if the component is visible
      if (vpComponentInfo[labelIter.GetVoxel()]->IsIntensityVisible())
      {
        // sample and accumulate opacity
        int4 iSample( min(iMaxVoxelValue4Clamp, dataIter.GetSampleInt()));
        float4 fAlpha(float(pfRGBA[ 4 * iSample + 3 ]) / float(16 * 255));
        fAccumulatedOpacity += (1.0F - fAccumulatedOpacity) * fAlpha;

        // check if exceeded the opacity threshold
        const float4 fOpacityThreshold(0.5F);
        if (fAccumulatedOpacity > fOpacityThreshold)
        {
          curLoc = dataIter.GetPosFloat();
          worldPos = Point<float4>(curLoc.X() * units.X(), curLoc.Y() * units.Y(), curLoc.Z() * units.Z());
          return true;
        }
      } // if component visible
    } // for all samples
  } // else MIP/X-ray

  return false;
} // ConvertRayToWorld()


/**
 * Processes the modified queue
 * @param pInfoList modified list
 */
void vxViewerType::Modified(vxModifiedInfoList & infoList)
//******************************************************
{
  // iterate throuh all modified infos
  for (const vxModifiedInfo * pInfo = infoList.Begin(); pInfo != NULL; pInfo = infoList.Next())
  {
    const type_info & elementType(pInfo->GetTypeInfo());

    /// vxAnnotation events
    if (elementType == typeid(vxAnnotations))
    {
      /// cast the modified info structure to indexed type
      const vxModifiedIndexedInfo * pIndexInfo = dynamic_cast<const vxModifiedIndexedInfo *>(pInfo);
      
      /// has the annotation benn selected from the gui?
      if (pInfo->GetFlag() == vxUtils::ANNOTATIONS_UPDATE_PLANE)
      {
        InitializeFrom(*(GetEnvironment().GetAnnotations()->Get(pIndexInfo->GetIndex())));
      }
    }
    else
    {
      OnModified(pInfo->GetTypeInfo(), *pInfo);
    }
  } // end for all vxModifiedInfo
} // Modified()


/**
 * Get left plane
 * @param tight fit or outer circle
 * @return left plane
 */
float4 vxViewerType::GetLeftPlane() const
//*******************************************************************
{
  bool bPrecision(false);
  float4 fResult;

  if (bPrecision == true)
  {
    Normal<float4> direction(GetEnvironment().GetCamera()->GetViewReference() - GetEnvironment().GetCamera()->GetEye());
    Point<float4> center(GetEnvironment().GetZoomFactor()->GetCenter());
    uint1 uIndex(direction.GetMaxAbsComponentCoord());
    switch(uIndex)
    {
    case 0:
      fResult = -fabs(0.5 * GetEnvironment().GetZoomFactor()->GetSize());
      break;
    case 1:
      fResult = -fabs(0.5 * GetEnvironment().GetZoomFactor()->GetSize());
      break;
    case 2:
      fResult = -fabs(0.5 * GetEnvironment().GetZoomFactor()->GetSize());
      break;
    default:
      break;
    }    
  } // if tight fit
  else
  {
    if (GetEnvironment().GetCamera()->GetAspectRatio() >= 1)
    {
      fResult = -tan((GetEnvironment().GetCamera()->GetFovDegrees() / 2.0F) * (M_PI/180.0F)) * GetNearPlane() * GetEnvironment().GetCamera()->GetAspectRatio();
    }
    else
    {
      fResult = -(tan((GetEnvironment().GetCamera()->GetFovDegrees() / 2.0F) * (M_PI/180.0F)) * GetNearPlane()) / GetEnvironment().GetCamera()->GetAspectRatio();
    }
   }

  return fResult;
} // GetLeftPlane()


/**
 * Get right plane
 * @param tight fit or outer circle
 * @return right plane
 */
float4 vxViewerType::GetRightPlane() const
//*******************************************************************
{
  return -GetLeftPlane();
} // GetRightPlane()


/**
 * return the bottom plane
 * @param tight fit or outer circle
 * @return Bottom plane
 */
float4 vxViewerType::GetBottomPlane() const
//*******************************************************************
{
  bool bPrecision(false);
  float4 fResult(0.0F);

  if (bPrecision == true)
  {
    Normal<float4> direction(GetEnvironment().GetCamera()->GetViewReference() - GetEnvironment().GetCamera()->GetEye());
    Point<float4> center(GetEnvironment().GetZoomFactor()->GetCenter());
    uint1 uIndex(direction.GetMaxAbsComponentCoord());

    switch(uIndex)
    {
    case 0:
      fResult = -fabs(0.5 * GetEnvironment().GetZoomFactor()->GetSize());
      break;
    case 1:
      fResult = -fabs(0.5 * GetEnvironment().GetZoomFactor()->GetSize());
      break;
    case 2:
      fResult = -fabs(0.5 * GetEnvironment().GetZoomFactor()->GetSize());
      break;
    default:
      break;
    }
  } // if tight fit
  else
  {
    fResult = -tan((GetEnvironment().GetCamera()->GetFovDegrees() / 2.0F) * (M_PI/180.0F)) * GetNearPlane();
  }

  return fResult;
} // GetBottomPlane()


/**
 * return the top plane
 * @param tight fit or outer circle
 * @return top plane
 */
float4 vxViewerType::GetTopPlane() const
//*******************************************************************
{
  return -GetBottomPlane();
} // GetTopPlane()


/**
 * Get near plane
 * @param tight fit or outer circle
 * @return near plane
 */
float4 vxViewerType::GetNearPlane() const
//*******************************************************************
{
  bool bPrecision(false);
  float4 fNearDist(GetEnvironment().GetCamera()->GetEye().GetEuclideanDistance(GetEnvironment().GetZoomFactor()->GetCenter()));
  fNearDist -= sqrt(3.0F) * GetEnvironment().GetZoomFactor()->GetSize();
  
  return Bound(0.001f, fNearDist, (std::numeric_limits<float>::max)());
} // GetNearPlane()


/**
 * Get far plane
 * @param tight fit or outer circle
 * @return far plane
 */
float4 vxViewerType::GetFarPlane() const
//*******************************************************************
{
  bool bPrecision(false);
  float4 fFarDist(GetEnvironment().GetCamera()->GetEye().GetEuclideanDistance(GetEnvironment().GetZoomFactor()->GetCenter()));
  fFarDist += sqrt(3.0F) * GetEnvironment().GetZoomFactor()->GetSize();

  return fFarDist;
} // GetFarPlane()


/*
 * Gets the rendering instance for this particular viewer type
 */
vxRenderingNodePlugin & vxViewerType::GetRenderingNodePlugin()
{
	if( m_pRenderingEnginePlugin == NULL && HasRenderingNodePlugin() )
  {
    m_pRenderingEnginePlugin = CreateRenderingEnginePlugin();
		if ( m_pRenderingEnginePlugin != NULL )
			m_pRenderingEnginePlugin->InitializeFromEnvironment();
  }
  return *m_pRenderingEnginePlugin;
} // GetRenderingNodePlugin()


// Revision History:
// $Log: vxViewerType.C,v $
// Revision 1.13  2007/03/05 15:23:28  geconomos
// code review preparation
//
// Revision 1.12  2007/02/05 20:17:20  geconomos
// change vxViewerType::GetRenderingEnginePlugin => GetRenderingNodePlugin
//
// Revision 1.11  2007/02/05 19:28:17  geconomos
// renamed  the following:
//   vxRenderingManager => vxRenderingNode
//   vxRenderingEngine => vxRenderingNodeThreadManager
//   vxRenderingEngineThread => vxRenderingNodeThread
//   vxRenderingEnginePlugin => vxRenderingNodePlugin
//
// Revision 1.10  2007/01/16 16:48:58  gdavidson
// Changed vxRenderingNodePlugin
//
// Revision 1.9  2006/12/04 20:29:12  romy
// Eliminated the Rendering Engine's excess Threads
//
// Revision 1.8  2006/08/03 14:07:50  geconomos
// added "rendering engine" support
//
// Revision 1.7  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.6  2005/11/18 21:15:41  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.5.6.1  2005/08/01 19:12:32  frank
// changed function name according to coding standards
//
// Revision 1.5  2004/10/05 14:36:11  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.4  2004/08/05 13:54:46  michael
// reduced code to e.g. have GetLeftPlane() return -GetRightPlane() instead of
// having the code twice with *-1.
//
// Revision 1.3  2004/04/26 18:58:39  michael
// moved GetLeft() / GetRight() / etc. from vxCamera to vxViewerType
//
// Revision 1.2  2004/04/06 16:27:47  frank
// unified environment access naming
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.33  2004/02/07 01:11:04  michael
// added one more comment ...
//
// Revision 1.32  2004/02/06 19:34:59  frank
// Issue #3726: Fixed projection logic
//
// Revision 1.31  2004/01/14 14:18:17  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.30  2004/01/05 23:19:03  michael
// added "mailto:"
//
// Revision 1.29  2004/01/05 19:43:17  michael
// making local variables const, adding @param/@return, and comments
//
// Revision 1.28  2003/12/23 23:02:38  wenli
// Add GetBoundingRect() to slove the ssue #3167: missing ablility to move the annotation text in curved
//
// Revision 1.27  2003/11/06 20:44:05  michael
// Fixed rendering mode related issues, renamed the "algorithm" to be the mode,
// and the display options to be the options so that things are consistent. Also
// fixed the progressive refinement that broke because George forgot to add
// the assignment of the mode in vxRenderingMode::operator=()
//
// Revision 1.26  2003/11/05 01:18:45  geconomos
// Updated to use new vxRenderingAlgorithmEnum
//
// Revision 1.25  2003/06/18 15:19:53  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.24  2003/06/09 16:10:16  michael
// took out redundant include
//
// Revision 1.23  2003/06/09 12:29:54  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.22  2003/06/04 18:33:54  michael
// accounting for new vxIntensityConverter parameters (header, not modality)
//
// Revision 1.21  2003/04/25 12:09:01  michael
// more code review
//
// Revision 1.20  2003/04/24 14:01:45  frank
// code review
//
// Revision 1.19  2003/04/16 14:34:51  michael
// performed initial code review
//
// Revision 1.18  2003/04/16 13:42:26  michael
// performed initial code review
//
// Revision 1.17  2003/04/11 17:59:51  frank
// Refactored 2D/3D selection code so that the same algorithm is used regardless of 2D or 3D and the mode (x-ray, MIP, thin slab, normal)
//
// Revision 1.16  2003/03/31 13:50:56  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.15  2003/03/20 21:28:16  frank
// Individual IsVisible() for a single point, rather than for a whole annotation
//
// Revision 1.14  2003/03/20 15:35:32  geconomos
// Added handling for updating views when an annotation is selected from the gui.
//
// Revision 1.13  2003/03/06 15:57:52  frank
// Moved Is2D and Is3D to vxViewerType
//
// Revision 1.12  2003/03/05 12:51:08  frank
// Extended interface to include the ability to render in either screen or world space.
//
// Revision 1.11  2003/03/04 17:57:29  frank
// comments
//
// Revision 1.10  2003/03/04 15:34:17  geconomos
// Added methods for processing parent environment's modified queue.
//
// Revision 1.9  2003/02/24 14:57:07  frank
// Added local coordinate transformation for non-OpenGL projections
//
// Revision 1.8  2003/01/22 21:48:01  ingmar
// added missing includes
//
// Revision 1.7  2003/01/22 15:07:02  michael
// adjusted code to account for our Y-flip in the GetScreenCoordinates()
//
// Revision 1.6  2003/01/17 00:33:22  michael
// further iteration on new annotations. Text now shows up and there is a utility
// in the viewerType that allows to pass in the OpenGL parameters in case the
// current one should not be used, e.g. as in render text of annotations. Also extended
// the glFontGeorge class to return width and height of the given string
//
// Revision 1.5  2003/01/15 23:53:49  michael
// added annotation manipulation and handle highlighting
//
// Revision 1.4  2003/01/15 16:29:52  michael
// made methods const that do not alter the state
//
// Revision 1.3  2002/12/18 18:22:02  michael
// moved OpenGL frame buffer reading to base class
//
// $Id: vxViewerType.C,v 1.13 2007/03/05 15:23:28 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxViewerType.C,v 1.13 2007/03/05 15:23:28 geconomos Exp $
