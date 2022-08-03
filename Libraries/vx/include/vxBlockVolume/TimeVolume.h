// $Id: TimeVolume.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: Frank Dachille (frank@viatronix.com)
//
// Complete history at bottom of file.

#ifndef TimeVolume_h
#define TimeVolume_h



// Include declarations
#include "Quad.h"



// Define declarations
#define FILE_REVISION "$Revision: 1.1.1.1 $"



// Forward declarations



// class definition
template <class T>
class TimeVolume
{
// Member Functions
public:

  /// Constructor.
  TimeVolume();

  /// Destructor.
  ~TimeVolume();
  
  /// Reads the volume specified into memory, ALWAYS check the return value.
  void Read(const std::string & sFileName);

  /// Writes the volume to the specified file.
  void Write(const std::string & sfileName);
  
  /// Set the value at the location specified.
  inline void SetVoxel(const Quad<uint2> & location, T value);

  /// Gets the voxel at the specified location (No Bounds checking!).
  inline const int2 & GetVoxel(const Quad<uint2> & location) const;

  /// Set a volume in the list.
  inline void SetVolume(const uint2 & uTime, T & inVol);
  
  /// Retrieve a volume from the list.
  inline T & GetVolume(const uint2 & uTime);
  
  /// Set the time (in seconds) of a volume in the list.
  inline void SetVolumeTimeSeconds(const uint2 & uTimeIndex, const float4 & fTimeValueSeconds);
  
  /// Get the time (in seconds) of a volume in the list.
  inline const float4 & GetVolumeTimeSeconds(const uint2 & uTimeIndex) const;

  /// Sets the number of volumes to store.
  inline void SetTimeDimension(const uint2 & uTimeDimension);

  /// Gets the number of volumes in the time dimension.
  inline const uint2 & GetTimeDimension() const;
  
  static bool Test();

// Data Members
private:

  /// The vector of volume datasets.
  std::vector<T *> m_volumeVector;

  /// The vector of time values (what time each volume was acquired in seconds).
  std::vector<float4> m_timeVector;

  /// The prefix of the volume datasets.
  std::string m_sFilePrefix;

};

#undef FILE_REVISION

#include "TimeVolume.inl"

#endif // TimeVolume_h

// $Log: TimeVolume.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.7  2003/05/16 11:53:31  michael
// code review
//
// Revision 1.6  2003/01/02 16:47:21  ingmar
// added missing include
//
// Revision 1.5  2002/03/08 17:15:17  michael
// further removed include dependencies
//
// Revision 1.4  2002/02/22 00:26:27  frank
// Wrote I/O routines and test for class.
//
// Revision 1.3  2002/02/18 20:08:04  frank
// Changed to store a vector of pointers.
//
// Revision 1.2  2002/02/15 20:19:12  frank
// Revised entire class so that it stores a vector of volumes.
//
// Revision 1.1  2002/01/11 04:55:11  frank
// Initial version.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/TimeVolume.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: TimeVolume.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
