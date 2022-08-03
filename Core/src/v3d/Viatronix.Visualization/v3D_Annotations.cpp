// $Id: v3D_Annotations.cpp,v 1.12 2006/10/04 18:29:04 gdavidson Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)


// includes
#include "StdAfx.h"
#include "v3D_Annotations.h"
#include "vxManagedObserver.h"
#include "v3D_EventArgs.h"
#include "v3D_Volume.h"
#include "v3D_Interfaces.h"
#include "v3D_Environment.h"
#include "vxSerializer.h"
#include "vxAnnotations.h"
#include "vxUndoActionAnnotation.h"


// namespaces
USING_VISUALIZATION_NS
using namespace System::Xml;


// includes
#include "vxAnnotationBox.h"
#include "vxAnnotationEllipse.h"
#include "vxAnnotationComponent.h"
#include "vxAnnotationDistance.h"
#include "vxAnnotationPoint.h"
#include "vxAnnotationArrow.h"
#include "vxAnnotationAngle.h"


// defines
DEFINE_ANNOTATION( AnnotationPoint )
DEFINE_ANNOTATION( AnnotationArrow )
DEFINE_ANNOTATION( AnnotationDistance )
DEFINE_ANNOTATION( AnnotationAngle )
DEFINE_ANNOTATION( AnnotationBox )
DEFINE_ANNOTATION( AnnotationEllipse )
DEFINE_ANNOTATION( AnnotationComponent )


/**
 * Initializes a Annotations object with default values..
 */
Annotations::Annotations()
{
  m_pAnnotations = new vxShareableObject< vxAnnotations >();
  
  // create and connect the observer
  m_pObserver = new vxManagedObserver ( this );
  m_pAnnotations->Connect(m_pObserver);

} // Annotations()


/**
 * Initializes a Annotations object from the specified vxAnnotations. 
 * Object assumes ownership of unamanaged class.
 *
 * @param pAnnotations Unmananged vxAnnotations implementation.
 */
Annotations::Annotations( vxAnnotations * pAnnotations )
{ 
  // create a shareable object 
  m_pAnnotations = new vxShareableObject< vxAnnotations >( *pAnnotations );
  
  // create and connect the observer
  m_pObserver = new vxManagedObserver( this );
  m_pAnnotations->Connect( m_pObserver );

} // Annotations( vxAnnotations * pAnnotations )


/**
 * desturctor.
 */
Annotations::~Annotations()
{
  this->!Annotations();
} // ~Annotations()


/**
 * Finalizer.
 */
Annotations::!Annotations()
{
  delete m_pAnnotations; m_pAnnotations = NULL;
  delete m_pObserver;   m_pObserver = NULL;
} // !Annotations()


/**
 * Creates a new object that is a copy of the current instance.
 *
 * @return  A new object that is a copy of this instance.
 */
System::Object ^ Annotations::Clone()
{
  // create a new instance of the annotations class
  Annotations ^ mpAnnotations = gcnew Annotations();

  // get the underlying implementation pointer  
  vxAnnotations * pAnnotations = dynamic_cast< vxAnnotations *>( mpAnnotations->m_pAnnotations );
  
  // make a copy of the underying implementations
  *pAnnotations = *dynamic_cast< vxAnnotations *>( m_pAnnotations );  
  
  return mpAnnotations;
} // Clone()



/**
 * Adds an annotation using the specified factory object.
 *
 * @param mpFactory Annotation factory object.
 */
