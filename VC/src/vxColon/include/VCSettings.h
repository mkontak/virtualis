// $Id: VCSettings.h,v 1.19.2.5 2010/12/20 19:06:05 dongqing Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Jeffrey R. Meade (jmeade@viatronix.net)


// pragmas
#pragma once


// includes
#include <list>
#include <map>
#include <comutil.h>
#include "typedef.h"
#import "msxml3.dll"
#include "vxDOMElement.h"
#include "vxDOMDocument.h"
#include <memory>


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
    const std::string& sAttrName, const variant_t& value);


  /**
   * Appends a node with the given attribute value with tagname "Value"
   *
   * @param parent      parent node
   * @param sAttName    name of appended node's attribute
   * @param sAttValue   value of appended node's attribute
   */
  void AppendAttributeValue(vxDOMElement & parent, const std::string& sAttName, const std::string& sAttValue);

} // namespace XMLHelper


class VCSettings  
{
public:
  /// processor xml subsection
  inline static std::string ProcessorXML() { return "processor"; }
  /// whether high-quality 2D is displayed
  inline static std::string DisplayQuality2D() { return "displayQuality2D"; }
  /// whether studies are processed with electronic cleansing
  inline static std::string ElectronicCleanse() { return "electronicCleanse"; }
  /// whether 3D data is compressed during processing??
  inline static std::string Compressed3DOn() { return "compressed3DOn"; }
  /// whether interpolation is applied??
  inline static std::string ApplyInterpolation() { return "applyInterpolation"; }
  /// whether 3D slice bounds are compressed??
  inline static std::string Compressed3DSliceBound() { return "compressed3DSliceBound"; }
  /// set the residue precent threshold
  inline static std::string ResiduePercent() { return "residuePercent"; }

  /// set the transport lasyer
  inline static std::string TransportLayer() { return "transportLayey"; }

  /// viewer xml subsection
  inline static std::string ViewerXML() { return "viewer"; }
  /// is 2D flight enabled
  inline static std::string Auto2DFlight() { return "auto2DFlight"; }
  /// cut image render buffer size
  inline static std::string CutRenderSize() { return "cutRenderSize"; }
  /// minimum render buffer depth
  inline static std::string MinRenderSize() { return "minRenderSize"; }
  /// maximum render buffer depth
  inline static std::string MaxRenderSize() { return "maxRenderSize"; }
  /// minimum allowable refresh frame rate
  inline static std::string MinFrameRate() { return "minFrameRate"; }
  /// maximum allowable refresh frame rate
  inline static std::string MaxFrameRate() { return "maxFrameRate"; }
  /// disclaimer for beta versions
  inline static std::string DrawDisclaimer() { return "drawDisclaimer"; }
  /// 3D field of view value
  inline static std::string FieldOfView3D() { return "fieldOfView3D"; }
  ///
  inline static std::string WindowWidthIncreaseRight() { return "windowWidthIncreaseRight"; }
  /// size of movie frames
  inline static std::string MovieImageSize() { return "movieImageSize"; }
  /// frame rate of movies
  inline static std::string MovieFrameRate() { return "movieFrameRate"; }
  /// movie render buffer depth
  inline static std::string MovieRenderSize() { return "movieRenderSize"; }
  /// maximum number of render threads allowed
  inline static std::string MaxRenderThreads() { return "maxRenderThreads"; }
  /// is 3D simultaneous flight (both datasets flying simultaneously) allowed
  inline static std::string AllowDual3DFlight() { return "allowDual3DFlight"; }
  /// default linked datasets
  inline static std::string DefaultLinkedDatasets() { return "defaultLinkedDatasets"; }
  /// export missed region data??
  inline static std::string AllowDataExport() { return "allowDataExport"; }
  /// default cad slider value
  inline static std::string DefaultCadSliderValue() { return "defaultCadSliderValue"; }
  /// cad vendor info listing
  inline static std::string CadVendors() { return "cadVendors"; }
  /// include view layouts that include "alternate" view projections
  inline static std::string AlternateLayouts() { return "alternateProjectionLayouts"; }
  /// regulatory- or "politically"-correct name to be used for CAD options
  inline static std::string CADDisplayName() { return "cadDisplayName"; }
  /// whether to enable suspicious region tool
  inline static std::string EnableSRT() { return "enableSRT"; }
  /// output suspicious results for CAD vendor
  inline static std::string OutputSuspiciousROI() { return "outputSROI"; }
  /// using ULD volume for improving 3D endo fly performance for uncleansing images
  inline static std::string UseULDVolume() { return "useULDVolume"; }
	/// enable dual display layouts
	inline static std::string EnableDualDisplayTabs() { return "Booking"; }
	/// flag for display 2D ruler in all 2D views
	inline static std::string Draw2DRuler() { return "Draw2DRuler"; }
	/// flag for display 2D ruler in all 2D views
	inline static std::string NormalizeSnapshot() { return "NormalizeSnapshot"; }

public:
  /// cad vendor information, small helper class to store information, no methods, doesn't need separate module
  class CadVendorInfo
  {
  public:
    /// default constructor
    CadVendorInfo() {}
    /// constructor
    CadVendorInfo(const std::string & sname, const std::string & spath, const std::string & ssite)
      : m_sVendor(sname), m_sImagePath(spath), m_sWebsite(ssite) { }
      std::string m_sVendor, m_sImagePath, m_sWebsite;
  }; // class CadVendorInfo
  /// maps cad vendor ID to information on the vendor
  typedef std::map<std::string, CadVendorInfo> CadVendorMap;


public:

  /// constructor
  VCSettings( const std::string & sPath );

  VCSettings( const VCSettings & other );

