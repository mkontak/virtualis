// $Id: v3D_Elements.cpp,v 1.15.2.1 2009/07/29 13:23:40 kchalupa Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.com)


// includes
#include "Stdafx.h"
#include "v3D_Elements.h"
#include "v3D_Interfaces.h"
#include "v3D_Enums.h"
#include "vxManagedObserver.h"
#include "vxElementInt4.h"
#include "vxElementFloat4.h"
#include "vxElementPixelRGBA.h"
#include "vxElementPoint3D.h"
#include "PixelRGBA.h"
#include "vxRibbon.h"
#include "vxDataRendererSoftware.h"
#include "vxElementString.h"


// namespaces
USING_VISUALIZATION_NS


/**
* Constructor
* @param elementValue the initial value
*/
ElementBool::ElementBool( bool bElementValue )
{
  CommonInit( bElementValue );
} // ElementBool( bElementValue )


/**
* destructor
*/
ElementBool::~ElementBool()
{
  this->!ElementBool();
} // ~ElementBool()


/**
* Finalize
*/
ElementBool::!ElementBool()
{
  m_pBool->Disconnect( m_pObserver );

  delete m_pBool;      m_pBool = NULL;
  delete m_pObserver; m_pObserver = NULL;
} // !ElementBool()


/**
 * Returns a pointer to the extended element
 *
 * @return pointer to element
 */
System::IntPtr ElementBool::GetElementPtr()
{
  return System::IntPtr( m_pBool );
} // GetElementPtr()


/**
 * Connects an observer to the object
 *
 * @param observer the overver
 */
