#include "Stdafx.h"
#include "IDcmImage.h"
#include "DcmCosines.h"
#include "DcmTriple.h"
#include "DcmDate.h"
#include "DcmTime.h"
#include "DcmName.h"
#include "DcmLog.h"
#include "DcmElementCollection.h"
#include "DcmImageCollection.h"
#include "DcmIndexList.h"
#include "DcmIndex.h"


using namespace dcm;


/**
 * constructor
 */
IDcmImage::IDcmImage(bool bIsFrame) :
m_bIsFrame(bIsFrame)
{ 
}

/**
 * Determines if the specified element exists 
 *
 * @return True is found, false if not
 */
bool IDcmImage::Contains(const DcmTag & tag)
{
  std::shared_ptr<dcm::DcmElement> element;

  return TryGetElement(tag, element, true);
} // Contains(const DcmTag & tag)

/**
 * TRries to get the element specified 
 *
 * @param elementTag        Dicom element tag
 * @param searchSubElements Flag indicating we are to search the sub-elements
 *
 * @return True if the element was found, flase if not
 */
bool IDcmImage::TryGetElement(const DcmTag & elementTag, std::shared_ptr<dcm::DcmElement> & element, bool searchSubElements)
{ 
  return TryGetElement(DcmTag(0,0), elementTag, element, searchSubElements); 
} // TryGetElement(const DcmTag & elementTag, std::shared_ptr<DcmElement> & element, bool searchSubElements)


/**
 * Gets the header as xml
 */
std::string IDcmImage::GetHeader()
{ 
  return GetElements()->ToXml();
} // GetHeader()


/**
 * True if the image is monochorme false if RGB
 */
bool IDcmImage::IsMonochrome()
{ 

  bool bIsMonochrome(false);
  unsigned short samples(0);

  if ( TryGetValue(DCM_SamplesPerPixel, samples, true ) )
    bIsMonochrome = samples != 3;
 
  return bIsMonochrome;
} // IsMonochorme()


/**
 * Gets the flag indicating that the image is an encapsulated pdf
 */
bool IDcmImage::IsEncapsulatedPdf() 
{
  std::string uid("");

  return ( TryGetValue(DCM_SOPClassUID,uid) && uid == UID_EncapsulatedPDFStorage );
}


/**
 * Gets the plane for this image
 **/
DcmPlanes IDcmImage::GetPlane()
{ 

  DcmCosines cosines;

  if ( TryGetValue<DcmCosines>(DCM_ImageOrientationPatient, cosines, true) )
    throw DcmInvalidDataException(LogRec("No image orientation element can be found, cannot deduce the plane", "IDcmImage", "GetPlane"));

  return cosines.GetPlane();  
}


/**
 * Gets the study date
 *
 *  @return study date
 */
dcm::DcmDate IDcmImage::GetStudyDate()
{
  std::string sValue("");
  TryGetValue<std::string>(DCM_StudyDate, sValue, true);
  return dcm::DcmDate(sValue);
} // GetStudyDate()


/**
 * Gets the series date
 *
 *  @return series date
 */
dcm::DcmDate IDcmImage::GetSeriesDate()
{
  std::string sValue("");
  TryGetValue<std::string>(DCM_SeriesDate, sValue, true);
  return dcm::DcmDate(sValue);
} // GetStudyDate()


/**
 * Gets the patient namee
 *
 *  @return patient name
 */
DcmName IDcmImage::GetPatientName()
{
  std::string sValue("");
  TryGetValue<std::string>(DCM_PatientName, sValue, true);
  return DcmName(sValue);
} // GetPatientName()


/*
 * Gets the patient id
 *
 *  @return patient id
 */
std::string IDcmImage::GetPatientId()
{
  std::string sValue("");
  TryGetValue<std::string>(DCM_PatientID, sValue);
  return sValue;
} // GetPatientId()


/**
 * Gets the patient DOB
 *
 *  @return the DOB
 */
dcm::DcmDate IDcmImage::GetPatientDob()
{
  std::string sValue("");
  TryGetValue<std::string>(DCM_PatientBirthDate, sValue);
  return dcm::DcmDate(sValue);
} // GetPatientDob()

/**
 * Gets the time zone offset from UTC
 *
 *  Format:   &ZZXX
 *  Where:    &   + or -
 *            ZZ  hours
 *            XX  minutes
 *
 *  @return time zone offset
 */
std::string IDcmImage::GetTimeZoneOffsetFromUTC()
{
  std::string sValue("");
  TryGetValue<std::string>(DCM_TimezoneOffsetFromUTC, sValue, true);
  return sValue;
} // GetTimeZoneOffsetFromUTC()


