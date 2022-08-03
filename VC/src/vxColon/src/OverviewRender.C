// $Id: OverviewRender.C,v 1.34.2.11 2010/06/14 20:23:05 dongqing Exp $
//
// Copyright© 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeff Meade  jmeade@viatronix.net


// includes
//#define _WIN32_WINNT  0x0501 // Needed For SetThreadIdealProcessor(..)
#include "stdafx.h"
//#undef _WIN32_WINNT // Needed For SetThreadIdealProcessor(..)
#include "ReaderGlobal.h"
#include "SystemInfo.h"
#include "RenderUtil.h"
#include "ImageUtilities.h"
#include "V3DFile.h"
#include "vxUtils.h"
#include "InteractiveSegmentation.h"
#include "vxPreset.h"
#include "vxSerializer.h"
#include "vxVisualizationPreset.h"
#include "vxIntensityConverter.h"
#include "vxRenderingNodePluginRaytracer.h"
#include <iostream>
#include <fstream>
#include <utility>
#include "vxRenderingNodeThreadManager.h"
#include "vxEnvironment.h"

#include "OverviewRender.h"


// defines
#define FILE_REVISION "$Revision: 1.34.2.11 $"

// namespaces
using namespace ReaderLib;
using namespace vxCAD;
using namespace OpenGLUtils;


//////////////////////////////////////////////////////////////////////////
// OverViewRenderThread

/**
 * Constructor 
 *
 * @param rRenderContext   Overview render context
 * @param rVolRender       Volume render instance
 * @param rDataset         Dataset to be rendered
 * @param uId              Thread ID
 */
OverviewRenderThread::OverviewRenderThread(RenderContext& rRenderContext, VolumeRender& rVolRender, Dataset& rDataset, const uint4 uId)
 : m_rRenderContext(rRenderContext), RenderThread(rVolRender, rDataset, uId)
{
} // constructor


/**
 * Renders the object
 */
void OverviewRenderThread::Render()
{
  m_rVolRender.RaycastASExternal(m_uID, m_rRenderContext, m_rDataset);
} // Render()


namespace OpenGLUtils
{
  /// helper class, resets the attribute stack automatically upon going out of scope
  class GLAttribStack
  {
  public:
    /// constructor, pushes attribute stack
    GLAttribStack(GLbitfield uBits) { glPushAttrib(uBits); }
    /// destructor, pops attribute stack
    ~GLAttribStack() { glPopAttrib(); }
  };
} // namespace OpenGLUtils


////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OverviewRender class


/**
 * Constructor; Defer creation of rendering threads until first call to Render() method
 */
OverviewRender::OverviewRender() : m_bRenderReportArrow(false), m_pQuadObject(gluNewQuadric()), m_iFrameDisplayList(0),
  m_pRenderThread0(NULL), m_pRenderThread1(NULL),
  m_bRegistrationColoredCenterline(false), m_eCurrRenderMode(OverviewRender::eNormal),
  m_uRegistrationVerificationPreviewIndex(0),
  m_previousRenderState( vxCamera(), true, 1 ),
  m_bRaytracerInitialized( false )
{
  m_pRenderingEnginePluginRaytracer = new vxRenderingNodePluginRaytracer( *(new vxEnvironment()) );   
  
  // TODO: read these from the config file
  m_pRenderingEnginePluginRaytracer->SetMinRenderSize( Triple<uint4>( 256, 256, 0 ) );
  m_pRenderingEnginePluginRaytracer->SetMaxRenderSize( Triple<uint4>( 1024, 1024, 0 ) );
  m_pRenderingEnginePluginRaytracer->SetMinFramerate( 3.0F );
  m_pRenderingEnginePluginRaytracer->SetMaxFramerate( 16.0F );
  m_pRenderingEnginePluginRaytracer->GetRenderBuffer().SetDepthBufferFlags( vxRenderBuffer::DEPTH_IMAGE_VC );

  m_pRenderingEnginePluginRaytracer->SupportsAdaptiveRendering();

  m_pRenderingThreadManager = new vxRenderingNodeThreadManager();
} // Constructor


/** 
 * Destruction
 */
OverviewRender::~OverviewRender() 
{
  // End render threads (auto-delete their own memory)
  delete m_pRenderThread0;
	m_pRenderThread0 = NULL;

  delete m_pRenderThread1;
	m_pRenderThread1 = NULL;

	delete m_pRenderingThreadManager;
	m_pRenderingThreadManager = NULL;

	delete m_pRenderingEnginePluginRaytracer;
	m_pRenderingEnginePluginRaytracer = NULL;

  gluDeleteQuadric(m_pQuadObject);
  DeleteAllDisplayLists();
} // Destruction.


/**
 * build display lists: triangle/framebox/...
 *
 * @param rDataset  reference to a study.
 @ @return true when success, else, false.
 */
bool OverviewRender::BuildAllDisplayLists(Dataset & rDataset)
{
  // generate triangle.
  if (!BuildTriDisplayList(rDataset))
    return false;

  // generate framebox.
  if (!BuildFrameDisplayList(rDataset))
    return false;

  return true;
} // BuildAllDisplayLists()


/**
 * triangle display list
 *
 * @param rDataset  reference to a dataset.
 * @return true when success, else, false.
 */
bool OverviewRender::BuildTriDisplayList(Dataset & rDataset)
{
  try
  {
    int4 iNumSegments = rDataset.m_segmentList.GetNumUnsortedSegments();
    m_viTriDisplayList.resize(iNumSegments);

    for (int4 s(0); s < iNumSegments; s++)
    {
      m_viTriDisplayList[s] = glGenLists(1);
      if (m_viTriDisplayList[s])
      {
        glNewList(m_viTriDisplayList[s], GL_COMPILE);
        Segment & segment = rDataset.m_segmentList.GetUnsortedSegment(s);
        segment.GetTriangleStrip().RenderSegment(TransferFunctionVC::m_vNotSeenScheme1);
        glEndList();
      }
      else
      {
        throw ex::OutOfMemoryException(LogRec("Failed to build triangle display list","OverviewRender", "BuildTriDisplayList"));
      } // endif (m_viTriDisplayList[s])
    } // end-for (each segment)
  }
  catch (ex::VException)
  {
    return false;
  }
  catch (...)
  {
    throw ex::RuntimeException(LogRec("Failed to build triangle display list", "OverviewRender", "BuildTriDisplayList"));
  }

  return true;
} // BuildTriDisplayList()


/**
 * frame box display list
 *
 * @param rDataset  reference to a dataset.
 * @return true when success, else, false.
 */
bool OverviewRender::BuildFrameDisplayList(Dataset & rDataset)
{
  try
  {
    if (m_iFrameDisplayList != 0)
    {
      // the display list has already generated, just return.
      return true;
    }

    if (m_iFrameDisplayList = glGenLists(1))
    {
      glNewList(m_iFrameDisplayList, GL_COMPILE);

      float4 fXSz = rDataset.m_vDim.m_x * rDataset.m_vUnits.m_x;
      float4 fYSz = rDataset.m_vDim.m_y * rDataset.m_vUnits.m_y;
      float4 fZSz = rDataset.m_vDim.m_z * rDataset.m_vUnits.m_z;

      glLineWidth(1.0);
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      glColor3f(0.10F, 0.4F, 0.85F);
      DrawFrame(fXSz, fYSz, fZSz);
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      glEndList();
    }
    else
    {
      // can't allocate a list item.
      throw ex::OutOfMemoryException(LogRec("Failed to build framebox display list","OverviewRender", "BuildFrameDisplayList"));
    } // endif ()
  }
  catch (ex::VException)
  {
    return false; 
  }
  catch (...)
  {
    throw ex::RuntimeException(LogRec("Failed to build framebox display list","OverviewRender", "BuildFrameDisplayList"));
  }

  return true;
} // RenderFrame()


/**
 * draw framebox.
 *
 * @param fx  size of box in x-direction.
 * @param fy  size of box in y-direction.
 * @param fz  size of box in z-direction.
 */
void OverviewRender::DrawFrame(float4 fx, float4 fy, float4 fz)
{
  glBegin(GL_QUADS);
  glVertex3f( fx, 0.0, 0.0);
  glVertex3f( fx,  fy, 0.0);
  glVertex3f( fx,  fy,  fz);
  glVertex3f( fx, 0.0,  fz);

  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f(0.0, 0.0,  fz);
  glVertex3f(0.0,  fy,  fz);
  glVertex3f(0.0,  fy, 0.0);

  glVertex3f(0.0, 0.0, 0.0);
  glVertex3f( fx, 0.0, 0.0);
  glVertex3f( fx, 0.0,  fz);
  glVertex3f(0.0, 0.0,  fz);

  glVertex3f( fx,  fy, 0.0);
  glVertex3f(0.0,  fy, 0.0);
  glVertex3f(0.0,  fy,  fz);
  glVertex3f( fx,  fy,  fz);
  glEnd();
} // DrawFrame()


/*
 * delete ALL display lists: triangle, framebox.
 */
void OverviewRender::DeleteAllDisplayLists()
{
  // delete triange display list.
  DeleteTriDisplayList();

  // delete frame box display list.
  DeleteFrameDisplayList();
} // DeleteDiaplyList()


/*
 * delete triangle display list.
 */
void OverviewRender::DeleteTriDisplayList()
{
  for (int4 i(0); i < m_viTriDisplayList.size(); i++)
  {
    if (m_viTriDisplayList[i] != 0)
      glDeleteLists(m_viTriDisplayList[i],1);
  }
  m_viTriDisplayList.clear();
} // DeleteTriDisplayList()


/*
 * delete frame box display list.
 */
void OverviewRender::DeleteFrameDisplayList()
{
  if (m_iFrameDisplayList)
  {
    glDeleteLists(m_iFrameDisplayList, 1);
  }
  m_iFrameDisplayList = 0;
} // DeleteFrameDisplayList()


/**
 * render object: frame box.
 */
