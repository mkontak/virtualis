// $Id: v3D_RenderingMode.cpp,v 1.16 2007/08/21 19:40:34 gdavidson Exp $
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
#include "v3d_RenderingMode.h"
#include "vxManagedObserver.h"
#include "vxSerializer.h"
#include "vxUndoActionPreset.h"


// namespaces
using namespace Viatronix::Visualization;
using namespace System::Xml;


/**
 * Initializes a RenderingMode object with default values.
 */
RenderingMode::RenderingMode()
{
  // unmanaged types 
  m_pObserver     = new vxManagedObserver ( this );
  m_pRenderingMode = new vxShareableObject< vxRenderingMode >();

  m_pRenderingMode->Connect( m_pObserver );
} // RenderingMode()


/**
 * destructor
 */
RenderingMode::~RenderingMode()
{
  this->!RenderingMode();
} // ~RenderingMode()


/**
 * finalizer
 */
RenderingMode::!RenderingMode()
{
  m_pRenderingMode->Disconnect( m_pObserver );

  delete m_pRenderingMode; m_pRenderingMode = NULL;
  delete m_pObserver;     m_pObserver = NULL;
} // !RenderingMode()


/**
 * Creates a new object that is a copy of the current instance
 *
 * @return RenderingMode instance
 */
System::Object ^ RenderingMode::Clone()
{

  // create a new instance of the rendering mode
  RenderingMode ^ mpMode = gcnew RenderingMode();

  // get the underlying implementation pointer  
  vxRenderingMode * pRenderingMode = dynamic_cast< vxRenderingMode *>( mpMode->m_pRenderingMode );

  // make a copy of the underying implementations
  *pRenderingMode = *dynamic_cast< vxRenderingMode *>( m_pRenderingMode );  

  return mpMode;
} // Clone()


/**
 * Resets the rendering mode object to a default state.
 */
void RenderingMode::Reset()
{
  m_pRenderingMode->Reset();
} // Reset()


/** 
 * Gets the current rendering algorithm.
 * @return RenderingAlgorithm enum value
 */
Viatronix::Visualization::RenderingAlgorithm RenderingMode::Mode::get()
{
  return static_cast<Viatronix::Visualization::RenderingAlgorithm>(m_pRenderingMode->GetRenderingAlgorithm() );
} // RenderingAlgorithm()


/** 
  * Sets the current rendering algorithm.
  * @param  eAlgorithm  New rendering algorithm
  */
void Viatronix::Visualization::RenderingMode::Mode::set( Viatronix::Visualization::RenderingAlgorithm eAlgorithm )
{
  m_pRenderingMode->SetRenderingAlgorithm( static_cast<vxRenderingMode::vxRenderingAlgorithmEnum>(eAlgorithm) );
  m_pRenderingMode->Modified( vxModInfo( vxRenderingMode, vxUtils::RENDERINGMODE_ALGORITHM_MODIFIED));
} // set_RenderingAlgorithm()


/** 
 * Gets the current slice shadowmode
 * @return slice shadow mode
 */
Viatronix::Visualization::SliceShadowMode RenderingMode::SliceShadowMode::get()
{
  return static_cast<Viatronix::Visualization::SliceShadowMode>(m_pRenderingMode->GetSliceShadowMode() );
} // SliceShadowMode()


/** 
  * Sets the current slice shadow mode
  * @param  eSliceShadowMode  new slice shadow mode
  */
void Viatronix::Visualization::RenderingMode::SliceShadowMode::set( Viatronix::Visualization::SliceShadowMode eSliceShadowMode )
{
  m_pRenderingMode->SetSliceShadowMode( static_cast<vxRenderingMode::vxSliceShadowModeEnum>(eSliceShadowMode) );
  m_pRenderingMode->Modified( vxModInfo( vxRenderingMode, vxUtils::RENDERINGMODE_SLICESHADOW_MODE_MODIFIED));
} // set_RenderingAlgorithm()