/**
 * Gets the modality
 *
 *  @return mopdality
 */
std::string IDcmImage::GetModality()
{
  std::string sValue("");
  TryGetValue<std::string>(DCM_Modality, sValue, true);
  return sValue;
} // GetStudyDate()


/**
 * Gets the referring physician name
 *
 * @return the physician name
 */
DcmName IDcmImage::GetReferringPhysician()
{
  std::string name("");
  TryGetValue<std::string>(DCM_ReferringPhysicianName, name);
  return DcmName(name);
} // GetReferringPhysician()

/**
 * Gets the accession number
 *
 *  @return number string
 */
std::string IDcmImage::GetAccessionNumber()
{
  std::string sValue("");
  TryGetValue<std::string>(DCM_AccessionNumber, sValue);
  return sValue;
} // GetAccessionNumber()



/**
 * Gets the study description
 *
 *  @return study description string
 */
std::string IDcmImage::GetStudyDescription()
{
  std::string sValue("");
  TryGetValue<std::string>(DCM_StudyDescription, sValue);
  return sValue;
} // GetStudyDescription()


/**
 * Gets the series description
 *
 *  @return description of series
 */
std::string IDcmImage::GetSeriesDescription()
{
  std::string sValue("");
  TryGetValue<std::string>(DCM_SeriesDescription, sValue);
  return sValue;
} // GetSeriesDescription()



/**
 * Gets the sop instance uid of this dataset
 *
 *  @return the string of Uid
 */
std::string IDcmImage::GetAffectedSopClassUid()
{
  
  std::string sValue("");
 
  /// This value must exists so an exception is thrown
  if ( ! TryGetValue<std::string>(DCM_AffectedSOPClassUID, sValue) )
    throw DcmInvalidDataException(LogRec("No sop class UID found", "IDcmImage", "GetAffectedSopClassUid"));

  return sValue;

} // GetSopInstanceUid()


/**
 * Gets the instance uid of this dataset
 *
 *  @return the string of uid
 */
std::string IDcmImage::GetInstanceUid()
{
  
  std::string sValue("");
 
  if ( ! TryGetValue<std::string>(DCM_SOPInstanceUID, sValue) )
    throw DcmInvalidDataException(LogRec("No instance UID found", "IDcmImage", "GetInstanceUid"));

  return sValue;

} // GetInstanceUid()

/**
 * Gets the study instance uid of this dataset
 *
 *  @return the string of the uid
 */
std::string IDcmImage::GetStudyInstanceUid()
{
  
  std::string sValue("");

  // This value must exist so an exception is thrown
  if ( ! TryGetValue<std::string>(DCM_StudyInstanceUID, sValue) )
    throw DcmInvalidDataException(LogRec("No study instance UID found", "IDcmImage", "GetInstanceUid"));

  return sValue;

} // GetStudyInstanceUid()

/**
 * Gets the series instance uid of this dataset
 *
 *  @return the string of uid
 */
std::string IDcmImage::GetSeriesInstanceUid()
{
  
  std::string sValue;

  // This value must exist so an exception is thrown
  if ( ! TryGetValue<std::string>(DCM_SeriesInstanceUID, sValue) )
    throw DcmInvalidDataException(LogRec("No series instance UID found", "IDcmImage", "GetInstanceUid"));

  return sValue;

} // GetSeriesInstanceUid()

/**
 * Gets the pixel padding value
 *
 *  @return padding value
 */
int IDcmImage::GetPixelPaddingValue()
{
  int iPaddingValue(INT_MAX);
  TryGetValue<int>(DCM_PixelPaddingValue, iPaddingValue);
  return iPaddingValue;
} // GetPixelPaddingValue()

/**
 * Gets the rescale intercept
 *
 *  @return the rescale intercept
 */
float IDcmImage::GetRescaleIntercept()
{
  float fRescaleIntercept(0.0F);
  TryGetValue<float>(DCM_RescaleIntercept, fRescaleIntercept, true);
  return fRescaleIntercept;
} // GetRescaleIntercept()

/**
 * Gets the rescale slope
 *
 *  @return the rescale slope
 */
float IDcmImage::GetRescaleSlope()
{
  float fRescaleSlope(1.0F);
  TryGetValue<float>(DCM_RescaleSlope, fRescaleSlope, true);
  return fRescaleSlope;
} // fRescaleSlope()


/**
 * Gets the rows
 *
 * @return the row index
 */
unsigned int IDcmImage::GetRows()
{
  unsigned int iRows(0);
  TryGetValue<unsigned int>(DCM_Rows, iRows, true);
  return iRows;
} // GetRows()