void OverviewRender::RenderFrame(Dataset & rDataset)
{
  if (m_iFrameDisplayList != 0)
  {
    // if the display list is generated, just call it.
    glCallList(m_iFrameDisplayList);
  }
  else
  {
    float4 fXSz = rDataset.m_vDim.m_x * rDataset.m_vUnits.m_x;
    float4 fYSz = rDataset.m_vDim.m_y * rDataset.m_vUnits.m_y;
    float4 fZSz = rDataset.m_vDim.m_z * rDataset.m_vUnits.m_z;

    glLineWidth(1.0);
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glColor3f(0.10F, 0.4F, 0.85F);
    DrawFrame(fXSz, fYSz, fZSz);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  } // endif ()
} // RenderFrame()


/**
 * render triangle strips
 *
 * @param rDataset            reference to dataset of which to display segments
 * @param viewModes           current view mode
 * @param bIndexRender        whether to render each segment in a different color
 * @param bIndexRenderSorted  If index rendering, whether to display sorted or all (unsorted) segments
 * @param vPickSegtColors[]   Colors in which to display the index render segments
 * @param bRefDisplayList     Whether to render from display list
 */
void OverviewRender::RenderTris(Dataset & rDataset, const VtxViewModes & viewModes, const bool bIndexRender,
                                const bool bIndexRenderSorted, const Triple<uint1> vPickSegtColors[], const bool bRefDisplayList)
{
  // Adjust model matrix for triangle coordinates in voxels
  // extracted from reduced resolution volume.
  // Be sure: the default GL RC is :
  //    GL_DEPTH_TEST: on
  //    GL_LIGHTING:   off
  //    GL_BLENDING:   off.
  glDisable(GL_NORMALIZE);
  glPushMatrix();
  {
    glEnable(GL_DEPTH_TEST); // -> should be removed after checking of other status.
    // Check if this is a non-shaded index-rendering for "pick" operation
    if (bIndexRender)
    {
      glDisable(GL_LIGHTING); // ?
      // this rendering occurs in the back buffer, it is never seen
      if (bIndexRenderSorted)
      {
        for (SegmentList::SelectedIterator itSel(rDataset.m_segmentList); !itSel.IsAtEnd(); itSel++)
        {
          // get the current segment
          Segment & segment = *itSel;
          segment.GetTriangleStrip().RenderSegment(vPickSegtColors[rDataset.m_segmentList.GetUnsortedSegmentIndex(int4(itSel))]);
        } // endfor ALL segments
      }
      else
      {
        for (SegmentList::UnsortedIterator itUnsort(rDataset.m_segmentList); !itUnsort.IsAtEnd(); itUnsort++)
        {
          // get the current segment
          Segment & segment = *itUnsort;
          segment.GetTriangleStrip().RenderSegment(vPickSegtColors[int4(itUnsort)]);
        } // endfor ALL segments
      }
    }
    else
    {
      // visible rendering
      glEnable(GL_LIGHTING);
      if ((viewModes & ViewModes::TRANSLUCENT) && (m_eCurrRenderMode != OverviewRender::eSegmentPicking) && (m_eCurrRenderMode != OverviewRender::eSegmentVerify)
        && (m_eCurrRenderMode != OverviewRender::eRegistrationVerify))
      { 
        // translucent: volume rendering.
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        for (SegmentList::SelectedIterator itSel(rDataset.m_segmentList); !itSel.IsAtEnd(); itSel++)
        {
          // get the current segment
          Segment & segment = *itSel;
          segment.GetTriangleStrip().RenderSegmentTTP(rDataset.m_pTrackball->GetCurrentMatrix());
        } // endfor all segments
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
      }
      else
      {
        // First, render all the selected polygons in the base color
        int4 iSegmentIndex = 0;
        if ( m_viCachedSeenValue.size() < rDataset.m_segmentList.GetNumSelectedSegments() )
        {
          m_viCachedSeenValue.resize( rDataset.m_segmentList.GetNumSelectedSegments(), -1 );
          m_vMarkingDisplayList.resize( rDataset.m_segmentList.GetNumSelectedSegments() );
        }
        for (SegmentList::SelectedIterator itSel(rDataset.m_segmentList); !itSel.IsAtEnd(); itSel++, iSegmentIndex++)
        {
          // get the current segment
          Segment & segment = *itSel;
          if (((uint1 *)rDataset.m_fields.m_paintVol.GetDataPtr()) && 
              !(viewModes & ViewModes::TRANSLUCENT) && 
              (viewModes & ViewModes::MISSED_REGIONS) && 
              ((m_eCurrRenderMode == OverviewRender::eNormal) || (m_eCurrRenderMode == OverviewRender::eRegisteredViews)))
          {

            // use a display list to improve the rendering performance
            const bool bUseCache = ( m_viCachedSeenValue[ iSegmentIndex ] == rDataset.m_fields.m_paintVol.m_uCurrentSeenVal );
            if ( bUseCache && m_vMarkingDisplayList[ iSegmentIndex ].GetValid() )
            {
              m_vMarkingDisplayList[ iSegmentIndex ].CallList();
            }
            else
            {
              m_viCachedSeenValue[ iSegmentIndex ] = rDataset.m_fields.m_paintVol.m_uCurrentSeenVal;
              m_vMarkingDisplayList[ iSegmentIndex ].BeginDefinition();
              segment.GetTriangleStrip().RenderSegment((uint1 *)rDataset.m_fields.m_paintVol.GetDataPtr(), 
                                                       rDataset.m_fields.m_paintVol.m_uCurrentSeenVal,
                                                       Triple<uint4>(1, rDataset.m_vDim.m_x, rDataset.m_vDim.m_x*rDataset.m_vDim.m_y),
                                                       Point<float4>(1.0/rDataset.m_vUnits.m_x, 1.0/rDataset.m_vUnits.m_y, 1.0/rDataset.m_vUnits.m_z));
              m_vMarkingDisplayList[ iSegmentIndex ].EndDefinition();
            }

          }
          else
          {
            if (bRefDisplayList)
            {
              // sorted segment -> unsorted segment
              int4 us = rDataset.m_segmentList.GetUnsortedSegmentIndex(int4(itSel));
              if (m_viTriDisplayList[us])
              {
                glCallList(m_viTriDisplayList[us]);
              }
              else
              {
                segment.GetTriangleStrip().RenderSegment(TransferFunctionVC::m_vNotSeenScheme1);
              }
            } // reference display list except ReportView.
            else
            {
              // not to reference to display list (in Report view).
              segment.GetTriangleStrip().RenderSegment(TransferFunctionVC::m_vNotSeenScheme1);
            } // endif (bRefDisplayList)
          } // endif this segment not selected yet
        } // endfor all segments
      } // endif ttp render

      // Then, if "picking" render the rest in color
      if (m_eCurrRenderMode == OverviewRender::eSegmentPicking || m_eCurrRenderMode == OverviewRender::eSegmentVerify)
      {
        // glDisable(GL_LIGHTING);
        for (int4 s=0; s<rDataset.m_segmentList.GetNumUnsortedSegments(); s++)
        {
          // get the current segment
          Segment & segment = rDataset.m_segmentList.GetUnsortedSegment(s);
          if (segment.IsSelected() != true)
          {
            segment.GetTriangleStrip().RenderSegment(vPickSegtColors[s]);
          } // endif this segment not selected yet
        } // endfor all segments
      } // endif picking
      glDisable(GL_LIGHTING);
    } // endif index rendering or not
  } 
  glPopMatrix();
  glEnable(GL_NORMALIZE);
} // RenderTris()


/**
 * Render A/S/C/Cross-sectional planes.
 * (note: the m_study should be replaced by m_pStudy --- be careful.)
 *
 * @param rDataset      reference to attached dataset.
 * @param viewModes   current view mode.
 */
void OverviewRender::RenderSlices(Dataset & rDataset, const VtxViewModes & viewModes)
{
  // no need to display these planes in VerifyView.
  if (m_eCurrRenderMode == eSegmentPicking || m_eCurrRenderMode == eSegmentVerify || m_eCurrRenderMode == eRegistrationVerify)
  {
    return;
  }

  OpenGLUtils::GLAttribStack bitstack( GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT );
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  // Draw CrossSectional plane.
  if (viewModes & ViewModes::CROSS_SECTION)
  {
    // determine if we need to update the oblique slice
    if ( rDataset.m_uObliqueViewIdentifier != rDataset.m_submarine.GetViewIdentifier() )
    {
      // oblique view identifier is set to match the submarine inside this function
      rDataset.CrossSectionCalc();
    }
    glColor4f(0.5F,0.5F,0.5F,0.9F);
    glBegin(GL_QUADS); 
    glVertex3f(rDataset.m_vObliqueBottomLeft.m_x,  rDataset.m_vObliqueBottomLeft.m_y,  rDataset.m_vObliqueBottomLeft.m_z);
    glVertex3f(rDataset.m_vObliqueTopLeft.m_x,     rDataset.m_vObliqueTopLeft.m_y,     rDataset.m_vObliqueTopLeft.m_z);
    glVertex3f(rDataset.m_vObliqueTopRight.m_x,    rDataset.m_vObliqueTopRight.m_y,    rDataset.m_vObliqueTopRight.m_z);
    glVertex3f(rDataset.m_vObliqueBottomRight.m_x, rDataset.m_vObliqueBottomRight.m_y, rDataset.m_vObliqueBottomRight.m_z);
    glEnd();
  } // endif draw CrossSectional plane

  glMatrixMode(GL_MODELVIEW); 
  glPushMatrix(); 
  {
    Triple<float4> vUnits(rDataset.m_vUnits);
    glScalef(vUnits.m_x, vUnits.m_y, vUnits.m_z);
    Point<float4> vOrthoPoint(rDataset.m_vOrthogonalPtWorld.GetDividedAnIso(vUnits));
    Triple<uint4> vDim(rDataset.m_vDim);

    // Draw Transverse plane
    if (viewModes & ViewModes::AXIAL)
    {
      glColor4f(1.0F,0.0F,0.0F,0.6F);
      glBegin(GL_QUADS);
      glVertex3f(0       , 0       , vOrthoPoint.m_z);
      glVertex3f(0       , vDim.m_y, vOrthoPoint.m_z);
      glVertex3f(vDim.m_x, vDim.m_y, vOrthoPoint.m_z);
      glVertex3f(vDim.m_x, 0       , vOrthoPoint.m_z);
      glEnd();
    } // endif draw Transverse plane

    // Draw Sagittal plane
    if (viewModes & ViewModes::SAGITTAL)
    {
      glColor4f(0.0F,1.0F,0.0F,0.6F);
      glBegin(GL_QUADS); 
      glVertex3f(vOrthoPoint.m_x, 0       , 0       );
      glVertex3f(vOrthoPoint.m_x, 0       , vDim.m_z);
      glVertex3f(vOrthoPoint.m_x, vDim.m_y, vDim.m_z);
      glVertex3f(vOrthoPoint.m_x, vDim.m_y, 0       );
      glEnd();
    } // endif draw Coronal plane

    // Draw Coronal plane
    if (viewModes & ViewModes::CORONAL)
    {
      glColor4f(0.0F,0.0F,1.0F,0.6F);
      glBegin(GL_QUADS); 
      glVertex3f(0       , vOrthoPoint.m_y, 0       );
      glVertex3f(0       , vOrthoPoint.m_y, vDim.m_z);
      glVertex3f(vDim.m_x, vOrthoPoint.m_y, vDim.m_z);
      glVertex3f(vDim.m_x, vOrthoPoint.m_y, 0       );
      glEnd();
    } // endif draw Sagittal plane
  }
  glMatrixMode(GL_MODELVIEW); 
  glPopMatrix();
} // RenderSlices()


