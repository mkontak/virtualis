#pragma once



namespace dcm
{

// forward declarations
class DCM_DLL DcmBuffer;


 /**
  * Implements a DICOM LUT (lookup Table) 
  */
class DCM_DLL DcmLut
{
public:

  DcmLut();

  DcmLut(short iNumberOfEntries, short iBitsPerEntry, short iFirstMappedValue );

public:


  /// Creates a new lut to be filled in
  static std::shared_ptr<DcmLut> Create(short iNumberOfEntries, short iBitePerEntry, short iFirstMappedValue );

  /// Creates the lut from the data extracted from the dicom header
  static std::shared_ptr<DcmLut> Create(const std::string & sDescription, std::shared_ptr<DcmBuffer> data);
 

  unsigned char GetMappedValue(unsigned char value, unsigned char defaultValue);
  short GetMappedValue(short pixelValue, short defaultValue);

public:

  const short GetNumberOfEntries() const
  { return m_iNumberOfEntries; }

  const short GetFirstMappedValue() const
  { return m_iFirstMappedValue; }

  const short GetNumberOfBitsPerEntry() const
  { return m_iNumberOfBits; }



  /// Gets the LUT table
  std::shared_ptr<DcmBuffer> & GetData()
  { return m_lutBuffer; }
 
  
private:

  /// Number of entrines in the data table
  short m_iNumberOfEntries;

  /// First mapped pixel value
  short m_iFirstMappedValue;

  /// Number of bits per entry
  short m_iNumberOfBits;

  /// Last mapped value
  short m_iLastMappedValue;

  /// Loojup table OW
  std::shared_ptr<DcmBuffer> m_lutBuffer;

};  // class DcmLut
} // namespace dcm