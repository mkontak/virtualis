// $Id: vxPhaseVolume.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// 
// Copyright © 2002, Viatronix Inc., All Rights Reserved.  
// 
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the written permission Viatronix Inc.
//
// Author: Bin Li (binli@viatronix.net)

/*
 Stores a set of volumes containing the various phases 
 captured by cine-acquisition. Commonly used to store 
 the phases of a cardiac cycle.
*/

#ifndef vxPhaseVolume_h
#define vxPhaseVolume_h


// includes
#include "vxBlockVolume.h"
#include "vxBlockVolumeIterator.h"


// class definition
class VX_BLOCK_VOLUME_DLL vxPhaseVolume  
{
public:

  /// default constructor
  vxPhaseVolume() {}

  /// destructor
  virtual ~vxPhaseVolume() {}

  /// copy constructor
  vxPhaseVolume(const vxPhaseVolume & other);

  /// operator assignment
  vxPhaseVolume & operator= (const vxPhaseVolume & other);

// functions
public:

  /// reset
  void Reset();

  /// initialization
  void Initialize(int4 iSize);

  /// retrieves the number of bytes of the serialized data
  uint4 GetSerializedDataSize() const;

  /// retrieves all the data in serialized form
  bool WriteSerializedData(int1 ** hiBuffer, const int1 * piBufferEnd);

  /// reads in all the data from serialized form
  bool ReadSerializedData(int1 ** hiBuffer, const int1 * piBufferEnd);

  /// returns the volume data
  vxBlockVolume<uint2> & GetVolume() { return m_volume; }

  /// set image time
  void SetImageTime(const int4 iPos, const int4 iImageTime);
    
  /// get image time
  const int4 GetImageTime(const int4 iPos);

  /// get z-dim of the volume
  inline const int4 GetDimZ() { return static_cast< int4 >( m_viImageTime.size() ); };

/// data
private:

  /// volume to hold data for a single phase
  vxBlockVolume<uint2> m_volume;

  /// image time
  std::vector<int4> m_viImageTime;

  /// string delimiter for debugging I/O
  static const std::string m_sDelimiter;
}; // vxPhaseVolume


#endif // vxPhaseVolume_h


// Revision History
// $Log: vxPhaseVolume.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2003/06/23 15:54:55  michael
// completed 2nd code review
//
// Revision 1.9  2003/06/18 15:19:48  geconomos
// Changed all instances of vxBlockVolume<int2> to vxBlockVolume<uint2>
//
// Revision 1.8  2003/05/16 14:58:21  dongqing
// code review
//
// Revision 1.7  2003/05/16 13:15:58  frank
// formatting
//
// Revision 1.6  2003/05/16 11:54:10  michael
// code review
//
// Revision 1.5  2002/08/02 21:34:25  mkontak
// Added Time Volume to project.
//
// Revision 1.4  2002/07/16 17:08:02  frank
// Centralized the delimiter string.
//
// Revision 1.3  2002/07/15 21:17:25  frank
// Removed templates
//
// Revision 1.2  2002/07/15 13:22:27  geconomos
// exported classes.
//
// Revision 1.1  2002/07/10 18:34:07  frank
// Initial version.
//
// Revision 1.7  2002/06/11 13:39:01  binli
// copy constructor & assign operator
//
// Revision 1.6  2002/06/10 20:57:15  binli
// Reset
//
// Revision 1.5  2002/06/06 16:44:25  binli
// added GetDim function.
//
// Revision 1.4  2002/06/05 22:17:30  uday
// Block Volume instead of pointer to block volume.
//
// Revision 1.3  2002/06/05 17:04:58  uday
// Added vxBlockVolume member data.
//
// Revision 1.2  2002/06/05 15:17:07  binli
// Get/Set image time
//
// Revision 1.1  2002/06/03 19:32:22  binli
// initial check in & serialization
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxPhaseVolume.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: vxPhaseVolume.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
