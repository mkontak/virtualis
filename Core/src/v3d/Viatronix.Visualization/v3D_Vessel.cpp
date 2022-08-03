// $Id: v3D_Vessel.cpp,v 1.3.8.1 2009/01/16 21:53:40 kchalupa Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.com)

/**
 * Interface between vxVessel and v3D Explorer.
 */

// includes
#include "StdAfx.h"
#include "v3d_Vessel.h"
#include "vxManagedObserver.h"
#include "vxSerializer.h"
#include "vxVessel.h"


// namespaces
USING_VISUALIZATION_NS


// defines
#define FILE_REVISION "$Revision: 1.3.8.1 $"


/**
 * constructor
 */
Vessel::Vessel()
{
  m_pVessel = new vxShareableObject< vxVessel >();
}


/**
 * destructor
 */
Vessel::~Vessel()
{
  this->!Vessel();
}


/**
 * finalizer
 */
Vessel::!Vessel()
{
  delete m_pVessel;
  m_pVessel = NULL;
}


/**
 * Implements the IObserver interface.
 */
void Vessel::OnModified( System::IntPtr ptrInfo )
{
} // OnModified()


/** 
 * Connects an observer to the object.
 */
void Vessel::Connect( System::IntPtr observer )
{
  m_pVessel->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect()


/** 
 * Disconnects an observer from the object.
 */
void Vessel::Disconnect( System::IntPtr  observer )
{
  m_pVessel->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect()


/**
 * Get the vessel label.
 */
long Vessel::Label::get()
{
  try
  {
    return m_pVessel->GetLabel();
  }
  catch ( ex::VException & e )
  {
    throw gcnew v3DException( & e );
  }
} // get_Label()


/**
 * serializes the vessel instance to xml
 *
 * @return the vessel as xml
 */
System::String ^ Vessel::ToXml()
{
  try
  {
    // get all the data
    vxVessel * pVessel( ToNativeVesselPtr( GetElementPtr() ) );

    // write it
    vxSerializer serializer( "Vessel" );
    serializer.WriteAttribute( "version", "1.0" );
    serializer.Write(vxSerializer::AddElement( serializer.GetCurrent(), vxSerializer::GetElementName( * pVessel ), "vessel" ), * pVessel );
    return gcnew System::String( serializer.ToString().c_str() );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( & e );
  }
} // ToXml()


/**
 * creates a new vessel instance from the specified xml string
 *
 * @param mpXml the xml string
 *
 * @return a new vessel instance
 */
Vessel ^ Vessel::FromXml( System::String ^ mpXml )
{
  try
  {
    // create xml document and get root element
    vxDOMDocument document = vxDOMDocument::FromXml( ss( mpXml ) );
    vxDOMElement element = document.GetDocumentElement();

    std::string sVersion( "0.0" );
    vxSerializer::GetAttribute( element, "version", sVersion );

    if ( sVersion == "1.0" )
    {
      // read vessel
      Vessel ^ mpVessel( gcnew Vessel() );
      vxVessel & vessel( * ToNativeVesselPtr( mpVessel->GetElementPtr() ) );
      vxSerializer::Read( vxSerializer::FindElement( element, vxSerializer::GetElementName( vessel ), "vessel" ), vessel );
      LogDbg( "read a vessel with " + ToAscii( vessel.size() ) + " points", "Vessel", "FromXml" );
      return mpVessel;
    }
    return gcnew Vessel();
  } // try
  catch( ex::VException & e )
  {
    throw gcnew v3DException( & e );
  }
} // FromXml()


/**
 * Import from a simple text file ( <numPoints> \n x y z \n x y z \n ... ).
 * The file contains the number of points (in ASCII) on the first line and
 * the x y z values separated by whitespace for each centerline position on the following lines
 *
 * @param mpFilename the input file name
 *
 * @return a new vessel
 */
Vessel ^ Vessel::FromTextFile( System::String ^ mpFilename )
{

  // check that the file exists
  if ( ! System::IO::File::Exists( mpFilename ) )
  {
    throw gcnew System::ArgumentException( "unknown file: {0}", mpFilename );
  }

  // read the file
  System::IO::TextReader ^ mpReader( gcnew System::IO::StreamReader( gcnew System::IO::FileStream( mpFilename, System::IO::FileMode::Open ) ) );
  System::String ^ mpAllLines = mpReader->ReadToEnd();
  mpReader->Close();

  // parse the file
  array< System::Char > ^ mpLineSplits = gcnew array< System::Char > { '\n' };
  array< System::String ^ > ^ mpLines = mpAllLines->Split( mpLineSplits );
  int numPoints = System::Int32::Parse(  mpLines[0] );

  // create the vessel
  Vessel ^ mpNewVessel = gcnew Vessel();
  vxVessel * pVessel = reinterpret_cast< vxVessel * >( ToNativeVesselPtr( mpNewVessel->GetElementPtr() ) );
  for ( int4 i(0); i<numPoints; ++i )
  {
    array< System::String ^ > ^ mpXYZ = mpLines[i+1]->Split( nullptr );
    float4 fX = System::Xml::XmlConvert::ToSingle( mpXYZ[0] );
    float4 fY = System::Xml::XmlConvert::ToSingle( mpXYZ[1] );
    float4 fZ = System::Xml::XmlConvert::ToSingle( mpXYZ[2] );

    // store the centerline point in a vessel section
    vxVesselSection section;
    section.SetPosition( Point3Df( fX, fY, fZ ) );
    section.SetTanDirection( Normal3Df( 0.0F, 1.0F, 1.0F ) );
    section.SetUpDirection( Normal3Df( 1.0F, 0.0F, 1.0F ) );

    // add the section to the vessel
    pVessel->push_back( section );
    pVessel->SetMeasurementDomain( pVessel->GetEntireDomain() );
    const float4 fArcLengthMM( 20 );
    pVessel->EstimateTangents( fArcLengthMM );
    pVessel->RecalculateLengths();

  }

  return mpNewVessel;

} // FromTextFile()


// undefines
#undef FILE_REVISION


// $Log: v3D_Vessel.cpp,v $
// Revision 1.3.8.1  2009/01/16 21:53:40  kchalupa
// Issue # 6222; Cardiac Regional Localization Crashproofing.
//
// Revision 1.3  2006/10/04 18:29:06  gdavidson
// Ported to C++/CLI
//
// Revision 1.2  2006/09/27 17:23:49  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.1  2006/09/27 17:09:54  geconomos
// changed file extension to .cpp
//
// Revision 1.5  2005/11/18 21:12:28  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.4.2.2  2005/07/18 18:00:52  frank
// fixed vessel reading
//
// Revision 1.4.2.1  2005/07/18 13:51:02  frank
// added importing from a text file
//
// Revision 1.4  2005/06/07 19:01:20  vxconfig
// removed the get/set name property of vessels
//
// Revision 1.3  2005/04/12 15:23:52  geconomos
// changed xml library to the vxDOM classes
//
// Revision 1.2  2004/09/23 15:32:25  frank
// implemented to/from xml
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2004/03/02 01:10:42  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.3.2.1  2004/02/24 20:10:42  michael
// took out code that was commented out
//
// Revision 1.3  2003/12/16 13:27:00  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.2  2003/08/22 19:22:02  frank
// Added IObserver implementation
//
// Revision 1.1  2003/08/12 15:47:31  frank
// Moved the vessel data structure down to vxBase
//
// Revision 1.2  2003/05/13 12:31:51  frank
// updated the namespace declarations for the official project name
//
// Revision 1.1  2003/05/08 19:59:25  geconomos
// Moved from Viatronix.Viewers.Explorer.Plugins.CTA
//
// Revision 1.4  2003/01/14 16:17:54  dongqing
// Code review
//
// Revision 1.3  2002/11/21 02:43:09  frank
// Added a property to get the label
//
// Revision 1.2  2002/11/04 20:07:22  frank
// Added get and set Name properties
//
// Revision 1.1  2002/10/31 12:46:52  geconomos
// Moved from Viatronix.Visualization.Ext.Vascular
//
// Revision 1.7  2002/10/25 01:51:48  frank
// Moved working vessel into the VesselArray
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Vessel.cpp,v 1.3.8.1 2009/01/16 21:53:40 kchalupa Exp $
// $Id: v3D_Vessel.cpp,v 1.3.8.1 2009/01/16 21:53:40 kchalupa Exp $
