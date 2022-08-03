// $Id: v3D_Cursor3d.cpp,v 1.4 2006/10/04 18:29:04 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (michael@viatronix.com)


// includes
#include "StdAfx.h"
#include "v3D_Cursor3d.h"
#include "vxManagedObserver.h"
#include "vxUtils.h"
#include "vxSerializer.h"


// namespaces
using namespace Viatronix::Visualization;
using namespace System;
using namespace System::Xml;


/**
 * Default constructor
 */
Cursor3d::Cursor3d()
{
  m_pCursor3d = new vxShareableObject<vxCursor3d>();
  CommonInit();
} // default constructor


/**
 * Constructor from vxCursor3d
 */
Cursor3d::Cursor3d( vxCursor3d * pCursor3d )
{ 
  m_pCursor3d = new vxShareableObject<vxCursor3d>( *pCursor3d );
  CommonInit();
} // constructor


/**
 * Constructor from v3D Cursor3d
 */
Cursor3d::Cursor3d( Cursor3d ^ mpCursor3d )
{
  m_pCursor3d = new vxShareableObject<vxCursor3d>( );
  *m_pCursor3d = * static_cast<vxCursor3d *>( ToNativeCursor3dPtr( mpCursor3d->GetCursor3dPtr() ) );

  CommonInit();  
} // copy constructor


/**
 * Destructor
 */
Cursor3d::~Cursor3d()
{
  this->!Cursor3d();
} // ~Cursor3d()


/**
 * finalizer
 */
Cursor3d::!Cursor3d()
{
  if ( m_pCursor3d != NULL )
  {
    m_pCursor3d->Disconnect( m_pObserver );
  }

  delete m_pCursor3d;  m_pCursor3d = NULL;
  delete m_pObserver; m_pObserver = NULL;
} // !Cursor3d()


/**
 * Returns the position
 */
Triplet ^ Cursor3d::Position::get()
{
  Point<float4> position(m_pCursor3d->GetEye());
  Triplet ^ mpPosition = gcnew Triplet(position.X(), position.Y(), position.Z());

  return mpPosition;
} // get_Position()


/**
 * Sets the position
 */
void Cursor3d::Position::set( Triplet ^ mpPosition )
{
  m_pCursor3d->SetEye(Point<float4>(mpPosition->x, mpPosition->y, mpPosition->z));
} // set_Position()


/**
 * Returns the view reference point - not the view direction
 *
 * @return the view reference point (not the view direction as you might want)
 */
Triplet ^ Cursor3d::Reference::get()
{
  Point<float4> viewReferencePoint( m_pCursor3d->GetViewReference() );
  return gcnew Triplet( viewReferencePoint.m_x, viewReferencePoint.m_y, viewReferencePoint.m_z );
} // get_Reference()


/**
 * sets the view reference point - not the view direction
 */
void Cursor3d::Reference::set( Triplet ^ mpViewReferencePoint )
{
  m_pCursor3d->SetViewReference( Point<float4>( mpViewReferencePoint->x, mpViewReferencePoint->y, mpViewReferencePoint->z ) );
} // set_Reference()


/**
 * Returns the view direction
 *
 * @return the view direction
 */
Triplet ^ Cursor3d::Direction::get()
{
  Normal3Df viewDirection( m_pCursor3d->GetViewDirection() );
  return gcnew Triplet( viewDirection.X(), viewDirection.Y(), viewDirection.Z() );
} // get_Direction()


/**
 * sets the view direction
 *
 * @param mpViewDirection the view direction
 */
void Cursor3d::Direction::set( Triplet ^ mpViewDirection )
{
  m_pCursor3d->SetViewDirection( Normal3Df( mpViewDirection->x, mpViewDirection->y, mpViewDirection->z ) );
} // set_Direction()


/**
 * Common initalization routine.
 */
void Cursor3d::CommonInit()
{
  // unmanaged types 
  m_pObserver = new vxManagedObserver( this );
  m_pCursor3d->Connect( m_pObserver );
} // CommonInit();


/** 
 * Receives modified information on connected objects. 
 */
void Cursor3d::OnModified( System::IntPtr ptrInfo )
{
} // OnModified()


/** 
 * Connects an observer to the object.
 */
