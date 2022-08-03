// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmImage.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

#include "DcmEnums.h"
#include "DcmBuffer.h"
#include "IDcmImage.h"



#pragma warning(push)
#pragma warning(disable:4251)


// Forward declarations (global namespace)
struct _xmlNode;

namespace dcm
{

#pragma region forward declarations

// Forward declarations

// Frame
class DCM_DLL DcmFrame;

/// Unit cosines
class DCM_DLL DcmCosines;

/// Triple
template<class T>
class DCM_DLL DcmTriple;

/// Double
template<class T>
class DCM_DLL DcmDouble;

/// Date
class DCM_DLL dcm::DcmDate;

/// Time
class DCM_DLL dcm::DcmTime;

/// Name
class DCM_DLL DcmName;

/// Element list
class DCM_DLL DcmElementCollection;

/// Element
class DCM_DLL DcmElement;

// Sequence
class DCM_DLL DcmSequence;

/// Dicom Image Collection
class DCM_DLL DcmImageCollection;

#pragma endregion

// Stack ID/Image List sorted by InStackPositionNumber 
typedef   std::pair<std::string, std::shared_ptr<dcm::DcmImageCollection>> DcmMultiFrameCollectionItem;
typedef   std::map<std::string, std::shared_ptr<dcm::DcmImageCollection>> DcmMultiFrameCollection;


/**
 * Implements a DICOM image containing the header and the pixel data
 */
class DCM_DLL DcmImage : public IDcmImage
{

#pragma region constructors/destructors

protected:

  /// Constructor ( If bEmpty is true no initial values are set)
  DcmImage(bool bEmpty = false);

  /// Constructor (Used for cloning)
  DcmImage(DcmDataset * pDcmDataset);

  /// Copy constructor
  DcmImage(DcmImage & image) : IDcmImage(false) {}

public:

  /// Destructor
  virtual ~DcmImage();

#pragma endregion 

#pragma region methods


#pragma region CreateFrame methods

protected:

  /// Creates a frame for this image
  inline std::shared_ptr<DcmFrame> CreateFrame(unsigned int iFrameNumber, std::shared_ptr<DcmElementCollection> & frameElements)
  { return CreateFrame(iFrameNumber, frameElements, std::shared_ptr<DcmBuffer>(__nullptr) ); }

  /// Creates a frame for this image
  std::shared_ptr<DcmFrame> CreateFrame(unsigned int iFrameNumber, std::shared_ptr<DcmElementCollection> & frameElements, std::shared_ptr<DcmBuffer> & pixelData);

public:

    /// Creates a frame for this image
  std::shared_ptr<DcmFrame> CreateFrame(std::shared_ptr<DcmElementCollection> & frameElements, std::shared_ptr<DcmBuffer> & pixelData);

#pragma endregion

public:

  /// Clone
  virtual std::shared_ptr<IDcmImage> Clone();

  /// Equality is based on the instance UID
  virtual bool Equals(const std::shared_ptr<IDcmImage> & image)
  { return GetInstanceUid() == image->GetInstanceUid(); }

  // Saves the image to the specified file
  void Save(const std::string & sFilename);

  /// Saves the header as xml
  void SaveHeader(const std::string & sFilename);
  
  /// Removes the specified element using the tag
  void Remove(const DcmTag & tag);

  /// Removes the specified group's elements from the dataset
  void Remove(unsigned short iGroup);

  /// Clears all element from the list
  void Clear();

  /// Gets the dicom header as XML
  virtual std::string GetHeader(bool bStripControlChracters = false);


  /// Set the pixel data 
  void SetPixelData(const std::shared_ptr<Gdiplus::Bitmap> & pImage,  const std::shared_ptr<DcmBuffer> & pPayLoad = std::shared_ptr<DcmBuffer>(__nullptr) );


  /// Gets a list of elements in the DcmImage
  virtual std::shared_ptr<DcmElementCollection> & GetElements();

  /// Gets a list of per frame elements in the DcmImage
  virtual DcmElementCollectionList & GetFrameElements();


  /// Gets the frames for the image if there are any
  virtual const std::shared_ptr<DcmImageCollection> & GetFrames();

  /// Gets the DimensionalOragnizationalSequence or indexes
  virtual std::shared_ptr<DcmIndexList> GetIndexes();

  /// Adds or Updates the element
  virtual void Insert(std::shared_ptr<dcm::DcmElement> & element);

public:

 /// Loads the dicom file into the DcmImage ( returns true if success, false if failed )
  bool Load(const std::string & sFilename, bool bThrowException = true);



 protected:

  /// Determines if the tag is included in the modification process. This is virtual to allow for an override if we have a unique image type
  virtual bool IsIncluded(const DcmTag & tag);

