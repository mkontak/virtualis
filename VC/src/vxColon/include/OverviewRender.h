// $Id: OverviewRender.h,v 1.16.2.1 2008/01/29 14:24:01 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeff Meade  jmeade@viatronix.net

/**
 * Description: This class is reponsible for rendering following objects in overview:
 *    colon, skeleton(centerline) and connections, planes(axial/coronal/sagittal/corss-section), 
 *    bookmarks, framebox, camera.
 */

// pragmas
#pragma once


// includes
#include "Dataset.h"
#include "Triple.h"
#include "VtxViewModes.h"
#include "Typedef.h"
#include "RenderThread.h"
#include "GLFont.h"
#include "GlDisplayList.h"
#include "vxCadFinding.h"
#include "vxCamera.h"

class vxRenderingNodePluginRaytracer;
class vxRenderingNodeThreadManager;


namespace ReaderLib
{
  /**
   * Thread for rendering images
   */
  class OverviewRenderThread : public RenderThread
  {
  public:

    /// Constructor
    OverviewRenderThread(RenderContext & rRenderContext, VolumeRender & rVolRender, Dataset & rDataset, const uint4 uId);

  protected:

    /// Render the object
    virtual void Render();

  public:

    /// Overview render context
    RenderContext & m_rRenderContext;

  }; // class OverviewRenderThread

  /**
  * Overview (external colon view) image rendering
  */
  class OverviewRender
  {
  public:

    /// view mode
    enum RenderModeEnum { eNormal, eSegmentVerify, eSegmentPicking, eRegistrationVerify, eRegisteredViews };

  public:

    /// constructor
    OverviewRender();

    /// destructor
    virtual ~OverviewRender();

  public:

    /// render the object: framebox
    void RenderFrame(Dataset & rDataset);

    /// render the object: colon
    void RenderTris(Dataset & rDataset, const VtxViewModes & viewModes, const bool bIndexRender,
      const bool bIndexRenderSorted, const Triple<uint1> vPickSegtColor[], const bool bRefDisplayList);

    /// render the object: A/C/S/CS planes
    void RenderSlices(Dataset & rDataset, const VtxViewModes & viewModes);

    /// render the object: skeleton
    void RenderSkel(Dataset & rDataset, const VtxViewModes & viewModes, const float4 fLineWidth,
                    const Rect<int4> & rViewport);

    /// render the object: skeleton
    void RenderSimplifiedSegments(Dataset & rDataset, const VtxViewModes & viewModes, const float4 fLineWidth,
                                  const Rect<int4> & rViewport, const float4 fRadius);

    /// render the object: connections between segments
    void RenderConnections(Dataset & rDataset, const VtxViewModes & viewModes);

    /// render the object: connections between segments (in report)
    void RenderReportConnection(const Point<float4> & vReportStart, const Point<float4> & vReportEnd);

    /// render a single bookmark
    void RenderSingleBookmark(const float4 fRadius);

    /// render a single Cross-sectional square
    void RenderSingleCrossSection(const Point<float4> pos, const Normal<float4> up, const Normal<float4> tan, const float4 fRadius);

    /// render the object: BookMarks
    void RenderBookMarks(BookmarkList & rMarkList, const float4 fRadius);

    /// render the object: CAD findings
    void RenderCadFindings( vxCAD::vxCadFindings & rCadFindings,
                            std::map<std::string, ColonSession::FindingInfo::DecisionEnum> & cadFindingVisMap,
                            const int4 iCadSliderValue,
                            const float4 fRadius );

    /// render the matching registration features
    void RenderRegistrationMatchedFeatures(Dataset & rDataset, const float4 fRadius, 
      const Triple<uint1> vPickSegtColors[], GLFont * pGLFont);
  
    /// render all the features found during registration
    void RenderRegistrationAllFeatures(Dataset & rDataset, const float4 fRadius, GLFont * pGLFont);
   
    /// renders the centerline with color indicating the quality of the registration
    void RenderRegistrationColoredCenterline(Dataset & rDataset, const float4 fLineWidth);

    /// renders the feature number
    void RenderFeatureNumber(Point<float4> vPosition, int1 iNumber, GLFont * pGLFont);
      
    // Computes the color for a given point in the centerline
    float4 ColorFunction(float4 fCorrelation);

    /// render the object: camera position (arrow)
    void RenderCamera(Dataset & rDataset, const Rect<int4> & rect, const float8 vProjMatrix[], 
                      const float8 vModelViewMatrix[], const bool bRenderReportArrow, const int4 iArrowSize = 1);

    /// render the object: the segment number when it is is picked
    void RenderSegNum(Dataset & rDataset, GLFont * pGLFont);

