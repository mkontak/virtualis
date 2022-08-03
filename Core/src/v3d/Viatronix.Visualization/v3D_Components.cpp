// $Id: v3D_Components.cpp,v 1.24 2008/04/18 21:10:24 dongqing Exp $
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
#include "v3d_components.h"
#include "vxManagedObserver.h"
#include "v3D_EventArgs.h"
#include "v3D_Volume.h"
#include "vxMouseCursor.h"
#include "vxSerializer.h"
#include "vxUtils.h"
#include "vxUndoActionSegmentation.h"
#include <queue>


// namespaces
USING_VISUALIZATION_NS
using namespace System;
using namespace System::Xml;
using namespace System::Text::RegularExpressions;
using namespace std;


// defines
#define FILE_REVISION "$Revision: 1.24 $"


/**
 * Initializes a Components object with default values.
 */
Components::Components()
{
  m_pObserver = new vxManagedObserver ( this );

  m_pvComponents = new vxShareableObject< vxComponentArray >();
  m_pvComponents->Connect(m_pObserver);
  m_pComponentHandler= new ComponentHandler();
  m_pComponentHandler->SetComponentsPtr( m_pvComponents );
} // Components()


/**
 * Initializes a Components object from the specified one.
 *
 * @param mpComponents Components object to copy settings from.
 */
Components::Components( Components ^ mpComponents )
{
  m_pObserver = new vxManagedObserver ( this );

  m_pvComponents = new vxShareableObject< vxComponentArray >( *dynamic_cast<vxComponentArray*>( ToNativeComponentsPtr( mpComponents->GetElementPtr() ) ) );
  m_pvComponents->Connect( m_pObserver );
  
  m_pComponentHandler = new ComponentHandler();
  m_pComponentHandler->SetComponentsPtr( m_pvComponents );
  m_pComponentHandler->Initialize();
} // Components( mpComponents )


/**
 * Initializes a Components object from the specified vxComponentArray. 
 * Object assumes ownership of unamanaged class.
 *
 * @param pComponentArray Unmananged vxComponentArray implementation.
 */
Components::Components( System::IntPtr componentArray )
{
  m_pObserver = new vxManagedObserver ( this );

  m_pvComponents = new vxShareableObject< vxComponentArray >( * static_cast< vxComponentArray * >( componentArray.ToPointer() ) );
  m_pvComponents->Connect( m_pObserver );
  
  m_pComponentHandler = new ComponentHandler();
  m_pComponentHandler->SetComponentsPtr( m_pvComponents );
} // Components( pComponentArray )


/**
 * Destructor
 */
Components::~Components()
{
  this->!Components();

} // ~Components()


/**
 * Finalizer
 */
Components::!Components()
{
  if ( m_pvComponents != NULL )
  {
    delete m_pvComponents;
    m_pvComponents = NULL;
  }

  if ( m_pComponentHandler != NULL )
  {
    delete m_pComponentHandler;
    m_pComponentHandler = NULL;
  }

  if ( m_pObserver != NULL )
  {
    delete m_pObserver;       
    m_pObserver = NULL;
  }
} // !Components()


/*
 *Starts the Composite Undo
 */
void Components::StartCompositeUndo()
{
	vxEnvironment::GetUndoActions().PushAction( new vxUndoActionCompositeBegin());
}//StartCompositeUndo()


/*
 *Ends the Composite Undo
 */
void Components::EndCompositeUndo()
{
	vxEnvironment::GetUndoActions().PushAction( new vxUndoActionCompositeEnd());
}//EndCompositeUndo



/**
 * Set a restore point
 */
void Components::SetRestorePoint()
{

  vxEnvironment::GetUndoActions().PushAction( new vxUndoActionSegmentation(
      ToNativeVolumeUint2Ptr( m_mpLabelVolume->GetElementPtr() ),
      m_pvComponents ) );

} // SetRestorePoint()


/**
 * Deletes a component.
 *
 * @param iIndex Component's history index.
 */
