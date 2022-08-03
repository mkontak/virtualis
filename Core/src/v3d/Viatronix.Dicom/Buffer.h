// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

#include "scoped_ptr.h"

/// Usings
using namespace System;
using namespace System::ComponentModel;
using namespace System::Globalization;

OPEN_DICOM_NS

// Forward declarations
ref class BufferTypeConverter;
ref class Image;

/**
 * Implements a DICOM buffer used to store binary data
 */
[TypeConverter(Viatronix::Dicom::BufferTypeConverter::typeid)]
public ref class Buffer : TypeConverter
{

#pragma region constrctors/destructors


public:

  // Constructor (Default)
  Buffer();

 
  static Buffer ^ Create(IntPtr pDcmBufferSharedPtr);

 
  void Reset(IntPtr pDcmBufferSharedPtr);

#pragma endregion


#pragma region methods

public:

  array<short> ^ GetShortData()
  {

    int iSize(Size / 2);
    array<short> ^ managed  = gcnew array<short>(iSize);

    short* native = (*m_pDcmBuffer)->GetData<short>();

    for ( register int i(0); i < iSize; i++ )
    {
      managed[i] = native[i];
    }

    return managed;
  }


  array<unsigned char> ^ GetByteData()
  {

    array<unsigned char> ^ managed  = gcnew array<unsigned char>(Size);

    unsigned char* native = (*m_pDcmBuffer)->GetData<unsigned char>();

    for ( register int i(0); i < Size; i++ )
    {
      managed[i] = native[i];
    }

    return managed;
  }
 
  /// Gets the buffer pointer 
  IntPtr GetBufferPtr() 
  {
    return System::IntPtr((void *)(*m_pDcmBuffer)->GetData<void>());
  } //  GetBufferPtr() 

  /// Get the value at the row/column
  template<class R>
  R GetValue(int iRow, int iColumn)
  {
    return (*m_pDcmBuffer)->GetValue<R>(iRow, iColumn);
  } // GetValue(int iRow, int iColumn)



#pragma endregion


public:

  /// Gets the length of the buffer in bytes
  property long Size
  { long get(); } // Length

#pragma region fields

private:

   /// DcmImage
  scoped_ptr<std::shared_ptr<dcm::DcmBuffer>> m_pDcmBuffer;


#pragma endregion

};  // class Buffer


CLOSE_DICOM_NS