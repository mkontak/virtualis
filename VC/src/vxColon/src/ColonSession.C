// $Id: ColonSession.C,v 1.19.2.8 2009/08/24 22:24:49 gdavidson Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeffrey Meade  jmeade@viatronix.net


// includes
#include "stdafx.h"
#include "ColonSession.h"
#include "Dataset.h"
#include "File.h"
#include "FileExtensions.h"
#include "ReaderGlobal.h"
#include "vxDOMNodeList.h"
#include "vxSuspiciousROIoutput4CADpartner.h"
#include "FilePtrFactory.h"

// imports
#import "msxml3.dll"


// defines
#define FILE_REVISION "$Revision: 1.19.2.8 $"
#define COLON_SESSION_HEADER  "V3DColonSessionFile"
enum ColonSessionFileVersionEnum { eColonSessionFileVersion = 0x02 };


// namespaces
using namespace ReaderLib;


//////////////////////////////////////////////////////////////////////
// ColonSession::FindingInfo


/**
 * Constructor
 */
ColonSession::FindingInfo::FindingInfo()
: m_eDecision( eDecisionUndecided ),
m_iSkeletonIndex( 0 ),
m_bInLumen( true )
{
} // FindingInfo()


/**
 * Copy constructor
 *
 * @param other the other finding information object
 */
ColonSession::FindingInfo::FindingInfo( const ColonSession::FindingInfo & other )
: m_eDecision( other.m_eDecision ),
m_iSkeletonIndex( other.m_iSkeletonIndex ),
m_bInLumen( other.m_bInLumen )
{
} // FindingInfo()


/**
 * Assignment operator
 *
 * @param other the other finding information object
 *
 * @return itself
 */
ColonSession::FindingInfo & ColonSession::FindingInfo::operator =( const ColonSession::FindingInfo & other )
{
  m_eDecision = other.m_eDecision;
  m_iSkeletonIndex = other.m_iSkeletonIndex;
  m_bInLumen = other.m_bInLumen;
  return * this;
} // operator =()

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ColonSession

/**
 * Constructor
 */
ColonSession::ColonSession() : 
m_bookmarkList(vx::BookmarkExt), m_2DAnnotationList(vx::AnnotationExt), 
m_sVersion("4.2"), m_bIsReadOnly( false )  /// version 4.1 add the public m_bIsReadOnly to flag the session status of editing
                                           /// version 4.2 add camera info for each 3D line measure
{
} // ColonSession()


/**
 * Clears all session data.
 */
void ColonSession::Reset()
{
  m_bookmarkList.Reset();
  m_2DAnnotationList.Reset();
  m_vUserFeatures.clear();
  m_vSegmentOrder.clear();
  m_measureLineList.clear();
	m_bIsReadOnly = false;
} // Reset()


/**
 * Reads a session file, or the (previous) individual data files.
 *
 * @param sFileTitle              Study file title.
 * @param inSegmentListForOrder   Segment order list to set.
 * @return bool                   Success/failure.
 */
bool ColonSession::Read(const std::string& sFileTitle, Dataset& inDataset)
{
  bool bSessionRead(false);

  bool bSegmentsRead(false);
  try
  {
    // Read xml
    std::string sSessionFilename( sFileTitle + vx::ColonSessionXMLExt );
    LogDbg( "Reading session: " + sSessionFilename, "ColonSession", "Read" );


    bSessionRead = io::FilePtrFactory::Exists( sSessionFilename ) && ReadXML( sFileTitle, inDataset);

    // Read vcsn if no xml
    if (!bSessionRead)
    {
      std::ifstream file((sFileTitle + ".vcsn").c_str(), std::ios::binary | std::ios::in);

      if (file.is_open())
      {
        // Read file header
        std::string sHeader;
        InputTermFmtString(file, sHeader);

        // Read version number
        int4 iVersion;
        file >> iVersion;
        m_sVersion = (iVersion < 3) ? ToAscii(iVersion) : "2.0";

        // Read bookmarks, 2D annotations, user registration points, and 3D measure lines
        file >> m_bookmarkList >> m_2DAnnotationList >> m_vUserFeatures >> m_measureLineList;

        // Read segment ordering
        SegmentList::SegmentOrderVector vSegOrder; file >> vSegOrder;
        inDataset.m_segmentList.SetSegmentOrder(vSegOrder);
        bSegmentsRead = true;

        // Read current world coordinates
        file >> m_worldCamera;

        file.close();
        bSessionRead = true;
      }
    }

    // Else read from individual files
    if (!bSessionRead)
    {
      LogDbg("Session file not found/read, reading session data from separate files.", "ColonSession", "Read");

      inDataset.m_segmentList.SelectAllSegments();
      bSegmentsRead = true;

      const bool bBookmarksFileRead = m_bookmarkList.ReadFile(sFileTitle);
      const bool bAnnotationsFlRead = m_2DAnnotationList.ReadFile(sFileTitle);
      if (bBookmarksFileRead || bAnnotationsFlRead)
      {
        m_sVersion = "1.5";
      }

      // Note: User registration points and measurement lines never saved in individual files (i.e. in previous
      //  versions, so there is nothing to read here for <m_userRegPtsList> or <m_measureLineList>

      Segment & segment = inDataset.m_segmentList.GetSegment(0);
      const SkeletonNode& firstNode = segment.GetBeginningSkelNode();
      m_worldCamera = Camera(firstNode.m_vWorldPos, firstNode.m_vWorldTan, firstNode.m_vWorldUp);
    }

  }
  catch (...)
  {
    LogErr("Exception caught while opening/reading session.", "ColonSession", "Read");

    if (!bSegmentsRead)
    {
      inDataset.m_segmentList.SelectAllSegments();
    }

    try
    {
      Segment & segment = inDataset.m_segmentList.GetSegment(0);
      const SkeletonNode& firstNode = segment.GetBeginningSkelNode();
      m_worldCamera = Camera(firstNode.m_vWorldPos, firstNode.m_vWorldTan, firstNode.m_vWorldUp);
    }
    catch (...)
    {
    }

    return false;
  }

  return true;
} // Read()


