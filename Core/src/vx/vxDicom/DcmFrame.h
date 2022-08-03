#pragma once

#include "IDcmImage.h"


namespace dcm
{

 // Forward declarations
class DCM_DLL DcmElementCollection;
class DCM_DLL DcmImage;
class DCM_DLL DcmImageCollection;

/**
 * Implements a DICOM frame image
 */
class DCM_DLL DcmFrame : public IDcmImage
{

#pragma region constructors

protected:

  /// Default constructor
  DcmFrame();

  /// Constrcutor
  DcmFrame(unsigned int iFrameNumber, DcmImage * pDcmImage, std::shared_ptr<DcmElementCollection> & frameElements);

  /// Copy constructor (not implemented)
  DcmFrame(const DcmFrame & frame) : IDcmImage(true) { }

#pragma endregion


#pragma region TryGet methods

public:

  // Try and get a DcmBuffer
  virtual bool TryGetBuffer(const DcmTag & tag, std::shared_ptr<DcmBuffer> & dcmBuffer,bool searchSubElements = false  );

  /// Try and get the element
  virtual bool TryGetElement(const DcmTag & sequenceTag, const DcmTag & elementTag, std::shared_ptr<DcmElement> & element, bool searchSubElement = false );

protected:

  /// Try get a string representation of the element specified
  virtual bool TryGetString(const DcmTag & tag, std::string & str, bool searchSubelements = false);

#pragma endregion

public:


  /// CLones the frame
  virtual std::shared_ptr<IDcmImage> Clone();

  /// Equality is based on the instance UID and FrameNumber
  virtual bool Equals(const std::shared_ptr<IDcmImage> & image)
  { return ( image->IsFrame() && (GetInstanceUid() == image->GetInstanceUid()) && 
            ( GetFrameNumber() == std::dynamic_pointer_cast<DcmFrame>(image)->GetFrameNumber() ) ); }


#pragma region properties

public:

  /// Returns the frame number
  const unsigned int GetFrameNumber() const
  { return m_iFrameNumber; }

  /// Get the indexes
  virtual std::shared_ptr<DcmIndexList> GetIndexes();

  /// Get the associated file name for this element
  virtual const std::string & GetFilename();

  /// Get the elements for this image
  virtual std::shared_ptr<DcmElementCollection> & GetElements();

  /// Get the frames pixel data
  virtual std::shared_ptr<DcmBuffer> GetPixelData()
  { return m_pixelData; }

  /// Sets the frames pixel data
  virtual void SetPixelData(std::shared_ptr<DcmBuffer> buffer)
  { m_pixelData = buffer; }

#pragma endregion


#pragma region fields

private:

  // frame number
  unsigned short m_iFrameNumber;

  // frame index (Implied)
  unsigned short m_iFrameIndex;

  // Frames pixel data
  std::shared_ptr<DcmBuffer> m_pixelData;

  /// Parent image
  DcmImage * m_pDcmImage;

#pragma endregion


#pragma region friends

  /// Friend
  friend class DCM_DLL DcmImageFactory;
  friend class DCM_DLL DcmImage;
  friend class DCM_DLL DcmImageCollection;

#pragma endregion

}; // class DcmFrame

} // namespace dcm