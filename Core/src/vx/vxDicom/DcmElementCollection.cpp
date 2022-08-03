// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net

#include "Stdafx.h"
#include "DcmElementFactory.h"
#include "DcmElementCollection.h"
#include "DcmExceptions.h"
#include "DcmLog.h"
#include "DcmUtility.h"
#include "DcmSequence.h"
#include "DcmElement.h"
#include "DcmGroup.h"
#include "DcmDictionary.h"
#include "DcmExceptions.h"
#include "DcmDate.h"
#include "DcmTime.h"
#include "DcmIndex.h"

#include <regex>
#include "fcntl.h"



using namespace dcm;


#pragma region constructors

/**
 * Default constructor
 */
DcmElementCollection::DcmElementCollection()  :
ICollection(),
m_indexer(-1),
m_iLevel(0)
{

} // DcmElementCollection()

/**
 * Constructor (Copy)
 */
DcmElementCollection::DcmElementCollection(const DcmElementCollection & elementCollection) :
ICollection(),
m_indexer(-1),
m_iLevel(elementCollection.m_iLevel)
{
 
} // DcmElementCollection(const DcmElementCollection & dcmElementList)



#pragma endregion

#pragma region Create Methods

/**
 * Creates the element list from the header
 *
 * @param sHeader     Header
 *
 * @return the element list
 */
std::shared_ptr<dcm::DcmElementCollection> DcmElementCollection::Create(const std::string & sParameter, const DcmParameterType & type)
{
  std::shared_ptr<DcmElementCollection> list(new DcmElementCollection());

  if ( list == __nullptr )
    throw DcmOutOfMemoryException(LogRec("Failed to allocate DcmElementCollection object","DcmElementCollection","Create"));


 
  if ( type == PT_FILE )
  {
    // XML string
    std::string sXml("");

    // Strip off control characters
    std::regex rx("[\\x00-\\x1f\\x7f]");
    std::string sReplace("");


    if (util::IO::SplitPath(sParameter)[util::IO::EXTENSION] == ".xml")
    {

      xml::XmlDocument doc;
      doc.LoadFile(sParameter);

      

      // XML string
      sXml.assign(doc.ToXml());

    }
    else
    {

      DcmFileFormat fileFormat;

      // REad the file
      OFCondition cond = fileFormat.loadFile(sParameter.c_str());

      // =================================================================================
      // Check the return condition. If it failed then we will examine the file for a PDF
      // =================================================================================
      if ( cond.bad() )
        throw DcmInvalidDicomFile(LogRec(util::Strings::Format("Failed to load file %s : %s", sParameter.c_str(), cond.text()), "DcmElementCollection", "Create"));

      // stream for the header
      std::ostringstream os;

      // Get the header XML
      fileFormat.getDataset()->writeXML(os);

      // XML string
      sXml.assign(os.str());



    

    }

    // return the element collection
    list->LoadHeader(std::regex_replace(sXml, rx, sReplace));


  } // END ... Else we have what appears to be a file
  else 
    list->LoadHeader(sParameter);
 
  return list;

} // Create(const std::string & sHeader)


/**
 * Creates the element list from the header
 *
 * @param sHeader     Header
 *
 * @return the element list
 */
std::shared_ptr<dcm::DcmElementCollection> DcmElementCollection::Create(const xml::XmlNode & node)
{
  std::shared_ptr<DcmElementCollection> list(new DcmElementCollection());

  if ( list == __nullptr )
    throw DcmOutOfMemoryException(LogRec("Failed to allocate DcmElementCollection object","DcmElementCollection","Create"));

  list->Initialize(const_cast<xml::XmlNode &>(node));


  return list;

} // Create(xmlNodePtr pNode)


/**
 * Creates an empty list
 *
 * @return the element list
 */