/**
 * render skeleton
 *
 * @param viewModes   current view mode.
 * @param lineWidth   line width.
 */
void OverviewRender::RenderSkel(Dataset & rDataset, const VtxViewModes & viewModes, const float4 fLineWidth,
                                const Rect<int4> & rViewport)
{
  if ( (m_eCurrRenderMode == OverviewRender::eRegistrationVerify) || (m_eCurrRenderMode == OverviewRender::eRegisteredViews) )
  {
    // Render the centerline with the registration quality colors
    RenderRegistrationColoredCenterline(rDataset,fLineWidth);
  }
  else if ((m_eCurrRenderMode == OverviewRender::eSegmentVerify) || (m_eCurrRenderMode == OverviewRender::eSegmentPicking))
  {
    for (int4 s=0; s<rDataset.m_segmentList.GetNumSelectedSegments(); s++)
    {
      // get the current segment
      Segment & segment = rDataset.m_segmentList.GetSegment(s);
      // Draw main centerline
      SkeletonRender * pSkelRender = reinterpret_cast<SkeletonRender *> (& segment.GetCenterline(0));
      // when selecting, show each segment with gradient
      if (segment.IsRegularDirection() == false)
        pSkelRender->DrawBranch(Vector<float4>(0.0F, 1.0F, 0.0F), Vector<float4>(1.0F, 0.0F, 0.0F), fLineWidth);
      else
        pSkelRender->DrawBranch(Vector<float4>(1.0F, 0.0F, 0.0F), Vector<float4>(0.0F, 1.0F, 0.0F), fLineWidth);
    } // endfor all segments

    RenderConnections(rDataset, viewModes);
  }
  else
  {
    SkeletonRender * pSkelRender = reinterpret_cast<SkeletonRender *> (& rDataset.m_joinedSkeleton);
    pSkelRender->DrawBranch(Vector<float4>(0.0F, 1.0F, 0.0F), fLineWidth);    
  }
} // RenderSkel()


/**
 * render Simplified segments
 *
 * @param viewModes   current view mode.
 * @param lineWidth   line width.
 */
void OverviewRender::RenderSimplifiedSegments(Dataset & rDataset, const VtxViewModes & viewModes, const float4 fLineWidth,
                                              const Rect<int4> & rViewport, const float4 fRadius)
{
  glColor3f(0,0,1);
  glLineWidth(fLineWidth);
  
  for (int4 s=0; s<rDataset.m_straightSegmentNodes.size(); s++)
  {
    glPushMatrix(); 
    Point<float4> pos = rDataset.m_joinedSkeleton.GetWorldPos(rDataset.m_straightSegmentNodes[s]);
    glTranslated(pos.m_x, pos.m_y, pos.m_z);
    RenderSingleBookmark(fRadius);
    glPopMatrix();
  } // end loop over vertices

  glBegin(GL_LINE_STRIP);
  {
    for (int4 s=0; s<rDataset.m_straightSegmentNodes.size(); s++)
    {
      RenderUtil::GlVertex(rDataset.m_joinedSkeleton.GetWorldPos(rDataset.m_straightSegmentNodes[s]));
    } // end loop over vertices
  } // glBegin
  glEnd();
  
} // RenderSimplifiedSegments()


/**
 * render the object: connections between segments 
 *
 * @param viewModes   current view mode.
 */
void OverviewRender::RenderConnections(Dataset & rDataset, const VtxViewModes & viewModes)
{
  // only animate if we have a large window
  GLAttribStack glAttr(GL_CURRENT_BIT | GL_LINE_BIT);

  // loop over all the selected
  for (int4 s=1; s<rDataset.m_segmentList.GetNumSelectedSegments(); s++)
  {
    // get the current and previous segments
    Segment & segment1 = rDataset.m_segmentList.GetSegment(s-1);
    Segment & segment2 = rDataset.m_segmentList.GetSegment(s);

    Skeleton & skel1 = segment1.GetCenterline(0);
    Skeleton & skel2 = segment2.GetCenterline(0);

    Point<float4> startPoint = skel1.GetWorldPos(segment1.IsRegularDirection() ? skel1.GetNumOfElements()-1 : 0);
    Point<float4>   endPoint = skel2.GetWorldPos(segment2.IsRegularDirection() ? 0 : skel2.GetNumOfElements()-1);

    // Draw a line between the segments
    glColor3f(0,1,0);
    glLineStipple(2,0xAAAA); // 10101010
    glEnable(GL_LINE_STIPPLE);

    glBegin(GL_LINES); 
    {
      glColor3f(0.0F, 1.0F, 0.0F);
      glVertex3f(startPoint.m_x, startPoint.m_y, startPoint.m_z);
      glVertex3f(  endPoint.m_x,   endPoint.m_y,   endPoint.m_z);
    } 
    glEnd();
  } // endfor all selected segments
} // RenderConnections()


/**
 * render the object: connections between segments (in report)
 *
 * @param rReportStart  reference of a point<float4>.
 * @param rReportEnd    reference of a point<float4>.
 */
void OverviewRender::RenderReportConnection(const Point<float4> & rReportStart, const Point<float4> & rReportEnd)
{
  // Save the necessary OpenGL state values
  GLAttribStack glAttr(GL_CURRENT_BIT | // for changing the current color in glColor3f(0,1,0) below
                       GL_LINE_BIT);    // for glLineStipple(1, 0xAAAA) and glEnable(GL_LINE_STIPPLE) below

  // Draw a line between the segments
  glColor3f(0,1,0);
  glLineStipple(2,0xAAAA);
  glEnable(GL_LINE_STIPPLE);
  glLineWidth(3);
  glBegin(GL_LINES);
  {
    glVertex3f(rReportStart.m_x, rReportStart.m_y, rReportStart.m_z);
    glVertex3f(rReportEnd.m_x, rReportEnd.m_y, rReportEnd.m_z);
  } 
  glEnd();
} // RenderReportConnection().


/**
 * render the object: bookmarks
 *
 * @param rMarkList   reference to list of bookmarks
 * @param fRadius     radius of bookmarks
 */
void OverviewRender::RenderBookMarks(BookmarkList & rMarkList, const float4 fRadius)
{
  GLAttribStack glAttr(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);

  glEnable(GL_LIGHTING);
  glEnable(GL_COLOR_MATERIAL);

  // set color once, only change when necessary
  Bookmark::PriorityEnum eCurrPriorityColor = Bookmark::HIGH;
  const float4 fRed[3] = {0.8f,0,0};
  const float4 fOrange[3] = {0.8f,0.4f,0};
  const float4 fGreen[3] = {0,0.5f,0};
  glColor3fv(fRed);

  for (int4 iBk=0; iBk<rMarkList.size(); iBk++)
  {
    glPushMatrix();
    Point<float4> pos = rMarkList[iBk].m_position;
    glTranslated(pos.m_x, pos.m_y, pos.m_z);

    // set color once, only change when necessary
    if (eCurrPriorityColor != rMarkList[iBk].m_ePriority)
    {
      switch (rMarkList[iBk].m_ePriority)
      {
        case Bookmark::HIGH:    glColor3fv(fRed);       break;
        case Bookmark::MEDIUM:  glColor3fv(fOrange);    break;
        case Bookmark::LOW:     glColor3fv(fGreen);     break;
        default:                glColor3fv(fRed);       break;
      } // end-switch(m_priority)

      eCurrPriorityColor = rMarkList[iBk].m_ePriority;
    } // end-if

    RenderSingleBookmark(fRadius);
    glPopMatrix();
  } // endfor all bookmarks
} // RenderBookMarks()


