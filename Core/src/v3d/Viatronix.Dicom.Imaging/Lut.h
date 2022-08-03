#pragma once

using namespace System;
using namespace Viatronix::Dicom;



OPEN_DICOM_IMAGING_NS


public ref class  Lut
{

protected:

  Lut() :
  m_iNumberOfEntries(0),
  m_iFirstValue(Int16::MaxValue),
  m_iBitsPerEntry(0),
  m_mpBuffer(nullptr)
  {
  }

public:

  static Lut ^ Create(String ^ mpDescriptor, Buffer ^ mpBuffer);

  static Lut ^ Create();

 
  unsigned char Lut::GetLutValue(short pixel, unsigned char value);



public:

  property Int16 NumberOfEntries
  {
    Int16 get() { return m_iNumberOfEntries; }
  }

  property Int16 BitsPerEntry
  {
    Int16 get() { return  m_iBitsPerEntry; }
  }

  property Int16 FirstValue
  {
    Int16 get() { return  m_iFirstValue; }
  }

private:

  Buffer ^ m_mpBuffer;

  Int16 m_iNumberOfEntries;


  Int16 m_iFirstValue;


  Int16 m_iBitsPerEntry;

  Int16 m_iLastEntry;
  Int16 m_iLastValue;

  



}; // class Lut

CLOSE_DICOM_IMAGING_NS
