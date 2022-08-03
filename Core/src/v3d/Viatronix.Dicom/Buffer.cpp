// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak  mkontak@viatronix.net

#include "Stdafx.h"
#include "Buffer.h"





USING_DICOM_NS


 /**
  *  Constructor (Default)
  */
 Viatronix::Dicom::Buffer::Buffer() : 
 m_pDcmBuffer(scoped_ptr<std::shared_ptr<dcm::DcmBuffer>>(__nullptr)) 
 {
   
 }// Buffer()




  Viatronix::Dicom::Buffer ^  Viatronix::Dicom::Buffer::Create(IntPtr pDcmBufferSharedPtr)
  {

    Viatronix::Dicom::Buffer ^ mpBuffer(gcnew Viatronix::Dicom::Buffer());

    if ( mpBuffer == nullptr )
      throw gcnew OutOfMemoryException("Failed to allocate Buffer");

    std::shared_ptr<dcm::DcmBuffer> pDcmBuffer(*reinterpret_cast<std::shared_ptr<dcm::DcmBuffer> *>(pDcmBufferSharedPtr.ToPointer()));

    mpBuffer->m_pDcmBuffer.reset(new std::shared_ptr<dcm::DcmBuffer>(pDcmBuffer));

    return mpBuffer;

  }

  void Viatronix::Dicom::Buffer::Reset(IntPtr pDcmBufferSharedPtr)
  {
    std::shared_ptr<dcm::DcmBuffer> pDcmBuffer(*reinterpret_cast<std::shared_ptr<dcm::DcmBuffer> *>(pDcmBufferSharedPtr.ToPointer()));

    m_pDcmBuffer.reset(new std::shared_ptr<dcm::DcmBuffer>(pDcmBuffer));

  }

  /**
   * Gets the size of of the buffer
   */
  long Viatronix::Dicom::Buffer::Size::get()
  {
    return (long)(*m_pDcmBuffer)->GetSize(); 
  } // Length

   