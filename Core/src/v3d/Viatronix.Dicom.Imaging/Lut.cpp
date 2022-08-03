#include "Stdafx.h"
#include "Lut.h"



USING_DICOM_IMAGING_NS



Lut ^ Lut::Create()
{
  

  Lut ^ mpLut = gcnew Lut();


  if ( mpLut == nullptr )
    throw gcnew OutOfMemoryException("Failed to allocate Lut object");

   return mpLut;
}

Lut ^ Lut::Create(String ^ mpDescriptor, Viatronix::Dicom::Buffer ^ mpBuffer)
{
  

  Lut ^ mpLut = Create();

  
  array<Char> ^ del = { '\\' };

  array<String ^> ^ fields = mpDescriptor->Split(del);

  mpLut->m_iNumberOfEntries = Int16::Parse(fields[0]);
  mpLut->m_iFirstValue = Int16::Parse(fields[1]);
  mpLut->m_iLastValue  = mpLut->m_iFirstValue + mpLut->m_iNumberOfEntries;
  mpLut->m_iBitsPerEntry = Int16::Parse(fields[2]);
  mpLut->m_iLastEntry = mpLut->m_iNumberOfEntries - 1;

  
  mpLut->m_mpBuffer = mpBuffer;
  
  return mpLut;
}






unsigned char  Lut::GetLutValue(short pixel, unsigned char value)
{

  unsigned char rvalue(value);

  if ( pixel >= m_iFirstValue &&  pixel < m_iLastValue )
    return (unsigned char)(m_mpBuffer->GetShortData()[pixel - m_iFirstValue]);
  else
    return value;

  //if ( pixel <= m_iFirstValue )
  //  rvalue = (unsigned char)(m_mpBuffer->GetShortData()[0]);
  //else if ( pixel > m_iLastEntry )
  //  rvalue = (unsigned char)(m_mpBuffer->GetShortData()[m_iLastEntry]);


  //else
  //    rvalue = (unsigned char)(m_mpBuffer->GetShortData()[pixel - m_iFirstValue]);


  return rvalue;

}

