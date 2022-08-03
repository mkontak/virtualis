// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmImageCollection.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net


#include "Stdafx.h"
#include "DcmEnums.h"
#include "DcmImageCollection.h"
#include "DcmUtility.h"
#include "DcmImage.h"
#include "DcmTriple.h"
#include "DcmExceptions.h"
#include "DcmCosines.h"
#include "DcmImageFactory.h"
#include "DcmIndex.h"
#include "DcmIndexList.h"
#include "DcmFrame.h"

using namespace dcm;




/**
 * Constructor
 *
 *  @param eSortBy
 */
DcmImageCollection::DcmImageCollection() :
ICollection(),
m_sSeriesUid(""),
m_iBias(INT_MIN),
m_indexer(-1),
m_state(SORTED_IMPLIED),
m_indexes(DcmIndexList::Create()),
m_isFrameCollection(false)
{ 

} // DcmImageCollection


/**
 * Destructor
 */
DcmImageCollection::~DcmImageCollection()
{
  /// Clears the image list
  m_images.clear();

} // ~DcmImageCollection()


/**
 * Creates the collection
 */
std::shared_ptr<DcmImageCollection> DcmImageCollection::Create()
{
  std::shared_ptr<DcmImageCollection> collection(new DcmImageCollection());

  if ( collection == __nullptr )
    throw DcmOutOfMemoryException(LogRec("Failed to allocate DcmImageCollection","DcmImageCollection", "Create"));


  return collection;
} // Create()



#pragma region sort methods

/**
 * Sorts the list
 * 
 * @param indexes     List of indexes to be used for sorting
 * @param direction   Direction of the sort
 */
std::shared_ptr<DcmImageCollection> DcmImageCollection::Sort( const std::shared_ptr<DcmIndexList> & indexes, SortDirection direction)
{

  std::shared_ptr<DcmImageCollection> sorted(DcmImageCollection::Create());

  /// Set the current collection
  sorted->m_indexes = indexes;
  sorted->m_sSeriesUid = m_sSeriesUid;

  CopyTo(sorted);

  
  // We only need to sort if we have more then 1 image
  if ( sorted->Count() > 1 )
    sorted->Sort(direction);



  return sorted;
 
} // Sort(std::vector<DcmTag> sortTags, SortDirection direction)



/**
 * Sorts the list
 * 
 * @param DcmTag    sortTag
 */
std::shared_ptr<DcmImageCollection>  DcmImageCollection::Sort( std::shared_ptr<DcmIndex> & index, SortDirection direction )
{
  std::shared_ptr<DcmIndexList> indexes = DcmIndexList::Create();

  indexes->Add(index);

  return Sort(indexes, direction);


} // Sort( DcmTag sortTag, SortDirection direction)


/**
 * Sorts the collection based on the internal indexes
 */
void DcmImageCollection::Sort(SortDirection direction)
{
  std::shared_ptr<DcmIndexList> indexes = m_indexes;

  if ( m_indexes->HasMultiValuedIndex() )
  {
    indexes = DcmIndexList::Create();
    indexes->Add(const_cast<std::shared_ptr<DcmIndex> &>(m_indexes->GetMultiValuedIndex()));
  }


  MergeSort(m_images, indexes, direction);

  m_state = (direction == ASCENDING ? SORTED_ASCENDING : SORTED_DECENDING );
} // Sort()

/**
 * Implements a merge sort. This methd is called recursivly.
 *
 * @param list        List to be sorted
 * @param sortTags    Tags to sort by
 * @param direction   Direction of the sort
 *
 * @return Returns true if list was sorted
 */
bool DcmImageCollection::MergeSort(std::vector<std::shared_ptr<IDcmImage>> & list, std::shared_ptr<DcmIndexList> & indexes,  SortDirection direction)
{
  std::vector<std::shared_ptr<IDcmImage>> left;
  std::vector<std::shared_ptr<IDcmImage>> right;

  const int size(list.size());

  if ( size > 1 )
  {
    const int mid( (int)((float)list.size() / 2.0 + 0.5) );

    int i(0);

    // Fill in the left 
    for ( ; i < mid; left.push_back(list[i++]) );

    // Fill in the right
    for ( ; i < size; right.push_back(list[i++]) );

    MergeSort(left, indexes, direction) ;
    MergeSort(right, indexes, direction);


    i = 0;      // Reset the list Indexer
    int r(0);   // Right indexer
    int l(0);   // Left indexer

    if ( direction == ASCENDING )
    {
      // =======================================================
      // Loop until we reach the end of the right or left side
      // ======================================================
      while ( r < right.size() && l < left.size() )
      {
        if ( SortAscending( left[l], right[r], indexes ) )
          list[i++] = left[l++];
        else
          list[i++] = right[r++];
      } // END ... while
    }
    else
    {
      // =======================================================
      // Loop until we reach the end of the right or left side
      // ======================================================
      while ( r < right.size() && l < left.size() )
      {
        if ( SortDecending( left[l], right[r], indexes ) )
          list[i++] = left[l++];
        else
          list[i++] = right[r++];
      } // END ... while

    }

    for ( ; r < right.size(); list[i++] = right[r++] );
    for ( ; l < left.size(); list[i++] = left[l++] );

 
  }

  return true;
}


