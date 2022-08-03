// $Id: v3D_Colormap.cpp,v 1.8 2007/02/27 20:41:46 dongqing Exp $
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
#include "v3d_Colormap.h"
#include "vxManagedObserver.h"
#include "vxUtils.h"
#include "vxSerializer.h"
#include "Point2D.h"
#include "vxColorTableCPU.h"


// namespaces
using namespace Viatronix::Visualization;
using namespace System;
using namespace System::Xml;
using namespace std;


/**
 * Initializes a Colormap object with default values.
 */
Colormap::Colormap()
{
  m_pColormap = new vxShareableObject< vxColormap >();

  CommonInit();
} // Colormap()


/**
 * Initializes a Colormap object with default values.
 */
Colormap::Colormap( Colormap ^ mpOther )
{
  m_pColormap = new vxShareableObject< vxColormap >();

  CommonInit();

  CopyFrom( mpOther );
} // Colormap()


/**
 * Destructor
 */
Colormap::~Colormap()
{
  this->!Colormap();
} // ~Colormap()


/**
 * Finalizer
 */
Colormap::!Colormap()
{
  if ( m_pColormap != NULL ) 
    m_pColormap->Disconnect( m_pObserver );
  
  delete m_pColormap;
  m_pColormap = NULL;

  delete m_pObserver;
  m_pObserver = NULL;
} // !Colormap()



/**
 * Creates a new object that is a copy of the current instance.
 *
 * @return  A new object that is a copy of this instance.
 */
System::Object ^ Colormap::Clone()
{
  // create a new instance of the annotations class
  Colormap ^ mpColormap = gcnew Colormap();
  
  // copy the properties
  mpColormap->m_bIsActiveRange = m_bIsActiveRange;
  mpColormap->m_mpName = System::String::Copy( m_mpName );

  // get the underlying implementation pointer  
  vxColormap * pColormap = dynamic_cast< vxColormap *>( mpColormap->m_pColormap );

  // make a copy of the underying implementations
  *pColormap = *dynamic_cast< vxColormap *>( m_pColormap );  

  return mpColormap;
} // Clone()


/**
 * Initialize colormap from other.
 *
 * @param mpColormap Colormap object to copy settings from.
 */
void Colormap::CopyFrom( Colormap ^ mpColormap )
{
  *m_pColormap = * static_cast< vxColormap*> ( ToNativeColormapPtr( mpColormap->GetColormapPtr() ) );

  m_pColormap->Modified( vxModInfo( vxColormap, vxUtils::COLORMAP_MODIFIED ) );
} // CopyFrom()


/**
 * Common initialization routine.
 */
void Colormap::CommonInit()
{
  // unmanaged types 
  m_pObserver = new vxManagedObserver( this );
  m_pColormap->Connect( m_pObserver );

  m_mpName = System::String::Empty;
  m_bIsActiveRange = true;
  const bool bActiveRange( m_bIsActiveRange );
  m_pColormap->SetActiveRange( bActiveRange );

  // managed types
  Triple<uint1> color = m_mpColors->GetColor();
  m_color = System::Drawing::Color::FromArgb( color.X(), color.Y(), color.Z() );

} // CommonInit()


/**
 * Sets the associated name.
 *
 * @param mpName  colormap name
 */
void Colormap::Name::set( System::String ^ mpName ) 
{ 
  string sName = ss( mpName );
  m_pColormap->SetName( sName );
} // set_Name()


/**
 * Returns the window value.
 *
 * @return Window value.
 */
float Colormap::Window::get()
{
	return m_pColormap->GetWindowLevel().GetWindow();
} // get_Window()


/**
 * Sets the window value.
 *
 * @param fWindow New window value.
 */
void Colormap::Window::set( float fWindow )
{
  vxWindowLevel windowLevel( m_pColormap->GetWindowLevel() );
  windowLevel.SetWindow( fWindow );
	m_pColormap->SetWindowLevel( windowLevel );
	m_pColormap->Modified( vxModInfo( vxColormap, true ) );
} // set_Window()


/**
 * Gets the level value.
 *
 * @areturn Level value.
 */
float Colormap::Level::get()
{
	return m_pColormap->GetWindowLevel().GetLevel();
} // get_Level()


/**
 * Sets the level value.
 *
 * @param New level value.
 */
void Colormap::Level::set( float fLevel )
{
  vxWindowLevel windowLevel( m_pColormap->GetWindowLevel() );
  windowLevel.SetLevel( fLevel );
  m_pColormap->SetWindowLevel( windowLevel );
	m_pColormap->Modified( vxModInfo( vxColormap, true ) );
} // set_Level()


/** 
 * Sets the active range true or false
 * @param bFlag True to enable active range; false for full range.
 */
void Colormap::EnableActiveRange::set( bool bFlag )
{
  if ( m_bIsActiveRange != bFlag )
  {
    GetColormap().SetActiveRange(bFlag);

    m_bIsActiveRange = bFlag;
    m_pColormap->Modified( vxModInfo( vxColormap, vxUtils::COLORMAP_MODIFIED ) );
  }
} // set_EnableActiveRange()