/**
 * Writes session data to disk.
 *
 * @param sFileTitle               Study file title.
 * @param outSegmentListForOrder   Segment order list to output.
 * @return bool                    Success/failure.
 */
bool ColonSession::Write(const std::string & sFileTitle, const Dataset& outDataset) const
{
  /// The following codes are for CAD vendor creat CAD truth from the suspicious ROI tool
  /// It is not for release product.
  try
  {
    if(rdrGlobal.GetSettings()->GetBoolValue(VCSettings::ViewerXML(), VCSettings::OutputSuspiciousROI()))
    {
      std::string sFileName = sFileTitle + "_cCAD.txt";
      vxSuspiciousROIoutput4CADpartner output( sFileName );
      output.Write( outDataset );
    } // if
  } // try
  catch (...)
  {
    LogErr("Fail to write out the suspicious ROI", "ColonSession.C", "Write");
  }

  // Output XML session data
  if (WriteXML(sFileTitle, outDataset))
  {
    // Delete any obsolete format data if xml output successful
    try
    {
      vx::File(sFileTitle + ".vcsn").Delete();
      vx::File(sFileTitle + vx::AnnotationExt).Delete();
      vx::File(sFileTitle + vx::BookmarkExt).Delete();
    }
    catch (...)
    {
      LogWrn("Error attempting to remove obsolete session data", "ColonSession", "Write");
    }
    return true;
  }

  try
  {
    std::ofstream file((sFileTitle + ".vcsn").c_str(), std::ios::binary | std::ios::out | std::ios::trunc);

    if (!file.is_open())
    {
      return false;
    }

    // Write file header and version
    OutputTermFmtString(file, std::string("V3D.Colon.Session.File"));
    file << eColonSessionFileVersion;

    // Write bookmarks, 2D annotations, user registration points, 3D measurement lines
    file << m_bookmarkList << m_2DAnnotationList << m_vUserFeatures << m_measureLineList;

    // Write segment ordering
    file << outDataset.m_segmentList.GetSegmentOrder();

    // Write current world coordinates
    file << Camera(outDataset.m_submarine.GetPositionWorld(),
                   outDataset.m_submarine.GetOrientation(),
                   outDataset.m_submarine.GetUpDirection());

    file.close();
  }
  catch (...)
  {
    return false;
  }

  return true;
} // Write()


////////////////////////////////////////////////////////////////////////////////////////////////////
// Session XML I/O

/**
 * XML attribute names
 */
namespace ColonSessionXML
{
  const std::string sVRP("VRP");
  const std::string sVPN("VPN");
  const std::string sVUP("VUP");
  const std::string sPriority("Priority");

  const std::string sUserFeatures("UserFeatures");
  namespace UserFeatures
  {
    const std::string sFeatures("Features");
  };

  const std::string sSegmentList("SegmentList");
  namespace SegmentList
  {
    const std::string sOrder("Order");
  };

  const std::string sCamera("Camera");
  namespace Camera
  {
    const std::string sView("View");
  }

  const std::string sFlightParameters("FlightParameters");
  namespace FlightParameters
  {
    const std::string sDirection("Direction");
  }

  const std::string sAlwaysShowCADMenuStatus("AlwaysShowCADMenuStatus");
  namespace AlwaysShowCADMenuStatus
  {
    const std::string sStatus("Status");
  }

  const std::string sIsReadOnly("IsReadOnly");
  namespace IsReadOnly
  {
    const std::string sFlag("Flag");
  }

}; // namespace ColonSessionXML


typedef std::list < std::pair <std::string,std::string> > stringpairlist;
typedef std::map <std::string, std::string> XMLAttribToValueMap;
/**
 * Map of item names to item values
 */
struct XMLAttribNode
{
  MSXML2::IXMLDOMNodePtr pNode; std::string sNodeName;
  XMLAttribToValueMap mapAttribVals;
};
typedef std::multimap<std::string,XMLAttribNode> XMLAttribNodeMap;


/**
 * Gets a list of items
 *
 * @param pNodeList           List of nodes
 * @return XMLAttribNodeMap   Maps item names to item values
 */
XMLAttribNodeMap GetListItemMap(MSXML2::IXMLDOMNodeListPtr pNodeList)
{
  XMLAttribNodeMap mapAttrib;

  for (int4 iItem(0), iListLen(pNodeList->Getlength()); iItem < iListLen; iItem++)
  {
    MSXML2::IXMLDOMNodePtr pIDOMNode = pNodeList->Getitem(iItem);
    MSXML2::IXMLDOMNamedNodeMapPtr pNodeMap = pIDOMNode->Getattributes();

    MSXML2::IXMLDOMNodePtr pItem = NULL;

    XMLAttribNode attNode;
    attNode.pNode = pIDOMNode;
    attNode.sNodeName = (const char *) pIDOMNode->GetnodeName();

    if (pNodeMap->Getlength() > 0)
    {
      for (int4 iNode(0); iNode < pNodeMap->Getlength(); iNode++)
      {
        pItem = pNodeMap->Getitem(iNode);
        attNode.mapAttribVals[(const char *) pItem->GetnodeName()] = (const char *) _bstr_t(pItem->GetnodeValue());
      }
    }
    else
    {
      pItem = pIDOMNode;
    }

    mapAttrib.insert(make_pair(attNode.sNodeName, attNode));
  }

  return mapAttrib;
} // GetListItemMap()


