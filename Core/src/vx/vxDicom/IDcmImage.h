#pragma once

#include "DcmEnums.h"
#include "DcmExceptions.h"
#include "DcmBuffer.h"
#include "DcmElementCollection.h"

namespace dcm
{


#pragma region forward declarations

// Forward declarations

/// Unit cosines
class DCM_DLL DcmCosines;

/// Triple
template<class T>
class DCM_DLL DcmTriple;

/// Double
template<class T>
class DCM_DLL DcmDouble;

/// Date
class DCM_DLL DcmDate;

/// Time
class DCM_DLL DcmTime;

/// Name
class DCM_DLL DcmName;

/// Element
class DCM_DLL DcmElement;

// Sequence
class DCM_DLL DcmSequence;

/// Dicom Image Collection
class DCM_DLL DcmImageCollection;

/// Dicom buffer
class DCM_DLL DcmBuffer;

/// Index list
class DCM_DLL DcmIndexList;

#pragma endregion



/**
 * Implements a base abstract image class that is used by the DcmImage and DcmFrame. Used the 
 * DcmImageCollection so that we may have collection of regular images or frames.
 */
class DCM_DLL IDcmImage
{
protected:

  /// constructor
  IDcmImage(bool bIsFrame);

public:

  /// Clone this imaage
  virtual std::shared_ptr<IDcmImage> Clone() = 0;


  /// Gets the dicom header as XML
  virtual std::string GetHeader();

  /// Gets the indexes associaed with the image (DimesionalOrganizational or FrameContentSequence)
  virtual std::shared_ptr<DcmIndexList> GetIndexes() = 0;

  /// Returns true if the image contains the specified indexes.
  virtual bool ContainsIndexes(std::shared_ptr<DcmIndexList> & indexes);

  /// Equality
  virtual bool Equals(const std::shared_ptr<IDcmImage> & image) = 0;

  /// Determines if the sepcified element exists 
  virtual bool Contains(const DcmTag & tag);

#pragma region TryGetValue Methods

public:

  /// Try and get the element
  virtual bool TryGetElement(const DcmTag & elementTag, std::shared_ptr<dcm::DcmElement> & element, bool searchSubElements = false);

  /// Try and get the element
  virtual bool TryGetElement(const DcmTag & sequenceTag, const DcmTag & elementTag, std::shared_ptr<dcm::DcmElement> & element, bool searchSubElements = false) = 0;


  /// TryGetValue() that will convert the string representation to the type specified
  template<class T>
  bool TryGetValue(const DcmTag & tag, T & value, bool searchSubElements = false )
  {

    std::string str;
    bool found(TryGetString(tag, str, searchSubElements));

    if ( found )
    {
      std::istringstream is(str.c_str());
      is >> value;
    }

    return found;
   
  } // TryGetValue(const DcmTag & tag, T & value, bool searchSubElements = false )

    /// Try and get string value
  template<class T>
  bool TryGetValue(const DcmTag & tag, std::string & value , bool searchSubElements = false )
  {
    return TryGetString(tag, value, searchSubElements);
  } // TryGetValue(unsigned short iGroup, unsigned short iElement, std::string & sValue)

    /// TryGetValue
  template<class T>
  bool TryGetValue(const DcmTag & tag, std::shared_ptr<DcmBuffer> & value, bool searchSubElements = false )
  {
    return TryGetBuffer(tag, value, searchSubElements);
  } // TryGetValue(unsigned short iGroup, unsigned short iElement, const std::string & sPrivateCreator, T value )

protected:

  /// Base TryGetValue() abstract implemented in the main class
  virtual bool TryGetString(const DcmTag & tag, std::string & value, bool searchSubElements = false ) = 0;

  /// Try an get a OB or OW elements
  virtual bool TryGetBuffer(const DcmTag & tag, std::shared_ptr<DcmBuffer> & dcmBuffer, bool searchSubElements = false  ) = 0;


#pragma endregion


#pragma region properties

public:

    /// Gets the pixel value at the specified row/column
  short GetPixelValue(unsigned int iRow, unsigned int iCol);

   /// Gets a list of elements for this object
  virtual std::shared_ptr<DcmElementCollection> & GetElements() = 0;
 
  /// Returns true if the object is a frame
  const bool IsFrame() const
  { return m_bIsFrame; }

  /// True if the image is monochorme false if RGB
  virtual bool IsMonochrome();
  
  /// Gets the flag indicating that the image is an encapsulated pdf
  virtual bool IsEncapsulatedPdf();

  /// Gets the study date
  virtual dcm::DcmDate GetStudyDate();

  /// Gets the series date
  virtual dcm::DcmDate GetSeriesDate();

  /// Gets the patient name
  virtual DcmName GetPatientName();

  /// Gets the patient id
  virtual std::string GetPatientId();

  /// Gets the medical id
  virtual inline std::string GetMedicalId()
  { return GetPatientId(); }

  /// Gets the patients birth date
  virtual dcm::DcmDate GetPatientDob();

