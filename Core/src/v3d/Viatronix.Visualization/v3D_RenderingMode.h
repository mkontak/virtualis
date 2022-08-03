// $Id: v3D_RenderingMode.h,v 1.15 2007/08/21 19:40:34 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

// pragmas
#pragma once


// includes
#include "v3D_Interfaces.h"
#include "v3D_RenderingModeAttribute.h"
#include "vxRenderingMode.h"
#include "vxShareableObject.h"


// forward declarations
class vxManagedObserver;


// namespaces
OPEN_VISUALIZATION_NS


// class definition
public ref class RenderingMode : 
  public IElement, 
  public IObserver,
  public System::ICloneable
{
// enums
public:

  /// rendering quality
  enum class RenderingQualityEnum
  {
    DEFAULT         = vxRenderingMode::QUALITY_DEFAULT,
    ENDOLUMINAL     = vxRenderingMode::QUALITY_ENDOLUMINAL,
    HIGHEST         = vxRenderingMode::QUALITY_HIGHEST,
  }; // enum RenderingQualityEnum

// events
public:

  // raised when the rendering mode object has been modified
  event System::EventHandler ^ Changed;

// IElement implementation
public:

  /// gets a pointer to the extended element
  virtual System::IntPtr GetElementPtr() { return System::IntPtr( m_pRenderingMode ); }

  /// connects an observer to the object
  virtual void Connect( System::IntPtr observer);

  /// disconnects an observer from the object
  virtual void Disconnect( System::IntPtr observer );

  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;

// IObserver implementation
public:
  
  /// implements the IObserver interface
  virtual void OnModified( System::IntPtr modifiedList );

// ICloneable
public:

  /// creates a new object that is a copy of the current instance
  virtual System::Object ^ Clone();

// Serialization
public:

  /// serializes the rendering mode object to xml
  System::String ^ ToXml();

  /// creates a new instance from the specified xml string
  static RenderingMode ^ FromXml( System::String ^ mpXml );

  /// creates a new instance from the specified xml resource
  static RenderingMode ^ FromResource( System::String ^ mpXmlResource );

  /// creates a new instance from the specified xml file
  static RenderingMode ^ FromFile( System::String ^ mpXmlFile );

// functions
public:
  
  /// initializer
  RenderingMode();
  
  /// destructor
  virtual ~RenderingMode();

  /// finalizer
  !RenderingMode();
  
  // resets the rendering mode object to a default state
  void Reset();

  /// Save current state of the vizualization preset for later undo.
  void SetRestorePoint();

  ///Starts the Composite Undo
  void StartCompositeUndo();
  
  ///Ends the Composite Undo
  void EndCompositeUndo();

  /// gets the current rendering mode
  property Viatronix::Visualization::RenderingAlgorithm Mode
  {
    Viatronix::Visualization::RenderingAlgorithm get();
    void set( Viatronix::Visualization::RenderingAlgorithm eAlgorithm );
  }

  /// gets the slice shadow mode
  property Viatronix::Visualization::SliceShadowMode SliceShadowMode
  {
    Viatronix::Visualization::SliceShadowMode get();
    void set( Viatronix::Visualization::SliceShadowMode eSliceShadowMode );
  }

  /// gets the patient data visible option
  [ RenderingModeAttribute( "Patient Info", Viatronix::Visualization::RenderingOptions::ShowPatientInfo ) ]
  property bool PatientDataVisible
  {
    bool get();
    void set( bool bVisible );
  }

  /// gets the image data visible option
  [ RenderingModeAttribute( "Image Info", Viatronix::Visualization::RenderingOptions::ShowImageInfo ) ]
  property bool ImageDataVisible
  {
    bool get();
    void set( bool bVisible );
  }
 
  /// gets the show cuts shaded
  [ RenderingModeAttribute( "Show Cuts Shaded", Viatronix::Visualization::RenderingOptions::ShowCutsShaded ) ]
  property bool ShowCutsShaded
  {
    bool get();
    void set( bool bShaded );
  }

  /// gets the show cuts grayscale
  [ RenderingModeAttribute( "Show Cuts Grayscale", Viatronix::Visualization::RenderingOptions::ShowCutsGrayscale ) ]
  property bool ShowCutsGrayscale
  {
    bool get();
    void set( bool bGrayscale );
  }

  /// gets the crop box visible option
  [ RenderingModeAttribute( "Crop Box", Viatronix::Visualization::RenderingOptions::ShowCropBox ) ]
  property bool CropBoxVisible
  {
    bool get();
    void set( bool bVisible );
  }

  /// gets the vessel centerline visible option
  [ RenderingModeAttribute( "Centerline Path", Viatronix::Visualization::RenderingOptions::ShowCenterlinePath ) ]
  property bool ShowVesselCenterline
  {
    bool get();
    void set( bool bVisible );
  }
  
  /// gets the label visible option
  [ RenderingModeAttribute( "Components (MPR)", Viatronix::Visualization::RenderingOptions::ShowComponentsInMPR ) ]
  property bool LabelVisible
  {
    bool get();
    void set( bool bVisible );
  }

  /// gets the label visible option
  [ RenderingModeAttribute( "Annotations (MPR)", Viatronix::Visualization::RenderingOptions::ShowAnnotations ) ]
  property bool AnnotationsVisible
  {
    bool get();
    void set( bool bVisible );
  }

  /// gets the color im mpr option
  [ RenderingModeAttribute( "Color (MPR)", Viatronix::Visualization::RenderingOptions::ShowColorInMPR ) ]
  property bool ColorUsedInMPR
  {
    bool get();
    void set( bool bVisible );
  }

  /// gets the ShowCursor3D option
  [ RenderingModeAttribute( "3D Cursor (MPR)", Viatronix::Visualization::RenderingOptions::ShowCursor3DInMPR ) ]
  property bool ShowCursor3D
  {
    bool get();
    void set( bool bVisible );
  }

  /// gets the intensity range visible option
  [ RenderingModeAttribute( "Intensity", Viatronix::Visualization::RenderingOptions::ShowIntensityRange ) ]
  property bool IntensityRangeVisible
  {
    bool get();
    void set( bool bVisible );
  }
  
  /// gets the label on top option
  [ RenderingModeAttribute( "Component On Top", Viatronix::Visualization::RenderingOptions::ShowComponentsOnTop ) ]
  property bool LabelOnTop
  {
    bool get();
    void set( bool bVisible );
  }
  
  /// gets the box of interest visible option
  [ RenderingModeAttribute( "Box of Interest", Viatronix::Visualization::RenderingOptions::ShowBoxOfInterest ) ]
  property bool ShowBoxOfInterest
  {
    bool get();
    void set( bool bVisible );
  }

  /// gets if shading is enabled
  [ RenderingModeAttribute( "Shading", Viatronix::Visualization::RenderingOptions::ShowShading ) ]
  property bool DisplayShading
  {
    bool get();
    void set( bool bDisplayShading );
  }

  /// gets the slab thickness
  property float SlabThickness
  {
    float get();
    void set( float fThickness );
  }

  /// Set rendering quality level
  property RenderingQualityEnum RenderingQuality
  {
    RenderingQualityEnum get() { return static_cast< RenderingQualityEnum >( m_pRenderingMode->GetRenderingQuality() ); }
    void set( RenderingQualityEnum quality );
  }

  /// get quality level scale factor
  property float QualityScaleFactor
  { 
    float get() { return m_pRenderingMode->GetQualityScaleFactor(); }
    void set( float fFactor ) { m_pRenderingMode->SetQualityScaleFactor(fFactor); }
  }

// data
private:

  // vxRenderingMode pointer
  vxShareableObject<vxRenderingMode> * m_pRenderingMode;

  // observer
  vxManagedObserver * m_pObserver;

}; // RenderingMode


