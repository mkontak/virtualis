// $Id: v3D_Elements.h,v 1.14.2.1 2009/07/29 13:23:40 kchalupa Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (mailto:frank@viatronix.com)


// pragmas
#pragma once


// includes
#include "v3D_Interfaces.h"
#include "vxShareableObject.h"
#include "vxElementInt4.h"
#include "vxElementFloat4.h"
#include "vxElementBool.h"
#include "vxElementPixelRGBA.h"
#include "vxElementPoint3D.h"
#include "vxRibbon.h"
#include "vxElementVector.h"
#include "vxPhaseInfo.h"
#include "vxElementString.h"


// forward declarations
class vxManagedObserver;
class vxRibbon;


// namespaces
OPEN_VISUALIZATION_NS

/**
 * class declaration for ElementInt4
 */
public ref class ElementBool :
  public IObserver,
  public IElement
{
// events
public:

  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;

// construction
public:

  /// constructor
  ElementBool( bool bElementValue );

  /// destructor
  virtual ~ElementBool();

  /// finalize
  !ElementBool();

// member functions
public:

  /// returns a pointer to the extended element
  virtual System::IntPtr GetElementPtr();

  /// connects an observer to the object
  virtual void Connect( System::IntPtr observer );

  /// disconnects an observer from the object
  virtual void Disconnect( System::IntPtr observer );

  /// initialize
  void CommonInit( bool bElementValue );

// event handlers
public:

  /// implements the IObserver interface
  virtual void OnModified( System::IntPtr modifiedList );


// properties
public:

  /// retrieves the value of the object
  property bool Value
  {
    bool get();
    void set( bool bValue );
  } // Value

// member variables
private:

  // boolean value
  vxShareableObject< vxElementBool > * m_pBool;

  // observer
  vxManagedObserver  * m_pObserver;

}; // ElementBool


/**
 * class declaration for ElementInt4
 */
public ref class ElementInt4 :
  public IObserver,
  public IElement
{
// events
public:
  
  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;

// construction
public:

  /// constructor
  ElementInt4();

  /// constructor
  ElementInt4( int iElementValue );

  /// destructor
  virtual ~ElementInt4();

  /// finalize
  !ElementInt4();

// member functions
public:

  /// returns a pointer to the extended element
  virtual System::IntPtr GetElementPtr();

  /// connects an observer to the object
  virtual void Connect( System::IntPtr observer );

  /// disconnects an observer from the object
  virtual void Disconnect( System::IntPtr observer );

  /// initialize
  void CommonInit( int iElementValue );

// event handlers
public:

  /// implements the IObserver interface
  virtual void OnModified( System::IntPtr modifiedList );

// properties
public:

  /// retrieves the value of the object
  property int Value
  {
    int get();
    void set( int iValue );
  } // Value

// member variables
private:

  // int4 value
  vxShareableObject< vxElementInt4 > * m_pInt4;

  // observer
  vxManagedObserver * m_pObserver;

}; // ElementInt4


/**
 * class declaration for ElementFloat4 
 */
public ref class ElementFloat4 :
  public IObserver,
  public IElement
{
// events
public:

  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;

// construction
public:

  /// constructor
  ElementFloat4();

  /// constructor
  ElementFloat4( float4 fElementValue );

  /// destructor
  virtual ~ElementFloat4();

  /// finalize
  !ElementFloat4();

// member functions
public:

  /// returns a pointer to the extended element
  virtual System::IntPtr GetElementPtr();

  /// connects an observer to the object
  virtual void Connect( System::IntPtr observer );

  /// disconnects an observer from the object
  virtual void Disconnect( System::IntPtr observer );

  /// initialize
  void CommonInit( float4 fElementValue );

// event handlers
public:

  /// implements the IObserver interface
  virtual void OnModified( System::IntPtr modifiedList );

// properties
public:

  /// retrieves the value of the object
  property float Value
  {
    float get();
    void set( float fValue );
  } // Value

// member variables
private:

  // float4 value
  vxShareableObject< vxElementFloat4 > * m_pFloat4;

  // observer
  vxManagedObserver  * m_pObserver;
}; // ElementFloat4


/**
 * class declaration for ElementPixelRGBA
 */
public ref class ElementPixelRGBA :
  public IObserver,
  public IElement
{
// events
public:

  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;

// construction
public:

  /// constructor
  ElementPixelRGBA();

  /// constructor
  ElementPixelRGBA( System::Drawing::Color color );

  /// destructor
  virtual ~ElementPixelRGBA();

  /// finalize
  !ElementPixelRGBA();

// member functions
public:

  /// returns a pointer to the extended element
  virtual System::IntPtr GetElementPtr();

  /// connects an observer to the object
  virtual void Connect( System::IntPtr observer );

  /// disconnects an observer from the object
  virtual void Disconnect( System::IntPtr observer );

  /// initialize
  void CommonInit();

// event handlers
public:

  /// implements the IObserver interface
  virtual void OnModified( System::IntPtr modifiedList );

// properties
public:

  /// retrieves the value of the object
  property System::Drawing::Color Color
  {
    System::Drawing::Color get();
    void set( System::Drawing::Color color );
  } // Color

// member variables
private:

  // float4 value
  vxShareableObject< vxElementPixelRGBA< uint1 > > * m_pPixelRGBA;

  // observer
  vxManagedObserver  * m_pObserver;
}; // ElementFloat4


