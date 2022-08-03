// $Id: Serialization.h,v 1.2 2005/07/01 12:30:57 vxconfig Exp $
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/Serialization.h,v 1.2 2005/07/01 12:30:57 vxconfig Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file 
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.com)

#ifndef Serialization_h
#define Serialization_h

namespace Serialization
{
  /**
   * Copies all the information from a stringstream to an ofstream.
   * Use this to go from a memory-based stream to disk.
   * 
   * @param ss is assumed to be at the end of the data
   * @param ofs is assumed to be at the start of where you want to begin writing.
   */
  inline void CopyStringstreamToOfstream(std::stringstream & ss, std::ofstream & ofs)
  {
    const uint4 length = ss.tellp();
    uint1 * pBuffer = new uint1[length];
    ss.seekp(0);
    ss.read((int1 *)pBuffer, length);
    ofs.write((int1 *)pBuffer, length);
    delete [] pBuffer;
  } // CopyStringstreamToOfstream


  /**
   * Copies all the information from an ifstream to a stringstream.
   * Use this to read from disk into a memory-based stream.
   * 
   * @param ifs is assumed to be at the beggining of the file 
   * @param ss is assumed to be at the start of where you want to begin writing.
   */
  inline void CopyIfstreamToStringstream(std::ifstream & ifs, std::stringstream & ss)
  {
    ifs.seekg(0, std::ios::end);
    const uint4 length = ifs.tellg();
    uint1 * pBuffer = new uint1[length];
    ifs.seekg(0);
    ifs.read((int1 *)pBuffer, length);
    ss.write((int1 *)pBuffer, length);
    delete [] pBuffer;
  } // CopyIfstreamToStringstream


  /**
   * Copies a simple datatype into a memory buffer.
   * It then increments the buffer pointer and decrements the buffer size.
   *
   *  @param pBuffer
   *  @param pBufferEnd
   *  @param data
   *  @return
   */
  template <class T>
  inline static void SerializeSimpleData(int1 ** pBuffer, const int1 * pBufferEnd, const T & data)
  {
    const uint4 dataSize = sizeof(T);
    if (*pBuffer + dataSize > pBufferEnd)
      throw ex::OutOfMemoryException(LogRec("Buffer overrun while serializing simple data","", "SerializeSimpleData"));
 
    try
    {
      memmove(*pBuffer, & data, dataSize);
    }
    catch (...)
    {
      throw ex::IOException(LogRec("Buffer overrun while serializing simple data","", "SerializeSimpleData"));
    }
    *pBuffer += dataSize;
  } // SerializeSimpleData


  /**
   * Copies from a memory buffer into a simple datatype.
   * It then increments the buffer pointer and decrements the buffer size.
   *
   *  @param pBuffer
   *  @param pBufferEnd
   *  @param data
   */
  template <class T>
  inline static void DeserializeSimpleData(int1 ** pBuffer, const int1 * pBufferEnd, T & data)
  {
    const uint4 dataSize = sizeof(T);
    if (*pBuffer + dataSize > pBufferEnd)
      throw ex::OutOfMemoryException(LogRec("Buffer overrun while deserializing simple data","", "DeserializeSimpleData"));
 
    try
    {
      memmove(& data, *pBuffer, dataSize);
    }
    catch (...)
    {
      throw ex::IOException(LogRec("Buffer overrun while deserializing simple data","", "DeserializeSimpleData"));
     }
    *pBuffer += dataSize;
  } // DeserializeSimpleData


  /**
   * Copies an std::string into a memory buffer and null terminates it.
   * It then increments the buffer pointer and decrements the buffer size.
   *
   * @param pBuffer
   * @param pBufferEnd
   * @param string
   */
  inline void SerializeStringData(int1 ** pBuffer, const int1 * pBufferEnd, const std::string & string)
  {
    const uint4 dataSize = static_cast< uint4 >( string.length() );
    const uint4 totalSize = dataSize + 1;
    if (*pBuffer + totalSize > pBufferEnd)
      throw ex::OutOfMemoryException(LogRec("Buffer overrun while serializing string data","","SerializeStringData"));
 
    try
    {
      memmove(*pBuffer, string.c_str(), dataSize);
      (*pBuffer)[dataSize] = 0;
    }
    catch (...)
    {
      throw ex::IOException(LogRec("Buffer overrun while serializing string data","", "SerializeStringData"));
       
    }
    *pBuffer += totalSize;
  } // SerializeStringData
  