std::shared_ptr<dcm::DcmElementCollection> DcmElementCollection::Create()
{
  std::shared_ptr<DcmElementCollection> list(new DcmElementCollection());

  if ( list == __nullptr )
    throw DcmOutOfMemoryException(LogRec("Failed to allocate DcmElementCollection object","DcmElementCollection","Create"));

 
  return list;

} // Create()

#pragma endregion




/**
 * Clones the list
 */
std::shared_ptr<DcmElementCollection> DcmElementCollection::Clone()
{

  // Create an empty element list
  std::shared_ptr<DcmElementCollection> pDcmElementCollection(Create());
  
  CopyTo(pDcmElementCollection, true);
 
  return pDcmElementCollection;

} // Clone()


/**
 * Determines if the collections are equal. Since we work with share pointers only the memory 
 * is compared not the content.
 *
 * @param collection      Collection to compare to
 *
 * @returns True is the collections are equal, false otherwise
 */
bool DcmElementCollection::Equals(const std::shared_ptr<DcmElementCollection> & collection)
{
  return ( collection.get() == this );
} //  Equals(const std::shared_ptr<DcmElementCollection> & collection)

/**
 * Copies all elements from this list to the other list
 *
 * @param pDcmElementCollection       List to copy to 
 */
void DcmElementCollection::CopyTo(std::shared_ptr<DcmElementCollection> & pDcmElementCollection, bool clone)
{

  /// Clears the list
  pDcmElementCollection->Clear();

  // ===========================================
  // If clone is true then cloine each element
  // ===========================================
  if ( clone )
  {
    std::for_each(m_elements.begin(), m_elements.end(), [&pDcmElementCollection] ( std::pair<int, std::shared_ptr<dcm::DcmElement>> item )
    {
      pDcmElementCollection->Add(item.second->Clone());
    } );
  }
  else
  {
    std::for_each(m_elements.begin(), m_elements.end(), [&pDcmElementCollection] ( std::pair<int, std::shared_ptr<dcm::DcmElement>> item )
    {
      pDcmElementCollection->Add(item.second);
    } );
 
  }


} // CopyTo(std::shared_ptr<DcmElementCollection> & pDcmElementCollection)



/**
 * Removes the specified element from the list
 */
bool DcmElementCollection::Remove(const DcmTag & tag)
{

  bool bRemoved(false);

  DcmElementMapIterator it(m_elements.find(tag.hash()));
  if ( it != m_elements.end() )
    m_elements.erase(it);

  return bRemoved;

} // Remove(std::shared_ptr<DcmElement> DcmElement)

/**
 * Removes all elements in the group
 *
 * @param iGroup      Group to be removed
 */
void DcmElementCollection::Remove(const unsigned short & iGroup)
{

  // Get the upper and lower bound for the group
  DcmElementMapIterator lower = m_elements.lower_bound(DcmUtility::CreateDcmTag(iGroup, 0x0000).hash());
  DcmElementMapIterator upper = m_elements.upper_bound(DcmUtility::CreateDcmTag(iGroup, 0xffff).hash());

  // Erase all elements 
  m_elements.erase(lower, upper);

}  // Removes(const unsigned short & iGroup)


/**
 * Gets the element list for a multi-frame dataset
 */
DcmElementCollectionList DcmElementCollection::GetFrames()
{

  DcmElementCollectionList frames;


  std::shared_ptr<dcm::DcmElement> pDcmElement;
  unsigned short iNumberOfFrames(0);
 


  // ==============================================================================
  // Get the (5200,9230) PerFrameFunctionalGroupsSequence this is enhanced dicom
  // ==============================================================================
  if ( TryGetElement(DCM_PerFrameFunctionalGroupsSequence, pDcmElement) )
  {

    std::shared_ptr<dcm::DcmSequence> perFrameFunctaionGroupSequence(std::dynamic_pointer_cast<dcm::DcmSequence>(pDcmElement));

     
    perFrameFunctaionGroupSequence->Reset();


    // =====================
    // Get all the frames
    // ====================
    while ( perFrameFunctaionGroupSequence->MoveNext() )
    {
      frames.push_back(perFrameFunctaionGroupSequence->Current());          
    } // END ... 


  }

 
  return frames;
} // GerFrames()