/**
* render the object: cad findings
*
* @param rCadFinds   reference to cad findings list
* @param fRadius     radius of drawn findings markers
*/
void OverviewRender::RenderCadFindings( vxCadFindings & rCadFindings,
                                        std::map<std::string, ColonSession::FindingInfo> & cadFindingInformation,
                                        const int4 iCadSliderValue,
                                        const float4 fRadius )
{
  GLAttribStack glAttr(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);

  glEnable( GL_LIGHTING );
  glEnable( GL_COLOR_MATERIAL );
  GLfloat sourceLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
  GLfloat lightPos[] = { 640.0f, 640.0f, 640.0f, 1.0f };
  glLightfv( GL_LIGHT2, GL_DIFFUSE, sourceLight );
  glLightfv( GL_LIGHT2, GL_POSITION, lightPos );
  glEnable( GL_LIGHT2 );
  lightPos[ 1 ] = - lightPos[ 1 ];
  glLightfv( GL_LIGHT3, GL_DIFFUSE, sourceLight );
  glLightfv( GL_LIGHT3, GL_POSITION, lightPos );
  glEnable( GL_LIGHT3 );


  // get all the visible findings
  std::list< vxCadFinding * > visibleFindings;
  for (int4 i(0); i<rCadFindings.GetCount(); i++)
  {
    vxCadFinding & finding = rCadFindings.GetFinding( i );
    if ( RenderUtil::IsFindingVisible( finding, cadFindingInformation, iCadSliderValue, rdrGlobal.m_bShowExternalCadFindings )
      && RenderUtil::GetFindingDecision( finding, cadFindingInformation ) != ColonSession::FindingInfo::eDecisionDenied )
    {
      visibleFindings.push_back( & finding );
    }
  } // for all findings

  // draw black shadows
  glColor3f( 0, 0, 0 );
  for ( std::list< vxCadFinding * >::iterator iter = visibleFindings.begin();
        iter != visibleFindings.end();
        ++iter )
  {
    glPushMatrix();
    {

      const Vector3D<float4> & pos( ( * iter )->GetCentroidMM().GetVector() );
      glTranslated( pos.m_x, pos.m_y, pos.m_z );
      RenderSingleBookmark( fRadius * 1.2F );

    } // glPushMatrix()
    glPopMatrix();

  } // for all visible bookmarks

  glClear( GL_DEPTH_BUFFER_BIT );

  // draw spheres on top; use yellow for undecided and green for shown (i.e. true positives)
  const float4 fYellow[3] = { 0.5F, 0.5F, 0 };
  const float4 fGreen[3]  = { 0, 0.3F, 0.1F };

  ColonSession::FindingInfo::DecisionEnum eCurrVis(ColonSession::FindingInfo::eDecisionUndecided);
  ColonSession::FindingInfo::DecisionEnum eNewVis;
  glColor3fv( fYellow );

  for ( std::list< vxCadFinding * >::iterator iter = visibleFindings.begin();
        iter != visibleFindings.end();
        ++iter )
  {
    glPushMatrix();
    {

      if ( (eNewVis = RenderUtil::GetFindingDecision( **iter, cadFindingInformation )) != eCurrVis )
      {
        glColor3fv( eNewVis == ColonSession::FindingInfo::eDecisionUndecided ? fYellow : fGreen );
        eCurrVis = eNewVis;
      }

      const Vector3D<float4> & pos( ( * iter )->GetCentroidMM().GetVector() );
      glTranslated( pos.m_x, pos.m_y, pos.m_z );
      RenderSingleBookmark( fRadius );

    } // glPushMatrix()
    glPopMatrix();

  } // for all visible bookmarks

} // RenderCadFindings()


/**
 * render the object: bookmarks of matched registration features.
 *
 * @param rDataset  reference to attached dataset.
 * @param rRect   reference to the CRect
 */
void OverviewRender::RenderRegistrationMatchedFeatures(Dataset & rDataset, const float4 fRadius, 
  const Triple<uint1> vPickSegtColors[], GLFont * pGLFont)
{
  GLAttribStack glAttr(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);

  glEnable(GL_LIGHTING);
  glEnable(GL_COLOR_MATERIAL);
  const float4 fRed[3] = {0.8f,0,0};

  const int4 iNumOfFeatures = rdrGlobal.m_vRegMatchingFeatures.size();
  
  for (int i=0;i < iNumOfFeatures; i++)
  {
    glColor3ub(vPickSegtColors[i].m_x, vPickSegtColors[i].m_y, vPickSegtColors[i].m_z);
    Point<float4> pos = rDataset.m_joinedSkeleton.GetWorldPos((rDataset.GetOrientation() == DatasetOrientations::eSUPINE) ?
                                                               rdrGlobal.m_vRegMatchingFeatures[i].m_uSupineFeatureIndex :
                                                               rdrGlobal.m_vRegMatchingFeatures[i].m_uProneFeatureIndex );
    glPushMatrix(); 
    {
      glTranslated(pos.m_x, pos.m_y, pos.m_z);
      RenderSingleBookmark(fRadius);
    }
    glPopMatrix();
    RenderFeatureNumber(Point<float4>(pos.m_x+5,pos.m_y,pos.m_z),i, pGLFont);
  }
  const bool bSupine = (rDataset.GetOrientation() == DatasetOrientations::eSUPINE);
  
} // RenderRegistrationMatchedFeatures()



/**
 * render the object: bookmarks of all registration features, 
 * even the ones that are not matched.
 *
 * @param rDataset  reference to attached dataset.
 * @param rRect   reference to the CRect
 */
void OverviewRender::RenderRegistrationAllFeatures(Dataset & rDataset, const float4 fRadius, GLFont * pGLFont)
{
  GLAttribStack glAttr(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);

  glEnable(GL_LIGHTING);
  glEnable(GL_COLOR_MATERIAL);

  // Find out which set of features that is going to be bookmarked.
  //const int4 iNumOfFeatures = rdrGlobal.m_registration->GetNumberOfFeatures(orientation);
  const int4 iNumOfFeatures = rDataset.m_ivFeatures.size();
  
  for (int i=0;i < iNumOfFeatures; i++)
  {
    glColor3f(0,0,0);
    Point<float4> pos = rDataset.m_joinedSkeleton.GetWorldPos(rDataset.m_ivFeatures[i]);
    glPushMatrix();
      glTranslated(pos.m_x, pos.m_y, pos.m_z);
      RenderSingleBookmark(fRadius);
    glPopMatrix();  
    RenderFeatureNumber(Point<float4>(pos.m_x+5,pos.m_y,pos.m_z),i, pGLFont);
  }

  glColor3f(1,0,0);
  Point<float4> pos = rDataset.m_joinedSkeleton.GetWorldPos(rDataset.m_ivFeatures[rDataset.m_iFeatureToDisplayInfo]);
  glPushMatrix(); 
    glTranslated(pos.m_x, pos.m_y, pos.m_z);
    RenderSingleBookmark(fRadius+1);
  glPopMatrix();

} // RenderRegistrationAllFeatures


void OverviewRender::RenderFeatureNumber(Point<float4> vPosition, int1 iNumber, GLFont * pGLFont)
{
  GLAttribStack glAttr(GL_DEPTH_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);
  int1 vStrNum[5];
  sprintf(vStrNum, "%d", iNumber);
  pGLFont->CharBitmap(vPosition, Triple<float4>(0,0,0), vStrNum, true, Triple<float4>(1,1,1));
}



/**
 * Render the centerline with registration quality colors
 */
void OverviewRender::RenderRegistrationColoredCenterline(Dataset & rDataset, const float4 fLineWidth)
{
  DatasetOrientations::TypeEnum orientation = rDataset.GetOrientation(); 
  
  Vector<float4> vStartColor(0.0F, 1.0F, 0.0F);
  Vector<float4> vEndColor(0.5F, 0.0F, 0.0F);
  glLineWidth(fLineWidth);
  
  glBegin(GL_LINE_STRIP);
  {
    for ( int4 j=0; j<rDataset.m_joinedSkeleton.GetNumOfElements(); j++ )
    {
      Vector<float4> vColor;
      float4 corr = rDataset.m_joinedSkeleton.GetRegistrationConfidence(j);
      if (corr != -1.0F)
      {
        float4 fColorFactor = rdrGlobal.ColorFunction(corr);
        vColor = Vector<float4>(vStartColor * fColorFactor + vEndColor * (1.0F - fColorFactor));
        glColor3f (vColor.m_x, vColor.m_y, vColor.m_z);
        RenderUtil::GlVertex(rDataset.m_joinedSkeleton.GetWorldPos(j));
      }
    }
  }
  glEnd();
} // RenderRegistrationColoredCenterline



/**
 * Renders a single bookmark (at the current translated position in the current context)
 *
 * @param const float4 fRadius   Size of the bookmark
 */
void OverviewRender::RenderSingleBookmark(const float4 fRadius)
{
  gluSphere(m_pQuadObject, fRadius, 10, 4);
}


/**
 * render the object: camera position in an arrow.
 * (The camera is handled seperatly so it can be placed on top of the captured image?)
 *
 * @param rDataset  reference to the attached dataset.
 * @param rRect   reference to CRect of the current window.
 * @param fAspect aspect of fov.
 * @param bRenderReportArrow  render arrow in report when true.
 */
void OverviewRender::RenderCamera(Dataset & rDataset, const Rect<int4> & rRect, const float8 mProjMatrix[], 
  const float8 mModelViewMatrix[], const bool bRenderReportArrow, const int4 iArrowSize)
{
  Point<float4> vPosition;
  Vector<float4> vViewDirection;
  bool bCenter = true;

  if (bRenderReportArrow)
  {
    Normal<float4> vViewDir(0.0f, 0.0f, -1.0f);
    vViewDirection = m_vReportDirection.Cross(rDataset.m_pTrackball->GetCurrentMatrix() * vViewDir) * -1.0f;
    vViewDirection.m_z = 0.0f;
    vViewDirection.Normalize();
    vViewDirection = rDataset.m_pTrackball->GetCurrentMatrix().GetInverse() * vViewDirection;
    vPosition = m_vReportPosition;// (nViewDirection * 25.0f);
    bCenter = false;
  }
  else
  {
    vPosition = rDataset.m_submarine.GetPositionWorld();
    vViewDirection = Vector<float4>(rDataset.m_submarine.GetOrientation());
  } // end-if (bRenderReportArrow)

  float4 fViewDirLength = 10000.0F/rRect.Width() * iArrowSize;
  Point<float4> vEndPoint(vPosition + Vector<float4>(vViewDirection) * fViewDirLength);

  GLAttribStack glAttr(GL_TRANSFORM_BIT);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadMatrixd(mProjMatrix);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadMatrixd(mModelViewMatrix);

  // draw the camera position as a 3D arrow
  glColor3f(1.0F, 1.0F, 0.0F);
  RenderUtil::RenderArrow( vPosition, Normal<float4>(vViewDirection), fViewDirLength, bCenter, 0, PixelRGBA<float4>(0.3F, 0.0F, 0.5F, 1.0F), true );
  glPopMatrix();

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
}  // RenderCamera()


/**
 * render the object: the segment number when it is is picked.
 * 
 * @param rDataset reference to attached dataset.
 * @param glFont  reference to glfont.
 */