  /// Loads the element list
  void Load(const  std::shared_ptr<dcm::DcmElementCollection> & dcmElementList); 



#pragma endregion
 



#pragma region TryGet Methods

public:

  /// Try and get the element
  virtual bool TryGetElement(const DcmTag & sequenceTag, const DcmTag & elementTag, std::shared_ptr<DcmElement> & element, bool searchSubElement = false );


  // Try and get a DcmBuffer
  virtual bool TryGetBuffer(const DcmTag & tag, std::shared_ptr<DcmBuffer> & dcmBuffer,bool searchSubElements = false  );

protected:

  /// Try and get string value
  bool TryGetString(const DcmTag & tag, std::string & value , bool searchSubElements = false );



#pragma endregion


#pragma region SetValue Methods

public:

  /// TryGetValue
  template<class T>
  void SetValue(const DcmTag & tag, T value )
  {
    std::ostringstream os;
    os << value;
    SetValue<std::string>(tag, os.str());
  } // SetValue<std::string>(unsigned short iGroup, unsigned short iElement, const std::string & sPrivateCreator, T value )


  /// Sets the buffer value
  template<class T>
  void SetValue(const DcmTag & tag, const std::shared_ptr<DcmBuffer> & pDcmBuffer )
  {
 

    OFCondition cond = m_pDcmDataset->putAndInsertUint8Array(tag, pDcmBuffer->GetData<unsigned char>(), pDcmBuffer->GetSize() );

    if ( cond.bad() )
      throw DcmException(LogRec(util::Strings::Format("Failed to set buffer value for (%04x,%04x)", tag.getGroup(), tag.getElement()),"DcmImage", "SetValue"));

  } // SetValue<std::string>(unsigned short iGroup, unsigned short iElement, std::shared_ptr<DcmBuffer> & pDcmBuffer )



  /// Sets the value of the specified element
  template<class T>
  void SetValue(const DcmTag & tag, const std::string & sValue )
  {

    if ( sValue.size() > 0 )
    {
      OFCondition cond = m_pDcmDataset->putAndInsertString(tag, sValue.c_str() );
                               
      if ( cond.bad() )
        throw DcmInvalidDataException(LogRec(util::Strings::Format("Failed to set string value  (%04x,%04x): %s", tag.getGroup(), tag.getElement(), cond.text()), "DcmImage", "SetValue"));
    }
    else
      SetEmptyValue(tag);
 
  } // END ... SetValue<std::string>(unsigned short iGroup, unsigned short iElement, const std::string & sValue )


    /// TryGetValue
  template<class T>
  void SetValue(const DcmTag & tag, unsigned short value )
  {

    OFCondition cond = m_pDcmDataset->putAndInsertUint16(tag, value );
                               
    if ( cond.bad() )
      throw DcmInvalidDataException(LogRec(util::Strings::Format("Failed to set string value (%04x,%04x): %s", tag.getGroup(), tag.getElement(), cond.text()), "DcmImage", "SetValue"));
   
  } // SetValue<std::string>(unsigned short iGroup, unsigned short iElement, T value )


  // ==========================
  // Set the value as empty
  // =========================
  void SetEmptyValue(const DcmTag & tag)
  {
    OFCondition cond = m_pDcmDataset->insertEmptyElement(tag);
                               
    if ( cond.bad() )
      throw DcmInvalidDataException(LogRec(util::Strings::Format("Failed to set empty value (%04x,%04x): %s", tag.getGroup(), tag.getElement(), cond.text()), "DcmImage", "SetValue"));

  } // SetEmptyValue()


#pragma endregion




#pragma region Xml Loading Methods

private:


  /// Attempts the load the xml node (Calls one or more of the methods below )
  void LoadXml( std::shared_ptr<DcmImage> & image, const xml::XmlNode & node );

  /// Loads the patient node or the node that contains patient data
  void LoadPatientNode( std::shared_ptr<DcmImage> & image, const  xml::XmlNode & node );

  /// Loads the study node or the node that contains study data
  void LoadStudyNode( std::shared_ptr<DcmImage> & image, const  xml::XmlNode & node );

  /// Load the series node or node that contains series data
  void LoadSeriesNode( std::shared_ptr<DcmImage> & image, const  xml::XmlNode & node );

  /// Load data-set node
  void LoadDatasetNode( std::shared_ptr<DcmImage> & image, const  xml::XmlNode & node );

  /// Loads all the element nodes found in the supplied node
  void LoadElements( std::shared_ptr<DcmImage> & image, const  xml::XmlNode & node  );

