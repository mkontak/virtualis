// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmDictionary.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net

#include "Stdafx.h"
#include "DcmDictionary.h"
#include "DcmUtility.h"
#include "DcmLog.h"
#include "DcmElement.h"
#include "DcmGroup.h"


using namespace dcm;

DcmGroupMap DcmDictionary::m_groups;

/**
 * Determines ifthe element Exists
 *
 * @param pElement      Element
 *
 * @return Returns true of the element exists, false otherwise
 */
bool DcmDictionary::Exists(std::shared_ptr<dcm::DcmElement> & pElement)
{ 
  return Exists( pElement->GetDcmTag() ); 
} // Exists(std::shared_ptr<dcm::DcmElement> & pElement)


/**
 * Attempts to extract the DcmDictaEntry from the dictionary
 *
 * @param   iGroup      Group id
 * @param   iElement    Element id
 * @param   pEntry      Entry to set
 *
 * @return True is exists, false otherwise
 */
bool DcmDictionary::TryGetEntry(const DcmTag & tag, DcmDictEntry ** pEntry)
{

  bool bFound(false);

  /// Set to NULL
  *pEntry = __nullptr;

  /// Locks the data dictionary form reading
  const DcmDataDictionary & dict = dcmDataDict.rdlock();

  try
  {
    // Get the entry from the data dictionary
    *pEntry = const_cast<DcmDictEntry *>(dict.findEntry(tag, tag.getPrivateCreator()));

    bFound = ( *pEntry != __nullptr );
  } 
  catch ( ... )
  {
    DcmLog::Error(util::Strings::Format("Failed to get dictionary entry for (%04x,%04x)", tag.getGroup(), tag.getElement()), "DcmUtility", "IsValid");
  }

  /// Unlock the dat dictionary
  dcmDataDict.unlock();

  return bFound;

} // TryGetEntry(unsigned short iGroup, unsigned short iElement, DcmDictEntry * pEntry)


/**
 * Determines existence
 *
 * @param   iGroup      Group id
 * @param   iElement    Element id
 *
 * @return True is exists, false otherwise
 */
bool DcmDictionary::Exists(const DcmTag & tag)
{

  DcmDictEntry * pEntry;

  return TryGetEntry(tag, &pEntry);

} // Exists(unsigned short iGroup, unsigned short iElement)

/**
 * Loads any external distionaries
 */
bool DcmDictionary::LoadExternalDictionary(const std::string & sFilePath)
{
  bool loaded(false);

  DcmDataDictionary & dict = dcmDataDict.wrlock();

  try
  {
    loaded = dict.loadDictionary(sFilePath.c_str());
  }
  catch ( ... ) 
  {
    DcmLog::Error(util::Strings::Format("Failed to load external dictionary %s", sFilePath.c_str()), "DcmUtility", "LoadExternalDictionary");
  }

  /// Unlock the dat dictionary
  dcmDataDict.unlock();

  return loaded;
} // 




/**
 * Adds element to dictaionary
 *
 * @param pElement      Element to add
 */
void DcmDictionary::AddElement(std::shared_ptr<dcm::DcmElement> dcmElement)
{

  /// Locks the data dictaion form reading
  DcmDataDictionary & dict = dcmDataDict.wrlock();

  try
  {

    dict.addEntry(new DcmDictEntry( dcmElement->GetGroup(),                     // Group Id
                                    dcmElement->GetElement(),                   // Element Id
                                    DcmVR(dcmElement->GetVR().c_str()),         // Value Representation
                                    dcmElement->GetName().c_str(),              // Element Name
                                    1,                                        // Minimum value multiplicity
                                    (int)dcmElement->GetVM(),                   // Maximum value multiplicity
                                    NULL,                                     // version (NULL)
                                    OFTrue,                                   // Copy string flags (True - copies all string to local, False  - uses reference )    
                                    dcmElement->GetPrivateCreator().c_str()     // Creator Value
                                   ) );
 
  } 
  catch ( ... )
  {
    DcmLog::Error(util::Strings::Format("Failed to add element to dictionary (%04x,%04x)", dcmElement->GetGroup(), dcmElement->GetElement()), "DcmUtility", "IsValid");
  }

  /// Unlock the dat dictionary
  dcmDataDict.unlock();

}   // AddElement(std::shared_ptr<dcm::DcmElement> pElement)