void OverviewRender::RenderSegNum(Dataset & rDataset, GLFont * pGLFont)
{
  GLAttribStack glAttr(GL_DEPTH_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);
  Point<float4> vPoint, vNormal;
  for (int4 s=0; s<rDataset.m_segmentList.GetNumSelectedSegments(); s++)
  {
    // get the current segment
    Segment & segment = rDataset.m_segmentList.GetSegment(s);
    // get main centerline
    SkeletonRender * pSkelRender = reinterpret_cast<SkeletonRender *> (& segment.GetCenterline(0));

    int4 iMiddle = pSkelRender->GetNumOfElements() / 2;
    Point<float4> vPosition(segment.GetCurrentSkeleton().GetWorldPos(iMiddle));

    int1 vStrNum[5];
    sprintf(vStrNum, "%d", s+1);
    pGLFont->CharBitmap(vPosition, Triple<float4>(0,0,0), vStrNum, true, Triple<float4>(1,1,1));
  } // endfor all segments
} // RenderMidPtOfSkel().


/** 
 * render the object: colon (volume rendering).
 *
 * @param rDataset  reference to attached dataset.
 * @param bIsMainViewWindow   true if current view window is a main one.
 * @param fZoomFactor         equals to any isotropic scale factor passed to glScale, if any. Otherwise, it is 1.0
 * @param fFov                The field of view 
 * @param bTransparent        flag for isosurface and transparency mode
 */
void OverviewRender::RayCastVolume(Dataset & rDataset, bool bIsMainViewWindow, float4 fZoomFactor, float4 fFov, uint4 uViewWth, uint4 uViewHgt)
{
  // first see if we can actually allocate and create the opacity volume
  // if not, then don't try this again
  if ( ! rDataset.m_fields.IsOpacityVolumeCreated() && rDataset.m_fields.GetOpacityVolumePossible() )
  {
    try
    {
      rDataset.BuildOpacityVol();
    }
    catch ( ... )
    {
      LogWrn( "Unable to create the opacity volume - probably due to lack of available memory", "OverviewRender", "RayCastVolume" );
      rDataset.m_fields.SetOpacityVolumePossible( false );
      return;
    }
  }

  // go ahead and render
  GLAttribStack glAttr(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT | GL_TRANSFORM_BIT);
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);

  if (bIsMainViewWindow)
  {
    m_renderContext.SetDisplaySize(512, 512);
  }
  else
  {
    m_renderContext.SetDisplaySize(256, 256);
  }
  m_renderContext.ClearBuffer();

  float8 vfGlmm[16];
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glGetDoublev(GL_MODELVIEW_MATRIX, vfGlmm); 
  glPopMatrix();

  Matrix<float4> vm(vfGlmm[0], vfGlmm[4], vfGlmm[8],  vfGlmm[12],
    vfGlmm[1], vfGlmm[5], vfGlmm[9],  vfGlmm[13],
    vfGlmm[2], vfGlmm[6], vfGlmm[10], vfGlmm[14],
    vfGlmm[3], vfGlmm[7], vfGlmm[11], vfGlmm[15]);
  vm = vm.GetInverse();

  m_renderContext.m_fZoomFactor = fZoomFactor;
  m_renderContext.m_fFov = fFov;

  m_renderContext.m_viewpoint = vm* Point<float4>(0, 0, 0);
  m_renderContext.m_viewpoint *= m_renderContext.m_fZoomFactor;

  m_renderContext.m_vViewDirection = vm*Vector<float4>(0,0,-1);
  m_renderContext.m_vUpDirection = vm*Vector<float4>(0,1,0);
  m_renderContext.m_vDownDirection = Vector<float4>(-m_renderContext.m_vUpDirection);
  m_renderContext.m_vRight = vm*Vector<float4>(1,0,0);

  m_renderContext.m_pDataset = & rDataset;
  m_renderContext.m_pSld   = &(rDataset.m_fields.m_sldExterior);
  m_renderContext.m_fRatio = (float)uViewWth / (float)uViewHgt;

  m_renderContext.m_pVolOpa = &(rDataset.m_fields.m_opacityVol); 

  if (SystemInfo::GetCpuInfo().GetTotalThreads() > 1)
  {
    // Create the render threads if necessary
    if (m_pRenderThread0 == NULL) m_pRenderThread0 = new OverviewRenderThread(m_renderContext, rDataset.m_fields.m_volumeRender, rDataset, 0);
    if (m_pRenderThread1 == NULL) m_pRenderThread1 = new OverviewRenderThread(m_renderContext, rDataset.m_fields.m_volumeRender, rDataset, 1);

    if ((m_pRenderThread0 != NULL) && (m_pRenderThread1 != NULL) && m_pRenderThread0->IsThreadValid() && m_pRenderThread1->IsThreadValid())
    {
      m_pRenderThread0->StartRender();
      m_pRenderThread1->StartRender();
      m_pRenderThread0->WaitForRender();
      m_pRenderThread1->WaitForRender();
    }
    else
    {
      LogWrn("Missing Threads, Rendering Sequentially","OverviewRender","RayCastVolume");
      rDataset.m_fields.m_volumeRender.RaycastASExternal(-1, m_renderContext, rDataset);
    }
  }
  else
  {
    rDataset.m_fields.m_volumeRender.RaycastASExternal(-1, m_renderContext, rDataset);
  }

  m_renderContext.DisplayRenderedImage();
} // RayCastVolume()


/** 
 * render the object: colon (volume rendering).
 *
 * @param rDataset  reference to attached dataset.
 * @param bIsMainViewWindow   true if current view window is a main one.
 * @param fZoomFactor         equals to any isotropic scale factor passed to glScale, if any. Otherwise, it is 1.0
 * @param fFov                The field of view 
 * @param bTransparent        flag for isosurface and transparency mode
 */