CLOSE_VISUALIZATION_NS


// Revision history:
// $Log: v3D_RenderingMode.h,v $
// Revision 1.15  2007/08/21 19:40:34  gdavidson
// Added property to show or hide annotations
//
// Revision 1.14  2007/01/22 15:34:44  geconomos
// Issue# 5321: Slice shadows mode reworked
//
// Revision 1.13  2006/12/21 15:12:05  gdavidson
// Issue #5189 & 5106: Moved RenderingQuality functionality to vxRenderingMode
//
// Revision 1.12  2006/12/11 16:28:20  romy
// Added CompositeUndo for rendering Mode
//
// Revision 1.11  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.10  2006/10/03 20:14:46  geconomos
// added SliceShadowMode enumeration to replace old bit flag
//
// Revision 1.9  2006/09/27 17:23:48  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.8  2006/04/27 19:16:01  romy
// UnDoAction Reset method
//
// Revision 1.7  2005/11/18 21:12:28  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.6.2.3  2005/08/04 18:57:09  frank
// removed unused cursor 3D tracking
//
// Revision 1.6.2.2  2005/07/08 15:33:51  frank
// added rendering option for unshaded and uncolored edge, crop, slice cuts
//
// Revision 1.6.2.1  2005/07/05 17:53:02  frank
// spelling
//
// Revision 1.6  2005/03/08 19:09:47  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.5.2.1  2005/02/17 20:59:18  geconomos
// implemented ICloneable
//
// Revision 1.5  2004/10/18 14:50:55  frank
// moved rendering mode data out of software 3D data renderer
//
// Revision 1.4  2004/10/05 14:42:00  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.3  2004/10/04 15:10:59  frank
// improved the extended elements modified interface
//
// Revision 1.2  2004/04/26 19:09:15  michael
// Adjusted code to reflect the changes in zomm factor, roi not being part of
// camera anymore, etc. and moved serialization of preset from V3D Preset to
// use the one in vxSerializer (removed duplicate code and fixed errors)
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.35  2003/12/16 13:27:00  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.34  2003/11/19 14:15:40  frank
// Added initializer from other managed object
//
// Revision 1.33  2003/11/18 15:30:08  frank
// Updated text of display settings
//
// Revision 1.32  2003/11/11 12:35:23  geconomos
// Wrapped vxRenderingMode's Reset method.
//
// Revision 1.31  2003/11/06 20:44:03  michael
// Fixed rendering mode related issues, renamed the "algorithm" to be the mode,
// and the display options to be the options so that things are consistent. Also
// fixed the progressive refinement that broke because George forgot to add
// the assignment of the mode in vxRenderingMode::operator=()
//
// Revision 1.30  2003/11/05 01:18:45  geconomos
// Updated to use new vxRenderingAlgorithmEnum
//
// Revision 1.29  2003/10/15 01:28:05  geconomos
// Added RenderingModeAttribute to all properties.
//
// Revision 1.28  2003/09/03 17:16:58  frank
// Added slice shadow clipping mode
//
// Revision 1.27  2003/08/20 14:31:12  michael
// added "mailto:"
//
// Revision 1.26  2003/08/13 20:03:06  frank
// Added vessel centerline display option
//
// Revision 1.25  2003/06/03 12:25:07  michael
// added Angio (XA) presets for Shimadzu that are sensitive to a specific DICOM entry
//
// Revision 1.24  2003/05/23 20:13:20  michael
// added image information display flag
//
// Revision 1.23  2003/05/16 13:13:56  frank
// formatting
//
// Revision 1.22  2003/05/13 13:28:46  michael
// code review
//
// Revision 1.21  2003/05/12 18:42:09  geconomos
// Coding standards.
//
// Revision 1.20  2003/05/12 18:40:41  geconomos
// Coding standards.
//
// Revision 1.19  2003/04/01 18:01:38  geconomos
// Coding convention for event naming.
//
// Revision 1.18  2003/03/07 20:35:10  geconomos
// Added ShowBoxOfInterest property.
//
// Revision 1.17  2003/02/14 19:22:54  frank
// Fixed interface to access rendering mode encapsulated object
//
// Revision 1.16  2002/12/20 16:12:17  frank
// Added x-ray projection
//
// Revision 1.15  2002/12/06 19:21:58  ingmar
// added color used in MPR
//
// Revision 1.14  2002/11/14 14:51:18  geconomos
// added ShowCursor3D
//
// Revision 1.13  2002/11/12 23:55:48  michael
// added constructor from vxRenderingMode and inital To/FromXml()
//
// Revision 1.12  2002/11/12 21:31:01  geconomos
// All standard Environment elements now implement IElement.
//
// Revision 1.11  2002/11/11 19:19:07  geconomos
// Reworked serialization.
//
// Revision 1.10  2002/11/07 17:23:55  geconomos
// Added TrackCursor3D property.
//
// Revision 1.9  2002/11/02 03:10:58  frank
// spelling
//
// Revision 1.8  2002/11/01 19:56:30  frank
// Added Crop Box display mode.
//
// Revision 1.7  2002/10/03 19:12:54  dmitry
// Modified event called.
//
// Revision 1.6  2002/09/24 18:17:55  geconomos
// Exception handling and formatting.
//
// Revision 1.5  2002/08/16 04:32:10  frank
// Added LABEL_TOP_TOP mode.
//
// Revision 1.4  2002/07/16 23:23:26  geconomos
// no message
//
// Revision 1.3  2002/07/10 23:15:56  dmitry
// Derived from IRenderingMode.
//
// Revision 1.2  2002/07/09 20:14:24  dmitry
// implementation added
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_RenderingMode.h,v 1.15 2007/08/21 19:40:34 gdavidson Exp $
// $Id: v3D_RenderingMode.h,v 1.15 2007/08/21 19:40:34 gdavidson Exp $