/**
 * Gets the groups description
 *
 *  @param iGroup     Group description
 *  @return the string
 */
std::string DcmDictionary::GetGroupDescription(unsigned short iGroup)
{

  std::string sDesc("");

  if ( iGroup % 2 == 1 )
   sDesc = "Private Group";
  else
  {
 
    DcmGroupMap groups = GetGroups();
    DcmGroupMap::iterator it(groups.find(iGroup));

    if ( it != groups.end() )
      sDesc = it->second.GetDescription();
    else
      sDesc = util::Strings::Format("Group %04x",iGroup);

  } 

  return sDesc;

} // GetGroupDescription(unsigned short iGroup)


 /**
  * Gets the list of groups
  */
 const DcmGroupMap & DcmDictionary::GetGroups()
 {

  

   if ( m_groups.size() == 0 )
   {
     std::string dicomGroups = util::IO::Combine(util::IO::GetAppDataFolder(),"DicomGroups.xml");

     if ( util::IO::Exists(dicomGroups) )
     {
       xml::XmlDocument doc;

       doc.LoadFile(dicomGroups);

       xml::XmlNodeList nodes = doc.GetDocumentElement().SelectNodes("group");

       for ( int i(0); i < nodes.size(); i++ )
       {

         std::string id("");
         std::string desc("");

         if ( ! nodes[i].TryGetAttributeValue<std::string>("id", id ) )
           throw dcm::DcmConfigurationException(LogRec("Invalid group definition, missing id", "DcmDictionary", "GetGroups"));

        if ( ! nodes[i].TryGetAttributeValue<std::string>("desc", desc ) )
           throw dcm::DcmConfigurationException(LogRec("Invalid group definition, missing description", "DcmDictionary", "GetGroups"));

        
        if ( id.size() != 4 )
           throw dcm::DcmConfigurationException(LogRec("Invalid group definition, id not coreect format", "DcmDictionary", "GetGroups"));

        // ===========================================================
        // If the last 2 characters are XX not XX then just insert a 
        // new group otherwise assume 00-99 so we loop to all all 
        // groups 00-99.
        // ==========================================================
        if ( toupper(id[2]) != 'X' && toupper(id[3]) != 'X' )
          Insert(DcmGroup(atoi(id.c_str()), desc));
        else
        {
          // Reset the last two characters to 00 to start
          id[2] = '0'; id[3] = '0';
        
          // Get the starting and ending group number
          unsigned short grp(atoi(id.c_str()));
          unsigned short end(grp + 0x00FF );

          // We bump value by 2 since odd group numbers are considered private
          for ( ; grp < end; Insert(DcmGroup(grp, desc)), grp+=2);

        }
        
       }
     }
     else
     {

        Insert(DcmGroup(0x0000,"Command Group"));
        Insert(DcmGroup(0x0002,"File Meta Elements"));
        Insert(DcmGroup(0x0004,"Directory Structure Elements"));
        Insert(DcmGroup(0x0008,"General Information"));
        Insert(DcmGroup(0x0010,"Patient Information"));
        Insert(DcmGroup(0x0012,"Clinical Trial Information"));
        Insert(DcmGroup(0x0014,"Component Information"));
        Insert(DcmGroup(0x0018,"Acquisition Information"));
        Insert(DcmGroup(0x0020,"Relationship Information"));
        Insert(DcmGroup(0x0022,"Lighting Information"));
        Insert(DcmGroup(0x0024,"Visual Information"));
        Insert(DcmGroup(0x0028,"Image Presentation Information"));
        Insert(DcmGroup(0x0032,"Study Information"));
        Insert(DcmGroup(0x0038,"Admission Information"));
        Insert(DcmGroup(0x003a,"Waveform Channels"));
        Insert(DcmGroup(0x0040,"Schedule Information"));
        Insert(DcmGroup(0x0042,"Encapsulated Document"));
        Insert(DcmGroup(0x0044,"Product Information"));
        Insert(DcmGroup(0x0046,"Lens Information"));
        Insert(DcmGroup(0x0048,"Imaged Volume"));
        Insert(DcmGroup(0x0050,"Device Information"));
        Insert(DcmGroup(0x0052,"Contrast Information"));
        Insert(DcmGroup(0x0054,"Energy Window"));
        Insert(DcmGroup(0x0060,"Historgram"));
        Insert(DcmGroup(0x0062,"Segmentation"));
        Insert(DcmGroup(0x0064,"Deformable Registration"));
        Insert(DcmGroup(0x0066,"Surface Information"));
        Insert(DcmGroup(0x0068,"Implant Information"));
        Insert(DcmGroup(0x0070,"Graphic Information"));
        Insert(DcmGroup(0x0072,"Hanging Protcol"));
        Insert(DcmGroup(0x0074,"Continutation Information"));
        Insert(DcmGroup(0x0076,"Implant Assembly"));
        Insert(DcmGroup(0x0078,"Implant Template"));
        Insert(DcmGroup(0x0088,"Strogae Media"));
        Insert(DcmGroup(0x0100,"SOP Information"));
        Insert(DcmGroup(0x0400,"Digital Information"));
        Insert(DcmGroup(0x1000,"Triplet"));
        Insert(DcmGroup(0x2000,"Film Session"));
        Insert(DcmGroup(0x2010,"Film Box"));
        Insert(DcmGroup(0x2020,"Image Box"));
        Insert(DcmGroup(0x2030,"Annotation Box"));
        Insert(DcmGroup(0x2040,"Image Overlay Box"));
        Insert(DcmGroup(0x2050,"Presentation LUT"));
        Insert(DcmGroup(0x2100,"Print Job"));
        Insert(DcmGroup(0x2110,"Printer Status"));
        Insert(DcmGroup(0x2120,"Queue Status"));
        Insert(DcmGroup(0x2130,"Printer Management"));
        Insert(DcmGroup(0x2200,"Label"));
        Insert(DcmGroup(0x3002,"RT Image"));
        Insert(DcmGroup(0x3004,"Dose Information"));
        Insert(DcmGroup(0x3006,"Structure Information"));
        Insert(DcmGroup(0x3008,"Treatment Information"));
        Insert(DcmGroup(0x300A,"RT Plan"));
        Insert(DcmGroup(0x300C,"Referenced RT Plan"));
        Insert(DcmGroup(0x300E,"Approval"));
        Insert(DcmGroup(0x4000,"Arbitrary Comments"));
        Insert(DcmGroup(0x4008,"Interpretation"));
        Insert(DcmGroup(0x4010,"Threat"));
        Insert(DcmGroup(0x4FFE,"MAC Parameters"));
        for ( short grp(0x5000); grp < 0x5100; Insert(DcmGroup(grp,"Curve Information")), grp+=2 );
        Insert(DcmGroup(0x5200,"Functional Information"));
        Insert(DcmGroup(0x5400,"Waveform"));
        Insert(DcmGroup(0x5600,"Phase Correction"));
        for ( short grp(0x6000); grp < 0x6100; Insert(DcmGroup(grp,"Overlay")), grp+=2 );
        Insert(DcmGroup(0x7fe0,"Pixel Data"));
        Insert(DcmGroup(0xFFFA,"Digital Signature"));
        Insert(DcmGroup(0xFFFC,"Data Set Trailing Padding"));
        Insert(DcmGroup(0xFFFE,"Item"));
 
     }
   }


   return m_groups;

 }


 /**
  * Inserts the group into the map
  *
  * @param group  Group to be inserted
  */
 void DcmDictionary::Insert(const DcmGroup & group)
 {

   if ( m_groups.find(group.GetId()) != m_groups.end() )
     throw dcm::DcmAlreadyExistsException(LogRec(util::Strings::Format("Group already exists %04d-%s in list", group.GetId(), group.GetDescription().c_str()), "DcmDictionary", "Insert"));

   m_groups.insert(std::pair<short,DcmGroup>(group.GetId(), group));

 }