  /// Loads all the sequence nodes found in the supplied node
  void LoadSequences( std::shared_ptr<DcmImage> & image, const xml::XmlNode & node );

#pragma endregion

 
  /// Load elements from collection
  void LoadElements(std::shared_ptr<DcmElementCollection> & elements);


#pragma region properties

public:

  /// Determines if this image is an concatenated image
  virtual bool IsConcatenated();

  /// Gets the filename associated with this image
  virtual const std::string & GetFilename()
  { return m_sFilename; }

  /// Gets the Retrieval ae title
  std::string GetRetrievalAETitle();

  /// Sets the Retrieval ae title
  void SetRetrievalAEtitle(const std::string & sAETitle);

  /// Sets the image type
  void SetImageType(const std::string & sImageType);


  /// Sets the photometric interpretation
  void SetPhotometricInterpretation(const std::string & sPhotometricInterpretation);

  /// Gets the photometric interpretation
  virtual std::string GetPhotometricInterpretation();

  /// Sets the high bit
  void SetHighBit(unsigned short iHighBit);

  /// Sets the bits stored
  void SetBitsStored(unsigned short iBitsStored);


  /// Sets the bits allocated
  void SetBitsAllocated(unsigned short iBitsAllocated);


  /// Sets the pisel spacing
  void SetPixelSpacing(DcmDouble<double> pixelSpacing);


  /// Sets the instance uid
  void  SetInstanceUid(const std::string & sUid);


  /// Sets the sop instance uid
  void SetAffectedSopClassUid(const std::string & sUid);


  /// Sets the study instance uid
  void  SetStudyInstanceUid(const std::string & sUid);

  /// Sets the series instance uid
  void  SetSeriesInstanceUid(const std::string & sUid);


  /// Sets the institution
  void  SetInstitutionName(const std::string & sInstitution);

  /// Sets the manufacturer
  void  SetManufacturer(const std::string & sManufacturer);


  /// Sets the manufacturer model
  void  SetManufacturerModel(const std::string & sModel);

  /// Gets the manufacturer model
  virtual unsigned int GetInstanceNumber();

  /// Sets the instance number
  void  SetInstanceNumber(unsigned int iInstanceNumber);

  /// Sets the rescale intercept
  void  SetRescaleIntercept(float fRecaleIntecept);

  /// Sets the rescale slope
  void  SetRescaleSlope(float fRecaleSlope);

  /// Sets the rows
  void  SetRows(unsigned int iRows);


  /// Sets the rows
  void  SetColumns(unsigned int iColumns);


  /// Gets the additional patient history
  virtual std::string GetAdditionalPatientHistory();

  /// Sets the additional patient history
  void  SetAdditionalPatientHistory(const std::string & sPatientHistory);

  /// Gets the operator name
  virtual std::string GetOperatorName();

  /// Sets the operator name
  void  SetOperatorName(const std::string & sOperatorName);


  /// Sets the accession number
  void SetAccessionNumber(const std::string & sNumber);


  /// Sets the study description
  void SetStudyDescription(const std::string & sDescription);


  /// Sets the series description
  void SetSeriesDescription(const std::string & sDescription);


  /// Sets the referring physician
  void SetReferringPhysician(const std::string & sPhysician);


  /// Sets the patient position
  void SetPatientPosition(const std::string & sPosition);

  /// Gets the patient sex
  virtual std::string GetPatientSex();

  /// Sets the patient sex
  void SetPatientSex(const std::string & sSex);

  /// Gets the patient DOB
  virtual dcm::DcmDate GetPatientDob();

  /// Sets the patient DOB
  void SetPatientDob(const dcm::DcmDate & dob);
 
  /// Gets the patient TOB
  virtual dcm::DcmTime GetPatientTob();

  /// Sets the patient TOB
  void SetPatientTob(const dcm::DcmTime & tob);

  /// Gets the KVP
  virtual float GetKvp();

  /// Sets the KVP
  void SetKvp(float fKvp);

  /// Gets the Xray Tube Current
  virtual float GetXrayTubeCurrent();

  /// Sets the KVP
  void SetXrayTubeCurrent(float fCurrent);


  /// Sets the slice thickness
  void SetSliceThickness(float fThickness);

  /// Gets the Xray Tube Current
  virtual float GetSliceThickness();

  /// Sets the patient weight
  void SetPatientWeight(int iWeight);

  /// Gets the patient weight
  virtual float GetPatientWeight();

   /// Sets the patient age
  void SetPatientAge(const std::string & sAge);

  /// Gets the patient age
  virtual std::string GetPatientAge();

  /// Gets the pixel data
  virtual std::shared_ptr<DcmBuffer> GetPixelData();

  /// Gets frame 
  std::shared_ptr<DcmBuffer> GetFrame(int iFrameIndex);