/**
 * Copies the supplied sequence item into the supplied element list
 *
 * @param collection        Collection to insert
 * @param overwrite         Overwrite flag indicating that any elements that wlaredy exist will
 *                          be overridden
 */
void DcmElementCollection::Insert(std::shared_ptr<DcmElementCollection> & collection, bool overwrite)
{

  collection->Reset();

  // =====================================================================================
  // Loop through all the sub-elements of the sequence item inserting each one into the 
  // supplied element list
  // ====================================================================================
  while ( collection->MoveNext() )
  {
  
    std::shared_ptr<dcm::DcmElement> element(collection->Current());
  
    // ==========================================================
    // If we are overwriting the data then remove the old element
    // ==========================================================
    if ( overwrite )
      Remove(element);

    // Add then new
    Add(element->Clone());

  } // END ... for each element 

} // Insert(std::shared_ptr<DcmElementCollection> & collection, bool overwrite)


/**
 * Generates the XML for the element
 */
std::string DcmElementCollection::ToXml()
{


  std::string sXml( m_iLevel > 0 ? util::Strings::Format("<item card=%d>", Count()) : "<data-set>" );

  Reset();

  while ( MoveNext() )
  {
    sXml.append(Current()->ToXml());
  }
  
  if ( m_iLevel > 0 )
    sXml.append(util::Strings::Format("</item>", Count()));
  else
    sXml.append("</data-set>");

  return sXml;
 
  
}// ToXml()


/**
 * Inserts or updates the specified element in the list
 *
 * @param dcmElement      DCMElement
 */
void DcmElementCollection::Add(std::shared_ptr<dcm::DcmElement> & element)
{

  DcmElementMapIterator it( m_elements.find(element->GetId()) );
  if (  it  == m_elements.end() )
  {
    element->m_bIsSubElement = false;
    element->SetLevel(m_iLevel);
    m_elements.insert(DcmElementMapItem(element->GetId(), element));
    m_iterator = m_elements.begin();
  }

} // Insert(std::shared_ptr<DcmElement> pDcmElement)



/**
 * Creates the list of elements from the header
 *
 * @param sHeader      Header string
 */
void DcmElementCollection::LoadHeader(const std::string & sHeader)
{

  try
  {

    // ===============================================
    // If the first character is a '<' assume xml
    // ==============================================
    if ( sHeader[0] == '<' )
    {


      xml::XmlDocument doc;

      doc.LoadXml(sHeader);

      xml::XmlNode node(doc.GetDocumentElement());

      if ( node.GetName() == "patient"  )
        LoadPatientNode(node);
      else if ( node.GetName() == "study" )
        LoadStudyNode(node);
      else if ( node.GetName() == "series" )
        LoadSeriesNode(node);
      else if ( node.GetName() == "data-set" || node.GetName() == "dataset" )
        Initialize(node);
      else
        throw DcmInvalidDataException(LogRec(util::Strings::Format("Invalid xml received, cannot create elements from node %s", 
                                      const_cast<xml::XmlNode &>(node).GetName().c_str()), "DcmElementCollection", "LoadXml"));     

    }

    // ====================================
    // Else we assume a serialized string
    // ===================================
    else
    {

      // Get the fields which should be in pairs of "tag value tag value" formats
      std::vector<std::string> fields = util::Strings::Split(sHeader, ' ');

      // The serialized element is a pair or "tag value" so the fields should be event
      if ( fields.size() % 2 == 1 )
        throw DcmInvalidDataException(LogRec("Invalid serialized string", "DcmImage", "Create"));

      // ==============================
      // Loop through all the elements
      // ===============================
      for ( register int i(0), j(1); j < fields.size(); i+=2, j+=2 )
      {
        Add(dcm::DcmElement::Create(fields[i], fields[j] ));
      } // END ... For each element

    } // END ... Else

  }
  catch ( DcmException & ex )
  {

    DcmLog::Error(util::Strings::Format("Failed to load header : %s", ex.GetTextMessage()), "DcmElementCollection", "LoadHeader");

    throw;

  }
  catch ( ... )
  {

    throw DcmException(LogRec("Failed to load header : unspecified error","DcmElementCollection","LoadHeader"));

  }


} // LoadHeader(const std::string sHeader)


