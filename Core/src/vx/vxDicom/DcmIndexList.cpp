#include "Stdafx.h"
#include "DcmIndexList.h"
#include "DcmIndex.h"
#include "DcmExceptions.h"
#include "DcmDictionary.h"
#include "DcmElementCollection.h"
#include "DcmElement.h"


using namespace dcm;


/**
 * Default constructor
 */
DcmIndexList::DcmIndexList() :
m_tag(DcmTag(0,0)),
m_sName(""),
m_multiValuedIndex(DcmIndex::Create(DcmTag(0,0)))
{
}



/**
 * Creates the index list based on the name
 *
 * @param sName       Name
 *
 * @return DcmIndexList
 */
std::shared_ptr<DcmIndexList> DcmIndexList::Create()
{
  std::shared_ptr<DcmIndexList> list( new DcmIndexList() );

  if ( list == __nullptr )
    throw DcmOutOfMemoryException(LogRec("Faild to allocate DcmIndexList","DcmIndexList","Create"));


  return list;
} // Create(const std::string & sName)

/**
 * Creates the index list based on the name
 *
 * @param sName       Name
 *
 * @return DcmIndexList
 */
std::shared_ptr<DcmIndexList> DcmIndexList::Create(const std::string & sName)
{
  std::shared_ptr<DcmIndexList> list( Create() );

  list->m_sName = sName;

  return list;
} // Create(const std::string & sName)

/**
 * Creates the index list based on the tag
 *
 * @param tag       Dicom tag
 *
 * @return DcmIndexList
 */
std::shared_ptr<DcmIndexList> DcmIndexList::Create(const DcmTag & tag)
{
  std::shared_ptr<DcmIndexList> list( Create() );

  list->m_tag = tag;

  DcmDictEntry * pEntry;

  if ( DcmDictionary::TryGetEntry(tag, &pEntry ) )
    list->m_sName.assign(pEntry->getTagName());

  // ======================================================================================
  // If this is the DCM_DimensionIndexSequence then we only need to look for a single
  // entry of   (0020,9157) DimensionIndexValues which is a multi-valued element of 
  // unsigned longs. Founf that string values are mapped to integers by the planar device 
  // constructing the multi-phase images
  // ======================================================================================
  if ( tag == DCM_DimensionIndexSequence )
    list->m_multiValuedIndex = DcmIndex::Create("", DCM_FrameContentSequence, DCM_DimensionIndexValues);

  return list;
} // Create(const DcmTag & tag)

/**
 * Determines if the index list references a multi-valued index item
 */
bool DcmIndexList::HasMultiValuedIndex()
{
  return ( ! m_multiValuedIndex->IsEmpty() ); 
}

/**
 * Adds the index to the list
 *
 * @param index       INdex to be added
 */
void DcmIndexList::Add(std::shared_ptr<DcmIndex> & index)
{
  if ( Contains(index) )
    throw DcmAlreadyExistsException(LogRec("Index already exists in list","DcmINdexList","Add"));


  m_indexes.push_back(index);
} // Add(std::shared_ptr<DcmIndex> & index)

/**
 * Adds all elements as indexes.
 */
void DcmIndexList::Add(std::shared_ptr<DcmElementCollection> & elements)
{

  elements->Reset();

  while ( elements->MoveNext() )
  {
    Add(DcmIndex::Create(elements->Current()->GetDcmTag()));
  }

} // Add(std::shared_ptr<DcmElementCollection> & collection)

/**
 * Adds all elements as indexes.
 */
inline void DcmIndexList::Add(std::shared_ptr<dcm::DcmElement> & element)
{
  Add(DcmIndex::Create(element->GetDcmTag()));
} // Add(std::shared_ptr<DcmElementCollection> & collection)


/**
 * Determines if the specified index is already in the list
 *
 * @param index       INdex to check
 *
 * @return Returns true if the index is contained in the list, false if not
 */
bool DcmIndexList::Contains(const std::shared_ptr<DcmIndex> & index)
{

  bool bContains(false);

  // =====================================================
  // Loop through all indexes to ccheck for duplicate
  // ====================================================
  for ( int i(0); i < m_indexes.size() && ! bContains ; i++ )
  {
    if ( *m_indexes[i] == *index )
      bContains = true;
  } // END ... For each index

  return bContains;
} // Contains(std::shared_ptr<DcmIndex> & index)


/**
 * Returns the index at the index specfied
 *
 * @param index     INdex of the DcmINdex
 *
 * @return reference to the DcmINdex
 */
std::shared_ptr<DcmIndex> & DcmIndexList::GetIndex(unsigned int index)
{

  if ( index > m_indexes.size() )
    throw DcmOutOfBoundsException(LogRec("Specifed index list index is out of bounds","DcmIndexList","GetIndex"));

  return m_indexes[index];
} // GetIndex(unsigned int index)


/**
 * Indeicates if the index list is empty
 */
bool DcmIndexList::IsEmpty()
{ 
 return ( m_indexes.size() == 0 && m_multiValuedIndex->IsEmpty()) ; 
}


/**
 * Gets the default frame indexes
 *  StackId
 *  InStackPosition
 *  TemporalIndex
 *
 * @return Default frame collection indexex
 */
std::shared_ptr<DcmIndexList> DcmIndexList::GetDefaultSpatialIndexes()
{
  std::shared_ptr<DcmIndexList> index = DcmIndexList::Create("DefaultSpatialIndexes");
  index->Add(DcmIndex::Create(DCM_StackID));
  index->Add(DcmIndex::Create(DCM_InStackPositionNumber));
  //index->Add(DcmIndex::Create(DCM_TemporalPositionIndex));

  return index;
}