/** 
 * Gets the label visible mode.
 * @return True if labels are visible; false otherwise.
 */
bool RenderingMode::LabelVisible::get()
{
  return m_pRenderingMode->IsEnabled( vxRenderingMode::SHOW_COMPONENT_IN_MPR );
} // get_LabelVisible()


/** 
 * Sets the label visible mode.
 * @param True if labels are visible; false otherwise.
 */
void RenderingMode::LabelVisible::set( bool bVisible )
{
  m_pRenderingMode->SetEnabled( vxRenderingMode::SHOW_COMPONENT_IN_MPR, bVisible );
  m_pRenderingMode->Modified( vxModInfo( vxRenderingMode, vxRenderingMode::SHOW_ANNOTATIONS ) );
} // set_LabelVisible()


/** 
 * Gets the annotation visible mode.
 * @return True if annotations are visible; false otherwise.
 */
bool RenderingMode::AnnotationsVisible::get()
{
  return m_pRenderingMode->IsEnabled( vxRenderingMode::SHOW_ANNOTATIONS );
} // get_AnnotationVisible()


/** 
 * Sets the annotation visible mode.
 * @param True if annotations are visible; false otherwise.
 */
void RenderingMode::AnnotationsVisible::set( bool bVisible )
{
  m_pRenderingMode->SetEnabled( vxRenderingMode::SHOW_ANNOTATIONS, bVisible );
  m_pRenderingMode->Modified( vxModInfo( vxRenderingMode, vxRenderingMode::SHOW_ANNOTATIONS ) );
} // set_AnnotationVisible()


	
/** 
 * Gets the color is MPR views.
 * @return True if color is displayed in MPR views; false otherwise.
 */
bool RenderingMode::ColorUsedInMPR::get()
{
  return m_pRenderingMode->IsEnabled( vxRenderingMode::SHOW_COLORMAP_IN_MPR );
} // get_ColorUsedInMPR()


/** 
 * Sets the color is MPR views.
 * @param bVisible True if color is displayed in MPR views; false otherwise.
 */
void RenderingMode::ColorUsedInMPR::set( bool bVisible )
{
  m_pRenderingMode->SetEnabled( vxRenderingMode::SHOW_COLORMAP_IN_MPR, bVisible );
  m_pRenderingMode->Modified( vxModInfo( vxRenderingMode, vxRenderingMode::SHOW_COLORMAP_IN_MPR ) );
} // set_LabelVisible()

	
/** 
 * Gets the intensity range visible mode.
 * @return True if intensity is visible; false otherwise.
 */
bool RenderingMode::IntensityRangeVisible::get()
{
  return m_pRenderingMode->IsEnabled( vxRenderingMode::SHOW_INTENSITY_RANGE );
} // get_IntensityRangeVisible()


/** 
 * Sets the intensity range visible mode.
 * @Param bVisible True if intensity is visible; false otherwise.
 */
void RenderingMode::IntensityRangeVisible::set( bool bVisible )
{
  m_pRenderingMode->SetEnabled( vxRenderingMode::SHOW_INTENSITY_RANGE, bVisible );
  m_pRenderingMode->Modified( vxModInfo( vxRenderingMode, vxRenderingMode::SHOW_INTENSITY_RANGE ) );
} // set_IntensityRangeVisible()


/**
 * Gets the show cuts shaded mode.
 * @return True if cuts are shaded.
 */
bool RenderingMode::ShowCutsShaded::get()
{
  return m_pRenderingMode->IsEnabled( vxRenderingMode::SHOW_CUTS_SHADED );
} // get_ShowCutsShaded()


/**
 * Gets the show cuts shaded mode.
 * @param bVisible True if cuts are shaded.
 */
void RenderingMode::ShowCutsShaded::set( bool bShaded )
{
  m_pRenderingMode->SetEnabled( vxRenderingMode::SHOW_CUTS_SHADED, bShaded );
  m_pRenderingMode->Modified( vxModInfo( vxRenderingMode, vxRenderingMode::SHOW_CUTS_SHADED ) );
} // set_ShowCutsShaded()