/**
 * Sorting method used in the sort algorithms (<)
 *
 * @param first       First item
 * @param second      Second item
 *
 * @return Returns true if first < second
 */
bool DcmImageCollection::SortAscending(std::shared_ptr<IDcmImage> & item1, std::shared_ptr<IDcmImage> & item2, std::shared_ptr<DcmIndexList> & indexes)
{
 
  int sorted(0);

  if ( indexes->Count() > 0 )
  {

    // ==========================
    // Check all the indexes
    // ==========================
    for ( int i(0); i < indexes->Count() && sorted == 0 ; i++ )
    {
      if ( indexes->GetIndex(i)->GetIndexElement().hash() == DCM_ImagePositionPatient.hash() )
      {
        std::string sFirstValue("");
        if ( item1->TryGetValue<std::string>(indexes->GetIndex(i)->GetIndexElement(), sFirstValue) )
        {
          std::string sSecondValue("");
          if ( item2->TryGetValue<std::string>(indexes->GetIndex(i)->GetIndexElement(), sSecondValue) )
          {
            DcmTriple<float> firstPositions(sFirstValue);
            DcmTriple<float> secondPositions(sSecondValue);

            DcmPlanes plane = GetPlane();

            switch (plane)
            {
            case dcm::P_AXIAL:
              sorted += (firstPositions.GetZ() == secondPositions.GetZ() ? 0 : ( firstPositions.GetZ() < secondPositions.GetZ() ? -1 : 1 ) );
              break;
            case dcm::P_CORONAL:
              sorted += (firstPositions.GetY() == secondPositions.GetY() ? 0 : ( firstPositions.GetY() < secondPositions.GetY() ? -1 : 1 ));
              break;
            case dcm::P_SAGITTAL:
              sorted += (firstPositions.GetX() == secondPositions.GetX() ? 0 : ( firstPositions.GetX() < secondPositions.GetX() ? -1 : 1 ));
              break;
            case dcm::P_NONE:
            case dcm::P_OBLIQUE:
            default:
              break;
            }
          }
          
        }

      }
      else
      {
          sorted += Compare(item1, item2, indexes->GetIndex(i)->GetFunctionalGroup(), indexes->GetIndex(i)->GetIndexElement() );
      }

    } //  END ... For each index

 
  } // END ... If there are indexes

  return ( sorted <= 0 );
} // SortAcending()

/**
 * Sorting method used in the sort algorithms (>)
 *
 * @param first       First item
 * @param second      Second item
 *
 * @return Returns true if first > second
 */
bool DcmImageCollection::SortDecending(std::shared_ptr<IDcmImage> & item1, std::shared_ptr<IDcmImage> & item2, std::shared_ptr<DcmIndexList> & indexes)
{

  int sorted(0);
 

  if ( indexes->Count() > 0 )
  {

    // ==========================
    // Check all the indexes
    // ==========================
    for ( int i(0); i < indexes->Count() && sorted == 0 ; i++ )
    {

      // =======================================================================
      // If the index indicate image position then we need to account for X,Y,Z
      // ========================================================================
      if ( indexes->GetIndex(i)->GetIndexElement() == DCM_ImagePositionPatient )
      {
        std::string sFirstValue("");
        if ( item1->TryGetValue<std::string>(indexes->GetIndex(i)->GetIndexElement(), sFirstValue) )
        {
          std::string sSecondValue("");
          if ( item2->TryGetValue<std::string>(indexes->GetIndex(i)->GetIndexElement(), sSecondValue) )
          {
            DcmTriple<float> firstPositions(sFirstValue);
            DcmTriple<float> secondPositions(sSecondValue);

            DcmPlanes plane = GetPlane();

            switch (plane)
            {
            case dcm::P_AXIAL:
              sorted += (firstPositions.GetZ() == secondPositions.GetZ() ? 0 : ( firstPositions.GetZ() < secondPositions.GetZ() ? -1 : 1 ) );
              break;
            case dcm::P_CORONAL:
              sorted += (firstPositions.GetY() == secondPositions.GetY() ? 0 : ( firstPositions.GetY() < secondPositions.GetY() ? -1 : 1 ));
              break;
            case dcm::P_SAGITTAL:
              sorted += (firstPositions.GetX() == secondPositions.GetX() ? 0 : ( firstPositions.GetX() < secondPositions.GetX() ? -1 : 1 ));
              break;
            case dcm::P_NONE:
            case dcm::P_OBLIQUE:
            default:
              break;
            }
          }
          
        }

      }
      else
      {
        sorted += Compare(item1, item2, indexes->GetIndex(i)->GetFunctionalGroup(), indexes->GetIndex(i)->GetIndexElement() );
      }
    }
 
  }

  return ( sorted >= 0 );
} // SortDecending()


