// $Id: vxPhaseVolume.C,v 1.2 2005/07/01 12:30:57 vxconfig Exp $
// 
// Copyright © 2001, Viatronix Inc., All Rights Reserved.  
// 
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the written permission Viatronix Inc.
//
// Author: Bin Li binli@viatronix.com


// includes
#include "stdafx.h"
#include "vxPhaseVolume.h"
#include "Serialization.h"


// namespaces
using namespace std;
using namespace Serialization;


// static initialization
const string vxPhaseVolume::m_sDelimiter("TimeVolume::PhaseVolume");


// defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 *  copy constructor
 *  @param other
 */
vxPhaseVolume::vxPhaseVolume(const vxPhaseVolume & other)
{
  // copy block volume
  m_volume = other.m_volume;

  // copy 'm_viImageTime'
  m_viImageTime = other.m_viImageTime;
} // copy()


/** 
 * operator =
 * @param other
 */
vxPhaseVolume & vxPhaseVolume::operator= (const vxPhaseVolume & other)
{
  // copy block volume
  m_volume = other.m_volume;

  // copy 'm_viImageTime'
  m_viImageTime = other.m_viImageTime;

  return *this;
} // operator=


/**
 *  Reset
 */
void vxPhaseVolume::Reset()
{
  // reset block volume
  m_volume.Clear();

  // reset vector 'm_viImageTime'
  m_viImageTime.resize(0);
} // Reset()


/**
 * Set image time
 * @param iPos position
 * @param iImageTime time of image
 */
void vxPhaseVolume::SetImageTime(const int4 iPos, const int4 iImageTime)
{
  int4 iCorrectPos(static_cast< int4 >( ( static_cast< uint4 >( iPos ) >= m_viImageTime.size()) ? m_viImageTime.size()-1 : iPos));
  m_viImageTime[iCorrectPos] = iImageTime;
} // SetImageTime()


/**
 * Get image time
 * @param iPos position
 * @return time image
 */
const int4 vxPhaseVolume::GetImageTime(const int4 iPos)
{
  int4 iCorrectPos(static_cast<int4>((static_cast<uint4>(iPos) >= m_viImageTime.size()) ? m_viImageTime.size()-1 : iPos));
  return m_viImageTime[iCorrectPos];
} // SetImageTime()


/**
 * Initialization
 * @param iSize
 */
void vxPhaseVolume::Initialize(int4 iSize)
{
  m_viImageTime.resize(iSize);
} // Initialize().


/**
 * Retrieves the number of bytes of the serialized data
 * @return data size
 */
uint4 vxPhaseVolume::GetSerializedDataSize() const
{
  uint4 uSum(0);

  // starting delimiter.
  uSum += static_cast< uint4 >( m_sDelimiter.length() + 1 );

  // size of the vector 'ImageTime'.
  uSum += static_cast< uint4 >(  sizeof(int4) );

  // vector 'ImageTime'
  uSum += static_cast< uint4 >( m_viImageTime.size() * sizeof(int4) );
  
  // ending delimiter
  uSum += static_cast< uint4 >(  m_sDelimiter.length() + 1 );

  return uSum;
} // GetSerializedDataSize()


/**
 *  Reads in all the data from serialized form.
 *  @param hiBuffer    2-level pointer to the memory buffer which stores the strips.
 *  @param piBufferEnd pointer to the end of strips memory buffer.
 *  @return true when success, else, false.
 */
bool vxPhaseVolume::ReadSerializedData(int1 ** hiBuffer, const int1 * piBufferEnd)
{
  try
  {
    // verify the stream by reading start delimiter.
    if (DeserializeStringData(hiBuffer, piBufferEnd) != m_sDelimiter)
      throw ex::DataCorruptionException(LogRec("data corruption: bad starting delimiter not '" + m_sDelimiter + "'.", "vxPhaseVolume", "ReadSerializedData"));

    // size of vector 'ImageTime'.
    uint4 uSize(0);
    DeserializeSimpleData(hiBuffer, piBufferEnd, uSize);
    m_viImageTime.resize(uSize);
    // Image time.
    for (uint4 i(0); i<uSize; i++)
    {
      int4 iImageTime(0);
      DeserializeSimpleData(hiBuffer, piBufferEnd, iImageTime);
      m_viImageTime[i] = iImageTime;
    }

    // verify the stream again by reading end delimiter.
    if (DeserializeStringData(hiBuffer, piBufferEnd) != m_sDelimiter) 
	  {
      throw ex::DataCorruptionException(LogRec("data corruption: bad ending delimiter: Not '" + m_sDelimiter  + "'." ,"vxPhaseVolume", "ReadSerializedData"));
    }
  }
  catch (ex::OutOfMemoryException & error)
  {
    throw ex::OutOfMemoryException(LogRec( "Out of memory" + error.GetLogRecord().GetTextMessage(),"vxPhaseVolume", "ReadSerializedData"));
   }
  catch (ex::VException & error)
  {
    throw ex::VException(LogRec(error.GetLogRecord().GetTextMessage(),"vxPhaseVolume", "ReadSerializedData"));
  }

  return true;

} // ReadSerializedData()


/**
 *  Retrieves all the data in serialized form
 *  @param hiBuffer    2-level pointer to the memory buffer which stores the strips.
 *  @param piBufferEnd pointer to the end of strips memory buffer.
 *  @return true when success, else, false.
 */
bool vxPhaseVolume::WriteSerializedData(int1 ** hiBuffer, const int1 * piBufferEnd)
{
  // ensure there is enough space
  if (*hiBuffer + GetSerializedDataSize() > piBufferEnd) 
  {
    throw ex::VException(LogRec("<vxPhaseVolume::WriteSerializedData> Not enough space.", "vxPhaseVolume", "WriteSerializedData"));
  }

  /// start delimiter
  SerializeStringData(hiBuffer, piBufferEnd, m_sDelimiter);

  // size of the m_viImageTime.
  SerializeSimpleData(hiBuffer, piBufferEnd, m_viImageTime.size());

  // image time.
  for (uint4 i(0); i<m_viImageTime.size(); i++)
  {
    // size of the m_viImageTime.
    SerializeSimpleData(hiBuffer, piBufferEnd, m_viImageTime[i]);
  }

  /// end delimiter
  SerializeStringData(hiBuffer, piBufferEnd, m_sDelimiter);

  return true;

} // WriteSerializedData()


#undef FILE_REVISION


// Revision History:
// $Log: vxPhaseVolume.C,v $
// Revision 1.2  2005/07/01 12:30:57  vxconfig
// Added new Logger.
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.8  2003/06/23 15:54:55  michael
// completed 2nd code review
//
// Revision 1.7  2003/06/18 15:19:47  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.6  2003/05/16 14:56:54  dongqing
// code review
//
// Revision 1.5  2003/05/16 13:15:58  frank
// formatting
//
// Revision 1.4  2003/05/16 11:54:10  michael
// code review
//
// Revision 1.3  2002/07/16 17:08:02  frank
// Centralized the delimiter string.
//
// Revision 1.2  2002/07/15 21:17:25  frank
// Removed templates
//
// Revision 1.1  2002/07/10 18:34:07  frank
// Initial version.
//
// Revision 1.1  2002/06/03 19:32:22  binli
// initial check in & serialization
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/vxPhaseVolume.C,v 1.2 2005/07/01 12:30:57 vxconfig Exp $
// $Id: vxPhaseVolume.C,v 1.2 2005/07/01 12:30:57 vxconfig Exp $