    /// render the object: colon (volume rendering)
    void RayCastVolume(Dataset & rDataset, const bool bIsMainViewWindow, const float4 fZoomFactor,
      const float4 fFov, const uint4 uWidth, const uint4 uHight);

    /// render the object: colon (volume rendering using new volume renderer)
    void RayCastVolume2(Dataset & rDataset, const bool bIsMainViewWindow, const float4 fZoomFactor,
      const float4 fFOV, const uint4 uWidth, const uint4 uHight);

    /// build all display lists: triangle/framebox/...
    bool BuildAllDisplayLists(Dataset & rDataset);

    /// build display lists: triangle
    bool BuildTriDisplayList(Dataset & rDataset);

    /// Whether both frame and triangle lists have been built
    const bool AreDisplayListsGenerated() const;

    /// build display lists: triangle
    bool BuildFrameDisplayList(Dataset & rDataset);

    /// render the object: CAD findings
    void RenderCadFindings( vxCAD::vxCadFindings & rCadFindings,
                            std::map<std::string, ColonSession::FindingInfo> & cadFindingInformation,
                            const int4 iCadSliderValue,
                            const float4 fRadius );

    /// delete all display lists: triangle/framebox/...
    void DeleteAllDisplayLists();

    /// delete display lists
    void DeleteTriDisplayList();
    
    /// delete frame box display list
    void DeleteFrameDisplayList();

    /// set arrow parameters in report
    void SetArrowInReport(const Point<float4> & vPosition, const Normal<float4> & vDir);

    /// set current overview render mode
    void SetRenderMode(const RenderModeEnum eMode);

    /// get current rendering mode
    const RenderModeEnum & GetRenderMode() const;
    
  private:

    /// draw framebox
    void DrawFrame(const float4 fx, const float4 fy, const float4 fz);

  /// data members
  #ifndef FINAL_REALEASE
  public:

    bool m_bRegistrationColoredCenterline;

    uint4 m_uRegistrationVerificationPreviewIndex;
  #endif

  private:

    /// indicates if the raytracer has been initialized
    bool m_bRaytracerInitialized;

    /// overview rendering mode
    RenderModeEnum m_eCurrRenderMode;

    /// A GLU object for drawing spheres
    GLUquadricObj * m_pQuadObject;

    /// flag for whether the report arrow will be rendered
    bool m_bRenderReportArrow;

    /// arrow position in report
    Point<float4> m_vReportPosition;

    /// arrow direction in report
    Normal<float4> m_vReportDirection;

    /// display list of framebox
    int4 m_iFrameDisplayList;

    /// display list of segments; the size of the list denotes the number of segments in the dataset
    std::vector<int4> m_viTriDisplayList;

    /// the overview with marking as a display list
    std::vector< OpenGLUtils::GlDisplayList > m_vMarkingDisplayList;

    /// the current seen valid for the stored marking display list
    std::vector< int4 > m_viCachedSeenValue;

    /// For multi-processing the rendering
    OverviewRenderThread *m_pRenderThread0;
    
    /// For multi-processing the rendering
    OverviewRenderThread *m_pRenderThread1;

    /// For volume rendering the transparent overview
    RenderContext m_renderContext;

    /// the raytracer
    vxRenderingNodeThreadManager * m_pRenderingThreadManager;

		///Plugin Ray Tracer Object
		vxRenderingNodePluginRaytracer * m_pRenderingEnginePluginRaytracer;

    /// the previous render state for the translucent overview image
    class RenderState
    {
    // member functions
    public:

      /// constructor
      RenderState( const vxCamera & camera, const bool & bHighResolution, const float4 fFOV ) :
          m_camera( camera ), m_bHighResolution( bHighResolution ), m_fFOV( fFOV ) {}

      /// copy constructor
      RenderState( const RenderState & other ) : m_camera( other.m_camera ), m_bHighResolution( other.m_bHighResolution ), m_fFOV( other.m_fFOV ) {}

      /// copy assignment operator
      RenderState & operator =( const RenderState & other )
      {
        m_camera = other.m_camera;
        m_bHighResolution = other.m_bHighResolution;
        m_fFOV = other.m_fFOV;
        return * this;
      }

      /// equality operator
      bool operator ==( const RenderState & other ) const
      {
        return m_camera == other.m_camera &&
               m_bHighResolution == other.m_bHighResolution &&
               m_fFOV == other.m_fFOV;
      }

    // data members
    private:

      /// the current camera
      vxCamera m_camera;

      /// if high resolution
      bool m_bHighResolution;

      /// the field of view
      float4 m_fFOV;

    }; // class RenderState

    /// the previous render state (to determine if we need to re-render)
    RenderState m_previousRenderState;

  }; // class OverviewRender

} // namespace ReaderLib