/**
 * Gets the show cuts grayscale mode.
 * @return True if cuts are shown in grayscale.
 */
bool RenderingMode::ShowCutsGrayscale::get()
{
  return m_pRenderingMode->IsEnabled( vxRenderingMode::SHOW_CUTS_GRAYSCALE );
} // get_ShowCutsGrayscale()


/**
 * Gets the show cuts grayscale mode.
 * @param bGrayscale True if cuts are shown in grayscale.
 */
void RenderingMode::ShowCutsGrayscale::set( bool bGrayscale )
{
  m_pRenderingMode->SetEnabled( vxRenderingMode::SHOW_CUTS_GRAYSCALE, bGrayscale );
  m_pRenderingMode->Modified( vxModInfo( vxRenderingMode, vxRenderingMode::SHOW_CUTS_GRAYSCALE ) );
} // set_ShowCutsGrayscale()


/**
 * Gets the patient data visible mode.
 * @return True if patient data is visible; false otherwise.
 */
bool RenderingMode::PatientDataVisible::get()
{
  return m_pRenderingMode->IsEnabled( vxRenderingMode::SHOW_PATIENT_INFORMATION );
} // get_PatientDataVisible()


/**
 * Sets the patient data visible mode.
 * @param bVisible True if patient data is visible; false otherwise.
 */
void RenderingMode::PatientDataVisible::set( bool bVisible )
{
  m_pRenderingMode->SetEnabled( vxRenderingMode::SHOW_PATIENT_INFORMATION, bVisible );
  m_pRenderingMode->Modified( vxModInfo(vxRenderingMode, vxRenderingMode::SHOW_PATIENT_INFORMATION ) );
} // set_PatientDataVisible()


/** 
 * Gets the image data visible mode.
 * @return True if patient data is visible; false otherwise.
 */
bool RenderingMode::ImageDataVisible::get()
{
  return m_pRenderingMode->IsEnabled( vxRenderingMode::SHOW_IMAGE_INFORMATION );
} // get_ImageDataVisible()


/** 
 * Sets the image data visible mode.
 * @param bVisible True if patient data is visible; false otherwise.
 */
void RenderingMode::ImageDataVisible::set( bool bVisible )
{
  m_pRenderingMode->SetEnabled( vxRenderingMode::SHOW_IMAGE_INFORMATION, bVisible );
  m_pRenderingMode->Modified( vxModInfo(vxRenderingMode, vxRenderingMode::SHOW_IMAGE_INFORMATION ) );
} // set_ImageDataVisible()


/** 
 * Gets the label on top mode.
 * @return True if labels are on top; false otherwise.
 */
bool RenderingMode::LabelOnTop::get()
{
  return m_pRenderingMode->IsEnabled( vxRenderingMode::SHOW_LABEL_ON_TOP );
} // get_LabelOnTop()


/** 
 * Sets the label on top mode.
 * @param bVisible True if labels are on top; false otherwise.
 */
void RenderingMode::LabelOnTop::set( bool bVisible )
{
  m_pRenderingMode->SetEnabled( vxRenderingMode::SHOW_LABEL_ON_TOP, bVisible );
  m_pRenderingMode->Modified( vxModInfo( vxRenderingMode, vxRenderingMode::SHOW_LABEL_ON_TOP ) );
} // set_LabelOnTop()

/** 
 * Gets the slab thickness.
 * @return Slab thickness.
 */
float RenderingMode::SlabThickness::get()
{
  return m_pRenderingMode->GetSlabThickness();
} // get_SlabThickness()


/** 
 * Sets the slab thickness.
 * @param fThickness Slab thickness.
 */
