// $Id: v3D_Preset.cpp,v 1.10.8.2 2009/07/29 19:54:06 kchalupa Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (mailto:meissner@viatronix.com)


// includes
#include "StdAfx.h"
#include "v3D_Preset.h"
#include "v3D_Camera.h"
#include "v3D_VisualizationPreset.h"
#include "vxSerializer.h"


// namespaces
USING_VISUALIZATION_NS
using namespace System;
using namespace System::Xml;


//defines
#define FILE_REVISION "$Revision: 1.10.8.2 $"


/**
 * Default constructor
 */
Preset::Preset()
{
  m_pPreset = new vxShareableObject<vxPreset>();
  //m_pPreset->Connect( m_pObserver );
} // Preset()


/**
 * destructor
 */
Preset::~Preset()
{
  this->!Preset();
} // ~Preset()


/**
 * Finalizer
 */
Preset::!Preset()
{
  if (m_pPreset != NULL)
  {
    //m_pPreset->Disconnect( m_pObserver );
    delete m_pPreset;
    m_pPreset = NULL;
  }  
} // !Preset()


/**
 * Compare preset to other one using alphabetical order of modality, body part
 *
 * @param preset  Preset to compare to
 */
int Preset::CompareTo( Viatronix::Visualization::Preset ^ preset )
{
  if (Modality->CompareTo(preset->Modality) > 0)
  {
    return 1;
  }
  else
  {
    if (Modality->CompareTo(preset->Modality) == 0)
    {
      return BodyPart->CompareTo(preset->BodyPart);
    }
    else
    {
      return -1;
    }
  }
} // CompareTo( preset )


/**
 * Creates a XML string of the Preset
 *
 * @return  String representation
 */
System::String ^ Preset::ToString()
{
  return this->Name;
} // ToString()


/**
 * Returns name of camera
 *
 * @param int iIndex
 */
System::String ^ Preset::GetNameOfCamera(int iIndex)
{
  return gcnew System::String( m_pPreset->GetCameraPresets()[iIndex].GetName().c_str() );
} // GetNameOfCamera( iIndex )


/**
 * Serializes the preset object to xml.
 *
 * @return xml string representing object.
 */