/**
 * class declaration for ElementPoint3D
 */
public ref class ElementPoint3D :
  public IObserver,
  public IElement
{
// events
public:

  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;

// functions
public:

  /// constructor
  ElementPoint3D();

  /// destructor
  virtual ~ElementPoint3D();

  /// finalize
  !ElementPoint3D();

// member functions
public:

  /// returns a pointer to the extended element
  virtual System::IntPtr GetElementPtr();

  /// connects an observer to the object
  virtual void Connect( System::IntPtr observer );

  /// disconnects an observer from the object
  virtual void Disconnect( System::IntPtr observer );

  /// initialize
  void CommonInit();

// event handlers
public:

  /// implements the IObserver interface
  virtual void OnModified( System::IntPtr modifiedList );

// data
private:

  // float4 value
  vxShareableObject< vxElementPoint3D< float4 > > * m_pPoint3D;

  // observer
  vxManagedObserver  * m_pObserver;
}; // ElementFloat4


// class declaration for ElementPoint3D
public ref class Ribbon :
  public IObserver,
  public IElement
{
public:
  
  // the possible orientations for the ribbon
  enum class RibbonOrientationEnum
  {
    ORIENTATION_X       = vxRibbon::X_AXIS,
    ORIENTATION_Y       = vxRibbon::Y_AXIS,
    ORIENTATION_Z       = vxRibbon::Z_AXIS,
    ORIENTATION_VESSEL  = vxRibbon::VESSEL,
  }; // enum RibbonOrientationEnum

// events
public:

  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;

// construction
public:

  /// constructor
  Ribbon();

  /// destructor
  virtual ~Ribbon();

  /// finalize
  !Ribbon();

// member functions
public:

  /// returns a pointer to the extended element
  virtual System::IntPtr GetElementPtr();

  /// connects an observer to the object
  virtual void Connect( System::IntPtr observer );

  /// disconnects an observer from the object
  virtual void Disconnect( System::IntPtr observer );

  /// initialize
  void CommonInit();

// properties
public:

  /// retrieves the ribbon orientation
  property RibbonOrientationEnum Orientation
  {
    RibbonOrientationEnum get();
    void set( RibbonOrientationEnum eOrientation );
  } // Orientation

// event handlers
public:

  /// implements the IObserver interface
  virtual void OnModified( System::IntPtr modifiedList );

// member variables
private:

  // vxRibbon value
  vxShareableObject< vxRibbon > * m_pRibbon;

  // observer
  vxManagedObserver  * m_pObserver;
}; // class ElementFloat4
  

/**
 * class declaration for DataRendererSoftware
 */
public ref class DataRendererSoftware :
  public IObserver,
  public IElement
{
// construction
public:

  /// constructor
  DataRendererSoftware();

  /// destructor
  virtual ~DataRendererSoftware();

  /// finalize
  !DataRendererSoftware();

// member functions
public:

  /// returns a pointer to the extended element
  virtual System::IntPtr GetElementPtr();

  /// connects an observer to the object
  virtual void Connect( System::IntPtr observer );

  /// disconnects an observer from the object
  virtual void Disconnect( System::IntPtr observer );

  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;

  /// initialize
  void CommonInit();

// event handlers
public:

  /// implements the IObserver interface
  virtual void OnModified( System::IntPtr modifiedList );

// member variables
private:

  // vxDataRendererSoftware
  vxShareableObject< vxDataRendererSoftware > * m_pObject;

  // observer
  vxManagedObserver  * m_pObserver;

}; // class DataRendererSoftware


/**
 * class declaration for ElementVector
 */
public ref class ElementVector :
  public IObserver,
  public IElement
{
// enums and constants
public:
  
  /// enumeration of vector units
  enum class VectorUnits { Millimeter, Voxels };

// events
public:

  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;

// construction
public:

  /// constructor
  ElementVector();

  /// destructor
  virtual ~ElementVector();

  /// finalize
  !ElementVector();

// member functions
public:

  /// returns a pointer to the extended element
  virtual System::IntPtr GetElementPtr();

  /// connects an observer to the object
  virtual void Connect( System::IntPtr observer );

  /// disconnects an observer from the object
  virtual void Disconnect( System::IntPtr observer );

  /// initialize
  void CommonInit();

// properties
public:

  /// return x component
  property float4 X { float4 get() { return m_pVector->GetValue().X(); }}

  /// return y component
  property float4 Y { float4 get() { return m_pVector->GetValue().Y(); }}

  /// return z component
  property float4 Z { float4 get() { return m_pVector->GetValue().Z(); }}

  /// Gets or sets the vector units
  property VectorUnits Units
  {
    VectorUnits get() { return m_eUnits; }
    void set( VectorUnits eUnits ) { m_eUnits = eUnits; }
  } // Units

// event handlers
public:

  /// implements the IObserver interface
  virtual void OnModified( System::IntPtr modifiedList );

// member variables
private:

  // float4 value
  vxShareableObject< vxElementVector< float4 > > * m_pVector;

  // observer
  vxManagedObserver  * m_pObserver;

  // vector units
  VectorUnits m_eUnits;

}; // ElementFloat4


