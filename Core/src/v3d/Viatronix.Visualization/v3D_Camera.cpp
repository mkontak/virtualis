// $Id: v3D_Camera.cpp,v 1.7 2006/10/04 18:29:04 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)


// includes
#include "StdAfx.h"
#include "v3d_Camera.h"
#include "vxManagedObserver.h"
#include "vxUtils.h"
#include "vxSerializer.h"


// namespaces
using namespace Viatronix::Visualization;
using namespace System;
using namespace System::Xml;


/**
 * Initializes a Camera object with default values.
 */
Camera::Camera()
{
  m_pCamera = new vxShareableObject< vxCamera >();
  CommonInit();
} // Camera()


/**
 * Destructor
 */
Camera::~Camera()
{
  this->!Camera();
} // ~Camera()


/**
 * fnalizer
 */
Camera::!Camera()
{
  if ( m_pCamera != NULL ) 
  {
    m_pCamera->Disconnect( m_pObserver );
  }

  delete m_pCamera;
  m_pCamera = NULL;

  delete m_pObserver;
  m_pObserver = NULL;
} // !Camera()


/**
 * Creates a new object that is a copy of the current instance.
 *
 * @return  A new object that is a copy of this instance.
 */
System::Object ^ Camera::Clone()
{
  // create a new instance of the annotations class
  Camera ^ mpCamera = gcnew Camera();

  // get the underlying implementation pointer  
  vxCamera * pCamera = dynamic_cast< vxCamera *>( mpCamera->m_pCamera );

  // make a copy of the underying implementations
  *pCamera = *dynamic_cast< vxCamera *>( m_pCamera );  

  return mpCamera;
} // Clone()


/**
 * Initializes this camera from another camera.
 *
 * @param mpCamera Other camera.
 * @para, mpVolume Volume to adjust camera for.
 */
void Camera::CopyFrom( Camera ^ mpCamera, Volume ^ mpVolume )
{
  *m_pCamera = * static_cast< vxCamera*> ( ToNativeCameraPtr( mpCamera->GetElementPtr() ) );
  
  Triple<float4> size( ToNativeVolumeInt2Ptr( mpVolume->GetVolumePtr() )->GetHeader().GetWorldDim() );
  
  // Cameras should only be assigned within the same dataset to ensure that it can be proportionally.
  // Otherwise, we need access to the bounding box of this and the OTHER VOLUME
  //Triple<float4> sizeOther( ToNativeVolumeInt2Ptr( mpVolumeOther->GetVolumePtr() )->GetHeader().GetWorldDim() );
  //vxUtils::AdjustCameraProportionally(*m_pCamera, Box<float4>(0, 0, 0, sizeOther[0], sizeOther[1], sizeOther[2]), Box<float4>(0, 0, 0, size[0], size[1], size[2]));
  
  m_pCamera->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );
} // CopyFrom()


/**
 * Common initalization routine.
 */
void Camera::CommonInit()
{
  // managed types
  Triple<uint1> color = m_pColors->GetColor();
  m_color = System::Drawing::Color::FromArgb( color.X(), color.Y(), color.Z() );

  // unmanaged types 
  m_pObserver = new vxManagedObserver( this );
  m_pCamera->Connect( m_pObserver );
} // CommonInit();


/** 
 * Resets the camera to home position.
 */
