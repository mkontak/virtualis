// $Id: v3D_VisualizationPreset.h,v 1.8 2007/06/06 18:23:02 romy Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (mailto:frank@viatronix.com)


// pragmas
#pragma once


// includes
#include "v3D_Interfaces.h"
#include "v3D_Colormap.h"
#include "v3D_RenderingMode.h"
#include "vxVisualizationPreset.h"
#include "vxShareableObject.h"


// forward declarations
class vxManagedObserver;


// namespaces
OPEN_VISUALIZATION_NS


public ref class VisualizationPreset :
  public IElement, 
  public IObserver,
  public System::ICloneable
{
// events
public:

  // fired when an annotation is modified
  event System::EventHandler ^ Changed;

// IElement implementation
public:

  /// returns a pointer to the extended element
  virtual System::IntPtr GetElementPtr() { return System::IntPtr( m_pVisualizationPreset ); }

  /// connects an observer to the object
  virtual void Connect( System::IntPtr observer );

  /// disconnects an observer from the object
  virtual void Disconnect( System::IntPtr observer );

  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;

// IObserver implementation
public:

  /// processes modified queue events
  virtual void OnModified( System::IntPtr modifiedList );
  
// ICloneable implementation
public:

  /// creates a new object that is a copy of the current instance.
  virtual System::Object ^ Clone();

// Serialization
public:

  /// serializes the VisualizationPreset object to xml
  System::String ^ ToXml();

  /// creates a new VisualizationPreset instance from the specified xml string
  static VisualizationPreset ^ FromXml( System::String ^ mpXml );

  /// creates a new VisualizationPreset instance from the specified xml resource
  static VisualizationPreset ^ FromResource( System::String ^ mpXmlResource );

  /// creates a new VisualizationPreset instance from the specified xml file
  static VisualizationPreset ^ FromFile( System::String ^ mpXmlFile );

// methods
public:
  
  /// initializer
  VisualizationPreset();

  //// initializer from vxVisualizationPreset
  //VisualizationPreset( vxVisualizationPreset * pVisualizationPreset );

  //// initializer from v3D VisualizationPreset
  //VisualizationPreset( VisualizationPreset ^ mpVisualizationPreset );
  
  /// destructor
  virtual ~VisualizationPreset();

  /// finalizer
  !VisualizationPreset();

  /// copies the specified Visualization Preset settings into this Visualization Preset
  void CopyFrom( VisualizationPreset ^ mpVisualizationPreset );

  /// gets the associated name
  property System::String ^ Name
  {
    System::String ^ get() { return gcnew System::String( m_pVisualizationPreset->GetName().c_str() ); }
    void set( System::String ^ mpsame );
  }


  /// gets/sets the associated PresetImage File
  property System::String ^ PresetImageFile
  {
    System::String ^ get() { return gcnew System::String( m_pVisualizationPreset->GetPresetImageFile().c_str() ); }
    void set( System::String ^ mpFile );
  }

	/// gets/sets the PresetImage show flag
	property bool ShowPresetImage
  {
		bool get() { return  m_pVisualizationPreset->IsShowPresetImage(); }
		void set( bool bShow ) { m_pVisualizationPreset->SetShowPresetImage( bShow ) ;}
  }//ShowPresetImage


  /// gets the associated colormap
  property Viatronix::Visualization::Colormap ^ Colormap
  {
    Viatronix::Visualization::Colormap ^ get();
    void set( Viatronix::Visualization::Colormap ^  mpColormap );
  }

  /// gets the associated rendering mode
  property Viatronix::Visualization::RenderingMode ^ RenderingMode
  {
    Viatronix::Visualization::RenderingMode ^ get();
    void set( Viatronix::Visualization::RenderingMode ^ mpRenderingMode );
  }

  /// gets the background color
  property System::Drawing::Color BackgroundColor
  {
    System::Drawing::Color get();
    void set( System::Drawing::Color color );
  }

// fields
private:

  /// shared visualization preset object
  vxShareableObject< vxVisualizationPreset > * m_pVisualizationPreset;
  
  /// the colormap
  Viatronix::Visualization::Colormap ^ m_mpColormap;

  /// the rendering mode
  Viatronix::Visualization::RenderingMode ^ m_mpRenderingMode;

  /// observer
  vxManagedObserver * m_pObserver;

}; // class VisualizationPreset


CLOSE_VISUALIZATION_NS


// $Log: v3D_VisualizationPreset.h,v $
// Revision 1.8  2007/06/06 18:23:02  romy
// added wrapper to ShowPresetImage
//
// Revision 1.7  2007/03/23 15:37:02  romy
// Added PresetImageFile method
//
// Revision 1.6  2006/10/04 18:29:06  gdavidson
// Ported to C++/CLI
//
// Revision 1.5  2006/09/27 17:23:49  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.4  2005/03/08 19:09:47  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.3.2.1  2005/02/17 20:59:45  geconomos
// added the ownership of colormaps and rendering modes (will revisit)
//
// Revision 1.3  2004/12/10 14:18:53  frank
// extended to include the colors
//
// Revision 1.2  2004/10/04 15:11:55  frank
// improved the extended elements modified interface
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2003/12/16 13:27:52  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.1  2003/08/05 13:35:06  frank
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_VisualizationPreset.h,v 1.8 2007/06/06 18:23:02 romy Exp $
// $Id: v3D_VisualizationPreset.h,v 1.8 2007/06/06 18:23:02 romy Exp $