  /**
   * Copies a null terminated string from memory into an std::string.
   * It then increments the buffer pointer and decrements the buffer size.
   *
   * @param pBuffer
   * @param pBufferEnd
   * @return myString
   */
  inline std::string DeserializeStringData(int1 ** pBuffer, const int1 * pBufferEnd)
  {
    // verify that the buffer contains at least one null character
    int1 * pBufferTest = *pBuffer;
    try
    {
      while (pBufferTest < pBufferEnd && pBufferTest[0] != NULL)
        pBufferTest++;
    }
    catch (...)
    {
      throw ex::IOException(LogRec("Buffer overrun while deserializing string data","", "DeserializeStringData"));
     }

    if (pBufferTest == pBufferEnd)
      throw ex::OutOfMemoryException(LogRec("Buffer overrun while deserializing string data","", "DeserializeStringData"));
    
    // read in the string
    std::string myString;
    myString = *pBuffer;

    const uint4 totalSize = static_cast< uint4 >( myString.length() + 1 );
    if (*pBuffer + totalSize > pBufferEnd)
      throw ex::OutOfMemoryException(LogRec("Buffer overrun while deserializing string data","", "DeserializeStringData"));

    *pBuffer += totalSize;
    return myString;
  } // DeserializeStringData

}; // namespace Serialization


#endif 


// $Log: Serialization.h,v $
// Revision 1.2  2005/07/01 12:30:57  vxconfig
// Added new Logger.
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.5  2003/05/16 13:08:20  frank
// code formatting
//
// Revision 3.4  2003/05/16 11:53:31  michael
// code review
//
// Revision 3.3  2003/05/13 21:09:19  dongqing
// code review
//
// Revision 3.2  2001/12/20 15:00:15  ingmar
// added class Sec_03_utils to structure the doxygen documentation
//
// Revision 3.1  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0  2001/10/14 23:02:15  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 19 2001 14:30:00   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:46:22   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:33:40   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:08:30  soeren
// renamed utils directory
//
// Revision 1.10  2001/08/13 20:04:53  tushar
// removed errors due to the new logger
//
// Revision 1.9  2001/07/25 01:56:26  sachin
// bad fix of exceptions
//
// Revision 1.8  2001/07/25 01:53:55  sachin
// no message
//
// Revision 1.7  2001/07/24 22:26:03  binli
// new exception constructor.
//
// Revision 1.6  2001/07/13 19:51:53  binli
// defect 547: the error message (to user) has simplified.
//
// Revision 1.5  2001/07/12 21:13:15  binli
// defect ID: 547.
// (the error message shold be simplified.)
//
// Revision 1.4  2001/05/03 14:30:41  binli
// bad use of pointer pBuffferTest
// Reported&Fixed by Frank&BinLi.
//
// Revision 1.3  2001/05/02 19:37:51  ingmar
// Global.h
//
// Revision 1.2.2.1  2001/05/01 23:09:36  jmeade
// Added exception handling. (Frank D.)
//
// Revision 1.3  2001/04/26 11:00:43  frank
// Added exception handling.
//
// Revision 1.2  2001/03/30 18:47:22  dmitry
// Updated datatypes.
//
// Revision 1.1  2001/03/26 17:16:25  frank
// Initial release of helper functions to aid serialization.
//
// 
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/Serialization.h,v 1.2 2005/07/01 12:30:57 vxconfig Exp $
// $Id: Serialization.h,v 1.2 2005/07/01 12:30:57 vxconfig Exp $