/**
 * Increments to the next element in the list 
 *
 * @return DcmElement
 */
bool DcmElementCollection::MoveNext()
{
  bool bMoved(false);

  if ( ! m_elements.empty() && ++m_indexer < m_elements.size()  )
  {
    if ( m_indexer == 0 )
      m_iterator = m_elements.begin();
    else
      m_iterator++;

    bMoved = true;
  }
  
  return bMoved;
} // MoveNext()


/**
 * Gets the next element from the list or null if we are at the end
 *
 * @return DcmElement
 */
std::shared_ptr<dcm::DcmElement> DcmElementCollection::Current()
{
  std::shared_ptr<dcm::DcmElement> pDcmElement(__nullptr);

  if ( ! m_elements.empty() && m_indexer > -1 && m_iterator != m_elements.cend() )
    pDcmElement = (*m_iterator).second;

  return pDcmElement;
} // Next()

/**
 * Try and get the element from the list.
 *  
 * @param tag     Tag to get
 *
 * @return True if found, false if not
 */
bool DcmElementCollection::TryGetElement(const DcmTag & sequenceTag, const DcmTag & elementTag, std::shared_ptr<dcm::DcmElement> & element, bool searchSubElements)
{
  bool bFound(false);

  element = __nullptr;

  std::shared_ptr<DcmSequence> searchSequence(__nullptr);

  if ( sequenceTag.hash() != 0 )
  {
    DcmElementMapIterator it(m_elements.find(sequenceTag.hash()));

  // ==========================================================================================================================
    // If we found the item on the first level then we are done. Otherwise if the searchSubElements is true then search through
    // all sequences and items until you find the first match.
    // ==========================================================================================================================
    if ( it != m_elements.end() )
    {
      searchSequence = std::dynamic_pointer_cast<dcm::DcmSequence>((*it).second);
    }
    else if ( searchSubElements ) 
    {

      std::shared_ptr<DcmElement> sequnceElement;

      // =========================================================================================================================
      // Look through all the elements and if we have a sequence the search through its items to search their corresponding lists
      // =========================================================================================================================
      std::for_each(m_elements.begin(), m_elements.end(), [&] (DcmElementMapItem elementItem )
      {

        // ==========================================
        // We only care about sequence elements
        // ==========================================
        if ( elementItem.second->IsSequence() )
        {
          // Cast to a DcmSequence
          std::shared_ptr<dcm::DcmSequence> sequence(std::dynamic_pointer_cast<dcm::DcmSequence>(elementItem.second));

          sequence->Reset();

          // =========================================================================================================
          // Search through alll items and look for the specfied element. The first match will be retruned so any
          // additional elements that have the same tag eill not be returned.
          // ==========================================================================================================
          while ( sequence->MoveNext() )
          {

            std::shared_ptr<dcm::DcmElementCollection> collection(sequence->Current());

            // If the sequnce is found in the item set the found flag to true which breaks out of the loop.
            if ( collection->TryGetElement(sequenceTag, sequnceElement, searchSubElements) ) 
            {
              searchSequence = std::dynamic_pointer_cast<DcmSequence>(sequnceElement);
              break;
            }

          } // END ... For each sequence item


        }
      } );  

    } // END ... Else if we want to search sub-elements


    /// If the sequnce specified was not found then return false
    if ( searchSequence == __nullptr )
      return false;

  }


  if ( searchSequence == __nullptr )
  {
    DcmElementMapIterator it(m_elements.find(elementTag.hash()));


    // ==========================================================================================================================
    // If we found the item on the first level then we are done. Otherwise if the searchSubElements is true then search through
    // all sequences and items until you find the first match.
    // ==========================================================================================================================
    if ( it != m_elements.end() )
    {
      element = (*it).second;
      bFound = true;
    }
    else if ( searchSubElements ) 
    {

      // =========================================================================================================================
      // Look through all the elements and if we have a sequence the search through its items to search their corresponding lists
      // =========================================================================================================================
      std::for_each(m_elements.begin(), m_elements.end(), [&] (DcmElementMapItem elementItem )
      {

        // ==========================================
        // We only care about sequence elements
        // ==========================================
        if ( elementItem.second->IsSequence() )
        {
          // Cast to a DcmSequence
          std::shared_ptr<dcm::DcmSequence> sequence(std::dynamic_pointer_cast<dcm::DcmSequence>(elementItem.second));

          sequence->Reset();

          // =========================================================================================================
          // Search through alll items and look for the specfied element. The first match will be retruned so any
          // additional elements that have the same tag eill not be returned.
          // ==========================================================================================================
          while ( sequence->MoveNext() && !bFound )
          {

            std::shared_ptr<dcm::DcmElementCollection> collection(sequence->Current());

            // If the element is found in the item set the found flag to true which breaks out of the loop.
            if ( collection->TryGetElement(elementTag, element, searchSubElements) ) 
              bFound = true;

          } // END ... For each sequence item


        }
      } );  

    } // END ... Else if we want to search sub-elements
  }
  else
  {
      searchSequence->Reset();

      // =========================================================================================================
      // Search through alll items and look for the specfied element. The first match will be retruned so any
      // additional elements that have the same tag eill not be returned.
      // ==========================================================================================================
      while ( searchSequence->MoveNext() && !bFound )
      {

        std::shared_ptr<dcm::DcmElementCollection> collection(searchSequence->Current());

        // If the element is found in the item set the found flag to true which breaks out of the loop.
        if ( collection->TryGetElement(elementTag, element, searchSubElements) ) 
          bFound = true;

      } // END ... For each sequence item



  }

  return bFound;

} // TryGetElement(const DcmTag & tag, std::shared_ptr<dcm::DcmElement> & pDcmElement)


