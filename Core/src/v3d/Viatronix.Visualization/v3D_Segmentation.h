// $Id: v3D_Segmentation.h,v 1.7 2006/10/04 18:29:05 gdavidson Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

// pragmas
#pragma once

// includes
#include "v3D_Volume.h"
#include "v3D_Components.h"
#include "InteractiveSegmentation.h"
#include "vxEnvironment.h"

// forward declarations
class vxManagedObserver;

// namespaces
OPEN_VISUALIZATION_NS
ref class Environment;

// class declaration
public ref class Segmentation : public IElement, public IObserver
{
// events
public:

  // raised when the Segmentation object has changed
  event System::EventHandler ^ Changed;

  // raised when the Segmentation threshold has changed
  event System::EventHandler ^ ThresholdChanged;

// IElement implementation
public:

  /// gets a pointer to the extended element
  virtual System::IntPtr GetElementPtr() { return System::IntPtr( m_pSegmentation ); }

  /// connects an observer to the object
  virtual void Connect( System::IntPtr observer );

  /// disconnects an observer from the object
  virtual void Disconnect( System::IntPtr observer );

  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;

// IObserver implementation
public:
  
  /// processes modified queue events.
  virtual void OnModified( System::IntPtr modifiedList );

// Serialization
public:

  /// serializes the segmentation object to xml
  System::String ^ ToXml();

  /// creates a new object from the specified xml string
  static Segmentation ^ FromXml( System::String ^ mpXml );

  /// creates a new segmentation from the specified xml resource
  static Segmentation ^ FromResource( System::String ^ mpXmlResource );

  /// creates a new segmentation from the specified xml file
  static Segmentation ^ FromFile( System::String ^ mpXmlFile );

// functions
public:
  
  /// initializer
  Segmentation();
  
  /// destructor
  virtual ~Segmentation();

  /// finalizer
  !Segmentation();

  /// runs the specified operation
  void Run( Viatronix::Visualization::SegmentationModes eMode, long iOption, bool bRestrictedSubset,
            Viatronix::Visualization::Environment ^ mpEnvironment);

  /// Threshold segments the entire volume
  void ThresholdSegment( Viatronix::Visualization::Environment ^ mpEnvironment, bool bRestrictedSubset );

  /// grow a component
  bool GrowComponent( Viatronix::Visualization::Environment ^ mpEnvironment, bool bRestrictedSubset );

  /// shrink a component
  bool ShrinkComponent( Viatronix::Visualization::Environment ^ mpEnvironment, bool bRestrictedSubset );

  /// sets a restore point
  void SetRestorePoint();

  /// gets the intensity volume
  property Viatronix::Visualization::Volume ^ IntensityVolume
  {
    Viatronix::Visualization::Volume ^ get();
    void set( Viatronix::Visualization::Volume ^ mpVolume );
  }
    
  /// gets the label volume
  property Viatronix::Visualization::Volume ^ LabelVolume
  {
    Viatronix::Visualization::Volume ^ get();
    void set( Viatronix::Visualization::Volume ^ mpLabelVolume );
  }
  
  /// gets the components
  property Viatronix::Visualization::Components ^ Components
  {
    Viatronix::Visualization::Components ^ get();
    void set( Viatronix::Visualization::Components ^ mpComponents );
  }

  /// gets region threshold - high
  property long RegionThresholdHigh
  {
    long get();
    void set( long iThresholdHigh );
  }

  /// gets region threshold - low
  property long RegionThresholdLow
  {
    long get();
    void set( long iThresholdLow );
  }

  /// gets the region grow method
  property Viatronix::Visualization::RegionGrowMethods RegionGrowMethod
  {
    Viatronix::Visualization::RegionGrowMethods get();
    void set( Viatronix::Visualization::RegionGrowMethods eMethod );
  }
   
  /// gets the region grow add option
  property Viatronix::Visualization::RegionGrowAddOptions RegionGrowAddOpt
  {
    Viatronix::Visualization::RegionGrowAddOptions get();
    void set( Viatronix::Visualization::RegionGrowAddOptions eOption ); 
  }
   
  /// sets the component color for the next segmented object
  property System::Drawing::Color NextComponentColor
  {
    void set( System::Drawing::Color color );
  }

  /// sets the component name for the next segmented object
  property System::String ^ NextComponentName
  {
    void set( System::String ^ mpName );
  }

private:

  /// shared segmentation object
  vxShareableObject< InteractiveSegmentation > * m_pSegmentation;
  
  /// atached volume
  Viatronix::Visualization::Volume ^ m_mpIntensityVolume;
  
  /// label volume
  Viatronix::Visualization::Volume ^ m_mpLabelVolume;
  
  /// attached components
  Viatronix::Visualization::Components ^ m_mpComponents;

  /// observer
  vxManagedObserver * m_pObserver;

}; // class Segmentation

CLOSE_VISUALIZATION_NS

// $Log: v3D_Segmentation.h,v $
// Revision 1.7  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.6  2006/09/27 17:23:49  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.5  2006/03/02 15:51:28  frank
// cleaned up grow and shrink operations
//
// Revision 1.4  2006/02/17 15:18:36  frank
// rendering mode is no longer necessary for the segmentation object
//
// Revision 1.3  2005/11/18 21:12:28  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.2.6.2  2005/07/11 18:11:46  frank
// cleaned up naming and color of components
//
// Revision 1.2.6.1  2005/07/05 17:55:37  frank
// added names and colors to the segmentation presets
//
// Revision 1.2  2004/10/04 15:10:59  frank
// improved the extended elements modified interface
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.13  2003/12/22 12:55:39  frank
// Issue #3593: Restricted segmentation to the visible volume on the shift key
//
// Revision 1.12  2003/12/16 13:27:00  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.11  2003/08/20 14:31:12  michael
// added "mailto:"
//
// Revision 1.10  2003/07/30 15:41:00  geconomos
// Added RenderingMode property to Segmentation object.
//
// Revision 1.9  2003/05/13 13:28:46  michael
// code review
//
// Revision 1.8  2003/05/12 18:58:04  geconomos
// Coding standards.
//
// Revision 1.7  2003/04/01 18:01:38  geconomos
// Coding convention for event naming.
//
// Revision 1.6  2003/03/25 21:42:37  frank
// Access to InteractiveSegmentation object is now through a System::IntPtr
//
// Revision 1.5  2002/11/12 21:31:01  geconomos
// All standard Environment elements now implement IElement.
//
// Revision 1.4  2002/11/11 19:19:07  geconomos
// Reworked serialization.
//
// Revision 1.3  2002/10/14 20:04:27  geconomos
// Added missing functionailty.
//
// Revision 1.2  2002/10/07 15:20:33  geconomos
// Merged Volume_Int2 and Volume_uint2 into Volume.
//
// Revision 1.1  2002/10/04 12:56:42  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Segmentation.h,v 1.7 2006/10/04 18:29:05 gdavidson Exp $
// $Id: v3D_Segmentation.h,v 1.7 2006/10/04 18:29:05 gdavidson Exp $