/**
 * Reads a bookmark list from an XML document node.
 *
 * @param pXmlDoc          XML document.
 * @param sListNodeQuery   Name of node header.
 * @param outBkList        [out] Bookmark list.
 * @return bool            Success/failure.
 */
bool ReadBookmarkList(MSXML2::IXMLDOMDocument2Ptr pXmlDoc, const std::string& sListNodeQuery, BookmarkList& outBkList)
{
  MSXML2::IXMLDOMNodeListPtr pIDOMNodeList;
  pIDOMNodeList = pXmlDoc->documentElement->selectNodes(sListNodeQuery.c_str());

  if (pIDOMNodeList == NULL)
  {
    return false;
  }

  outBkList.clear();
  XMLAttribNodeMap nodeMap = GetListItemMap(pIDOMNodeList);

  for (XMLAttribNodeMap::iterator nodeIt = nodeMap.begin(); nodeIt != nodeMap.end(); nodeIt++)
  {
    XMLAttribNode& rNode = nodeIt->second;
    MSXML2::IXMLDOMNodeListPtr pChildList = rNode.pNode->GetchildNodes();
    XMLAttribNodeMap childNodeMap = GetListItemMap(pChildList);

    Bookmark bm;
    bm.m_sName = rNode.mapAttribVals.begin()->second; // sAttribValue;

    XMLAttribNodeMap::const_iterator itEnt;

    if ((itEnt = childNodeMap.find(ColonSessionXML::sVRP)) != childNodeMap.end())
    { std::istrstream(itEnt->second.mapAttribVals.begin()->second.c_str()) >> bm.m_position; }
    if ((itEnt = childNodeMap.find(ColonSessionXML::sVPN)) != childNodeMap.end())
    { std::istrstream(itEnt->second.mapAttribVals.begin()->second.c_str()) >> bm.m_orientation; }
    if ((itEnt = childNodeMap.find(ColonSessionXML::sVUP)) != childNodeMap.end())
    { std::istrstream(itEnt->second.mapAttribVals.begin()->second.c_str()) >> bm.m_upDirection; }
    if ((itEnt = childNodeMap.find(ColonSessionXML::sPriority)) != childNodeMap.end())
    { bm.m_ePriority = Bookmark::PriorityEnum(atoi(itEnt->second.mapAttribVals.begin()->second.c_str())); }

    outBkList.push_back(bm);
  }

  return true;
} // ReadBookmarkList()


/**
 * Reads session data from XML file.
 *
 * TDOD: REST Need to be able to read the XML from the FileServer
 *
 * @param sFileTitle   File path and title.
 * @param inDataset    Dataset for session.
 * @return bool        Success/failure.
 */
