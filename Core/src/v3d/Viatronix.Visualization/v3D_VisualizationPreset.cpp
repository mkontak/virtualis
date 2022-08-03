// $Id: v3D_VisualizationPreset.cpp,v 1.12 2007/06/07 18:04:38 romy Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (mailto:frank@viatronix.com)


// includes
#include "StdAfx.h"
#include "v3d_VisualizationPreset.h"
#include "vxManagedObserver.h"
#include "v3D_Colormap.h"
#include "vxUtils.h"
#include "vxSerializer.h"


// namespaces
using namespace Viatronix::Visualization;
using namespace System;
using namespace System::Xml;
using namespace std;


/**
 * Initializes a Colormap object with default values.
 */
VisualizationPreset::VisualizationPreset()
{
  m_pObserver = new vxManagedObserver( this );
  m_pVisualizationPreset = new vxShareableObject< vxVisualizationPreset >();
  m_pVisualizationPreset->Connect( m_pObserver );

  this->Colormap      = gcnew Viatronix::Visualization::Colormap();
  this->RenderingMode = gcnew Viatronix::Visualization::RenderingMode();
} // VisualizationPreset()


///**
// * Initializer from vxVisualizationPreset. Class assumes ownership of unmanaged class.
// *
// * @param pVisualizationPreset Unmananged vxVisualizationPreset implementation.
// */
//VisualizationPreset::VisualizationPreset( vxVisualizationPreset * pVisualizationPreset )
//{
//  m_pObserver = new vxManagedObserver( this );
//  m_pVisualizationPreset = new vxShareableObject< vxVisualizationPreset >( * pVisualizationPreset );
//  m_pVisualizationPreset->Connect( m_pObserver );
//
//  this->Colormap      = new Viatronix::Visualization::Colormap( m_pVisualizationPreset->GetColormap() );
//  this->RenderingMode = new Viatronix::Visualization::RenderingMode( m_pVisualizationPreset->GetRenderingMode() );
//} // VisualizationPreset()


///**
// * Initializer from VisualizationPreset object.
// *
// * @param mpVisualizationPreset VisualizationPreset object to copy settings from.
// */
//VisualizationPreset::VisualizationPreset( VisualizationPreset ^ mpVisualizationPreset )
//{
//  m_pObserver = new vxManagedObserver( this );
//  m_pVisualizationPreset = new vxShareableObject< vxVisualizationPreset >();
//  m_pVisualizationPreset->Connect( m_pObserver );
//
//  * m_pVisualizationPreset = * static_cast< vxVisualizationPreset * > ( ToNativeVisualizationPresetPtr( mpVisualizationPreset->GetElementPtr() ) );
//} // VisualizationPreset()


/**
 * Creates a new object that is a copy of the current instance.
 *
 * @return  A new object that is a copy of this instance.
 */
System::Object ^ VisualizationPreset::Clone()
{
  // create a new instance of the VisualizationPreset class
  VisualizationPreset ^ mpVisualizationPreset = gcnew VisualizationPreset();
  
  // get the underlying implementation pointer  
  vxVisualizationPreset * pVisualizationPreset = dynamic_cast< vxVisualizationPreset * >( mpVisualizationPreset->m_pVisualizationPreset );

  // make a copy of the underying implementations
  *pVisualizationPreset = * dynamic_cast< vxVisualizationPreset * >( m_pVisualizationPreset );

  return mpVisualizationPreset;
} // Clone()


/**
 * Initialize VisualizationPreset from other.
 *
 * @param mpVisualizationPreset VisualizationPreset object to copy settings from.
 */
void VisualizationPreset::CopyFrom( VisualizationPreset ^ mpVisualizationPreset )
{
  * m_pVisualizationPreset = * reinterpret_cast< vxVisualizationPreset * > ( ToNativeVisualizationPresetPtr( mpVisualizationPreset->GetElementPtr() ) );

  m_pVisualizationPreset->Modified( vxModInfo( vxVisualizationPreset, vxUtils::PRESET_MODIFIED ) );
} // CopyFrom()


/**
 * Destructor
 */
VisualizationPreset::~VisualizationPreset()
{
  this->!VisualizationPreset();
} // ~VisualizationPreset()


/**
 * Destructor
 */
VisualizationPreset::!VisualizationPreset()
{
  if ( m_pVisualizationPreset != NULL )
  {
    m_pVisualizationPreset->Disconnect( m_pObserver );
    delete m_pVisualizationPreset; 
    m_pVisualizationPreset = NULL;
  }
  
  if(  m_pObserver )
  {
    delete m_pObserver;           
    m_pObserver = NULL;
  }
} // !VisualizationPreset()


/**
 * Sets the associated name.
 *
 * @param mpName  preset name
 */
void VisualizationPreset::Name::set( System::String ^ mpName ) 
{ 
  string sName = ss( mpName );
  m_pVisualizationPreset->SetName( sName );
} // set_Name()


/**
 * Sets the associated preset image file name.
 *
 * @param mpFileName  preset image file name
 */