/**
 * Phase information
 */
public ref class PhaseInfo : public IObserver, public IElement
{
// construction
public:

  /// constructor
  PhaseInfo();

  /// constructor
  PhaseInfo( int4 iIndex, int4 iCount, float4 fPercentage );

  /// destructor
  virtual ~PhaseInfo();

  /// finalize
  !PhaseInfo();

// member functions
public:

  /// returns a pointer to the extended element
  virtual System::IntPtr GetElementPtr();

  /// connects an observer to the object
  virtual void Connect( System::IntPtr observer );

  /// disconnects an observer from the object
  virtual void Disconnect( System::IntPtr observer );

  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;

  /// initialize
  void CommonInit();

// event handlers
public:

  /// implements the IObserver interface
  virtual void OnModified( System::IntPtr modifiedList );

// properties
public:

  /// gets or sets the phase index
  property int4 Index
  { 
    int4 get();
    void set( int4 iIndex );
  } // Index

  /// gets or sets the total phase count
  property int4 Count
  { 
    int4 get();
    void set( int4 iCount );
  } // Count

  /// gets or sets the phase percentage
  property float4 Percentage
  { 
    float4 get();
    void set( float4 iPercentage );
  } // Percentage

// member variables
private:

  // vxCardiacPhaseInfo value
  vxShareableObject< vxPhaseInfo > * m_pPhaseInfo;

  // observer
  vxManagedObserver  * m_pObserver;
}; // class CardiacPhaseInfo


/**
 * class declaration for ElementString
 */
public ref class ElementString :
  public IElement
{
// events
public:

  /// raised when element is modified
  virtual event System::EventHandler ^ Modified;

// construction
public:

    /// constructor
  ElementString();

  /// destructor
  virtual ~ElementString();

  /// finalize
  !ElementString();

public:

  /// returns a pointer to the extended element
  virtual System::IntPtr GetElementPtr();

  /// initialize
  void CommonInit();

  /// connects to the specified vxObserver
  virtual void Connect( System::IntPtr observer ){;};
  
  /// disconnects from the specified vxObserver
  virtual void Disconnect( System::IntPtr observer ){;};

// event handlers
public:

  /// implements the IObserver interface
  virtual void OnModified( System::IntPtr modifiedList );

// member variables
private:

  // vxRibbon value
  vxElementString * m_pElementString;

}; // ElementString
  


// namespaces
CLOSE_VISUALIZATION_NS


// $Log: v3D_Elements.h,v $
// Revision 1.14.2.1  2009/07/29 13:23:40  kchalupa
// Code Walkthrough
//
// Revision 1.14  2008/04/03 22:38:12  gdavidson
// Added an orientation property to Ribbon
//
// Revision 1.13  2008/03/20 16:55:12  cqian
// add elementstring wrapper
//
// Revision 1.12  2007/09/19 14:36:12  gdavidson
// Added PhaseInfo object
//
// Revision 1.11  2007/04/06 14:10:57  gdavidson
// Added ElementVector
//
// Revision 1.10  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.9  2006/09/27 17:23:48  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.8  2006/08/17 14:07:16  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.7  2006/08/07 19:44:05  gdavidson
// Added DataRendererSoftware
//
// Revision 1.6  2006/07/12 19:38:35  geconomos
// modified ElementPixelRGBA to contain vxPixelRgba< uint1 >instead of vxPixelRgba< float4  >
//
// Revision 1.5  2006/05/18 17:49:46  gdavidson
// Added ElementPoint3D
//
// Revision 1.4  2006/03/27 19:46:52  gdavidson
// Added ElementPixelRGBA
//
// Revision 1.3  2004/10/05 12:35:20  frank
// unmanaged object needs to be private
//
// Revision 1.2  2004/10/04 15:09:21  frank
// improved the extended elements modified interface
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.7  2004/03/02 01:10:45  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.6.2.1  2004/02/24 20:07:32  michael
// coding standard: assign NULL after deletion and m_pOberserver --> m_pObserver
//
// Revision 1.6  2003/12/16 13:25:51  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.5  2003/08/08 19:57:10  wenli
// Add a new element: ElementBool
//
// Revision 1.4  2003/05/13 13:28:44  michael
// code review
//
// Revision 1.3  2003/05/06 16:29:04  geconomos
// Coding standards.
//
// Revision 1.2  2003/05/06 16:20:49  frank
// code review
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Elements.h,v 1.14.2.1 2009/07/29 13:23:40 kchalupa Exp $
// $Id: v3D_Elements.h,v 1.14.2.1 2009/07/29 13:23:40 kchalupa Exp $