bool ColonSession::ReadXML(const std::string& sFileTitle, Dataset& inDataset)
{
  try
  {
    MSXML2::IXMLDOMDocument2Ptr pXmlDoc;
    HRESULT hRes = pXmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument30));
    pXmlDoc->async = VARIANT_FALSE;

    const std::string sSessionFile = sFileTitle + vx::ColonSessionXMLExt;
    LogDbg( "Opening Session from " + sSessionFile, "ColonSession", "ReadXML" );
    if (pXmlDoc->load((sSessionFile).c_str()) != VARIANT_TRUE)
    {
      throw ex::IOException(LogRec("Could not read Colon Session XML file", "ColonSession", "Read"));
    }

    MSXML2::IXMLDOMNodeListPtr pIDOMNodeList;

    //{{ 0. Session data
    pIDOMNodeList = pXmlDoc->documentElement->selectNodes("//Session");
    if ((pIDOMNodeList != NULL) && (pIDOMNodeList->Getlength() > 0))
    {
      MSXML2::IXMLDOMNodePtr pItem = pIDOMNodeList->Getitem(0)->Getattributes()->getNamedItem("Version");
      if (pItem != NULL)
        std::istrstream( (const char *) _bstr_t(pItem->GetnodeValue().bstrVal, true)) >> m_sVersion;
      else
        m_sVersion = "1.0";
    }

    //}} 0. Session data

    // 1. Bookmarks
    ReadBookmarkList(pXmlDoc, "//Session/BookmarkList/Bookmark", m_bookmarkList);
    // 2. Annotations
    ReadBookmarkList(pXmlDoc, "//Session/AnnotationList/Annotation", m_2DAnnotationList);

    //{{ 3. User features
    pIDOMNodeList = pXmlDoc->documentElement->selectNodes(("//Session/"+ColonSessionXML::sUserFeatures).c_str());
    if ((pIDOMNodeList != NULL) && (pIDOMNodeList->Getlength() > 0))
    {
      MSXML2::IXMLDOMNodePtr pItem = pIDOMNodeList->Getitem(0)->Getattributes()->
        getNamedItem((ColonSessionXML::UserFeatures::sFeatures).c_str());
      //std::istrstream((const char *) _bstr_t(pItem->GetnodeValue().bstrVal, true)) >> m_vUserFeatures;
			// The above line caused trouble for Visual Studio 2010 compiler. The following alternative works.
      std::strstream sstr_temp;
      sstr_temp << ((const char *) _bstr_t(pItem->GetnodeValue().bstrVal, true));
      sstr_temp >> m_vUserFeatures;

		}
    else if(pIDOMNodeList == NULL)
      LogFyi("User features list is empty", "ColonSession", "ReadXML");
    //}} 3. User features

    //{{ 4. Measurements
    try
    {
      pIDOMNodeList = pXmlDoc->documentElement->selectNodes("//Session/MeasureLineList/Measurement");

      if (pIDOMNodeList != NULL)
      {
        m_measureLineList.clear();
        XMLAttribNodeMap nodeMap = GetListItemMap(pIDOMNodeList);

        for (XMLAttribNodeMap::iterator nodeIt = nodeMap.begin(); nodeIt != nodeMap.end(); nodeIt++)
        {
          MeasureLine mLine;
          std::istrstream(nodeIt->second.mapAttribVals.begin()->second.c_str()) >> mLine;
					if (GetVersion() < 4.2f) mLine.m_bIsCameraInfoAvailable = false;  /// This is only for backward compatibility
          m_measureLineList.push_back(mLine);
        }
      }
    }
    catch (...)
    {
      LogWrn("An exception occurred while reading measurements nodes data.", "ColonSession", "ReadXML");
    }
    //}} 4. Measurements

    //{{ 5. Segment list
    pIDOMNodeList = pXmlDoc->documentElement->selectNodes(("//Session/"+ColonSessionXML::sSegmentList).c_str());
    if ((pIDOMNodeList != NULL) && (pIDOMNodeList->Getlength() > 0))
    {
      MSXML2::IXMLDOMNodePtr pItem = pIDOMNodeList->Getitem(0)->Getattributes()->
        getNamedItem((ColonSessionXML::SegmentList::sOrder).c_str());

      typedef std::vector< std::pair<uint4,bool> > UIntBoolPairVector;
      UIntBoolPairVector vOrder;

			std::strstream sstr_temp;
      sstr_temp << ((const char *) _bstr_t(pItem->GetnodeValue().bstrVal, true));
      sstr_temp >> vOrder;

      SegmentList::SegmentOrderVector vSegOrder; vSegOrder.resize(vOrder.size());
      for (uint4 i(0); i < vOrder.size(); vSegOrder[i] = vOrder[i], i++);

      inDataset.m_segmentList.SetSegmentOrder(vSegOrder);
    }
    //}} 5. Segment list

    //{{ 6. Camera
    pIDOMNodeList = pXmlDoc->documentElement->selectNodes("//Session/Camera");
    if ((pIDOMNodeList != NULL) && (pIDOMNodeList->Getlength() > 0))
    {
      MSXML2::IXMLDOMNodeListPtr pChildList = pIDOMNodeList->Getitem(0)->GetchildNodes();
      XMLAttribNodeMap nodeMap = GetListItemMap(pChildList);
      XMLAttribNodeMap::const_iterator itEnt;

      std::strstream ssCamera;
      if ((itEnt = nodeMap.find("Header")) != nodeMap.end()) { ssCamera << itEnt->second.mapAttribVals.begin()->second; }
      ssCamera << std::ends << std::endl;
      if ((itEnt = nodeMap.find("View")) != nodeMap.end())   { ssCamera << itEnt->second.mapAttribVals.begin()->second; }

      ssCamera >> m_worldCamera;
    }
    //}} 6. Camera

    //{{ 7. Flight Direction
    pIDOMNodeList = pXmlDoc->documentElement->selectNodes(("//Session/"+ColonSessionXML::sFlightParameters).c_str());
    if ((pIDOMNodeList != NULL) && (pIDOMNodeList->Getlength() > 0))
    {
      variant_t varValue = pIDOMNodeList->Getitem(0)->Getattributes()->
        getNamedItem((ColonSessionXML::FlightParameters::sDirection).c_str())->GetnodeValue();
      inDataset.m_submarine.SetFlightDirection(Submarine::NavigationDirection(atoi((const char *) _bstr_t(varValue.bstrVal, true))));
    }
    //}} 7. Flight Direction
 
    // 8. Status of Always Show CAD Marks
    float4 fSessionVersion = atof(m_sVersion.c_str());
    if( fSessionVersion > 3.0F )
    {
      pIDOMNodeList = pXmlDoc->documentElement->selectNodes(("//Session/"+ColonSessionXML::sAlwaysShowCADMenuStatus).c_str());
      {
        // 8. Status of Always Show CAD Marks
        variant_t varValue = pIDOMNodeList->Getitem(0)->Getattributes()->
          getNamedItem((ColonSessionXML::AlwaysShowCADMenuStatus::sStatus).c_str())->GetnodeValue();

        std::string ss = _bstr_t(varValue.bstrVal);
        if ( ss == "true" )
        {
          rdrGlobal.m_bAlwaysShowCadArrows = true; 
        }
        else
        {
          rdrGlobal.m_bAlwaysShowCadArrows = false; 
        }
      }  // 8. Status of Always Show CAD Marks
    } // if session version is greater than 3.0

    //{{ 8. Cad Findings data
    pIDOMNodeList = pXmlDoc->documentElement->selectNodes("//Session/CadFindings");
    if ((pIDOMNodeList != NULL) && (pIDOMNodeList->Getlength() > 0))
    {
      variant_t varValue = pIDOMNodeList->Getitem(0)->Getattributes()->getNamedItem("ShowAll")->GetnodeValue();
      rdrGlobal.m_bDisplayCadFindings = (bool(atoi((const char *) _bstr_t(varValue.bstrVal, true))));

      MSXML2::IXMLDOMNodeListPtr pChildList = pIDOMNodeList->Getitem(0)->GetchildNodes();
      XMLAttribNodeMap nodeMap = GetListItemMap(pChildList);
      for (XMLAttribNodeMap::iterator nodeIt = nodeMap.begin(); nodeIt != nodeMap.end(); nodeIt++)
      {
        XMLAttribToValueMap & attMap = nodeIt->second.mapAttribVals;
        if (attMap.size() == 2)
        {
          XMLAttribToValueMap::iterator attIt = attMap.find("GUID");
          XMLAttribToValueMap::iterator visIt = attMap.find("Visibility");
          if (attIt != attMap.end() && visIt != attMap.end())
          {
            m_mapCadFindingInfo[ attIt->second ].SetDecision( ColonSession::FindingInfo::DecisionEnum( atoi( visIt->second.c_str() ) ) );
          }
        }
      }
    }
    //}} 8. Cad Findings data

    // P.S. XML Data with vx classes
    vxDOMDocument vxdocSession(vxDOMDocument::FromFile( sSessionFile ));
    {
      //{{ 9. SRT Findings
      vxDOMNodeList findingNodeList = vxdocSession.GetDocumentElement().GetElementsByTagName( "SRTFindings" );
      if (findingNodeList.GetLength() > 0)
      {
        inDataset.m_suspiciousRegions = vxCAD::vxCadFindings::FromXml(findingNodeList.Item( 0 ).ToElement());
      }
      //}} 9. SRT Findings
    }

    if( fSessionVersion >= 4.1F )
    {
			pIDOMNodeList = pXmlDoc->documentElement->selectNodes(("//Session/"+ColonSessionXML::sIsReadOnly).c_str());
      { // 10. Status flag
        variant_t varValue = pIDOMNodeList->Getitem(0)->Getattributes()->
					getNamedItem((ColonSessionXML::IsReadOnly::sFlag).c_str())->GetnodeValue();

        std::string ss = _bstr_t(varValue.bstrVal);
        if ( ss == "true" )
        {
					m_bIsReadOnly = true;
          rdrGlobal.m_bReadOnly = true; 
        } // if
      }  // 10. Flag of editbility
    } // if session version >= 4.1

  }
  catch (...)
  {
    LogErr("An exception occurred while reading xml data.","ColonSession","Read");
    return false;
  }

  return true;
} // ReadXML()