void VisualizationPreset::PresetImageFile::set( System::String ^ mpFileName ) 
{ 
  string sFileName = ss( mpFileName );
  m_pVisualizationPreset->SetPresetImageFile( sFileName );
} // set_PresetImageFile()





/**
 * Sets the associated colormap
 *
 * @param mpColormap the new colormap
 */
void VisualizationPreset::Colormap::set( Viatronix::Visualization::Colormap ^ mpColormap )
{
  try
  {
    if ( m_mpColormap != nullptr ) m_mpColormap->Disconnect( System::IntPtr( m_pObserver ) );
    m_mpColormap = mpColormap;
    if ( m_mpColormap != nullptr ) m_mpColormap->Connect( System::IntPtr( m_pObserver ) );

    m_pVisualizationPreset->SetColormap( ToNativeColormapPtr( m_mpColormap->GetElementPtr() ) );

    ToNativeColormapPtr( m_mpColormap->GetColormapPtr() )->Modified( vxModInfo( vxColormap, vxUtils::COLORMAP_MODIFIED ) );
    m_pVisualizationPreset->Modified( vxModInfo( vxVisualizationPreset, vxUtils::PRESET_MODIFIED ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( & e );
  }
} // set_Colormap()


/**
 * Gets the associated colormap
 *
 * @return the desired colormap
 */
Viatronix::Visualization::Colormap ^ VisualizationPreset::Colormap::get()
{
  return m_mpColormap;
} // get_Colormap()


/**
 * Sets the associated rendering mode
 *
 * @param mpRenderingMode the new rendering mode
 */
void VisualizationPreset::RenderingMode::set( Viatronix::Visualization::RenderingMode ^ mpRenderingMode )
{
  try
  {
    if ( m_mpRenderingMode != nullptr ) m_mpRenderingMode->Disconnect( System::IntPtr( m_pObserver ) );
    m_mpRenderingMode = mpRenderingMode;
    if ( m_mpRenderingMode != nullptr ) m_mpRenderingMode->Connect( System::IntPtr( m_pObserver ) );

    m_pVisualizationPreset->SetRenderingMode( ToNativeRenderingModePtr( m_mpRenderingMode->GetElementPtr() ) );

    ToNativeRenderingModePtr( m_mpRenderingMode->GetElementPtr() )->Modified( vxModInfo( vxRenderingMode, 0 ) );
    m_pVisualizationPreset->Modified( vxModInfo( vxVisualizationPreset, vxUtils::PRESET_MODIFIED ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( & e );
  }

} // set_RenderingMode()


/**
 * Gets the associated rendering mode
 *
 * @return the desired rendering mode
 */
Viatronix::Visualization::RenderingMode ^ VisualizationPreset::RenderingMode::get()
{
  return m_mpRenderingMode;
} // get_RenderingMode()


/** 
 * Connects an observer to the object.
 *
 * @param observer vxManagedObserver object interested in events.
 */
void VisualizationPreset::Connect( System::IntPtr observer )
{
  m_pVisualizationPreset->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect()


/** 
 * Disconnects an observer from the object.
 *
 * @param observer vxManagedObserver object interested in events.
 */
void VisualizationPreset::Disconnect( System::IntPtr  observer )
{
  m_pVisualizationPreset->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect()



/** 
 * Receives modified information on connected objects. 
 *
 * @param modifiedList Modified info list.
 */
void VisualizationPreset::OnModified( System::IntPtr modifiedList )
{
  Changed( this, System::EventArgs::Empty );
} // OnModified()


/**
* Serializes the VisualizationPreset object to xml.
*
* @return Xml string representing object.
*/
System::String ^ VisualizationPreset::ToXml()
{
  try
  {
    // get all the data
    vxVisualizationPreset * pVisualizationPreset( ToNativeVisualizationPresetPtr( GetElementPtr() ) );
    vxSerializer serializer( "vxVisualizationPreset" );
    serializer.WriteAttribute( "version", "1.0" );
    serializer.Write(serializer.GetDocument().GetDocumentElement(), *pVisualizationPreset);
    return gcnew System::String( serializer.ToString().c_str() );
  } // try
  catch( ex::VException & e )
  {
    throw gcnew v3DException( & e );
  }
} // ToXml()


/**
* Creates a new VisualizationPreset instance from the specified xml string.
*
* @param mpXml Xml string to create instance from.
* @return New VisualizationPreset instance.
*/
VisualizationPreset ^ VisualizationPreset::FromXml( System::String ^ mpXml )
{
  bool bSuccess(false);

  try
  { 
    // create new preset object
    VisualizationPreset ^ mpVisualizationPreset = gcnew VisualizationPreset();
    vxVisualizationPreset * pVisPreset = new vxVisualizationPreset;

    // create xml document and get root element
    vxDOMDocument document = vxDOMDocument::FromXml( ss( mpXml ) );
    vxDOMElement element( document.GetDocumentElement() );

    std::string sVersion( "0.1" );
    vxSerializer::GetAttribute( element, "version", sVersion );

    if ( sVersion == "1.0" )
    {
      bSuccess = vxSerializer::Read(element, *pVisPreset);

      *mpVisualizationPreset->m_pVisualizationPreset = *pVisPreset;
      return mpVisualizationPreset;
    }
    else
      return nullptr;
  } // try
  catch ( ex::VException & e )
  {
    throw gcnew v3DException( & e );
  }
} // FromXml()


/**
 * Creates a new VisualizationPreset instance from the specified xml assembly resource.
 *
 * @param mpXmlResource Xml resource to create instance from.
 *
 * @return New VisualizationPreset instance.
 */
VisualizationPreset ^ VisualizationPreset::FromResource( System::String ^ mpXmlResource )
{
  try
  {
    XmlTextReader xmlReader( System::Reflection::Assembly::GetCallingAssembly()->GetManifestResourceStream( mpXmlResource ) );
    XmlDocument xmlDocument;
    xmlDocument.Load( %xmlReader );

    return VisualizationPreset::FromXml( xmlDocument.InnerXml );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // FromResource()


/**
 * Creates a new VisualizationPreset instance from the specified xml file.
 *
 * @param mpXmlFile Xml file to create instance from.
 * @param mpXml Xml file to create instance from.
 */
VisualizationPreset ^ VisualizationPreset::FromFile( System::String ^ mpXmlFile )
{
  try
  {
    XmlDocument xmlDocument;
    xmlDocument.Load( mpXmlFile );

    return VisualizationPreset::FromXml( xmlDocument.InnerXml );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( & e );
  }
} // FromFile()


/**
 * gets the background color
 *
 * @return the background color
 */
System::Drawing::Color VisualizationPreset::BackgroundColor::get()
{
  try
  {

    // get the color from viz preset (range of 0-1)
    vxVisualizationPreset * pVisualizationPreset( ToNativeVisualizationPresetPtr( GetElementPtr() ) );
    PixelRGB<float4> fColor( pVisualizationPreset->GetBackgroundColor() );

    // convert to range 0-255
    uint1 uRed  ( static_cast< uint1 >( Bound( 0.0F, fColor.R() * 255.0F, 255.0F ) ) );
    uint1 uGreen( static_cast< uint1 >( Bound( 0.0F, fColor.G() * 255.0F, 255.0F ) ) );
    uint1 uBlue ( static_cast< uint1 >( Bound( 0.0F, fColor.B() * 255.0F, 255.0F ) ) );

    return System::Drawing::Color::FromArgb( uRed, uGreen, uBlue );

  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( & e );
  }
} // get_BackgroundColor()


/**
 * sets the background color 
 *
 * @param color the new background color
 */
void VisualizationPreset::BackgroundColor::set( System::Drawing::Color color )
{
  try
  {

    // get the color from viz preset (range of 0-1)
    vxVisualizationPreset * pVisualizationPreset( ToNativeVisualizationPresetPtr( GetElementPtr() ) );
    pVisualizationPreset->SetBackgroundColor( PixelRGB<float4>( color.R / 255.0F, color.G / 255.0F, color.B / 255.0F ) );

  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( & e );
  }
} // set_BackgroundColor()


// $Log: v3D_VisualizationPreset.cpp,v $
// Revision 1.12  2007/06/07 18:04:38  romy
// ToXml VizualizationPreset type was wrong
//
// Revision 1.11  2007/03/23 15:37:17  romy
// Added PresetImageFile method
//
// Revision 1.10  2006/10/04 18:29:06  gdavidson
// Ported to C++/CLI
//
// Revision 1.9  2006/09/27 17:23:49  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.8  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.7  2005/11/22 21:17:21  geconomos
// added sanity checks in destructor before deleteing  instance variables
//
// Revision 1.6  2005/04/12 15:23:52  geconomos
// changed xml library to the vxDOM classes
//
// Revision 1.5  2005/03/08 19:09:47  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.4.2.2  2005/02/17 20:59:45  geconomos
// added the ownership of colormaps and rendering modes (will revisit)
//
// Revision 1.4.2.1  2005/02/10 20:57:24  michael
// Introduced vxCameraPreset with BoundingBox, name (to be displayed in GUI),
// and new Preset xml version (2.1). Converted presets, except for PET and XA
// ones. Added V3D Utils function and added a call to it to adjust the camera
// relative to the bounding box of the (camera) preset once it is assigned from
// the preset to the environment camera object (CopyFrom()).
//
// Revision 1.4  2004/12/10 14:18:53  frank
// extended to include the colors
//
// Revision 1.3  2004/05/17 19:07:23  frank
// moved visualization preset serialization to its own class
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
// Revision 1.3  2004/03/02 01:10:42  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.2.2.1  2004/02/24 20:12:53  michael
// took out code that was commented out
//
// Revision 1.2  2003/12/16 13:27:52  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.1  2003/08/05 13:35:06  frank
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_VisualizationPreset.cpp,v 1.12 2007/06/07 18:04:38 romy Exp $
// $Id: v3D_VisualizationPreset.cpp,v 1.12 2007/06/07 18:04:38 romy Exp $
