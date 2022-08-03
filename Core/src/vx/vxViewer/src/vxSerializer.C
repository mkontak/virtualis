// $Id: vxSerializer.C,v 1.30.2.1 2009/05/01 21:43:54 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (mailto:meissner@viatronix.com)


// includes
#include "stdafx.h"
#include "vxSerializer.h"
#include "utilities.h"
#include "vxCameraPreset.h"
#include "vxVisualizationPreset.h"
#include "vxColormap.h"
#include "vxWindowLevel.h"
#include "vxRenderingMode.h"
#include "vxComponent.h"
#include "vxComponentArray.h"
#include "vxPreset.h"
#include "vxVisualizationPreset.h"
#include "vxHandle.h"
#include "vxVesselSection.h"
#include "MemoryBuffer.h"
#include "Timer.h"


// pragmas
#pragma unmanaged


// define declarations
#define FILE_REVISION "$Revision: 1.30.2.1 $"


// namespaces
using namespace std;


/** 
 * Constructor
 * @param sTag Tag name of the root element.
 */
vxSerializer::vxSerializer( const std::string & sTagName ) : m_document( sTagName )
{
  m_current = m_document.GetDocumentElement();
} // constructor


/** 
 * Initializes from a string
 */
void vxSerializer::FromString( std::string sContent )
//*******************************************************************
{
  m_document = vxDOMDocument::FromXml( sContent );
  
  m_current = m_document.GetDocumentElement();
} // FromString()


/** 
 * Returns a string representation of the serialized data.
 */
std::string vxSerializer::ToString()
//*******************************************************************
{
  return m_document.ToString();
} // ToString()


//*******************************************************************
//*******************************************************************
//
//                        HELPER methods
//
//*******************************************************************
//*******************************************************************


/**
 * Adds a node
 * @param node
 * @param sTagName Name of element to find
 * @return DOM node ptr, NULL if unsuccessful
 */
vxDOMElement vxSerializer::AddElement( vxDOMElement & parent, const std::string & sTagName, const std::string & sMember )
{
  vxDOMElement element = parent.GetOwnerDocument().CreateElement( sTagName );
  if( sMember != "" )
    SetAttribute( element, "member", sMember );
  
  return parent.AppendChild( element ).ToElement();
} // AddElement()


/**
 * Finds a child element with the spcified tag name.
 * @param element
 * @return DOM element ptr, NULL if unsuccessful
 */
vxDOMElement vxSerializer::FindElementByMember( vxDOMElement & parentElement, const std::string & sMember )
{
  vxDOMElement foundElement;

  vxDOMNode childNode( parentElement.GetFirstChild() );
  while( !childNode.IsNull() )
  {
    vxDOMElement element = childNode.ToElement();
		if ( element.IsNull() )
		{
			childNode = childNode.GetNextSibling();
			continue;
		}
    
    string sValue( "" );
    GetAttribute( element, "member", sValue );
    
    if( sValue == sMember )
    {
      foundElement = element;
      break;
    }
    childNode = childNode.GetNextSibling();
  }
  return foundElement;
} // FindElement()


/**
 * Finds a child element with the spcified tag name.
 * @param element
 * @param sTagName Name of element to find
 * @return DOM element ptr, NULL if unsuccessful
 */
vxDOMElement vxSerializer::FindElement( vxDOMElement & parentElement,
                                              const std::string & sTagName, 
                                              const std::string & sMember)
{
  Timer timer;

  vxDOMElement foundElement;
  vxDOMNode childNode = parentElement.GetFirstChild();


  while( !childNode.IsNull() )
  {
    vxDOMElement element( childNode.ToElement() );

		if ( element.IsNull() )
		{
			childNode = childNode.GetNextSibling();
			continue;
		}
    
    string sValue;
    GetAttribute( element, "member", sValue );
    
    if( element.GetTagName() == sTagName && sValue == sMember )
    {
      foundElement = element;
      break;
    }
    childNode = childNode.GetNextSibling();
  }
  return foundElement;
} // FindElement()


/** 
 * Appends an attribute to the current element
 * @param sName Attribute name.
 * @param sValue Contains the attribute value.
 */
void vxSerializer::WriteAttribute(const std::string & sField,
                                  const std::string & sValue)
//*******************************************************************
{
  SetAttribute( m_current, sField, sValue );
} // WriteAttribute()


/** 
 * Reads an existing attribute from the current element
 * @param sName Attribute name.
 * @param sValue Receives the attribute value.
 * @return true on success, false otherwise
 */
bool vxSerializer::ReadAttribute( const std::string & sField, std::string & sValue )
//*******************************************************************
{
  return GetAttribute( m_current, sField, sValue) ;
} // ReadAttribute()


/** 
 * Reads an existing attribute from the current element
 * @param element
 * @param attribute name
 * @param receives the attribute value
 * @return true on success, false otherwise
 */
bool vxSerializer::GetAttribute(  vxDOMElement & element, 
                                        const std::string & sName, 
                                        std::string & sValue)
//*******************************************************************
{
  Timer timer;

  vxDOMAttribute attribute = element.GetAttributeNode( sName );
  if( attribute.IsNull() )
    return false;

  sValue = attribute.GetValue();

  return true;
} // GetAttribute()


/** 
 * Appends an attribute to the current element
 * @param element
 * @param attribute name
 * @param contains the attribute value
 */
void vxSerializer::SetAttribute(vxDOMElement & element, 
                                const std::string & sField,
                                const std::string & sValue)
//*******************************************************************
{
  element.SetAttribute( sField, sValue );
} // SetAttribute()


/** 
 * Reads an attribute to the current element
 * @param element
 * @param contains the attribute value
 */
bool vxSerializer::GetValue(vxDOMElement & element, 
                            std::string & sValue)
//*******************************************************************
{
  sValue = element.GetNodeValue();
  return true;
} // GetValue()


/** 
 * Appends an attribute to the current element
 * @param element
 * @param contains the attribute value
 */
void vxSerializer::SetValue( vxDOMElement & element, const std::string & sValue )
//*******************************************************************
{
  element.SetNodeValue( sValue );
} // SetValue()


//*******************************************************************
//*******************************************************************
//
//                    static READ/WRITE methods
//
//*******************************************************************
//*******************************************************************


/**
* Write vxHandle
* @param element
* @param component array
*/
void vxSerializer::Write(vxDOMElement & element,
                         const vxHandle & handle)
//*******************************************************************
{
  Point3Df position(handle.GetPosition());
  bool bSelected(handle.IsSelected());
  PixelRGBA<uint1> color(handle.GetColorHandle());
  PixelRGBA<uint1> colorSelected(handle.GetColorHandleSelected());
  float4 fScreenSize(handle.GetHandleScreenSize());

  SetAttribute(element, "version", "2.0");
  Write(AddElement(element, GetElementName(position),       "position"), position);
  Write(AddElement(element, GetElementName(bSelected),      "selected"), bSelected);
  Write(AddElement(element, GetElementName(color),          "color"), color);
  Write(AddElement(element, GetElementName(colorSelected),  "selected color"), colorSelected);
  Write(AddElement(element, GetElementName(fScreenSize),    "screen size"),  fScreenSize);
} // Write(vxHandle)


/**
* Read vxHandle
* @param element
* @param component array
* @return success
*/
bool vxSerializer::Read(vxDOMElement & element, 
                        vxHandle & handle)
//*******************************************************************
{
  if( element.IsNull() ) 
    return false;

  bool bSuccess(true);

  string sVersion("0.0");
  bSuccess &= GetAttribute(element, "version", sVersion);
  if (atof(sVersion.c_str()) == 2.0)
  {
    Point3Df position(0, 0, 0);
    bool bSelected(false);
    PixelRGBA<uint1> color(PixelRGBA<uint1>(128, 255, 192, 255));
    PixelRGBA<uint1> colorSelected(PixelRGBA<uint1>(255, 128, 128, 255));
    float4 fScreenSize(7);

    bSuccess &= vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(position),       "position"), position);
    bSuccess &= vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(bSelected),      "selected"), bSelected);
    bSuccess &= vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(color),          "color"), color);
    bSuccess &= vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(colorSelected),  "selected color"), colorSelected);
    bSuccess &= vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(fScreenSize),    "screen size"),  fScreenSize);

    handle.SetPosition(position);
    handle.SetSelected(bSelected);
    handle.SetColorHandle(color);
    handle.SetColorHandleSelected(colorSelected);
    //handle.SetHandleScreenSize(fScreenSize);
  }
  else
  {
    LogDbg("Unknown version number for XML of vxHandle", "vxSerializer", "Read");
  }

  return bSuccess;
} // Read(vxHandle)


/**
 * Write vxComponentArray
 * @param element
 * @param component array
 */
void vxSerializer::Write(vxDOMElement & element,
                         const vxComponentArray & componentArray)
//*******************************************************************
{
  uint2 iSelected(componentArray.GetSelectedComponentIndex());
  const vector<vxComponent*> vComponentPtr = componentArray.ArrayByLabel();

  vector<vxComponent> vComponent = vxSerializer::CreateRealVectorFromPtr(vComponentPtr);
  map<uint2, uint2> map = componentArray.GetMapLabelToHistory();

  SetAttribute(element, "version", "2.1");
  Write(AddElement(element, vxSerializer::GetElementName(iSelected), "selected"), iSelected);
  Write(AddElement(element, vxSerializer::GetElementName(vComponent), "components"), vComponent);
  Write(AddElement(element, vxSerializer::GetElementName(map), "map"), map);
} // Write(vxComponentArray)


/**
 * Read vxComponentArray
 * @param element
 * @param component array
 * @return success
 */
bool vxSerializer::Read(vxDOMElement & element, 
                        vxComponentArray & componentArray)
//*******************************************************************
{
  if( element.IsNull() ) 
    return false;

  bool bSuccess(true);
  string sVersion("0.0");
  bSuccess &= GetAttribute(element, "version", sVersion);
  const float4 fVersion = atof( sVersion.c_str() );

  // 2.1 is the same as 2.0 except that the background component may not have been called Background
  // and it may not have been able to be removed, grown, and shrunk.
  if ( sVersion == "2.0" || sVersion == "2.1" )
  {
    uint2 iSelected(0);
    vector<vxComponent> vComponent;
    vector<vxComponent *> vComponentPtr;
    map<uint2, uint2> map;

    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(iSelected), "selected"), iSelected);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(vComponent), "components"), vComponent);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(map), "map"), map);

    vComponentPtr = vxSerializer::CreatePtrVectorFromReal(vComponent);
    componentArray.InitializeFromComponents(vComponentPtr, map);
    componentArray.SetSelectedComponentIndex(iSelected);

    // in older version, manually set the background component to "Background" and not-removable, etc.
    if ( sVersion == "2.0" )
    {
      componentArray.ArrayByLabel()[ 0 ]->SetName( "Background" );
      componentArray.ArrayByLabel()[ 0 ]->SetRemovable( false );
      componentArray.ArrayByLabel()[ 0 ]->SetGrowable( false );
      componentArray.ArrayByLabel()[ 0 ]->SetShrinkable( false );
      componentArray.ArrayByLabel()[ 0 ]->SetNameEditable( false );
    }
  }
  else
  {
    LogDbg("Unknown version number for XML of vxComponentArray: " + ToAscii( sVersion ), "vxSerializer", "Read");
  }

  return bSuccess;
} // Read(vxComponentArray)


/**
 * Serializes vxComponent
 * @param element
 * @param component
 */
void vxSerializer::Write(vxDOMElement & element,
                         const vxComponent & component)
//*******************************************************************
{
  const bool bLocked(component.IsLocked());
  const bool bIntensityVisible(component.IsIntensityVisible());
  const bool bLabelVisible(component.IsLabelVisible());
  const uint2 uLabelId(component.GetLabel());
  const string sName(component.GetName());             
  const PixelRGBA<uint1> color(component.GetColor());
  const bool bGrowable(component.GetGrowable());
  const bool bShrinkable(component.GetShrinkable());
  const bool bRemovable(component.GetRemovable());
  const bool bUnlockable(component.GetUnlockable());
  const bool bNameEditable(component.GetNameEditable());
  vxComponentData * pComponentData(component.GetComponentDataPtr());
  const bool bComponentDataAvailable(pComponentData != NULL);

  SetAttribute(element, "version", "3.0");
  Write(AddElement(element, vxSerializer::GetElementName(bLocked), "locked"), bLocked);
  Write(AddElement(element, vxSerializer::GetElementName(bIntensityVisible), "intensity visible"), bIntensityVisible);
  Write(AddElement(element, vxSerializer::GetElementName(bLabelVisible), "label visible"), bLabelVisible);
  Write(AddElement(element, vxSerializer::GetElementName(uLabelId), "label id"), uLabelId);
  Write(AddElement(element, vxSerializer::GetElementName(sName), "name"), sName);
  Write(AddElement(element, vxSerializer::GetElementName(color), "color"), color);
  Write(AddElement(element, vxSerializer::GetElementName(bGrowable), "growable"), bGrowable);
  Write(AddElement(element, vxSerializer::GetElementName(bShrinkable), "shrinkable"), bShrinkable);
  Write(AddElement(element, vxSerializer::GetElementName(bRemovable), "removable"), bRemovable);
  Write(AddElement(element, vxSerializer::GetElementName(bUnlockable), "unlockable"), bUnlockable);
  Write(AddElement(element, vxSerializer::GetElementName(bNameEditable), "name editable"), bNameEditable);
  Write(AddElement(element, vxSerializer::GetElementName(bComponentDataAvailable), "component data available"), bComponentDataAvailable);
  if (bComponentDataAvailable)
  {
    WriteISerialize(AddElement(element, "vxComponentData", "component data"), pComponentData);
  }
} // Write(vxComponent)


/**
 * Deserializes vxComponent
 * @param element
 * @param component
 * @return success
 */
bool vxSerializer::Read(vxDOMElement & element, 
                        vxComponent & component)
//*******************************************************************
{
  if( element.IsNull() ) 
    return false;

  bool bSuccess(true);
  string sVersion("0.0");
  bSuccess &= GetAttribute(element, "version", sVersion);

  if (atof(sVersion.c_str()) == 2.0)
  {
    bool bLocked(false);
    bool bIntensityVisible(false);
    bool bLabelVisible(false);
    uint2 uLabelId;
    string sName("");             
    PixelRGBA<uint1> color;
    bool bGrowable(true);
    bool bShrinkable(true);
    bool bRemovable(true);
    vxComponentData * pComponentData(NULL);

    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bLocked), "locked"), bLocked);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bIntensityVisible), "intensity visible"), bIntensityVisible);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bLabelVisible), "label visible"), bLabelVisible);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(uLabelId), "label id"), uLabelId);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(sName), "name"), sName);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(color), "color"), color);

    component = vxComponent(uLabelId, sName, color, bIntensityVisible, bLabelVisible, bLocked, bSuccess);
    component.SetGrowable(bGrowable);
    component.SetShrinkable(bShrinkable);
    component.SetRemovable(bRemovable);
    component.SetComponentDataPtr(pComponentData);
  }
  else if (atof(sVersion.c_str()) == 3.0)
  {
    bool bLocked(false);
    bool bIntensityVisible(false);
    bool bLabelVisible(false);
    uint2 uLabelId;
    string sName("");             
    PixelRGBA<uint1> color;
    bool bGrowable(true);
    bool bShrinkable(true);
    bool bRemovable(true);
    bool bComponentDataAvailable(false);
    bool bUnlockable(true);
    bool bNameEditable(true);
    vxComponentData * pComponentData(NULL);

    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bLocked), "locked"), bLocked);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bIntensityVisible), "intensity visible"), bIntensityVisible);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bLabelVisible), "label visible"), bLabelVisible);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(uLabelId), "label id"), uLabelId);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(sName), "name"), sName);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(color), "color"), color);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bGrowable), "growable"), bGrowable);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bShrinkable), "shrinkable"), bShrinkable);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bRemovable), "removable"), bRemovable);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bUnlockable), "unlockable"), bUnlockable);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bNameEditable), "name editable"), bNameEditable);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bComponentDataAvailable), "component data available"), bComponentDataAvailable);

    if (bComponentDataAvailable)
    {
      bSuccess &= ReadISerialize(FindElement(element, "vxComponentData", "component data"), & pComponentData);
    }

    if ( bSuccess )
    {
      component = vxComponent(uLabelId, sName, color, bIntensityVisible, bLabelVisible, bLocked, bSuccess);
      component.SetGrowable(bGrowable);
      component.SetShrinkable(bShrinkable);
      component.SetRemovable(bRemovable);
      component.SetUnlockable(bUnlockable);
      component.SetNameEditable(bNameEditable);
      component.SetComponentDataPtr(pComponentData);
    }
    else
    {
      LogWrn( "unable to read vxComponent XML", "vxSerializer", "Read" );
    }
  }

  else
  {
    LogDbg("Unknown version number for XML of vxComponent", "vxSerializer", "Read");
  }

  return bSuccess;
} // Read(vxComponent)


/**
 * Serializes vxRenderingMode
 * @param element
 * @param rendering mode
 */
void vxSerializer::Write(vxDOMElement & element,
                         const vxRenderingMode & renderingMode)
//*******************************************************************
{
  int iOption(renderingMode.GetOptions());
  float fSlabThickness(renderingMode.GetSlabThickness());
  bool bOptionPatientInformation( renderingMode.IsEnabled( vxRenderingMode::SHOW_PATIENT_INFORMATION ) );
  bool bOptionImageInformation( renderingMode.IsEnabled( vxRenderingMode::SHOW_IMAGE_INFORMATION ) );
  bool bOptionVesselCenterline( renderingMode.IsEnabled( vxRenderingMode::SHOW_VESSEL_CENTERLINE ) );
  bool bOptionComponentMPR( renderingMode.IsEnabled( vxRenderingMode::SHOW_COMPONENT_IN_MPR ) );
  bool bOptionColormapMPR( renderingMode.IsEnabled( vxRenderingMode::SHOW_COLORMAP_IN_MPR ) );
  bool bOptionCursor3D( renderingMode.IsEnabled( vxRenderingMode::SHOW_CURSOR_3D_IN_MPR ) );
  bool bOptionIntensityRange( renderingMode.IsEnabled( vxRenderingMode::SHOW_INTENSITY_RANGE ) );  
  bool bOptionLabelOnTop( renderingMode.IsEnabled( vxRenderingMode::SHOW_LABEL_ON_TOP ) );
  bool bOptionCropBox( renderingMode.IsEnabled( vxRenderingMode::SHOW_CROP_BOX ) );
  bool bOptionBoxOfInterest( renderingMode.IsEnabled( vxRenderingMode::SHOW_BOX_OF_INTEREST ) );
  bool bOptionAutoFlyMode( renderingMode.IsEnabled( vxRenderingMode::AUTO_FLY_MODE ) );
  bool bOptionJittering( renderingMode.IsEnabled( vxRenderingMode::SHOW_JITTERING ) );
  bool bOptionPreIntegration( renderingMode.IsEnabled( vxRenderingMode::SHOW_PRE_INTEGRATION ) );
  bool bOptionShading( renderingMode.IsEnabled( vxRenderingMode::SHOW_SHADING ) );
  bool bOptionShadows( renderingMode.IsEnabled( vxRenderingMode::SHOW_SHADOWS ) );
  bool bOptionEveryPixel( renderingMode.IsEnabled( vxRenderingMode::SHOW_EVERY_PIXEL ) );
  bool bOptionCutsShaded( renderingMode.IsEnabled( vxRenderingMode::SHOW_CUTS_SHADED ) );
  bool bOptionCutsGrayscale( renderingMode.IsEnabled( vxRenderingMode::SHOW_CUTS_GRAYSCALE ) );
  
  string sAlgorithm("DEFAULT");
  if (renderingMode.GetRenderingAlgorithm() == vxRenderingMode::THIN_SLAB)
    sAlgorithm = "THIN SLAB";
  if (renderingMode.GetRenderingAlgorithm() == vxRenderingMode::MIP)
    sAlgorithm = "MIP";
  if (renderingMode.GetRenderingAlgorithm() == vxRenderingMode::XRAY)
    sAlgorithm = "XRAY";

  string sSliceShadowMode("SHADOWS_NONE");
  if (renderingMode.GetSliceShadowMode() == vxRenderingMode::SHADOWS_ACTIVE )
    sAlgorithm = "SHADOWS_ACTIVE";
  else if (renderingMode.GetSliceShadowMode() == vxRenderingMode::SHADOWS_ALL )
    sAlgorithm = "SHADOWS_ALL";


  SetAttribute(element, "version", "3.1");
  Write(AddElement(element, vxSerializer::GetElementName(sAlgorithm), "algorithm"), sAlgorithm);
  Write(AddElement(element, vxSerializer::GetElementName(sAlgorithm), "slice shadow mode"), sSliceShadowMode);
  Write(AddElement(element, vxSerializer::GetElementName(fSlabThickness), "slab thickness"), fSlabThickness);
  Write(AddElement(element, vxSerializer::GetElementName(bOptionPatientInformation), "show patient information"), bOptionPatientInformation);
  Write(AddElement(element, vxSerializer::GetElementName(bOptionImageInformation), "show image information"), bOptionImageInformation);
  Write(AddElement(element, vxSerializer::GetElementName(bOptionVesselCenterline), "show vessel centerline"), bOptionVesselCenterline);
  Write(AddElement(element, vxSerializer::GetElementName(bOptionComponentMPR), "show component MPR"), bOptionComponentMPR);
  Write(AddElement(element, vxSerializer::GetElementName(bOptionColormapMPR), "show colormap MPR"), bOptionColormapMPR);
  Write(AddElement(element, vxSerializer::GetElementName(bOptionCursor3D), "show cursor 3D"), bOptionCursor3D);
  Write(AddElement(element, vxSerializer::GetElementName(bOptionIntensityRange), "show intensity range"), bOptionIntensityRange);
  Write(AddElement(element, vxSerializer::GetElementName(bOptionLabelOnTop), "show label on top"), bOptionLabelOnTop);
  Write(AddElement(element, vxSerializer::GetElementName(bOptionCropBox), "show crop box"), bOptionCropBox);
  Write(AddElement(element, vxSerializer::GetElementName(bOptionBoxOfInterest), "show box of interest"), bOptionBoxOfInterest);
  Write(AddElement(element, vxSerializer::GetElementName(bOptionAutoFlyMode), "auto fly mode"), bOptionAutoFlyMode);
  Write(AddElement(element, vxSerializer::GetElementName(bOptionJittering), "show jittering"), bOptionJittering);
  Write(AddElement(element, vxSerializer::GetElementName(bOptionPreIntegration), "show pre-integration"), bOptionPreIntegration);
  Write(AddElement(element, vxSerializer::GetElementName(bOptionShading), "show shading"), bOptionShading);
  Write(AddElement(element, vxSerializer::GetElementName(bOptionShadows), "show shadows"), bOptionShadows);
  Write(AddElement(element, vxSerializer::GetElementName(bOptionEveryPixel), "show every pixel"), bOptionEveryPixel);
  Write(AddElement(element, vxSerializer::GetElementName(bOptionCutsShaded), "show cuts shaded"), bOptionCutsShaded);
  Write(AddElement(element, vxSerializer::GetElementName(bOptionCutsGrayscale), "show cuts grayscale"), bOptionCutsGrayscale);

} // Write(vxRenderingMode)


/**
 * Deserializes vxRenderingMode
 * @param element
 * @param rendering mode
 * @return success
 */
bool vxSerializer::Read(vxDOMElement & element, 
                        vxRenderingMode & renderingMode)
//*******************************************************************
{
  if( element.IsNull() ) 
    return false;

  bool bSuccess(true);
  string sVersion("0.0");
  bSuccess &= GetAttribute(element, "version", sVersion);
  if ( sVersion == "3.1" )
  {
    vxRenderingMode::vxRenderingAlgorithmEnum eAlgorithm;
    string sAlgorithm;
    vxRenderingMode::vxSliceShadowModeEnum eSliceShadowMode;
    string sSliceShadowMode;
    float fSlabThickness;
    bool bOptionPatientInformation( false );
    bool bOptionImageInformation( false );
    bool bOptionVesselCenterline( false );
    bool bOptionComponentMPR( false );
    bool bOptionColormapMPR( false );
    bool bOptionCursor3D( false );
    bool bOptionIntensityRange( false );
    bool bOptionLabelOnTop( false );
    bool bOptionCropBox( false );
    bool bOptionBoxOfInterest( false );
    bool bOptionShadowClipped( false );
    bool bOptionAutoFlyMode( false );
    bool bOptionJittering( false );
    bool bOptionPreIntegration( false );
    bool bOptionShading( false );
    bool bOptionShadows( false );
    bool bOptionEveryPixel( false );
    bool bOptionCutsShaded( false );
    bool bOptionCutsGrayscale( false );

    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(sAlgorithm), "algorithm"), sAlgorithm);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(sSliceShadowMode), "slice shadow mode"), sSliceShadowMode);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(fSlabThickness), "slab thickness"), fSlabThickness);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionPatientInformation), "show patient information"), bOptionPatientInformation);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionImageInformation), "show image information"), bOptionImageInformation);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionVesselCenterline), "show vessel centerline"), bOptionVesselCenterline);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionComponentMPR), "show component MPR"), bOptionComponentMPR);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionColormapMPR), "show colormap MPR"), bOptionColormapMPR);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionCursor3D), "show cursor 3D"), bOptionCursor3D);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionIntensityRange), "show intensity range"), bOptionIntensityRange);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionLabelOnTop), "show label on top"), bOptionLabelOnTop);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionCropBox), "show crop box"), bOptionCropBox);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionBoxOfInterest), "show box of interest"), bOptionBoxOfInterest);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionAutoFlyMode), "auto fly mode"), bOptionAutoFlyMode);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionJittering), "show jittering"), bOptionJittering);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionPreIntegration), "show pre-integration"), bOptionPreIntegration);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionShading), "show shading"), bOptionShading);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionShadows), "show shadows"), bOptionShadows);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionEveryPixel), "show every pixel"), bOptionEveryPixel);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionCutsShaded), "show cuts shaded"), bOptionCutsShaded);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionCutsGrayscale), "show cuts grayscale"), bOptionCutsGrayscale);

    if (sAlgorithm == "DEFAULT")
      eAlgorithm = vxRenderingMode::DEFAULT;
    else if (sAlgorithm == "THIN_SLAB")
      eAlgorithm = vxRenderingMode::THIN_SLAB;
    else if (sAlgorithm == "MIP")
      eAlgorithm = vxRenderingMode::MIP;
    else if (sAlgorithm == "XRAY")
      eAlgorithm = vxRenderingMode::XRAY;
    else
    {
      eAlgorithm = vxRenderingMode::DEFAULT;
      LogDbg("Illegal rendering mode, using default", "vxSerializer", "Read");
    }


    if (sAlgorithm == "SHADOWS_NONE")
      eSliceShadowMode= vxRenderingMode::SHADOWS_NONE;
    else if (sAlgorithm == "SHADOWS_ACTIVE")
      eSliceShadowMode= vxRenderingMode::SHADOWS_ACTIVE;
    else if (sAlgorithm == "SHADOWS_ALL")
      eSliceShadowMode= vxRenderingMode::SHADOWS_ALL;
    else
    {
      eSliceShadowMode= vxRenderingMode::SHADOWS_NONE;
      LogDbg("Illegal slice shadow mode mode, using default", "vxSerializer", "Read");
    }

    if ( bSuccess )
    {
      renderingMode.SetRenderingAlgorithm(eAlgorithm);
      renderingMode.SetSliceShadowMode(eSliceShadowMode);
      renderingMode.SetSlabThickness(fSlabThickness);
      renderingMode.SetEnabled( vxRenderingMode::SHOW_PATIENT_INFORMATION, bOptionPatientInformation );
      renderingMode.SetEnabled( vxRenderingMode::SHOW_IMAGE_INFORMATION, bOptionImageInformation );
      renderingMode.SetEnabled( vxRenderingMode::SHOW_VESSEL_CENTERLINE, bOptionVesselCenterline );
      renderingMode.SetEnabled( vxRenderingMode::SHOW_COMPONENT_IN_MPR, bOptionComponentMPR );
      renderingMode.SetEnabled( vxRenderingMode::SHOW_COLORMAP_IN_MPR, bOptionColormapMPR );
      renderingMode.SetEnabled( vxRenderingMode::SHOW_CURSOR_3D_IN_MPR, bOptionCursor3D );
      renderingMode.SetEnabled( vxRenderingMode::SHOW_INTENSITY_RANGE, bOptionIntensityRange );
      renderingMode.SetEnabled( vxRenderingMode::SHOW_LABEL_ON_TOP, bOptionLabelOnTop );
      renderingMode.SetEnabled( vxRenderingMode::SHOW_CROP_BOX, bOptionCropBox );
      renderingMode.SetEnabled( vxRenderingMode::SHOW_BOX_OF_INTEREST, bOptionBoxOfInterest );
      renderingMode.SetEnabled( vxRenderingMode::AUTO_FLY_MODE, bOptionAutoFlyMode );
      renderingMode.SetEnabled( vxRenderingMode::SHOW_JITTERING, bOptionJittering );
      renderingMode.SetEnabled( vxRenderingMode::SHOW_PRE_INTEGRATION, bOptionPreIntegration );
      renderingMode.SetEnabled( vxRenderingMode::SHOW_SHADING, bOptionShading );
      renderingMode.SetEnabled( vxRenderingMode::SHOW_SHADOWS, bOptionShadows );
      renderingMode.SetEnabled( vxRenderingMode::SHOW_EVERY_PIXEL, bOptionEveryPixel );
      renderingMode.SetEnabled( vxRenderingMode::SHOW_CUTS_SHADED, bOptionCutsShaded );
      renderingMode.SetEnabled( vxRenderingMode::SHOW_CUTS_GRAYSCALE, bOptionCutsGrayscale );
    }
  }
  else if ( sVersion == "3.0" )
  {
    vxRenderingMode::vxRenderingAlgorithmEnum eAlgorithm;
    string sAlgorithm;
    float fSlabThickness;
    bool bOptionPatientInformation( false );
    bool bOptionImageInformation( false );
    bool bOptionVesselCenterline( false );
    bool bOptionComponentMPR( false );
    bool bOptionColormapMPR( false );
    bool bOptionCursor3D( false );
    bool bOptionIntensityRange( false );
    bool bOptionSliceShadow( false );
    bool bOptionLabelOnTop( false );
    bool bOptionCropBox( false );
    bool bOptionBoxOfInterest( false );
    bool bOptionShadowClipped( false );
    bool bOptionAutoFlyMode( false );
    bool bOptionJittering( false );
    bool bOptionPreIntegration( false );
    bool bOptionShading( false );
    bool bOptionShadows( false );
    bool bOptionEveryPixel( false );
    bool bOptionCutsShaded( false );
    bool bOptionCutsGrayscale( false );

    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(sAlgorithm), "algorithm"), sAlgorithm);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(fSlabThickness), "slab thickness"), fSlabThickness);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionPatientInformation), "show patient information"), bOptionPatientInformation);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionImageInformation), "show image information"), bOptionImageInformation);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionVesselCenterline), "show vessel centerline"), bOptionVesselCenterline);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionComponentMPR), "show component MPR"), bOptionComponentMPR);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionColormapMPR), "show colormap MPR"), bOptionColormapMPR);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionCursor3D), "show cursor 3D"), bOptionCursor3D);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionIntensityRange), "show intensity range"), bOptionIntensityRange);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionSliceShadow), "show slice shadow"), bOptionSliceShadow);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionLabelOnTop), "show label on top"), bOptionLabelOnTop);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionCropBox), "show crop box"), bOptionCropBox);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionBoxOfInterest), "show box of interest"), bOptionBoxOfInterest);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionShadowClipped), "show shadow clipped"), bOptionShadowClipped);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionAutoFlyMode), "auto fly mode"), bOptionAutoFlyMode);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionJittering), "show jittering"), bOptionJittering);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionPreIntegration), "show pre-integration"), bOptionPreIntegration);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionShading), "show shading"), bOptionShading);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionShadows), "show shadows"), bOptionShadows);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionEveryPixel), "show every pixel"), bOptionEveryPixel);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionCutsShaded), "show cuts shaded"), bOptionCutsShaded);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(bOptionCutsGrayscale), "show cuts grayscale"), bOptionCutsGrayscale);

    if (sAlgorithm == "DEFAULT")
      eAlgorithm = vxRenderingMode::DEFAULT;
    else if (sAlgorithm == "THIN_SLAB")
      eAlgorithm = vxRenderingMode::THIN_SLAB;
    else if (sAlgorithm == "MIP")
      eAlgorithm = vxRenderingMode::MIP;
    else if (sAlgorithm == "XRAY")
      eAlgorithm = vxRenderingMode::XRAY;
    else
    {
      eAlgorithm = vxRenderingMode::DEFAULT;
      LogDbg("Illegal rendering mode, using default", "vxSerializer", "Read");
    }

    if ( bSuccess )
    {
      renderingMode.SetRenderingAlgorithm(eAlgorithm);
      renderingMode.SetSliceShadowMode( bOptionSliceShadow? vxRenderingMode::SHADOWS_ALL : vxRenderingMode::SHADOWS_NONE ); 
      renderingMode.SetSlabThickness(fSlabThickness);
      renderingMode.SetEnabled( vxRenderingMode::SHOW_PATIENT_INFORMATION, bOptionPatientInformation );
      renderingMode.SetEnabled( vxRenderingMode::SHOW_IMAGE_INFORMATION, bOptionImageInformation );
      renderingMode.SetEnabled( vxRenderingMode::SHOW_VESSEL_CENTERLINE, bOptionVesselCenterline );
      renderingMode.SetEnabled( vxRenderingMode::SHOW_COMPONENT_IN_MPR, bOptionComponentMPR );
      renderingMode.SetEnabled( vxRenderingMode::SHOW_COLORMAP_IN_MPR, bOptionColormapMPR );
      renderingMode.SetEnabled( vxRenderingMode::SHOW_CURSOR_3D_IN_MPR, bOptionCursor3D );
      renderingMode.SetEnabled( vxRenderingMode::SHOW_INTENSITY_RANGE, bOptionIntensityRange );
      renderingMode.SetEnabled( vxRenderingMode::SHOW_LABEL_ON_TOP, bOptionLabelOnTop );
      renderingMode.SetEnabled( vxRenderingMode::SHOW_CROP_BOX, bOptionCropBox );
      renderingMode.SetEnabled( vxRenderingMode::SHOW_BOX_OF_INTEREST, bOptionBoxOfInterest );
      renderingMode.SetEnabled( vxRenderingMode::AUTO_FLY_MODE, bOptionAutoFlyMode );
      renderingMode.SetEnabled( vxRenderingMode::SHOW_JITTERING, bOptionJittering );
      renderingMode.SetEnabled( vxRenderingMode::SHOW_PRE_INTEGRATION, bOptionPreIntegration );
      renderingMode.SetEnabled( vxRenderingMode::SHOW_SHADING, bOptionShading );
      renderingMode.SetEnabled( vxRenderingMode::SHOW_SHADOWS, bOptionShadows );
      renderingMode.SetEnabled( vxRenderingMode::SHOW_EVERY_PIXEL, bOptionEveryPixel );
      renderingMode.SetEnabled( vxRenderingMode::SHOW_CUTS_SHADED, bOptionCutsShaded );
      renderingMode.SetEnabled( vxRenderingMode::SHOW_CUTS_GRAYSCALE, bOptionCutsGrayscale );
    }
  }
  else if (atof(sVersion.c_str()) == 2.0)
  {
    vxRenderingMode::vxRenderingAlgorithmEnum eAlgorithm;
    string sAlgorithm;
    int iOptions;
    float fSlabThickness;

    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(sAlgorithm), "algorithm"), sAlgorithm);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(iOptions), "options"), iOptions);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(fSlabThickness), "slab thickness"), fSlabThickness);

    if (sAlgorithm == "DEFAULT")
      eAlgorithm = vxRenderingMode::DEFAULT;
    else if (sAlgorithm == "THIN_SLAB")
      eAlgorithm = vxRenderingMode::THIN_SLAB;
    else if (sAlgorithm == "MIP")
      eAlgorithm = vxRenderingMode::MIP;
    else if (sAlgorithm == "XRAY")
      eAlgorithm = vxRenderingMode::XRAY;
    else
    {
      eAlgorithm = vxRenderingMode::DEFAULT;
      LogDbg("Illegal rendering mode, using default", "vxSerializer", "Read");
    }

    renderingMode.SetRenderingAlgorithm(eAlgorithm);
    renderingMode.SetOptions(iOptions);
    renderingMode.SetSlabThickness(fSlabThickness);
  }
  else
  {
    LogDbg("Unknown version number for XML of vxRenderingMode", "vxSerializer", "Read");
  }

  return bSuccess;
} // Read(vxRenderingMode)


/**
 * Serializes vxColormap
 * @param element
 * @param colormap
 */
void vxSerializer::Write(vxDOMElement & element,
                         const vxColormap & colormap)
//*******************************************************************
{
  string sName(colormap.GetName());
  vxWindowLevel windowLevel(colormap.GetWindowLevel());
  vector<Point2D<ZeroOne> > vRed(colormap.GetRedList());
  vector<Point2D<ZeroOne> > vGreen(colormap.GetGreenList());
  vector<Point2D<ZeroOne> > vBlue(colormap.GetBlueList());
  vector<Point2D<ZeroOne> > vAlpha(colormap.GetAlphaList());
  bool bActiveRange(colormap.IsActiveRange());

  SetAttribute(element, "version", "2.0");
  Write(AddElement(element, vxSerializer::GetElementName(sName), "name"), sName);
  Write(AddElement(element, vxSerializer::GetElementName(windowLevel), "window/level"), windowLevel);
  Write(AddElement(element, vxSerializer::GetElementName(vRed),   "red list"),   vRed);
  Write(AddElement(element, vxSerializer::GetElementName(vGreen), "green list"), vGreen);
  Write(AddElement(element, vxSerializer::GetElementName(vBlue),  "blue list"),  vBlue);
  Write(AddElement(element, vxSerializer::GetElementName(vAlpha), "alpha list"), vAlpha);
  Write(AddElement(element, vxSerializer::GetElementName(bActiveRange), "active range"), bActiveRange);
} // Write(vxColormap)


/**
 * Deserializes vxColormap
 * @param element
 * @param colormap
 * @return success
 */
bool vxSerializer::Read(vxDOMElement & element, 
                        vxColormap & colormap)
//*******************************************************************
{
  if( element.IsNull() ) 
    return false;

  bool bSuccess(true);
  string sVersion("0.0");
  bSuccess &= GetAttribute(element, "version", sVersion);

  if (atof(sVersion.c_str()) == 2.0)
  {
    string sName;
    vxWindowLevel windowLevel;
    vector<Point2D<ZeroOne> > vRed;
    vector<Point2D<ZeroOne> > vGreen;
    vector<Point2D<ZeroOne> > vBlue;
    vector<Point2D<ZeroOne> > vAlpha;
    bool bActiveRange(true);

    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(sName), "name"), sName);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(windowLevel), "window/level"), windowLevel);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(vRed),   "red list"),   vRed);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(vGreen), "green list"), vGreen);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(vBlue),  "blue list"),  vBlue);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(vAlpha), "alpha list"), vAlpha);

    // if not available, just default active range to true
    Read(FindElement(element, vxSerializer::GetElementName(bActiveRange), "active range"), bActiveRange);

    colormap.SetName(sName);
    colormap.SetWindowLevel(windowLevel);
    colormap.SetRedList(vRed);
    colormap.SetGreenList(vGreen);
    colormap.SetBlueList(vBlue);
    colormap.SetAlphaList(vAlpha);
    colormap.SetActiveRange(bActiveRange);
  }
  else
  {
    LogDbg("Unknown version number for XML of vxColormap", "vxSerializer", "Read");
  }

  return bSuccess;
} // Read(vxColormap)


/**
 * Serializes vxPreset
 * @param element
 * @param preset the preset to write
 */
void vxSerializer::Write(vxDOMElement & element, 
                         const vxPreset & preset)
//*******************************************************************
{
  // get all the data
  int4 iUserId(preset.GetUserId());
  std::string sName(preset.GetName());
  std::string sModality(preset.GetModality());
  std::string sBodyPart(preset.GetBodyPart());
  std::string sGroup(preset.GetDicomGroup());
  std::string sField(preset.GetDicomField());
  std::string sValue(preset.GetDicomValue());
  int4 iInitialCamera(preset.GetInitialCameraPreset());
  int4 iInitialVisualizationPreset2D(preset.GetInitialVisualizationPreset2D());
  int4 iInitialVisualizationPreset3D(preset.GetInitialVisualizationPreset3D());
  std::vector<vxCameraPreset> vCameraPresets(preset.GetCameraPresets());
  const std::vector<const vxVisualizationPreset *> & vVisualizationPresets2D(preset.GetVisualizationPresets2D());
  const std::vector<const vxVisualizationPreset *> & vVisualizationPresets3D(preset.GetVisualizationPresets3D());
  std::vector<vxVisualizationPreset> vVisualizationPresets2DReal(CreateRealVectorFromPtr(vVisualizationPresets2D));
  std::vector<vxVisualizationPreset> vVisualizationPresets3DReal(CreateRealVectorFromPtr(vVisualizationPresets3D));

  vxSerializer serializer("vxPreset");
  SetAttribute(element, "version", "2.1");
  serializer.Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(iUserId), "user"), iUserId);
  serializer.Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(iInitialCamera), "initial camera preset"), iInitialCamera);
  serializer.Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(iInitialVisualizationPreset2D), "initial visualization preset 2D"), iInitialVisualizationPreset2D);
  serializer.Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(iInitialVisualizationPreset3D), "initial visualization preset 3D"), iInitialVisualizationPreset3D);
  serializer.Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(sName), "name"), sName);
  serializer.Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(sModality), "modality"), sModality);
  serializer.Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(sBodyPart), "body part"), sBodyPart);
  serializer.Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(sGroup), "dicom group"), sGroup);
  serializer.Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(sField), "dicom field"), sField);
  serializer.Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(sValue), "dicom value"), sValue);
  serializer.Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(vCameraPresets), "camera presets"), vCameraPresets);
  serializer.Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(vVisualizationPresets2DReal), "visualization presets 2D"), vVisualizationPresets2DReal);
  serializer.Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(vVisualizationPresets3DReal), "visualization presets 3D"), vVisualizationPresets3DReal);
} // Write(vxPreset)


/**
 * Deserializes vxPreset
 * @param element
 * @param preset the preset to read
 * @return success
 */
bool vxSerializer::Read(vxDOMElement & element, 
                        vxPreset & preset)
//*******************************************************************
{
  if( element.IsNull() )
    return false;

  bool bSuccess(true);
  std::string sVersion("0.0");
  bSuccess &= GetAttribute(element, "version", sVersion);

  if (atof(sVersion.c_str()) == 2.0)
  {
    int4 iUserId;
    std::string sName;
    std::string sModality;
    std::string sBodyPart;
    std::string sDicomGroup;
    std::string sDicomField;
    std::string sDicomValue;
    Box<float4> box(0, 0, 0, 1, 1, 1);
    int4 iInitialCameraPreset;
    int4 iInitialColormap2D;
    int4 iInitialColormap3D;
    std::vector<vxCamera> vCameras;
    std::vector<vxColormap> vColormaps2D;
    std::vector<vxColormap> vColormaps3D;

    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(iUserId), "user"), iUserId);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(sName), "name"), sName);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(iInitialColormap2D), "initial colormap 2d"), iInitialColormap2D);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(iInitialColormap3D), "initial colormap 3d"), iInitialColormap3D);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(iInitialCameraPreset), "initial camera 3d"), iInitialCameraPreset);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(sModality), "modality"), sModality);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(sBodyPart), "body part"), sBodyPart);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(sDicomGroup), "dicom group"), sDicomGroup);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(sDicomField), "dicom field"), sDicomField);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(sDicomValue), "dicom value"), sDicomValue);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(box), "bounding box"), box);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(vCameras), "cameras 3d"), vCameras);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(vColormaps2D), "colormaps 2d"), vColormaps2D);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(vColormaps3D), "colormaps 3d"), vColormaps3D);

    std::vector<vxCameraPreset> vCameraPresets;
    for (int i(0); i<vCameras.size(); ++i)
    {
      vCameraPresets.push_back(vxCameraPreset("default", vCameras[i], box));
    }

    // create vector<const vxVisualizationPreset *> from colormap and (default) rendering mode
    std::vector<const vxVisualizationPreset *> vpVisualizationPresets2D;
    for (int i(0); i<vColormaps2D.size(); ++i)
    {
      vxVisualizationPreset * pVisualizationPreset = new vxVisualizationPreset();
      pVisualizationPreset->SetColormap(new vxColormap(vColormaps2D[i]));
      pVisualizationPreset->SetRenderingMode(new vxRenderingMode);
      vpVisualizationPresets2D.push_back(pVisualizationPreset);
    }
    std::vector<const vxVisualizationPreset *> vpVisualizationPresets3D;
    for (int i(0); i<vColormaps3D.size(); ++i)
    {
      vxVisualizationPreset * pVisualizationPreset = new vxVisualizationPreset();
      pVisualizationPreset->SetColormap(new vxColormap(vColormaps3D[i]));
      pVisualizationPreset->SetRenderingMode(new vxRenderingMode);
      vpVisualizationPresets3D.push_back(pVisualizationPreset);
    }

    // now store the data in the preset
    preset.SetName(sName);
    preset.SetModality(sModality);
    preset.SetBodyPart(sBodyPart);
    preset.SetUserId(iUserId);
    preset.SetDicomGroup(sDicomGroup);
    preset.SetDicomField(sDicomField);
    preset.SetDicomValue(sDicomValue);
    preset.SetInitialCameraPreset(iInitialCameraPreset);
    preset.SetInitialVisualizationPreset2D(iInitialColormap2D);
    preset.SetInitialVisualizationPreset3D(iInitialColormap3D);
    preset.SetCameraPresets(vCameraPresets);
    preset.SetVisualizationPresets2D(vpVisualizationPresets2D);
    preset.SetVisualizationPresets3D(vpVisualizationPresets3D);
  }
  else if (atof(sVersion.c_str()) == 3.0)
  {
    int4 iUserId;
    std::string sName;
    std::string sModality;
    std::string sBodyPart;
    std::string sDicomGroup;
    std::string sDicomField;
    std::string sDicomValue;
    int4 iInitialCameraPreset;
    int4 iInitialVisualizationPreset2D;
    int4 iInitialVisualizationPreset3D;
    Box<float4> box(0, 0, 0, 1, 1, 1);
    std::vector<vxCamera> vCameras;
    std::vector<vxVisualizationPreset> vVisualizationPresets2D;
    std::vector<vxVisualizationPreset> vVisualizationPresets3D;

    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(iUserId), "user"), iUserId);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(sName), "name"), sName);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(iInitialCameraPreset), "initial camera 3d"), iInitialCameraPreset);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(iInitialVisualizationPreset2D), "initial visualization preset 2D"), iInitialVisualizationPreset2D);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(iInitialVisualizationPreset3D), "initial visualization preset 3D"), iInitialVisualizationPreset3D);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(sModality), "modality"), sModality);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(sBodyPart), "body part"), sBodyPart);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(sDicomGroup), "dicom group"), sDicomGroup);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(sDicomField), "dicom field"), sDicomField);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(sDicomValue), "dicom value"), sDicomValue);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(box), "bounding box"), box);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(vCameras), "cameras 3d"), vCameras);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(vVisualizationPresets2D), "visualization presets 2D"), vVisualizationPresets2D);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(vVisualizationPresets2D), "visualization presets 3D"), vVisualizationPresets3D);

    std::vector<vxCameraPreset> vCameraPresets;
    for (int i(0); i<vCameras.size(); ++i)
    {
      vCameraPresets.push_back(vxCameraPreset("default", vCameras[i], box));
    }

    // create vector<const vxVisualizationPreset *> from vector<vxVisualizationPreset>
    std::vector<const vxVisualizationPreset *> vpVisualizationPresets2D;
    for (int i(0); i<vVisualizationPresets2D.size(); ++i)
    {
      vxVisualizationPreset * pVisualizationPreset = new vxVisualizationPreset(vVisualizationPresets2D[i]);
      vpVisualizationPresets2D.push_back(pVisualizationPreset);
    }
    std::vector<const vxVisualizationPreset *> vpVisualizationPresets3D;
    for (int i(0); i<vVisualizationPresets3D.size(); ++i)
    {
      vxVisualizationPreset * pVisualizationPreset = new vxVisualizationPreset(vVisualizationPresets3D[i]);
      vpVisualizationPresets3D.push_back(pVisualizationPreset);
    }

    // now store the data in the preset
    preset.SetName(sName);
    preset.SetModality(sModality);
    preset.SetBodyPart(sBodyPart);
    preset.SetUserId(iUserId);
    preset.SetDicomGroup(sDicomGroup);
    preset.SetDicomField(sDicomField);
    preset.SetDicomValue(sDicomValue);
    preset.SetInitialCameraPreset(iInitialCameraPreset);
    preset.SetInitialVisualizationPreset2D(iInitialVisualizationPreset2D);
    preset.SetInitialVisualizationPreset3D(iInitialVisualizationPreset3D);
    preset.SetCameraPresets(vCameraPresets);
    preset.SetVisualizationPresets2D(vpVisualizationPresets2D);
    preset.SetVisualizationPresets3D(vpVisualizationPresets3D);
  }
  else if (atof(sVersion.c_str()) == 2.1)
  {
    int4 iUserId;
    std::string sName;
    std::string sModality;
    std::string sBodyPart;
    std::string sDicomGroup;
    std::string sDicomField;
    std::string sDicomValue;
    int4 iInitialCameraPreset;
    int4 iInitialVisualizationPreset2D;
    int4 iInitialVisualizationPreset3D;
    std::vector<vxCameraPreset> vCameraPresets;
    std::vector<vxVisualizationPreset> vVisualizationPresets2D;
    std::vector<vxVisualizationPreset> vVisualizationPresets3D;

    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(iUserId), "user"), iUserId);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(sName), "name"), sName);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(iInitialCameraPreset), "initial camera preset"), iInitialCameraPreset);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(iInitialVisualizationPreset2D), "initial visualization preset 2D"), iInitialVisualizationPreset2D);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(iInitialVisualizationPreset3D), "initial visualization preset 3D"), iInitialVisualizationPreset3D);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(sModality), "modality"), sModality);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(sBodyPart), "body part"), sBodyPart);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(sDicomGroup), "dicom group"), sDicomGroup);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(sDicomField), "dicom field"), sDicomField);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(sDicomValue), "dicom value"), sDicomValue);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(vCameraPresets), "camera presets"), vCameraPresets);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(vVisualizationPresets2D), "visualization presets 2D"), vVisualizationPresets2D);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(vVisualizationPresets2D), "visualization presets 3D"), vVisualizationPresets3D);

    // create vector<const vxVisualizationPreset *> from vector<vxVisualizationPreset>
    std::vector<const vxVisualizationPreset *> vpVisualizationPresets2D;
    for (int i(0); i<vVisualizationPresets2D.size(); ++i)
    {
      vxVisualizationPreset * pVisualizationPreset = new vxVisualizationPreset(vVisualizationPresets2D[i]);
      vpVisualizationPresets2D.push_back(pVisualizationPreset);
    }
    std::vector<const vxVisualizationPreset *> vpVisualizationPresets3D;
    for (int i(0); i<vVisualizationPresets3D.size(); ++i)
    {
      vxVisualizationPreset * pVisualizationPreset = new vxVisualizationPreset(vVisualizationPresets3D[i]);
      vpVisualizationPresets3D.push_back(pVisualizationPreset);
    }

    // now store the data in the preset
    preset.SetName(sName);
    preset.SetModality(sModality);
    preset.SetBodyPart(sBodyPart);
    preset.SetUserId(iUserId);
    preset.SetDicomGroup(sDicomGroup);
    preset.SetDicomField(sDicomField);
    preset.SetDicomValue(sDicomValue);
    preset.SetInitialCameraPreset(iInitialCameraPreset);
    preset.SetInitialVisualizationPreset2D(iInitialVisualizationPreset2D);
    preset.SetInitialVisualizationPreset3D(iInitialVisualizationPreset3D);
    preset.SetCameraPresets(vCameraPresets);
    preset.SetVisualizationPresets2D(vpVisualizationPresets2D);
    preset.SetVisualizationPresets3D(vpVisualizationPresets3D);
  }
  else
  {
    LogDbg("Unknown version number for XML of vxPreset: " + sVersion, "vxSerializer", "Read");
  }

  return bSuccess;
} // Read(vxPreset)


/**
 * Write vxWindowLevel
 * @param element
 * @param window level
 */
void vxSerializer::Write(vxDOMElement & element,
                         const vxWindowLevel & windowLevel)
//*******************************************************************
{
  ZeroOne window(windowLevel.GetWindow());
  ZeroOne level(windowLevel.GetLevel());

  SetAttribute(element, "version", "2.0");
  Write(AddElement(element, vxSerializer::GetElementName(window), "window"), window);
  Write(AddElement(element, vxSerializer::GetElementName(level),  "level"),  level);
} // Write(vxWindowLevel)


/**
 * Read vxWindowLevel
 * @param element
 * @param window level
 * @return success
 */
bool vxSerializer::Read(vxDOMElement & element, 
                        vxWindowLevel & windowLevel)
//*******************************************************************
{
  if( element.IsNull() ) 
    return false;

  bool bSuccess(true);
  
  string sVersion("0.0");
  bSuccess &= GetAttribute(element, "version", sVersion);

  if (atof(sVersion.c_str()) == 2.0)
  {
    ZeroOne window;
    ZeroOne level;
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(window), "window"), window);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(level), "level"), level);
    windowLevel.SetWindow(window);
    windowLevel.SetLevel(level);
  }
  else
  {
    LogDbg("Unknown version number for XML of vxWindowLevel", "vxSerializer", "Read");
  }

  return bSuccess;
} // Read(vxWindowLevel)


/**
 * Serializes vxCamera
 * @param element
 * @param camera
 */
void vxSerializer::Write(vxDOMElement & element,
                         const vxCamera & camera)
//*******************************************************************
{
  Point<float> ref(camera.GetViewReference());
  Point<float> eye(camera.GetEye());
  Normal<float> up(camera.GetViewUp());
  float fAspect(camera.GetAspectRatio());
  float fVerticleAngle(camera.GetFovDegrees());
  //Box<float> box(camera.GetBoundingBox());
  string sProjectionType("");

  if (camera.GetProjectionType() == vxCamera::PERSPECTIVE)
    sProjectionType = "perspective";
  else if (camera.GetProjectionType() == vxCamera::ORTHOGRAPHIC)
    sProjectionType = "orthographic";

  SetAttribute(element, "version", "2.0");
  Write(AddElement(element, vxSerializer::GetElementName(ref), "reference"), ref);
  Write(AddElement(element, vxSerializer::GetElementName(eye), "eye"), eye);
  Write(AddElement(element, vxSerializer::GetElementName(up), "up"), up);
  Write(AddElement(element, vxSerializer::GetElementName(fAspect), "aspect ratio"), fAspect);
  Write(AddElement(element, vxSerializer::GetElementName(fVerticleAngle), "verticle angle"), fVerticleAngle);
  Write(AddElement(element, vxSerializer::GetElementName(sProjectionType), "projection type"), sProjectionType);
  //Write(AddElement(element, vxSerializer::GetElementName(box), "bounding box"), box);
} // Write()


/**
 * Deserializes vxCamera
 * @param element
 * @param camera
 * @return success
 */
bool vxSerializer::Read(vxDOMElement & element, 
                        vxCamera & camera)
//*******************************************************************
{
  if( element.IsNull() ) 
    return false;

  bool bSuccess(true);
  string sVersion("0.0");
  GetAttribute(element, "version", sVersion);

  if (sVersion == "2.0")
  {
    Point<float> ref(0, 0, 0);
    Point<float> eye(0, 0, -1);
    Normal<float> up(0, 1, 0);
    float fAspect;
    float fVerticleAngle; 
    string sProjectionType;
    vxCamera::ProjectionTypeEnum eProjectionType;
    //Box<float> box;

    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(ref), "reference"), ref);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(eye), "eye"), eye);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(up), "up"), up);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(fAspect), "aspect ratio"), fAspect);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(fVerticleAngle), "verticle angle"), fVerticleAngle);
    //bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(box), "bounding box"), box);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(sProjectionType), "projection type"), sProjectionType);
    if (sProjectionType == "perspective")
      eProjectionType = vxCamera::PERSPECTIVE;
    else if (sProjectionType == "orthographic")
      eProjectionType = vxCamera::ORTHOGRAPHIC;

    camera.SetViewReference(ref);
    camera.SetEye(eye);
    camera.SetViewUp(up);
    camera.SetAspectRatio(fAspect);
    camera.SetFovDegrees(fVerticleAngle);
    camera.SetProjectionType(eProjectionType);
    //camera.SetBoundingBox(box);
  } // if version 2.0
  else
  {
    bSuccess = false;
    LogDbg("Could not read format of vxCamera", "vxSerializer", "Read");
  }
  return bSuccess;

} // Read(vxCamera)


/**
 * Serializes vxCameraPreset
 * @param element
 * @param cameraPreset
 */
void vxSerializer::Write(vxDOMElement & element,
                         const vxCameraPreset & cameraPreset)
//*******************************************************************
{
  std::string sName(cameraPreset.GetName());
  vxCamera camera(cameraPreset.GetCamera());
  Box<float> box(cameraPreset.GetBoundingBox());

  SetAttribute(element, "version", "2.0");
  Write(AddElement(element, vxSerializer::GetElementName(sName), "name"), sName);
  Write(AddElement(element, vxSerializer::GetElementName(camera), "camera"), camera);
  Write(AddElement(element, vxSerializer::GetElementName(box), "bounding box"), box);
} // Write(vxCameraPreset)


/**
 * Deserializes vxCameraPreset
 * @param element
 * @param cameraPreset
 * @return success
 */
bool vxSerializer::Read(vxDOMElement & element, 
                        vxCameraPreset & cameraPreset)
//*******************************************************************
{
  if( element.IsNull() ) 
    return false;

  bool bSuccess(true);
  string sVersion("0.0");
  GetAttribute(element, "version", sVersion);

  if (sVersion == "2.0")
  {
    std::string sName(cameraPreset.GetName());
    vxCamera camera(cameraPreset.GetCamera());
    Box<float> box(cameraPreset.GetBoundingBox());

    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(sName), "name"), sName);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(camera), "camera"), camera);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(box), "bounding box"), box);

    cameraPreset.SetName(sName);
    cameraPreset.SetCamera(camera);
    cameraPreset.SetBoundingBox(box);
  } // if version 2.0
  else
  {
    bSuccess = false;
    LogDbg("Could not read format of vxCameraPreset", "vxSerializer", "Read");
  }

  return bSuccess;
} // Read(vxCameraPreset)


/**
 * Write object to XML node
 * @param element the output node
 * @param visPreset
 */
void vxSerializer::Write(vxDOMElement & element,
                         const vxVisualizationPreset & visPreset)
//*******************************************************************
{
  string sName(visPreset.GetName());
  string sPresetImage(visPreset.GetPresetImageFile() );
	bool   bShowPresetImage( visPreset.IsShowPresetImage() );
  
  vxRenderingMode renderingMode(*visPreset.GetRenderingMode());
  vxColormap colormap(*visPreset.GetColormap());
  PixelRGB<float4> backgroundColor(visPreset.GetBackgroundColor());
  PixelRGB<float4> foregroundColor(visPreset.GetForegroundColor());

  vxSerializer::SetAttribute(element, "version", "1.0");
  vxSerializer::Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(sName), "name"), sName);
  vxSerializer::Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(sPresetImage), "presetImage"), sPresetImage);
  vxSerializer::Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(bShowPresetImage), "showPresetImage"), bShowPresetImage);
	
  vxSerializer::Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(colormap), "colormap"), colormap);
  vxSerializer::Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(renderingMode), "rendering mode"), renderingMode);
  vxSerializer::Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(backgroundColor), "background color"), backgroundColor);
  vxSerializer::Write(vxSerializer::AddElement(element, vxSerializer::GetElementName(foregroundColor), "foreground color"), foregroundColor);

  return;
} // ToXml()


/**
 * Initialize from XML node
 * @param element the input node
 * @return successful?
 */
bool vxSerializer::Read(vxDOMElement & element,
                        vxVisualizationPreset & visPreset)
//*******************************************************************
{
  if( element.IsNull() ) 
    return false;

  bool bSuccess(true);
  string sVersion("0.0");
  GetAttribute(element, "version", sVersion);

  bSuccess &= vxSerializer::GetAttribute(element, "version", sVersion);
  
  //TODO: Check the proper Version 
  if (atof(sVersion.c_str()) == 1.0)
  {
    string sName;
    string sPresetImage;
		bool   bShowPresetImage = false;
    vxColormap colormap;
    vxRenderingMode renderingMode;
    PixelRGB<float4> backgroundColor;
    PixelRGB<float4> foregroundColor;

    bSuccess &= vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(sName), "name"), sName);

    try
    {
      vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(sPresetImage), "presetImage"), sPresetImage);
    }
    catch ( ... )
    {
      LogDbg( "No presetimage node available for vxVisualizationPreset", "vxSerializer", "Read");
    }

		try
    {
      vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(bShowPresetImage), "showPresetImage"), bShowPresetImage);
    }
    catch ( ... )
    {
      LogDbg( "Do not display this image" , "vxSerializer", "Read");
    }

    bSuccess &= vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(colormap), "colormap"), colormap);
    bSuccess &= vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(renderingMode), "rendering mode"), renderingMode);
    bSuccess &= vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(backgroundColor), "background color"), backgroundColor);
    bSuccess &= vxSerializer::Read(vxSerializer::FindElement(element, vxSerializer::GetElementName(foregroundColor), "foreground color"), foregroundColor);

    visPreset.SetName(sName);
    visPreset.SetPresetImageFile( sPresetImage );
		visPreset.SetShowPresetImage( bShowPresetImage );
    visPreset.SetRenderingMode(new vxRenderingMode(renderingMode));
    visPreset.SetColormap(new vxColormap(colormap));
    visPreset.SetBackgroundColor(backgroundColor);
    visPreset.SetForegroundColor(foregroundColor);
  }
  else
  {
    LogDbg("Unknown version number for XML of vxVisualizationPreset", "vxSerializer", "Read");
  }

  return bSuccess;
} // FromXml()


/**
 * Serializes std::string
 * @param element
 * @param string
 */
void vxSerializer::Write(vxDOMElement & element,
                         const std::string & sValue)
//*******************************************************************
{
  SetValue(element, sValue.c_str());
} // Write(std::string)


/**
 * Deserializes std::string
 * @param element
 * @param string
 * @return success
 */
bool vxSerializer::Read(vxDOMElement & element, 
                        std::string & sValue)
//*******************************************************************
{
  if( element.IsNull() ) 
    return false;

  return GetValue(element, sValue);
} // Read(std::string)


/**
 * Serializes ZeroOne
 * @param element
 * @param zero one
 */
void vxSerializer::Write(vxDOMElement & element,
                         const ZeroOne & fValue)
//*******************************************************************
{
  float fTmp(fValue.V());
  Write(element, fTmp);
} // Write(ZeroOne)


/**
 * Deserializes ZeroOne
 * @param element
 * @param zero one
 * @return success
 */
bool vxSerializer::Read(vxDOMElement & element, 
                        ZeroOne & fValue)
//*******************************************************************
{
  if( element.IsNull() ) 
    return false;

  float fTmp;
  bool bSuccess(Read(element, fTmp));
  fValue = fTmp;

  return bSuccess;
} // Read(ZeroOne)


/**
 * Serializes a float
 * @param element
 * @param value
 */
void vxSerializer::Write(vxDOMElement & element,
                         const float & fValue)
//*******************************************************************
{
  SetAttribute(element, "value", ToAscii(fValue));
} // Write(float)


/**
 * Deserializes a float
 * @param element
 * @param value
 * @return success
 */
bool vxSerializer::Read(vxDOMElement & element, 
                        float & fValue)
//*******************************************************************
{
  if( element.IsNull() ) 
    return false;

  string sAttribute("");
  bool bSuccess(GetAttribute(element, "value", sAttribute));
  fValue = atof(sAttribute.c_str());

  return bSuccess;
} // Read(float)


/**
 * Serializes a int
 * @param element
 * @param value
 */
void vxSerializer::Write(vxDOMElement & element,
                         const int & iValue)
//*******************************************************************
{
  SetAttribute(element, "value", ToAscii(iValue));
} // Write(int)


/**
 * Deserializes a int
 * @param element
 * @param value
 * @return success
 */
bool vxSerializer::Read(vxDOMElement & element, 
                        int & iValue)
//*******************************************************************
{
  if( element.IsNull() ) 
    return false;

  string sAttribute("");
  bool bSuccess(GetAttribute(element, "value", sAttribute));
  iValue = atof(sAttribute.c_str());

  return bSuccess;
} // Read(int)


/**
 * Serializes a uint2
 * @param element
 * @param value
 */
void vxSerializer::Write(vxDOMElement & element,
                         const uint2 & uValue)
//*******************************************************************
{
  SetAttribute(element, "value", ToAscii(uValue));
} // Write(uint2)


/**
 * Deserializes a uint2
 * @param element
 * @param value
 * @return success
 */
bool vxSerializer::Read(vxDOMElement & element, 
                        uint2 & uValue)
//*******************************************************************
{
  if( element.IsNull() )
    return false;

  string sAttribute("");
  bool bSuccess(GetAttribute(element, "value", sAttribute));
  uValue = atoi(sAttribute.c_str());

  return bSuccess;
} // Read(uint2)


/**
 * Serializes a int2
 * @param element
 * @param value
 */
void vxSerializer::Write(vxDOMElement & element,
                         const int2 & iValue)
//*******************************************************************
{
  SetAttribute(element, "value", ToAscii(iValue));
} // Write(uint2)


/**
 * Deserializes a int2
 * @param element
 * @param value
 * @return success
 */
bool vxSerializer::Read(vxDOMElement & element, 
                        int2 & iValue)
//*******************************************************************
{
  if( element.IsNull() )
    return false;

  string sAttribute("");
  bool bSuccess(GetAttribute(element, "value", sAttribute));
  iValue = atoi(sAttribute.c_str());

  return bSuccess;
} // Read(uint2)


/**
 * Serializes a uint1
 * @param element
 * @param value
 */
void vxSerializer::Write(vxDOMElement & element,
                         const uint1 & uValue)
//*******************************************************************
{
  int iValue(uValue); // conversion to enhance readability
  SetAttribute(element, "value", ToAscii(iValue));
} // Write(uint1)


/**
 * Deserializes a uint1
 * @param element
 * @param value
 * @return success
 */
bool vxSerializer::Read(vxDOMElement & element, 
                        uint1 & uValue)
//*******************************************************************
{
  if( element.IsNull() )
    return false;

  int iValue(uValue); // conversion to enhance readability
  string sAttribute("");
  bool bSuccess(GetAttribute(element, "value", sAttribute));
  iValue = Bound(0, atoi(sAttribute.c_str()), 255);
  uValue = iValue;

  return bSuccess;
} // Read(uint1)


/**
 * Serializes a bool
 * @param element
 * @param value
 */
void vxSerializer::Write(vxDOMElement & element,
                         const bool & bValue)
//*******************************************************************
{
  SetAttribute(element, "value", (bValue == true) ? "true" : "false");
} // Write(bool)


/**
 * Deserializes a bool
 * @param element
 * @param value
 * @return success
 */
bool vxSerializer::Read( vxDOMElement & element, 
                        bool & bValue)
//*******************************************************************
{
  if( element.IsNull() ) 
    return false;

  string sAttribute("");
  bool bSuccess(GetAttribute(element, "value", sAttribute));
  bValue = (sAttribute == "true") ? true : false;

  return bSuccess;
} // Read(bool)


/**
 * Serializes vxVesselSection
 * @param element the element
 * @param section the vessel section
 */
void vxSerializer::Write(vxDOMElement & element, const vxVesselSection & section)
{

  Point3Df position(section.GetPosition());
  Normal3Df tangent(section.GetTanDirection());
  Normal3Df up(section.GetUpDirection());
  vector< Point3Df > lumenContour(section.GetLumenContour());
  vector< Point3Df > wallContour(section.GetWallContour());
  float4 fLumenArea(section.GetLumenArea());
  float4 fWallArea(section.GetWallArea());
  float4 fAccumulatedVolume(section.GetAccumulatedVolumeCubicMM());
  float4 fAccumulatedLength(section.GetAccumulatedLengthMM());
  float4 fCurvature(section.GetCurvatureDegree2MM());
  vxVesselSection::RaySegmentPair diameters(section.GetDiameterPoints());

  // wall contour encoded to be more compact
  MemoryBuffer wallMemoryBuffer(vxVesselSection::GetSerializedContourSize(wallContour));
  int1 * pWallRawBuffer = reinterpret_cast< int1 * >(wallMemoryBuffer.GetDataPtr());
  const int1 * pWallRawBufferEnd = pWallRawBuffer + wallMemoryBuffer.GetSize();
  vxVesselSection::WriteSerializedContour(& pWallRawBuffer, pWallRawBufferEnd, wallContour);
  std::string stringEncodedWall(wallMemoryBuffer.GetEncodedBase16().ToString());

  // lumen contour encoded to be more compact
  // the lumen and wall are normally the same except for areas of calcification
  // so in the common case use wall as a pointer to the wall contour for the lumen
  std::string stringEncodedLumen("wall");
  if (lumenContour != wallContour)
  {
    MemoryBuffer lumenMemoryBuffer(vxVesselSection::GetSerializedContourSize(lumenContour));
    int1 * pLumenRawBuffer = reinterpret_cast< int1 * >(lumenMemoryBuffer.GetDataPtr());
    const int1 * pLumenRawBufferEnd = pLumenRawBuffer + lumenMemoryBuffer.GetSize();
    vxVesselSection::WriteSerializedContour(& pLumenRawBuffer, pLumenRawBufferEnd, lumenContour);
    stringEncodedLumen = lumenMemoryBuffer.GetEncodedBase16().ToString();
  }

  SetAttribute(element, "version", "2.0");
  Write(AddElement(element, vxSerializer::GetElementName(position), "position"), position);
  Write(AddElement(element, vxSerializer::GetElementName(tangent), "tangent direction"), tangent);
  Write(AddElement(element, vxSerializer::GetElementName(up), "up direction"), up);
  Write(AddElement(element, vxSerializer::GetElementName(stringEncodedLumen), "lumen contour"), stringEncodedLumen);
  Write(AddElement(element, vxSerializer::GetElementName(stringEncodedWall), "wall contour"), stringEncodedWall);
  Write(AddElement(element, vxSerializer::GetElementName(fLumenArea), "lumen area"), fLumenArea);
  Write(AddElement(element, vxSerializer::GetElementName(fWallArea), "wall area"), fWallArea);
  Write(AddElement(element, vxSerializer::GetElementName(fAccumulatedVolume), "accumulated volume"), fAccumulatedVolume);
  Write(AddElement(element, vxSerializer::GetElementName(fAccumulatedLength), "accumulated length"), fAccumulatedLength);
  Write(AddElement(element, vxSerializer::GetElementName(fCurvature), "curvature"), fCurvature);
  Write(AddElement(element, vxSerializer::GetElementName(diameters), "diameters"), diameters);

} // Write(vxVesselSection)


/**
 * Deserializes vxVesselSection
 * @param element the element
 * @param section the vessel section
 * @return success
 */
bool vxSerializer::Read(vxDOMElement & element, vxVesselSection & section)
{

  bool bSuccess(false);

  if( element.IsNull() ) return bSuccess;

  string sVersion("0.0");
  GetAttribute(element, "version", sVersion);

  if (sVersion == "2.0")
  {

    Point3Df position(0, 0, 0);
    Normal3Df tangent(0, 0, 1);
    Normal3Df up(0, 0, 1);
    std::string stringEncodedLumen;
    std::string stringEncodedWall;
    float4 fLumenArea(0);
    float4 fWallArea(0);
    float4 fAccumulatedVolume(0);
    float4 fAccumulatedLength(0);
    float4 fCurvature(0);
    pair< RaySegment, RaySegment > diameters = make_pair(RaySegment(Point3Df(0,0,0), Point3Df(1,1,1)),
                                                          RaySegment(Point3Df(0,0,0), Point3Df(1,1,1)));

    bSuccess = true;
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(position), "position"), position);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(tangent), "tangent direction"), tangent);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(up), "up direction"), up);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(stringEncodedLumen), "lumen contour"), stringEncodedLumen);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(stringEncodedWall), "wall contour"), stringEncodedWall);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(fLumenArea), "lumen area"), fLumenArea);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(fWallArea), "wall area"), fWallArea);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(fAccumulatedVolume), "accumulated volume"), fAccumulatedVolume);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(fAccumulatedLength), "accumulated length"), fAccumulatedLength);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(fCurvature), "curvature"), fCurvature);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(diameters), "diameters"), diameters);

    // decode the wall contour from the compact binary format
    MemoryBuffer wallMemoryBuffer(MemoryBuffer::FromString(stringEncodedWall).GetDecodedBase16());
    int1 * pWallRawBuffer = reinterpret_cast< int1 * >(wallMemoryBuffer.GetDataPtr());
    vector< Point3Df > wallContour(vxVesselSection::ReadSerializedContour(& pWallRawBuffer, pWallRawBuffer + wallMemoryBuffer.GetSize()));

    // decode the lumen contour from the compact binary format
    vector< Point3Df > lumenContour(wallContour);
    if (stringEncodedLumen != "wall")
    {
      MemoryBuffer lumenMemoryBuffer(MemoryBuffer::FromString(stringEncodedLumen).GetDecodedBase16());
      int1 * pLumenRawBuffer = reinterpret_cast< int1 * >(lumenMemoryBuffer.GetDataPtr());
      lumenContour = vxVesselSection::ReadSerializedContour(& pLumenRawBuffer, pLumenRawBuffer + lumenMemoryBuffer.GetSize());
    }

    section.SetPosition(position);
    section.SetTanDirection(tangent);
    section.SetUpDirection(up);
    section.SetLumenContour(lumenContour);
    section.SetWallContour(wallContour);
    section.SetLumenArea(fLumenArea);
    section.SetWallArea(fWallArea);
    section.SetAccumulatedVolumeCubicMM(fAccumulatedVolume);
    section.SetAccumulatedLengthMM(fAccumulatedLength);
    section.SetCurvatureDegree2MM(fCurvature);
    section.SetDiameterPoints(diameters);
    
    bSuccess = true;

  } // if version 2.0
  else
  {

    LogErr("Could not read format of vxVesselSection", "vxSerializer", "Read");

  }

  return bSuccess;

} // Read(vxVesselSection)