/**
 * Appends an attribute/child pair to a parent node.
 *
 * @param pXmlDoc          XML document.
 * @param pParentElement   Parent node.
 * @param sElement         Attribute child name.
 * @param sAttrName        Child attribute name.
 * @param sAttrValue       Child attribute value.
 * @return                 The new element.
 */
MSXML2::IXMLDOMElementPtr AppendAttributeChild(MSXML2::IXMLDOMDocument2Ptr pXmlDoc, MSXML2::IXMLDOMNodePtr pParentElement, const std::string& sElement,
                                       const std::string& sAttrName = "", const std::string& sAttrValue = "")
{
  MSXML2::IXMLDOMElementPtr pElement = pXmlDoc->createElement(sElement.c_str());

  if (!sAttrName.empty() && !sAttrValue.empty())
  {
    MSXML2::IXMLDOMAttributePtr pAttr = pXmlDoc->createAttribute(sAttrName.c_str());
    pAttr->value = sAttrValue.c_str();
    pElement->setAttributeNode(pAttr);
  }
  pParentElement->appendChild(pElement);
  return pElement;
} // AppendAttributeChild()


/**
 * Appends a list of attribute/child pairs to a parent node.
 *
 * @param pXmlDoc              XML document.
 * @param pParentElement       Parent node.
 * @param sElement             Attribute child name.
 * @param listAttrValue        Pairs of child attribute names and value.
 * @return                     The new element.
 */
MSXML2::IXMLDOMElementPtr AppendAttributeChild(MSXML2::IXMLDOMDocument2Ptr pXmlDoc, MSXML2::IXMLDOMNodePtr pParentElement,
                                               const std::string& sElement, const stringpairlist & listAttrValue)
{
  MSXML2::IXMLDOMElementPtr pElement = pXmlDoc->createElement(sElement.c_str());

  for (stringpairlist::const_iterator it = listAttrValue.begin(); it != listAttrValue.end(); it++)
  {
    MSXML2::IXMLDOMAttributePtr pAttr = pXmlDoc->createAttribute(it->first.c_str());
    pAttr->value = it->second.c_str();
    pElement->setAttributeNode(pAttr);
  }
  pParentElement->appendChild(pElement);
  return pElement;
} // AppendAttributeChild


/**
 * Appends a bookmark list to the xml document.
 *
 * @param pXmlDoc              XML document.
 * @param bmlist               The bookmark list to append to document.
 * @param sListName            (Attribute) name of list.
 * @param sEntryName           Entry name.
 * @return MSXML2::IXMLDOMElementPtr   Newly created bookmark list document element.
 */
MSXML2::IXMLDOMElementPtr AppendBookmarkList(MSXML2::IXMLDOMDocument2Ptr pXmlDoc, const BookmarkList& bmlist, const std::string& sListName,
                                     const std::string& sEntryName)
{
  MSXML2::IXMLDOMElementPtr pElement = AppendAttributeChild(pXmlDoc, pXmlDoc->documentElement, sListName.c_str(), "Version",
    ToAscii(BookmarkList::eBOOKMARK_FILE_CURRENT_VERSION).c_str());
  MSXML2::IXMLDOMDocumentFragmentPtr pdf;
  pdf = pXmlDoc->createDocumentFragment();
  for (BookmarkList::const_iterator it = bmlist.begin(); it != bmlist.end(); it++)
  {
    MSXML2::IXMLDOMElementPtr pBkmElement = pXmlDoc->createElement(sEntryName.c_str());
    MSXML2::IXMLDOMAttributePtr pBkmAttr = pXmlDoc->createAttribute("Name");
    pBkmAttr->value = it->m_sName.c_str();
    pBkmElement->setAttributeNode(pBkmAttr);

    std::strstream ptSstr;

    ptSstr << it->m_position << std::ends;
    AppendAttributeChild(pXmlDoc, pBkmElement, ColonSessionXML::sVRP, "Value", ptSstr.str());
    ptSstr.freeze(false); ptSstr.seekp(0);

    ptSstr << it->m_orientation << std::ends;
    AppendAttributeChild(pXmlDoc, pBkmElement, ColonSessionXML::sVPN, "Value", ptSstr.str());
    ptSstr.freeze(false); ptSstr.seekp(0);

    ptSstr << it->m_upDirection << std::ends;
    AppendAttributeChild(pXmlDoc, pBkmElement, ColonSessionXML::sVUP, "Value", ptSstr.str());
    ptSstr.freeze(false); ptSstr.seekp(0);

    AppendAttributeChild(pXmlDoc, pBkmElement, ColonSessionXML::sPriority, "Value", ToAscii(it->m_ePriority).c_str());

    pdf->appendChild(pBkmElement);
  }

  pElement->appendChild(pdf);

  return pElement;
} // AppendBookmarkList


