
// includes
#include "StdAfx.h"
#include <string>

#include "ImageFactory.h"
#include "ImageCollection.h"
#include "ImageCollectionEnumerator.h"
#include "Cosines.h"
#include "scoped_ptr.h"
#include "DcmImageCollection.h"
#include "DcmImageFactory.h"
#include "DcmTriple.h"
#include "DcmImage.h"
#include "Image.h"
#include "Logger.h"

// namespaces
USING_DICOM_NS

#pragma region constructors/destructors

/**
 * Default constructor.
 */
ImageCollection::ImageCollection() :
m_bIsReadOnly(false),
m_pImageCollection(scoped_ptr<std::shared_ptr<dcm::DcmImageCollection>>(new std::shared_ptr<dcm::DcmImageCollection>(dcm::DcmImageCollection::Create())))
{

} // ImageCollection()


/**
 * Destructor.
 */
ImageCollection::~ImageCollection()
{
  this->!ImageCollection();
} // ~ImageCollection()


/**
 * Finalizer.
 */
ImageCollection::!ImageCollection()
{
  m_pImageCollection.reset();
} // !ImageCollection()

#pragma endregion

#pragma region properties


/**
 * Gets the current element
 *
 * @return Current element
 */
IImage ^ ImageCollection::Current::get()
{
  std::shared_ptr<dcm::IDcmImage> * pImage(new std::shared_ptr<dcm::IDcmImage>((*m_pImageCollection)->Current()));

  if ( pImage->get() == __nullptr )
    return nullptr;

  return ImageFactory::Create(System::IntPtr((void *)(pImage)));
} // Current

/**
 * Gets the count of elements
 */
int ImageCollection::Count::get()
{
  return (*m_pImageCollection)->Count();
} // COunt


/**
 * Gets the synchronized flag
 */
bool ImageCollection::IsSynchronized::get()
{
  return true;
} // Synchronized



/**
 * Gets the synchronized flag
 */
Object ^ ImageCollection::SyncRoot::get()
{
  return nullptr;
} // SyncRoot



/**
 * Gets the slice spacing.
 */
double ImageCollection::SliceSpacing::get()
{
  return (*m_pImageCollection)->GetSliceSpacing();
} // SliceSpacing::get()



/**
 * Gets the series orientation.
 */
Viatronix::Dicom::Cosines ^ ImageCollection::ImageOrientation::get()
{
  dcm::DcmCosines cosines = (*m_pImageCollection)->GetImageOrientation();
  Viatronix::Dicom::Cosines ^ mpCosines = gcnew Viatronix::Dicom::Cosines();

  mpCosines->RowX = cosines.GetRowX();
  mpCosines->RowY = cosines.GetRowY();
  mpCosines->RowZ = cosines.GetRowZ();

  mpCosines->ColumnX = cosines.GetColumnX();
  mpCosines->ColumnY = cosines.GetColumnY();
  mpCosines->ColumnZ = cosines.GetColumnZ();

  return mpCosines;
} // ImageOrientation::get()


/**
 * Gets the series pixel spacing.
 */
cli::array<double> ^ ImageCollection::PixelSpacing::get()
{
  dcm::DcmTriple<double> pixelSpacing = (*m_pImageCollection)->GetPixelSpacing();
  cli::array<double> ^ mpPixelSpacing = gcnew cli::array<double>(3);

  mpPixelSpacing[0] = pixelSpacing.GetX();
  mpPixelSpacing[1] = pixelSpacing.GetY();
  mpPixelSpacing[2] = pixelSpacing.GetZ();

  return mpPixelSpacing;
} // PixelSpacing::get()


/**
 * Gets the series dimensions
 */
cli::array<double> ^ ImageCollection::Dimensions::get()
{
  cli::array<double> ^ mpDimensions = gcnew cli::array<double>(3);

  mpDimensions[0] = (*m_pImageCollection)->GetColumns();
  mpDimensions[1] = (*m_pImageCollection)->GetRows();
  mpDimensions[2] = (*m_pImageCollection)->Count();

  return mpDimensions;
} // Dimensions::get()


/**
 * Gets the reference point from the first image in the collection.
 */
cli::array<float> ^ ImageCollection::ReferencePoint::get()
{
  dcm::DcmTriple<float> referencePoint = (*m_pImageCollection)->GetFirstImage()->GetImagePosition();
  cli::array<float> ^ mpReferencePoint = gcnew cli::array<float>(3);

  mpReferencePoint[0] = referencePoint.GetX();
  mpReferencePoint[1] = referencePoint.GetY();
  mpReferencePoint[2] = referencePoint.GetZ();

  return mpReferencePoint;
} // ReferencePoint::get()

#pragma endregion


#pragma region methods

/**
 * Creates the image collecion from the std::sared_ptr
 *
 * @param dcmImageCOllectionPtr     
 *
 * @rerturn ImageCollection
 */