  /// Sets the pixel data
  void SetPixelData(const std::shared_ptr<DcmBuffer> & pixelData);


   /// Sets the reconstruction diameter
  void SetReconstructionDiameter(float fDiameter);

  /// Gets the reconstruction diameter
  virtual float GetReconstructionDiameter();

  /// Gets the patient id 
  virtual std::string GetPatientId();

  /// Sets the patient id
  void SetPatientId(const std::string & sPatientId);

  /// Sets the medical id
  inline void SetMedicalId(const std::string & sMedicalId)
  { SetPatientId(sMedicalId); } 


  /// Sets the samples per pixel
  void SetSamplePerPixel(unsigned short iSamples);


  /// Sets the modality
  void SetModality(const std::string & sModality);

  /// Gets the study id
  virtual std::string GetStudyId();

  /// Sets the study Id
  void SetStudyId(const std::string & sId);

    /// Gets the series number 
  virtual std::string GetSeriesNumber();

  /// Sets the series Number
  void SetSeriesNumber(const std::string & sNumber);

  /// Sets the patient name
  void SetPatientName(const std::string & sPatientName);
 
  /// Gets the gantry detector tilt
  virtual float GetGantryDetectorTilt();

  /// Sets the gantry detector tilt
  void SetGantryDetectorTilt(float fTilt);
   

  /// Sets the study date
  void SetStudyDate(const dcm::DcmDate & date);

  /// Sets the study time
  void SetStudyTime(const dcm::DcmTime & time);

  /// Sets the series date
  void SetSeriesDate(const dcm::DcmDate & date);


  /// Sets the series time
  void SetSeriesTime(const dcm::DcmTime & time);

  /// Gets the acquisition time
  virtual dcm::DcmDate GetAcquisitionDate();

  /// Sets the acquisition date
  void SetAcquisitionDate(const dcm::DcmDate & date);

  /// Gets the acquisition time
  virtual dcm::DcmTime GetAcquisitionTime();

  /// Sets the acquisition time
  void SetAcquisitionTime(const dcm::DcmTime & time);

  /// Gets the image time
  virtual dcm::DcmDate GetImageDate();

  /// Sets the image date
  void SetImageDate(const dcm::DcmDate & date);

  /// Gets the image time
  virtual dcm::DcmTime GetImageTime();

  /// Sets the image time
  void SetImageTime(const dcm::DcmTime & time);

  /// Gets the acquisition number
  virtual __int64 GetAcquisitionNumber();

  /// Sets the pixel padding value
  void SetAcquisitionNumber(__int64 iNumber);

  /// Sets the pixel padding value
  void SetPixelPaddingValue(unsigned short iPaddingValue);

  /// Sets the number of frames
  void SetNumberOfFrames(unsigned short iFrames);

  /// Sets the samples per pixel
  void SetSamplesPerPixel(unsigned short iSamples);
 
  /// Gets the query retrieval level
  std::string GetQueryRetrievalLevel();

  /// Sets the query retrieval level
  void  SetQueryRetrievalLevel(const std::string & sLevel);
 
  /// Sets the pixel representation
  void SetPixelRepresentation(unsigned short iPixelRepresentation);

  /// Sets the window width
  void SetWindowWidth(double dWidth);

  /// Sets the windows center
  void SetWindowCenter(double dCenter);

   /// Gets the position of this image (Relative to the plane)
  virtual float GetPosition();

protected:

    //// Gets the DcmDataset
  DcmDataset * GetDcmDataset()
  { return m_pDcmDataset; }

  // Clears the cached elements
  inline void ClearCachedElements()
  { m_elements = __nullptr; m_frameElements.clear(); }

#pragma endregion properties


#pragma region fields

protected:

  // Contains all tags that should be included in the loading
  std::vector<DcmTag> m_inclusion;

  // Contains tags that should not be loaded
  std::vector<DcmTag> m_exclusion;

private:

  /// Filename associated with the image
  std::string m_sFilename;

  /// File format
  DcmFileFormat m_fileFormat;

  /// Dataset 
  DcmDataset * m_pDcmDataset;

  /// main elements (Conatins the frame elements)
  std::shared_ptr<DcmElementCollection> m_elements;

  /// collection of frame elements (PerFrameSequence)
  DcmElementCollectionList m_frameElements;

  /// Frames collection
  std::shared_ptr<DcmImageCollection> m_frames;

#pragma endregion


#pragma region friends

protected:


  friend class DCM_DLL DcmAssociation;
  friend class DCM_DLL DcmClient;
  friend class DCM_DLL DcmFind;
  friend class DCM_DLL DcmImageFactory;


#pragma endregion friends


};  // class DcmImage




};  // namespace dcm


#pragma warning(pop)