/**
 * Gets the columns
 *
 *  @return the number of columns
 */
unsigned int IDcmImage::GetColumns()
{
  unsigned int iColumns(0);
  TryGetValue<unsigned int>(DCM_Columns, iColumns, true);
  return iColumns;
} // GetColumns()


/**
 * Gets the number of frames
 *
 *  @return number of frames
 */
unsigned short IDcmImage::GetNumberOfFrames()
{
  unsigned short iValue = 0;
  TryGetValue<unsigned short>(DCM_NumberOfFrames, iValue);
  return iValue;
} // GetNumberOfFrames()



/*
 * Gets the window width 
 *
 *  @return window width
 */
double IDcmImage::GetWindowWidth()
{
  double dWidth(0.0);
  TryGetValue<double>(DCM_WindowWidth, dWidth, true);
  return dWidth;
} // GetWindowsWidth()

/*
 * Gets the window center 
 *
 *  @return window center
 */
double IDcmImage::GetWindowCenter()
{
  double dCenter(0.0);
  TryGetValue<double>(DCM_WindowCenter, dCenter, true);
  return dCenter;
} // GetWindowsCenter()




/*
 * Gets the pixel representation
 *
 *  @return pixel representation
 */
unsigned short IDcmImage::GetPixelRepresentation()
{
  unsigned short iPixelReprensentation(0);
  TryGetValue<unsigned short>(DCM_PixelRepresentation, iPixelReprensentation);
  return iPixelReprensentation;
} // GetPixelRepresentation()

/*
 * Gets the image position
 */
DcmTriple<float> IDcmImage::GetImagePosition()
{
  
  std::string sImagePosition = "0/0/0";
 
  if ( ! TryGetValue<std::string>(DCM_ImagePositionPatient, sImagePosition, true) )
    LogWrn("No image position found", "IDcmImage", "GetImagePosition");


  return DcmTriple<float>(sImagePosition);

} // GetImagePosition()


/**
 * Gets the pixel spacing
 */
DcmDouble<double> IDcmImage::GetPixelSpacing()
{
  
  std::string sPixelSpacing;
 
  if ( ! TryGetValue<std::string>(DCM_PixelSpacing, sPixelSpacing) )
    throw DcmInvalidDataException(LogRec("No pixel spacing  found", "IDcmImage", "GetPixelSpacing"));

  return DcmDouble<double>(sPixelSpacing);

} // GetPixelSpacing()


/**
 * Gets the bits stored
 *
 *  @return bits stored
 */
unsigned short IDcmImage::GetBitsStored()
{
  unsigned short iValue(0);
  TryGetValue<unsigned short>(DCM_BitsStored, iValue);
  return iValue;
} // GetBitsStored()



/**
 * Gets the bits allocated
 */
unsigned short IDcmImage::GetBitsAllocated()
{
  unsigned short iValue(0);
  TryGetValue<unsigned short>(DCM_BitsAllocated, iValue);
  return iValue;
} // GetBitsAllocated()


/**
 * Gets the image orientation
 *
 *  @return the cosines
 */
DcmCosines IDcmImage::GetImageOrientation()
{
  
  std::string sImageOrientation("");
 
  if ( ! TryGetValue<std::string>(DCM_ImageOrientationPatient, sImageOrientation, true) )
    DcmLog::Warning("No image orientation found", "IDcmImage", "GetImageOrientation");

  return DcmCosines(sImageOrientation);

} // GetImageOrientation()


/**
 * Gets the high bit value
 *
 *  @return the high bit value
 */
unsigned short IDcmImage::GetHighBit()
{
  unsigned short iValue(0);
  TryGetValue<unsigned short>(DcmTag(0x0028, 0x0102), iValue);
  return iValue;
} // GetHighBit()


/**
 * Gets the image type
 *
 *  @return the string of image type
 */
std::string IDcmImage::GetImageType()
{
  std::string sImageType("");
  TryGetValue<std::string>(DCM_ImageType,sImageType);
  return sImageType;
} // GetImageType()




/**
 * Gets the Institution
 *
 *  @return the string of the name
 */
std::string IDcmImage::GetInstitutionName()
{
  std::string sInstitution("");
  TryGetValue<std::string>(DCM_InstitutionName, sInstitution);
  return sInstitution;
} // GetInstitutionName()

/**
 * Gets the manufacturer
 *
 *  @return the string of the vendor
 */
std::string IDcmImage::GetManufacturer()
{
  std::string sManufacturer("");
  TryGetValue<std::string>(DCM_Manufacturer, sManufacturer);
  return sManufacturer;
} // GetManufacturer()