void Camera::Reset()
{
  try
  {
    m_pCamera->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_INITIALIZED ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // Reset()


/**
 * Places the camera such that all of the object is in view.
 * /
void Camera::ViewAll()
{
  try
  {
    m_pCamera->ViewAll();
    m_pCamera->Modified( vxModInfo( vxCamera, vxUtils::CAMERA_MODIFIED ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // ViewAll()


/** 
 * Processes modified queue events.
 *
 * @param modifiedList Modified info list.
 */
void Camera::OnModified( System::IntPtr modifiedList )
{
} // OnModified()


/** 
 * Connects an observer to the object.
 * @param observer vxManagedObserver object interested in events.
 */
void Camera::Connect( System::IntPtr observer )
{
  m_pCamera->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect()


/** 
 * Disconnects an observer from the object.
 * @param observer vxManagedObserver object interested in events.
 */
void Camera::Disconnect( System::IntPtr observer )
{
  m_pCamera->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect()


/**
 * Serializes the camera object to xml.
 * @return Xml string representing object.
 */
System::String ^ Camera::ToXml()
{
  try
  {
    vxCamera * pCamera = ToNativeCameraPtr( GetElementPtr() );

    // write it
    vxSerializer serializer( "Camera" );
    serializer.WriteAttribute( "version", "2.0" );
    serializer.Write( vxSerializer::AddElement( serializer.GetCurrent(), vxSerializer::GetElementName( *pCamera ), "camera" ), *pCamera );

    return gcnew System::String( serializer.ToString().c_str() );
  }
  catch(ex::VException & e)
  {
    throw gcnew v3DException(&e);
  }
} // ToXml()


/**
 * Creates a new camera object from the specified xml string.
 *
 * @param mpXml Xml string to create object from.
 * @return Camera object.
 */
Camera ^ Camera::FromXml( System::String ^ mpXml )
{
  try
  {
    // create new colormap object
    Camera ^ mpCamera = gcnew Camera();

    // create xml document and get root element
    vxDOMDocument document = vxDOMDocument::FromXml( ss( mpXml ) );
    vxDOMElement element = document.GetDocumentElement();

    std::string sVersion( "1.0" );
    vxSerializer::GetAttribute( element, "version", sVersion );

    if ( sVersion == "2.0" )
    {
      // read colormap
      vxSerializer::Read( vxSerializer::FindElement( element, vxSerializer::GetElementName( *(mpCamera->m_pCamera) ), "camera" ), *(mpCamera->m_pCamera) );
    }
    return mpCamera;
  } // try
  catch(ex::VException & e)
  {
    throw gcnew v3DException(&e);
  }
} // FromXml()


/**
 * Creates a new camera from the specified xml resource.
 * @param mpXmlResource Xml resource string.
 * @return Camera object.
 */
Camera ^ Camera::FromResource( System::String ^ mpXmlResource )
{
  try
  {
    XmlTextReader xmlReader( System::Reflection::Assembly::GetCallingAssembly()->GetManifestResourceStream( mpXmlResource ) );
    XmlDocument xmlDocument;
    xmlDocument.Load( %xmlReader );

    return Camera::FromXml( xmlDocument.InnerXml );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // FromResource()


/**
 * Creates a new camera from the specified xml file string.
 * @param mpXmlFile Xml file.
 * @return Camera object.
 */
Camera ^ Camera::FromFile( System::String ^ mpXmlFile )
{
  try
  {
    XmlDocument xmlDocument;
    xmlDocument.Load( mpXmlFile );

    return Camera::FromXml( xmlDocument.InnerXml );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // FromFile()


// $Log: v3D_Camera.cpp,v $
// Revision 1.7  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.6  2006/09/27 17:23:48  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.5  2005/11/18 21:12:28  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.4.2.1  2005/07/29 16:24:39  frank
// removed dead code
//
// Revision 1.4  2005/04/12 15:23:52  geconomos
// changed xml library to the vxDOM classes
//
// Revision 1.3  2005/03/08 19:09:47  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.2.6.1  2005/02/17 21:00:21  geconomos
// cleaned up ICloneable
//
// Revision 1.2  2004/04/26 19:05:15  michael
// removed ViewAll from camera, need to use utilities
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.29  2004/03/02 01:10:47  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.28.2.1  2004/02/24 20:05:01  michael
// coding standard: assign NULL after deletion
//
// Revision 1.28  2003/12/16 13:25:50  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.27  2003/09/29 19:25:29  michael
// cosmetics
//
// Revision 1.26  2003/09/29 17:29:04  michael
// accounting for the new event types
//
// Revision 1.25  2003/09/11 20:37:33  frank
// Needed to set the camera modified after changing it
//
// Revision 1.24  2003/08/20 14:31:10  michael
// added "mailto:"
//
// Revision 1.23  2003/07/02 15:04:43  geconomos
// Implemented ICloneable
//
// Revision 1.22  2003/05/16 13:09:36  frank
// code formatting
//
// Revision 1.21  2003/05/13 15:15:56  michael
// code review
//
// Revision 1.20  2003/05/13 13:28:43  michael
// code review
//
// Revision 1.19  2003/05/06 15:31:33  geconomos
// More coding standards.
//
// Revision 1.18  2003/05/05 17:13:04  geconomos
// Coding standards.
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
// Chaged GetCameraPtr() to return a System::IntPtr for interoperbility.
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Camera.cpp,v 1.7 2006/10/04 18:29:04 gdavidson Exp $
// $Id: v3D_Camera.cpp,v 1.7 2006/10/04 18:29:04 gdavidson Exp $