void OverviewRender::RayCastVolume2(Dataset & rDataset, bool bHighResolution, float4 fZoomFactor, float4 fFOV, uint4 uViewWth, uint4 uViewHgt)
{ 

  // go ahead and render
  GLAttribStack glAttr( GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT | GL_TRANSFORM_BIT );
  glEnable( GL_BLEND );
  glDisable( GL_DEPTH_TEST );

  float8 vfGlmm[16];
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glGetDoublev(GL_MODELVIEW_MATRIX, vfGlmm); 
  glPopMatrix();

  Matrix<float4> vm(vfGlmm[0], vfGlmm[4], vfGlmm[8],  vfGlmm[12],
    vfGlmm[1], vfGlmm[5], vfGlmm[9],  vfGlmm[13],
    vfGlmm[2], vfGlmm[6], vfGlmm[10], vfGlmm[14],
    vfGlmm[3], vfGlmm[7], vfGlmm[11], vfGlmm[15]);
  vm = vm.GetInverse();

  m_renderContext.m_fZoomFactor = fZoomFactor;

  m_renderContext.m_viewpoint = vm* Point<float4>(0, 0, 0);
  //m_renderContext.m_viewpoint *= m_renderContext.m_fZoomFactor;

  m_renderContext.m_vViewDirection = vm*Vector<float4>(0,0,-1);
  m_renderContext.m_vUpDirection = vm*Vector<float4>(0,1,0);

  // not used right now
  vxCamera camera( m_renderContext.m_viewpoint + m_renderContext.m_vViewDirection,
		               m_renderContext.m_viewpoint, Normal3Df( m_renderContext.m_vUpDirection ) );
	camera.SetFovDegrees( fFOV );

  // initialize the rendering primitives
  vxBlockVolume<uint2> * pIntensityVolume = rDataset.m_fields.m_sliceRendererData.GetCleansedVolume();
  vxBlockVolume<uint2> & labelVolume      = rDataset.m_fields.GetLabelVolume(); // really slow the first time, immediate after that
  vxComponentArray     & componentArray   = rDataset.GetComponentArray(); // a little slow the first time, immediate after that

  // initialize the colormap
	static vxColormap * pColorMap = NULL;
  if ( pColorMap == NULL )
  {

    // extract settings from configuration file
    vxDOMDocument doc( vxDOMDocument::FromFile( VCSettings::GetInstance().GetSettingsPath() ));
    vxDOMNodeList settingsList = doc.GetDocumentElement().GetElementsByTagName( "vxColormap" );
    if( settingsList.GetLength() != 1 )
      throw ex::XmlException( LogRec( "Unable to load translucency settings.", "OverviewRender", "RayCastVolume2" ));

    vxDOMElement settingsElement = settingsList.Item( 0 ).ToElement();

    //read the preset from disk
		pColorMap =  vxColormap::FromXml( settingsElement.ToString() );

    vxUtils::AdjustColormapValuesToDataRange( *pColorMap, * pIntensityVolume );
    pColorMap->SetDirty();
  }


  // determine if we need to re-render
  RenderState currentRenderState( camera, bHighResolution, fFOV );
  if ( !( currentRenderState == m_previousRenderState ) )
  { 
    {
      // TODO: think about making these non-static
      static vxCropBox cropBox;
      static InteractiveSegmentation interactiveSegmentation;
      static vxRenderingMode renderingMode;

      Box<float4> visibleBoundingBox = vxUtils::GetVisibleBoundingBox(
        pIntensityVolume, & labelVolume, & renderingMode, & cropBox, pColorMap, & componentArray, & interactiveSegmentation );

      // set up the raytracer
      vxRaytracerSettings & settings = m_pRenderingEnginePluginRaytracer->GetRaytracer().GetSettings();
      
      settings.SetBackgroundColor( PixelRGBA< uint1 >( 255, 255, 255, 255 ) );
      settings.SetIntensityVolume( * pIntensityVolume );
      settings.SetLabelVolume( labelVolume );
      settings.SetComponents( componentArray );
      settings.SetBoxOfInterestActive( false );
      settings.GetRenderingMode().SetEnabled( vxRenderingMode::SHOW_COLORMAP_IN_MPR, true );
      // The following is needed if the proper camera rotation tracking has been implemented.
      // VC viewer uses the Tracker ball. It should have a simple way to use the Tracker ball transformation matrix
      // to properly align the camera. In XP based viewer, it uses the vxCamera class directly.
      //settings.GetRenderingMode().SetQualityScaleFactor(1.0F);
      settings.SetSampleEveryPixel(  false );
      settings.SetPreIntegrationEnabled( true );
      settings.SetShadingEnabled(  true );
      settings.SetJitteringEnabled( true );
      settings.SetShadowingEnabled( false );
      settings.SetVisibilityBox( visibleBoundingBox );
      settings.SetTransferFunction( * pIntensityVolume, * pColorMap );
      settings.UpdateMaskForIntensities( * pIntensityVolume, * pColorMap );
      settings.UpdateMaskForLabels( componentArray );
      settings.UpdateSpaceLeapVolume();
      m_bRaytracerInitialized = true;
    }

    m_previousRenderState = currentRenderState;

 		vxRaytracerSettings & settings = m_pRenderingEnginePluginRaytracer->GetRaytracer().GetSettings();

    settings.SetCamera( camera );

    int4 iImageDimX = m_pRenderingEnginePluginRaytracer->GetMaxRenderSize().m_x;
    int4 iImageDimY = m_pRenderingEnginePluginRaytracer->GetMaxRenderSize().m_y;

    // if not highest resolution adjust image size to maintain consistent framerate
    if( !bHighResolution )
    {
    //  const int4 iRenderSizeGranularity = 16;   
    //  const float4 fMinFrameRate = m_pRenderingEnginePluginRaytracer->GetMinFramerate();
    //  const float4 fMaxFrameRate = m_pRenderingEnginePluginRaytracer->GetMaxFramerate();

    //  vxRenderBuffer & buffer = m_pRenderingEnginePluginRaytracer->GetRenderBuffer();

    //  const Triple< uint4 > & originalRenderSize = buffer.GetRenderSize();

    //  //  simulate square image
    //  const int4 iCurrentRenderSize = sqrt( static_cast< float4 >( originalRenderSize.m_x * originalRenderSize.m_y ) );
  
    //  // get current fps based on the last render time  
    //  float4 fCurrentFps = ( buffer.GetRenderTime() > 0.0F ) ? 1.0F / buffer.GetRenderTime() : 0.0F;

    //  // are rendering too slow?
    //  int4 iDesiredSize( iCurrentRenderSize );
    //  if( fCurrentFps < fMinFrameRate )
    //  {
    //    iDesiredSize = sqrt( fCurrentFps / fMinFrameRate ) * iCurrentRenderSize;  
    //    iDesiredSize = max( m_pRenderingEnginePluginRaytracer->GetMinRenderSize().m_x, iDesiredSize );
    //  }  
  
    //  // are rendering too fast?
    //  else if( fCurrentFps > fMaxFrameRate ) 
    //  {
    //    iDesiredSize = sqrt( fCurrentFps / fMaxFrameRate ) * iCurrentRenderSize;  
    //    iDesiredSize = min( m_pRenderingEnginePluginRaytracer->GetMaxRenderSize().m_x, iDesiredSize );
    //  }

    //  // should we try and squeeze out some more quality?
    //  else
    //  {
    //    iDesiredSize = iCurrentRenderSize + iRenderSizeGranularity + 1;  
    //    iDesiredSize = min( m_pRenderingEnginePluginRaytracer->GetMaxRenderSize().m_x, iDesiredSize );
    //  }

    //  // convert to actual aspect ratio image
    //  const float4 fAspectRatio = static_cast<float4>(uViewWth) / uViewHgt;
    //  const int4 iDesiredSizeX = iDesiredSize *sqrt( 1.0F / fAspectRatio ) + 0.5F;
    //  const int4 iDesiredSizeY = iDesiredSizeX * fAspectRatio + 0.5F;

    //  if( iDesiredSizeX != originalRenderSize.m_x || iDesiredSizeY != originalRenderSize.m_y  )
    //  {
		  //  iImageDimX = min( ( iDesiredSizeX / iRenderSizeGranularity ) * iRenderSizeGranularity, m_pRenderingEnginePluginRaytracer->GetMaxRenderSize().m_x );
    //    iImageDimY = min( ( iDesiredSizeY / iRenderSizeGranularity ) * iRenderSizeGranularity, m_pRenderingEnginePluginRaytracer->GetMaxRenderSize().m_y );
    //  }
    //  else
    //  {
    //    iImageDimX = originalRenderSize.m_x;
    //    iImageDimY = originalRenderSize.m_y;
    //  }

      // above code block is still needed after the proper camera motion tracking implemented.
      iImageDimX = m_pRenderingEnginePluginRaytracer->GetMinRenderSize().m_x;
      iImageDimY = m_pRenderingEnginePluginRaytracer->GetMinRenderSize().m_y;
    } // if it is not high resolution display
    
    /// must use this for resolution setting rather than m_pRenderingEnginePluginRaytracer
    m_pRenderingThreadManager->SetRenderSize( Triple< uint4>( iImageDimX, iImageDimY, 0 ) );
    m_pRenderingEnginePluginRaytracer->SetRenderSize( Triple< uint4>( iImageDimX, iImageDimY, 0 ) );

		m_pRenderingEnginePluginRaytracer->GetRenderBuffer().SetBufferSize( Triple<uint4>( iImageDimX , iImageDimY, 0 ) );

		vxWinRect rect;
		rect.SetWindowSize( std::make_pair( uViewWth, uViewHgt ) );
		settings.SetWinRect( rect );

    m_pRenderingThreadManager->RenderAndWait( m_pRenderingEnginePluginRaytracer, Thread::NORMAL );

  } // if we need to re-render

  else
    m_bRaytracerInitialized = false;


  // choose the proper buffer for display
  vxRenderBuffer & displayBuffer = m_pRenderingEnginePluginRaytracer->GetRenderBuffer();

  // display the image
  int4 iOldUnpackRowLength;
  glGetIntegerv( GL_UNPACK_ROW_LENGTH, & iOldUnpackRowLength );

  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  {

    glLoadIdentity();

    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    {
      glDisable( GL_LIGHTING );
      //glDisable( GL_BLEND );

      glLoadIdentity();

      glOrtho( 0, 1, 0, 1, 0, -1 );
      float fOffsetX = 0;
      float fOffsetY = 0;

      glPixelStorei( GL_UNPACK_ROW_LENGTH, displayBuffer.GetBufferSize().m_x );
      
      glBindTexture( GL_TEXTURE_2D, 0 );
      glTexImage2D( GL_TEXTURE_2D, 0, 4, displayBuffer.GetBufferSize().m_x,
                    displayBuffer.GetBufferSize().m_y, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                    reinterpret_cast< uint1 * >( displayBuffer.GetColorImage().GetDataPtr() ) );
      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
      glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
      glEnable( GL_TEXTURE_2D );
      glDisable( GL_DEPTH_TEST );
      glDisable( GL_ALPHA_TEST );
      glBegin( GL_QUADS ); 
      {
        float4 xmaxt( displayBuffer.GetRenderSize().m_x / (float4)displayBuffer.GetColorImage().GetDim().m_x );
        float4 ymaxt( displayBuffer.GetRenderSize().m_y / (float4)displayBuffer.GetColorImage().GetDim().m_y );

        float4 xmaxv =  1.0F;
        float4 ymaxv =  1.0F;

        glTexCoord2f( 0.0,   0.0 );    glVertex2f(0,     0     );
        glTexCoord2f( 0.0,   ymaxt );  glVertex2f(0,     ymaxv );
        glTexCoord2f( xmaxt, ymaxt );  glVertex2f(xmaxv, ymaxv );
        glTexCoord2f( xmaxt, 0.0 );    glVertex2f(xmaxv, 0     );
      } 
      glEnd();
      glDisable( GL_TEXTURE_2D );
    } glPopMatrix();
    glMatrixMode( GL_MODELVIEW );
  } glPopMatrix();
  glPixelStorei( GL_UNPACK_ROW_LENGTH, iOldUnpackRowLength );

} // RayCastVolume2()



/// Whether both frame and triangle lists have been built
const bool OverviewRender::AreDisplayListsGenerated() const
{
  return (m_viTriDisplayList.size() > 0) && (m_iFrameDisplayList != 0);
}

/// set arrow parameters in report
void OverviewRender::SetArrowInReport(const Point<float4> & vPosition, const Normal<float4> & vDir)
{
  m_vReportPosition = vPosition;
  m_vReportDirection = vDir;
}; // SetArrowInReport()


/// set current overview render mode
void OverviewRender::SetRenderMode(const RenderModeEnum eMode) 
{
  m_eCurrRenderMode = eMode; 
}; // SetOVMode()


/// get current overview render Mode
const OverviewRender::RenderModeEnum & OverviewRender::GetRenderMode() const
{
  return m_eCurrRenderMode;
}; // GetOVMode()