void RenderingMode::SlabThickness::set( float fThickness )
{
  m_pRenderingMode->SetSlabThickness( fThickness );
  m_pRenderingMode->Modified( vxModInfo( vxRenderingMode, vxUtils::RENDERINGMODE_SLAB_THICKESSS_MODIFIED ) );
} // set_SlabThickness()


/** 
 * Gets the crop box visible mode.
 * @return True if crop box is visible; false otherwise.
 */
bool RenderingMode::CropBoxVisible::get()
{
  return m_pRenderingMode->IsEnabled( vxRenderingMode::SHOW_CROP_BOX );
} // get_CropBoxVisible()


/** 
 * Sets the crop box visible mode.
 * @return bVisible True if crop box is visible; false otherwise.
 */
void RenderingMode::CropBoxVisible::set( bool bVisible )
{
  m_pRenderingMode->SetEnabled( vxRenderingMode::SHOW_CROP_BOX, bVisible );
  m_pRenderingMode->Modified( vxModInfo( vxRenderingMode, vxRenderingMode::SHOW_CROP_BOX ) );
} // set_CropBoxVisible()


/** 
 * Gets the box of interest visible mode.
 * @return True if box of interest is visible; false otherwise.
 */
bool RenderingMode::ShowBoxOfInterest::get()
{
  return m_pRenderingMode->IsEnabled( vxRenderingMode::SHOW_BOX_OF_INTEREST );
} // get_ShowBoxOfInterest()


/** 
 * Sets the box of interest visible mode.
 * @param bVisible True if box of interest is visible; false otherwise.
 */
void RenderingMode::ShowBoxOfInterest::set( bool bVisible )
{
  m_pRenderingMode->SetEnabled( vxRenderingMode::SHOW_BOX_OF_INTEREST, bVisible );
  m_pRenderingMode->Modified( vxModInfo( vxRenderingMode, vxRenderingMode::SHOW_BOX_OF_INTEREST ) );
} // set_ShowBoxOfInterest()


/** 
 * Gets the shading mode.
 * @return True if shading is visible; false otherwise.
 */
bool RenderingMode::DisplayShading::get()
{
  return m_pRenderingMode->IsEnabled( vxRenderingMode::SHOW_SHADING );
} // get_DisplayShading()


/** 
 * Sets the shading visible mode.
 * @param bVisible True if shading is visible; false otherwise.
 */
void RenderingMode::DisplayShading::set( bool bDisplayShadows )
{
  m_pRenderingMode->SetEnabled( vxRenderingMode::SHOW_SHADING, bDisplayShadows );
  m_pRenderingMode->Modified( vxModInfo( vxRenderingMode, vxRenderingMode::SHOW_SHADING ) );
} // set_DisplayShading()


/** 
 * Gets the ShowCursor3D flag.
 * @return True if 3D cursor is visible; false otherwise.
 */
bool RenderingMode::ShowCursor3D::get()
{
  return m_pRenderingMode->IsEnabled( vxRenderingMode::SHOW_CURSOR_3D_IN_MPR );
} // get_ShowCursor3D()


/** 
 * Sets the ShowCursor3D flag.
 * @param bVisible True if 3D cursor is visible; false otherwise.
 */
void RenderingMode::ShowCursor3D::set( bool bVisible )
{
  m_pRenderingMode->SetEnabled( vxRenderingMode::SHOW_CURSOR_3D_IN_MPR, bVisible );
  m_pRenderingMode->Modified( vxModInfo( vxRenderingMode, vxRenderingMode::SHOW_CURSOR_3D_IN_MPR ) );
} // set_ShowCursor3D()

  
/** 
 * gets the vessel centerline visible mode
 *
 * @return vessel centerline visible
 */
bool RenderingMode::ShowVesselCenterline::get()
{
  return m_pRenderingMode->IsEnabled( vxRenderingMode::SHOW_VESSEL_CENTERLINE );
} // get_ShowVesselCenterline()


/** 
 * sets the vessel centerline visible mode
 *
 * @param bVisible vessel centerline visible
 */
