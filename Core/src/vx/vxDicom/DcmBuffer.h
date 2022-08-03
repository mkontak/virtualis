// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmBuffer.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

#include "DcmExceptions.h"

#pragma warning(push)
#pragma warning(disable:4251)

namespace dcm
{

// Forward declarations
class DCM_DLL DcmImage;

/**
 * Wraps a buffer for OB and OW data so that the memory and size can be 
 * grouped together for ease.
 */
class DCM_DLL DcmBuffer
{

#pragma region enums

public:

  typedef enum DcmDataTypes
  {
    DATA_ANY      = -1,
    DATA_GENERIC,
    DATA_PDF,
    DATA_JPEG,
    DATA_PNG,
    DATA_BITMAP,
    DATA_DICOM
  } DcmDataTypes;

#pragma endregion

#pragma region constructors/destructors

public:

  /// Constructor
  DcmBuffer(unsigned char * pData, size_t iSize, DcmDataTypes eType = DATA_ANY);

  /// Constructor
  DcmBuffer(size_t iSize = 0, DcmDataTypes eType = DATA_GENERIC);

  /// Copy constructor (Only copies the pointer)
  DcmBuffer(const DcmBuffer & buffer);

  /// Destructor
  virtual ~DcmBuffer();

  /// Clones the buffer making a copy
  std::shared_ptr<DcmBuffer> Clone();

#pragma endregion


#pragma region methods

#pragma region Create Methods

public:

  /// Create a DcmBuffer using the data supplied. The memory is assumed to be not owned and it will be responsibility of the user to clean up
  static std::shared_ptr<DcmBuffer> Create(unsigned char * pData, const size_t iSize, DcmDataTypes eType = DATA_ANY);

  /// Creates the DcmBuffer of a specific size (This memory is owned and will be cleaned up)
  static std::shared_ptr<DcmBuffer> Create(const size_t iSize, DcmDataTypes eType = DATA_GENERIC);

  /// Creates the DcmBuffer of a specific size (This memory is owned and will be cleaned up)
  static std::shared_ptr<DcmBuffer> Create(std::istream & stream, DcmDataTypes eType = DATA_ANY);

#pragma endregion

public:

    /// Overloaded == operator
  inline bool operator==(std::shared_ptr<DcmBuffer> & rhs) const
  { return Equals(rhs); }


public:

  /// Determines equality
  bool Equals(std::shared_ptr<DcmBuffer> & rhs) const
  { return ( m_iSize == rhs->m_iSize && memcmp(m_pData, rhs->m_pData, m_iSize ) == 0 ); }

  /// Releases the internal resources
  void Release();

  /// Fills the buffer with the supplied character
  void Fill(char ch);

  /// Zero fills the buffer
  inline void Zero()
  { Fill('\0'); }

  /// Returns true if the buffer is empty
  bool IsEmpty()
  { return ( m_pData == __nullptr ); }

  /// Gets the value at the row/column position
  template<class T>
  T GetValue(size_t iOffset)
  {
    T * dataPtr((T *)m_pData);

    if ( iOffset < 0 || iOffset > m_iSize )
      throw DcmOutOfBoundsException(LogRec("Value requested is out of bounds","DcmBuffer","GetValue"));

    return *(dataPtr + iOffset);
  } // GetValue(site_t iOffset)

  /// Gets the value at the row/column position
  template<class T>
  T SetValue(size_t iOffset, T value)
  {
    T * dataPtr((T *)m_pData);

    if ( iOffset < 0 || iOffset > m_iSize )
      throw DcmOutOfBoundsException(LogRec("Value requested is out of bounds","DcmBuffer","GetValue"));

    *(dataPtr + iOffset) = value;
  } // SetValue(size_t iOffset, T value)


  /// Gets the value at the row/column position
  template<class T>
  void GetMinMaxValues( std::pair<T,T> & minMax, T paddingValueToIgnore = (T)(USHRT_MAX), bool ignorePaddingValue = false )
  {
    T * dataPtr((T *)m_pData);

    size_t iSize = m_iSize / sizeof(T);

 
    // =========================================
    // Loop through data detecting min and max
    // =========================================
    for ( register int i(0); i < iSize; ++i, dataPtr++ )
    {
      T value = *dataPtr;

      if ( ! ignorePaddingValue || ( ignorePaddingValue && value != paddingValueToIgnore ) )
      {
        if ( value < minMax.first )
          minMax.first = value;

        if ( value > minMax.second )
          minMax.second = value;
      }
    } // END ... For each element

  } // GetValue(int iRow, int iCol)

  /// Assigns memory to wrapper
  void Assign(unsigned char * pData, const unsigned int iSize, bool bTakeOwnership = false);


  /// Copies the pixel data to the buffer supplied.
  size_t CopyTo( unsigned char * pBuffer, const size_t iSize, const size_t iPosition = 0 );

  /// Copies the pixel data to the DcmBuffer supplied.
  size_t CopyTo( std::shared_ptr<DcmBuffer> & buffer, const size_t iSize,  const size_t iPosition = 0 );

  /// Appends the supplied buffer to this.
  void Append(std::shared_ptr<DcmBuffer> buffer);

private:

  /// Returns true if the 
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

  /// Determines the data types and set the property
  void DetermineDataType();


public:

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

  /// Gets the data type if possible
  const DcmDataTypes GetDataType() 
 { DetermineDataType(); return m_eType; }

  /// Sets the data type
  void SetDataType(DcmDataTypes eType)
  { m_eType = eType;  }

#pragma endregion

#pragma region fields

private:

  /// Data
  unsigned char * m_pData;

  /// Size
  size_t m_iSize;

  /// Actual size (Size of the data prior to aligining)
  size_t m_iActualSize;


  /// Flag indication we own the buffer
  bool m_bOwner;

  /// Type of data
  DcmDataTypes m_eType;

#pragma endregion


  friend class DcmImage;

}; // class DcmBuffer

}; // namespace dcm

#pragma warning(pop)