/**
 * Compares the teo items element and returns 0 if equal, -1 if item1 < item2,1 if item1 > item2 
 *
 * @param item1         Item 1
 * @param item2         Item 2
 * @param tag           Element to compare
 *
 * @return    0 - Equals, -1 item1<item2, 1 item1>item2
 */
int DcmImageCollection::Compare(std::shared_ptr<IDcmImage> item1, std::shared_ptr<IDcmImage> item2, const DcmTag & sequenceTag, const DcmTag & elementTag )
{
  int iCompareResults(0);


  std::shared_ptr<DcmElement> element1;
  std::shared_ptr<DcmElement> element2;

  if ( item1->TryGetElement(sequenceTag, elementTag, element1, true) )
  {
    if ( item2->TryGetElement(sequenceTag, elementTag, element2, true) )
    {
      if ( elementTag.hash() != DCM_StackID.hash() && DcmUtility::IsStringRepresentation(elementTag) )
      {
        iCompareResults = strcmp(element1->GetValue().c_str(), element2->GetValue().c_str());
      }
      else if ( element1->GetVM() > 0 )
      {
        std::vector<std::string> fields1 = util::Strings::Split(element1->GetValue(),'\\');
        std::vector<std::string> fields2 = util::Strings::Split(element2->GetValue(),'\\');

        int iMin = min((int)fields1.size(), (int)fields2.size());

        for ( int i(0); i < iMin && iCompareResults == 0; i++ )
        {
          double value1;
          double value2;

           std::istringstream is1(fields1[i].c_str());
           is1 >> value1;

           std::istringstream is2(fields2[i].c_str());
           is2 >> value2;

           iCompareResults = ( value1 == value2 ? 0 : ( value1 < value2 ? -1 : 1 ) );

       }

      }
      else
      {
        double dFirstValue(element1->GetValue<double>());
        double dSecondValue(element2->GetValue<double>());

        iCompareResults = ( dFirstValue == dSecondValue ? 0 : ( dFirstValue < dSecondValue ? -1 : 1 ) );
      }
   
      
    }
    
  }

  return iCompareResults;

}

#pragma endregion

/**
 * Copies the list of image from this collection to the supplied collection
 */
void DcmImageCollection::CopyTo(std::shared_ptr<DcmImageCollection> & collection)
{
    /// Copy images
  for ( register unsigned int i(0); i < Count(); collection->m_images.push_back(GetImage(i++)) );


} // CopyTo(std::shared_ptr<DcmImageCollection> & collection)


/**
 * Returns the list of files that make up this collection. Only uniqiue files are returned no
 * duplicates.
 *
 * @retun Vector of files that make up this collection
 */
std::vector<std::string> DcmImageCollection::GetFiles()
{

  std::vector<std::string> files;

  for ( int i(0); i < m_images.size(); i++ )
  {
    if ( ! m_images[i]->GetFilename().empty() )
    {
      std::string filename(m_images[i]->GetFilename());

      bool found(false);
      for ( int j(0); j < files.size() && !found; j++ )
      {
        if ( files[j] == filename )
          found = true;
      }

      if ( ! found )
        files.push_back(m_images[i]->GetFilename());

    }
  }

  return files;

} // GetFiles();


/**
 * Loads the specified directory of dicom files into the list
 *
 *  @param sDirectory
 *  @param sPattern
 *  @param bIgnoreNonDicomFiles
 */
void DcmImageCollection::Load(const std::string & sDirectory, const std::string & sPattern, bool bIgnoreNonDicomFiles)
{
  std::vector<std::string> files = dcm::DcmUtility::GetFiles(sDirectory, sPattern);

  for ( std::vector<std::string>::const_iterator it = files.cbegin(); it != files.cend(); it++ )
  {
    try
    {
      Add(std::dynamic_pointer_cast<IDcmImage>(DcmImageFactory::Create( (*it), PT_FILE)));
    }
    catch ( DcmInvalidDicomFile )
    {
      if ( ! bIgnoreNonDicomFiles )
        throw;
    }
  } // for

} // Load(const std::string & sDirectory, const std::string & sPattern, bool bIgnoreNonDicomFiles)