/**
 * Try and get the element from the list.
 *  
 * @param tag     Tag to get
 *
 * @return True if found, false if not
 */
bool DcmElementCollection::Contains(const DcmTag & tag)
{

  DcmElementMapIterator it(m_elements.find(tag.hash()));

  return ( it != m_elements.end() );

} // Contains(const DcmTag & tag)


/**
 * Returns a collection of elements for the secified group
 *
 * @param iGroup      Group id
 */
std::shared_ptr<dcm::DcmElementCollection> DcmElementCollection::GetElements(unsigned short iGroup)
{
  std::shared_ptr<dcm::DcmElementCollection> collection = DcmElementCollection::Create();

  std::for_each(m_elements.begin(), m_elements.end(), [&collection, &iGroup] ( DcmElementMapItem elementItem ) 
  {
    if ( elementItem.second->GetGroup() == iGroup )
      collection->Add(elementItem.second);
  } );

  return collection;

} // GetElementCollection(unsigned short iGroup)

/**
 * Gets the list of groups
 */
DcmGroupMap DcmElementCollection::GetGroups()
{
  DcmGroupMap groups;

  std::for_each(m_elements.begin(), m_elements.end(), [&groups] ( DcmElementMapItem item )
  {
    unsigned short id(item.second->GetGroup());

     if ( groups.find(id) == groups.end() )
       groups.insert(std::pair<unsigned short,dcm::DcmGroup>(id, DcmGroup(id)));
  } );

  return groups;

} // GetGroups()


