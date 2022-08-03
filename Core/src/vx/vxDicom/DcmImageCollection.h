// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmImageCollection.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

#include "DcmTypedefs.h"
#include "IDcmImage.h"
#include "ICollection.h"

#pragma warning(push)
#pragma warning(disable:4251)


namespace dcm
{

// Forward declarations
//class DCM_DLL DcmImage;
class DCM_DLL DcmCosines;

template<class T> 
class DCM_DLL DcmDouble;

template<class T> 
class DCM_DLL DcmTriple;

class DCM_DLL DcmIndexList;

class DCM_DLL DcmIndex;

/**
 * Implements a auto sorted collection of images. Only a single series can be added so and
 * exception is thrown when an image is attempted to be added to this list.
 */
class DCM_DLL DcmImageCollection : public ICollection<dcm::IDcmImage>
{
public:

#pragma region enums

  /**
   * Defines the sorting directions
   */
  typedef enum SortDirection
  {
    ASCENDING,
    DECENDING
  } SortDirection;

  /**
   * Defines the sort stated of the collection
   */
  typedef enum SortState
  {
    SORTED_IMPLIED,           /* Implied ordering is the order the images were loaded */
    SORTED_ASCENDING,         /* Sorted ascending ordering is the indexed order */
    SORTED_DECENDING          /* Sorted decending */
  } SortState;

#pragma endregion

#pragma region constructors/destructors

protected:

  /// Constructor
  DcmImageCollection();

  /// Copy constructor
  DcmImageCollection(DcmImageCollection & col) {}

public:

  /// Destructor
  virtual ~DcmImageCollection();

#pragma endregion

#pragma region methods

public:

  /// Creates the image collection
  static std::shared_ptr<DcmImageCollection> Create();



#pragma region Sort methods

public:
  
  /// Sorts the image collection based on the internal indexes
  void Sort(SortDirection direction = ASCENDING);

  /// Sorts the list (if the list is empty sets the sorting)
  virtual std::shared_ptr<DcmImageCollection> Sort( std::shared_ptr<DcmIndex> & index, SortDirection direction = ASCENDING );

  /// Sorts by 1 tag
  virtual std::shared_ptr<DcmImageCollection> Sort(const std::shared_ptr<DcmIndexList> & indexes, SortDirection direction = ASCENDING );


private:

  /// Sort acending
  bool SortAscending(std::shared_ptr<IDcmImage> & first, std::shared_ptr<IDcmImage> & second, std::shared_ptr<DcmIndexList> & indexes);

  /// Sort Decending
  bool SortDecending(std::shared_ptr<IDcmImage> & first, std::shared_ptr<IDcmImage> & second, std::shared_ptr<DcmIndexList> & indexes );

  /// Implements a merg sort
  bool MergeSort(std::vector<std::shared_ptr<IDcmImage>> & list, std::shared_ptr<DcmIndexList> & indexes, SortDirection direction = ASCENDING );

  /// Compares the two images tag against each other
  int Compare(std::shared_ptr<IDcmImage> item1, std::shared_ptr<IDcmImage> item2, const DcmTag & sequenceTag, const DcmTag & elementTag );

#pragma endregion

#pragma region ICollection methods

public:

  /// Removes the specified image if possible. Returns true if deleted, false otherwise
  virtual bool Remove(const std::shared_ptr<IDcmImage> & pDcmImage);
  
  /// Removes by uid 
  virtual bool Remove( const std::string & sInstanceUid);

  /// Contains the specified image if possible
  virtual bool Contains(const std::shared_ptr<IDcmImage> & pDcmImage);
  
  /// Contains by uid 
  virtual bool Contains( const std::string & sInstanceUid);

  /// Resets the iterator
  virtual inline void Reset()
  { m_indexer = -1; }

  /// Moves the next item in the list and return true is present false if we are at the end
  virtual bool  MoveNext();

  /// Increments the iterator and returns the next item or null if we are at the end
  virtual std::shared_ptr<IDcmImage> Current();

  /// Determines if empty
  virtual bool IsEmpty()
  { return m_images.empty(); }

  /// Clears the list
  virtual inline void Clear()
  { m_images.clear(); }

  /// Gets the image count
  virtual unsigned int Count()
  { return (unsigned int)m_images.size(); }

  /// Adds image to collection and if the bSetImpliedIndexing is true a new implied index is applied
  virtual void Add(std::shared_ptr<IDcmImage> & pDcmImage);

#pragma endregion

public:

  /// Loads the specified directory of dicom files into the list
  virtual void Load(const std::string & sDirectory, const std::string & sPattern = "*.dcm", bool bIgnoreNonDicomFiles = true);

  /// Inserts images from the supplied collection if the bSetImpliedIndexing is true a new implied index is applied
  virtual void Insert(const std::shared_ptr<DcmImageCollection> & pDcmImageCollection);

    /// Gets the first image
  virtual std::shared_ptr<IDcmImage> GetFirstImage()
  { return ( m_images.empty() ? __nullptr : (*m_images.begin()) ); }

