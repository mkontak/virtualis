#include "Stdafx.h"
#include "DcmLut.h"
#include "DcmBuffer.h"
#include "DcmTriple.h"




using namespace dcm;


DcmLut::DcmLut() :
m_iFirstMappedValue(0),
m_iNumberOfBits(0),
m_iNumberOfEntries(0),
m_iLastMappedValue(0),
m_lutBuffer(__nullptr)
{
}


DcmLut::DcmLut(short iNumberOfEntries, short iBitsPerEntry, short iFirstMappedValue ) :
m_iFirstMappedValue(iFirstMappedValue),
m_iNumberOfBits(iBitsPerEntry),
m_iNumberOfEntries(iNumberOfEntries),
m_iLastMappedValue(iFirstMappedValue + iNumberOfEntries),
m_lutBuffer(__nullptr)
{
}





 std::shared_ptr<DcmLut> DcmLut::Create(short iNumberOfEntries, short iBitsPerEntry, short iFirstMappedValue )
 {

   if ( iBitsPerEntry != 8 || iBitsPerEntry != 16 )
     throw DcmInvalidDataException(LogRec("Invalid bits per entry specified for the LUT table","DcmLut","Create"));


   std::shared_ptr<DcmLut> lut = std::shared_ptr<DcmLut>(new DcmLut(iNumberOfEntries, iBitsPerEntry, iFirstMappedValue));

   if ( lut == __nullptr )
     throw DcmOutOfMemoryException(LogRec("Failed to allocate DcmLut","DcmLut","Create"));

   /// Create the buffer necessary for the table using the bits per entry and the number of entries
   lut->m_lutBuffer = DcmBuffer::Create( ( iBitsPerEntry * ( iBitsPerEntry / 8 ) ) * iNumberOfEntries);

   return lut;
 }  // Create(short iNumberOfEntries, short iBitsPerEntry, short iFirstMappedValue )


 /**
  * Creates the LUT from the data extracted from the DICOM header
  *
  * @param sDescriptor        Multi-valued descriptor
  * @param lutTable           LUT Data table
  *
  * @return   Returns the DcmLut
  */
 std::shared_ptr<DcmLut> DcmLut::Create(const std::string & sDescriptor, std::shared_ptr<DcmBuffer> data )
 {

   std::shared_ptr<DcmLut> lut = std::shared_ptr<DcmLut>(new DcmLut());

   if ( lut == __nullptr )
     throw DcmOutOfMemoryException(LogRec("Failed to allocate DcmLut","DcmLut","Create"));


   DcmTriple<short> descriptor = DcmTriple<short>(sDescriptor);

    lut->m_iNumberOfEntries = descriptor.GetX();
    lut->m_iFirstMappedValue = descriptor.GetY();
    lut->m_iNumberOfBits = descriptor.GetZ();
    lut->m_iLastMappedValue = lut->m_iFirstMappedValue + lut->m_iNumberOfEntries;

    lut->m_lutBuffer = data;
   
    return lut;

 }  // Create(short iNumberOfEntries, short iBitsPerEntry, short iFirstMappedValue )


/**
 * Returns the iunsigned char mapped pixel value
 *
 * @param pixelValue    Pixel value to map
 * @param defaultValue  Default value
 *
 * @return Mapped value
 */
unsigned char DcmLut::GetMappedValue(unsigned char pixelValue, unsigned char defaultValue)
{

unsigned char returnValue = defaultValue;

if ( pixelValue >= m_iFirstMappedValue && pixelValue <= m_iLastMappedValue )
{
  returnValue = *(m_lutBuffer->GetData<unsigned char>() + (pixelValue - m_iFirstMappedValue));
}

   
return returnValue;

}


/**
 * Returns the iunsigned char mapped pixel value
 *
 * @param pixelValue    Pixel value to map
 * @param defaultValue  Default value
 *
 * @return Mapped value
 */
short DcmLut::GetMappedValue(short pixelValue, short defaultValue)
{

  short returnValue = defaultValue;

  if ( pixelValue >= m_iFirstMappedValue && pixelValue <= m_iLastMappedValue )
  {
    returnValue = *(m_lutBuffer->GetData<short>() + (pixelValue - m_iFirstMappedValue));
  }

   
  return returnValue;

}