/**
 * Removes the specied image if it exists.
 *
 * @param pDmcImage       Image to be removed
 *
 * @return Returns true if the image was deleted, false if the image was not
 */
inline bool DcmImageCollection::Remove(const std::shared_ptr<IDcmImage> & pDcmImage)
{
  return Remove(pDcmImage->GetInstanceUid());
} // Remove(const std::shared_ptr<IDcmImage> & pDcmImage)


/**
 * Removes the specified key if it exists
 *
 * @param sInstanceUid    UiD
 *
 * @return TRue is removed, false otherwise
 */
bool DcmImageCollection::Remove(const std::string & sInstanceUid)
{
  bool bRemoved(false);

  for ( std::vector<std::shared_ptr<IDcmImage>>::iterator it(m_images.begin()); it != m_images.end(); it++ )
  {
    if ( (*it)->GetInstanceUid() == sInstanceUid )
    {
      m_images.erase(it);
      bRemoved = true;
    }
  }
  
  if ( m_images.empty()  )
    m_sSeriesUid = "";

 return bRemoved;
} // Remove(const std::string & sInstanceUid)


/**
 * Determines if the image is contained in this collection.
 *
 * @param pDmcImage       Image to be removed
 *
 * @return Returns true if the image is containsed in the list, false if the image was not
 */
inline bool DcmImageCollection::Contains(const std::shared_ptr<IDcmImage> & pDcmImage)
{
  return Contains(pDcmImage->GetInstanceUid());
} // Contains(const std::shared_ptr<IDcmImage> & pDcmImage)


/**
 * Determines if the instance uid is contained in the list
 *
 * @param sInstanceUid    UiD
 *
 * @return True if contained
 */
bool DcmImageCollection::Contains(const std::string & sInstanceUid)
{
  bool bContained(false);

  for ( std::vector<std::shared_ptr<IDcmImage>>::iterator it(m_images.begin()); it != m_images.end(); it++ )
  {
    if ( (*it)->GetInstanceUid() == sInstanceUid )
    {
      bContained = true;
    }
  }

  return bContained;
} // Contains(const std::string & sInstanceUid)


/**
 * Adds the image t0 the collection
 *
 * @param pDcmImage     Image to be added
 */
void DcmImageCollection::Add(std::shared_ptr<IDcmImage> & pDcmImage)
{

  // Get the series UID
  std::string sSeriesUid(pDcmImage->GetSeriesInstanceUid());

  // =======================================================
  // Make sure the image is in the same series 
  // ========================================================
  if ( ! m_sSeriesUid.empty() && m_sSeriesUid != sSeriesUid )
    throw DcmInvalidDataException(LogRec(util::Strings::Format("Image is not from the same series %s != %s", sSeriesUid.c_str(), m_sSeriesUid.c_str()),"DcmImageCollection", "Add"));

  m_sSeriesUid = sSeriesUid;

  // ==================================================================================
  // If this is the first image then set the is frame flag based on the type of image.
  // Else check to make sure that images and frames are not mixed.
  // ==================================================================================
  if ( m_images.size() == 0 )
    m_isFrameCollection = pDcmImage->IsFrame();
  else if ( pDcmImage->IsFrame() != m_isFrameCollection )
    throw DcmInvalidDataException(LogRec("Cannot mix dicom image and frames in the same collection", "DcmImageCollection","Add"));

  // ======================================================================================================================
  // Only check the instance UID if we are an image so that the image is unique. If its a frame then multiple-frames will 
  // have the parent images insance uid
  // ======================================================================================================================
  if ( ! pDcmImage->IsFrame() )
  {
  
    std::string sInstanceUid(pDcmImage->GetInstanceUid());

    // =====================================
    // Make sure there is an instance UID
    // =====================================
    if ( sInstanceUid.empty() )
      throw DcmInvalidDataException(LogRec("Image does not contain an instance UID","DcmImageCollection", "Add"));

    // ============================================================================
    // Cannot add duplicate images if the unique flag is set which is the default
    // ============================================================================
    if ( Contains(pDcmImage) )
      throw DcmDuplicateImageException(LogRec(util::Strings::Format("Image %s already exists in collection", sInstanceUid.c_str()),"DcmImageCollection", "Add"));
  }

  // Inserts the image into the map collection
  m_images.push_back(pDcmImage);


  // Set the bias
  m_iBias = INT_MIN;

} // Insert(DcmImage * pDcmImage)


/**
 * Inserts all images from the supplied collection to this collection
 *
 * @param pDcmImageCollection       Collection to copy from
 *
 */
void DcmImageCollection::Insert(const std::shared_ptr<DcmImageCollection> & pDcmImageCollection)
{
  for ( register unsigned int i(0); i < pDcmImageCollection->Count(); Add(pDcmImageCollection->GetImage(i++)) );
} // Insert()