void RenderingMode::ShowVesselCenterline::set( bool bVisible )
{
  m_pRenderingMode->SetEnabled( vxRenderingMode::SHOW_VESSEL_CENTERLINE, bVisible );
  m_pRenderingMode->Modified( vxModInfo( vxRenderingMode, vxRenderingMode::SHOW_VESSEL_CENTERLINE ) );
} // set_ShowVesselCenterline()


/** 
 * Set rendering quality level
 */
void RenderingMode::RenderingQuality::set( RenderingQualityEnum quality )
{
  m_pRenderingMode->SetRenderingQuality( static_cast< vxRenderingMode::vxRenderingQualityEnum >( quality ));

  m_pRenderingMode->Modified( vxModInfo( vxRenderingMode, vxRenderingMode::SHOW_JITTERING ) );
  m_pRenderingMode->Modified( vxModInfo( vxRenderingMode, vxRenderingMode::SHOW_PRE_INTEGRATION ) );
  m_pRenderingMode->Modified( vxModInfo( vxRenderingMode, vxRenderingMode::SHOW_EVERY_PIXEL ) );
} // SetQuality()


/** 
 * Connects an observer from the object.
 * @param observer vxManagedObserver interested in events.
 */
void RenderingMode::Connect( System::IntPtr observer )
{
  m_pRenderingMode->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect()


/** 
 * Disconnects an observer from the object.
 * @param observer vxManagedObserver interested in events.
 */
void RenderingMode::Disconnect( System::IntPtr  observer )
{
  m_pRenderingMode->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect()


/**
 * Processes modified queue events.
 * @param modifiedList Modified info list.
 */
void RenderingMode::OnModified( System::IntPtr modifiedList )
{
  Changed( this, System::EventArgs::Empty );
} // OnModified()


/**
 * Serializes the rendering mode object to xml.
 * @return xml string representing object.
 */
System::String ^ RenderingMode::ToXml()
{
  try
  {
    vxRenderingMode * pRenderingMode = ToNativeRenderingModePtr( GetElementPtr() );

    // write it
    vxSerializer serializer("RenderingMode");
    serializer.WriteAttribute("version", "2.0");
    serializer.Write(vxSerializer::AddElement(serializer.GetCurrent(), vxSerializer::GetElementName(*pRenderingMode), "rendering mode"), *pRenderingMode);

    return gcnew System::String( serializer.ToString().c_str() );
  }
  catch(ex::VException & e)
  {
    throw gcnew v3DException(&e);
  }
} // ToXml()


/**
 * Creates a new rendering mode object from the specified xml string.
 * @param mpXml xml string
 * @return new rendering mode instance.
 */
RenderingMode ^ RenderingMode::FromXml( System::String ^ mpXml )
{
  try
  {
    // create new colormap object
    RenderingMode ^ mpRenderingMode = gcnew RenderingMode();

    // create xml document and get root element
    vxDOMDocument document = vxDOMDocument::FromXml( ss( mpXml ) );
    vxDOMElement element = document.GetDocumentElement();

    std::string sVersion("1.0");
    vxSerializer::GetAttribute(element, "version", sVersion);

    if (sVersion == "2.0")
    {
      vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(*(mpRenderingMode->m_pRenderingMode)), "rendering mode"), *(mpRenderingMode->m_pRenderingMode));
    }
    return mpRenderingMode;
  } // try
  catch(ex::VException & e)
  {
    throw gcnew v3DException(&e);
  }
} // FromXml()


/**
 * Creates a new rendering mode from the specified xml resource
 * @param mpXmlResource Xml assmebly resource name.
 * @return New rendering mode instance.
 */