/** 
 * Connects an observer to the object.
 *
 * @param observer vxManagedObserver object interested in events.
 */
void Colormap::Connect( System::IntPtr observer )
{
  m_pColormap->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect()


/** 
 * Disconnects an observer from the object.
 *
 * @param observer vxManagedObserver object interested in events.
 */
void Colormap::Disconnect( System::IntPtr  observer )
{
  m_pColormap->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect()



/** 
 * Receives modified information on connected objects. 
 *
 * @param modifiedList Modified info list.
 */
void Colormap::OnModified( System::IntPtr modifiedList )
{
  GetColormap().SetDirty();

  Changed( this, System::EventArgs::Empty );
} // OnModified()


/**
 * Serializes the colormap object to xml.
 *
 * @return Xml string representing object.
 */
System::String ^ Colormap::ToXml()
{
  try
  {
    // get all the data
    vxColormap * pColormap(ToNativeColormapPtr(GetColormapPtr()));

    // write it
    vxSerializer serializer("Colormap");
    serializer.WriteAttribute("version", "2.0");
    serializer.Write(vxSerializer::AddElement(serializer.GetCurrent(), vxSerializer::GetElementName(*pColormap), "colormap"), *pColormap);

    /// this is only for development purpose. not for release product
    /// ================================== 
    //FILE * fp;
    //fp = fopen("d:/temp/colormap.xml", "wb");

    //if( fp != NULL )
    //{
    //  std::string s = serializer.ToString().c_str();
    //  int n = s.size();
    //  char * ccc = new char[n];
    //  for( int i=0; i<n; i++ )
    //    ccc[i] = s[i];
    //  fwrite(ccc, sizeof(char), n, fp);
    //  fclose(fp);
    //}
    /// ==================================

    return gcnew System::String( serializer.ToString().c_str() );
  }
  catch(ex::VException & e)
  {
    throw gcnew v3DException(&e);
  }
} // ToXml()


/**
 * Creates a new colormap instance from the specified xml string.
 *
 * @param mpXml Xml string to create instance from.
 * @return New Colormap instance.
 */
Colormap ^ Colormap::FromXml( System::String ^ mpXml )
{
  try
  {
    // create new colormap object
    Colormap ^ mpColormap = gcnew Colormap();

    // create xml document and get root element
    vxDOMDocument document = vxDOMDocument::FromXml( ss( mpXml ) );
    vxDOMElement element = document.GetDocumentElement();

    std::string sVersion("1.0");
    vxSerializer::GetAttribute(element, "version", sVersion);

    if ( sVersion == "2.0" )
    {
      // read colormap
      vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(*(mpColormap->m_pColormap)), "colormap"), *(mpColormap->m_pColormap));
    }
    return mpColormap;
  } // try
  catch(ex::VException & e)
  {
    throw gcnew v3DException(&e);
  }
} // FromXml()


/**
 * Creates a new colormap instance from the specified xml assembly resource.
 *
 * @param mpXmlResource Xml resource to create instance from.
 * @return New colormap instance.
 */