/**
 * Pops the first image from the list
 *
 * @return Returns the DcmImage or __nullptr
 * @return the pointer of the image
 */
std::shared_ptr<IDcmImage>  DcmImageCollection::Pop()
{ 
  if ( m_images.empty() )
  {
    return std::shared_ptr<IDcmImage>(__nullptr);
  }
  else
  {
    std::shared_ptr<IDcmImage> pDcmImage((*m_images.begin()) ); 

    m_images.erase(m_images.begin());

    if ( m_images.empty()  )
        m_sSeriesUid = "";


    return pDcmImage;

    m_iBias = INT_MIN;
  }
} // Pop()



/**
 * Gets the next image on the list
 * 
 * @return true if we moved or false if we are at the end
 */
bool DcmImageCollection::MoveNext()
{
 bool bMoved(false);

  if ( ++m_indexer < m_images.size() )
    bMoved = true;
  
 
  return bMoved;
} // MoveNext()



/**
 * Returns a collection of stacked images based on the StackId. If indexes are supplied then the stacks will be sorted by
 * indexes supplied. 
 *
 * @param indexes       Indexes to be used to sort the stack. If the indexes are Empty() then no sorting 
 *                      will be done.
 *
 * @return Returns a map of image collections where the map key is the StackId.
 */
DcmStackMap DcmImageCollection::GetStacks( std::shared_ptr<DcmIndexList> & indexes)
{
  

  // Default key 
  int stackId(0);

  // Stack MAP to be returned
  DcmStackMap stacks;

  // =================================================================================================
  // If we have images then we segregate the stack out of the current collection if images taking 
  // into account any multi-framed images.
  // ================================================================================================
  if ( m_images.size() > 0 )
  {

    // Collection to hold either a copy
    std::shared_ptr<DcmImageCollection> collection = ReassembleConcatenatedImages();


    // =====================================================
    // Only need to copy if this collection was concatenated 
    // ======================================================
    if ( ! IsConcatenated() )
    {
      collection->m_indexes = m_indexes;
      CopyTo(collection);
    } // END ... If concatenated


 
    // ==================================================================
    // Loop through all the images building the multi-frame collections
    // ===================================================================
    for ( register unsigned int i(0); i < collection->Count();  i++ )
    {

      std::shared_ptr<IDcmImage> image( collection->GetImage(i) );
     

      // ===================================================
      // Only check for frames if the image contains frames
      // ===================================================
      if ( ! image->IsFrame() && image->GetNumberOfFrames() > 0 )
      {

        /// Get the image frames for the image
        std::shared_ptr<DcmImageCollection> imageFrames(std::dynamic_pointer_cast<DcmImage>(image)->GetFrames());


        /// Get the stacks from the images farmes
        DcmStackMap  stackedFrames = imageFrames->GetStacks();


        // =======================================================================
        /// Loop through all stacks returned and load them into the local copy
        // =======================================================================
        std::for_each(stackedFrames.begin(), stackedFrames.end(), [&] ( DcmStackMapItem item ) 
        {

          /// Set the stacks to the same index as the frames
          item.second->m_indexes = imageFrames->m_indexes;

          // Try to find the corresponding frames stack id in the local stack collection
          DcmStackMap::iterator it(stacks.find(item.first));

          // ============================================================================================
          // If the frames stack id was not found then add a new stack to the collection otherwise add 
          // all the frames from the frames stacks to the corresponding local stack.
          // ============================================================================================
          if ( it == stacks.end() )
            stacks.insert(DcmStackMapItem(item.first, item.second));
          else
            for ( register unsigned int j(0); j < item.second->Count(); (*it).second->Add(item.second->GetImage(j++)) );

        } );

      } // END ... if the image cointains frames

      //==================================================
      // Else the image is a single DcmImage or DcmFrame
      // =================================================
      else
      {

        // ========================================================
        // if the stack id was not found the the stack id is empty
        // =========================================================
        if ( ! image->TryGetValue<int>(DCM_StackID,stackId, true) )
          stackId = 0;

        // Find the stack id in the local list
        DcmStackMap::iterator it(stacks.find(stackId));

        // ====================================================================================
        // If the ID was not found then create a new stack collection and insert into the list
        // ===================================================================================
        if ( it == stacks.end() )
        {
          std::shared_ptr<DcmImageCollection> newCollection(DcmImageCollection::Create());
          newCollection->Add(image);
          newCollection->m_indexes = collection->m_indexes;
          stacks.insert(DcmStackMapItem(stackId, newCollection));
        }
        else
          stacks[stackId]->Add(image);

      }
    }



    // =====================================================================
    // If the supplied indexes is not empty then sort each of the stacks
    // by this list.
    // =====================================================================
    if ( ! indexes->IsEmpty() )
    {

      // ====================================================
      // Loop through all the stacks and perform the sort
      // ===================================================
      std::for_each( stacks.begin(), stacks.end(), [&] ( DcmStackMapItem item )
      {

        if ( item.second->GetIndexes() != indexes )
        {
          // ================================================================================
          // If the specified indexes supplied exist in the stacks images (only check the first 
          // image ) then use the index otherwsie leave the index from this collection.
          // ================================================================================
          if ( item.second->GetFirstImage()->ContainsIndexes(indexes) )
            item.second->m_indexes = indexes;
          else if ( item.second->GetIndexes()->IsEmpty() )
            item.second->m_indexes = collection->m_indexes;
        }

        item.second->Sort();

      } );

    }

  
  } // END ... If there are images in the list



  return stacks;

} // GetStacks()


/**
 * Gets the stack if images based on the StackID if present otherwise a copy of the collection is 
 * returned as 1 stack.
 *
 * @return DcmStackMap
 */
inline DcmStackMap DcmImageCollection::GetStacks()
{ 
  return GetStacks(DcmIndexList::Create()); 
} // GetStacks()

/**
 * Gets the indexes for this collection. It is either set internally or I will attempt to get the defined DimesionIndexSequence 
 * from the first image and use that as the index.
 *
 * @return DcmIndexList
 */
const std::shared_ptr<DcmIndexList> & DcmImageCollection::GetIndexes() 
{

  // ===========================================================================================
  // If this is not a collection of frames, the index is current empty and there are images 
  // then look for the first images index which will return the DimesionIndexSequence values
  // if present.
  // ===========================================================================================
  if ( ! IsFrames() &&  m_indexes->IsEmpty() && m_images.size() > 0 )
  {
    m_indexes = GetFirstImage()->GetIndexes() ;
  }

  return m_indexes;
}

/**
 * Gets the current dicom image from the list or null if we are at the end
 *
 * @return DcmImage
 */
std::shared_ptr<IDcmImage> DcmImageCollection::Current()
{
  std::shared_ptr<IDcmImage> image(__nullptr);

  if ( m_indexer > -1 && m_indexer < m_images.size() )
    image = m_images[m_indexer];
  
  return image;
} // Next()

/**
 * Gets the image at the spcified index
 *
 * @return DcmImage
 */
std::shared_ptr<IDcmImage> & DcmImageCollection::GetImage(unsigned int index)
{
  if ( index > m_images.size() )
    throw DcmOutOfBoundsException(LogRec("Index out of bounds","DcmImageCollection","GetImage"));
  
  return m_images[index];
} // GetImage(unsigned int index)


#pragma region proeprties


#pragma region Concatenated Methods

/**
 * Returns tru if the images collection is a concatenated collection
 */
bool DcmImageCollection::IsConcatenated()
{ 
  return ( m_images.size() > 0 ? GetFirstImage()->IsConcatenated() : false );
} // IsConcatenated()

/**
 * Returns the number of expected images that are to be concatenated
 *
 * @return expected number of concatenated images
 */
unsigned int DcmImageCollection::GetExpectedNumberOfConcatenatedImages()
{
  return ( m_images.size() > 0 ? GetFirstImage()->GetNumberOfConcatenatedImages() : false );
} // GetExpectedNumberOfConcatenatedImages()


/**
 * Reassmbles a collection of concatenated images. Re-numbering the frames to their original number prior to being split 
 * into multiple images.
 *
 *
 *  Example:
 *
 *    Image 1 (Concatenation Number = 1 ) has 10 frames with numbers 1-10 (frame numbers relative to the image)
 *    Image 2 (Concatenation Number = 2 ) has 10 frames with numbers 1-10 (frame numbers relative to the image)
 *
 *    New Collection contains only the frames:
 *      Frame 1   .... Image 1/Frame 1
 *      Frame 2   .... Image 1/Frame 2
 *                
 *                ....
 *
 *      Frame 11  .... Image 2/Frame 1
 *      Frame 12  .... Image 2/Frame 2
 *
 *
 * @return New collection of frames from this collectin of images if thet ar concatenated, otherwise an empty collection.
 */
std::shared_ptr<DcmImageCollection> DcmImageCollection::ReassembleConcatenatedImages()
{

  std::shared_ptr<DcmImageCollection> collection(DcmImageCollection::Create());

  // =====================================================================================================================
  // Check if the image collection is concatenated meaning that it is a collection if multi-framed images that require
  // re-assembly. This check is done by checking if the first images is a concatenated image (one of a group of images). 
  // This is really a redundant check since the calling application should check if the collection is concatenated before
  // calling this method.
  // =====================================================================================================================
  if ( IsConcatenated() )
  { 

    /// Set the defult indexes
    collection->m_indexes = m_indexes;

    // Get the number of expected images in the concatenation (Will be 0 if not concatenated)
    const unsigned short numberOfConcatenatedImages(GetExpectedNumberOfConcatenatedImages());

    // ===============================================================================================================================
    // Make sure that we have all the images that are suppose to be part of the concatenation otherwise is makes no sense to process.
    // ================================================================================================================================
    if ( numberOfConcatenatedImages != m_images.size() )
      throw DcmInvalidDataException(LogRec(util::Strings::Format("Number of images %d does not match the number of concatenated images %d", m_images.size(), numberOfConcatenatedImages), "DcmImageCollection","GetStacks"));
    
    /// Sort by the concatenation number which orders the images in the order they should appear 
    std::shared_ptr<DcmImageCollection> sortedByConcatNumber(this->Sort(DcmIndex::Create(DCM_InConcatenationNumber)));

   
    int iFrameNumber(1);  

    // =================================================================================================================================================
    // Loop though all the images to extract the frames. Then insert all the frames into the new collection assigning a new frame numbare that 
    // applies to the concatentation.
    //
    //  Example:
    //
    //    Image 1 (Concatenation Number = 1 ) has 10 frames with numbers 1-10 (frame numbers relative to the image)
    //    Image 2 (Concatenation Number = 2 ) has 10 frames with numbers 1-10 (frame numbers relative to the image)
    //
    //    New Collection contains only the frames:
    //      Frame 1   .... Image 1/Frame 1
    //      Frame 2   .... Image 1/Frame 2
    //                
    //      Frame 11  .... Image 2/Frame 1
    //      Frame 12  .... Image 2/Frame 2
    // ===============================================================================================================================================
    for ( register unsigned int i(0); i < sortedByConcatNumber->Count(); i++ )
    {

      // Get the frames from the image
      std::shared_ptr<DcmImageCollection> frameCollection = std::dynamic_pointer_cast<DcmImage>(sortedByConcatNumber->GetImage(i))->GetFrames();

      // ==========================================================================================================
      // Loop though each frame and add to the new collection so that the frame number can be set. The frame number
      // is the implied frame number based on the order in the image. 
      // ===========================================================================================================
      for ( register unsigned int j(0); j < frameCollection->Count(); j++ )
      {
        // Get the frame
        std::shared_ptr<DcmFrame> frame(std::dynamic_pointer_cast<DcmFrame>(frameCollection->GetImage(j)));

        // Set the new frame number based on the re-assembled multi-frame
        frame->m_iFrameNumber = iFrameNumber++;

        // Insert into new collection
        collection->Add(frameCollection->GetImage(j));

      } // END ... Fro each frame

    } // END ... For each image

  } // END ... .If the collection is concatenated

  return collection;

}   // ReassembleConcatenatedImages()

#pragma endregion

/**
 * Gets the image plane for the collection of images
 *
 *  @return image list or collection
 */
DcmPlanes DcmImageCollection::GetPlane()
{
  return ( m_images.empty() ? DcmPlanes::P_NONE : (*m_images.begin())->GetPlane() );
} // GetPlane()


/**
 * Gets the pixel spacing for collection
 *
 *  @return pixel spacing
 */
DcmTriple<double> DcmImageCollection::GetPixelSpacing()
{
  DcmDouble<double> pixelSpacing = ( m_images.empty() ? DcmDouble<double>() : (*m_images.begin())->GetPixelSpacing() );

  return DcmTriple<double>(pixelSpacing.GetX(), pixelSpacing.GetY(), GetSliceSpacing() );

} // GetPixelSpacing()


/**
 * Gets the modality for the collection of images
 *
 *  @return the modality string
 */
std::string DcmImageCollection::GetModality()
{
  return ( m_images.empty() ? std::string("") : (*m_images.begin())->GetModality() );
} // GetModality()


/**
 * Gets the institution for the collection of images
 *
 *  @return  the string of institutional name
 */
std::string DcmImageCollection::GetInstitutioName()
{
  return ( m_images.empty() ? std::string("") : (*m_images.begin())->GetInstitutionName() );
} // GetInstitution()


/**
 * Gets the manufacturer for the collection of images
 *
 *  @return the string of the manufacturer name
 */
std::string DcmImageCollection::GetManufacturer()
{
  return ( m_images.empty() ? std::string("") : (*m_images.begin())->GetManufacturer() );
} // GetManufacturer()


/**
 * Gets the manufacturer model for the collection of images
 *
 *  @return the string of scanner model name
 */
std::string DcmImageCollection::GetManufacturerModel()
{
  return ( m_images.empty() ? std::string("") : (*m_images.begin())->GetManufacturerModel() );
} // GetManufacturerModel()


