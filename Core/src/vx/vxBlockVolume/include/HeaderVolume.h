// $Id: HeaderVolume.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Created: Oct. 00
// Author: David dave@cs.sunysb.edu
//
// Complete history at bottom of file


#ifndef HeaderVolume_h
#define HeaderVolume_h

// includes
#include "vxInflate.h"
#include "Point.h"
#include "Vector.h"
#include "V3DFile.h"
#include "VxVolIterator.h"


/**
 * Initilizes all volume header variables, does not read, allocate or store the accompanying volume data.
 * Note that some functions available in other volume layouts are purposely made private here since there
 * is not data.
 * $Id: HeaderVolume.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $ <a href="../../vxBase/8_volume/spec/VolumeDesign_Specification.htm">Design Specification</a>
*/
template <class T>
class HeaderVolume : public hdr::VolAPI<T>
{
public:
  /// Default constructor for an empty HeaderVolume.
  HeaderVolume();

  /// Reads the volume specified into memory. (new V3D File format)
  void Read(std::string sFileName); 
 
  /// Clear out all the information
  void Clear();

  /// Returns the name of the stored volume type
  const std::string & GetDataType();

  int4 GetDiskLayout();

  // simply use generic VxVolIterator
  typedef  VxVolIterator<HeaderVolume<T>,T> VolIterator;

protected: //functions
  // Shell function to prevent VolAPI version from being called.
  void CheckBitsPerVoxel();

  hdr::MemoryLayoutEnum ReadHeader(const std::string & headerPtr);

private: // data
  std::string m_sTypeName;
  int4 m_iLayout;

private: //functions
  /// Constructor for building new volumes.
  HeaderVolume(const Triple<uint4> & dim, const uint4 & uiBitsPerVoxel, 
               const Triple<float4> & units = Triple<float4>(1.0,1.0,1.0), 
               hdr::UnitTypeEnum unitType = hdr::MILLIMETER, 
               const T & background = 0, hdr::ExtensionEnum ext=VOL,
               const std::string & sFileName = "Blank.vol",
               const std::string & sTitle    = "Blank",
               const std::string & sAuthor   = "Blank",
               const vx::Date & date = vx::Date());

  /// Writes the volume to the specified file.
  void Write(std::string sFileName = "NULL");

  /// Set the value at the location specified.
  void SetVoxelRaw(const Point<uint2> & location, T value);
  /// Gets the voxel at the specified location.
  const T & GetVoxelRaw(const Point<uint2> & location) const;

  /// Returns the total memory usage in bytes.
  const uint4 GetMemoryUsage() const;

  /// Reports relevant volume layout statistics
  void Stats(std::ostream & os = std::cout) const;
};


typedef HeaderVolume<uint1> HeaderVol;

#include "HeaderVolume.inl"

#endif

// $Log: HeaderVolume.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.3  2003/09/04 17:29:43  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 3.2.2.1  2003/08/11 11:07:22  mkontak
// moved Date into vx namespace
//
// Revision 3.2  2003/05/16 11:53:11  michael
// code review
//
// Revision 3.1  2001/12/24 16:46:19  ingmar
// added class Sec_30_volume to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:21  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:47:34   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:35:02   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:12:19  soeren
// renamed volume  directory
//
// Revision 1.21  2001/07/18 20:53:17  binli
// In volume class: read PNG image. removed old Read(), referred new Read()
//
// Revision 1.20  2001/07/09 16:51:50  frank
// Added support for the new V3D file format.
//
// Revision 1.19  2001/04/19 20:59:05  dave
// updated variable names
//
// Revision 1.18  2001/04/12 18:54:06  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.17  2001/04/12 15:50:51  dave
// added namespace vx for exceptions
//
// Revision 1.16  2001/04/10 16:13:26  dave
// updated types of exceptions throw by Read() functions
//
// Revision 1.15  2001/04/08 21:38:21  dave
// updated doc headers
//
// Revision 1.14  2001/04/03 05:47:00  soeren
// added documentation
//
// Revision 1.13  2001/03/31 05:34:38  ingmar
// uint8 -> uint4
//
// Revision 1.12  2001/03/30 18:50:04  dmitry
// Updated datatypes.
//
// Revision 1.11  2001/02/06 02:25:11  dave
// re-organized linVol Read for memory savings
//
// Revision 1.10  2000/12/22 15:52:00  frank
// Added function to clear out old data.
//
// Revision 1.9  2000/12/11 21:38:40  dave
// new libpng, exception, file versions etc
//
// $Id: HeaderVolume.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $