// $Id: VCSettings.C,v 1.21.2.5 2010/12/20 18:53:27 dongqing Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Jeffrey R. Meade (jmeade@viatronix.net)

// includes
#include "stdafx.h"
#include "VCSettings.h"
#include "File.h"
#include <iostream>
#include "vxDOMNodeList.h"
#include "vxDOMNamedNodeMap.h"
#include "vxVisualizationPreferences.h"

#define XML_TRUE "-1"
#define XML_FALSE "0"

#define XML_TRUE_STR "true"
#define XML_FALSE_STR "false"

bool VCSettings::m_bSettingsRead( false );
vxDOMDocument * VCSettings::m_pDocSettings( NULL );


// defines
#define FILE_REVISION "$Revision: 1.21.2.5 $"

/**
 * XML helper functions
 */
namespace XMLHelper
{
  /**
   * Appends an attribute child node to a given parent node
   *
   * @param pXmlDoc          The xml document
   * @param pParentElement   parent node
   * @param sElement         name of child node
   * @param sAttrName        attribute for child
   * @param value            attribute value
   * @return                 newly-created child node
   */
  MSXML2::IXMLDOMElementPtr AppendAttributeChild(MSXML2::IXMLDOMDocument2Ptr pXmlDoc, MSXML2::IXMLDOMNodePtr pParentElement, const std::string& sElement,
    const std::string& sAttrName, const variant_t& value)
  {
    MSXML2::IXMLDOMElementPtr pElement = pXmlDoc->createElement(sElement.c_str());

    //if (!sAttrName.empty() && !sAttrValue.empty())
    {
      MSXML2::IXMLDOMAttributePtr pAttr = pXmlDoc->createAttribute(sAttrName.c_str());
      pAttr->value = value;
      pElement->setAttributeNode(pAttr);
    }
    pParentElement->appendChild(pElement);
    return pElement;
  } // AppendAttributeChild()


  /**
   * Appends a node with the given attribute value with tagname "value"
   *
   * @param parent      parent node
   * @param sAttName    name of appended node's attribute
   * @param sAttValue   value of appended node's attribute
   */
  void AppendAttributeValue(vxDOMElement & parent, const std::string& sAttName, const std::string& sAttValue)
  {
    vxDOMElement childElem = parent.GetOwnerDocument().CreateElement(sAttName);
    childElem.SetAttribute("value", sAttValue);
    parent.AppendChild(childElem);
  } // AppendAttributeValue()

} // namespace XMLHelper


/**
 * Constructor
 */
VCSettings::VCSettings( const std::string & sSettingsPath ) :
m_sSettingsPath( sSettingsPath )
{
  try
  {
    // read the first time only
    if ( ! VCSettings::m_bSettingsRead )
    {

      if ( m_sSettingsPath.length() == 0 )
      {
        throw ex::AbortOperationException( LogRec( "The settings path must be specified prior to use", "VCSettings", "VCSettings" ) );
      }

      CoInitialize( NULL );

      vxDOMDocument::Initialize();

      if (!vx::File(sSettingsPath).Exists())
      {
        WriteDefaultSettings();
      }

      m_pDocSettings = new vxDOMDocument(vxDOMDocument::FromFile( sSettingsPath ));

      AddMissingSettings( true );

      VCSettings::m_bSettingsRead = true;

    } // if the first time

  }
  catch (...)
  {
    LogErr("Error during VCSettings initialization", "VCSettings", "VCSettings");
  }

} // VCSettings()


/**
 * copy constructor
 */
VCSettings::VCSettings( const VCSettings & other ) :
  m_sSettingsPath( other.m_sSettingsPath )
{
} // VCSettings( other )


/**
 * copy constructor
 */
VCSettings::VCSettings( VCSettings && other ) :
  m_sSettingsPath( nullptr )
{
  m_sSettingsPath = other.m_sSettingsPath;
  other.m_sSettingsPath = nullptr;
} // VCSettings( other )


/**
 * default constructor
 */
VCSettings::VCSettings() :
  m_sSettingsPath( "" )
{
}

/**
 * Destructor
 */
VCSettings::~VCSettings()
{
} // ~VCSettings()


const VCSettings & VCSettings::GetInstance()
{
  static VCSettings settings;

  // read the first time only
  if( !VCSettings::m_bSettingsRead )
  {
    CoInitialize( NULL );

    vxDOMDocument::Initialize();
 
    // get the path of the executable
    char sExePath[ _MAX_PATH ];
    GetModuleFileName( NULL, sExePath, _MAX_PATH );
    std::string path( sExePath );
    settings.SetSettingsPath( path + ".config" );

    // extract settings from configuration file
    vxDOMDocument doc( vxDOMDocument::FromFile( settings.GetSettingsPath() ));
    vxDOMNodeList settingsList = doc.GetDocumentElement().GetElementsByTagName( "settings" );
    if( settingsList.GetLength() != 1 )
      throw ex::XmlException( LogRec( "Unable to load VCSettings.", "VCSettings", "Create" ));

    vxDOMElement settingsElement = settingsList.Item( 0 ).ToElement();
    m_pDocSettings = new vxDOMDocument( vxDOMDocument::FromXml( settingsElement.ToString() ));

    settings.AddMissingSettings( false );

    // set the render thread count
    vxVisualizationPreferences::SetMaxRenderThreads( std::stoi( settings.GetValue( ViewerXML(), MaxRenderThreads() )));

    VCSettings::m_bSettingsRead = true;
  } // if the first time

  return settings;
} // FromXml( sXml )


/**
 * Adds map entries for the default values of any missing settings
 *
 * @return success/failure
 */
bool VCSettings::AddMissingSettings( bool bSave )
{
  vxDOMElement docElem = m_pDocSettings->GetDocumentElement();
  vxDOMDocument doc = docElem.GetOwnerDocument();
  vxDOMElement procElem;
  try
  {
    procElem = docElem.GetElementsByTagName(ProcessorXML()).Item(0).ToElement();
  }
  catch (...)
  {
    procElem = doc.CreateElement(ProcessorXML());
  }
  procElem.SetAttribute("ver", "2.0");

  if (GetValue(ProcessorXML(), DisplayQuality2D()).empty())           { XMLHelper::AppendAttributeValue(procElem, DisplayQuality2D(), "16"); }
  if (GetValue(ProcessorXML(), ElectronicCleanse()).empty())          { XMLHelper::AppendAttributeValue(procElem, ElectronicCleanse(), XML_TRUE);  }
  if (GetValue(ProcessorXML(), Compressed3DOn()).empty())             { XMLHelper::AppendAttributeValue(procElem, Compressed3DOn(), XML_TRUE);  }
  if (GetValue(ProcessorXML(), ApplyInterpolation()).empty())         { XMLHelper::AppendAttributeValue(procElem, ApplyInterpolation(), XML_FALSE);  }
  if (GetValue(ProcessorXML(), Compressed3DSliceBound()).empty())     { XMLHelper::AppendAttributeValue(procElem, Compressed3DSliceBound(), "481");  }
  if (GetValue(ProcessorXML(), ResiduePercent()).empty())             { XMLHelper::AppendAttributeValue(procElem, ResiduePercent(), "0.02F");  }

  vxDOMElement viewerElem;
  try
  {
    viewerElem = docElem.GetElementsByTagName(ViewerXML()).Item(0).ToElement();
  }
  catch (...)
  {
    viewerElem = doc.CreateElement(ViewerXML());
  }
  viewerElem.SetAttribute("ver", "2.0");

  if (GetValue(ViewerXML(), Auto2DFlight()).empty())               { XMLHelper::AppendAttributeValue(viewerElem, Auto2DFlight(), XML_FALSE);  }
  if (GetValue(ViewerXML(), CutRenderSize()).empty())              { XMLHelper::AppendAttributeValue(viewerElem, CutRenderSize(), "256");  }
  if (GetValue(ViewerXML(), MinRenderSize()).empty())              { XMLHelper::AppendAttributeValue(viewerElem, MinRenderSize(), "161");  }
  if (GetValue(ViewerXML(), MaxRenderSize()).empty())              { XMLHelper::AppendAttributeValue(viewerElem, MaxRenderSize(), "512");  }
  if (GetValue(ViewerXML(), MinFrameRate()).empty())               { XMLHelper::AppendAttributeValue(viewerElem, MinFrameRate(), "10.0F");  }
  if (GetValue(ViewerXML(), MaxFrameRate()).empty())               { XMLHelper::AppendAttributeValue(viewerElem, MaxFrameRate(), "12.0F");  }
  if (GetValue(ViewerXML(), DrawDisclaimer()).empty())             { XMLHelper::AppendAttributeValue(viewerElem, DrawDisclaimer(), XML_FALSE);  }
  if (GetValue(ViewerXML(), FieldOfView3D()).empty())              { XMLHelper::AppendAttributeValue(viewerElem, FieldOfView3D(), "90");  }
  if (GetValue(ViewerXML(), WindowWidthIncreaseRight()).empty())   { XMLHelper::AppendAttributeValue(viewerElem, WindowWidthIncreaseRight(), XML_TRUE);  }

  if (GetValue(ViewerXML(), MovieImageSize()).empty())             { XMLHelper::AppendAttributeValue(viewerElem, MovieImageSize(), "256");  }
  if (GetValue(ViewerXML(), MovieFrameRate()).empty())             { XMLHelper::AppendAttributeValue(viewerElem, MovieFrameRate(), "15");  }
  if (GetValue(ViewerXML(), MovieRenderSize()).empty())            { XMLHelper::AppendAttributeValue(viewerElem, MovieRenderSize(), "509");  }

  if (GetValue(ViewerXML(), MaxRenderThreads()).empty())           { XMLHelper::AppendAttributeValue(viewerElem, MaxRenderThreads(), "8");  }

  if (GetValue(ViewerXML(), AllowDual3DFlight()).empty())          { XMLHelper::AppendAttributeValue(viewerElem, AllowDual3DFlight(), XML_FALSE);  }
  if (GetValue(ViewerXML(), AllowDataExport()).empty())            { XMLHelper::AppendAttributeValue(viewerElem, AllowDataExport(), XML_FALSE);  }

  if (GetValue(ViewerXML(), DefaultLinkedDatasets()).empty())      { XMLHelper::AppendAttributeValue(viewerElem, DefaultLinkedDatasets(), XML_TRUE); }
  if (GetValue(ViewerXML(), DefaultCadSliderValue()).empty())      { XMLHelper::AppendAttributeValue(viewerElem, DefaultCadSliderValue(), "75");  }
  if (GetValue(ViewerXML(), AlternateLayouts()).empty())           { XMLHelper::AppendAttributeValue(viewerElem, AlternateLayouts(), XML_FALSE); }

  if (GetValue(ViewerXML(), CADDisplayName()).empty())             { XMLHelper::AppendAttributeValue(viewerElem, CADDisplayName(), "SRDI"); }
  if (GetValue(ViewerXML(), EnableSRT()).empty())                  { XMLHelper::AppendAttributeValue(viewerElem, EnableSRT(), XML_FALSE); }

  if (GetValue(ViewerXML(), OutputSuspiciousROI()).empty())        { XMLHelper::AppendAttributeValue(viewerElem, OutputSuspiciousROI(), XML_FALSE); }

  if (GetValue(ViewerXML(), UseULDVolume()).empty())               { XMLHelper::AppendAttributeValue(viewerElem, UseULDVolume(), XML_FALSE); }

  if (GetValue(ViewerXML(), EnableDualDisplayTabs()).empty())      { XMLHelper::AppendAttributeValue(viewerElem, EnableDualDisplayTabs(), XML_FALSE); }
  if (GetValue(ViewerXML(), Draw2DRuler()).empty())                { XMLHelper::AppendAttributeValue(viewerElem, Draw2DRuler(), XML_FALSE); }
	if (GetValue(ViewerXML(), NormalizeSnapshot()).empty())          { XMLHelper::AppendAttributeValue(viewerElem, NormalizeSnapshot(), XML_TRUE); }

  // todo: figure out how to add defaults for cad vendors...or at least a default vendor

  docElem.SetTagName("settings");

  if( bSave )
    m_pDocSettings->Save(m_sSettingsPath);

  return true;
} // AddMissingSettings()


/**
 * Writes settings with default values to settings file
 */
void VCSettings::WriteDefaultSettings()
{
  try
  {
    MSXML2::IXMLDOMDocument2Ptr pXmlDoc;
    HRESULT hRes = pXmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument30));
    pXmlDoc->preserveWhiteSpace = VARIANT_TRUE;

    try
    {
      // Add xml data
      {
        MSXML2::IXMLDOMProcessingInstructionPtr pInstr;
        pInstr = pXmlDoc->createProcessingInstruction("xml", "ver='2.0'");
        pXmlDoc->appendChild(pInstr);

        MSXML2::IXMLDOMElementPtr pElement, pElTop, pElNest;
        MSXML2::IXMLDOMAttributePtr pAttr;

        std::strstream sstr;

        // processor settings
        pElTop   = XMLHelper::AppendAttributeChild(pXmlDoc, pXmlDoc, "settings", "ver", variant_t(1.0));
        pElNest  = XMLHelper::AppendAttributeChild(pXmlDoc, pXmlDoc->documentElement, "processor", "ver", variant_t(1.0));
        pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElNest, DisplayQuality2D(), "value", variant_t(16));
        pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElNest, ElectronicCleanse(), "value", variant_t(true));
        pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElNest, Compressed3DOn(), "value", variant_t(true));
        pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElNest, ApplyInterpolation(), "value", variant_t(false));
        pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElNest, Compressed3DSliceBound(), "value", variant_t(481));
        pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElNest, ResiduePercent(), "value", variant_t(0.02F));

        // viewer settings
        pElNest  = XMLHelper::AppendAttributeChild(pXmlDoc, pXmlDoc->documentElement, "viewer", "ver", variant_t(1.0));
        pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElNest, Auto2DFlight(), "value", variant_t(false));
        pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElNest, CutRenderSize(), "value", variant_t(256));
        pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElNest, MinRenderSize(), "value", variant_t(161));
        pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElNest, MaxRenderSize(), "value", variant_t(512));
        pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElNest, MinFrameRate(), "value", variant_t(10.0F));
        pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElNest, MaxFrameRate(), "value", variant_t(12.0F));
        pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElNest, DrawDisclaimer(), "value", variant_t(false));
        pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElNest, FieldOfView3D(), "value", variant_t(90));
        pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElNest, WindowWidthIncreaseRight(), "value", variant_t(true));

        pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElNest, MovieImageSize(), "value", variant_t(256));
        pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElNest, MovieFrameRate(), "value", variant_t(15));
        pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElNest, MovieRenderSize(), "value", variant_t(509));

        pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElNest, MaxRenderThreads(), "value", variant_t(8));

        pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElNest, AllowDual3DFlight(), "value", variant_t(false));
        pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElNest, DefaultLinkedDatasets(), "value", variant_t(true));
        pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElNest, AllowDataExport(), "value", variant_t(false));

        pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElNest, DefaultCadSliderValue(), "value", variant_t(75));
        pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElNest, CadVendors(), "value", variant_t(""));
        pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElNest, AlternateLayouts(), "value", variant_t(false));
        pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElNest, CADDisplayName(), "value", variant_t("SRDI"));
        pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElNest, EnableSRT(), "value", variant_t(true));
        pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElNest, OutputSuspiciousROI(), "value", variant_t(false));
        pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElNest, UseULDVolume(), "value", variant_t(false));
        pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElNest, EnableDualDisplayTabs(), "value", variant_t("Disable"));
				pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElNest, Draw2DRuler(), "value", variant_t(true));
				pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElNest, NormalizeSnapshot(), "value", variant_t(true));
			}

      vx::File( m_sSettingsPath ).GetParentFile().Mkdirs();
      if (FAILED(pXmlDoc->save( m_sSettingsPath.c_str() )))  { throw false; }
    }
    catch (...)
    {
      LogDbg("An error occurred while creating default xml data/file", "VCSettings", "WriteDefaultSettings");
      throw;
    }
  }
  catch(...)
  {
    return;
  }
} // WriteDefaultSettings()


/**
 * Gets the children of the node with specified tagname
 *
 * @param sTagName   tagname of children
 * @param sEntry     entry within the specified tag
 * @return           list of all elements in the entry
 */
vxDOMNodeList VCSettings::GetElements(const std::string& sTagName, const std::string& sEntry) const
{
  vxDOMElement elem = m_pDocSettings->GetDocumentElement();
  vxDOMNodeList nodelist = elem.GetElementsByTagName(sTagName);
  if( sEntry == "" )
    return nodelist;

  if (nodelist.GetLength() != 1)
  {
    LogErr("can't find settings", "VCSettings", "GetValue");
    return vxDOMNodeList();
  }

  return nodelist.Item(0).ToElement().GetElementsByTagName(sEntry);
} // GetElements()

/**
 * Gets the extire section within the vc settings file
 *
 * @param sSectionName      Section name
 *
 * @return  Xml string
 */
std::string VCSettings::GetSection(const std::string & sSectionName) const
{

  std::string section("");

  vxDOMNodeList nodelist = GetElements(sSectionName, "");

  if ( nodelist.GetLength() > 0 )
    section = nodelist.Item(0).ToElement().ToString();

     
  return section;

}

/**
 * Gets a single value of a entry within the specified tagname
 *
 * @param sTagName   tagname of children
 * @param sEntry     entry within the specified tag
 * @return           value of the entry
 */
std::string VCSettings::GetValue(const std::string& sTagName, const std::string& sEntry, const std::string& sAttr) const
{
  vxDOMNodeList entryList = GetElements(sTagName, sEntry);
  if (entryList.GetLength() != 1)
  {
    LogErr("can't find entry " + sEntry, "VCSettings", "GetValue");
    return std::string();
  }
  
  return entryList.Item(0).GetAttributes().GetNamedItem( sAttr ).GetNodeValue();
} // GetValue()


/**
 * Gets a single integer value of a entry within the specified tagname
 *
 * @param sTagName   tagname of children
 * @param sEntry     entry within the specified tag
 * @return           value of the entry
 */
int4 VCSettings::GetIntValue(const std::string& sTagName, const std::string& sEntry) const
{
  return atoi(GetValue(sTagName, sEntry).c_str());
} // GetIntValue()


/**
 * Gets a single bool value of a entry within the specified tagname
 *
 * @param sTagName   tagname of children
 * @param sEntry     entry within the specified tag
 * @return           value of the entry
 */
bool VCSettings::GetBoolValue(const std::string& sTagName, const std::string& sEntry) const
{
  std::string val( GetValue(sTagName, sEntry));
  return bool( val == XML_TRUE || val == XML_TRUE_STR );
} // GetBoolValue()


/**
 * Gets a single float value of a entry within the specified tagname
 *
 * @param sTagName   tagname of children
 * @param sEntry     entry within the specified tag
 * @return           value of the entry
 */
float4 VCSettings::GetFloatValue(const std::string& sTagName, const std::string& sEntry) const
{
  return atof(GetValue(sTagName, sEntry).c_str());
} // GetFloatValue()


/** 
 * Cad vendor info
 *
 * @param vendorMap   out: list of cad vendors and related info
 */
void VCSettings::GetCadVendors(CadVendorMap & vendorMap) const
{
  try
  {
    if (m_pDocSettings == NULL)
    {
      return;
    }

    vxDOMElement elem = m_pDocSettings->GetDocumentElement();
    vxDOMNodeList nodelist = elem.GetElementsByTagName("cadVendor");
    for (int4 iNode(0); iNode < nodelist.GetLength(); iNode++)
    {
      vxDOMNamedNodeMap nodemap = nodelist.Item(iNode).GetAttributes();
      std::string sName = nodemap.GetNamedItem("name").GetNodeValue();
      std::string sPath = nodemap.GetNamedItem("imagePath").GetNodeValue();
      std::string sSite = nodemap.GetNamedItem("website").GetNodeValue();

      vendorMap[sName] = CadVendorInfo(sName, sPath, sSite);
    }
  }
  catch (...)
  {
    return;
  }
} // GetCadVendors()


/**
 * Sets the filepath for the settings file
 *
 * @param sSettingsPath   path to the settings file
 */
void VCSettings::SetSettingsPath( const std::string & sSettingsPath )
{
  m_sSettingsPath = sSettingsPath;
} // SetSettingsPath()


// $Log: VCSettings.C,v $
// Revision 1.21.2.5  2010/12/20 18:53:27  dongqing
// add VCSetting switch for using ULD volume
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.21.2.4  2008/01/17 20:51:25  dongqing
// add OutputSuspiciousROI switch
//
// Revision 1.21.2.3  2007/08/30 12:55:51  dongqing
// add residue percent threshold for preprocessing
//
// Revision 1.21.2.2  2007/04/12 16:08:09  jmeade
// system setting for enabling srt tool.
//
// Revision 1.21.2.1  2007/04/04 20:04:53  jmeade
// politically-correct names for CAD in menus.
//
// Revision 1.21  2007/02/15 00:08:23  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.20  2006/12/05 20:18:36  jmeade
// Issue 5047: exposing xml helper class.
//
// Revision 1.19  2006/11/24 21:25:31  jmeade
// alternate projection layouts entry.
//
// Revision 1.18  2006/10/09 17:54:55  frank
// made the UD segment image size adjustable
//
// Revision 1.17  2006/10/04 20:56:41  frank
// Issue #5041: Made automatic dataset linking a configuration setting
// Issue #5042: Made centerline display default a configuration setting
//
// Revision 1.16  2006/02/07 21:16:56  jmeade
// code review: comments.
//
// Revision 1.15  2006/01/27 20:31:38  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.14  2005/11/16 04:51:39  jmeade
// Merge from Merge_VC_2-0_051115.
//
// Revision 1.13  2005/11/04 20:04:34  dongqing
// change usage of VCSetting for HQ2Don as GetIntValue
//
// Revision 1.12.2.6  2006/08/02 17:42:27  frank
// added capability to export missed region data
//
// Revision 1.12.2.5  2006/01/09 16:36:31  frank
// changed 2D flight default to off
//
// Revision 1.12.2.4  2006/01/05 21:41:20  frank
// Added logic for controlling visibility of cad findings according to sphericity slider threshold
//
// Revision 1.12.2.3  2006/01/03 23:59:46  jmeade
// Issue 4507: Config option for dual-fly mode.
//
// Revision 1.12.2.2  2005/11/04 19:25:57  dongqing
// change usage of VCSetting for HQ2Don as GetIntValue
//
// Revision 1.12.2.1  2005/11/04 17:12:36  jmeade
// Issue 4520: Fixed error in GetBoolValue() method.
//
// Revision 1.12  2005/11/04 00:55:22  jmeade
// Issue 4513: Recoded vcsettings xml functionality.
//
// Revision 1.11  2005/10/28 17:29:42  geconomos
// added MaxRenderThreads
//
// Revision 1.10  2005/10/28 13:31:05  geconomos
// added movie related settings
//
// Revision 1.9  2005/10/27 01:27:21  jmeade
// split cad vendors into separate xml node/attribute sets.
//
// Revision 1.8  2005/10/13 18:11:31  vxconfig
// moved settings file relative to executable in config folder
// removed numbers from arrows prior to revealing the CAD findings list
//
// Revision 1.7  2005/10/07 16:17:13  geconomos
// reworked cad vendor images
//
// Revision 1.6  2005/10/07 13:13:28  geconomos
// removed DrawBackgroundImage flag and added DrawDisclaimer flag
//
// Revision 1.5  2005/10/04 23:53:55  jmeade
// cad vendor info entries.
//
// Revision 1.4  2005/09/27 14:45:04  vxconfig
// Issue #3375: Synchronized window/level across products
//
// Revision 1.3  2005/09/27 13:50:18  vxconfig
// added 3D field of view
//
// Revision 1.2  2005/08/17 18:57:37  geconomos
// Added "DrawBackgroundImage" property
//
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 1.1.2.7.2.3.2.1  2005/06/17 00:23:22  jmeade
// write out default values for older versions of file that are missing entries.
//
// Revision 1.1.2.7.2.3  2005/06/10 18:18:12  geconomos
// Added settings for 3D navigation and rendering.
//
// Revision 1.1.2.7.2.2  2005/03/10 20:21:31  dongqing
// add GetCompressed3DSliceBound() entry
//
// Revision 1.1.2.7.2.1  2005/03/07 20:04:05  dongqing
// merge from enhanced VC_1-4
//
// Revision 1.1.2.7  2005/02/15 20:04:57  jmeade
// merge from vc1.4 branch.
//
// Revision 1.1.2.6.2.4  2005/02/10 16:58:52  dongqing
// add Half3DOn switch
//
// Revision 1.1.2.6.2.3  2005/01/21 21:45:18  dongqing
// take out the bKeepOriginalVol check
//
// Revision 1.1.2.6.2.2  2005/01/20 16:38:05  dongqing
// Add bKeepOriginalVol switch in VCSettings
//
// Revision 1.1.2.6.2.1  2005/01/07 19:45:48  jmeade
// new location for config file.
//
// Revision 1.1.2.6  2004/09/27 20:54:37  jmeade
// Preference to disable 2d auto flight.
//
// Revision 1.1.2.5  2004/08/23 21:33:03  jmeade
// VC++6.0 compiler errors.
//
// Revision 1.1.2.4  2004/08/13 22:07:47  jmeade
// Well, it's official, xml only reads values as strings, everything else needs an atoi() call.
//
// Revision 1.1.2.3  2004/08/12 23:37:21  jmeade
// Read bool values as strings then convert to bool.
//
// Revision 1.1.2.2  2004/08/12 15:37:46  jmeade
// Default xml file.
//
// Revision 1.1.2.1  2004/08/11 03:52:38  jmeade
// xml file for vc settings.
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/VCSettings.C,v 1.21.2.5 2010/12/20 18:53:27 dongqing Exp $
// $Id: VCSettings.C,v 1.21.2.5 2010/12/20 18:53:27 dongqing Exp $