  /// Gets the last image
  virtual std::shared_ptr<IDcmImage> GetLastImage()
  { return ( m_images.empty() ? __nullptr : (*m_images.rbegin()) ); }


  /// Get the image at the supplied index
  std::shared_ptr<IDcmImage> & GetImage(unsigned int index);

  /// Pops the top element off the list
  virtual std::shared_ptr<IDcmImage>  Pop();

  // ===============================================================================================
  // Groups all images based on the StackId as a bin indciator. If the items do not contain a 
  // StackID element then only 1 stack is returned with the images copied. A list of indexes can be
  // suuplied to indicate a sort to be applied.
  // ===============================================================================================
  DcmStackMap GetStacks(std::shared_ptr<DcmIndexList> & indexes);
  DcmStackMap GetStacks();

  /// Copies these image to the new collection 
  void CopyTo(std::shared_ptr<DcmImageCollection> & collection);

  /// Gets the list of files that make up this collection
  std::vector<std::string> GetFiles();

#pragma region Concatenated Methods
  
  /// Returns true if the collection if of concatenated images
  bool IsConcatenated();
  
  /// Gets the number of expected concatenated images
  unsigned int GetExpectedNumberOfConcatenatedImages();

  /// Re-assembles the concatenated images.
  std::shared_ptr<DcmImageCollection> ReassembleConcatenatedImages();

#pragma endregion



#pragma region properties

public:


  /// Returns true if the collection is sorted
  bool IsSorted()
  { return ( m_state == SORTED_ASCENDING ||  m_state == SORTED_DECENDING  ); }

  /// Gets the sort state
  const SortState & GetSortState() const
  { return m_state; }
  
  /// Gets the indexes associated with the collection
  const  std::shared_ptr<DcmIndexList> & GetIndexes();


  /// Gets the slice spacing
  double GetSliceSpacing();

  /// Gets the series UID association with this collection
  const std::string & GetSeriesUid() const
  { return m_sSeriesUid; }

  /// Gets the plane for these images
  DcmPlanes GetPlane();

  /// Gets Pixel Spacing for the x, y and z
  DcmTriple<double> GetPixelSpacing();

  /// Gets the modality
  std::string GetModality();

  /// Gets the institution for this collection
  std::string GetInstitutioName();

  /// Gets the manufacturer for this collection
  std::string GetManufacturer();

  /// gets the manufacturer's model for this collection
  std::string GetManufacturerModel();

  /// Gets the rescale intercept for this collection
  float GetRescaleIntercept();

  /// Gets the rescale slope for this collection
  float GetRescaleSlope();

  /// Gets the rows for this collection
  unsigned int GetRows();

  /// Gets the columns for this collection
  unsigned int GetColumns();

  ///// Gets the image orientation (cosines)
  DcmCosines GetImageOrientation();

  /// Gets the pixel bias 
  int GetBias();


  /// Returns true if the collection is made up entirly of frames
  const bool IsFrames() const
  { return m_isFrameCollection; }
  
  /// Gets the pixel bias
  template<class T>
  void GetMinMaxValues(std::pair<T,T> & minMax)
  {

    // =================================
    // Only process if there are images
    // ================================
    if ( ! m_images.empty() )
    {

      std::map<T,int> minMap;
      std::map<T,int> maxMap;

      std::vector<std::shared_ptr<IDcmImage>>::iterator it = m_images.begin();

      T min = minMax.first;
      T max = minMax.second;
      for ( it++; it != m_images.end(); it++ )
      {
        std::pair<T,T> mm(min, max); 

        (*it)->GetMinMaxPixelValues<T>(mm);

        std::map<T,int>::iterator mapIt;

        if ( ( mapIt = minMap.find(mm.first)) != minMap.end() )
          mapIt->second++;
        else
          minMap.insert(std::pair<T,int>(mm.first,1));

        if ( ( mapIt = maxMap.find(minMax.second)) != maxMap.end() )
          mapIt->second++;
        else
          maxMap.insert(std::pair<T,int>(mm.second,1));


        if ( mm.first < minMax.first )
          minMax.first = mm.first;

        if ( mm.second > minMax.second )
          minMax.second = mm.second;
      } // END ... If there are images in the list

    }

  } // GetMinMaxValues()


#pragma endregion 

#pragma region fields

private:
  
  /// Indexes associated with the colIlection
  std::shared_ptr<DcmIndexList> m_indexes;

  /// Series UID for this collection
  std::string m_sSeriesUid;

  /// Image list
  DcmImageList m_images;

  /// Sort state
  SortState m_state;

  /// Bias
  int m_iBias;

  /// Indexer used for enumeration
  int m_indexer;

  /// Is Frame collection
  bool m_isFrameCollection;

#pragma endregion 

#pragma region

  friend class DCM_DLL DcmImage;

#pragma endregion

};  // class DcmImageCollection


};  // namespace dcm

#pragma warning(pop)