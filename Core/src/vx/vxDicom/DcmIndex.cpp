#include "stdafx.h"
#include "DcmIndex.h"
#include "DcmExceptions.h"
#include "DcmDictionary.h"




using namespace dcm;



/**
 * Constructor using the paremeters supplied
 *
 * @param sUid      Associated index uid
 * @param group     Functaion group sequnce tag
 * @param index     Element tag
 *
 * @return DcmIndex
 */
DcmIndex::DcmIndex(const std::string & sUid, const  DcmTag & functionalGroup, const DcmTag & indexElement):
m_sUid(sUid),
m_functionalGroup(functionalGroup),
m_indexElement(indexElement),
m_sIndexElementName(""),
m_sFuntcionalGroupName("")
{

  DcmDictEntry * pEntry;

  if ( DcmDictionary::TryGetEntry(m_indexElement, &pEntry ) )
    m_sIndexElementName.assign(pEntry->getTagName());


  if ( m_functionalGroup.hash() != 0 && DcmDictionary::TryGetEntry(m_functionalGroup, &pEntry ) )
  {
    if ( pEntry->getEVR() != DcmEVR::EVR_SQ )
      throw DcmInvalidDataException(LogRec("Indexes functional group must be a sequence","DcmIndex","DcmIndex"));
    
    m_sFuntcionalGroupName.assign(pEntry->getTagName());
  }

}

/**
 * Creates the DcmIndex using the paremeters supplied
 *
 * @param sUid      Associated index uid
 * @param group     Functaion group sequnce tag
 * @param index     Element tag
 *
 * @return DcmIndex
 */
std::shared_ptr<DcmIndex> DcmIndex::Create(const std::string & sUid, const  DcmTag & groupTag, const DcmTag & indexTag)
{

   std::shared_ptr<DcmIndex> dcmIndex( std::shared_ptr<DcmIndex>(new DcmIndex(sUid, groupTag, indexTag)) );

    if ( dcmIndex == __nullptr )
      throw DcmOutOfMemoryException(LogRec("Failed to allocate DcmIndex", "DcmIndex", "Create"));

    return dcmIndex;
}



/**
 * Creates the DcmIndex using the paremeters supplied
 *
 * @param sUid      Associated index uid
 * @param group     Functaion group sequnce tag
 * @param index     Element tag
 *
 * @return DcmIndex
 */
std::shared_ptr<DcmIndex> DcmIndex::Create( const DcmTag & index )
{
   return Create("", DcmTag(0,0), index);
}



/**
 * Returns true if the index is empty, false if if contains information
 */
bool DcmIndex::IsEmpty()
{ 
  return ( m_indexElement.hash() == 0 ); 
} // IsEmpty()