ImageCollection ^ ImageCollection::Create(System::IntPtr dcmImageCollectionPtr)
{
  ImageCollection ^ mpImageCollection(gcnew ImageCollection());

  if ( mpImageCollection == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate ImageCOllection object");

  mpImageCollection->m_pImageCollection.reset((std::shared_ptr<dcm::DcmImageCollection> *)(dcmImageCollectionPtr.ToPointer()));

  return mpImageCollection;

} // Create()

/**
 * Creates the image collecion from the std::sared_ptr
 *
 * @param bUnique       Unique indicator
 *
 * @rerturn ImageCollection
 */
ImageCollection ^ ImageCollection::Create()
{
  ImageCollection ^ mpImageCollection(gcnew ImageCollection());

  if ( mpImageCollection == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate ImageCOllection object");

  return mpImageCollection;

} // Create()

#pragma region enumeration implementation


/**
 * Continues the iteration
 *
 * @return next element or null if none
 */
void ImageCollection::Reset()
{
  (*m_pImageCollection)->Reset();
} // Reset()


/**
 * Clears the collection
 */
void ImageCollection::Clear()
{
  (*m_pImageCollection)->Clear();

} // Clear()


/**
 * Clears the collection
 */
ImageCollection ^ ImageCollection::Sort()
{

  std::shared_ptr<dcm::DcmImageCollection> * pCollection(new std::shared_ptr<dcm::DcmImageCollection>((*m_pImageCollection)->Sort((*m_pImageCollection)->GetIndexes())));

  return Create(System::IntPtr((void *)(pCollection)));

} // Sort()

/**
 * Moves to the next item in the list
 */
bool ImageCollection::MoveNext()
{
  return ( (*m_pImageCollection)->MoveNext() );
 } // MoveNext()


/**
 * Gets the enumerator
 */
Generic::IEnumerator<IImage ^> ^ ImageCollection::GetEnumerator()
{
  return gcnew ImageCollectionEnumerator(this);
} // GetEnumerator()



/**
 * Copies collection to array
 */
void ImageCollection::CopyTo(array<IImage ^> ^ mpArray, int index)
{

  Reset();

  int i(-1);

  while ( MoveNext() )
  {

    if ( ++i >= index )
      mpArray[i] = this->Current;
  }

  
} // CopyTo()


/** 
 * Adds the specified element to the list if it does not exist. 
 *
 * @param mpElement     Element to add
 */
void ImageCollection::Add(IImage ^ mpImage)
{
 
   (*m_pImageCollection)->Add(  *((std::shared_ptr<dcm::IDcmImage> *)(mpImage->GetIDcmImagePtr().ToPointer())) );
 
} // Insert(Element ^ mpElement)

/** 
 * Removes the specified element to the list if it does not exist. 
 *
 * @param mpElement     Element to remove
 */
bool ImageCollection::Remove(IImage ^ mpImage)
{
 return (*m_pImageCollection)->Remove(  *((std::shared_ptr<dcm::IDcmImage> *)(mpImage->GetIDcmImagePtr().ToPointer())) );
} // Insert(Element ^ mpElement)




/** 
 * Determines in the collection contains the element
 *
 * @param mpElement     Element to check for
 *
 * @return Returns the true if the element is in the list, flase othersie
 */
bool ImageCollection::Contains(String ^ msUid)
{

  return (*m_pImageCollection)->Contains(ss(msUid));

} // Contains(Element ^ mpElement)

/** 
 * Determines in the collection contains the element
 *
 * @param mpElement     Element to check for
 *
 * @return Returns the true if the element is in the list, flase othersie
 */
inline bool ImageCollection::Contains(IImage ^ mpImage)
{

  return Contains(mpImage->InstanceUid);

} // Contains(Element ^ mpElement)


#pragma endregion

/**
 * Adds the specified image file to the collection.
 *
 * @param mpFilePath  The dicom image file to add.
 * @exception         Throws a System::ArgumentException if the path doesn't exist or is empty.
 */
void ImageCollection::Add(System::String ^ mpFilePath)
{
  if(mpFilePath != System::String::Empty && System::IO::File::Exists(mpFilePath))
  {
    (*m_pImageCollection)->Add(std::dynamic_pointer_cast<dcm::IDcmImage>(dcm::DcmImageFactory::Create(ss(mpFilePath), dcm::PT_FILE)));
  }
  else
  {
    throw gcnew System::ArgumentException(System::String::Format("The specified file was invalid or does not exist.  [FILE={0}", mpFilePath));
  }
} // Add( mpFilePath )


/**
 * Adds all files in the specified directory to the collection.
 *
 * @param mpFolderPath  The dicom folder path.
 * @exception           Throws a System::ArgumentException if the path doesn't exist or is empty.
 */
void ImageCollection::AddDirectory(System::String ^ mpFolderPath)
{
  AddDirectory(mpFolderPath, "*.*");
} // AddDirectory( mpFolderPath )


/**
 * Adds the files conforming to the specified pattern in the specified directory to the image collection.
 *
 * @param mpFolderPath  The dicom folder path to load up.
 * @param mpPattern     The file pattern to accept.
 * @exception           Throws a System::ArgumentException if the path doesn't exist or is empty.
 */
void ImageCollection::AddDirectory(System::String ^ mpFolderPath, System::String ^ mpPattern)
{
  if(mpFolderPath != System::String::Empty && System::IO::Directory::Exists(mpFolderPath))
  {
    for each(System::String ^ mpFilePath in System::IO::Directory::GetFiles(mpFolderPath, mpPattern))
    {
      Add(mpFilePath);
    }
  }
  else
  {
    throw gcnew System::ArgumentException(System::String::Format("The specified directory was invalid or does not exist.  [DIR={0}", mpFolderPath));
  }
} // AddDirectory( mpFolderPath, mpPattern )

#pragma endregion