  VCSettings( VCSettings&& other );

  /// destructor
  virtual ~VCSettings();

  /// Copy assignment operator.
  inline VCSettings& operator=( const VCSettings& other ) 
  { 
    if( this != &other ) 
      m_sSettingsPath = other.m_sSettingsPath; 
    return *this; 
  }

  /// move assignment operator.
  inline VCSettings& operator=( VCSettings&& other )
  {
    if( this != &other ) 
    {
      m_sSettingsPath = other.m_sSettingsPath; 
      other.m_sSettingsPath = nullptr; 
    }
    return *this; 
  }

private:

  VCSettings();

public:

  /// Returns the section xml from yje VC Settings file.
  std::string GetSection(const std::string& sSectionName) const;

  /// creates the static settings
  static const VCSettings & GetInstance();

  /// get name of cad vendor and path to a image
  void GetCadVendors(CadVendorMap & vendorMap) const;

  /// returns the settings path
  const std::string & GetSettingsPath() const { return m_sSettingsPath; }

  /// set the path to the settings file
  void SetSettingsPath( const std::string & sSettingsPath );

  /// Returns the string value for the tag's "Value" attribute
  std::string GetValue(const std::string& sTagName, const std::string& sEntry = "", const std::string & sAttr = "value" ) const;

  /// Returns the value for the tag's "Value" attribute, casted to a bool
  bool GetBoolValue(const std::string& sTagName, const std::string& sEntry = "" ) const;

  /// Returns the value for the tag's "Value" attribute, casted to a float
  float4 GetFloatValue(const std::string& sTagName, const std::string& sEntry = "" ) const;

  /// Returns the value for the tag's "Value" attribute, casted to a integer
  int4 GetIntValue(const std::string& sTagName, const std::string& sEntry = "" ) const;

private:
  /// Initializes default values for settings
  bool AddMissingSettings( bool bSave );
  /// Writes settings with default values to settings file
  void WriteDefaultSettings();
  /// Returns the node's element list
  vxDOMNodeList GetElements(const std::string& sTagName, const std::string& sEntry) const;

private:

  /// Ensures settings will be read once per application setting
  static bool m_bSettingsRead;

  /// stores the path to the settings file
  std::string m_sSettingsPath;

  /// settings document
  static vxDOMDocument * m_pDocSettings;

}; // class VCSettings


// $Log: VCSettings.h,v $
// Revision 1.19.2.5  2010/12/20 19:06:05  dongqing
// add VCSetting switch for using ULD volume
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.19.2.4  2008/01/17 20:51:25  dongqing
// add OutputSuspiciousROI switch
//
// Revision 1.19.2.3  2007/08/30 12:55:29  dongqing
// add residue percent threshold for preprocessing
//
// Revision 1.19.2.2  2007/04/12 16:08:09  jmeade
// system setting for enabling srt tool.
//
// Revision 1.19.2.1  2007/04/04 20:04:53  jmeade
// politically-correct names for CAD in menus.
//
// Revision 1.19  2007/02/15 00:08:23  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.18  2006/12/05 20:18:36  jmeade
// Issue 5047: exposing xml helper class.
//
// Revision 1.17  2006/11/24 21:25:31  jmeade
// alternate projection layouts entry.
//
// Revision 1.16  2006/10/09 17:54:54  frank
// made the UD segment image size adjustable
//
// Revision 1.15  2006/10/04 20:56:41  frank
// Issue #5041: Made automatic dataset linking a configuration setting
// Issue #5042: Made centerline display default a configuration setting
//
// Revision 1.14  2006/02/07 21:17:01  jmeade
// code review: comments.
//
// Revision 1.13  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.12.2.3  2006/08/02 17:42:27  frank
// added capability to export missed region data
//
// Revision 1.12.2.2  2006/01/05 21:41:19  frank
// Added logic for controlling visibility of cad findings according to sphericity slider threshold
//
// Revision 1.12.2.1  2006/01/03 23:59:46  jmeade
// Issue 4507: Config option for dual-fly mode.
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
// Revision 1.1.2.4.2.3.2.1  2005/06/17 00:23:23  jmeade
// write out default values for older versions of file that are missing entries.
//
// Revision 1.1.2.4.2.3  2005/06/10 18:18:12  geconomos
// Added settings for 3D navigation and rendering.
//
// Revision 1.1.2.4.2.2  2005/03/10 20:21:31  dongqing
// add GetCompressed3DSliceBound() entry
//
// Revision 1.1.2.4.2.1  2005/03/07 20:04:05  dongqing
// merge from enhanced VC_1-4
//
// Revision 1.1.2.4  2005/02/15 20:04:58  jmeade
// merge from vc1.4 branch.
//
// Revision 1.1.2.3.2.3  2005/02/10 16:58:52  dongqing
// add Half3DOn switch
//
// Revision 1.1.2.3.2.2  2005/01/21 21:45:18  dongqing
// take out the bKeepOriginalVol check
//
// Revision 1.1.2.3.2.1  2005/01/20 16:38:05  dongqing
// Add bKeepOriginalVol switch in VCSettings
//
// Revision 1.1.2.3  2004/09/27 20:54:37  jmeade
// Preference to disable 2d auto flight.
//
// Revision 1.1.2.2  2004/08/12 15:37:46  jmeade
// Default xml file.
//
// Revision 1.1.2.1  2004/08/11 03:52:39  jmeade
// xml file for vc settings.
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/VCSettings.h,v 1.19.2.5 2010/12/20 19:06:05 dongqing Exp $
// $Id: VCSettings.h,v 1.19.2.5 2010/12/20 19:06:05 dongqing Exp $