void Cursor3d::Connect( System::IntPtr observer )
{
  m_pCursor3d->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect()


/** 
 * Disconnects an observer from the object.
 */
void Cursor3d::Disconnect( System::IntPtr  observer )
{
  m_pCursor3d->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect()


/**
 * Serializes the Cursor3d object to xml.
 *
 * @return xml string representing object.
 */
System::String ^ Cursor3d::ToXml()
{
  try
  {
    vxCursor3d * pCursor3d(ToNativeCursor3dPtr(GetCursor3dPtr()));

    // write it
    vxSerializer serializer("Cursor3d");
    serializer.WriteAttribute("version", "2.0");
    serializer.Write(vxSerializer::AddElement(serializer.GetCurrent(), vxSerializer::GetElementName(*pCursor3d), "Cursor3d"), *pCursor3d);

    return gcnew System::String( serializer.ToString().c_str() );
  }
  catch(ex::VException & e)
  {
    throw gcnew v3DException(&e);
  }
} // ToXml()


/**
 * Creates a new Cursor3d object from the specified xml string
 * @param xml xml string
 * @return Cursor3d object
 */
Cursor3d ^ Cursor3d::FromXml( System::String ^ mpXml )
{
  try
  {
    // create new colormap object
    Cursor3d ^ mpCursor3d( nullptr );

    // create xml document and get root element
    vxDOMDocument document = vxDOMDocument::FromXml( ss( mpXml ) );
    vxDOMElement element = document.GetDocumentElement();

    std::string sVersion("1.0");
    vxSerializer::GetAttribute(element, "version", sVersion);

    if (sVersion == "2.0")
    {
      // read cursor
      vxCursor3d cursor3d;
      vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(cursor3d), "cursor3d"), cursor3d);
      mpCursor3d = gcnew Cursor3d(&cursor3d);
    }
    return mpCursor3d;
  } // try
  catch(ex::VException & e)
  {
    throw gcnew v3DException(&e);
  }
} // FromXml()


/**
 * Creates a new Cursor3d from the specified xml resource.
 * @param xml xml string
 * @return Cursor3d object
 */
Cursor3d ^ Cursor3d::FromResource( System::String ^ mpXmlResource )
{
  try
  {
    XmlTextReader xmlReader( System::Reflection::Assembly::GetCallingAssembly()->GetManifestResourceStream( mpXmlResource ) );
    XmlDocument xmlDocument;
    xmlDocument.Load( %xmlReader );

    return Cursor3d::FromXml( xmlDocument.InnerXml );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // FromResource()


/**
 * Creates a new Cursor3d from the specified xml file string
 * @param xml xml string
 * @return Cursor3d object
 */
Cursor3d ^ Cursor3d::FromFile( System::String ^ mpXmlFile )
{
  try
  {
    XmlDocument xmlDocument;
    xmlDocument.Load( mpXmlFile );

    return Cursor3d::FromXml( xmlDocument.InnerXml );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // FromFile()


// $Log: v3D_Cursor3d.cpp,v $
// Revision 1.4  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.3  2006/09/27 17:23:48  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.2  2005/04/12 15:23:52  geconomos
// changed xml library to the vxDOM classes
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.7  2004/03/02 01:10:46  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.6.2.1  2004/02/24 20:06:41  michael
// coding standard: assign NULL after deletion
//
// Revision 1.6  2003/12/16 13:25:51  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.5  2003/08/25 15:52:58  frank
// fixed function descriptions
//
// Revision 1.4  2003/05/16 13:09:37  frank
// code formatting
//
// Revision 1.3  2003/05/13 13:28:44  michael
// code review
//
// Revision 1.2  2003/05/06 15:14:53  michael
// code review
//
// Revision 1.1  2003/04/24 14:01:29  michael
// added vxCursor3d class
//
// Revision 1.17  2003/04/16 19:22:52  geconomos
// Issue #3100: Cemar not resetting properly.
//
// Revision 1.16  2002/11/27 16:31:19  geconomos
// Added CopyFrom method
//
// Revision 1.15  2002/11/19 20:38:39  geconomos
// Removed remaining comments for serialization.
//
// Revision 1.14  2002/11/18 20:36:16  geconomos
// Removed xml reminders.
//
// Revision 1.13  2002/11/12 23:54:28  michael
// added inital To/FromXml()
//
// Revision 1.12  2002/11/11 21:22:52  geconomos
// Changed copy constructor NOT to copy connect clients.
//
// Revision 1.11  2002/11/11 19:19:06  geconomos
// Reworked serialization.
//
// Revision 1.10  2002/11/07 17:23:38  geconomos
// Added ViewAll methods.
//
// Revision 1.9  2002/11/05 22:43:57  ingmar
// removed dead code
//
// Revision 1.8  2002/11/04 14:30:35  geconomos
// new toolbar class.
//
// Revision 1.7  2002/10/31 12:49:57  geconomos
// Chaged GetCursor3dPtr() to return a System::IntPtr for interoperbility.
//
// Revision 1.6  2002/10/24 01:24:17  michael
// added additional constructor
//
// Revision 1.5  2002/10/18 20:30:17  frank
// Added IElement interface.
//
// Revision 1.4  2002/10/17 15:28:15  geconomos
// Added constructor from native type and created CommonInit() method
//
// Revision 1.3  2002/10/16 22:20:00  michael
// added initial serialization from resource code ... needs work :-)
//
// Revision 1.2  2002/09/24 16:08:21  geconomos
// Exception handling and formatting.
//
// Revision 1.1  2002/08/02 13:53:56  dmitry
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Cursor3d.cpp,v 1.4 2006/10/04 18:29:04 gdavidson Exp $
// $Id: v3D_Cursor3d.cpp,v 1.4 2006/10/04 18:29:04 gdavidson Exp $