void ElementBool::Connect( System::IntPtr observer )
{
  m_pBool->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect( observer )


/**
 * Disconnects an observer from the object
 *
 * @param observer the observer
 */
void ElementBool::Disconnect( System::IntPtr observer )
{
  m_pBool->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect( observer )


/**
* Implements the IObserver interface
 *
 * @param modifiedList the modified list
 */
void ElementBool::OnModified( System::IntPtr modifiedList )
{
} // OnModified( modifiedList )


/**
 * Initialize
 *
 * @param bElementValue the initial value
 */
void ElementBool::CommonInit( bool bElementValue )
{
  m_pBool = new vxShareableObject< vxElementBool >();
  m_pBool->SetValue( bElementValue );

  m_pObserver = new vxManagedObserver( this );
  m_pBool->Connect( m_pObserver );
} // CommonInit( bElementValue )


/**
 * Retrieves the value of the object
 *
 * @return the value
 */
bool ElementBool::Value::get()
{
  return m_pBool->GetValue();
} // get_Value()


/**
 * Sets the value of the object
 *
 * @param bValue the value
 */
void ElementBool::Value::set( bool bValue )
{
  m_pBool->SetValue( bValue );
  Modified( this, System::EventArgs::Empty );
} // set_Value( bValue )


/**
 * Constructor
 */
ElementInt4::ElementInt4()
{
  CommonInit( 0 );
} // ElementInt4()


/**
 * Constructor
 * @param elementValue the initial value
 */
ElementInt4::ElementInt4( int iElementValue )
{
  CommonInit( iElementValue );
} // ElementInt4()


/**
 * destructor
 */
ElementInt4::~ElementInt4()
{
  this->!ElementInt4();
} // ~ElementInt4()


/**
 * Finalize
 */
ElementInt4::!ElementInt4()
{
  m_pInt4->Disconnect( m_pObserver );
  delete m_pInt4;
} // !ElementInt4()


/**
 * Returns a pointer to the extended element
 *
 * @return pointer to element
 */
System::IntPtr ElementInt4::GetElementPtr()
{
  return System::IntPtr( m_pInt4 );
} // GetElementPtr()


/**
 * Connects an observer to the object
 *
 * @param observer the overver
 */
void ElementInt4::Connect( System::IntPtr observer )
{
  m_pInt4->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect( observer )


/**
 * Disconnects an observer from the object
 *
 * @param observer the observer
 */
void ElementInt4::Disconnect( System::IntPtr observer )
{
  m_pInt4->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect( observer )


/**
 * Implements the IObserver interface
 *
 * @param modifiedList the modified list
 */
void ElementInt4::OnModified( System::IntPtr modifiedList )
{
} // OnModified( modifiedList )
  

/**
 * Initialize
 *
 * @param elementValue the initial value
 */
void ElementInt4::CommonInit( int iElementValue )
{
  m_pInt4 = new vxShareableObject< vxElementInt4 >();
  m_pInt4->SetValue( iElementValue );
  
  m_pObserver = new vxManagedObserver( this );
  m_pInt4->Connect( m_pObserver );
} // CommonInit( iElementInfo )


/**
 * Retrieves the value of the object
 *
 * @return the value
 */
int ElementInt4::Value::get()
{
  return m_pInt4->GetValue();
} // get_Value()


/**
 * Sets the value of the object
 *
 * @param value the value
 */
void ElementInt4::Value::set( int iValue )
{
  m_pInt4->SetValue( iValue );
  Modified( this, System::EventArgs::Empty );
} // set_Value()


/**
 * Constructor
 */
ElementFloat4::ElementFloat4()
{
  CommonInit( 0 );
} // ElementFloat4()


/**
 * Constructor
 *
 * @param fElementValue  Inital value
 */
ElementFloat4::ElementFloat4( float4 fElementValue )
{
  CommonInit( fElementValue );
} // ElementFloat4( fElementValue )


/**
 * destructor
 */
ElementFloat4::~ElementFloat4()
{
  this->!ElementFloat4(); 
} // ~ElementFloat4()


/**
 * Finalize
 */
ElementFloat4::!ElementFloat4()
{
  m_pFloat4->Disconnect( m_pObserver );
  delete m_pFloat4;
} // !ElementFloat4()


/**
 * Returns a pointer to the extended element
 *
 * @return a pointer to the extended element
 */
System::IntPtr ElementFloat4::GetElementPtr()
{
  return System::IntPtr( m_pFloat4 );
} // GetElementPtr()


/**
 * Connects an observer to the object
 *
 * @param observer the observer
 */
void ElementFloat4::Connect( System::IntPtr observer )
{
  m_pFloat4->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect( observer )


/**
 * Disconnects an observer from the object
 *
 * @param observer the observer
 */
void ElementFloat4::Disconnect( System::IntPtr observer )
{
  m_pFloat4->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect( observer )


/**
 * Implements the IObserver interface
 *
 * @param ptrInfo the modified list
 */
void ElementFloat4::OnModified( System::IntPtr modifiedList )
{
} // OnModified( modifiedList )


/**
 * Initialize
 *
 * @param elementValue the initial value
 */
void ElementFloat4::CommonInit( float4 fElementValue )
{
  m_pFloat4 = new vxShareableObject< vxElementFloat4 >();
  m_pFloat4->SetValue( fElementValue );
  
  m_pObserver = new vxManagedObserver( this );
  m_pFloat4->Connect( m_pObserver );
} // CommonInit()


/**
 * Retrieves the value of the object
 * @return the value
 */
float ElementFloat4::Value::get()
{
  return m_pFloat4->GetValue();
} // get_Value()


/**
 * Sets the value of the object
 * @param value the value
 */
void ElementFloat4::Value::set( float4 fValue )
{
  m_pFloat4->SetValue( fValue );
  Modified( this, System::EventArgs::Empty );
} // set_Value()


/**
 * Constructor
 */
ElementPixelRGBA::ElementPixelRGBA()
{
  CommonInit();
  this->Color = System::Drawing::Color::White;
} // ElementPixelRGBA()


/**
 * Constructor
 */
ElementPixelRGBA::ElementPixelRGBA( System::Drawing::Color color )
{
  CommonInit();
  this->Color = color;
} // ElementPixelRGBA()


/**
 * destructor
 */
ElementPixelRGBA::~ElementPixelRGBA()
{
  this->!ElementPixelRGBA();
} // ~ElementPixelRGBA()


/**
 * Finalize
 */
ElementPixelRGBA::!ElementPixelRGBA()
{
  m_pPixelRGBA->Disconnect( m_pObserver );
  
  if( m_pPixelRGBA == NULL )
    delete m_pPixelRGBA;
  m_pPixelRGBA = NULL;
} // !ElementPixelRGBA()


/**
 * Returns a pointer to the extended element
 *
 * @return a pointer to the extended element
 */
System::IntPtr ElementPixelRGBA::GetElementPtr()
{
  return System::IntPtr( m_pPixelRGBA );
} // GetElementPtr()


/**
 * Connects an observer to the object
 *
 * @param observer the observer
 */
void ElementPixelRGBA::Connect( System::IntPtr observer )
{
  m_pPixelRGBA->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect( observer )


/**
 * Disconnects an observer from the object
 *
 * @param observer the observer
 */
void ElementPixelRGBA::Disconnect( System::IntPtr observer )
{
  m_pPixelRGBA->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect()


/**
 * Implements the IObserver interface
 *
 * @param ptrInfo the modified list
 */
void ElementPixelRGBA::OnModified( System::IntPtr modifiedList )
{
} // OnModified( modifiedList )


/**
 * Initialize
 *
 * @param elementValue the initial value
 */
void ElementPixelRGBA::CommonInit()
{
  m_pPixelRGBA = new vxShareableObject< vxElementPixelRGBA< uint1 > >();
  m_pObserver = new vxManagedObserver( this );
  m_pPixelRGBA->Connect( m_pObserver );
} // CommonInit()


/**
 * Retrieves the value of the object
 * @return the value
 */
System::Drawing::Color ElementPixelRGBA::Color::get()
{
  return System::Drawing::Color::FromArgb( m_pPixelRGBA->GetValue().A(), 
                                           m_pPixelRGBA->GetValue().R(), 
                                           m_pPixelRGBA->GetValue().G(), 
                                           m_pPixelRGBA->GetValue().B());
} // get_Color()


/**
 * Sets the value of the object
 *
 * @param mpColor the value
 */
void ElementPixelRGBA::Color::set( System::Drawing::Color mpColor )
{
  PixelRGBA< uint1 > pixelRGBA( mpColor.R, mpColor.G, mpColor.B, mpColor.A );
  m_pPixelRGBA->SetValue( pixelRGBA );

  Modified( this, System::EventArgs::Empty );
} // set_Color( mpColor )


/**
 * Constructor
 */
ElementPoint3D::ElementPoint3D()
{
  CommonInit();
} // ElementPoint3D()


/**
 * destructor
 */
ElementPoint3D::~ElementPoint3D()
{
  this->!ElementPoint3D();
} // ~ElementPoint3D()


/**
 * Finalize
 */
ElementPoint3D::!ElementPoint3D()
{
  m_pPoint3D->Disconnect( m_pObserver );
  delete m_pPoint3D;
} // !ElementPoint3D()


/**
 * Returns a pointer to the extended element
 *
 * @return a pointer to the extended element
 */
System::IntPtr ElementPoint3D::GetElementPtr()
{
  return System::IntPtr( m_pPoint3D );
} // GetElementPtr()


/**
 * Connects an observer to the object
 *
 * @param observer the observer
 */
void ElementPoint3D::Connect( System::IntPtr observer )
{
  m_pPoint3D->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect( observer )


/**
 * Disconnects an observer from the object
 *
 * @param observer the observer
 */
void ElementPoint3D::Disconnect( System::IntPtr observer )
{
  m_pPoint3D->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect( observer )


/**
 * Implements the IObserver interface
 *
 * @param ptrInfo the modified list
 */
void ElementPoint3D::OnModified( System::IntPtr modifiedList )
{
} // OnModified( modifiedList )


/**
 * Initialize
 */
void ElementPoint3D::CommonInit()
{
  m_pPoint3D = new vxShareableObject< vxElementPoint3D< float4 > >();
  m_pObserver = new vxManagedObserver( this );
  m_pPoint3D->Connect( m_pObserver );
} // CommonInit()


/**
 * Constructor
 */
Ribbon::Ribbon()
{
  CommonInit();
} // Ribbon()


/**
 * destructor
 */
Ribbon::~Ribbon()
{
   this->!Ribbon();
} // ~Ribbon()


/**
 * Finalize
 */
Ribbon::!Ribbon()
{
  m_pRibbon->Disconnect( m_pObserver );
  delete m_pRibbon;
} // !Ribbon()


/**
 * Initialize
 *
 * @param elementValue the initial value
 */
void Ribbon::CommonInit()
{
  m_pRibbon = new vxShareableObject< vxRibbon >();
  m_pObserver = new vxManagedObserver( this );
  m_pRibbon->Connect( m_pObserver );
} // CommonInit()


/**
 * Returns a pointer to the extended element
 *
 * @return a pointer to the extended element
 */
System::IntPtr Ribbon::GetElementPtr()
{
  return System::IntPtr( m_pRibbon );
} // GetElementPtr()


/**
 * Connects an observer to the object
 *
 * @param observer the observer
 */
void Ribbon::Connect( System::IntPtr observer )
{
  m_pRibbon->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect( observer )


/**
 * Disconnects an observer from the object
 *
 * @param observer the observer
 */
void Ribbon::Disconnect( System::IntPtr observer )
{
  m_pRibbon->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect( observer )


/**
 * Implements the IObserver interface
 *
 * @param modifiedList the modified list
 */
void Ribbon::OnModified( System::IntPtr modifiedList )
{
} // OnModified( modifiedList )


/**
 * Retrieves the value of the object
 *
 * @return the value
 */
Ribbon::RibbonOrientationEnum Ribbon::Orientation::get()
{
  return static_cast< Ribbon::RibbonOrientationEnum >( m_pRibbon->GetOriModeOfMPR() );
} // get_Orientation()


/**
 * Sets the value of the object
 *
 * @param value the value
 */
void Ribbon::Orientation::set( Ribbon::RibbonOrientationEnum eOrientation )
{
  m_pRibbon->SetOriModeOfMPR( static_cast< vxRibbon::OrientationModeOfMPR >( eOrientation ));
  Modified( this, System::EventArgs::Empty );
} // set_Orientation( eOrientation )


/**
 * Constructor
 */
DataRendererSoftware::DataRendererSoftware()
{
  CommonInit();
} // DataRendererSoftware()


/**
 * destructor
 */
DataRendererSoftware::~DataRendererSoftware()
{
  this->!DataRendererSoftware();
} // ~DataRendererSoftware()


/**
 * Finalize
 */
DataRendererSoftware::!DataRendererSoftware()
{
  m_pObject->Disconnect( m_pObserver );
  delete m_pObject;
} // !DataRendererSoftware()


/**
 * Returns a pointer to the extended element
 *
 * @return a pointer to the extended element
 */
System::IntPtr DataRendererSoftware::GetElementPtr()
{
  return System::IntPtr( m_pObject );
} // GetElementPtr()


/**
 * Connects an observer to the object
 *
 * @param observer the observer
 */
void DataRendererSoftware::Connect( System::IntPtr observer )
{
  m_pObject->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect( observer )


/**
 * Disconnects an observer from the object
 *
 * @param observer the observer
 */
void DataRendererSoftware::Disconnect( System::IntPtr observer )
{
  m_pObject->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect( observer )


/**
 * Implements the IObserver interface
 *
 * @param ptrInfo the modified list
 */
void DataRendererSoftware::OnModified( System::IntPtr modifiedList )
{
} // OnModified( modifiedList )


/**
 * Initialize
 *
 * @param elementValue the initial value
 */
void DataRendererSoftware::CommonInit()
{
  m_pObject = new vxShareableObject< vxDataRendererSoftware >();
  m_pObserver = new vxManagedObserver( this );
  m_pObject->Connect( m_pObserver );
} // CommonInit()


/**
 * Constructor
 */
ElementVector::ElementVector()
{
  CommonInit();
} // ElementVector()


/**
 * destructor
 */
ElementVector::~ElementVector()
{
  this->!ElementVector();
} // ~ElementVector()


/**
 * Finalize
 */
ElementVector::!ElementVector()
{
  m_pVector->Disconnect( m_pObserver );
  delete m_pVector;
} // !ElementVector()


/**
 * Returns a pointer to the extended element
 *
 * @return a pointer to the extended element
 */
System::IntPtr ElementVector::GetElementPtr()
{
  return System::IntPtr( m_pVector );
} // GetElementPtr()


/**
 * Connects an observer to the object
 *
 * @param observer the observer
 */
void ElementVector::Connect( System::IntPtr observer )
{
  m_pVector->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect( observer )


/**
 * Disconnects an observer from the object
 *
 * @param observer the observer
 */
void ElementVector::Disconnect( System::IntPtr observer )
{
  m_pVector->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect( observer )


/**
 * Implements the IObserver interface
 *
 * @param modifiedList the modified list
 */
void ElementVector::OnModified( System::IntPtr modifiedList )
{
} // OnModified( modifiedList )


/**
 * Initialize
 *
 * @param elementValue the initial value
 */
void ElementVector::CommonInit()
{
  m_pVector = new vxShareableObject< vxElementVector< float4 > >();
  m_pObserver = new vxManagedObserver( this );
  m_pVector->Connect( m_pObserver );
  m_eUnits = ElementVector::VectorUnits::Millimeter;
} // CommonInit()


/**
 * Constructor
 */
PhaseInfo::PhaseInfo()
{
  CommonInit();
} // PhaseInfo()


/**
 * Constructor
 */
PhaseInfo::PhaseInfo( int4 iIndex, int4 iCount, float4 fPercentage )
{
  CommonInit();
  this->Index = iIndex;
  this->Count = iCount;
  this->Percentage = fPercentage;
} // PhaseInfo()


/**
 * destructor
 */
PhaseInfo::~PhaseInfo()
{
   this->!PhaseInfo();
} // ~PhaseInfo()


/**
 * Finalize
 */
PhaseInfo::!PhaseInfo()
{
  m_pPhaseInfo->Disconnect( m_pObserver );
  delete m_pPhaseInfo;
} // !PhaseInfo()


/**
 * Returns a pointer to the extended element
 *
 * @return a pointer to the extended element
 */
System::IntPtr PhaseInfo::GetElementPtr()
{
  return System::IntPtr( m_pPhaseInfo );
} // GetElementPtr()


/**
 * Connects an vxManagedObserver to the object
 *
 * @param observer the vxManagedObserver
 */
void PhaseInfo::Connect( System::IntPtr observer )
{
  m_pPhaseInfo->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect( observer )


/**
 * Disconnects an vxManagedObserver from the object
 *
 * @param observer the vxManagedObserver
 */
void PhaseInfo::Disconnect( System::IntPtr observer )
{
  m_pPhaseInfo->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect( observer )


/**
 * Implements the IvxManagedObserver interface
 *
 * @param modifiedList the modified list
 */
void PhaseInfo::OnModified( System::IntPtr modifiedList )
{
} // OnModified( modifiedList )


/**
 * Initialize
 *
 * @param elementValue the initial value
 */
void PhaseInfo::CommonInit()
{
  m_pPhaseInfo = new vxShareableObject< vxPhaseInfo >();
  m_pObserver = new vxManagedObserver( this );
  m_pPhaseInfo->Connect( m_pObserver );
} // CommonInit()


/**
 * gets the phase index
 */
int4 PhaseInfo::Index::get()
{
  return m_pPhaseInfo->GetIndex(); 
} // get_Index()


/**
 * sets the phase index
 *
 * @param iIndex  The specified index
 */
void PhaseInfo::Index::set( int4 iIndex )
{
  m_pPhaseInfo->SetIndex( iIndex ); 
} // set_Index( iIndex )


/**
 * gets the total phase count
 */
int4 PhaseInfo::Count::get()
{
  return m_pPhaseInfo->GetCount(); 
} // get_Count


/**
 * sets the total phase count
 *
 * @param iCount  The count desired
 */
void PhaseInfo::Count::set( int4 iCount )
{
  m_pPhaseInfo->SetCount( iCount ); 
} // set_Count( iCount )


/**
 * gets the phase percentage
 */
float4 PhaseInfo::Percentage::get()
{
  return m_pPhaseInfo->GetPercentage(); 
} // get_Percentage()


/**
 * sets the phase percentage
 */
void PhaseInfo::Percentage::set( float4 fPercentage )
{
  m_pPhaseInfo->SetPercentage( fPercentage ); 
} // set_Percentage( fPercentage )


/**
 * Constructor
 */
ElementString::ElementString()
{
  CommonInit();
} // PhaseInfo()


/**
 * destructor
 */
ElementString::~ElementString()
{
   this->!ElementString();
} // ~PhaseInfo()


/**
 * Finalize
 */
ElementString::!ElementString()
{
  delete m_pElementString;
} // !PhaseInfo()



/**
 * Implements the IvxManagedObserver interface
 *
 * @param ptrInfo the modified list
 */
void ElementString::OnModified( System::IntPtr modifiedList )
{
} // OnModified( modifiedList )


/**
 * Returns a pointer to the extended element
 *
 * @return a pointer to the extended element
 */
System::IntPtr ElementString::GetElementPtr()
{
  return System::IntPtr( m_pElementString );
} // GetElementPtr()


/**
 * Initialize
 *
 * @param elementValue the initial value
 */
void ElementString::CommonInit()
{
  m_pElementString = new vxElementString();
} // CommonInit


// $Log: v3D_Elements.cpp,v $
// Revision 1.15.2.1  2009/07/29 13:23:40  kchalupa
// Code Walkthrough
//
// Revision 1.15  2008/05/02 05:00:51  cqian
// check in
//
// Revision 1.14  2008/04/03 22:38:12  gdavidson
// Added an orientation property to Ribbon
//
// Revision 1.13  2008/03/20 16:55:07  cqian
// add elementstring wrapper
//
// Revision 1.12  2008/01/28 19:53:20  cqian
// Add a cut plane for LV segmentation
//
// Revision 1.11  2007/09/19 14:36:11  gdavidson
// Added PhaseInfo object
//
// Revision 1.10  2007/04/06 14:10:57  gdavidson
// Added ElementVector
//
// Revision 1.9  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.8  2006/09/27 17:23:48  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.7  2006/08/17 14:07:16  gdavidson
// Moved CurvedMPR to vxViewer
//
// Revision 1.6  2006/08/07 19:44:05  gdavidson
// Added DataRendererSoftware
//
// Revision 1.5  2006/07/12 19:38:35  geconomos
// modified ElementPixelRGBA to contain vxPixelRgba< uint1 >instead of vxPixelRgba< float4  >
//
// Revision 1.4  2006/05/18 17:49:46  gdavidson
// Added ElementPoint3D
//
// Revision 1.3  2006/03/27 19:46:52  gdavidson
// Added ElementPixelRGBA
//
// Revision 1.2  2004/10/04 15:08:44  frank
// improved the extended elements modified interface
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2004/03/02 01:10:45  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.9.2.1  2004/02/24 20:07:32  michael
// coding standard: assign NULL after deletion and m_pOberserver --> m_pObserver
//
// Revision 1.9  2003/12/16 13:25:51  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.8  2003/08/08 19:57:10  wenli
// Add a new element: ElementBool
//
// Revision 1.7  2003/05/13 13:28:44  michael
// code review
//
// Revision 1.6  2003/05/06 16:29:04  geconomos
// Coding standards.
//
// Revision 1.5  2003/05/06 16:20:49  frank
// code review
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Elements.cpp,v 1.15.2.1 2009/07/29 13:23:40 kchalupa Exp $
// $Id: v3D_Elements.cpp,v 1.15.2.1 2009/07/29 13:23:40 kchalupa Exp $