/**
 * Gets the rescale intercept for the collection of images
 *
 *  @return the scale intercept
 */
float DcmImageCollection::GetRescaleIntercept()
{
  return ( m_images.empty() ? 0.0F : (*m_images.begin())->GetRescaleIntercept() );
} // GetRescaleIntercept()


/**
 * Gets the rescale intercept for the collection of images
 *
 *  @return the rescale slope
 */
float DcmImageCollection::GetRescaleSlope()
{
  return ( m_images.empty() ? 0.0F  : (*m_images.begin())->GetRescaleSlope() );
} // GetRescaleSlope()


/**
 * Gets the rows for the collection of images
 *
 *  @return the number of rows
 */
unsigned int DcmImageCollection::GetRows()
{
  return ( m_images.empty() ? 0 : (*m_images.begin())->GetRows() );
} // GetRows()


/**
 * Gets the columns for the collection of images
 *
 *  @return the number of columns
 */
unsigned int DcmImageCollection::GetColumns()
{
  return ( m_images.empty() ? 0 : (*m_images.begin())->GetColumns() );
} // GetColumns()


/**
 * Gets the pixel spacing for collection
 *
 *  @return the slice spacing 
 */
double DcmImageCollection::GetSliceSpacing()
{

 
  // ===============================================================================================================
  // If the current collection is sorted and sorted by the DCM_ImagePositionPatient only then we can return the 
  // slice spacing
  // ===============================================================================================================
  if ( IsSorted() && ( m_indexes->Count() == 1 && m_indexes->GetIndex(0)->GetIndexElement() == DCM_ImagePositionPatient ) )
  {

    double count(1.0);
    double min = 0.0;
    double max = 0.0;

    if ( ! m_images.empty() )
    {
      switch ( GetPlane() )
      {
      case P_AXIAL:
        min = (double)((*m_images.begin())->GetImagePosition().GetZ());
        max = (double)((*m_images.rbegin())->GetImagePosition().GetZ());
        break;
      case P_CORONAL:
        min = (double)((*m_images.begin())->GetImagePosition().GetY());
        max = (double)((*m_images.rbegin())->GetImagePosition().GetY());
        break;
      case P_SAGITTAL:
        min = (double)((*m_images.begin())->GetImagePosition().GetX());
        max = (double)((*m_images.rbegin())->GetImagePosition().GetX());
        break;
      default:
        break;
      };


	  // We are only interested in the space between the slices
      count = (double)m_images.size() - 1;
    } // if


    return (( max - min ) / count );

  }
  else
  {
    std::shared_ptr<DcmImageCollection> sorted = Sort(DcmIndex::Create(DCM_ImagePositionPatient));

    return sorted->GetSliceSpacing();
  }

} // GetSliceSpacing()

 
/**
 * Gets the image orientation
 * 
 *  @return the cosines
 */
DcmCosines DcmImageCollection::GetImageOrientation()
{
  return ( IsEmpty() ? DcmCosines() : (*m_images.begin())->GetImageOrientation() );
} // GetImageOrientation()


/**
 * Gets the pixel bias
 *
 *  @return the bias for volume value shift
 */
int DcmImageCollection::GetBias()
{

  if ( m_iBias != INT_MIN )
    return m_iBias;  

  if (  ! m_images.empty() )
  {
    vxLogTimer timer;

    std::shared_ptr<IDcmImage> pDcmImage(GetFirstImage());

    std::string sManufacturer = toupper(pDcmImage->GetManufacturer());
   // std::string sManufacturer = pDcmImage->GetManufacturer();
   // std::transform(sManufacturer.begin(), sManufacturer.end(), sManufacturer.begin(), ::toupper); 

    std::string sModality     = pDcmImage->GetModality();
    float fRescaleSlope       = pDcmImage->GetRescaleSlope();
    float fRescaleIntecept    = pDcmImage->GetRescaleIntercept();

    // ========================================================================================================================
    // Due to the fact that the Toshiba does not fill in the rescale intercept fro the CT we set it here to a default of 1000
    // ========================================================================================================================
    if ( sManufacturer.find("TOSHIBA") != std::string::npos && sModality == "CT" && fRescaleIntecept == 0.0F )
    {
      m_iBias = 1000;
    }
    else
    {

      std::pair< short,  short> minMax(SHRT_MAX,SHRT_MIN);

      GetMinMaxValues<short>(minMax);
   
      int iMin = (int)( fRescaleSlope * (float)(minMax.first) + fRescaleIntecept );

      // We only need a bias if the minimum value is negative
      m_iBias = ( iMin < 0 ? abs(iMin) : 0 );

    } // else

    timer.Trace("Getting the bias");

    return m_iBias;

  } // END ... If there are images in the list
  else
    return 0;
} // GetBias()

#pragma endregion