// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: PdfBuffer.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once


#pragma warning(push)
#pragma warning(disable:4251)

namespace pdf
{


/**
 * Wraps a buffer for OB and OW data so that the memory and size can be 
 * grouped together for ease.
 */
class PdfBuffer
{




#pragma region constructors/destructors

protected:


  /// Constructor
  PdfBuffer(unsigned char * pData, size_t iSize);

  /// Constructor
  PdfBuffer(size_t iSize = 0);

  /// Copy constructor (Only copies the pointer)
  PdfBuffer(const PdfBuffer & buffer);

public:

  /// Destructor
  virtual ~PdfBuffer();


#pragma endregion


#pragma region methods

#pragma region Create Methods

public:

  /// Create a DcmBuffer using the data supplied. The memory is assumed to be not owned and it will be responsibility of the user to clean up
  static std::shared_ptr<PdfBuffer> Create(const std::string & sPdf);

  /// Create a DcmBuffer using the data supplied. The memory is assumed to be not owned and it will be responsibility of the user to clean up
  static std::shared_ptr<PdfBuffer> Create(unsigned char * pData, const size_t iSize);

  /// Creates the DcmBuffer of a specific size (This memory is owned and will be cleaned up)
  static std::shared_ptr<PdfBuffer> Create(const size_t iSize);

  /// Creates the DcmBuffer of a specific size (This memory is owned and will be cleaned up)
  static std::shared_ptr<PdfBuffer> Create(std::istream & stream);

#pragma endregion



public:

  /// Generates the string from the memory
  std::string ToString()
  { return std::string((char *)m_pData, m_iSize); }

  /// Releases the internal resources
  void Release();

  /// Fills the buffer with the supplied character
  void Fill(char ch);

  /// Returns true if the buffer is empty
  bool IsEmpty()
  { return ( m_pData == __nullptr ); }

 
  /// IsPdf
  static bool IsPdf(void * pBuffer, unsigned int iSize);

  /// Gets the version
  static std::string GetVersion(void * pBuffer, unsigned int iSize);

  /// Takes owner 
  template<class T>
  T * TakeOwnership()
  {
    T * pOwnership(__nullptr);

    if ( m_bOwner ) 
    {
      pOwnership = reinterpret_cast<T *>(m_pData);
      m_bOwner = false;
    }
    return pOwnership;
  }

private:

  /// Validates the data
  void Validate();

public:

  bool IsPdf()
  { return IsPdf(m_pData, (unsigned int)m_iSize); }


  bool IsBitmap()
  {  return ( m_iSize >= 2 && strncmp("BM", (char *)m_pData, 2) == 0 ); }


  /// Gets the version
  std::string GetVersion()
  { return GetVersion(m_pData, (unsigned int)m_iSize); }


  /// Get the data pointer
  template<class T>
  T * GetData()
  { return reinterpret_cast<T *>(m_pData); }

  /// Gets the size
  size_t GetSize()
  { return m_iSize; }

  /// Returns true if the buffer owns the memory, false otherwise
  const bool IsOwner() const
  { return m_bOwner; }


#pragma endregion

#pragma region fields

private:

  /// Data
  unsigned char * m_pData;

  /// Size
  size_t m_iSize;

  /// Flag indication we own the buffer
  bool m_bOwner;


#pragma endregion




}; // class PdfBuffer

}; // namespace pdf

#pragma warning(pop)