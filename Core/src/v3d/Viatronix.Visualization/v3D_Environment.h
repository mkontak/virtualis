// $Id: v3D_Environment.h,v 1.28.2.1 2009/07/29 19:54:06 kchalupa Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: george economos(mailto:george@viatronix.com)
//
// Complete history at bottom of file.

#pragma once


// includes
#include "vxEnvironment.h"
#include "vxShareableObject.h"
#include "v3D_Volume.h"
#include "v3D_Components.h"
#include "v3D_Interfaces.h"
#include "v3D_PlaneOrtho.h"
#include "v3D_PlaneOblique.h"
#include "v3D_PlaneCurved.h"
#include "v3D_RenderingMode.h"
#include "v3D_Annotations.h"
#include "v3D_Colormap.h"
#include "v3D_Camera.h"
#include "v3D_Segmentation.h"
#include "v3D_ContourList.h"
#include "v3D_CropBox.h"
#include "v3D_Cursor3d.h"
#include "v3D_SimpleTypes.h"
#include "v3D_FontInfo.h"
#include "v3D_ZoomFactor.h"
#include "v3D_Elements.h"
#include <gcroot.h>


// unmanaged forward declarations
class vxEnvironment;
class vxManagedObserver;
class vxOverlayStrings;


// forward declarations
OPEN_GRAPH_NS
ref class VisualizationGraph;
CLOSE_GRAPH_NS


// namespaces
OPEN_VISUALIZATION_NS


// managed forward declarations
ref class Preset;
ref class Dataset;
ref class Cine;
ref class Momentum;
ref class CameraFlySettings;
ref class CadFindings;


/**
 * Managed wrapper for the vxEnvironment
 */