  /// Gets the time zone offset from utc
  virtual std::string GetTimeZoneOffsetFromUTC(); 

  /// Gets the modality
  virtual std::string GetModality(); 

  /// Get the referring physician name
  virtual DcmName GetReferringPhysician();

  /// Gets the accession number
  virtual std::string GetAccessionNumber();

  /// Gets the  SOP instance uid
  virtual std::string GetInstanceUid();

  /// Gets the affected SOP class uid
  virtual std::string GetAffectedSopClassUid();

  /// Gets the study instance uid
  virtual std::string GetStudyInstanceUid();

  /// Gets the series instance uid
  virtual std::string GetSeriesInstanceUid();

  /// Gets the study description
  virtual std::string GetStudyDescription(); 

  /// Gets the series description
  virtual std::string GetSeriesDescription(); 

  /// Get the pixel padding value
  virtual int GetPixelPaddingValue();

  /// Gets the rescale intercept
  virtual float GetRescaleIntercept();

  /// Gets the rescale slope
  virtual float GetRescaleSlope();

  /// Gets the rows
  virtual unsigned int GetRows();

  /// Gets the columns
  virtual unsigned int GetColumns();

  /// Gets the number of frames
  virtual unsigned short GetNumberOfFrames();

  /// Gets the windows width
  virtual double GetWindowWidth();

  /// Gets the windows center
  virtual double GetWindowCenter();

  /// Gets the pixel representation
  virtual unsigned short GetPixelRepresentation();

  /// Gets the image position
  virtual DcmTriple<float> GetImagePosition();

  /// Gets the pixel spacing
  virtual DcmDouble<double> GetPixelSpacing();

  /// Gets the bits stored value
  virtual unsigned short GetBitsStored();

  /// Gets the bits allocated value
  virtual unsigned short GetBitsAllocated();

  /// Gets the image orientation (cosines)
  virtual DcmCosines GetImageOrientation();

  /// Gets the high bit value
  virtual unsigned short GetHighBit();

  /// Gets the plane for this image
  virtual DcmPlanes GetPlane();

  /// Gets the image type
  virtual std::string GetImageType();

  /// Gets the institution name
  virtual std::string GetInstitutionName();

  /// Gets the manufacturer name
  virtual std::string GetManufacturer();

  /// Gets the manufacturer model name
  virtual std::string GetManufacturerModel();

  /// Gets the samples per pixel
  virtual unsigned short GetSamplesPerPixel();

  /// Gets the patient position
  virtual std::string GetPatientPosition();

  /// Gets the study time
  virtual dcm::DcmTime GetStudyTime();

  /// Gets the series time
  virtual dcm::DcmTime GetSeriesTime();

  /// Determines if this image is an concatenated image
  virtual bool IsConcatenated()
  { return false; }

  /// Gets the concatenated number
  virtual unsigned short GetConcatenatedNumber();

  /// Gets the number of concatentaed images
  virtual unsigned short GetNumberOfConcatenatedImages();

  /// Gets the concatenated frame offset number
  virtual unsigned short GetConcatenatedFrameOffsetNumber();

  /// Gets the  SOP instance uid
  virtual std::string GetConcatenatedUid();

  // Get the MIN/MAX pixel value
  template<typename T>
  void GetMinMaxPixelValues(std::pair<T,T> & minMax)
  {
    int iPaddingValue(INT_MAX);
    if ( TryGetValue<int>(DCM_PixelPaddingValue, iPaddingValue) )
      return GetPixelData()->GetMinMaxValues<T>(minMax, (T)iPaddingValue, true);
    else
      return GetPixelData()->GetMinMaxValues<T>(minMax);
  }


  // ===================================================
  // Gets the image size using the type specified 
  //    Types:    unsigned char
  //              unsigned short
  //              short
  // ===================================================
  template<class T>
  unsigned int GetImageSize()
  {
    unsigned int rows(0);
    unsigned int columns(0);
    const unsigned short samples(GetSamplesPerPixel());
    unsigned short allocated(0);


    // ===========================================================================================================
    // Get the rest of the data needed to calculate the image size (These items must be present or I will throw
    // an exception.
    // =============================================================================================================
    if (  TryGetValue<unsigned int>(DCM_Rows, rows, true ) && 
          TryGetValue<unsigned int>(DCM_Columns, columns, true ) &&
          TryGetValue<unsigned short>(DCM_BitsAllocated, allocated, true ) )
      return rows * columns * samples * ( allocated / ( sizeof(T) * 8 ) );
    else
      throw DcmInvalidDataException(LogRec("Invalid data missing rows, columns or bits allocated, cannot calculate image size", "IDcmImage", "GetImageSize"));

  } // GetImageSize()


  /// Gets the pixel data
  virtual std::shared_ptr<DcmBuffer> GetPixelData() = 0;

  virtual const std::string & GetFilename()  = 0;
  

#pragma endregion

protected:



  /// Friend
  friend class DCM_DLL DcmImageFactory;

private:


  /// Falg indicating if the image is a frame
  bool m_bIsFrame;


};  // class IDcmImage

} // namespace dcm