// $Log: OverviewRender.h,v $
// Revision 1.16.2.1  2008/01/29 14:24:01  geconomos
// Added adaptable render size for translucent overview rendering. Need to maintain a consistent framerate.
//
// Revision 1.16  2007/03/13 02:12:46  jmeade
// code standards.
//
// Revision 1.15  2007/02/20 15:25:31  geconomos
// Issue# 5415: corrected improper initialization of thread manager for transparent overview renderering
//
// Revision 1.14  2007/02/06 18:32:03  geconomos
// corrected forward declaration of vxRenderingNodeThreadManager
//
// Revision 1.13  2007/02/05 20:02:04  geconomos
// name changes to vxRenderingEngine
//
// Revision 1.12  2007/02/02 21:40:22  romy
// exposes the m_bRayCastVolume2InitialRun flag for Renderer to decide whether to display a WaitCursor or not.
//
// Revision 1.11  2006/12/05 14:47:48  romy
// Modifiy to incorporate the Rendering Engine's excess Threads elimination code change
//
// Revision 1.10  2006/08/03 15:46:21  geconomos
// updated for modifications to vxRenderingEngine
//
// Revision 1.9  2006/06/28 18:21:22  frank
// used the new rendering engine for multi-threading
//
// Revision 1.8  2006/06/26 13:26:54  frank
// used the new raytracer for the transparent overview
//
// Revision 1.7  2006/04/12 23:14:45  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.6  2006/02/07 21:17:10  jmeade
// code review: comments.
//
// Revision 1.5  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.4.2.2  2006/03/22 20:54:03  frank
// moved the finding visibility and other information into a class
//
// Revision 1.4.2.1  2006/01/05 21:41:19  frank
// Added logic for controlling visibility of cad findings according to sphericity slider threshold
//
// Revision 1.4  2005/09/29 18:32:21  vxconfig
// moved cad finding visibility tester to utilities
//
// Revision 1.3  2005/09/28 20:25:13  vxconfig
// renamed study files to dataset
//
// Revision 1.2  2005/09/22 16:57:22  jmeade
// render cad findings in overview.
//
// Revision 1.1  2005/09/13 13:00:10  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.3  2005/08/16 21:44:50  jmeade
// namespaces for OpenGLUtils.
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.27.2.3.4.2  2005/06/08 19:45:26  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.27.2.3.4.1.2.1  2005/05/25 15:50:21  geconomos
// initial backup
//
// Revision 3.27.2.3.4.1  2005/04/06 15:45:29  frank
// Issue #3451: Used display lists to accelerate overview drawing with marking
//
// Revision 3.27.2.3  2003/03/20 20:36:42  kevin
// Make renerCOntext for TTP rednering a member since it is passed to the threads one
// time instead of every frame with the new RenderThread structure,
//
// Revision 3.27.2.2  2003/02/21 17:50:57  jmeade
// "Index rendering" should be allowed on both selected and all segments.
//
// Revision 3.27.2.1  2003/02/12 23:22:47  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 3.28  2003/02/10 18:47:10  kevin
// Registration Debug Info
//
// Revision 3.27  2003/02/05 18:32:20  kevin
// Make Registration Debug Global
//
// Revision 3.26  2003/01/29 18:53:38  jmeade
// Removed code duplication in RenderThread and derivatives.
//
// Revision 3.25  2003/01/07 15:05:25  kevin
// Render threads switched to worker-threads rather than windows-threads
//
// Revision 3.24  2002/11/21 22:52:40  jmeade
// Overview render mode cleanup.
//
// Revision 3.23  2002/11/11 14:31:17  kevin
// Franks idea of showing a moving registration point in the verification pane
//
// Revision 3.22  2002/10/21 18:51:52  kevin
// Kevin's refactored S/P Registration with new heirarchical algorithm
//
// Revision 3.21  2002/09/18 20:03:29  kevin
// An Overview can only ever be supine OR prone, no need
// for variables for each in each instance
//
// Revision 3.20  2002/09/10 20:49:03  jmeade
// Oops:  Each OverviewRender instance needs its own render mode variable.
//
// Revision 3.19  2002/09/10 19:05:51  jmeade
// One render mode for all OverviewRender instances; Code standards.
//
// Revision 3.18  2002/08/20 19:23:41  kevin
// 2D and 3D segment views.
//
// Revision 3.17  2002/08/15 15:37:30  kevin
// Added Segmental Visualization prototype/test code.
//
// Revision 3.16  2002/07/15 21:01:23  suzi
// Registration refinements.
//
// Revision 3.15  2002/07/09 20:12:53  jmeade
// Code standards.
//
// Revision 3.14  2002/05/01 15:31:12  ana
// Numbered features in the not matched view.
//
// Revision 3.13  2002/04/17 19:12:37  ana
// Feature bookmarks numbered.
//
// Revision 3.12  2002/04/12 14:25:17  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.11  2002/04/11 18:50:39  ana
// Rendering matched features bookmarks with the same color, so we know
// the exactly matches visually.
//
// Revision 3.10  2002/04/11 16:52:08  ana
// Merge with Boston code, which uses the blended correlation to color the
// centerline with the registration quality.
//
// Revision 3.9  2002/03/28 21:40:01  ana
// Method added, to color centerline with registration quality colors.
//
// Revision 3.8  2002/03/05 15:35:38  ana
// Using feature vectors.
//
// Revision 3.7  2002/02/06 21:18:36  ana
// Added some data members to handle the registration menu options
// added to overview image.
//
// Revision 3.6  2002/02/04 18:57:06  ana
// Added an item to overview menu, which enables whether bookmarks of
// all registration features or the matched features.
//
// Revision 3.5  2002/01/30 15:46:52  ana
// Added RenderRegistrationAllFeatures which render bookmarks in blue
// of all features founded during the first step of the registration.
//
// Revision 3.4  2002/01/03 02:29:55  kevin
// Fixed TTP rendering now that set, ses and sec are gone
//
// Revision 3.3  2001/11/14 01:34:16  kevin
// Cleaned up alot of Supine/Proneregistration stuff, fixed some
// of the multi-rendering and added feature match pair rendering
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
//    Rev 2.2   Oct 01 2001 16:51:14   binli
// function to ensure no black image in report file: arrow size (consistence)
// 
//    Rev 2.1   Sep 18 2001 14:34:56   binli
// track ID: 000523: translucent overview rendering -- proper w/h ratio (Endo-Slice view)
// 
//    Rev 2.0   Sep 16 2001 23:40:14   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:27:04   ingmar
// Initial revision.
// Revision 1.24  2001/06/04 20:12:22  binli
// added new mode: Endo-Orthogonal (fix bug 000484)
//
// Revision 1.23  2001/05/31 22:21:15  binli
// some works for code conventions and bug fixing (000489, 484)
//
// Revision 1.22  2001/05/23 18:28:10  binli
// code conventions
//
// Revision 1.21  2001/05/18 14:59:10  binli
// Moved CharBitmap to OverviewRender (has nothing to do with Windows)
//
// Revision 1.20  2001/05/15 18:40:22  jmeade
// Code conventions
//
// Revision 1.19  2001/05/11 17:37:22  binli
// removed duplicated OVModes in OverView class.
//
// Revision 1.18  2001/05/09 15:37:47  liwei
// OverviewRender now uses RenderThread for multiprocessors
//
// Revision 1.17  2001/05/08 18:35:43  binli
// Trackrecord ID 000377: green marked or not in S&PView and VerifyView.
// (No need to change snapshot in Report)
//
// Revision 1.16  2001/05/04 15:32:45  binli
// display lists: triangle/framebox.
//
// Revision 1.15  2001/05/03 15:52:00  binli
// used display list to render framebox.
//
// Revision 1.14  2001/05/03 15:22:47  binli
// m_currOVMode is private now.
//
// Revision 1.13  2001/04/19 19:26:22  jmeade
// CglFont class renamed to GLFont
//
// Revision 1.12  2001/04/19 19:11:57  binli
// adjust the default GL status in overview class
//
// Revision 1.11  2001/04/16 18:42:17  binli
// RenderCamera: use saved ModelView & ProjMatrix.
//
// Revision 1.10  2001/04/13 15:33:17  binli
// moved render function: RayCastVolume() from Overview Class to this.
//
// Revision 1.9  2001/04/12 22:26:39  binli
// moved render function: RenderSegNum() from Overview Class to this.
//
// Revision 1.8  2001/04/12 21:13:01  binli
// moved render function: RenderCamera() from Overview Class to this.
//
// Revision 1.7  2001/04/12 14:50:52  binli
// moved render function: RenderBookMarks() from Overview Class to this.
//
// Revision 1.6  2001/04/11 21:52:03  binli
// moved render function: RenderReportConnections from Overview Class to this.
//
// Revision 1.5  2001/04/11 21:04:29  binli
// moved render function: RenderConnections from Overview Class to this.
//
// Revision 1.4  2001/04/11 20:46:34  binli
// moved render function: RenderSkel from Overview Class to this.
//
// Revision 1.3  2001/04/11 16:10:50  binli
// moved function: RenderSlices from Overview class to this.
//
// Revision 1.2  2001/04/10 22:17:17  binli
// moved render functions "RenderTri" to the OverviewRender class.
//
// Revision 1.1  2001/04/09 16:07:03  binli
// moved render functions from Overview to this class.
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/OverviewRender.h,v 1.16.2.1 2008/01/29 14:24:01 geconomos Exp $
// $Id: OverviewRender.h,v 1.16.2.1 2008/01/29 14:24:01 geconomos Exp $