void Annotations::Add( IAnnotationFactory ^ mpFactory )
{
 try
  {
    // create the unmanaged annotation using the factory
    vxAnnotation * pAnnotation = reinterpret_cast< vxAnnotation * > ( mpFactory->CreateAnnotation().ToPointer() );
    
    // add the annotation
    m_pAnnotations->Add( pAnnotation );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // Add()


/** 
 * Removes the annotation at the specified index.
 *
 * @param iIndex Index of annotation to remove.
 */
void Annotations::Remove( long iIndex )
{
  try
  {
    vxEnvironment::GetUndoActions().PushAction( new vxUndoActionAnnotationRemove( m_pAnnotations, m_pAnnotations->Get( iIndex ) ) );
    m_pAnnotations->Remove( iIndex );
    m_pAnnotations->Modified( vxModIndInfo( vxAnnotations, iIndex, vxUtils::ANNOTATIONS_REMOVED ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // Remove()

/** 
 * Removes all the annotations
 */
void Annotations::RemoveAll( )
{
  try
  {
		vxEnvironment::GetUndoActions().PushAction( new vxUndoActionCompositeBegin());

		// loop through all the annotations and remove them.
		for( int iIndex = m_pAnnotations->GetCount() - 1; iIndex >= 0; iIndex-- )
			Remove( iIndex );

		vxEnvironment::GetUndoActions().PushAction( new vxUndoActionCompositeEnd());
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // RemoveAll()


/** 
 * Gets the index of the selected annotation.
 *
 * @return Index of selected annotation; -1 if none is selected
 */
long Annotations::Selected::get()
{
  try
  {
    return m_pAnnotations->GetActiveIndex();
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // get_Selected()


/** 
 * Sets the selected annotation.
 * 
 * @param iIndex Index of annotation to select.
 */
void Annotations::Selected::set( long iIndex )
{
  try
  {
    vxEnvironment::GetUndoActions().PushAction( new vxUndoActionAnnotationSelect( m_pAnnotations, m_pAnnotations->GetActiveIndex() ) );
    m_pAnnotations->SetActiveIndex( iIndex );
    m_pAnnotations->Modified( vxModIndInfo( vxAnnotations, iIndex, vxUtils::ANNOTATIONS_UPDATE_PLANE ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // set_Selected()

  
/** 
 * Gets the number of annotations.
 *
 * @param Number of annotations.
 */
long Annotations::Count::get()
{
  try
  {
    return m_pAnnotations->GetCount();
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // get_Count()


/** 
 *  Gets the annotation's text.
 *
 * @param iIndex Index of annotation.
 * @return Annotation text.
 */
System::String ^ Annotations::GetText( long iIndex )
{
  try
  {
    return gcnew System::String( m_pAnnotations->Get( iIndex )->GetText().c_str() );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // GetText()

  
/** 
 *  Gets the annotation's label.
 *
 * @param iIndex Index of annotation.
 * @return Annotation label.
 */
System::String ^ Annotations::GetLabel( long iIndex )
{
  try
  {
    return gcnew System::String( m_pAnnotations->Get( iIndex )->GetLabel( ).c_str() );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // GetLabel()


/** 
 *  Sets the annotation's label.
 *
 * @param iIndex Index of annotation.
 * @param mpLabel Label text.
 */
void Annotations::SetLabel( long iIndex, System::String ^ mpLabel )
{
  try
  {
    m_pAnnotations->Get( iIndex )->SetLabel( ss( mpLabel ) );
    m_pAnnotations->Modified( vxModIndInfo( vxAnnotations, iIndex, vxUtils::ANNOTATIONS_MODIFIED ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // SetLabel()


/**
 * Checks whether the label is editable or not
 */
bool Annotations::IsLabelEditable( long iIndex )
{
	try
  {
    return m_pAnnotations->Get( iIndex )->IsLabelEditable(); 
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
}//IsLabelEditable( long iIndex )


/** 
 * Gets the annotation's color.
 *
 * @param iIndex Index of annotation
 * @return Color of annotation.
 */
System::Drawing::Color Annotations::GetColor( long iIndex )
{
  try
  {
    PixelRGBA<uint1> & pixel = m_pAnnotations->Get( iIndex )->GetColor();  
    return System::Drawing::Color::FromArgb( pixel.A(), pixel.R(), pixel.G(), pixel.B() );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // GetColor()


/** 
 *  Sets the annotation's color.
 *
 * @param iIndex Index of annotation.
 * @param color Annotation color.
 */
void Annotations::SetColor( long iIndex, System::Drawing::Color color )
{
  try
  {
    m_pAnnotations->Get( iIndex )->SetColor( PixelRGBA<uint1>( color.R, color.G, color.B, color.A ) );  
    m_pAnnotations->Modified( vxModIndInfo( vxAnnotations, iIndex, vxUtils::ANNOTATIONS_MODIFIED ) );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // SetColor()


/**
 * Gets the associated image.
 *
 * @param iIndex Index of annotation.
 * @return Associated image.
 */
System::Drawing::Bitmap ^ Annotations::GetImage( long iIndex )
{
	try
	{
		// create a default bitmap
		System::Drawing::Bitmap ^ mpBitmap = gcnew System::Drawing::Bitmap( 1, 1 );

		// get the associated image
		const Image< PixelRGBA<uint1> > * pImage = m_pAnnotations->Get( iIndex )->GetImage();
		mpBitmap = gcnew System::Drawing::Bitmap
		(
			pImage->GetDim().X(), //width
			pImage->GetDim().Y(),  // height
			pImage->GetDim().X() * 4, //stride
			System::Drawing::Imaging::PixelFormat::Format32bppArgb,
			System::IntPtr( ( void * )pImage->GetDataPtr() )
		);
		return mpBitmap;
	} // try
	catch( ex::VException & e )
	{
		throw gcnew v3DException( &e );
	}
} // GetImage()


/**
 * Gets the associated component index.
 *
 * @param iIndex Index of annotation.
 * @return Component index; 0 if none associated.
 */
int Annotations::GetComponentIndex( int iIndex, Viatronix::Visualization::Volume ^ mpLabelVolume )
{
  try
  {
    // initially no associated component
    int iComponentIndex = 0;

    // is the annotation a vxAnnotationComponent?
    vxAnnotation * pAnnotation = m_pAnnotations->Get( iIndex );
    vxAnnotationComponent * pAnnotationComponent = dynamic_cast<vxAnnotationComponent *>(pAnnotation);

    if (pAnnotationComponent != NULL)
    {
      iComponentIndex = pAnnotationComponent->GetComponentIndex();
    }
    
    // return the associated component index
    return iComponentIndex;    
  } // try
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // GetComponentIndex()


/**
 * Updates all component based annotations.
 *
 * @param mpEnvironment Associated environment for Annotations object.
 */
void Annotations::UpdateComponentAnnotations( Viatronix::Visualization::Environment ^ mpEnvironment )
{
  // loop through all the annotations
  for( int iIndex = 0; iIndex < m_pAnnotations->GetCount(); ++iIndex )
  {
    // is the annotation a vxAnnotationComponent?
    vxAnnotation * pAnnotation = m_pAnnotations->Get( iIndex );
    if ( dynamic_cast< vxAnnotationComponent *>( pAnnotation ) != NULL )
    {
      pAnnotation->Update( *ToNativeEnvironmentPtr( mpEnvironment->GetEnvironmentPtr() ) );
      m_pAnnotations->Modified( vxModIndInfo( vxAnnotations, iIndex, vxUtils::ANNOTATIONS_MODIFIED ) );
    }
  } // for ( int iIndex = 0; iIndex < m_pAnnotations->GetCount(); ++iIndex )
} // UpdateComponentAnnotations()


/**
 * Processes modified queue events.
 * 
 * @param modifiedList Modified info list.
 */
void Annotations::OnModified( System::IntPtr modifiedList )
{
  vxModifiedInfoList * pInfoList = reinterpret_cast< vxModifiedInfoList * >( modifiedList.ToPointer() );
  
  for (const vxModifiedInfo * pInfo = pInfoList->Begin(); pInfo != NULL; pInfo = pInfoList->Next())
  {  
    uint4 uFlags = pInfo->GetFlag();

    uint4 uIndex( 0 );
    
    const vxModifiedIndexedInfo * pInfoExt =  dynamic_cast< const vxModifiedIndexedInfo * >( pInfo );
    
    if (pInfoExt != NULL)
      uIndex = pInfoExt->GetIndex();

    if (uFlags & vxUtils::ANNOTATIONS_ADDED)     
      Added( this, gcnew IntegerArg( uIndex ) );
    
    if (uFlags & vxUtils::ANNOTATIONS_COMPLETED) 
      Completed( this, gcnew IntegerArg( uIndex ) );
    
    if (uFlags & vxUtils::ANNOTATIONS_REMOVED)   
      Removed( this, gcnew IntegerArg( uIndex ) );
    
    if (uFlags & vxUtils::ANNOTATIONS_MODIFIED)  
      Changed( this, gcnew IntegerArg( uIndex ) );
    
    if (uFlags & vxUtils::ANNOTATIONS_SELECTED)  
      SelectionChanged( this, gcnew IntegerArg( uIndex ) );
  } // for (const vxModifiedInfo * pInfo = pInfoList->Begin(); pInfo != NULL; pInfo = pInfoList->Next())
} // OnModified()

 
/** 
 * Connects an observer to the object.
 *
 * @param observer vxManagedObserver object interested in events.
 */
void Annotations::Connect( System::IntPtr observer )
{
  m_pAnnotations->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect()


/** 
 * Disconnects an observer from the object.
 *
 * @param observer vxManagedObserver object interested in events.
 */
void Annotations::Disconnect( System::IntPtr  observer )
{
  m_pAnnotations->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect()


/**
 * Serializes the annotations object to xml.
 *
 * @return Xml string representing object.
 */
System::String ^ Annotations::ToXml()
{
  try
  {
    vxAnnotations * pAnnotations = ToNativeAnnotationsPtr(GetElementPtr());

    // write it
    vxSerializer serializer("Annotations");
    serializer.WriteAttribute("version", "2.0");
    pAnnotations->ToXml( serializer.GetCurrent() );

    return gcnew System::String( serializer.ToString().c_str() );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // ToXml()


/**
 * Creates a new Annotations instance from the specified xml string.
 *
 * @param mpXml Xml string.
 * @return New Annotations instance.
 */
Annotations ^ Annotations::FromXml( System::String ^ mpXml )
{
  try
  {
    // create new annotations object
    Annotations ^ mpAnnotations = gcnew Annotations();

    // create xml document and get root element
    vxDOMDocument document = vxDOMDocument::FromXml( ss( mpXml ) );
    vxDOMElement element = document.GetDocumentElement();

    std::string sVersion( "1.0" );
    vxSerializer::GetAttribute( element, "version", sVersion );

    if ( sVersion == "2.0" )
    {
      // read annotations
      vxAnnotations annotations;
      annotations.FromXml(element); 
      mpAnnotations = gcnew Annotations(&annotations);
    }
    return mpAnnotations;
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // FromXml()


/**
 * Creates a new Annotations instance from the specified xml assembly resource.
 *
 * @param mpXmlResource Xml Assembly resource name.
 * @return New Annotations instance.
 */
Annotations ^ Annotations::FromResource( System::String ^ mpXmlResource )
{
  try
  {
    XmlTextReader xmlReader( System::Reflection::Assembly::GetCallingAssembly()->GetManifestResourceStream( mpXmlResource ) );
    
    XmlDocument xmlDocument;
    xmlDocument.Load( %xmlReader );

    return Annotations::FromXml( xmlDocument.InnerXml );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // FromResource()


/**
 * Creates a new Annotations instance from the specified xml file string.
 *
 * @param mpXmlFile Xml file.
 * @return New Annotations instance.
 */
Annotations ^ Annotations::FromFile( System::String ^ mpXmlFile )
{
  try
  {
    XmlDocument xmlDocument;
    xmlDocument.Load( mpXmlFile );

    return Annotations::FromXml( xmlDocument.InnerXml );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // FromFile()


// $Log: v3D_Annotations.cpp,v $
// Revision 1.12  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.11  2006/09/27 21:31:38  romy
// put the resource pointer back
//
// Revision 1.10  2006/09/27 17:23:48  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.9  2006/09/26 21:26:26  romy
// Annotations GetImage interface modification incorporated
//
// Revision 1.8  2006/09/25 20:48:58  romy
// added label editable flag for annotations.
//
// Revision 1.7  2006/09/21 15:09:41  romy
// added annotation removeAll
//
// Revision 1.6  2006/03/02 15:51:47  frank
// cleaned up grow and shrink operations
//
// Revision 1.5  2006/02/21 21:21:35  frank
// added more undo for annotations
//
// Revision 1.4  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.3  2005/04/12 15:23:52  geconomos
// changed xml library to the vxDOM classes
//
// Revision 1.2  2005/03/08 19:09:47  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.1.1.1.6.1  2005/02/17 18:11:55  michael
// Using access function of vxComponentIndex to get label information instead
// of locally implementing redundant/different logic. Fixes issue #3972
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.42  2004/03/02 01:10:47  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.41.2.1  2004/02/24 20:15:10  michael
// coding standard: assign NULL after deletion and m_pOberserver --> m_pObserver
//
// Revision 1.41  2004/01/23 03:59:12  michael
// adjusted code to reflect changes made in refactoring annotations
//
// Revision 1.40  2003/12/16 13:25:50  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.39  2003/08/20 14:31:09  michael
// added "mailto:"
//
// Revision 1.38  2003/07/02 15:04:43  geconomos
// Implemented ICloneable
//
// Revision 1.37  2003/05/13 13:28:43  michael
// code review
//
// Revision 1.36  2003/05/12 19:14:36  frank
// code review
//
// Revision 1.35  2003/05/06 15:31:33  geconomos
// More coding standards.
//
// Revision 1.34  2003/05/05 17:13:04  geconomos
// Coding standards.
//
// Revision 1.33  2003/04/01 17:46:04  geconomos
// Coding convention for event naming.
//
// Revision 1.32  2003/03/31 13:51:49  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.31  2003/03/20 15:36:00  geconomos
// Added EVENT_UPDATE_PLANE event to modified queue when annotation is selected.
//
// Revision 1.30  2003/03/20 13:43:07  geconomos
// Rearranged includes for that flaky internal compiler error.
//
// Revision 1.29  2003/03/18 16:47:59  geconomos
// Added UpdateComponentAnnotations() method.
//
// Revision 1.28  2003/03/18 16:16:31  geconomos
// Issue # 2916: Deleting associated annotation when deleting  a component.
//
// Revision 1.27  2003/03/04 16:51:43  geconomos
// Removed unused methods.
//
// Revision 1.26  2003/03/04 01:43:04  michael
// renaming of methods removing the double occurence of "Annotation", e.g.
// vxAnnotations::GetActiveAnnotation() --> vxAnnotations::GetActive()
//
// Revision 1.25  2003/03/03 16:51:30  geconomos
// Removed unused methods from Annotations.
//
// Revision 1.24  2003/02/26 19:28:06  geconomos
// Added annotation images.
//
// Revision 1.23  2003/02/22 00:23:56  michael
// fixed annotations to serialize and deserialize
//
// Revision 1.22  2003/02/19 18:26:18  michael
// added serialization of annotations to the managed code
//
// Revision 1.21  2003/02/18 18:02:16  michael
// moved constructor further down in .cpp file so that Internal Compile Error does not occure.
//
// Revision 1.20  2003/02/13 16:56:46  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.19  2003/02/07 00:29:59  michael
// added annotation for components
//
// Revision 1.18.2.1  2003/02/12 22:59:19  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.18  2003/01/20 19:37:43  michael
// added ellipse annotation
//
// Revision 1.17  2003/01/19 21:07:48  michael
// added box annotation
//
// Revision 1.16  2003/01/14 23:37:10  michael
// added new annotation types
//
// Revision 1.15  2003/01/13 14:45:57  geconomos
// Switched implementation to new vxAnnotations class.
//
// Revision 1.14  2002/12/20 15:57:43  ingmar
// new PixelRGBA interface functions R() G() B() A()
//
// Revision 1.13  2002/11/12 21:31:01  geconomos
// All standard Environment elements now implement IElement.
//
// Revision 1.12  2002/11/11 19:19:06  geconomos
// Reworked serialization.
//
// Revision 1.11  2002/10/07 15:20:33  geconomos
// Merged Volume_Int2 and Volume_uint2 into Volume.
//
// Revision 1.10  2002/09/30 20:22:28  dmitry
// checking if the stream in deserialization is valid
//
// Revision 1.9  2002/09/30 14:37:58  dmitry
// All events changed to IntegerArg.
//
// Revision 1.8  2002/09/24 15:37:26  geconomos
// Exception handling and formatting.
//
// Revision 1.7  2002/09/20 15:34:05  dmitry
// Selected property added.
//
// Revision 1.6  2002/09/03 21:02:12  dmitry
// Update function fixed.
//
// Revision 1.5  2002/08/13 15:52:10  dmitry
// Removed template arguments to serialization stream.
//
// Revision 1.4  2002/08/08 21:05:30  dmitry
// EventCompleted added.
//
// Revision 1.3  2002/08/07 19:42:38  dmitry
// Event handler changed, code debugged, and serialization implemented.
//
// Revision 1.2  2002/07/16 23:23:26  geconomos
// no message
//
// Revision 1.1  2002/07/10 23:13:48  dmitry
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Annotations.cpp,v 1.12 2006/10/04 18:29:04 gdavidson Exp $
// $Id: v3D_Annotations.cpp,v 1.12 2006/10/04 18:29:04 gdavidson Exp $