// $Log: OverviewRender.C,v $
// Revision 1.34.2.11  2010/06/14 20:23:05  dongqing
// Minor update for support more than 16 threads in rendering
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.34.2.10  2009/11/24 20:05:04  dongqing
// reset the high resolution overview transparency display
//
// Revision 1.34.2.9  2009/11/11 17:16:37  dongqing
// reset the frame rate range
//
// Revision 1.34.2.8  2009/11/02 23:08:33  dongqing
// Issue Cad09-02: giving overview cad marks colours to indicate visibility setting.
//
// Revision 1.34.2.7  2008/02/01 15:56:53  dongqing
// fix the transparency overview toggling body position refresh problem
//
// Revision 1.34.2.6  2008/01/31 23:21:23  dongqing
// fix the series toggling bug. When toggling series, transparency overview does not
// get updated.
//
// Revision 1.34.2.5  2008/01/29 14:24:01  geconomos
// Added adaptable render size for translucent overview rendering. Need to maintain a consistent framerate.
//
// Revision 1.34.2.4  2008/01/21 22:39:53  dongqing
// change the overview transprancey background color
//
// Revision 1.34.2.3  2008/01/21 21:14:57  geconomos
// corrected initial settings for the raytracer
//
// Revision 1.34.2.2  2008/01/21 13:46:05  dongqing
// change the algorithm for creating label volume
//
// Revision 1.34.2.1  2007/04/11 13:28:26  geconomos
// corrected bad compile error
//
// Revision 1.34  2007/03/13 02:12:46  jmeade
// code standards.
//
// Revision 1.33  2007/02/20 15:25:31  geconomos
// Issue# 5415: corrected improper initialization of thread manager for transparent overview renderering
//
// Revision 1.32  2007/02/08 18:00:08  dongqing
// fix the bug that caused the translucent overview in lower resolution
//
// Revision 1.31  2007/02/05 20:02:04  geconomos
// name changes to vxRenderingEngine
//
// Revision 1.30  2007/02/02 21:40:22  romy
// exposes the m_bRayCastVolume2InitialRun flag for Renderer to decide whether to display a WaitCursor or not.
//
// Revision 1.29  2007/01/17 16:06:54  vxconfig
// fixed compilation error
//
// Revision 1.28  2006/12/16 03:48:33  jmeade
// Issue 5210: corrected code for translucent display of 2d slice positions.
//
// Revision 1.27  2006/12/05 14:47:48  romy
// Modifiy to incorporate the Rendering Engine's excess Threads elimination code change
//
// Revision 1.26  2006/09/29 18:37:39  romy
// fixed the issue of TranslucentView rendering in ATI cards
//
// Revision 1.25  2006/09/26 19:35:27  frank
// formatting
//
// Revision 1.24  2006/09/21 19:02:04  geconomos
// Issue #4968: bad aspect ratio for transparent rendering
//
// Revision 1.23  2006/09/21 16:26:35  dongqing
// temporary check in for consistence
//
// Revision 1.22  2006/09/14 18:03:40  frank
// got mutli-resolution rendering back into transparent view
//
// Revision 1.21  2006/09/14 16:19:42  frank
// Issue #4908: Fixed crash due to unsized color render buffer
//
// Revision 1.20  2006/08/28 14:04:48  geconomos
// changes to vxRenderingEngine
//
// Revision 1.19  2006/08/23 21:10:27  romy
// Colormap fix
//
// Revision 1.18  2006/08/23 19:49:31  geconomos
// updated for changes to vxRenderingEngine
//
// Revision 1.17  2006/08/23 19:32:12  romy
// added Colormap read for Colon Application
//
// Revision 1.16  2006/08/03 15:46:21  geconomos
// updated for modifications to vxRenderingEngine
//
// Revision 1.15  2006/08/01 12:44:14  frank
// Issue #4908: fixing up translucent view
//
// Revision 1.14  2006/07/25 14:52:19  geconomos
// Issue# 4823: translucent rendering crashes when enabled
//
// Revision 1.13  2006/07/09 13:19:16  geconomos
// corrected compiler errors due to changes in vxRenderingEngine
//
// Revision 1.12  2006/07/05 15:31:59  frank
// Issue #4823: Improved the translucent display quality
//
// Revision 1.11  2006/06/28 18:21:22  frank
// used the new rendering engine for multi-threading
//
// Revision 1.10  2006/06/26 13:26:54  frank
// used the new raytracer for the transparent overview
//
// Revision 1.9  2006/04/12 23:16:51  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.8  2006/02/02 20:01:46  geconomos
// updated for changes to SystemInfo
//
// Revision 1.7  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.6.2.5  2006/03/28 20:22:14  jmeade
// Issue 4642: clearer delineation of cad findings info.
//
// Revision 1.6.2.4  2006/03/24 23:37:23  jmeade
// Issue 4642: Allow hiding of extra-colonic cad findings.
//
// Revision 1.6.2.3  2006/03/22 20:54:20  frank
// moved the finding visibility and other information into a class
//
// Revision 1.6.2.2  2006/01/05 21:41:19  frank
// Added logic for controlling visibility of cad findings according to sphericity slider threshold
//
// Revision 1.6.2.1  2005/12/07 19:18:39  frank
// Issue #4548: Improved visibility of overview CAD marks
//
// Revision 1.6  2005/10/06 18:05:51  vxconfig
// changed medium priority bookmark to orange
//
// Revision 1.5  2005/09/29 18:32:21  vxconfig
// moved cad finding visibility tester to utilities
//
// Revision 1.4  2005/09/23 12:02:04  vxconfig
// further changes to show cad marks behind folds
//
// Revision 1.3  2005/09/23 03:19:23  vxconfig
// working on display of cad findings
//
// Revision 1.2  2005/09/22 16:57:22  jmeade
// render cad findings in overview.
//
// Revision 1.1  2005/09/13 12:59:56  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.3  2005/08/16 21:44:50  jmeade
// namespaces for OpenGLUtils.
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.49.2.10.2.6  2005/04/07 22:51:18  jmeade
// Issue 4104: No translucent rendering in registration verify.
//
// Revision 3.49.2.10.2.5  2005/04/06 15:45:29  frank
// Issue #3451: Used display lists to accelerate overview drawing with marking
//
// Revision 3.49.2.10.2.4  2005/04/01 19:55:51  frank
// Issue #4094: Properly handled the failure
//
// Revision 3.49.2.10.2.3  2005/04/01 17:33:33  frank
// Issue #4094: Overview will now try to create the transparent volume, if that fails, it will simply use triangles from that point forward.
//
// Revision 3.49.2.10.2.2  2005/03/29 15:20:05  jmeade
// Issue 4032: No slice indicators in registration verify.
//
// Revision 3.49.2.10.2.1  2001/01/02 11:21:58  frank
// Issue #3984: Fixing update of oblique slice shadow when stopping flight
//
// Revision 3.49.2.10  2005/02/15 18:04:25  jmeade
// merge from vc1.4 branch.
//
// Revision 3.49.2.9.2.1  2005/02/04 23:53:08  jmeade
// Allowing for variances in 2D and 3D volume coordinate units.
//
// Revision 3.49.2.9  2003/05/22 19:59:43  kevin
// Tracker issue 2784: This is now fixed correctly. The  system will now use
// more memory and the external DFB volume is sotred in it's own volume
// so the internal and enternal will not "leak" together.
//
// This also indirectly fixes the black holes problem
//
// Revision 3.49.2.8  2003/05/01 16:59:02  kevin
// Code Walkthrough cosmetic changes
//
// Revision 3.49.2.7  2003/04/14 16:45:22  jmeade
// Removed unneeded BookmarkList override.
//
// Revision 3.49.2.6  2003/04/04 20:13:26  kevin
// Issue 2835: The threads were crashing on creatiion. 2 things were
// done to fix this:
// (1) Handle rendierng if the threads are non-existant by switching
// to sequential rendering (this should have been in there nayway)
// (2) Create the threads early when there is plenty of memory blocks
//
// Revision 3.49.2.5  2003/03/20 20:36:42  kevin
// Make renerCOntext for TTP rednering a member since it is passed to the threads one
// time instead of every frame with the new RenderThread structure,
//
// Revision 3.49.2.4  2003/03/10 22:55:40  jmeade
// Overview marking displayed on registered views as well.
//
// Revision 3.49.2.3  2003/03/07 04:54:01  jmeade
// Issue 2871:  Use unsorted segment index to color each segment during "index rendering".
//
// Revision 3.49.2.2  2003/02/21 17:50:57  jmeade
// "Index rendering" should be allowed on both selected and all segments.
//
// Revision 3.49.2.1  2003/02/12 23:22:47  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 3.50  2003/02/10 18:47:10  kevin
// Registration Debug Info
//
// Revision 3.49  2003/02/05 18:32:20  kevin
// Make Registration Debug Global
//
// Revision 3.48  2003/01/29 18:53:38  jmeade
// Removed code duplication in RenderThread and derivatives.
//
// Revision 3.47  2003/01/22 22:26:15  ingmar
// synchronized to new Point3D Vector3D and Normal3D classes
//
// Revision 3.46  2003/01/07 15:05:25  kevin
// Render threads switched to worker-threads rather than windows-threads
//
// Revision 3.45  2002/12/23 22:28:08  kevin
// Adjust the rendersize based on the frame rate
//
// Revision 3.44  2002/11/24 17:53:01  kevin
// Upadtes now consider the flight direction
//
// Revision 3.43  2002/11/22 03:10:08  jmeade
// RenderConnections() must choose endpoints based on segment direction.
//
// Revision 3.42  2002/11/22 02:44:15  jmeade
// Centerline reflecting updated modes.
//
// Revision 3.41  2002/11/21 22:52:40  jmeade
// Overview render mode cleanup.
//
// Revision 3.40  2002/11/11 14:31:17  kevin
// Franks idea of showing a moving registration point in the verification pane
//
// Revision 3.39  2002/10/31 20:03:39  kevin
// Redfactor Selecting of segments, jump-to-points etc since this got brooken
//
// Revision 3.38  2002/10/21 18:51:52  kevin
// Kevin's refactored S/P Registration with new heirarchical algorithm
//
// Revision 3.37  2002/09/18 20:03:29  kevin
// An Overview can only ever be supine OR prone, no need
// for variables for each in each instance
//
// Revision 3.36  2002/09/13 00:46:09  jmeade
// Bookmarks in ColonSession class.
//
// Revision 3.35  2002/09/11 19:15:50  kevin
// Updates to when and how registration is run during the users traversal
// of the verification screen
//
// Revision 3.34  2002/09/10 20:49:03  jmeade
// Oops:  Each OverviewRender instance needs its own render mode variable.
//
// Revision 3.33  2002/09/10 19:05:51  jmeade
// One render mode for all OverviewRender instances; Code standards.
//
// Revision 3.32  2002/08/22 15:17:07  kevin
// Remove debug development code from Segment skel render
//
// Revision 3.31  2002/08/20 19:23:41  kevin
// 2D and 3D segment views.
//
// Revision 3.30  2002/08/15 15:37:30  kevin
// Added Segmental Visualization prototype/test code.
//
// Revision 3.29  2002/08/13 14:21:15  ana
// Registration worst quality color changed to brown.
//
// Revision 3.28  2002/07/25 15:31:53  ana
// Data member assignment was duplicated.
//
// Revision 3.27  2002/07/17 22:37:08  suzi
// Coloring the centerline based on feature-match confidence rather than windowed correlation.
//
// Revision 3.26  2002/07/15 21:01:23  suzi
// Registration refinements.
//
// Revision 3.25  2002/07/11 22:13:46  jmeade
// Use of the gl attribute stack (e.g. via glPushAttrib).
//
// Revision 3.24  2002/07/09 20:12:53  jmeade
// Code standards.
//
// Revision 3.23  2002/06/12 20:10:16  suzi
// Huge refactoring and implementation of new registration with multiple seeds.
//
// Revision 3.22  2002/06/07 14:17:38  suzi
// Improved registration handling.
//
// Revision 3.21  2002/05/01 15:31:12  ana
// Numbered features in the not matched view.
//
// Revision 3.20  2002/04/17 19:12:37  ana
// Feature bookmarks numbered.
//
// Revision 3.19  2002/04/12 14:25:16  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.18  2002/04/11 18:50:39  ana
// Rendering matched features bookmarks with the same color, so we know
// the exactly matches visually.
//
// Revision 3.17  2002/04/11 16:52:08  ana
// Merge with Boston code, which uses the blended correlation to color the
// centerline with the registration quality.
//
// Revision 3.16  2002/03/28 21:40:01  ana
// Method added, to color centerline with registration quality colors.
//
// Revision 3.15  2002/03/20 19:50:37  ana
// Finally displaying the in, out and rotation vectors for a given feature.
//
// Revision 3.14  2002/03/15 03:26:33  jmeade
// Code standards in BookmarkList classes.
//
// Revision 3.13  2002/03/13 21:56:40  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.12  2002/03/12 15:58:04  ana
// Displaying in and out vectors for the selected feature.
//
// Revision 3.11  2002/03/05 15:35:39  ana
// Using feature vectors.
//
// Revision 3.10  2002/02/06 21:18:36  ana
// Added some data members to handle the registration menu options
// added to overview image.
//
// Revision 3.9  2002/02/04 18:57:06  ana
// Added an item to overview menu, which enables whether bookmarks of
// all registration features or the matched features.
//
// Revision 3.8.2.1  2002/02/06 23:52:51  kevin
// Added color schemes to handle paintnig information for color blind people (NOTE: we should
// expand this to all our color information in the next release. e.g. centerline color, bookmark color etc)
//
// Revision 3.8  2002/01/30 15:46:52  ana
// Added RenderRegistrationAllFeatures which render bookmarks in blue
// of all features founded during the first step of the registration.
//
// Revision 3.7  2002/01/04 16:33:19  kevin
// Some cleanup after the hacking to get TTP rendering without ses,set,sec
//
// Revision 3.6  2002/01/03 02:29:55  kevin
// Fixed TTP rendering now that set, ses and sec are gone
//
// Revision 3.5  2001/11/14 01:34:16  kevin
// Cleaned up alot of Supine/Proneregistration stuff, fixed some
// of the multi-rendering and added feature match pair rendering
//
// Revision 3.4  2001/11/06 15:27:11  kevin
// Removed ses, set, and sec volumes.
// NOTE: NEED TO FIX TRANSLUCENT OVERVIEW RENDERING
// (BUT WANTED TO GET THIS CHECKED IN FOR OTHERS TO USE)
//
// Revision 3.3  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.2  2001/10/19 18:08:17  jmeade
// Coding standards:  Individual bookmark rendering should be encapsulated
// in one function (to allow for possible future code changes)
//
// Revision 3.1  2001/10/17 21:34:59  jmeade
// Coding standards.
//
// Revision 3.0  2001/10/14 23:01:53  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.7   Oct 12 2001 11:27:14   jmeade
// Removed crash-prone debug-only code
// 
//    Rev 2.6   Oct 11 2001 20:46:08   jmeade
// Merge 2.4.1.0
// 
//    Rev 2.5   Oct 10 2001 11:54:48   binli
// issue 1027: transparent rendering of overview reflect selection
// 
//    Rev 2.4   Oct 08 2001 21:37:44   jmeade
// Specify study to be rendered (removed legacy assumption of rdrGlobal.m_pStudy)
// Resolution for 1078: Registration support
// 
//    Rev 2.3   Oct 01 2001 16:51:44   binli
// function to ensure no black images: arrow size (consistence)
// 
//    Rev 2.2   Sep 28 2001 16:06:00   binli
// wider connections in report
// 
//    Rev 2.1   Sep 18 2001 14:37:38   binli
// track ID: 000523: transparent rendering of overview: proper w/h ratio. No freeze (ensured non-null thread pointer because of the fix job: ID 000589
// 
//    Rev 2.0   Sep 16 2001 23:40:14   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:02   ingmar
// Initial revision.
// Revision 1.49  2001/09/14 21:30:13  jmeade
// Bug fix for Track Record ID 589:Out-of-Memory when creating bkgd render
// threads:Ensure threads are created before rendering, render in primary thread otherwise
//
// Revision 1.48  2001/08/21 20:23:46  binli
// bug 612: fixed
//
// Revision 1.47  2001/08/20 20:08:08  tushar
// removed some errors in vc2000src due to logger class
//
// Revision 1.46  2001/07/30 16:30:36  liwei
// TransferFunction renamed to TransferFunctionVC.
//
// Revision 1.45  2001/07/25 22:52:16  binli
// ID 89: new exception handling.
//
// Revision 1.44  2001/07/24 21:50:19  binli
// ID 89: exception handling in v1k GUI
//
// Revision 1.43  2001/06/26 16:21:21  ana
// Results of code review for Fields and Jitter classes
//
// Revision 1.42  2001/06/14 19:50:39  binli
// 1> show each segment(skeleton) with gradient (color: red->green)
// 2> segement number in right position.
//
// Revision 1.41  2001/06/11 13:40:05  frank
// Removed unused skeleton rendering parameter.
//
// Revision 1.40  2001/06/04 20:12:33  binli
// added new mode: Endo-Orthogonal (fix bug 000484)
//
// Revision 1.39  2001/06/04 18:27:24  frank
// Reflected changes in SkeletonRender class.
//
// Revision 1.38  2001/05/31 22:21:15  binli
// some works for code conventions and bug fixing (000489, 484)
//
// Revision 1.37  2001/05/23 18:28:10  binli
// code conventions
//
// Revision 1.36  2001/05/22 14:53:20  binli
// dual-slice control: lock/unlock.
//
// Revision 1.35  2001/05/18 14:59:10  binli
// Moved CharBitmap to OverviewRender (has nothing to do with Windows)
//
// Revision 1.34  2001/05/17 20:42:19  binli
// VerifyView vs. StandardVIew/S&PView: controled under different trackball.
//
// Revision 1.33  2001/05/15 18:40:22  jmeade
// Code conventions
//
// Revision 1.32  2001/05/15 17:49:37  kevin
// Finished coding compliance for ReaderGlobal for wlakthrough
//
// Revision 1.31  2001/05/11 17:37:22  binli
// removed duplicated OVModes in OverView class.
//
// Revision 1.30  2001/05/09 18:34:47  liwei
// variable ReaderGlobal::m_bInteractingOverview deleted
// Unnecessary raycasting in translucent mode avoided.
//
// Revision 1.29  2001/05/09 15:37:47  liwei
// OverviewRender now uses RenderThread for multiprocessors
//
// Revision 1.28  2001/05/08 18:35:42  binli
// Trackrecord ID 000377: green marked or not in S&PView and VerifyView.
// (No need to change snapshot in Report)
//
// Revision 1.27  2001/05/08 14:42:55  binli
// distingushed the color of framebox from that of coronal plane.
//
// Revision 1.26  2001/05/07 22:36:30  soeren
// rewrote the whole SystemInfo class
//
// Revision 1.25  2001/05/07 00:51:26  kevin
// Added a paintVol and cleaned up ReaderGLobal some more
// (Got more to go, wanted to finish this weekend, but had
// problems chciking in CVS like Jeff Meade did last week)
//
// Revision 1.24  2001/05/04 15:32:45  binli
// display lists: triangle/framebox.
//
// Revision 1.23  2001/05/03 15:52:00  binli
// used display list to render framebox.
//
// Revision 1.22  2001/05/02 21:39:43  jmeade
// corrections after mergeLostApril26 merge
//
// Revision 1.21  2001/05/02 19:48:46  binli
// bug fixed: correct render planes when picked up a bookmark in S&PView/VerifyView.
//
// Revision 1.20  2001/04/20 20:11:10  binli
// fixed a bug: bookmark picking in S&P view or Verify view.
//
// Revision 1.19  2001/04/20 14:58:51  binli
// default settings -- no push/pop
//
// Revision 1.18  2001/04/19 23:14:44  jmeade
// push/pop all attribute bits in each function, fixing a triangle render bug
// temporary change, should only set necessary state bits)
//
// Revision 1.17  2001/04/19 20:25:07  jmeade
// Example use of glPushAttrib(), please apply throughout code
//
// Revision 1.16  2001/04/19 19:26:22  jmeade
// CglFont class renamed to GLFont
//
// Revision 1.15  2001/04/19 19:11:57  binli
// adjust the default GL status in overview class
//
// Revision 1.14  2001/04/18 19:11:37  liwei
// m_pOpacityVol changed to m_opacityVol
//
// Revision 1.13  2001/04/16 18:42:16  binli
// RenderCamera: use saved ModelView & ProjMatrix.
//
// Revision 1.12  2001/04/13 15:33:17  binli
// moved render function: RayCastVolume() from Overview Class to this.
//
// Revision 1.11  2001/04/12 22:26:39  binli
// moved render function: RenderSegNum() from Overview Class to this.
//
// Revision 1.10  2001/04/12 21:13:00  binli
// moved render function: RenderCamera() from Overview Class to this.
//
// Revision 1.9  2001/04/12 14:50:51  binli
// moved render function: RenderBookMarks() from Overview Class to this.
//
// Revision 1.8  2001/04/11 21:52:03  binli
// moved render function: RenderReportConnections from Overview Class to this.
//
// Revision 1.7  2001/04/11 21:05:23  binli
// moved render function: RenderConnections from Overview Class to this.
//
// Revision 1.6  2001/04/11 20:50:26  kevin
// Coding Standard updates of variable names, types and
// comments in the VolumeRender class.
//
// Revision 1.5  2001/04/11 20:46:34  binli
// moved render function: RenderSkel from Overview Class to this.
//
// Revision 1.4  2001/04/11 16:10:50  binli
// moved function: RenderSlices from Overview class to this.
//
// Revision 1.3  2001/04/11 13:27:31  frank
// Moved volume data to Fields class.
//
// Revision 1.2  2001/04/10 22:17:17  binli
// moved render functions "RenderTri" to the OverviewRender class.
//
// Revision 1.1  2001/04/09 16:07:03  binli
// moved render functions from Overview to this class.
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/OverviewRender.C,v 1.34.2.11 2010/06/14 20:23:05 dongqing Exp $
// $Id: OverviewRender.C,v 1.34.2.11 2010/06/14 20:23:05 dongqing Exp $