RenderingMode ^ RenderingMode::FromResource( System::String ^ mpXmlResource )
{
  try
  {
    XmlTextReader xmlReader( System::Reflection::Assembly::GetCallingAssembly()->GetManifestResourceStream( mpXmlResource ) );
    XmlDocument xmlDocument;
    xmlDocument.Load( %xmlReader );

    return RenderingMode::FromXml( xmlDocument.InnerXml );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // FromResource()


/**
 * Creates a new rendering mode from the specified xml file string
 * @param mpXmlResource Xml file.
 * @return New rendering mode instance.
 */
RenderingMode ^ RenderingMode::FromFile( System::String ^ mpXmlFile )
{
  try
  {
    XmlDocument xmlDocument;
    xmlDocument.Load( mpXmlFile );

    return RenderingMode::FromXml( xmlDocument.InnerXml );
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // FromFile()

/**
 * Save current state of the vizualization preset for later undo.
 *
 * @param mpEnvironment the environment
 */
void RenderingMode::SetRestorePoint( )
{
  vxEnvironment::GetUndoActions().PushAction( new vxUndoActionRenderingMode( m_pRenderingMode ) );
}//SetRestorePoint()


/*
 *Starts the Composite Undo
 */
void RenderingMode::StartCompositeUndo()
{
	vxEnvironment::GetUndoActions().PushAction( new vxUndoActionCompositeBegin());
}//StartCompositeUndo()


/*
 *Ends the Composite Undo
 */
void RenderingMode::EndCompositeUndo()
{
	vxEnvironment::GetUndoActions().PushAction( new vxUndoActionCompositeEnd());
}//EndCompositeUndo


// $Log: v3D_RenderingMode.cpp,v $
// Revision 1.16  2007/08/21 19:40:34  gdavidson
// Added property to show or hide annotations
//
// Revision 1.15  2007/01/22 15:34:44  geconomos
// Issue# 5321: Slice shadows mode reworked
//
// Revision 1.14  2006/12/21 15:12:05  gdavidson
// Issue #5189 & 5106: Moved RenderingQuality functionality to vxRenderingMode
//
// Revision 1.13  2006/12/11 16:28:35  romy
// Added CompositeUndo for rendering Mode
//
// Revision 1.12  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.11  2006/10/03 20:14:46  geconomos
// added SliceShadowMode enumeration to replace old bit flag
//
// Revision 1.10  2006/09/27 17:23:48  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.9  2006/04/27 19:15:51  romy
// UnDoAction Reset method
//
// Revision 1.8  2006/02/24 20:14:19  frank
// added undo for rendering mode and colormap
//
// Revision 1.7  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.6  2005/11/18 21:12:28  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.5.2.6  2005/08/04 18:57:09  frank
// removed unused cursor 3D tracking
//
// Revision 1.5.2.5  2005/08/04 18:30:56  frank
// Issue #4330: Moved cursor tracking from the 3D renderer to a controller
//
// Revision 1.5.2.4  2005/08/01 18:56:52  frank
// non-pre-integrated is now working again
//
// Revision 1.5.2.3  2005/07/29 13:03:52  frank
// non-preintegrated looks like crap right now - this will be looked at next
//
// Revision 1.5.2.2  2005/07/25 18:26:30  frank
// Issue #4308: Fixed intensity range display option
//
// Revision 1.5.2.1  2005/07/08 15:33:51  frank
// added rendering option for unshaded and uncolored edge, crop, slice cuts
//
// Revision 1.5  2005/04/12 15:23:52  geconomos
// changed xml library to the vxDOM classes
//
// Revision 1.4  2005/03/21 20:51:09  michael
// Gave rendering mode enum elements leading "RENDERINGMODE_" to make
// it unique. The actual enums unfortunately do not quality the scope.
//
// Revision 1.3  2005/03/08 19:09:47  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.2.2.1  2005/02/17 20:59:18  geconomos
// implemented ICloneable
//
// Revision 1.2  2004/10/05 14:42:00  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.36  2004/03/02 01:10:43  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.35.2.1  2004/02/24 20:11:58  michael
// coding standard: assign NULL after deletion and m_pOberserver --> m_pObserver
//
// Revision 1.35  2003/12/16 13:26:59  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.34  2003/11/19 14:15:40  frank
// Added initializer from other managed object
//
// Revision 1.33  2003/11/11 12:35:23  geconomos
// Wrapped vxRenderingMode's Reset method.
//
// Revision 1.32  2003/11/10 21:17:57  frank
// Fixing MIP slider events
//
// Revision 1.31  2003/11/06 20:44:03  michael
// Fixed rendering mode related issues, renamed the "algorithm" to be the mode,
// and the display options to be the options so that things are consistent. Also
// fixed the progressive refinement that broke because George forgot to add
// the assignment of the mode in vxRenderingMode::operator=()
//
// Revision 1.30  2003/11/05 01:18:45  geconomos
// Updated to use new vxRenderingAlgorithmEnum
//
// Revision 1.29  2003/09/03 17:16:58  frank
// Added slice shadow clipping mode
//
// Revision 1.28  2003/08/27 14:35:55  michael
// reworked the rendering mode by sorting flags into groups
//
// Revision 1.27  2003/08/20 14:31:12  michael
// added "mailto:"
//
// Revision 1.26  2003/08/13 20:03:06  frank
// Added vessel centerline display option
//
// Revision 1.25  2003/06/03 12:25:06  michael
// added Angio (XA) presets for Shimadzu that are sensitive to a specific DICOM entry
//
// Revision 1.24  2003/05/23 20:13:20  michael
// added image information display flag
//
// Revision 1.23  2003/05/16 13:13:55  frank
// formatting
//
// Revision 1.22  2003/05/13 13:28:46  michael
// code review
//
// Revision 1.21  2003/05/12 18:40:41  geconomos
// Coding standards.
//
// Revision 1.20  2003/04/01 18:01:38  geconomos
// Coding convention for event naming.
//
// Revision 1.19  2003/03/07 20:35:10  geconomos
// Added ShowBoxOfInterest property.
//
// Revision 1.18  2003/02/14 19:22:53  frank
// Fixed interface to access rendering mode encapsulated object
//
// Revision 1.17  2002/12/20 16:12:16  frank
// Added x-ray projection
//
// Revision 1.16  2002/12/06 19:22:17  ingmar
// added color used in MPR
//
// Revision 1.15  2002/11/25 13:54:41  geconomos
// Removed uneedde reminders for serialization.
//
// Revision 1.14  2002/11/14 14:51:18  geconomos
// added ShowCursor3D
//
// Revision 1.13  2002/11/12 23:55:48  michael
// added constructor from vxRenderingMode and inital To/FromXml()
//
// Revision 1.12  2002/11/12 21:31:01  geconomos
// All standard Environment elements now implement IElement.
//
// Revision 1.11  2002/11/11 19:19:07  geconomos
// Reworked serialization.
//
// Revision 1.10  2002/11/07 17:23:55  geconomos
// Added TrackCursor3D property.
//
// Revision 1.9  2002/11/05 20:00:57  frank
// Enabled crop box
//
// Revision 1.8  2002/11/04 14:30:36  geconomos
// new toolbar class.
//
// Revision 1.7  2002/11/01 20:30:49  frank
// typo
//
// Revision 1.6  2002/11/01 20:09:08  frank
// Added SHOW_CROP_BOX
//
// Revision 1.5  2002/11/01 19:56:30  frank
// Added Crop Box display mode.
//
// Revision 1.4  2002/10/03 19:12:54  dmitry
// Modified event called.
//
// Revision 1.3  2002/08/16 04:32:10  frank
// Added LABEL_TOP_TOP mode.
//
// Revision 1.2  2002/07/09 20:14:24  dmitry
// implementation added
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_RenderingMode.cpp,v 1.16 2007/08/21 19:40:34 gdavidson Exp $
// $Id: v3D_RenderingMode.cpp,v 1.16 2007/08/21 19:40:34 gdavidson Exp $