/**
 * writes a RaySegment
 * @param element the element
 * @param raySegment the ray segment
 */
void vxSerializer::Write(vxDOMElement & element, const RaySegment & raySegment)
{

  Vector4Df startPoint(raySegment.GetStartPoint());
  Vector4Df endPoint(raySegment.GetEndPoint());

  SetAttribute(element, "version", "2.0");
  Write(AddElement(element, vxSerializer::GetElementName(startPoint), "start point"), startPoint);
  Write(AddElement(element, vxSerializer::GetElementName(endPoint), "end point"), endPoint);

} // Write(RaySegment)


/**
 * read a RaySegment
 * @param element the element
 * @param raySegment the ray segment
 * @return success
 */
bool vxSerializer::Read(vxDOMElement & element, RaySegment & raySegment)
{

  bool bSuccess(false);

  if( element.IsNull() ) return bSuccess;

  string sVersion("0.0");
  GetAttribute(element, "version", sVersion);

  if (sVersion == "2.0")
  {

    Vector4Df startPoint;
    Vector4Df endPoint;

    bSuccess = true;
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(startPoint), "start point"), startPoint);
    bSuccess &= Read(FindElement(element, vxSerializer::GetElementName(endPoint), "end point"), endPoint);

    raySegment = RaySegment(startPoint, endPoint);

  } // if version 2.0
  else
  {

    LogErr("Could not read format of RaySegment", "vxSerializer", "Read");

  }

  return bSuccess;

} // Read(RaySegment)


#undef COM_ERROR
#undef FILE_REVISION


// $Log: vxSerializer.C,v $
// Revision 1.30.2.1  2009/05/01 21:43:54  gdavidson
// Removed ShadowClipped from Read method of RenderingMode
//
// Revision 1.30  2007/08/21 14:34:16  dongqing
// add int2 type for serilizer
//
// Revision 1.29  2007/06/07 18:03:49  romy
// vxVizPreset write version was wrong
//
// Revision 1.28  2007/06/06 18:22:25  romy
// added a new flag to Indicate whether we want to display the Preset Image or not
//
// Revision 1.27  2007/03/23 14:36:26  romy
// reading and writing modified for vizPreset
//
// Revision 1.26  2007/01/22 15:34:44  geconomos
// Issue# 5321: Slice shadows mode reworked
//
// Revision 1.25  2006/10/03 20:08:55  geconomos
// update serialization for vxRenderingMode to handle the new SliceShadowMode enumeration
//
// Revision 1.24  2006/08/23 19:27:46  romy
// modified Read to eliminate null DOM character elements
//
// Revision 1.23  2006/01/18 20:39:26  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.22  2005/11/22 15:30:50  frank
// recognized rendering mode format version 3.0
//
// Revision 1.21  2005/11/18 21:15:41  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.20.2.5  2005/09/01 20:03:34  frank
// floating point comparisons in version numbers was not always working as expected
//
// Revision 1.20.2.4  2005/08/26 12:34:14  frank
// Issue #4393: Locked NONE component when reading old sessions
//
// Revision 1.20.2.3  2005/08/04 18:57:09  frank
// removed unused cursor 3D tracking
//
// Revision 1.20.2.2  2005/07/19 12:15:30  frank
// upgraded rendering mode xml for easier use and extensibility
//
// Revision 1.20.2.1  2005/07/13 19:24:00  frank
// removed unused include
//
// Revision 1.20  2005/06/07 18:00:10  vxconfig
// moved vessel serialization to vessel class
//
// Revision 1.19  2005/05/24 11:32:30  frank
// serialized the active range
//
// Revision 1.18  2005/04/12 15:23:32  geconomos
// changed xml library to the vxDOM classes
//
// Revision 1.17  2005/03/08 19:09:13  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.16.2.2  2005/02/16 14:58:11  frank
// Issue #3959: Added serialization of new fields in component
//
// Revision 1.16.2.1  2005/02/10 20:57:24  michael
// Introduced vxCameraPreset with BoundingBox, name (to be displayed in GUI),
// and new Preset xml version (2.1). Converted presets, except for PET and XA
// ones. Added V3D Utils function and added a call to it to adjust the camera
// relative to the bounding box of the (camera) preset once it is assigned from
// the preset to the environment camera object (CopyFrom()).
//
// Revision 1.16  2004/10/05 14:35:32  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.15  2004/07/12 18:04:00  frank
// added component data
//
// Revision 1.14  2004/05/17 18:48:35  frank
// moved visualization preset serialization to its own class
//
// Revision 1.13  2004/05/14 16:03:28  frank
// fixed writing of visualization prest
//
// Revision 1.12  2004/04/26 19:00:17  michael
// moved BoundingBox() / GetLeft() / GetRight() / etc. from vxCamera to
// vxViewerType and also reworked the MPR pan/zoom behaviour so that
// we are able to share a vxZoomFactor
//
// Revision 1.11  2004/04/23 16:51:09  frank
// setting the right presets
//
// Revision 1.10  2004/04/22 15:16:05  frank
// working on writing new version
//
// Revision 1.9  2004/04/16 20:32:52  frank
// fixed reading visualization presets
//
// Revision 1.8  2004/04/14 18:48:53  frank
// Extended presets to set the rendering mode and other visualization settings
//
// Revision 1.7  2004/04/06 13:33:35  frank
// updated serialization of presets
//
// Revision 1.6  2004/04/05 18:17:56  frank
// bad checkin
//
// Revision 1.5  2004/04/05 15:59:07  frank
// working on expanding the presets to include rendering modes, etc.
//
// Revision 1.4  2004/04/05 15:01:35  frank
// fixed code and debug comments
//
// Revision 1.3  2004/03/30 21:52:38  frank
// added vessel XML serialization
//
// Revision 1.2  2004/03/23 21:14:03  frank
// code review, working on xml serialization for vessels
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.32  2004/01/23 03:52:36  michael
// added serialization of vxHandle
//
// Revision 1.31  2004/01/05 18:57:23  michael
// added "mailto:"
//
// Revision 1.30  2003/11/06 20:44:05  michael
// Fixed rendering mode related issues, renamed the "algorithm" to be the mode,
// and the display options to be the options so that things are consistent. Also
// fixed the progressive refinement that broke because George forgot to add
// the assignment of the mode in vxRenderingMode::operator=()
//
// Revision 1.29  2003/08/05 13:36:24  frank
// Added visualization preset and PixelRGB serialization
//
// Revision 1.28  2003/04/28 16:59:25  michael
// fixed issue #3116 and 3065
//
// Revision 1.27  2003/04/18 19:56:21  michael
// initial code review
//
// Revision 1.26  2003/04/09 15:05:31  michael
// further step in cleaning up the semgentation/component handler code. In most
// locations there now is a IndexHistory and IndexLabel to better differentiate what
// is used. E.g. SegmentationVolumes and renderers use IndexLabel but internally
// the segmentation/component handler mostly works on HistoryIndex (component pane).
//
// Revision 1.25  2003/02/22 01:34:32  michael
// removed redundant code in (de-)serialization
//
// Revision 1.24  2003/02/22 00:22:56  michael
// fixed annotations to serialize and deserialize
//
// Revision 1.23  2003/02/19 18:22:59  michael
// changes to account for serialization of annotations
//
// Revision 1.22  2003/02/18 21:06:24  michael
// added the routines to serialize annotations
//
// Revision 1.21  2002/11/19 16:21:20  michael
// added a FromString()
//
// Revision 1.20  2002/11/14 15:56:59  michael
// enhancement for uint1 (0-255 instead of one char)
//
// Revision 1.19  2002/11/14 01:20:29  michael
// added more objects :-) (vxComponentArray, std::map, std::pair)
//
// Revision 1.18  2002/11/13 02:21:08  michael
// added some more serialization (Components)
//
// Revision 1.17  2002/11/12 23:49:06  michael
// added more objects to serialization
//
// Revision 1.16  2002/11/12 00:08:58  michael
// cosmetics
//
// Revision 1.15  2002/11/11 19:31:04  michael
// fixed warnings on FindElement. Any while loop would finally produce such a
// warning.Thus, warning is omitted, return false should be enough ...
//
// Revision 1.14  2002/10/24 01:22:55  michael
// added serialize/deserialize for int
//
// Revision 1.13  2002/10/21 15:45:47  michael
// took out serialize/deserialize methods, except one which can serve as
// template once we need a real wrapper class to access this from C# code.
//
// Revision 1.12  2002/10/18 22:59:56  michael
// fixed bug in writing camera: took eye position as normal vector :-(
//
// Revision 1.11  2002/10/18 18:51:51  michael
// fixed issue when reading from bad node (NULL): Check in all Read() first!!!
//
// Revision 1.10  2002/10/17 19:38:43  michael
// moved static templatized member function code into header file. This is
// necessary for the MS VC 7.0 compiler but should be fixed in 7.1
//
// Revision 1.9  2002/10/16 23:10:35  michael
// extended Serialize/Deserialize() and also fixed naming in Serialize() using
// vxSerialize::GetElementName() instead of hardcoded text. I also made the
// parameters of all GetElementName() const which I forgot in the first place.
//
// Revision 1.8  2002/10/16 22:20:43  michael
// added Serialize/Deserialize for vxCamera ...
//
// Revision 1.7  2002/10/15 22:37:09  michael
// moved writing/reading of std::string from attributes to elements
// (std::string containing ' " ' would not be possible with attributes)
//
// Revision 1.6  2002/10/15 20:44:50  michael
// got the serialization to work :-)
//
// Revision 1.5  2002/10/14 22:05:15  michael
// goes slightly better, dies off later. :-)
//
// Revision 1.4  2002/10/14 16:04:01  michael
// added further serialization functionality
//
// Revision 1.3  2002/10/13 20:19:21  michael
// added method to serialize/deserialize colormap
//
// Revision 1.2  2002/10/12 23:25:16  michael
// Added many methods to serialize/deserialize vxBase elements
//
// Revision 1.1  2002/10/10 18:31:20  dmitry
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxSerializer.C,v 1.30.2.1 2009/05/01 21:43:54 gdavidson Exp $
// $Id: vxSerializer.C,v 1.30.2.1 2009/05/01 21:43:54 gdavidson Exp $