/**
 * Gets the manufacturer model
 *
 *  @return the string of the model
 */
std::string IDcmImage::GetManufacturerModel()
{
  std::string sModel("");
  TryGetValue<std::string>(DCM_ManufacturerModelName, sModel);
  return sModel;
} // GetManufacturerModel()


/**
 * Gets the sampls per pixel
 */
unsigned short IDcmImage::GetSamplesPerPixel()
{
  unsigned short iValue(1);
  TryGetValue<unsigned short>(DCM_SamplesPerPixel, iValue);
  return iValue;

} // GetSamplesPerPixel()

/**
 * Gets the patient position
 *
 *  @return patient position
 */
std::string IDcmImage::GetPatientPosition()
{
  std::string sPosition("");
  TryGetValue<std::string>(DCM_PatientPosition, sPosition);
  return sPosition;
} // GetPatientPosition()


/**
 * Gets the study time
 *
 *  @return study time
 */
dcm::DcmTime IDcmImage::GetStudyTime()
{
  std::string sTime;
  TryGetValue<std::string>(DCM_StudyTime, sTime);
  return dcm::DcmTime(sTime);
} // GetStudyTime()


/**
 * Gets the series time
 *
 *  @return series time
 */
dcm::DcmTime IDcmImage::GetSeriesTime()
{
  std::string sTime;
  TryGetValue<std::string>(DCM_SeriesTime, sTime);
  return dcm::DcmTime(sTime);
} // GetSeriesTime()


/**
 * Gets the concatenated number
 */
unsigned short IDcmImage::GetConcatenatedNumber()
{
  unsigned short iNumber(0);
  TryGetValue<unsigned short>(DCM_InConcatenationNumber,iNumber);
  return iNumber;
} // GetConcatenatedNumber()


/**
 * Gets the number of concatentaed images
 */
unsigned short IDcmImage::GetNumberOfConcatenatedImages()
{
  unsigned short iCount(0);
  TryGetValue<unsigned short>(DCM_InConcatenationTotalNumber,iCount);
  return iCount;

} // GetNumberOfConcatenatedImages()


/**
 * Gets the concatenation uid
 */
std::string IDcmImage::GetConcatenatedUid()
{
  std::string uid;
  TryGetValue<std::string>(DCM_ConcatenationUID,uid);
  return uid;

} // GetConcatenationUid()


/**
 * Gets the number of concatentaed images
 */
unsigned short IDcmImage::GetConcatenatedFrameOffsetNumber()
{
  unsigned short iNumber(0);
  TryGetValue<unsigned short>(DCM_ConcatenationFrameOffsetNumber,iNumber);
  return iNumber;

} // GetConcatenatedFrameOffsetNumber()


/**
 * Gets the pixel data from the row/col specified
 * 
 * @param x      x axis represents the column
 * @param y      y axis represents the row
 *
 * @retrn Pixel value
 */
short IDcmImage::GetPixelValue(unsigned int x, unsigned int y)
{

  unsigned int rows(GetRows());
  unsigned int columns(GetColumns());

  std::shared_ptr<DcmBuffer> pixelData(GetPixelData());

  if ( pixelData->IsEmpty() )
    throw DcmInvalidDataException(LogRec("No pixel data found","IDcmImage","GetPixelValue"));

  // Check the x,y coordinates against the row and columns
  if ( x < 0 || x > columns || y < 0 || y > rows )
    throw DcmOutOfBoundsException(LogRec("The specified x and y are not within the images bounds","DcmImage","GetPixelValue"));

  size_t iOffset( (y * columns) + x ); 
  
  return pixelData->GetValue<short>(iOffset);


} // GetPixelValue(unsigned int iRow, unsigned int iCol)


/**
 * Determines if the the specified index list is contained in the image
 *
 * @param indexes       Index list
 *
 * @return True is the image contains the all the indexes, false if evene one index is missing
 */
bool IDcmImage::ContainsIndexes(std::shared_ptr<DcmIndexList> & indexes)
{

  bool bContains( true );


  std::shared_ptr<DcmElement> element;

  for ( int i(0); i < indexes->Count() && bContains ; i++ )
  {
    bContains = TryGetElement(indexes->GetIndex(i)->GetFunctionalGroup(), indexes->GetIndex(i)->GetIndexElement(), element, true );
  }

  if ( bContains && indexes->HasMultiValuedIndex() )
    bContains = TryGetElement(indexes->GetMultiValuedIndex()->GetFunctionalGroup(), indexes->GetMultiValuedIndex()->GetIndexElement(), element, true );

  return bContains;

} // Contains