public ref class Environment: public IObserver
{
// events
public:
  
  /// raised when the environment changes
  event System::EventHandler ^ Changed
  {
    void add( System::EventHandler ^ mphandler ) 
    {
      m_mpChangedEvent = static_cast< System::EventHandler ^ >( System::Delegate::Combine( m_mpChangedEvent, mphandler )); 
    }

    void remove( System::EventHandler ^ mphandler )
    {
      m_mpChangedEvent = static_cast< System::EventHandler ^ >( System::Delegate::Remove( m_mpChangedEvent, mphandler )); 
    }

    void raise( System::Object ^ mpSender, System::EventArgs ^ mpArgs ) 
    {
      if( m_mpChangedEvent != nullptr )
        m_mpChangedEvent->Invoke( mpSender, mpArgs );
    }
  } // Changed
  
  /// removes all the handlers from the events
  void ResetEvents();

// construction
public:

  /// initializer
  Environment();

  /// destructor
  virtual ~Environment();

  /// finalizer
  !Environment();

// member functions
public:

  /// sets the camera to a standard position
  void SetStandardCameraView( Viatronix::Visualization::CameraViews eView, bool bViewAll );
  
  /// adds an element to the environment
  void AddElement( System::String ^ mpName, Viatronix::Visualization::IElement ^ mpElement );

  /// adds an element to the environment
  void RemoveElement( System::String ^ mpName );

  /// removes all extended elements from the collection
  void RemoveAllElements();

  /// returns the specified element if it exists, null otherwise 
  IElement ^ GetElement( System::String ^ mpName );

  /// returns the specified element if it exists, null otherwise 
  generic <typename T>
  where T : IElement
  T GetElement( System::String ^ mpName );

  /// returns the vxEnvironment
  System::IntPtr GetEnvironmentPtr() { return System::IntPtr( m_pEnvironment ); }

  /// sets this object as the primary observer
  void SetPrimary();  

  /// set all flags modified
  //void SetModified();

  /// begin a sequence of undo actions (must eventually be followed by an end)
  void UndoSequenceBegin();

  /// end a sequence of undo actions (must have been preceeded by a begin)
  void UndoSequenceEnd();

  /// sets the active environment 
  static void SetActiveEnvironment( Viatronix::Visualization::Environment ^ mpEnvironment );

  /// Hack for swapping in a new volume and label volume without accumulating modified events - good for 4D data
  void SwapInDataset( Viatronix::Visualization::Dataset ^ mpDataset );

private:

  /// adds an observer to the element
  void ConnectToElement( Viatronix::Visualization::IElement ^ mpElement );

  /// removes an observer from the element
  void DisconnectFromElement( Viatronix::Visualization::IElement ^ mpElement );  

  // checks if the specified element has been modified
  bool IsModified( const type_info& id, const vxModifiedInfo & info, Viatronix::Visualization::IElement ^ mpElement );

// event handlers
public:

  /// processes modified queue events
  virtual void OnModified( System::IntPtr modifiedList );

private:

  /// called when the modified event is raised
  void OnElementModifiedHandler( System::Object ^ mpSender, System::EventArgs ^ mpArgs );

// properties
public:

  /// gets the main viewer type
  property Viatronix::Visualization::IViewerType ^ ViewerType
  {
    Viatronix::Visualization::IViewerType ^ get() { return m_mpViewerType; }
    void set( Viatronix::Visualization::IViewerType ^ mpViewerType );
  } // ViewerType

  /// gets the intensity volume
  property Viatronix::Visualization::Volume ^ Volume
  {
    Viatronix::Visualization::Volume ^ get() { return m_mpVolume; }
    void set( Viatronix::Visualization::Volume ^ mpVolume );
  } // Volume
  
  /// gets the label volume
  property Viatronix::Visualization::Volume ^ LabelVolume
  {
    Viatronix::Visualization::Volume ^ get() { return m_mpLabelVolume; }
    void set( Viatronix::Visualization::Volume ^ mpLabelVolume );
  } // LabelVolume

  /// gets the Components
  property Viatronix::Visualization::Components ^ Components
  {
    Viatronix::Visualization::Components ^ get() { return m_mpComponents; }
    void set( Viatronix::Visualization::Components ^ mpComponents );
  } // Components
    
  /// gets the PlaneOrtho
  property Viatronix::Visualization::PlaneOrtho ^ Plane
  {
    Viatronix::Visualization::PlaneOrtho ^ get() { return m_mpPlaneOrtho; }
    void set( Viatronix::Visualization::PlaneOrtho ^ mpPlane );
  } // Plane

  /// gets the PlaneOblique
  property Viatronix::Visualization::PlaneOblique ^ PlaneOblique
  {
    Viatronix::Visualization::PlaneOblique ^ get() { return m_mpPlaneOblique; }
    void set( Viatronix::Visualization::PlaneOblique ^ mpPlane );
  } // PlaneOblique

  /// gets the PlaneCurved
  property Viatronix::Visualization::PlaneCurved ^ PlaneCurved
  {
    Viatronix::Visualization::PlaneCurved ^ get() { return m_mpPlaneCurved; }
    void set( Viatronix::Visualization::PlaneCurved ^ mpPlane );
  } // PlaneCurved

  /// gets the Colormap 
  property Viatronix::Visualization::Colormap ^ Colormap
  {
    Viatronix::Visualization::Colormap ^ get() { return m_mpColormap; }
    void set( Viatronix::Visualization::Colormap ^ mpColormap );
  } // Colormap

  /// gets the CropBox
  property Viatronix::Visualization::CropBox ^ CropBox
  {
    Viatronix::Visualization::CropBox ^ get() { return m_mpCropBox; }
    void set( Viatronix::Visualization::CropBox ^ mpCropBox );
  } // CropBox

  /// gets the ContourList
  property Viatronix::Visualization::ContourList ^ ContourList
  {
    Viatronix::Visualization::ContourList ^ get() { return m_mpContourList; }
    void set( Viatronix::Visualization::ContourList ^ mpContourList );
  } // ContourList

  /// gets the Cursor3d
  property Viatronix::Visualization::Cursor3d ^ Cursor3d
  {
    Viatronix::Visualization::Cursor3d ^ get() { return m_mpCursor3d; }
    void set( Viatronix::Visualization::Cursor3d ^ mpCursor3d );
  } // Cursor3d

  /// gets the Camera
  property Viatronix::Visualization::Camera ^ Camera
  {
    Viatronix::Visualization::Camera ^ get() { return m_mpCamera; }
    void set( Viatronix::Visualization::Camera ^ mpCamera );
  } // Camera
  
  /// gets the RenderingMode
  property Viatronix::Visualization::RenderingMode ^ RenderingMode
  {
    Viatronix::Visualization::RenderingMode ^ get() { return m_mpRenderingMode; }
    void set( Viatronix::Visualization::RenderingMode ^ mpRenderingMode );
  } // RenderingMode

  /// gets the Annotations
  property Viatronix::Visualization::Annotations ^ Annotations
  {
    Viatronix::Visualization::Annotations ^ get() { return m_mpAnnotations; }
    void set( Viatronix::Visualization::Annotations ^ mpAnnotations );
  } // Annotations

  /// gets the Segmentation
  property Viatronix::Visualization::Segmentation ^ Segmentation
  {
    Viatronix::Visualization::Segmentation ^ get() { return m_mpSegmentation; }
    void set( Viatronix::Visualization::Segmentation ^ mpSegmentation );
  } // Segmentation

  /// gets the Preset
  property Viatronix::Visualization::Preset ^ Preset
  {
    Viatronix::Visualization::Preset ^ get() { return m_mpPreset; }
    void set( Viatronix::Visualization::Preset ^ mpPreset );
  } // Preset
  
  /// gets the DataRendererSoftware3d
  property Viatronix::Visualization::DataRendererSoftware ^ DataRendererSoftware
  {
    Viatronix::Visualization::DataRendererSoftware ^ get() { return m_mpDataRendererSoftware; }
    void set( Viatronix::Visualization::DataRendererSoftware ^ mpDataRendererSoftware );
  } // DataRendererSoftware

  /// gets the zoom factor
  property Viatronix::Visualization::ZoomFactor ^ ZoomFactor
  {
    Viatronix::Visualization::ZoomFactor ^ get() { return m_mpZoomFactor; }
    void set( Viatronix::Visualization::ZoomFactor ^ mpZoomFactor );
  } // ZoomFactor

  /// gets the MaximumOffset
  property int MaximumOffset
  {
    int get() { return m_pEnvironment->GetMaximumOffset(); }
    void set( int iMaximumOffset ) { m_pEnvironment->SetMaximumOffset( iMaximumOffset ); }
  } // MaximumOffset
 
  /// gets the Offset
  property int Offset
  {
    int get() { return m_pEnvironment->GetOffset(); }
    void set( int iOffset ) { m_pEnvironment->SetOffset( iOffset ); }
  } // Offset
  
  /// gets the ScrollFactor
  property int ScrollFactor
  {
    int get() { return m_pEnvironment->GetScrollFactor(); }
    void set( int iFactor ) { m_pEnvironment->SetScrollFactor( iFactor ); }
  } // ScrollFactor
  
  /// gets the CineMode
  property Viatronix::Visualization::Cine ^ Cine
  {
    Viatronix::Visualization::Cine ^ get() { return m_mpCine; }
    void set( Viatronix::Visualization::Cine ^ mpCine );
  } // Cine
  
  /// gets the Momentum 
  property Viatronix::Visualization::Momentum ^ Momentum
  {
    Viatronix::Visualization::Momentum ^ get() { return m_mpMomentum; }
    void set( Viatronix::Visualization::Momentum ^ mpMomentum );
  } // Momentum
  
  /// gets the CameraFlySettings 
  property Viatronix::Visualization::CameraFlySettings ^ CameraFlySettings
  {
    Viatronix::Visualization::CameraFlySettings ^ get() { return m_mpCameraFlySettings; }
    void set( Viatronix::Visualization::CameraFlySettings ^ mpCameraFlySettings );
  } // CameraFlySettings

  /// gets the associated dataset
  property Viatronix::Visualization::Dataset ^ AssociatedDataset
  {
    Viatronix::Visualization::Dataset ^ get() { return m_mpDataset; }
    void set( Viatronix::Visualization::Dataset ^ mpDataset );
  } // AssociatedDataset

  /// gets the Cad Findings
  property Viatronix::Visualization::CadFindings ^ CadFindings
  {
    Viatronix::Visualization::CadFindings ^ get() { return m_mpCadFindings; }
    void set( Viatronix::Visualization::CadFindings ^ mpCadFindings );
  } // CadFindings

  /// gets the Cad Findings
  property Viatronix::Visualization::Graph::VisualizationGraph ^ VisualizationGraph
  {
    Viatronix::Visualization::Graph::VisualizationGraph ^ get() { return m_mpVisualizationGraph; }
    void set( Viatronix::Visualization::Graph::VisualizationGraph ^ mpVisualizationGraph );
  } // VisualizationGraph
  
  /// gets the ignore modidified events flag
  property bool IgnoreModifiedEvents
  {
    bool get() { return m_pEnvironment->GetIgnoreModifiedEvents(); }
    void set( bool bIgnoreModifiedEvents ) { m_pEnvironment->SetIgnoreModifiedEvents( bIgnoreModifiedEvents ); }
  } // IgnoreModifiedEvents

  /// gets the background color 
  property System::Drawing::Color BackgroundColor
  { 
    System::Drawing::Color get();
    void set( System::Drawing::Color color );
  } // BackgroundColor
  
  /// gets the shared font info object
  static property Viatronix::Visualization::FontInfo ^ FontInfo
  {
    Viatronix::Visualization::FontInfo ^ get() { return m_mpFontInfo; }
  } // FontInfo

  /// gets or sets the selected environment
  static property Viatronix::Visualization::Environment ^ SelectedEnvironment
  {
    Viatronix::Visualization::Environment ^ get() { return m_mpSelectedEnvironment; }
    void set( Viatronix::Visualization::Environment ^ mpEnvironment );
  } // SelectedEnvironment

// member variables
public:


  // George( 8-21-2003 ): This is a hack for the animations( i promise i will fix it soon! )   
  //       ( 9-25-2003 ): Still haven't fixed it :(
  //       ( 6-30-2004 ): Just checking in ;-)
  //       ( 1-27-2006 ): forgot what this does anymore
  static bool m_bEnableEvents = true;

private:

  /// viewer type
  Viatronix::Visualization::IViewerType ^ m_mpViewerType;

  /// intensity volume 
  Viatronix::Visualization::Volume ^ m_mpVolume;

  /// label volume 
  Viatronix::Visualization::Volume ^ m_mpLabelVolume;

  /// components
  Viatronix::Visualization::Components ^ m_mpComponents;
  
  /// plane ortho
  Viatronix::Visualization::PlaneOrtho ^ m_mpPlaneOrtho;

  /// plane oblique
  Viatronix::Visualization::PlaneOblique ^ m_mpPlaneOblique;

  /// plane curved
  Viatronix::Visualization::PlaneCurved ^ m_mpPlaneCurved;

  /// rendering mode 
  Viatronix::Visualization::RenderingMode ^ m_mpRenderingMode;
    
  /// colormap 
  Viatronix::Visualization::Colormap ^ m_mpColormap;

  /// camera 
  Viatronix::Visualization::Camera ^ m_mpCamera;

  /// crop box
  Viatronix::Visualization::CropBox ^ m_mpCropBox;

  /// contour 
  Viatronix::Visualization::ContourList ^ m_mpContourList;

  /// cursor 3d 
  Viatronix::Visualization::Cursor3d ^ m_mpCursor3d;
  
  /// annotations  
  Viatronix::Visualization::Annotations ^ m_mpAnnotations;

  /// segmentation 
  Viatronix::Visualization::Segmentation ^ m_mpSegmentation;

  /// preset 
  Viatronix::Visualization::Preset ^ m_mpPreset;

  /// dataset 
  Viatronix::Visualization::Dataset ^ m_mpDataset;

  /// RendererCpu3dData
  Viatronix::Visualization::DataRendererSoftware ^ m_mpDataRendererSoftware;

  /// zoom factor
  Viatronix::Visualization::ZoomFactor ^ m_mpZoomFactor;

  /// cine 
  Viatronix::Visualization::Cine ^ m_mpCine;

  /// momentum 
  Viatronix::Visualization::Momentum ^ m_mpMomentum;

  /// camera fly settings 
  Viatronix::Visualization::CameraFlySettings ^ m_mpCameraFlySettings;

  /// cad findings
  Viatronix::Visualization::CadFindings ^ m_mpCadFindings;

  /// visualization graph
  Viatronix::Visualization::Graph::VisualizationGraph ^ m_mpVisualizationGraph;

  /// vxEnvironment
  vxShareableObject<vxEnvironment> * m_pEnvironment; 

  /// current voxel position
  vxShareableObject<Vector3D<int4> > * m_pVoxelPosition;

  /// sculpt lasso
  vxShareableObject<vxContour> * m_pSculptLasso;

  /// overlay strings
  vxShareableObject<vxOverlayStrings> * m_pOverlayStrings;

  /// collection of IElement and indices into the collection
  typedef std::map< std::string, gcroot< IElement ^ > > Elements;
  Elements * m_elements;

  /// default viewer type
  vxViewerType * m_pDefaultViewerType;

  /// background 3D color
  vxShareableObject<PixelRGBA<uint1> > * m_pBackgroundColor3D;

  /// background 2D color
  vxShareableObject<PixelRGBA<uint1> > * m_pBackgroundColor2D;

  /// background default color
  static vxShareableObject<PixelRGBA<uint1> > * m_pBackgroundColorDefault = new vxShareableObject< PixelRGBA<uint1> >();
  
  /// shared font info
  static Viatronix::Visualization::FontInfo ^ m_mpFontInfo = gcnew Viatronix::Visualization::FontInfo();

  /// selected environment
  static Viatronix::Visualization::Environment ^ m_mpSelectedEnvironment = nullptr;

  /// observer
  vxManagedObserver * m_pObserver;

  /// raised when the environment changes
  System::EventHandler ^ m_mpChangedEvent;

}; //class Environment;