/**
 * Sets the level of the collection and all its elements
 *
 * @param iLevel     Level
 */
void dcm::DcmElementCollection::SetLevel(unsigned short iLevel)
{
  m_iLevel = iLevel;

  std::for_each( m_elements.begin(), m_elements.end(), [&iLevel] ( DcmElementMapItem item )
  {
    item.second->SetLevel(iLevel);
  } );
} // SetLevel(iLevel)


/**
 * Writes the file parth
 *
 * @param sFilePath     File path
 */
void dcm::DcmElementCollection::WriteXml(const std::string & sFilePath)
{

  /// Open the file path
  int fd = _open(sFilePath.c_str(),  _O_CREAT | _O_WRONLY,  _S_IWRITE );


  if ( fd == -1 )
    throw dcm::DcmIOException(LogRec(util::Strings::Format("Failed to open file %s (%d)", sFilePath.c_str(), errno), "DcmElementCollection","WriteXml"));

  SCOPE_EXIT()
  {
    _close(fd);
  };

  std::string sXml(ToXml());

  int rc = _write(fd, (void *)sXml.c_str(), sXml.size());

  if ( rc == -1 )
    throw dcm::DcmIOException(LogRec(util::Strings::Format("Failed to wite element list to %s (%d)", sFilePath.c_str(), errno), "DcmElementCollection","WriteXml"));


 
} // WriteXml()


/**
 * Copies the element collection to the DcmItem supplied
 *
 * @param pDcmItem    DcmItem
 */
void DcmElementCollection::CopyTo(DcmItem * pDcmItem)
{
  if ( pDcmItem != __nullptr )
  {

    Reset();


    while ( MoveNext() )
    {
      Current()->CopyTo(pDcmItem);
    }

  }
} // CopyTo(DcmItem * pDcmItem)



#pragma region Xml Loading Methods


/**
 * Loads the data from the patient node. 
 *
 * Used in the Query
 *
 * Example:
 *    <patient mid="564681141" name="Lession, AppleCore" sex="F" pstc="1" psec="1" />
 *
 * @param node    Patient node or node that contains patient data
 */
void dcm::DcmElementCollection::LoadPatientNode(const xml::XmlNode & node)
{

  std::string value; 

  /// Medical Id
  value.clear();
  if ( const_cast<xml::XmlNode &>(node).TryGetAttributeValue<std::string>( "mid", value) )
    Add(DcmElement::Create<std::string>(DCM_PatientID, value));

  /// Patient Name
  value.clear();
  if ( const_cast<xml::XmlNode &>(node).TryGetAttributeValue<std::string>( "name", value) )
    Add(DcmElement::Create<std::string>(DCM_PatientName, value));

  /// Patient DOB
  value.clear();
  if ( const_cast<xml::XmlNode &>(node).TryGetAttributeValue<std::string>( "dob", value) )
    Add(DcmElement::Create<dcm::DcmDate>(DCM_PatientBirthDate,dcm::DcmDate(value)));

  /// Patient TOB
  value.clear();
  if ( const_cast<xml::XmlNode &>(node).TryGetAttributeValue<std::string>( "tob", value) )
    Add(DcmElement::Create<dcm::DcmTime>(DCM_PatientBirthTime,dcm::DcmTime(value)));

  /// Patient Sex
  value.clear();
  if ( const_cast<xml::XmlNode &>(node).TryGetAttributeValue<std::string>( "sex", value) )
    Add(DcmElement::Create<std::string>(DCM_PatientSex,value));

  /// Patient Study Count
  int count(0);
  if ( const_cast<xml::XmlNode &>(node).TryGetAttributeValue<int>( "pstc", count) )
    Add(DcmElement::Create<int>( DcmTag(0x0020, 0x1200), count ) );

  /// Patient Series Count
  count = 0;
  if ( const_cast<xml::XmlNode &>(node).TryGetAttributeValue<int>( "psec", count) )
   Add(DcmElement::Create<int>( DcmTag(0x0020, 0x1202), count) );

} // LoadPatientNode(const xml::XmlNode & node)