System::String ^ Preset::ToXml()
{
  try
  {
    vxSerializer serializer("vxPreset");
    serializer.Write(serializer.GetDocument().GetDocumentElement(), *m_pPreset);
    return gcnew System::String( serializer.ToString().c_str() );
  } // try
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // ToXml()


/**
 * Creates a new preset object from the specified xml string.
 *
 * @param xml xml string
 * @return preset object
 */
Preset ^ Preset::FromXml( System::String ^ mpXml )
{
  bool bSuccess(false);

  try
  { 
    // create new preset object
    Preset ^ mpPreset = gcnew Preset();
    vxPreset * pPreset = new vxPreset;

    // create xml document and get root element
    vxDOMDocument document = vxDOMDocument::FromXml( ss( mpXml ) );
    vxDOMElement element = document.GetDocumentElement();
    
    bSuccess = vxSerializer::Read(element, *pPreset);

   *mpPreset->m_pPreset = *pPreset;
    return mpPreset;
  } // try
  catch ( ex::VException & e )
  {
    throw gcnew v3DException( & e );
  }

  return nullptr;
} // FromXml( mpXml )


/**
 * Creates a new preset from the specified xml resource.
 *
 * @param xml xml string
 * @return preset object
 */
Preset ^ Preset::FromResource(System::String ^ xmlResource)
{
  try
  {
    XmlTextReader xmlReader(System::Reflection::Assembly::GetCallingAssembly()->GetManifestResourceStream(xmlResource));
    
    XmlDocument xmlDocument;
    xmlDocument.Load( %xmlReader );

    return Preset::FromXml( xmlDocument.InnerXml );
  }
  catch(ex::VException & e)
  {
    throw gcnew v3DException( &e );
  }
} // FromResource( xmlResource )


/**
 * Creates a new preset from the specified xml file string.
 *
 * @param xml xml string
 * @return preset object
 */
Preset ^ Preset::FromFile( System::String ^ mpXmlFile )
{
  try
  {
    XmlDocument xmlDocument;
    xmlDocument.Load( mpXmlFile );

    return Preset::FromXml( xmlDocument.InnerXml );
  }
  catch(ex::VException & e)
  {
    throw gcnew v3DException(&e);
  }
} // FromFile( mpXmlFile )


/**
 * GetCamera
 *
 * @param iIndex
 * @return camera object
 */
Viatronix::Visualization::Camera ^ Preset::GetCamera(int iIndex)
{
  int4 TODO_REVISIT_PRESET_GETCAMERA;
  Camera ^ mpCamera = gcnew Camera();
  vxCamera * pCamera = reinterpret_cast< vxShareableObject< vxCamera > * >( mpCamera->GetElementPtr().ToPointer() );
  *pCamera = m_pPreset->GetCameraPresets()[iIndex].GetCamera();
  return mpCamera;
} // GetCamera( iIndex )


/**
 * GetVisualizationPreset2D
 *
 * @param iIndex
 * @return VisualizationPreset object
 */
Viatronix::Visualization::VisualizationPreset ^ Preset::GetVisualizationPreset2D(int iIndex)
{
  if (GetNumberOfVisualizationPresets2D() == 0)
  {
    return nullptr;
  }

  int4 TODO_REVISIT_PRESET_GETVISUALIZATION2D;
  
  VisualizationPreset ^ mpVisualizationPreset = gcnew VisualizationPreset();
  
  vxShareableObject< vxVisualizationPreset > * pVisualizationPreset = reinterpret_cast< vxShareableObject< vxVisualizationPreset > * >( mpVisualizationPreset->GetElementPtr().ToPointer() );
    
  *pVisualizationPreset = *m_pPreset->GetVisualizationPresets2D()[iIndex];
  
  return mpVisualizationPreset;
} // GetVisualizationPreset2D( iIndex )


/**
 * Adds a VisualizationPreset 2D
 *
 * @param       mpPreset        VisualizationPreset
 */
void Preset::AddVisualizationPreset2D( Viatronix::Visualization::VisualizationPreset ^ mpPreset )
{
  // create a new instance of the VisualizationPreset class
  vxVisualizationPreset * pCopy = new vxVisualizationPreset();
  
  // get the underlying implementation pointer  
  vxVisualizationPreset * pOriginal = dynamic_cast< vxVisualizationPreset * >( ToNativeVisualizationPresetPtr( mpPreset->GetElementPtr() ));

  // make a copy of the underying implementations
  *pCopy = *dynamic_cast< vxVisualizationPreset * >( pOriginal );

  m_pPreset->AddVisualizationPresets2D( *pCopy );

} // AddVisualizationPreset2D( mpPreset )


/**
 * GetVisualizationPreset3D
 *
 * @param iIndex
 * @return VisualizationPreset object
 */
Viatronix::Visualization::VisualizationPreset ^ Preset::GetVisualizationPreset3D(int iIndex)
{
  if (GetNumberOfVisualizationPresets3D() == 0)
  {
    return nullptr;
  }

  int4 TODO_REVISIT_PRESET_GETVISUALIZATION3D;
  
  VisualizationPreset ^ mpVisualizationPreset = gcnew VisualizationPreset();
  
  vxShareableObject< vxVisualizationPreset > * pVisualizationPreset = reinterpret_cast< vxShareableObject< vxVisualizationPreset > * >( mpVisualizationPreset->GetElementPtr().ToPointer() );
  
  *pVisualizationPreset = *m_pPreset->GetVisualizationPresets3D()[iIndex];
  
  return mpVisualizationPreset;
} // GetVisualizationPreset3D( iIndex )


/**
 * Adds a VisualizationPreset 3D
 *
 * @param       mpPreset        VisualizationPreset
 */
void Preset::AddVisualizationPreset3D( Viatronix::Visualization::VisualizationPreset ^ mpPreset )
{
  // create a new instance of the VisualizationPreset class
  vxVisualizationPreset * pCopy = new vxVisualizationPreset();
  
  // get the underlying implementation pointer  
  vxVisualizationPreset * pOriginal = dynamic_cast< vxVisualizationPreset * >( ToNativeVisualizationPresetPtr( mpPreset->GetElementPtr() ));

  // make a copy of the underying implementations
  *pCopy = *dynamic_cast< vxVisualizationPreset * >( pOriginal );

  m_pPreset->AddVisualizationPresets3D( *pCopy );
} // AddVisualizationPreset3D( mpPreset )


//undefines
#undef FILE_REVISION


// Revision History:
// $Log: v3D_Preset.cpp,v $
// Revision 1.10.8.2  2009/07/29 19:54:06  kchalupa
// Code Walkthrough
//
// Revision 1.10.8.1  2009/03/27 16:31:59  gdavidson
// Added AddVisualizationPreset2D and AddVisualizationPreset3D methods
//
// Revision 1.10  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.9  2006/09/27 17:23:48  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.8  2005/04/12 15:23:52  geconomos
// changed xml library to the vxDOM classes
//
// Revision 1.7  2005/03/08 19:09:47  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.6.2.2  2005/02/17 21:00:58  geconomos
// code cleanup
//
// Revision 1.6.2.1  2005/02/10 20:57:23  michael
// Introduced vxCameraPreset with BoundingBox, name (to be displayed in GUI),
// and new Preset xml version (2.1). Converted presets, except for PET and XA
// ones. Added V3D Utils function and added a call to it to adjust the camera
// relative to the bounding box of the (camera) preset once it is assigned from
// the preset to the environment camera object (CopyFrom()).
//
// Revision 1.6  2005/01/07 16:53:25  gdavidson
// Modified preset implementation.
//
// Revision 1.5  2004/10/29 20:18:46  frank
// formatting
//
// Revision 1.4  2004/04/26 19:02:50  michael
// made vxPreset/V3D Preset a first class citizen of the environment
//
// Revision 1.3  2004/04/16 20:32:52  frank
// fixed reading visualization presets
//
// Revision 1.2  2004/04/14 18:48:53  frank
// Extended presets to set the rendering mode and other visualization settings
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.22  2004/03/02 01:10:44  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.21.4.1  2004/02/24 20:09:05  michael
// took out code that was commented out
//
// Revision 1.21  2003/06/03 12:25:06  michael
// added Angio (XA) presets for Shimadzu that are sensitive to a specific DICOM entry
//
// Revision 1.20  2003/05/16 13:11:05  frank
// formatting
//
// Revision 1.19  2003/05/12 19:10:54  michael
// code review
//
// Revision 1.18  2003/03/30 21:01:11  michael
// extended presets to have 2D and 3D presets
//
// Revision 1.17  2003/03/07 20:35:30  geconomos
// Added InitialColormap2D to preset class.
//
// Revision 1.16  2002/11/27 19:48:25  michael
// rearranged the ToXml(0 so that it is easier to edit because initial camera and
// colormap index are at the begining of the stream
//
// Revision 1.15  2002/11/12 23:54:28  michael
// added inital To/FromXml()
//
// Revision 1.14  2002/11/11 19:19:07  geconomos
// Reworked serialization.
//
// Revision 1.13  2002/11/07 13:18:08  michael
// adjusted sorting to be alphabetical but also take "default, default" as the first one in the list.
// Additionally, updated default preset to match current XML version.
//
// Revision 1.12  2002/11/06 23:31:53  michael
// added compare method for preset: CompareTo()
//
// Revision 1.11  2002/10/24 20:16:21  michael
// added initial preset and colormap indices
//
// Revision 1.10  2002/10/24 01:32:45  michael
// added ToString method
//
// Revision 1.9  2002/10/22 15:25:20  michael
// further progress on presets: Can now set colormap & preset but it does not
// yet get reflected in vxBase ... somehow. :-(
//
// Revision 1.8  2002/10/21 15:42:29  geconomos
// Fixed bug in reading Xml from resource.
//
// Revision 1.7  2002/10/19 20:52:41  michael
// added modality and organ string
//
// Revision 1.6  2002/10/18 18:52:19  michael
// fixed initialization of preset from XML string/stream
//
// Revision 1.5  2002/10/18 17:22:22  geconomos
// Removed debugging code.
//
// Revision 1.4  2002/10/17 15:28:49  geconomos
// Clean up and worked on FromString() static method.
//
// Revision 1.3  2002/10/17 13:14:18  michael
// changed access to vxSerializer to have less interface methods using directly
// XML elements.
//
// Revision 1.2  2002/10/16 23:14:05  michael
// added method to initialize from a XML document passed in as string
//
// Revision 1.1  2002/10/16 22:19:00  michael
// added v3D_Preset for the administration of the presets
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Preset.cpp,v 1.10.8.2 2009/07/29 19:54:06 kchalupa Exp $
// $Id: v3D_Preset.cpp,v 1.10.8.2 2009/07/29 19:54:06 kchalupa Exp $