Colormap ^ Colormap::FromResource( System::String ^ mpXmlResource )
{
  try
  {
    XmlTextReader xmlReader( System::Reflection::Assembly::GetCallingAssembly()->GetManifestResourceStream( mpXmlResource ) );
    XmlDocument xmlDocument;
    xmlDocument.Load( %xmlReader );

    return Colormap::FromXml( xmlDocument.InnerXml );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // FromResource()


/**
 * Creates a new colormap instance from the specified xml file.
 *
 * @param mpXmlFile Xml file to create instance from.
 * @param mpXml Xml file to create instance from.
 */
Colormap ^ Colormap::FromFile( System::String ^ mpXmlFile )
{
  try
  {
    XmlDocument xmlDocument;
    xmlDocument.Load( mpXmlFile );

    return Colormap::FromXml( xmlDocument.InnerXml );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // FromFile()



// $Log: v3D_Colormap.cpp,v $
// Revision 1.8  2007/02/27 20:41:46  dongqing
// add debug code for writing out the current colormap
// for the viewer to the hard drive d:/temp/colormap.xml
//
// Revision 1.7  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.6  2006/09/27 17:23:48  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.5  2006/09/25 20:13:14  geconomos
// added copy constructor
//
// Revision 1.4  2005/05/24 11:33:10  frank
// serialized the active range
//
// Revision 1.3  2005/04/12 15:23:52  geconomos
// changed xml library to the vxDOM classes
//
// Revision 1.2  2005/03/08 19:09:47  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.1.1.1.6.1  2005/02/17 21:00:21  geconomos
// cleaned up ICloneable
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.39  2004/03/02 01:10:46  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.38.2.1  2004/02/24 20:03:34  michael
// coding standard: assign NULL after deletion
//
// Revision 1.38  2003/12/16 13:25:50  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.37  2003/08/20 14:31:10  michael
// added "mailto:"
//
// Revision 1.36  2003/07/02 15:04:43  geconomos
// Implemented ICloneable
//
// Revision 1.35  2003/06/09 12:29:53  michael
// Implemented a true colormap object that returns discretized colortables based
// on either volume header (taking modality and bits stored) or on the maximum
// index needed -> size = max index + 1
//
// Revision 1.34  2003/06/07 16:18:33  michael
// added additional functionality to support on demand lookup tables
//
// Revision 1.33  2003/06/04 18:29:17  michael
// added namespace declaration
//
// Revision 1.32  2003/05/27 14:20:01  geconomos
// Made GetName and SetName a propery of the class.
//
// Revision 1.31  2003/05/23 18:30:15  michael
// name is now pulled from/set in unmanaged object.
//
// Revision 1.30  2003/05/13 13:28:43  michael
// code review
//
// Revision 1.29  2003/05/06 15:32:10  geconomos
// More coding standards.
//
// Revision 1.28  2003/05/05 17:13:04  geconomos
// Coding standards.
//
// Revision 1.27  2003/04/16 18:53:27  michael
// active range is now default
//
// Revision 1.26  2003/04/16 17:28:18  michael
// active and full range are now working
//
// Revision 1.25  2003/04/16 12:48:55  michael
// added calls to set the active/full range in the vxColormap objects
//
// Revision 1.24  2003/04/01 17:46:04  geconomos
// Coding convention for event naming.
//
// Revision 1.23  2003/03/30 21:05:08  michael
// formatting and cosmetics
//
// Revision 1.22  2002/11/27 16:31:20  geconomos
// Added CopyFrom method
//
// Revision 1.21  2002/11/19 20:38:39  geconomos
// Removed remaining comments for serialization.
//
// Revision 1.20  2002/11/18 20:36:16  geconomos
// Removed xml reminders.
//
// Revision 1.19  2002/11/13 00:11:06  michael
// added inital To/FromXml()
//
// Revision 1.18  2002/11/12 21:31:01  geconomos
// All standard Environment elements now implement IElement.
//
// Revision 1.17  2002/11/11 21:22:53  geconomos
// Changed copy constructor NOT to copy connect clients.
//
// Revision 1.16  2002/11/11 19:19:06  geconomos
// Reworked serialization.
//
// Revision 1.15  2002/11/06 23:53:10  wenli
// Returned System::IntPtr instead of native pointer.
//
// Revision 1.14  2002/10/24 01:30:56  michael
// cosmetics
//
// Revision 1.13  2002/10/22 21:11:40  geconomos
// Added copy constructor from v3D Colormap.
//
// Revision 1.12  2002/10/18 19:17:52  frank
// Got back old colormap serialization/deserialization.
//
// Revision 1.11  2002/10/17 15:28:15  geconomos
// Added constructor from native type and created CommonInit() method
//
// Revision 1.10  2002/10/14 16:03:32  michael
// commented out old serialization because it does not account for the const
// of certain objects
//
// Revision 1.9  2002/10/08 19:02:30  michael
// added event handler to get the colormap GUI updated
//
// Revision 1.8  2002/10/02 21:51:46  michael
// Added modified call after deserialing object from XML
//
// Revision 1.7  2002/10/02 16:45:46  frank
// Fixed FromResource()
//
// Revision 1.6  2002/10/01 23:04:23  frank
// Adding FromResource function.  Needs more work.
//
// Revision 1.5  2002/10/01 01:30:44  dmitry
// Issue 2717: Version numbers matched-up in serialization. Validation check added for Colormap element.
//
// Revision 1.4  2002/09/25 20:37:02  michael
// Completed vxTransferFunction to vxColormap transition
//
// Revision 1.3  2002/09/23 17:04:33  michael
// fxi for reading version 1.1(deserialize)
//
// Revision 1.1  2002/09/20 22:16:03  michael
// Added colormap wrapper class
//
// Revision 1.11  2002/09/19 14:26:00  michael
// completed transition from vxTransferFunction to vxColormap
//
// Revision 1.10  2002/09/18 22:27:28  frank
// removed debug messages
//
// Revision 1.9  2002/09/18 21:20:59  frank
// Fixed transfer function deserialization.
//
// Revision 1.8  2002/09/17 14:37:12  geconomos
// Removed property AlphaFromWindow
//
// Revision 1.7  2002/09/17 12:58:07  michael
// removed all the old and redundant code
//
// Revision 1.6  2002/09/10 19:43:58  geconomos
// Fixed compile error.
//
// Revision 1.5  2002/08/29 20:30:20  dmitry
// Disconnect removed because of GC.
//
// Revision 1.4  2002/08/28 21:47:40  dmitry
// Window Level set on the tfunc.
//
// Revision 1.3  2002/08/22 23:58:48  dmitry
// Serialization implemented.
//
// Revision 1.2  2002/08/02 13:53:31  dmitry
// Color List added.
//
// Revision 1.1  2002/07/31 14:10:29  dmitry
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Colormap.cpp,v 1.8 2007/02/27 20:41:46 dongqing Exp $
// $Id: v3D_Colormap.cpp,v 1.8 2007/02/27 20:41:46 dongqing Exp $
