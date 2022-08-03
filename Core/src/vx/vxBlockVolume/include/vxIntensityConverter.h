// $Id: vxIntensityConverter.h,v 1.4 2005/09/23 14:33:18 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (michael@viatronix.com)

/*
  Class to convert to and from ZeroFloat window/level values to displayable values
  
  Usage:
    If you want to use this function to convert from a raw (data in the volume) to
    a displayable value (e.g., HU ) then you need to use code like this:
 
      int2 iHU = vxIntensityConverter::ToShort( vxIntensityConverter::FromShort( iVoxel, "CT", false ), "CT", true );
*/

#ifndef vxIntensityConverter_h
#define vxIntensityConverter_h


// includes
#include "ZeroOne.h"
#include "vxBlockVolume.h"


// forward declarations
class vxVolumeHeader;


// class definition
class VX_BLOCK_VOLUME_DLL vxIntensityConverter
{
// functions
public:

  /// initialize from raw short
  static inline int4 InitFromShort(const int4 iInput, const vxVolumeHeader & header, const bool bLevel = false);

  /// returns the Units of the given modality, e.g. HU for CT
  static inline std::string GetUnit(const std::string sModality);

  /// returns the Units of the given modality, e.g. HU for CT
  static inline std::string GetUnit(const vxVolumeHeader & header);

  /// get the minimum value
  static inline int4 GetMinValue(const vxVolumeHeader & header, const bool bLevel = false);

  /// get the minimum value
  static inline int4 GetMinValue(const vxBlockVolume<uint2> & volume, const bool bLevel = false);

  /// get the maximum value
  static inline int4 GetMaxValue(const vxVolumeHeader & header, const bool bLevel = false);

  /// get the maximum value
  static inline int4 GetMaxValue(const vxBlockVolume<uint2> & volume, const bool bLevel = false);

  /// convert to from short
  static inline ZeroOne FromShort(const int4 iInput, const vxVolumeHeader & header, const bool bLevel = false);

  /// clamp a short value short
  static inline int4 ClampShort(const int4 iInput, const vxVolumeHeader & header, const bool bLevel = false);

  /// convert to to short
  static inline int4 ToShort(const ZeroOne fInput, const vxVolumeHeader & header, const bool bLevel = false);

  /// convert raw value to modality value
  static inline int4 RawToModality( const int4 iRaw, const vxVolumeHeader & header );

  /// convert modality value to raw value
  static inline int4 ModalityToRaw( const int4 iModality, const vxVolumeHeader & header );

  /// returns level offset being potentially out of [0, max] as well as additional offsets of DICOM conversion
  static inline int4 GetBias(const vxVolumeHeader & header, const bool bLevel = false);
  
}; // vxIntensityConverter


#include "vxIntensityConverter.inl"


#endif // vxIntensityConverter_h


// Revision History:
// $Log: vxIntensityConverter.h,v $
// Revision 1.4  2005/09/23 14:33:18  geconomos
// chade accessibilty of GetBias to public
//
// Revision 1.3  2004/10/26 13:30:07  michael
// made static functions inline
//
// Revision 1.2  2004/03/24 21:51:06  frank
// fixed typo in modality to raw conversion
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.16  2004/01/22 15:42:34  wenli
// Add Raw and Modality conversion
//
// Revision 1.15  2003/12/19 00:36:19  michael
// Introduced a GetBias() to account for the bias for window/level and simplify
// the overall mechanism. Furthermore, it easily allows to implement a bias to
// account for different vendors.
//
// Revision 1.14  2003/07/03 17:24:47  michael
// added two new min/max functions, based on volume, not volume header
//
// Revision 1.13  2003/06/05 17:35:23  michael
// transition from short to int
//
// Revision 1.12  2003/06/04 18:32:39  michael
// extended the intensity converter to take a volume header so that not only
// modality but number of bits used can also be used.
//
// Revision 1.11  2003/05/27 14:12:00  michael
// completed code review
//
// Revision 1.10  2003/05/16 13:16:20  frank
// formatting
//
// Revision 1.9  2003/05/16 11:54:11  michael
// code review
//
// Revision 1.8  2003/04/18 14:44:26  frank
// Added usage assistance for the common case of converting a raw value into a displayable value.
//
// Revision 1.7  2003/04/08 16:06:53  michael
// cosmetics
//
// Revision 1.6  2003/01/27 23:18:49  michael
// added annotation stuff: flexible text location (handle index), multiple text lines etc.
//
// Revision 1.5  2002/10/10 15:14:21  michael
// Extended intensity converter by min/max methods
//
// Revision 1.4  2002/10/07 22:28:55  michael
// extended intensity converter to clamp values
//
// Revision 1.3  2002/10/07 14:32:59  michael
// fixed documentation include ... again
//
// Revision 1.2  2002/10/07 14:31:29  michael
// fixed documentation include ...
//
// Revision 1.1  2002/10/07 14:09:38  michael
// Added initial version to be used on the GUI level
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxIntensityConverter.h,v 1.4 2005/09/23 14:33:18 geconomos Exp $
// $Id: vxIntensityConverter.h,v 1.4 2005/09/23 14:33:18 geconomos Exp $