/**
 * Loads the data from the study node.
 *
 * Example:
 *    <study stuid="1.2.3.4.19203938.345.32.1" std="01/02/2011" sed="08:10:20" phy="Duck^Donald" an="12293821" 
 *           stdsc="Study Description" mid="564681141" stsc="1" stic="400" 
 *           name="Lession, AppleCore" sex="F" pstc="1" psec="1" />
 *
 * @param node    Study node or node that contains patient data
 */
void dcm::DcmElementCollection::LoadStudyNode(const xml::XmlNode & node)
{

  /// Load any patient information from the study node
  LoadPatientNode(node);

  std::string value; 

  /// Study Uid
  value.clear();
  if ( const_cast<xml::XmlNode &>(node).TryGetAttributeValue<std::string>("stuid", value) )
    Add(DcmElement::Create<std::string>(DCM_StudyInstanceUID, value));

  /// Study Accession Number Name
  value.clear();
  if ( const_cast<xml::XmlNode &>(node).TryGetAttributeValue<std::string>("an", value) )
    Add(DcmElement::Create<std::string>(DCM_AccessionNumber, value));
  
  /// Study Description
  value.clear();
  if ( const_cast<xml::XmlNode &>(node).TryGetAttributeValue<std::string>("stdesc", value) )
    Add(DcmElement::Create<std::string>(DCM_StudyDescription, value));
  /// Study Accession Number Name
  value.clear();
  if ( const_cast<xml::XmlNode &>(node).TryGetAttributeValue<std::string>( "phys", value) )
    Add(DcmElement::Create<std::string>(DCM_ReferringPhysicianName, value));

  /// Study Date
  value.clear();
  if ( const_cast<xml::XmlNode &>(node).TryGetAttributeValue<std::string>("std", value) )
    Add(DcmElement::Create<dcm::DcmDate>(DCM_StudyDate,dcm::DcmDate(value)));


  /// Study Time
  value.clear();
  if ( const_cast<xml::XmlNode &>(node).TryGetAttributeValue<std::string>( "stt", value) )
    Add(DcmElement::Create<dcm::DcmTime>(DCM_StudyTime, dcm::DcmTime(value)));

  /// Study Series Count
  int count(0);
  if ( const_cast<xml::XmlNode &>(node).TryGetAttributeValue<int>( "stsc", count) )
    Add(DcmElement::Create<int>( DcmTag(0x0020, 0x1200), count));

  /// Study Image Count
  count = 0;
  if ( const_cast<xml::XmlNode &>(node).TryGetAttributeValue<int>( "stic", count) )
    Add(DcmElement::Create<int>( DcmTag(0x0020, 0x1202), count));

} // LoadStudyNode(const xml::XmlNode & node)


/**
 * Loads the data from the Series node.
 *
 * Used in the Query (C-FIND)
 *
 * Example:
 *    <series seuid="1,2,3,4,5678,9876,09,1" sed="01/01/2011" set="08:08:12" seic="400" sedsc="Series Description" 
 *            stuid="1.2.3.4.19203938.345.32.1" std="01/02/2011" sed="08:10:20" phy="Duck^Donald" 
 *            an="12293821" mid="564681141" stsc="1" stic="400" name="Lession, AppleCore" sex="F" pstc="1" psec="1" />
 *
 * @param node    Series node or node that contains series data
 */
