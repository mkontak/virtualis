// $Id: vxEnvironment.h,v 1.44.2.1 2009/07/28 15:02:53 kchalupa Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Soeren Grimm (mailto:soeren@viatronix.com), Dmitry Gritsayenko (mailto:dmitry@viatronix.com)

#pragma once

// includes
#include "vxRenderingMode.h"
#include "vxShareableObject.h"
#include "vxBlockVolume.h"
#include "vxElement.h"
#include "vxInputFeedback.h"
#include "vxMouseCursor.h"
#include "vxColormap.h"
#include "vxColorTableCPU.h"
#include "vxComponentArray.h"
#include "vxCamera.h"
#include "vxEventTimer.h"
#include "vxIDs.h"
#include "GLEnv.h"
#include "InteractiveSegmentation.h"
#include "vxPlaneOrtho.h"
#include "vxPlaneOblique.h"
#include "vxPlaneCurved.h"
#include "vxDataRendererSoftware.h"
#include "vxCropBox.h"
#include "vxContourList.h"
#include "vxWinRect.h"
#include "vxAnnotations.h"
#include "vxViewerTypeUndefined.h"
#include "vxCursor3d.h"
#include "vxFontInfo.h"
#include "vxOverlayStrings.h"
#include "vxZoomFactor.h"
#include "BaseHistogram.h"
#include "vxContour.h"
#include "vxPreset.h"
#include "vxMomentum.h"
#include "vxUndoActionStack.h"
#include "vxRenderBuffer.h"
#include "Timer.h"
#include "vxCadFindings.h"


// forward declarations
class GLFontGeorge;
class GLContext;
class vxServerTexMap2d;
class vxServerTexMap3d;
class BaseHistogram;
class vxViewerType;
class vxIRenderingContext;
class vxCine;
class vxCameraFlySettings;
class vxVisualizationGraph;
class vxDataset;


/**
 *  This class encapsulates all environment parameters and settings
 */
class VX_VIEWER_DLL vxEnvironment
{
// enums and constants
public:

  /**
   * Environment enumerations
   */
  enum vxEnvironmentEnum
  {
    NONE = 0,
    VOLUME_INTENSITY,
    VOLUME_INTENSITY_HISTOGRAM,
    VOLUME_LABEL,
    COLORMAP,
    COMPONENT_VISUAL,
    COMPONENT_SELECTION,
    COMPONENT_NAME,
    INTERACTIVE_SEGMENTATION,
    HANDLESET, 
    CAMERA,
    PLANE_MPR,
    SLICE_SHADOWS,
    TOOLBAR,
    RENDERING_MODE,
    SCULPT_CONTOUR,
    ACTIVE_ENVIRONMENT,
    CURSOR3D,
    CURRENT_VOXEL,
    DATA_RENDERER_SOFTWARE,
    CROP_BOX,
    CONTOURLIST,
    BACKGROUND_COLOR,
    FONT,
    WIN_RECT,
    ZOOM_FACTOR,
    PRESET,
    CAD_FINDINGS,
    MOMENTUM,
    EVENT_TIMER_3D_PROGRESSIVE_REFINEMENT,
    EVENT_TIMER_3D_FLY,
    EVENT_TIMER_MOMENTUM,
    ALL
  }; // enum vxEnvironmentEnum


  /// Defines strings useful to the environment elements
  #define ENVIRONMENT_ELEMENT_STRINGS {"NONE", "VOLUME_INTENSITY", "VOLUME_INTENSITY_HISTOGRAM", "VOLUME_LABEL","COLORMAP",\
    "COMPONENT_VISUAL", "COMPONENT_SELECTION", "COMPONENT_NAME", \
    "INTERACTIVE_SEGMENTATION", "HANDLESET", "CAMERA", "PLANE_MPR", "SLICE_SHADOWS", "TOOLBAR", \
    "RENDERING_MODE", "SCULPT_CONTOUR", "ACTIVE_ENVIRONMENT", "CURSOR3D", "CURRENT_VOXEL",                        \
    "DATA_RENDERER_SOFTWARE", "CROP_BOX", "CONTOURLIST", "BACKGROUND_COLOR", "FONT", "WIN_RECT", "ZOOM_FACTOR", "PRESET", "CAD_FINDINGS", "MOMENTUM", \
    "EVENT_TIMER_3D_PROGRESSIVE_REFINEMENT", "EVENT_TIMER_3D_FLY", "EVENT_TIMER_MOMENTUM", "ALL" };

// construction
public:

  /// default constructor
  vxEnvironment();

  /// destructor
  virtual ~vxEnvironment();
  
private:

  /// copy constructor
  vxEnvironment(const vxEnvironment & other);