/**
 * Outputs an session as XML.
 *
 * @param sFileTitle   File path and title to output.
 * @param outDataset   Dataset whose session to output.
 * @return bool        Success/failure.
 */
bool ColonSession::WriteXML(const std::string& sFileTitle, const Dataset& outDataset) const
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
        pInstr = pXmlDoc->createProcessingInstruction("xml", "version='1.0'");
        pXmlDoc->appendChild(pInstr);
#ifdef VCSX_XML_STYLESHEET_DONE
        pInstr = pXmlDoc->createProcessingInstruction("xml-stylesheet", "type='text/xml' href='vcsx.xsl'");
        pXmlDoc->appendChild(pInstr);
#endif

        MSXML2::IXMLDOMElementPtr pElement;
        MSXML2::IXMLDOMAttributePtr pAttr;

        std::strstream sstr;

        //{{ 0. Session header element
        stringpairlist listSessionAttr;
        listSessionAttr.push_back(std::pair<std::string,std::string> ("Name", vx::File(sFileTitle).GetTitle().c_str()) );
        listSessionAttr.push_back(std::pair<std::string,std::string> ("Version" , m_sVersion) );  /// version 4.1 add m_bIsReadOnly flag  
				                                                                                          /// 4.2 add camera setting record for 3D line measure
        pElement = AppendAttributeChild(pXmlDoc, pXmlDoc, "Session", listSessionAttr);

        // 1. Bookmarks
        pElement = AppendBookmarkList(pXmlDoc, m_bookmarkList, "BookmarkList", "Bookmark");
        // 2. Annotations
        pElement = AppendBookmarkList(pXmlDoc, m_2DAnnotationList, "AnnotationList", "Annotation");

        //{{ 3. User features
        sstr << m_vUserFeatures << std::ends;
        AppendAttributeChild(pXmlDoc, pXmlDoc->documentElement, ColonSessionXML::sUserFeatures,
          ColonSessionXML::UserFeatures::sFeatures, sstr.str());
        sstr.freeze(false); sstr.seekp(0);
        //}} 3. User features

        //{{ 4. Measure line list
        MSXML2::IXMLDOMElementPtr pMsrListElement = pXmlDoc->createElement("MeasureLineList");
        for (MeasureLineList::const_iterator it = m_measureLineList.begin(); it != m_measureLineList.end(); it++)
        {
          sstr << *it << std::ends;
          AppendAttributeChild(pXmlDoc, pMsrListElement, "Measurement", "Value", sstr.str());
          sstr.freeze(false); sstr.seekp(0);
        }
        pXmlDoc->documentElement->appendChild(pMsrListElement);
        //}} 4. Measure line list

        //{{ 5. Segment order
        std::vector< std::pair<uint4,bool> > vSegOrder = outDataset.m_segmentList.GetSegmentOrder();
        sstr << vSegOrder << std::ends;
        AppendAttributeChild(pXmlDoc, pXmlDoc->documentElement, ColonSessionXML::sSegmentList,
          ColonSessionXML::SegmentList::sOrder, sstr.str());
        sstr.freeze(false); sstr.seekp(0);
        //}} 5. (end) Segment order

        //{{ 6. World view Camera
        sstr << Camera(outDataset.m_submarine.GetPositionWorld(), outDataset.m_submarine.GetOrientation(),
          outDataset.m_submarine.GetUpDirection()) << std::ends;

        pElement = AppendAttributeChild(pXmlDoc, pXmlDoc->documentElement, "Camera");
        AppendAttributeChild(pXmlDoc, pElement, "Header", "Version", sstr.str());
        // skip past header
        sstr.freeze(false); sstr.ignore(sstr.pcount(), '\0');  sstr.ignore(1, '\n'); // std::ends and std::endl
        AppendAttributeChild(pXmlDoc, pElement, "View", "Value", sstr.str());
        sstr.freeze(false); sstr.seekp(0);
        //}} 6. (end) World view Camera

        // 7. Flight params
        AppendAttributeChild(pXmlDoc, pXmlDoc->documentElement, ColonSessionXML::sFlightParameters,
          ColonSessionXML::FlightParameters::sDirection, ToAscii(outDataset.m_submarine.GetFlightDirection()));

        // 8. Status of Always Show CAD Marks
        std::string sStatus = "false";
        if( rdrGlobal.m_bAlwaysShowCadArrows )
        {
          sStatus = "true";
        } // if
        AppendAttributeChild(pXmlDoc, pXmlDoc->documentElement, ColonSessionXML::sAlwaysShowCADMenuStatus,
                             ColonSessionXML::AlwaysShowCADMenuStatus::sStatus, sStatus.c_str());
        // 8.

        //{{ 9. Cad Findings data
        stringpairlist findingsAttributes;
        findingsAttributes.push_back(std::pair<std::string,std::string> ("ShowAll" , ToAscii(rdrGlobal.m_bDisplayCadFindings) ) );

        const std::string sVendor(outDataset.m_cadFindings.GetCadVendor());
        if (sVendor.compare(0, sVendor.length(), "Medicsight", 0, sVendor.length()) == 0)
          findingsAttributes.push_back(std::pair<std::string,std::string> ("SliderValue", ToAscii( outDataset.m_cadFindings.m_iCadThresholdValue ) ) );

        pElement = AppendAttributeChild(pXmlDoc, pXmlDoc->documentElement, "CadFindings", findingsAttributes );
        for (std::map< std::string, FindingInfo >::const_iterator it = m_mapCadFindingInfo.begin(); it != m_mapCadFindingInfo.end(); it++)
        {
          const FindingInfo & info = it->second;
          stringpairlist attribs;
          attribs.push_back(std::pair<std::string,std::string> ("GUID" , it->first) );
          attribs.push_back(std::pair<std::string,std::string> ("Visibility" , ToAscii( info.GetDecision() ).c_str()) );

          MSXML2::IXMLDOMElementPtr pFinding = AppendAttributeChild(pXmlDoc, pElement, "Finding", attribs);
        }
        //}} 9. Cad Findings data

        // 10. Status of session lock for editing
        std::string sFlag = "false";
				if( m_bIsReadOnly ) 
					sFlag = "true";
				AppendAttributeChild(pXmlDoc, pXmlDoc->documentElement, ColonSessionXML::sIsReadOnly,
					                   ColonSessionXML::IsReadOnly::sFlag, sFlag.c_str());
        // 10.

      }
      // (end) Add xml data

      // Save the XML to the session file
      const std::string sSessionFile(sFileTitle + vx::ColonSessionXMLExt);
      if (FAILED(pXmlDoc->save(sSessionFile.c_str())))  { throw false; }

      // P.S. XML Data with vx classes
      vxDOMDocument vxdomdoc(vxDOMDocument::FromFile( sSessionFile ));
      {
        //{{ 10. SRT Findings
        {
          vxDOMElement elem = vxdomdoc.CreateElement("SRTFindings");
          outDataset.m_suspiciousRegions.ToXml(elem);
          vxdomdoc.GetDocumentElement().AppendChild(elem);
        } //}} 10. SRT Findings
      } // (end) P.S. vx xml data
      vxdomdoc.Save(sSessionFile);
    }
    catch (...)
    {
      LogDbg("An error occurred while creating colon session xml data/file", "ColonSession", "Write");
      throw;
    }
  }
  catch(...)
  {
    return false;
  }

  return true;
} // WriteXML()