CLOSE_VISUALIZATION_NS


// Revision History:
// $Log: v3D_Environment.h,v $
// Revision 1.28.2.1  2009/07/29 19:54:06  kchalupa
// Code Walkthrough
//
// Revision 1.28  2008/01/18 21:29:27  geconomos
// Issue# 5869: Images updating in background while making a movie
//
// Revision 1.27  2007/10/03 20:09:15  gdavidson
// Added SelectedEnvironment property
//
// Revision 1.26  2007/09/19 14:35:54  gdavidson
// Modified the AssociatedDataset property to update the vxEnvironment
//
// Revision 1.25  2007/05/03 21:28:44  gdavidson
// Added VisualizationGraph
//
// Revision 1.24  2007/04/11 20:58:01  romy
// added CadFindings
//
// Revision 1.23  2007/04/06 14:11:36  gdavidson
// Added CadFindings
//
// Revision 1.22  2007/01/16 17:10:54  gdavidson
// Added CameraFlySettings, Cine and Momentum
//
// Revision 1.21  2006/10/24 12:34:14  geconomos
// removed old slice shadows mechanism
//
// Revision 1.20  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.19  2006/09/27 17:23:48  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.18  2006/08/31 14:01:53  geconomos
// Modified SwapInDataset to take Dataset instance as argument
//
// Revision 1.17  2006/08/07 19:44:26  gdavidson
// included v3D_Elements.h
//
// Revision 1.16  2006/04/25 18:47:13  geconomos
// + renamed m_pObserver -> m_pObserver
// + removed unmanged vxSliceShadow instance
// + proper event handling of managed shadow object
//
// Revision 1.15  2006/03/27 19:45:21  gdavidson
// Added SliceShadow forward declaration
//
// Revision 1.14  2006/03/02 15:50:37  frank
// added composite undo actions
//
// Revision 1.13  2006/01/27 15:47:47  geconomos
// still migrating to visual studio 2005
//
// Revision 1.12  2005/11/18 21:12:28  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.11.6.1  2005/08/12 17:20:58  geconomos
// Implmentation of vxMomentum environment element
//
// Revision 1.11  2004/10/29 12:15:13  frank
// moved plane sychronization into a controller
//
// Revision 1.10  2004/10/04 15:13:15  frank
// improved the extended elements modified interface
//
// Revision 1.9  2004/08/30 18:19:38  michael
// Data used by renderers is now polymorph but so far only on the vx side of things.
// Need to add the inheritance on the managed V3D side, too. Still to be done!!!
//
// Revision 1.8  2004/07/09 18:37:53  geconomos
// Added comments.
//
// Revision 1.7  2004/05/13 18:40:26  frank
// un-shared the background color
//
// Revision 1.6  2004/04/27 14:08:15  michael
// removed dangling pointers to unmanaged zoom factor
//
// Revision 1.5  2004/04/26 19:02:50  michael
// made vxPreset/V3D Preset a first class citizen of the environment
//
// Revision 1.4  2004/04/06 16:31:24  frank
// unified environment access naming
//
// Revision 1.3  2004/03/12 19:54:49  frank
// removed annotation ROI and readjusted sculpting to remove handle set
//
// Revision 1.2  2004/03/12 18:07:51  frank
// working on removing annotation ROI - first renaming contour to contour list
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.73  2004/03/02 01:10:45  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.72  2004/02/20 23:33:43  michael
// added new zoom factor class
//
// Revision 1.71.2.1  2004/02/24 20:14:36  michael
// coding standard: assign NULL after deletion and m_pOberserver --> m_pObserver
//
// Revision 1.71  2003/12/17 01:01:28  geconomos
// Added ResetEvents method to clear out all handlers in multicast delegate "Changed"
//
// Revision 1.70  2003/12/16 13:40:25  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.69  2003/10/02 10:26:54  geconomos
// Added a static instance of FontInfo class to environment and added to observer list.
//
// Revision 1.68  2003/09/29 19:27:14  michael
// unmanaged vxShareableObject<vxRendererCpu3dData> ==> managed RendererCpu3dData
//
// Revision 1.67  2003/09/29 17:29:28  michael
// removed redundant code (was commented out anyway)
//
// Revision 1.66  2003/09/26 10:55:01  geconomos
// Updated some comments.
//
// Revision 1.65  2003/09/25 12:51:01  michael
// fixed typo
//
// Revision 1.64  2003/09/22 15:57:44  michael
// added event timer
//
// Revision 1.63  2003/08/22 03:44:04  geconomos
// Added temporary hack for animations( will remove in the next couple of days )
//
// Revision 1.62  2003/08/20 14:34:29  michael
// added "mailto:"
//
// Revision 1.61  2003/06/18 15:19:46  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.60  2003/06/03 15:18:23  geconomos
// Added OverlayStrings as a default element in the initializer of v3D_Environment.
//
// Revision 1.59  2003/05/16 13:10:13  frank
// code formatting
//
// Revision 1.58  2003/05/13 13:28:44  michael
// code review
//
// Revision 1.57  2003/05/12 18:16:51  frank
// Added slice shadows object
//
// Revision 1.56  2003/05/06 17:01:11  geconomos
// Coding conventions.
//
// Revision 1.55  2003/05/06 12:20:38  geconomos
// Issue # 3077: Added separate background color for undefined viewer types.
//
// Revision 1.54  2003/04/29 21:19:03  michael
// renamed RenderQualitySpeedTradeoff to vxRendererCpu3dData
//
// Revision 1.53  2003/04/24 14:03:28  michael
// code review
//
// Revision 1.52  2003/04/09 15:57:01  geconomos
// Issue# 3077: Maintaining background colors for 2D and 3D.
//
// Revision 1.51  2003/04/07 15:52:36  michael
// transition from BackColor to BackgroundColor
//
// Revision 1.50  2003/04/01 17:46:04  geconomos
// Coding convention for event naming.
//
// Revision 1.49  2003/03/30 21:00:30  michael
// added viewertype to environment
//
// Revision 1.48  2003/03/20 13:43:23  geconomos
// Removed property ViewerType.
//
// Revision 1.47  2003/03/03 17:18:29  geconomos
// Initial ViewerType implementation
//
// Revision 1.46  2003/01/28 19:31:36  geconomos
// Implementation of BackgroundColor.
//
// Revision 1.45  2003/01/06 21:15:04  ingmar
// removed unused static member variable
//
// Revision 1.44  2002/12/18 15:47:00  geconomos
// Added IViewerType property.
//
// Revision 1.43  2002/11/25 21:17:02  geconomos
// Increased speed while switching layouts.
//
// Revision 1.42  2002/11/19 21:44:07  michael
// added camera initialization code for oblique view ... still to be centralized ...
//
// Revision 1.41  2002/11/18 20:39:58  geconomos
// Added IgnoreModifiedEvents property.
//
// Revision 1.40  2002/11/18 20:23:23  michael
// added plane oblique
//
// Revision 1.39  2002/11/18 16:36:56  michael
// completed plane object renaming
//
// Revision 1.38  2002/11/15 18:04:43  geconomos
// Added AssociatedPreset field and property
//
// Revision 1.37  2002/11/13 22:07:20  frank
// Renamed vxPlaneOblique to vxViewObliqueData,
// Demoted the data to a second class citizen in the environment.
//
// Revision 1.36  2002/11/13 19:26:04  frank
// enabled oblique view renderer and manipulator
//
// Revision 1.35  2002/11/12 21:31:07  geconomos
// All standard Environment elements now implement IElement.
//
// Revision 1.34  2002/11/08 23:58:24  frank
// Added a contour element to the environment.
//
// Revision 1.33  2002/11/08 14:49:19  geconomos
// Added momentum property.
//
// Revision 1.32  2002/11/05 22:46:26  ingmar
// 3DCursor -> Cursor3d
//
// Revision 1.31  2002/11/05 20:52:56  geconomos
// Clean up of modified method.
//
// Revision 1.30  2002/11/04 14:30:36  geconomos
// new toolbar class.
//
// Revision 1.29  2002/11/02 03:11:37  frank
// Moved crop box into vxEnvironment.
//
// Revision 1.28  2002/10/31 12:51:08  geconomos
// Chaged GetEnvironmentPtr() to return a System::IntPtr for interoperability.
//
// Revision 1.27  2002/10/21 22:25:24  frank
// Added PlaneCurvilinear
//
// Revision 1.26  2002/10/19 20:53:06  michael
// cosmetics
//
// Revision 1.25  2002/10/18 17:22:49  geconomos
// Fixed spelling of segmentation property.
//
// Revision 1.24  2002/10/17 15:29:09  geconomos
// Added Preset property.
//
// Revision 1.23  2002/10/07 15:20:33  geconomos
// Merged Volume_Int2 and Volume_uint2 into Volume.
//
// Revision 1.22  2002/10/04 13:31:35  geconomos
// Added segmentation class.
//
// Revision 1.21  2002/09/26 19:04:27  dmitry
// RemoveAllElements implemented.
//
// Revision 1.20  2002/09/26 18:12:27  dmitry
// ColormapPresets property added.
//
// Revision 1.19  2002/09/24 17:36:31  geconomos
// Exception handling and formatting.
//
// Revision 1.18  2002/09/23 15:06:27  michael
// removed last instance of transfer function
//
// Revision 1.17  2002/09/19 15:47:02  dmitry
// gcroot used for elements
//
// Revision 1.16  2002/09/09 18:51:00  geconomos
// Implemented disposable pattern.
//
// Revision 1.15  2002/08/09 16:20:41  dmitry
// IElement management implemented and GetElement method added.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Environment.h,v 1.28.2.1 2009/07/29 19:54:06 kchalupa Exp $
// $Id: v3D_Environment.h,v 1.28.2.1 2009/07/29 19:54:06 kchalupa Exp $