// member functions
public:

  /// creates an fully populated instance of vxEnvironment
  static vxEnvironment * Create();
  
  /// frees all resources associated with the specified enviroment
  static void Free( vxEnvironment * pEnvironment );

  /// set part of environment as modified
  void SetModified(const vxEnvironmentEnum eElement, const bool bModified);

  /// set the modified state of an element
  void SetModified(const vxID id, const bool bModified);

  /// get true if modified
  bool IsModified(const vxEnvironmentEnum eElement) const;

  /// get the modified state of an element
  bool IsModified(const vxID id) const;

  /// gets the modified bits
  const std::bitset<ALL> & GetModifiedBits() { return m_modified; } 

  /// sets the modified bits
  void SetModifiedBits(std::bitset<ALL> & modified) { m_modified = modified; } 

  /// get the modified bits of the extended elements
  std::list<vxID> GetModifiedBitsOfExtendedElements() { return m_vModified; } 

  /// sets the modified bits of the extended elements
  void SetModifiedBitsOfExtendedElements(std::list<vxID> & vModified) { m_vModified = vModified; } 


  ////////////////////////////////////////////////////
  //
  //   2nd class citizens (dynamic elements)
  //
  ////////////////////////////////////////////////////

  /// adds an element to the environment
  void AddElement(const vxID & id, vxElement  * pElement);

  /// removes an element from the environment
  void RemoveElement(const vxID & id);

  /// get an element from the environment
  vxElement * GetElement(const vxID & id) const;

  /// get a string with the list of all changed environment elements
  std::string GetModifiedElementsString() const;

  ////////////////////////////////////////////////////
  //
  //   1st class citizens
  //
  ////////////////////////////////////////////////////

  /// get pointer to background color
  vxShareableObject<PixelRGBA<uint1> > * GetBackgroundColor() const { return m_pBackgroundColor; }

  /// set background color pointer
  void SetBackgroundColor(vxShareableObject<PixelRGBA<uint1> > * pColor) { m_pBackgroundColor = pColor; }

  /// get pointer to colormap
  vxShareableObject<vxColormap> * GetColormap() const { return m_pColormap; }

  /// set colormap pointer
  void SetColormap(vxShareableObject<vxColormap> * pColormap) { m_pColormap = pColormap; }

  /// get preset
  vxPreset * GetPreset() const { return m_pPreset; }

  /// set preset
  void SetPreset(vxShareableObject<vxPreset> * pPreset) { m_pPreset = pPreset; }

  /// get pointer to rendering mode
  vxShareableObject<vxRenderingMode> * GetRenderingMode() const { return m_pRenderingMode; }

  /// set rendering mode point in environment
  void SetRenderingMode(vxShareableObject<vxRenderingMode> * pRenderingMode) { m_pRenderingMode = pRenderingMode; }

  /// get GLEnvironment
  GLEnv & GetGLEnvironment() { return m_glEnv; }

  /// get rendering context pointer
  vxIRenderingContext * GetContext();

  /// get the component handler pointer
  vxShareableObject<vxComponentArray> * GetComponents() const { return m_pComponents; }

  /// set the component handler pointer
  void SetComponents(vxShareableObject<vxComponentArray> * pvComponents) { m_pComponents = pvComponents; }

  /// get the handles set pointer
  vxShareableObject<vxAnnotations> * GetAnnotations() const { return m_pAnnotations; }

  /// set the handles set pointer
  void SetAnnotations(vxShareableObject<vxAnnotations> * pAnnotations) { m_pAnnotations = pAnnotations; }

  /// get the sculpt contour pointer
  vxShareableObject<vxContour> * GetSculptContour() const { return m_pSculptContour; }

  /// set the sculpt contour pointer
  void SetSculptContour(vxShareableObject<vxContour> * pSculptContour) { m_pSculptContour = pSculptContour; }

  /// get the interactive segmentation pointer
  vxShareableObject<InteractiveSegmentation> * GetInteractiveSegmentation() const { return m_pInteractiveSeg; }

  /// set the interactive segmentation pointer
  void SetInteractiveSegmentation(vxShareableObject<InteractiveSegmentation> * pInteractiveSegmentation) { m_pInteractiveSeg = pInteractiveSegmentation; }

  /// get the camera pointer
  vxShareableObject<vxCamera> * GetCamera() const { return m_pCamera; }

  /// set the camera pointer
  void SetCamera(vxShareableObject<vxCamera> * pCamera) { m_pCamera = pCamera; }

  /// get the font info pointer
  vxShareableObject<vxFontInfo> * GetFontInfo() const { return m_pFontInfo; }

  /// set the font info pointer
  void SetFontInfo(vxShareableObject<vxFontInfo> * pFontInfo) { m_pFontInfo = pFontInfo; }

  /// get used font pointer
  GLFontGeorge * GetFont();

  /// creates a font for use during rendering
  void UpdateFont();

  /// creates a font for use during rendering
  void UpdateFont( GLFontGeorge * pFont, vxIRenderingContext * pContext );

  /// get the plane ortho plane pointer
  vxShareableObject<vxPlaneOrtho<float4> > * GetPlaneOrtho() const { return m_pPlaneOrtho; }

  /// set the plane ortho pointer
  void SetPlaneOrtho(vxShareableObject<vxPlaneOrtho<float4> > * pPlaneOrtho) { m_pPlaneOrtho = pPlaneOrtho; }

  /// get the plane ortho plane pointer
  vxShareableObject<vxPlaneOblique<float4> > * GetPlaneOblique() const { return m_pPlaneOblique; }

  /// set the plane ortho pointer
  void SetPlaneOblique(vxShareableObject<vxPlaneOblique<float4> > * pPlaneOblique) { m_pPlaneOblique = pPlaneOblique; }

  /// get the plane curved pointer
  vxShareableObject<vxPlaneCurved> * GetPlaneCurved() const { return m_pPlaneCurved; }

  /// set the plane curved pointer
  void SetPlaneCurved(vxShareableObject<vxPlaneCurved> * pPlaneCurved) { m_pPlaneCurved = pPlaneCurved; }

  /// get data object needed for image renderer
  vxShareableObject<vxDataRendererSoftware> * GetDataRendererSoftware() const { return m_pDataRendererSoftware; }

  /// set data object needed for image renderer
  void SetDataRendererSoftware(vxShareableObject<vxDataRendererSoftware> * pDataRendererSoftware) { m_pDataRendererSoftware = pDataRendererSoftware; }
 
  /// get the current offset value
  uint4 GetOffset() const { return m_uOffset; }

  /// set the current offset value
  void SetOffset(uint4 uOffset);
  
  /// get the maximum MaxOffset value
  uint4 GetMaximumOffset() const { return m_uMaxOffset; }

  /// set the maximum MaxOffset value
  void SetMaximumOffset(uint4 uMaxOffset);

  /// get the slice scroll factor
  uint4 GetScrollFactor() const { return m_uScrollFactor; }

  /// set the slice scroll factor
  void SetScrollFactor(uint4 uFactor) { m_uScrollFactor = uFactor; }

  /// get the intensity block volume pointer
  vxShareableObject<vxBlockVolume<uint2> > * GetIntensityVolume() const { return m_pVolumeIntensity; }

  /// set the intensity block volume pointer
  void SetIntensityVolume(vxShareableObject<vxBlockVolume<uint2> > * pVolumeIntensity) { m_pVolumeIntensity = pVolumeIntensity; }

  /// get the label block volume pointer
  vxShareableObject<vxBlockVolume<uint2> > * GetLabelVolume() const { return m_pVolumeLabel; }

  /// set the label block volume
  void SetLabelVolume(vxShareableObject<vxBlockVolume<uint2> > * pVolumeLabel) { m_pVolumeLabel = pVolumeLabel; }

  /// get current voxel position
  vxShareableObject<Vector3D<int4> > * GetCurrentVoxelPos() const { return m_pvCurrentVoxelPos; }

  /// set current voxel position
  void SetCurrentVoxelPos(vxShareableObject<Vector3D<int4> > * pvCurrentVoxelPos) { m_pvCurrentVoxelPos = pvCurrentVoxelPos; }

  /// get the zoom factor pointer
  vxShareableObject<vxZoomFactor> * GetZoomFactor() const { return m_pZoomFactor; }

  /// set the zoom factor pointer
  void SetZoomFactor(vxShareableObject<vxZoomFactor> * pvZoomFactor) { m_pZoomFactor = pvZoomFactor; }

  /// get the histogram pointer
  BaseHistogram * GetHistogram() const { return m_pHistogram; }

  /// set the histogram pointer
  void SetHistogram(BaseHistogram * pHistogram) { m_pHistogram = pHistogram; }

  /// get the cine movement
  vxCine * GetCine() const { return m_pCine; }

  /// set the cine movement
  void SetCine( vxCine * pCine ) { m_pCine = pCine; }

  /// get the camera fly
  vxCameraFlySettings * GetCameraFlySettings() const { return m_pCameraFlySettings; }

  /// set the camera fly
  void SetCameraFlySettings( vxCameraFlySettings * pCameraFly ) { m_pCameraFlySettings = pCameraFly; }

  /// get the momentum state
  vxShareableObject< vxMomentum > * GetMomentum() const { return m_pMomentum; }

  /// set the momentum
  void SetMomentum(vxShareableObject< vxMomentum > * pMomentum) { m_pMomentum = pMomentum; }

  /// get pointer to camera location and direction
  vxShareableObject<vxCursor3d> * GetCursor3d() const { return m_pCursor3d; }

  /// set camera location and direction pointer
  void SetCursor3d(vxShareableObject<vxCursor3d> * pCursor3d) { m_pCursor3d = pCursor3d; }

  /// get the crop box pointer
  vxShareableObject<vxCropBox> * GetCropBox() const { return m_pCropBox; }

  /// set the crop box pointer
  void SetCropBox(vxShareableObject<vxCropBox> * pCropBox) { m_pCropBox = pCropBox; }

  /// get the contour list pointer
  vxShareableObject<vxContourList> * GetContourList() const { return m_pContourList; }

  /// set the contour list pointer
  void SetContourList(vxShareableObject<vxContourList> * pContourList) { m_pContourList = pContourList; }

  /// return the feedback manager
  vxInputFeedback & GetInputFeedback() const { return m_inputFeedback; }

  /// return the mouse cursor object
  vxMouseCursor & GetMouseCursor() { return m_cursor; }

  /// returns the size and location of the renderers area
  const vxWinRect & GetWinRect() const { return m_winRect; }

  /// set the size and location of the renderers area
  void SetWinRect(const vxWinRect & winRect) { m_winRect = winRect; }

  /// returns pointer to the viewer type
  vxViewerType * GetViewerType() const { return m_pViewerType; }

  /// set pointer to the viewer type
  void SetViewerType(vxViewerType * pViewerType) { m_pViewerType = pViewerType; }
  
  /// sets the render buffer
  void SetRenderBuffer( const vxRenderBuffer & renderBuffer ){ m_renderBuffer = renderBuffer; }

  /// gets the render buffer
  vxRenderBuffer & GetRenderBuffer() { return m_renderBuffer; }

  /// get the cad findings
  vxShareableObject< vxCAD::vxCadFindings > *  GetCadFindings() const { return m_pCadFindings; }

  /// set the cad findings
  void SetCadFindings( vxShareableObject< vxCAD::vxCadFindings > * pCadFindings ) { m_pCadFindings = pCadFindings; }

  /// get the visualization graph
  vxVisualizationGraph * GetVisualizationGraph() const { return m_pVisualizationGraph; }

  /// set the visualization graph
  void SetVisualizationGraph( vxVisualizationGraph * pVisualizationGraph ) { m_pVisualizationGraph = pVisualizationGraph; }

  /// gets if modified events should be ignored
  const bool GetIgnoreModifiedEvents() const { return m_bIgnoreModifiedEvents; }

  /// sets if modified events should be ignored
  void SetIgnoreModifiedEvents( bool bIgnoreModifiedEvents ) { m_bIgnoreModifiedEvents = bIgnoreModifiedEvents; }

  /// get the associated dataset
  vxDataset * GetDataset() const { return m_pDataset; }

  /// set the associated dataset
  void SetDataset( vxDataset * pDataset ) { m_pDataset = pDataset; }

  /// get frame rate timer
  Timer & GetFrameRateTimer() { return m_frameRateTimer; }

  /// returns if the user is interacting with the environment
  static bool GetUserInteracting();

  /// sets if the user is interacting with the environment
  static void SetUserInteracting( const bool & bUserInteracting );

  /// returns the active environment pointer
  static const vxEnvironment * GetActiveEnvironment() { return m_pActiveEnvironment; }
  
  /// set the active environment pointer
  static void SetActiveEnvironment(const vxEnvironment * pActiveEnvironment) {m_pActiveEnvironment = pActiveEnvironment; }

  /// returns the active environment pointer
  static const vxEnvironment * GetSelectedEnvironment() { return m_pSelectedEnvironment; }
  
  /// set the active environment pointer
  static void SetSelectedEnvironment(const vxEnvironment * pSelectedEnvironment) {m_pSelectedEnvironment = pSelectedEnvironment; }

  /// returns the undo action stack
  static vxUndoActionStack & GetUndoActions() { return m_undoActions; }