/**
 * Determine if a CAD finding is within (or nearly within) the colon lumen.
 *
 * @param finding  the finding in question
 * @param dataset  the dataset within which to search
 *
 * @return if a CAD finding is within (or nearly within) the colon lumen
 */
bool ColonSession::IsFindingNearLumen( const vxCAD::vxCadFinding & finding, Dataset & dataset )
{
  // get the center coordinates
  const Vector3Df center = finding.GetCentroidMM().GetVector().GetDividedAnIso( dataset.m_fields.m_ins.GetHeader().GetVoxelUnits() );

  return dataset.IsPointNearSelectedLumen(Point3Df(center.m_x, center.m_y, center.m_z));
} // IsFindingNearLumen()


#define RotatePt(pt,worldDim)  pt.m_x = (worldDim.m_x - pt.m_x);pt.m_y = (worldDim.m_y - pt.m_y);
#define RotateNml(nml)  nml.SetXYZ(-1.0f*nml.X(),-1.0f*nml.Y(),nml.Z());

/**
 * Rotates older prone sessions (bookmarks, annotations, measurements) into compliance with 3.0 volume orientation
 * @param worldDim   the world dimensions of the volume
 * return success/failure
 **/
bool ColonSession::RotateProneToSupine( const Triple<float4> & worldDim )
{
  if ( atoi(m_sVersion.c_str()) < 3 )
  {
    for ( BookmarkList::iterator itBk = m_bookmarkList.begin(); itBk != m_bookmarkList.end(); itBk++)
    {
      RotatePt(itBk->m_position,worldDim);
      RotateNml(itBk->m_orientation);
    }

    for ( BookmarkList::iterator itBk = m_2DAnnotationList.begin(); itBk != m_2DAnnotationList.end(); itBk++ )
    {
      RotatePt(itBk->m_position,worldDim);
      RotateNml(itBk->m_orientation);
    }

    for ( MeasureLineList::iterator itMl = m_measureLineList.begin(); itMl != m_measureLineList.end(); itMl++ )
    {
      RotatePt(itMl->m_measureEnds.m_start,worldDim); RotatePt(itMl->m_measureEnds.m_end,worldDim);
    }
  }

  return true;
} // RotateProneToSupine()