void Components::DeleteComponent( long iIndex )
{
  try
  {
    vxMouseCursor cursor( "CURSOR_WAIT" );

    const uint2 uLabel = ( * m_pvComponents )[ iIndex ]->GetLabel();

    // clear the component from the label volume
    vxBlockVolumeIterator<uint2> labelIter( * ToNativeVolumeUint2Ptr( m_mpLabelVolume->GetElementPtr() ) );
    for( labelIter.SetPos(0,0,0); labelIter.IsNotAtEnd(); labelIter.NextBlockZYX() )
    {
      for ( ; labelIter.IsNotAtEndOfBlock(); labelIter.NextZYXinsideBlock() )
      {
        if ( labelIter.GetVoxel() == uLabel )
          labelIter.SetVoxel( 0 );
      }
    } // for
   
    //Rasie the removing event
    Removing( iIndex , System::EventArgs::Empty );
  
    // remove it from the component array
    m_pvComponents->RemoveComponent( iIndex );

    // update the max component index
    int4 iMaxComponentIndex( max( 0, m_pComponentHandler->GetComponentsPtr()->GetMaxLabel() ) );

    //m_pComponentHandler->DeleteComp(iIndex);

    _native_ptr_t<vxBlockVolume<uint2> > spLabelVolume( m_mpLabelVolume->GetVolumePtr() );
    
    spLabelVolume->Modified( vxModInfo( vxBlockVolume<uint2>, vxUtils::VOLUME_LABEL_MODIFIED ) );
    
    // ensure that a valid component is selected
    if ( m_pvComponents->GetSelectedComponentIndex() >= m_pvComponents->GetCount() )
    {
      m_pvComponents->SetSelectedComponentIndex( m_pvComponents->GetCount() -1 );
    }
    m_pvComponents->Modified( vxModIndInfo( vxComponentArray, iIndex, vxUtils::COMPONENTARRAY_REMOVED ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // DeleteComponent()



/*
 *Compacts the holes 
 */
void Components::Compact()
{
	m_pvComponents->Compact();
}//Compact()

/**
 * Resets the the collection to a default state with one component.
 */
void Components::Reset()
{
  try
  {
    vxMouseCursor cursor("CURSOR_WAIT");
    
    m_pComponentHandler->DeleteAllComps();
    m_pvComponents->ArrayByLabel()[0]->SetIntensityVisible( true );
    m_pvComponents->Modified(vxModIndInfo(vxComponentArray, 0, vxUtils::COMPONENTARRAY_REMOVED));
    
    _native_ptr_t< vxBlockVolume<uint2> > spLabelVolume( m_mpLabelVolume->GetVolumePtr() );
    
    spLabelVolume->Modified( vxModInfo( vxBlockVolume<uint2>, vxUtils::VOLUME_LABEL_MODIFIED ) );
    m_pvComponents->SetSelectedComponentIndex( 0 );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // Reset()


/**
 * Updates components on user inputs
 */
void Components::UpdateOnUserInput()
{
  try
  {
    vxMouseCursor cursor("CURSOR_WAIT");
    m_pComponentHandler->OnUserInputUpdate();
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // UpdateOnUserInput();


/**
 * Initializes the component handler.
 */
void Components::Initialize()
{ 
  try
  {
    m_pComponentHandler->Initialize();
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // Initialize()


/**
 * Get the currently selected component.
 *
 * @return index of selected component; -1 if none is selected
 */
int Components::SelectedItem::get()
{    
  return m_pvComponents->GetSelectedComponentIndex();
} // Selected::get()


/**
 * Gets the components name
 *
 * @param iIndex Component's history index.
 * @return Component's name.
 */
System::String ^ Components::GetName( long iIndex )
{
  try
  {
    return gcnew System::String(( *m_pvComponents )[ iIndex ]->GetName().c_str() );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // GetName()


/**
 * Sets the component's name.
 *
 * @param iIndex Component's history index.
 * @param name component name
 */
void Components::SetName( long iIndex, System::String ^ mpsName )
{ 
  try
  {
    (*m_pvComponents)[iIndex]->SetName( ss( mpsName ) );
    m_pvComponents->Modified( vxModIndInfo( vxComponentArray, iIndex, vxUtils::COMPONENTARRAY_NAME_MODIFIED ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // SetName()


/**
 * Gets the specified component's color.
 *
 * @param iIndex Component's history index.
 * @return Component's color.
 */
System::Drawing::Color Components::GetColor( long iIndex )
{
  try
  {
    PixelRGBA<uint1> tmp = ( *m_pvComponents )[ iIndex ]->GetColor();
    return System::Drawing::Color::FromArgb( tmp.A(), tmp.R(), tmp.G(), tmp.B() );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // GetColor()


/**
 * Sets the specified component's color
 *
 * @param iIndex Component's history index.
 * @param color Component's color.
 */
void Components::SetColor( long iIndex, System::Drawing::Color color )
{
  try
  {
    PixelRGBA<uint1> newColor( color.R, color.G, color.B, color.A );

    (*m_pvComponents)[iIndex]->SetColor( newColor );
    m_pvComponents->Modified( vxModIndInfo( vxComponentArray, iIndex, vxUtils::COMPONENTARRAY_MODIFIED ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // SetColor()


/**
 * Gets the component's label visibility flag.
 *
 * @param iIndex Component's history index.
 * @return True if label is visible; false otherwise.
 */
bool Components::GetLabelVisible( long iIndex )
{
  try
  {
    return ( *m_pvComponents )[ iIndex]->IsLabelVisible();
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // GetLabelVisible()


/**
 * Sets the specified component's label visible flag.
 *
 * @param iIndex Component's history index.
 * @param bVisible Component's visibility flag.
 */
void Components::SetLabelVisible( long iIndex, bool bVisible )
{
  try
  {
    (*m_pvComponents)[ iIndex ]->SetLabelVisible( bVisible );
    m_pvComponents->Modified( vxModIndInfo( vxComponentArray, iIndex, vxUtils::COMPONENTARRAY_MODIFIED ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // SetLabelVisible()


/**
 * Gets the component's label index.
 *
 * @param iIndex Component's history index.
 * @return Component's label index.
 */
int Components::GetLabelIndex( long iIndex )
{
  try
  {
    return ( *m_pvComponents )[ iIndex ]->GetLabel();
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // GetLabelIndex()


/**
 * gets the component's intensity visibility flag.
 *
 * @param iIndex Component's history index.
 * @return True if intensity is visible; false otherwise.
 */
bool Components::GetIntensityVisible( long iIndex )
{
  try
  {
    return ( *m_pvComponents )[ iIndex ]->IsIntensityVisible();
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // GetDensityVisible()


/**
 * Sets the specified component's density visible flag.
 *
 * @param iIndex Component's history index.
 * @param bVisible Component's intensity visibility flag.
 */
void Components::SetItensityVisible( long iIndex, bool bVisible )
{
  try
  {
    ( *m_pvComponents )[ iIndex ]->SetIntensityVisible( bVisible );
    m_pvComponents->Modified( vxModIndInfo( vxComponentArray, iIndex, vxUtils::COMPONENTARRAY_MODIFIED ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // SetDensityVisible()


/**
 * Gets the component's locked flag.
 *
 * @param iIndex Component's history index.
 * @return True if component is locked; false otherwise.
 */
bool Components::GetLocked( long iIndex )
{
  try
  {
    return ( *m_pvComponents )[ iIndex ]->IsLocked();
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // GetLocked()


/**
 * Sets the specified component's locked flag.
 *
 * @param iIndex Component's history index.
 * @param bLocked Component's locked flag.
 */
void Components::SetLocked( long iIndex, bool bLocked )
{
  try
  {
    ( *m_pvComponents )[ iIndex ]->SetLocked( bLocked );
    m_pComponentHandler->OnUserInputUpdate();
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // SetLocked()


/**
 * Gets the selected component.
 *
 * @return History index of selected component; 0 if none is selected.
 */
long Components::GetSelected()
{
  try
  {
    return m_pvComponents->GetSelectedComponentIndex();
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // GetSelected()


/**
 * Sets the specified component's selected flag.
 *
 * @param iIndex Component's history index.
 */
void Components::SetSelected( long iIndex )
{
  try
  {
    // this is a "hacked" solution because on RemoveAll() false is set on the last selected but the components are already gone
    if ( iIndex <= m_pvComponents->GetCount() - 1 )
    {
      m_pvComponents->SetSelectedComponentIndex( iIndex );
      m_pvComponents->Modified( vxModIndInfo( vxComponentArray, iIndex, vxUtils::COMPONENTARRAY_SELECTED ) );
    }
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // SetSelected()


/**
 * Gets if the specified component is segmented
 * @param history index of component
 * @return True if component is segmented; false otherwise.
 */
bool Components::GetSegmented( long iIndex )
{
  try
  {
    return m_pComponentHandler->IsComponentSegmented( iIndex );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // GetSegmented()


/**
 * gets the specified component's removable status
 *
 * @param iIndex the label index
 *
 * @return removable status
 */
bool Components::GetRemovable( long iIndex )
{
  try
  {
    return ( * m_pvComponents )[ iIndex ]->GetRemovable();
  }
  catch ( ex::VException & e )
  {
    throw gcnew v3DException( & e );
  }
} // GetRemovable()


/**
 * gets the specified component's growable status
 *
 * @param iIndex the label index
 *
 * @return growable status
 */
bool Components::GetGrowable( long iIndex )
{
  try
  {
    return ( * m_pvComponents )[ iIndex ]->GetGrowable();
  }
  catch ( ex::VException & e )
  {
    throw gcnew v3DException( & e );
  }
} // GetGrowable()


/**
 * gets the specified component's shrinkable status
 *
 * @param iIndex the label index
 *
 * @return shrinkable status
 */
bool Components::GetShrinkable( long iIndex )
{
  try
  {
    return ( * m_pvComponents )[ iIndex ]->GetShrinkable();
  }
  catch ( ex::VException & e )
  {
    throw gcnew v3DException( & e );
  }
} // GetShrinkable()


/**
 * gets the specified component's unlockable status
 *
 * @param iIndex the label index
 *
 * @return unlockable status
 */
bool Components::GetUnlockable( long iIndex )
{
  try
  {
    return ( * m_pvComponents )[ iIndex ]->GetUnlockable();
  }
  catch ( ex::VException & e )
  {
    throw gcnew v3DException( & e );
  }
} // GetUnlockable()


/**
 * gets the specified component's name editable status
 *
 * @param iIndex the label index
 *
 * @return name editable status
 */
bool Components::GetNameEditable( long iIndex )
{
  try
  {
    return ( * m_pvComponents )[ iIndex ]->GetNameEditable();
  }
  catch ( ex::VException & e )
  {
    throw gcnew v3DException( & e );
  }
} // GetNameEditable()


/**
 * Sets the label volume.
 *
 * @param mpLabelVolume reference to a label volume.
 */
void Components::LabelVolume::set ( Viatronix::Visualization::Volume ^ mpLabelVolume )
{
  try
  {
    m_mpLabelVolume = mpLabelVolume;

    m_pComponentHandler->SetSegmentVolume( _native_ptr_t< vxBlockVolume< uint2 > >( mpLabelVolume->GetVolumePtr() ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_LabelVolume()


/** 
 * Connects an observer to the object.
 *
 * @param observer vxManagedObserver object interested in events.
 */
void Components::Connect( System::IntPtr observer )
{
  m_pvComponents->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect()


/** 
 * Disconnects an observer from the object.
 *
 * @param observer vxManagedObserver object interested in events.
 */
void Components::Disconnect( System::IntPtr  observer )
{
  m_pvComponents->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect()


/**
 * Processes modified queue events.
 *
 * @param modifiedList Modified info list.
 */
void Components::OnModified( System::IntPtr modifiedList )
{
  vxModifiedInfoList * pInfoList = reinterpret_cast< vxModifiedInfoList * >( modifiedList.ToPointer() );
  
  for ( const vxModifiedInfo * pInfo(pInfoList->Begin()); pInfo != NULL; pInfo = pInfoList->Next() )
  {  
    uint4 uFlags(pInfo->GetFlag());
    
    if ( uFlags & vxUtils::COMPONENTARRAY_ADDED )     
      Added( this, System::EventArgs::Empty );
    
    if ( uFlags & vxUtils::COMPONENTARRAY_REMOVED )   
      Removed( this, System::EventArgs::Empty );

    if ( uFlags & vxUtils::COMPONENTARRAY_NAME_MODIFIED )  
    {
      const vxModifiedIndexedInfo * pIndexInfo = dynamic_cast< const vxModifiedIndexedInfo * >( pInfo ); 
      if ( pIndexInfo )
        NameChanged( this, gcnew IntegerArg( pIndexInfo->GetIndex() ) );
    }
    
    if ( uFlags & vxUtils::COMPONENTARRAY_MODIFIED )  
    {
      const vxModifiedIndexedInfo * pIndexInfo = dynamic_cast< const vxModifiedIndexedInfo * >( pInfo ); 
      if ( pIndexInfo )
        Changed( this, gcnew IntegerArg( pIndexInfo->GetIndex() ) );
      else
        Changed( this, System::EventArgs::Empty );
    } // if ( uFlags & vxUtils::COMPONENTARRAY_MODIFIED )  
    
    if ( uFlags & vxUtils::COMPONENTARRAY_SELECTED ) 
    {
      const vxModifiedIndexedInfo * pIndexInfo = dynamic_cast< const vxModifiedIndexedInfo * >( pInfo ); 
      Selected( this, gcnew IntegerArg( pIndexInfo->GetIndex() ) );
    } // if ( uFlags & vxUtils::COMPONENTARRAY_SELECTED ) 
  } // for (const vxModifiedInfo * pInfo = pInfoList->Begin(); pInfo != NULL; pInfo = pInfoList->Next())
} // OnModified()


/**
 * Serializes the components object to xml.
 *
 * @return Xml string representing object.
 */
System::String ^ Components::ToXml()
{
  try
  {
    // get all the data
    vxComponentArray * pComponentArray(ToNativeComponentsPtr( GetComponentsPtr() ));

    // write it
    vxSerializer serializer( "Components" );
    serializer.WriteAttribute( "version", "2.0" );
    serializer.Write(vxSerializer::AddElement( serializer.GetCurrent(), vxSerializer::GetElementName( *pComponentArray ), "component array" ), *pComponentArray );
    return gcnew System::String( serializer.ToString().c_str() );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException(&e);
  }
} // ToXml()


/**
 * Creates a new Components instance from the specified xml string.
 *
 * @param mpXml mml string.
 * @return New Components instance.
 */
Components ^ Components::FromXml( System::String ^ mpXml )
{
  try
  {
    // create xml document and get root element
    vxDOMDocument document = vxDOMDocument::FromXml( ss( mpXml ) );
    vxDOMElement element = document.GetDocumentElement();

    string sVersion( "1.0" );
    vxSerializer::GetAttribute( element, "version", sVersion );

    if ( sVersion == "2.0" )
    {
      // read colormap
      Components ^ mpComponents( gcnew Components() );
      vxComponentArray & componentArray( * ToNativeComponentsPtr( mpComponents->GetElementPtr() ) );
      vxSerializer::Read(vxSerializer::FindElement( element, vxSerializer::GetElementName( componentArray ), "component array" ), componentArray );
      return mpComponents;
    }
    return gcnew Components();
  } // try
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // FromXml()


/**
* Creates a new Components instance from the specified xml assembly resource.
*
* @param mpXmlResource Xml Assembly resource name.
* @return New Components instance.
*/
Components ^ Components::FromResource( System::String ^ mpXmlResource )
{
  try
  {
    XmlTextReader xmlReader( System::Reflection::Assembly::GetCallingAssembly()->GetManifestResourceStream( mpXmlResource ) );
    XmlDocument xmlDocument;
    xmlDocument.Load( %xmlReader );

    return Components::FromXml( xmlDocument.InnerXml );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // FromResource()


/**
* Creates a new Components instance from the specified xml file string.
*
* @param mpXmlFile Xml file.
* @return New Components instance.
*/
Components ^ Components::FromFile( System::String ^ mpXmlFile )
{
  try
  {
    XmlDocument xmlDocument;
    xmlDocument.Load( mpXmlFile );

    return Components::FromXml( xmlDocument.InnerXml );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // FromFile()


/**
 * configure the components from xml
 * reads something like:
 *
 * <componentVisibilities version="1.0" defaultVisible="false">
 *   <componentVisibility name="Background" visible="true"/>
 *   <componentVisibility name="component A" visible="true"/>
 *   <componentVisibility name="component B" visible="false"/>
 *   <componentVisibility regex="Carotid" visible="false"/> <!-- finds all names that contain 'Carotid' --->
 *   <selectedComponent name="component C"> <!-- can also be regex="" -->
 * </componentVisibilities>
 *
 * @param mpParent the parent xml node
 */
void Components::ConfigureFromXml( System::Xml::XmlNode ^ mpParent )
{

  try
  {

    //
    // check the version
    //
    XmlNode ^ mpVersionNode = mpParent->Attributes->GetNamedItem( "version" );
    if( mpVersionNode == nullptr )
      throw ex::DataCorruptionException( LogRec( "version missing from component configuration node", "Components", "ConfigureFromXml" ) );

    System::String ^ mpVersion = mpVersionNode->Value;
    if( mpVersion->CompareTo( "1.0" ) != 0 )
      throw ex::DataCorruptionException( LogRec( "Unknown XML version: " + ss( mpVersion ), "Components", "ConfigureFromXml" ) );

    //
    // get default visibility
    //
    XmlNode ^ mpDefaultNode = mpParent->Attributes->GetNamedItem( "defaultVisible" );
    if ( mpDefaultNode == nullptr )
    {
      throw ex::DataCorruptionException( LogRec( "default visibility attribute missing", "Components", "ConfigureFromXml" ) );
    }
    bool bDefaultVisible( true );
    System::String ^ mpDefaultVisible = mpDefaultNode->Value;
    if( mpDefaultVisible->CompareTo( "true" ) == 0 )
    {
      bDefaultVisible = true;
    }
    else if( mpDefaultVisible->CompareTo( "false" ) == 0 )
    {
      bDefaultVisible = false;
    }
    else
      throw ex::DataCorruptionException( LogRec( "unknown default visibility: " + ss( mpDefaultVisible ), "Components", "ConfigureFromXml" ) );

    //
    // set default visibility
    //
    for( int i(0); i<this->Count; ++i )
      SetItensityVisible( i, bDefaultVisible );

    //
    // get all the visibilities
    //
    for( int i(0); i < mpParent->ChildNodes->Count; ++i )
    {

      XmlNode ^ mpChildNode = mpParent->ChildNodes[ i ];
      if( mpChildNode->Name->CompareTo( "componentVisibility" ) == 0 )
      {

        //
        // get the visibility
        //
        XmlNode ^ mpVisibleNode = mpChildNode->Attributes->GetNamedItem( "visible" );
        bool bVisible( true );
        if( mpVisibleNode == nullptr )
          throw ex::DataCorruptionException( LogRec( "visibility attribute not specified", "Components", "ConfigureFromXml" ) );

        System::String ^ mpVisibleString = mpVisibleNode->Value;
        if ( mpVisibleString->CompareTo( "true" ) == 0 )
        {
          bVisible = true;
        }
        else if ( mpVisibleString->CompareTo( "false" ) == 0 )
        {
          bVisible = false;
        }
        else
          throw ex::DataCorruptionException( LogRec( "unknown component visibility: " + ss( mpChildNode->Name ), "Components", "ConfigureFromXml" ) );

        //
        // if by component name
        //
        bool bNamed( false );
        XmlNode ^ mpNameNode = mpChildNode->Attributes->GetNamedItem( "name" );
        XmlNode ^ mpRegexNode = mpChildNode->Attributes->GetNamedItem( "regex" );
        if( mpNameNode != nullptr )
        {

          // search for the named component and set its visibility
          bool bFound( false );
          LogDbg( "searching for named component: '" + ss( mpNameNode->Value ) + "'", "Components", "ConfigureFromXml" );
          for ( int j(0); j<this->Count; ++j )
          {
            if ( this->GetName( j )->CompareTo( mpNameNode->Value ) == 0 )
            {
              LogDbg( "Setting named component '" + ss( this->GetName( j ) ) + "' " +
                      ( bVisible ? std::string( "visible" ) : std::string( "invisible" ) ), "Components", "ConfigureFromXml" );
              SetItensityVisible( j, bVisible );
              bFound = true;
            }
          }
          if ( bFound == false )
          {
            LogDbg( "named component not found: " + ss( mpNameNode->Value ), "Components", "ConfigureFromXml" );
          }

        } // if name attribute is preset
        else if ( mpRegexNode != nullptr )
        {

          // search for the component that matches the regular expression and set its visibility
          LogDbg( "searching for regular expression matched component: '" + ss( mpRegexNode->Value ) + "'", "Components", "ConfigureFromXml" );
          bool bFound( false );
          for ( int j(0); j<this->Count; ++j )
          {
            if ( Regex::IsMatch( this->GetName( j ), mpRegexNode->Value ) )
            {
              LogDbg( "Setting regular expression matched component '" + ss( this->GetName( j ) ) + "' " +
                      ( bVisible ? std::string( "visible" ) : std::string( "invisible" ) ), "Components", "ConfigureFromXml" );
              SetItensityVisible( j, bVisible );
              bFound = true;
            }
          }
          if ( bFound == false )
          {
            LogDbg( "regex component not found: " + ss( mpRegexNode->Value ), "Components", "ConfigureFromXml" );
          }

        } // if regex attribute is preset
        else
        {
          throw ex::DataCorruptionException( LogRec( "neither name nor regex attributes specified", "Components", "ConfigureFromXml" ) );
        }

      } // if == "componentVisibility"
      else if ( mpChildNode->Name->CompareTo( "selectedComponent" ) == 0 )
      {

        if ( mpChildNode->Attributes->GetNamedItem( "regex" ) != nullptr )
        {
          // search for regular expression in components and select it
          for ( int j(0); j<this->Count; ++j )
          {
            if ( Regex::IsMatch( this->GetName( j ), mpChildNode->Attributes->GetNamedItem( "regex" )->Value ) )
            {
              LogDbg( "Selecting regular expression matched component '" + ss( this->GetName( j ) ) + "'", "Components", "ConfigureFromXml" );
              this->SetSelected( j );
            }
          }
        }
        else if ( mpChildNode->Attributes->GetNamedItem( "name" ) != nullptr )
        {
          // search for named component and select it
          for ( int j(0); j<this->Count; ++j )
          {
            if ( this->GetName( j )->CompareTo( mpChildNode->Attributes->GetNamedItem( "name" )->Value ) == 0 )
            {
              LogDbg( "Selecting named component '" + ss( this->GetName( j ) ) + "'", "Components", "ConfigureFromXml" );
              this->SetSelected( j );
            }
          }
        }
        else
        {
          throw ex::DataCorruptionException( LogRec( "neither name nor regex attributes specified in selected component", "Components", "ConfigureFromXml" ) );
        }

      }
      else
      {
        throw ex::DataCorruptionException( LogRec( "unknown element: " + ss( mpChildNode->Name ), "Components", "ConfigureFromXml" ) );
      }

    } // forall child nodes

  } // try
  catch ( System::Exception ^ e )
  {
    LogDbg( ss( e->ToString() ) + " " + ss( e->Message ), "Components", "ConfigureFromXml" );
    throw gcnew v3DException( e->Message, e );
  }
  catch ( ex::VException & e )
  {
    throw gcnew v3DException( & e );
  }
  catch ( ... )
  {
    throw gcnew v3DException( "unhandled exception in Components::ConfigureFromXml" );
  }

} // ConfigureFromXml()


// undefines
#undef FILE_REVISION


// $Log: v3D_Components.cpp,v $
// Revision 1.24  2008/04/18 21:10:24  dongqing
// using more safe (but slower) way to update label volume index
//
// Revision 1.23  2007/06/28 19:34:14  jmeade
// Merge from CON_3-0 on 2007-06-28; branch code from 2007-06-22
//
// Revision 1.22.2.1  2007/04/30 21:13:32  romy
// exposes the compact method
//
// Revision 1.22  2007/02/07 22:43:53  romy
// added Removing Event - This helps Plaques to reset the label volume before it gets removed.
//
// Revision 1.21  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.20  2006/09/27 17:23:48  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.19  2006/09/26 14:47:05  romy
// added Compiste Undo action interfaces for the managed calls
//
// Revision 1.18  2006/06/05 15:05:36  frank
// bad mix of managed and unmanaged code
//
// Revision 1.17  2006/04/18 18:49:15  romy
// naming convention followed for 'm_pObserver'
//
// Revision 1.16  2006/04/17 19:13:43  romy
// destructor , added Null checks
//
// Revision 1.15  2006/03/29 20:11:50  geconomos
// removed vxBlockVolumeBinaryHistogram related code
//
// Revision 1.14  2006/03/02 15:51:33  frank
// cleaned up grow and shrink operations
//
// Revision 1.13  2006/02/28 15:24:42  frank
// improved undo capabilities during interactive segmentation
//
// Revision 1.12  2006/02/27 15:23:42  frank
// split out component utilities
// added undo for component operations
//
// Revision 1.11  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.10  2005/04/12 15:23:52  geconomos
// changed xml library to the vxDOM classes
//
// Revision 1.9  2005/03/08 19:09:47  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.8.2.1  2005/02/15 17:08:11  frank
// added unlockable attribute
//
// Revision 1.8  2004/12/13 15:34:05  frank
// added name editable property
//
// Revision 1.7  2004/10/29 20:18:33  frank
// formatting
//
// Revision 1.6  2004/07/20 12:22:57  frank
// fixed ordering of components so buttons update properly in components pane
//
// Revision 1.5  2004/07/12 18:06:13  frank
// added support for the growable, shrinkable, and removable component properties
//
// Revision 1.4  2004/06/15 14:20:50  frank
// enabled selection of components through XML
//
// Revision 1.3  2004/05/13 19:18:10  frank
// added regular expression parsing to component names
//
// Revision 1.2  2004/05/11 14:41:46  frank
// added ability to configure the components from xml
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.49  2004/03/02 01:10:46  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.48.2.1  2004/02/24 20:05:16  michael
// coding standard: assign NULL after deletion
//
// Revision 1.48  2004/01/29 17:50:44  frank
// Set the none component to visible when all other components are removed
//
// Revision 1.47  2003/12/23 14:42:40  frank
// Ensured that a valid component is selected after component removal
//
// Revision 1.46  2003/12/23 13:23:52  frank
// Selected None component after "remove all"
//
// Revision 1.45  2003/12/16 13:25:50  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.44  2003/08/20 14:31:10  michael
// added "mailto:"
//
// Revision 1.43  2003/05/16 13:09:36  frank
// code formatting
//
// Revision 1.42  2003/05/13 13:28:43  michael
// code review
//
// Revision 1.41  2003/05/05 17:13:04  geconomos
// Coding standards.
//
// Revision 1.40  2003/04/28 16:59:24  michael
// fixed issue #3116 and 3065
//
// Revision 1.39  2003/04/10 18:30:28  geconomos
// Proper handling of selected index.
//
// Revision 1.38  2003/04/09 15:06:25  michael
// further step in cleaning up the semgentation/component handler code. In most
// locations there now is a IndexHistory and IndexLabel to better differentiate what
// is used. E.g. SegmentationVolumes and renderers use IndexLabel but internally
// the segmentation/component handler mostly works on HistoryIndex (component pane).
//
// Revision 1.37  2003/04/08 17:29:20  michael
// fixed setting of maximum component index number
//
// Revision 1.36  2003/04/01 16:02:08  geconomos
// Updated to public interface of managed volume class.
//
// Revision 1.35  2003/03/31 13:51:49  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.34  2003/03/31 13:42:22  geconomos
// Updated components class event names to coding conventions..
//
// Revision 1.33  2003/03/18 16:16:31  geconomos
// Issue # 2916: Deleting associated annotation when deleting  a component.
//
// Revision 1.32  2003/03/14 14:02:08  geconomos
// Updated for changed in Volume::BinaryHistogram return type.
//
// Revision 1.31  2003/03/10 15:00:44  geconomos
// Implemented EVENT_NAME_MODIFIED flag.
//
// Revision 1.30  2002/12/20 15:57:43  ingmar
// new PixelRGBA interface functions R() G() B() A()
//
// Revision 1.29  2002/11/28 04:08:20  michael
// added setting of the maximum component index so that binary histogram is
// computed correctly after restoring the data from session
//
// Revision 1.28  2002/11/28 02:46:41  frank
// component selected events
//
// Revision 1.27  2002/11/27 14:12:45  geconomos
// Removed call to Modified when setting the name and selected component.
//
// Revision 1.26  2002/11/26 13:41:20  michael
// moved initialize of ComponentHandler our of constructor (from vxComponentArray)
// Should only be called once label volume is set!!!
//
// Revision 1.25  2002/11/25 21:07:09  michael
// update mask volume
//
// Revision 1.24  2002/11/15 14:52:04  michael
// fixed the problem on unselecting an already gone component (after removeAll)
//
// Revision 1.23  2002/11/14 20:27:53  geconomos
// changed root node in serialization to class name.
//
// Revision 1.22  2002/11/14 14:14:20  geconomos
// Added constructor from vxComponentArray.
//
// Revision 1.21  2002/11/14 14:09:44  geconomos
// Added copy constructor.
//
// Revision 1.20  2002/11/13 02:21:19  michael
// added some more serialization (Components)
//
// Revision 1.19  2002/11/13 00:14:14  michael
// first try to serialize the Components
//
// Revision 1.18  2002/11/12 21:31:01  geconomos
// All standard Environment elements now implement IElement.
//
// Revision 1.17  2002/11/11 19:19:07  geconomos
// Reworked serialization.
//
// Revision 1.16  2002/11/10 23:11:20  michael
// removed update calls on textures
//
// Revision 1.15  2002/11/08 18:08:02  geconomos
// Removed setting of modified flag for Locked property ( at Michael's request ).
//
// Revision 1.14  2002/10/31 12:50:15  geconomos
// Chaged GetColormapPtr() to return a System::IntPtr for interoperability.
//
// Revision 1.13  2002/10/18 17:20:57  geconomos
// Added read-only property for Segmented.
//
// Revision 1.12  2002/10/11 21:33:49  geconomos
// Implemented IObserver interface.
//
// Revision 1.11  2002/10/07 15:20:33  geconomos
// Merged Volume_Int2 and Volume_uint2 into Volume.
//
// Revision 1.10  2002/10/04 13:31:00  geconomos
// Added component handler.
//
// Revision 1.9  2002/09/25 22:37:46  wenli
// Add BoxOfInterest
//
// Revision 1.8  2002/09/24 16:08:21  geconomos
// Exception handling and formatting.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Components.cpp,v 1.24 2008/04/18 21:10:24 dongqing Exp $
// $Id: v3D_Components.cpp,v 1.24 2008/04/18 21:10:24 dongqing Exp $