// operators
private:

  /// assignment operator
  const vxEnvironment & operator =(const vxEnvironment & other);

// member variables
private:

  /// modified flags
  std::bitset<ALL> m_modified; 

  /// modified flags
  std::list<vxID> m_vModified; 

  /// feedback for the manipulators to notifiy their action state
  static vxInputFeedback m_inputFeedback;

  /// intensity volume
  vxShareableObject<vxBlockVolume<uint2> > * m_pVolumeIntensity;  

  /// label volume
  vxShareableObject<vxBlockVolume<uint2> > * m_pVolumeLabel;      

  /// colormap
  vxShareableObject<vxColormap> * m_pColormap; 

  /// preset
  vxShareableObject<vxPreset> * m_pPreset;

  /// rendering mode information
  vxShareableObject<vxRenderingMode> * m_pRenderingMode;    

  /// component information
  vxShareableObject<vxComponentArray> * m_pComponents;       

  /// annotations
  vxShareableObject<vxAnnotations> * m_pAnnotations;       

  /// sculpt contour object
  vxShareableObject<vxContour> * m_pSculptContour;      

  /// camera used for viewing
  vxShareableObject<vxCamera> * m_pCamera;        

  /// current voxel
  vxShareableObject<Vector3D<int4> > * m_pvCurrentVoxelPos;

  /// interactive segmentation
  vxShareableObject<InteractiveSegmentation> * m_pInteractiveSeg;

  /// ortho plane
  vxShareableObject<vxPlaneOrtho<float4> > * m_pPlaneOrtho;

  /// oblique plane
  vxShareableObject<vxPlaneOblique<float4> > * m_pPlaneOblique;

  /// curved plane
  vxShareableObject<vxPlaneCurved> * m_pPlaneCurved;

  /// active environment
  static const vxEnvironment * m_pActiveEnvironment;

  /// selected environment
  static const vxEnvironment * m_pSelectedEnvironment;

  /// crop Box
  vxShareableObject<vxCropBox> * m_pCropBox;

  /// contour list
  vxShareableObject<vxContourList> * m_pContourList;

  /// camera location and direction
  vxShareableObject<vxCursor3d> * m_pCursor3d;

  /// zoom factor
  vxShareableObject<vxZoomFactor> * m_pZoomFactor;    

  /// histogram
  BaseHistogram * m_pHistogram;      

  /// object needed for the image renderer
  vxShareableObject<vxDataRendererSoftware> * m_pDataRendererSoftware;

  /// background color
  vxShareableObject<PixelRGBA<uint1> > * m_pBackgroundColor;

  /// momentum settings
  vxShareableObject<vxMomentum> * m_pMomentum;

  /// OpenGL environment
  GLEnv m_glEnv;         

  /// offset for slices relative to each other
  uint4 m_uOffset;  
  
  /// font information
  vxShareableObject<vxFontInfo> * m_pFontInfo;      

  /// maximum offset for slices relative to each other
  uint4 m_uMaxOffset;

  /// slice scroll factor
  uint4 m_uScrollFactor;

  /// cine movement
  vxCine * m_pCine;    

  /// camera flying settings
  vxCameraFlySettings * m_pCameraFlySettings;

  /// cad findings
  vxShareableObject< vxCAD::vxCadFindings > * m_pCadFindings;

  /// visualization graph
  vxVisualizationGraph * m_pVisualizationGraph;

  /// indicates if modified events should be ignored
  bool m_bIgnoreModifiedEvents;
   
  /// mouse cursor
  vxMouseCursor m_cursor;

  /// typedef'd elements collection type
  typedef std::list<std::pair<vxID, vxElement *> > vxElements;

  /// environment elements
  vxElements m_elements;

  /// area and location of rendering area
  vxWinRect m_winRect;

  /// type of the viewer
  vxViewerType * m_pViewerType;

  /// associated dataset
  vxDataset * m_pDataset;
  
  /// the undo actions
  static vxUndoActionStack m_undoActions;

  /// indicates if user is interacting with environment
  static LONG m_iInteractingCount;

  /// render buffer
  vxRenderBuffer  m_renderBuffer;

  /// frame rate timer
  Timer m_frameRateTimer;

}; // class vxEnvironment