// $Log: ColonSession.C,v $
// Revision 1.19.2.8  2009/08/24 22:24:49  gdavidson
// Fixed iterator problem
//
// Revision 1.19.2.7  2009/07/22 15:16:45  dongqing
// upgrade the Session file version to 4.0 after adding the field for the
// status of Always Show CAD Mark
//
// Revision 1.19.2.6  2009/04/07 22:13:28  dongqing
// change the Always Show CAD menu status as per session rather than per Viewer
//
// Revision 1.19.2.5  2008/11/12 00:09:31  jmeade
// Issue 6128: update old prone session data for new volume orientation.
//
// Revision 1.19.2.4  2008/01/18 22:38:28  dongqing
// initial check in the suspicious ROI output for CAD vendor codes
//
// Revision 1.19.2.3  2007/06/26 16:08:34  jmeade
// Issue 5700: set colon version to earlier than 3.0 ONLY for studies THAT HAVE an mrk/amk/vcsn files.
// (Vcsx reads version or sets version to <3.0, no files i.e. empty session retains current SW version).
//
// Revision 1.19.2.2  2007/06/25 21:32:11  jmeade
// Issue 5700: set colon version to earlier than 3.0 for builds with mrk/amk/vcsn files.
//
// Revision 1.19.2.1  2007/06/05 23:36:16  jmeade
// don't output session file or marking volume for studies older than session version 3.0.
//
// Revision 1.19  2007/03/13 02:12:46  jmeade
// code standards.
//
// Revision 1.18  2007/02/16 22:00:17  jmeade
// Issue 5400: CAD: Add checks to ensure vendor-specific code not used by other vendors.
//
// Revision 1.17  2007/02/15 00:08:23  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.16  2007/01/11 02:10:00  jmeade
// Issue 5235: save srt findings to session;
// Issue 5197: don't calculate distance-to-rectum when current location outside of lumen.
//
// Revision 1.15  2006/10/04 20:53:43  frank
// moved the VC settings to a singleton to avoid race conditions during initialization of static variables
//
// Revision 1.14  2006/10/04 13:25:18  dongqing
// fix the session read bug. After reading the UserFeatures, there is no action
// taken if list was empty. Now, a log info added.
//
// Revision 1.13  2006/09/26 19:34:56  frank
// formatting
//
// Revision 1.12  2006/04/12 23:16:51  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.11  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.10  2005/11/15 16:14:58  geconomos
// modified loading process to use new multi-directory structure
//
// Revision 1.9.2.12  2007/02/12 20:48:13  jmeade
// Merge from VC_2-0-2-0-RC50_Branch on 070212
//
// Revision 1.9.2.11  2006/08/17 19:19:54  dongqing
// change the SegmentID == 254 to == 0
//
// Revision 1.9.2.10  2006/08/17 19:04:19  dongqing
// add Segment ID == 254 check
//
// Revision 1.9.2.9  2006/05/09 13:52:50  geconomos
// correct usuage while converting _variant_t to _bstr_t
//
// Revision 1.9.2.8  2006/03/29 14:17:16  frank
// name change for finding info property
//
// Revision 1.9.2.7  2006/03/28 20:48:48  jmeade
// Issue 4642: clearer delineation of cad findings info.
//
// Revision 1.9.2.6  2006/03/27 19:50:02  frank
// fixed finding visibility for out-of-lumen findings
//
// Revision 1.9.2.5  2006/03/27 17:45:49  frank
// hid colon findings outside of 2cm of selected colon
//
// Revision 1.9.2.4  2006/03/22 20:52:26  frank
// moved the finding visibility and other information into a class
//
// Revision 1.9.2.3  2006/01/10 14:14:46  frank
// Added Medicsight default slider value
//
// Revision 1.9.2.2  2006/01/07 04:33:23  frank
// fixed slider functionality and adjusted finding number display
//
// Revision 1.9.2.1  2006/01/05 21:41:19  frank
// Added logic for controlling visibility of cad findings according to sphericity slider threshold
//
// Revision 1.9  2005/10/06 17:50:09  jmeade
// input cad session data.
//
// Revision 1.8  2005/10/06 15:59:41  jmeade
// output cad session data.
//
// Revision 1.7  2005/10/04 18:04:29  jmeade
// display cad findings study-wide global variable.
//
// Revision 1.6  2005/09/28 20:25:13  vxconfig
// renamed study files to dataset
//
// Revision 1.5  2005/09/27 21:41:47  jmeade
// show/hide all findings; hide on 1st open.
//
// Revision 1.4  2005/09/23 12:02:04  vxconfig
// further changes to show cad marks behind folds
//
// Revision 1.3  2005/09/22 16:55:30  jmeade
// convert from volume to world coords.
//
// Revision 1.2  2005/09/21 17:16:55  jmeade
// read, display cad findings list info.
//
// Revision 1.1  2005/09/13 12:59:56  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.3  2005/08/23 16:31:45  geconomos
// Corrected merge from vx repository.
//
// Revision 1.6.2.3.10.1  2005/07/08 00:59:06  jmeade
// catch errors with improperly-formatted measurments xml data.
//
// Revision 1.6.2.3  2004/03/22 21:46:20  geconomos
// Migrated all projects to Visual Studio 2003!
//
// Revision 1.6.2.2  2003/03/07 01:31:24  jmeade
// Comments, removed empty destructor.
//
// Revision 1.6.2.1  2003/03/04 06:22:27  jmeade
// Cleaned up order of session load when no vcs* files present.
//
// Revision 1.6  2003/02/05 16:30:21  jmeade
// XML Colon Session file extension.
//
// Revision 1.5  2003/01/27 21:43:13  jmeade
// XML Session data file.
//
// Revision 1.4  2002/10/31 20:05:18  kevin
// Changed user feature points to make them more robust and sorted
// so that they cannot become invalid (e.g. crossing user features)
//
// Revision 1.3  2002/10/29 19:36:10  kevin
// Normal read/write is not consistent. SO rather than risk breakign
// something else, I put a LARGE warnign in Normal read and
// fixed the Camera class I/O outside of the Normal elements.
//
// Revision 1.2  2002/09/27 19:06:14  jmeade
// Save world location in session; File header and version.
//
// Revision 1.1  2002/09/13 17:30:36  jmeade
// ColonSession class for appropriate data.
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/ColonSession.C,v 1.19.2.8 2009/08/24 22:24:49 gdavidson Exp $
// $Id: ColonSession.C,v 1.19.2.8 2009/08/24 22:24:49 gdavidson Exp $