void dcm::DcmElementCollection::LoadSeriesNode(const xml::XmlNode & node)
{

  /// Load any patient information from the study node
  LoadStudyNode(node);

  std::string value; 

  /// Series Uid
  value.clear();
  if ( const_cast<xml::XmlNode &>(node).TryGetAttributeValue<std::string>( "seuid", value) )
    Add(DcmElement::Create<std::string>(DCM_SeriesInstanceUID, value));

  /// Series Description
  value.clear();
  if ( const_cast<xml::XmlNode &>(node).TryGetAttributeValue<std::string>( "sedesc", value) )
    Add(DcmElement::Create<std::string>(DCM_SeriesDescription, value));

  /// Series Date
  value.clear();
  if ( const_cast<xml::XmlNode &>(node).TryGetAttributeValue<std::string>( "sed", value) )
   Add(DcmElement::Create<dcm::DcmDate>(DCM_SeriesDate,dcm::DcmDate(value)));

  /// Series Time
  value.clear();
  if ( const_cast<xml::XmlNode &>(node).TryGetAttributeValue<std::string>( "set", value) )
   Add(DcmElement::Create<dcm::DcmTime>(DCM_SeriesTime,dcm::DcmTime(value)));

  /// Series Image Count
  int count = 0;
  if ( const_cast<xml::XmlNode &>(node).TryGetAttributeValue<int>( "seic", count) )
    Add(DcmElement::Create<int>( DcmTag(0x0020, 0x1202), count));

} // LoadSeriesNode(xml::XmlNode & node)



/**
 * Leads the header the element list
 *
 * @param pNode       Node
 */
void DcmElementCollection::Initialize(const xml::XmlNode & node)
{
  
  std::shared_ptr<dcm::DcmElement> privateCreator(__nullptr);

  /// Get the element nodes
  std::vector<xml::XmlNode> elements = const_cast<xml::XmlNode &>(node).SelectNodes("\bsequence|element\b");
 
  if ( const_cast<xml::XmlNode &>(node).HasChildren() == 0 )
    DcmLog::Warning("No elements specified in the xml","DcmElementCollection","LoadHeader");


  // ==============================
  // Loop through all the elements
  // =============================
  for (  xml::XmlNode element = const_cast<xml::XmlNode &>(node).GetFirstChild(); element.IsValid() ; element = const_cast<xml::XmlNode &>(node).GetNextChild() )
  {

    //xml::XmlNode element(*it);

    std::string sTagName(element.GetName());
    std::shared_ptr<dcm::DcmElement> dcmElement(DcmElementFactory::Create(element));

    if ( dcmElement == __nullptr )  
      continue;

    //std::shared_ptr<dcm::DcmElement> dcmElement( element.GetName() == "sequence" ? DcmSequence::Create(element) : DcmElement::Create(element) );
 
    // ========================================================================================
    // If private and the group is not 0 (length) then set the private creator for the element
    // =======================================================================================
    if ( dcmElement->IsPrivate() && dcmElement->GetGroup() == 0 )
    {

      if ( dcmElement->IsPrivateCreator() )
        privateCreator = dcmElement;

      dcmElement->SetPrivateCreator(privateCreator->GetValue<std::string>());
  
    }
    else
      privateCreator = __nullptr;
        

    Add(dcmElement);

  }


} // Initialize(xmlNodePtr pNode)


#pragma endregion


/**
 * Gets the old style serialized header for the volume
 */
std::string  DcmElementCollection::GetSerializedHeader()
{

  std::string sSerializedHeader("");

  // Make sure there are elements found
  if ( m_elements.size() == 0 )
    DcmLog::Warning("No elements specified in the xml","DcmImage","Update");

  // ===================================================================
  // Iterate through the elements and convert to the serialized format
  // ===================================================================
  for ( DcmElementMap::iterator it = m_elements.begin(); it != m_elements.end(); it++ )
  {
    // Build the serialized header string 
    sSerializedHeader.append((*it).second->ToSerializedString());
    sSerializedHeader.append(" ");
  }  // END ... For each node
 
  // earse the last space
  return sSerializedHeader.erase(sSerializedHeader.size() - 1, 1);

} // GetSerializedHeader()