// $Log: vxEnvironment.h,v $
// Revision 1.44.2.1  2009/07/28 15:02:53  kchalupa
// Code Walkthrough
//
// Revision 1.44  2008/01/18 21:29:28  geconomos
// Issue# 5869: Images updating in background while making a movie
//
// Revision 1.43  2007/10/24 20:11:28  gdavidson
// Moved the rendering context and font to the visualization graph
//
// Revision 1.42  2007/10/03 20:35:31  gdavidson
// Added gets and sets for SelectedEnvironment
//
// Revision 1.41  2007/09/19 14:33:22  gdavidson
// Added get and set methods for the associated vxDataset
//
// Revision 1.40  2007/06/28 19:34:09  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.39  2007/05/03 21:30:53  gdavidson
// Added vxVisualizationGraph
//
// Revision 1.38  2007/04/11 20:53:39  romy
// Added CAD_FINDINGS bit added
//
// Revision 1.37  2007/04/06 14:12:29  gdavidson
// Added vxCadFindings
//
// Revision 1.36  2007/04/02 21:17:32  gdavidson
// Removed GetRenderBufferModified
//
// Revision 1.35.2.2  2007/03/27 21:54:33  gdavidson
// Rolled back
//
// Revision 1.35.2.1  2007/03/22 21:36:05  gdavidson
// Issue #5505: Added ResetUserInteracting method
//
// Revision 1.35  2007/02/05 15:29:25  geconomos
// reworked "user interacting" mechanism
//
// Revision 1.34  2007/01/16 17:05:23  gdavidson
// Added vxCine and vxCameraFlySettings and removed timer
//
// Revision 1.33  2006/11/11 03:12:48  geconomos
// made GetUserInteracting static
//
// Revision 1.32  2006/10/24 12:30:53  geconomos
// removed old slice shadows mechanism
//
// Revision 1.31  2006/10/20 20:47:52  geconomos
// replaced unused modified value "TEXTURE_SERVER" with "ACTIVE_ENVIRONMENT"
//
// Revision 1.30  2006/09/28 18:34:18  geconomos
// Issue 4963: added indicator for non-optimal image quality
//
// Revision 1.29  2006/08/23 17:57:33  geconomos
// + made "render buffer" a concrete instance
// + added flag to indicate if "render buffer" has been modified
//
// Revision 1.28  2006/08/03 13:59:53  geconomos
// + removed rendering engine and rendering engine data
// + added render buffer
//
// Revision 1.27  2006/07/07 13:33:06  geconomos
// added static boolean property that indicates if a user is interacting with any environment
//
// Revision 1.26  2006/06/27 20:12:42  geconomos
// added rendering engine and associated data as first class citizens
//
// Revision 1.25  2006/04/03 18:43:43  geconomos
// BaseHistogram is no longer a shareable object
//
// Revision 1.24  2006/03/29 20:03:19  geconomos
// binary histogram is now contained within the block info structure, updated accordingly
//
// Revision 1.23  2006/02/09 14:05:54  geconomos
// added SetFont()
//
// Revision 1.22  2006/01/27 15:47:47  geconomos
// still migrating to visual studio 2005
//
// Revision 1.21  2005/12/08 16:40:24  frank
// added undo support
//
// Revision 1.20  2005/11/18 21:13:45  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.19.2.1  2005/08/12 17:20:51  geconomos
// Implmentation of vxMomentum environment element
//
// Revision 1.19  2005/05/20 19:09:39  frank
// fixed 3D cursor on 3x3 view, fixed active environment and oblique 'self-shadowing' problem
//
// Revision 1.18  2005/05/20 12:13:18  frank
// cleaning up slice shadows in 3D
//
// Revision 1.17  2001/01/03 20:55:33  michael
// Ordered the functions, Get...() followed by Set...()
//
// Revision 1.16  2001/01/03 19:10:39  michael
// "Replaced" ActiveViewerType by ActiveEnvironment.
//
// Revision 1.15  2004/10/04 15:02:52  frank
// cleaned up bit set access functions
//
// Revision 1.14  2004/09/01 12:30:24  frank
// decreased the granularity of component array modified bits
//
// Revision 1.13  2004/08/31 15:55:14  michael
// Adjusted code to correclty load old 8 bit .sld/.slc into vxBlockVolume
//
// Revision 1.12  2004/08/30 18:19:38  michael
// Data used by renderers is now polymorph but so far only on the vx side of things.
// Need to add the inheritance on the managed V3D side, too. Still to be done!!!
//
// Revision 1.11  2004/04/26 19:04:42  michael
// Adjusted code to reflect the changes in zomm factor, roi not being part of
// camera anymore, etc.
//
// Revision 1.10  2004/04/21 18:34:07  geconomos
// Added static methods for creating and freeing a vxEnvionrment.
//
// Revision 1.9  2004/04/14 21:00:01  frank
// just one accessor function needs an inline for some reason
//
// Revision 1.8  2004/04/14 18:48:53  frank
// Extended presets to set the rendering mode and other visualization settings
//
// Revision 1.7  2004/04/06 16:31:15  frank
// unified environment access naming
//
// Revision 1.6  2004/04/06 12:09:52  frank
// no use for the skeleton in the environment
//
// Revision 1.5  2004/04/02 14:03:10  frank
// working on unifying the various rendering contexts
//
// Revision 1.4  2004/03/12 19:53:40  frank
// removed annotation ROI and readjusted sculpting to remove handle set
//
// Revision 1.3  2004/03/12 18:03:37  frank
// working on removing annotation ROI - first renaming contour to contour list
//
// Revision 1.2  2004/03/05 19:34:55  geconomos
// Removed references to vxServerTexMap2d and vxServerTexMap3d
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.51  2004/03/02 01:10:38  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.50  2004/02/20 23:33:43  michael
// added new zoom factor class
//
// Revision 1.49.2.2  2004/02/24 19:53:05  michael
// added "private:" since that was actually a function and not yet data
//
// Revision 1.49.2.1  2004/02/20 17:44:32  michael
// order of textified environment elements was not matching enum order
//
// Revision 1.49  2004/01/30 01:20:00  geconomos
// added WIN_RECT to event enum.
//
// Revision 1.48  2004/01/14 14:18:17  michael
// reworked vxViewerType to have conversion from mouse to screen, screen to
// world, and world to screen.
//
// Revision 1.47  2003/12/18 14:14:29  frank
// Renamed the element ID filename to match class name
//
// Revision 1.46  2003/12/17 15:38:56  michael
// effort of accelerating the Redraw() by finding out which renderer requires a redraw
//
// Revision 1.45  2003/10/02 10:23:29  geconomos
// Added vxFontInfo to environment as well as a method to update the font based on the settings contained in vxFontInfo.
//
// Revision 1.44  2003/09/29 17:17:55  michael
// added event enum flags to vxEnvironment and new events to plane & camera
//
// Revision 1.43  2003/09/22 15:56:40  michael
// added EVENT_TIMER
//
// Revision 1.42  2003/07/29 11:05:27  michael
// GetHistogram() --> GetHistogramPtr()
//
// Revision 1.41  2003/07/23 16:56:57  michael
// took out redundant stuff (old 8bit and 12 bit CPU color tables)
//
// Revision 1.40  2003/06/18 15:19:52  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.39  2003/06/09 12:29:54  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.38  2003/05/27 13:10:59  frank
// Removed dead references
//
// Revision 1.37  2003/05/12 18:15:56  frank
// Added slice shadows object
//
// Revision 1.36  2003/05/07 15:01:04  dongqing
// code walkthrough
//
// Revision 1.35  2003/05/06 18:25:50  michael
// initial code review
//
// Revision 1.34  2003/04/29 21:19:04  michael
// renamed RenderQualitySpeedTradeoff to vxRendererCpu3dData
//
// Revision 1.33  2003/04/24 13:58:57  michael
// initial code review
//
// Revision 1.32  2003/04/07 18:02:46  michael
// fixed background color for MPR views
//
// Revision 1.31  2003/04/07 15:52:36  michael
// transition from BackColor to BackgroundColor
//
// Revision 1.30  2003/04/01 16:02:51  geconomos
// Added VOLUME_INTENSITY_HISTOGRAM.
//
// Revision 1.29  2003/03/20 13:48:01  geconomos
// Proper initialization of m_pViewerType.
//
// Revision 1.28  2003/03/11 18:44:12  geconomos
// Added static member "ActiveViewerType" and related accessors.
//
// Revision 1.27  2003/01/28 19:31:28  geconomos
// Implementation of BackgroundColor.
//
// Revision 1.26  2003/01/28 15:03:40  frank
// Const access functions should be const!
//
// Revision 1.25  2003/01/24 21:06:39  michael
// added background color to the environment
//
// Revision 1.24  2003/01/09 14:54:57  michael
// added new annotations class to the environment
//
// Revision 1.23  2002/12/18 15:19:31  michael
// added vxViewerType
//
// Revision 1.22  2002/12/17 14:32:32  michael
// added the winRect to the environment
//
// Revision 1.21  2002/11/27 21:58:13  ingmar
// added GetModifiedElementsString()
//
// Revision 1.20  2002/11/18 16:36:55  michael
// completed plane object renaming
//
// Revision 1.19  2002/11/13 22:08:45  frank
// Renamed vxPlaneOblique to vxViewObliqueData,
// Demoted the data to a second class citizen in the environment.
//
// Revision 1.18  2002/11/13 20:11:55  frank
// enabled oblique view renderer and manipulator
//
// Revision 1.17  2002/11/12 21:29:21  geconomos
// Made vxContour  a first class citizen of vxEnvironment (frank really did it).
//
// Revision 1.16  2002/11/06 18:28:39  frank
// formatting
//
// Revision 1.15  2002/11/05 22:50:10  ingmar
// 3DCursor -> Cursor3d
//
// Revision 1.14  2002/11/01 21:27:53  frank
// Added Crop Box to environment.
//
// Revision 1.13  2002/10/03 17:36:13  michael
// Extended possible presets (Colormap and Camera)
//
// Revision 1.12  2002/09/26 18:11:42  dmitry
// Colormp presets are now an instance instead of a pointer.
//
// Revision 1.11  2002/09/25 20:36:41  michael
// Completed vxTransferFunction to vxColormap transition
//
// Revision 1.10  2002/09/20 13:12:19  michael
// removed old member transferfunction pointer
//
// Revision 1.9  2002/09/19 19:52:18  michael
// fixed missing initialization of two pointers and removed dangling member pointer
// to old transferfunction.
//
// Revision 1.8  2002/09/19 19:34:28  michael
// Initial version for std::vec of colormap presets in environment
//
// Revision 1.7  2002/09/19 14:49:49  michael
// Removed TransferFunction from the vxEnvironment
//
// Revision 1.6  2002/09/19 14:25:24  michael
// completed transition from vxFunction to vxColormap
//
// Revision 1.5  2002/09/16 19:57:23  dmitry
// vxID.h included
//
// Revision 1.4  2002/09/10 20:37:55  michael
// Fixed "foreward" --> "forward"
//
// Revision 1.3  2002/09/05 22:37:46  jaddonisio
// New vxComponent.
//
// Revision 1.2  2002/08/05 16:19:31  dmitry
// vxEventTimer added
//
// Revision 1.1  2002/07/30 20:21:05  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.47  2002/07/02 20:44:57  geconomos
// Exporting classes .
//
// Revision 1.46  2002/06/27 19:27:34  michael
// completed transition from CameraNew to vxCamera
//
// Revision 1.45  2002/06/21 18:21:07  dmitry
// CURRENT_VOXEL enum edded.
//
// Revision 1.44  2002/06/21 16:26:38  frank
// Implemented PlaneCurved.
//
// Revision 1.43  2002/06/20 02:53:06  dmitry
// Renamed Historgam from Volume to Binary.
//
// Revision 1.42  2002/06/20 02:22:14  dmitry
// Added to the Environment: 3D Cursor, MouseCursor, Intensity Volume Histogram, and Label Volume Histogram.
//
// Revision 1.41  2002/06/18 21:08:02  dmitry
// Changed vxElements to list instead of map and moved GetElement to C file.
//
// Revision 1.40  2002/06/13 23:22:35  michael
// added feedback information from the manipulators so that they can be removed
// from VB once operation is completed ...
//
// Revision 1.39  2002/06/13 19:24:23  michael
// added Cursor3d
//
// Revision 1.38  2002/06/07 21:42:25  dmitry
// GetElement changed to return * vxElement and old signature commented out.
//
// Revision 1.37  2002/06/07 20:15:03  dmitry
// modified mechanism based on vxID added
//
// Revision 1.36  2002/06/07 19:59:55  geconomos
// Changed extended elements fron shareable object to vxElement.
//
// Revision 1.35  2002/06/06 18:00:52  dmitry
// SWING added to CineEnum.
//
// Revision 1.34  2002/06/05 21:38:11  geconomos
// Initial extended element implementation.
//
// Revision 1.33  2002/06/04 16:11:52  dmitry
// slice scroll factor added
//
// Revision 1.32  2002/05/30 15:25:11  dmitry
// vxServerTexMap3D added.
//
// Revision 1.31  2002/05/16 22:33:47  dmitry
// Texture Server made Shareable.
//
// Revision 1.30  2002/05/09 21:03:24  jenny
// added histogram to the environment
//
// Revision 1.29.2.2  2002/05/23 18:44:00  frank
// added dynamic elements
//
// Revision 1.29.2.1  2002/05/07 23:10:22  frank
// Implemented GetPlaneCurved.
//
// Revision 1.29  2002/05/06 21:45:29  dmitry
// Cine and Momentum added.
//
// Revision 1.28  2002/05/03 17:38:10  jenny
// zoom factor
//
// Revision 1.27  2002/05/01 22:36:17  michael
// replaced VOLUMESET and VOLUMELABELSET by VOLUME_{INTENSITY,LABEL}
//
// Revision 1.26  2002/05/01 22:10:53  michael
// Replaced DensityVolume by IntensityVolume
//
// Revision 1.25  2002/05/01 21:14:59  jenny
// no volumeset
//
// Revision 1.24  2002/05/01 14:11:24  michael
// cosmetics
//
// Revision 1.23  2002/05/01 13:48:33  michael
// Clean-up of the environment ...
//
// Revision 1.22  2002/05/01 12:12:35  michael
// Removed slab thickness, slice number, number of slices, etc.
//
// Revision 1.21  2002/05/01 00:04:56  michael
// Removed old stuff such as CROPBOX, CARDIACPLAQUES, IsEnabled(), SetEnabled(), etc.
//
// Revision 1.20  2002/04/29 20:29:46  uday
// sliceshadows is a sharable object.
//
// Revision 1.19  2002/04/25 16:58:33  jenny
// worked on shareable object currentvoxel
//
// Revision 1.18  2002/04/24 17:39:31  jenny
// vxBlockVolume stuff
//
// Revision 1.17  2002/04/24 16:10:05  uday
// Added SliceShadow PlaneOrtho and access functions
//
// Revision 1.16  2002/04/23 20:12:23  michael
// replaced Density by Intensity
//
// Revision 1.15  2002/04/23 15:08:07  jenny
// switched to vxBlockVolume<uint2>
//
// Revision 1.14  2002/04/18 20:19:01  frank
// Added sculpting manipulator and renderer.
//
// Revision 1.13  2002/04/10 20:34:06  manju
// Removed Get/Set slice number and GetNumberofSlices methods.
//
// Revision 1.12  2002/04/08 21:08:54  dmitry
// Offset information added.
//
// Revision 1.11  2002/04/02 21:46:42  geconomos
// Added vxServerTexMap2d to environment.
//
// Revision 1.10  2002/04/01 16:29:15  dmitry
// Removed enums that are no longer supported or implemented in vxRenderingSettings.
//
// Revision 1.9  2002/03/29 22:43:37  dmitry
// TOOLBAR enum added.
//
// Revision 1.8  2002/03/29 15:52:01  michael
// added rendering mode class
//
// Revision 1.7  2002/03/29 01:29:32  ingmar
// added RenderQualitySpeedTradeoff
//
// Revision 1.6  2002/03/28 23:09:28  jaddonisio
// Changed "TAG" to "LABEL" in enum.
// Also, changed class to inherit from Sec_74_vxUtils.
//
// Revision 1.5  2002/03/26 18:42:06  dmitry
// Moved creation of GLContext to v3D_Viewer.
//
// Revision 1.4  2002/03/24 16:36:06  michael
// adjusted to new dsp files with less dependencies and introduced more forward declarations ... to be continued
//
// Revision 1.3  2002/03/20 15:31:05  dmitry
// Split the Planes into Ortho, Oblique and Curved.
//
// Revision 1.2  2002/03/20 11:46:09  michael
// comments
//
// Revision 1.1  2002/03/19 17:14:43  dmitry
// Added initial version of 74_vxUtils
//
// Revision 1.1  2002/03/19 16:40:10  dmitry
// Moved to 75_vxUtils from 75_vxManipulators
//
// Revision 1.5  2002/03/18 22:47:09  dmitry
// Put GlFontGeorge into the env, and moved functionality into the .C file.
//
// Revision 1.4  2002/03/16 18:08:25  michael
// made PlaneSimple a PlaneOrtho<float4> but this will be generalized soon ...
//
// Revision 1.3  2002/03/15 17:32:05  jaddonisio
// Change name of TagVolume variables to LabelVolume.
//
// Revision 1.2  2002/03/15 01:29:00  michael
// using box (math) instead of cropBox (renderutils)
//
// Revision 1.1  2002/03/11 19:42:44  michael
// moved xvManipulators, vxInput, and vxEnvironment to 75_vxManipulators
//
// Revision 1.3  2002/03/07 22:31:09  dmitry
// Modified to fit new architecture.
//
// Revision 1.2  2002/02/28 15:02:59  michael
// made code (alomst) coding guidelines conform
//
// Revision 1.1  2002/02/22 15:28:58  dmitry
// Initial Versions Replacing same classes w/o vx prefix.
//
// Revision 1.5  2002/01/12 22:47:49  uday
// curved and oblique stuff
//
// Revision 1.4  2001/12/21 18:48:14  ingmar
// added class Sec_75_rendererUtils to structure the doxygen documentation
//
// Revision 1.3  2001/12/19 17:01:09  soeren
// renamed get functions to GetPtr functions
//
// Revision 1.2  2001/12/19 15:39:59  soeren
// no message
//
// Revision 1.1  2001/12/18 17:00:18  soeren
// Rewrote Environement class and moved it from 76_rendererViews to 75_renderUtils
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxEnvironment.h,v 1.44.2.1 2009/07/28 15:02:53 kchalupa Exp $
// $Id: vxEnvironment.h,v 1.44.2.1 2009/07/28 15:02:53 kchalupa Exp $
