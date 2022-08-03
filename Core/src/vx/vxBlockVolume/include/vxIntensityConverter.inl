// $Id: vxIntensityConverter.inl,v 1.8 2007/03/09 16:27:20 dongqing Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (mailto:michael@viatronix.com)


// includes
//#include "stdafx.h"
//#include "vxIntensityConverter.h"
//#include "vxVolumeHeader.h"


// namespaces
using std::string;


// defines
#define FILE_REVISION "$Revision: 1.8 $"


/**
 * Initialize from short doing any necessary modality conversion
 * @param iInput initial value
 * @param header volume header
 * @param bLevel level flag
 * @return resulting short
 */
int4 vxIntensityConverter::InitFromShort(const int4 iInput, 
                                         const vxVolumeHeader & header, 
                                         const bool bLevel)
//*******************************************************************
{
  return iInput - GetBias(header, bLevel);
} // InitFromShort()


/**
 * Returns the Units of the given modality, e.g. HU for CT
 * @param sModality modality
 * @return resulting units string
 */
string vxIntensityConverter::GetUnit(const std::string sModality)
//*******************************************************************
{
  if (sModality == "CT")
  {
    return "HU";
  }

  return "RAW";
} // GetUnit()


/**
 * Returns the Units of the given modality, e.g. HU for CT
 * @param header volume header
 * @return resulting units string
 */
string vxIntensityConverter::GetUnit(const vxVolumeHeader & header)
//*******************************************************************
{
  string sModality;
  header.GetHeaderEntry(vxVolumeHeader::DataModality, sModality);

  return GetUnit(sModality);
} // GetUnit()


/**
 * Get minimum value for this modality and window/level
 * @param header volume header
 * @param bLevel level flag
 * @return resulting short
 */
int4 vxIntensityConverter::GetMinValue(const vxVolumeHeader & header,
                                       const bool bLevel)
//*******************************************************************
{
  return - GetBias(header, bLevel);
} // GetMinValue()


/**
 * Get minimum value for this modality and window/level
 * @param volume given intensity volume
 * @param bLevel flag
 * @return resulting short
 */
int4 vxIntensityConverter::GetMinValue(const vxBlockVolume<uint2> & volume,
                                       const bool bLevel)
//*******************************************************************
{
  return GetMinValue(volume.GetHeader(), bLevel);
} // GetMinValue()


/**
 * Get maximum value for this modality and window/level
 * @param header volume header
 * @param bLevel flag
 * @return resulting short
 */
int4 vxIntensityConverter::GetMaxValue(const vxVolumeHeader & header,
                                       const bool bLevel)
//*******************************************************************
{
  int4 iMax(0);
  string sModality;
  header.GetHeaderEntry(vxVolumeHeader::DataModality, sModality);

  //Use the iSignificantBitsPerVoxel value here
  int4 iSignificantBitsPerVoxel = header.GetSignificantBitsPerVoxel();
  if ( iSignificantBitsPerVoxel != 0 )
  {
    // ok, number of stored bits is set but is there also a "bias" value?
    std::string sBias;
    header.GetHeaderEntry(vxVolumeHeader::Bias, sBias);
    int4 iBias(atoi(sBias.c_str()));
    if (sModality == "CT")
    {
      // some jerky temporary version that hard coded CT bias to be 1000!!!
      iMax = 4095 - GetBias(header, bLevel);
    }
    else
    {
			int4 iVersion(0);
			header.GetVariableVolumeHeader().GetHdrVar("version", iVersion);
			if( iVersion >= 5.0 )
			{
				header.GetVariableVolumeHeader().GetHdrVar("maxValueRange", iMax);

				if( iMax < 4096 ) 
				{
					iMax = 4095 - GetBias(header, bLevel);
				}
				else
				{
					iMax = 65535 - GetBias(header, bLevel);
				}
			}
			else
			{ /// for old version of volume header
				iMax = pow(2.0F, iSignificantBitsPerVoxel ) - 1 - GetBias(header, bLevel);
			}
    }
  }
  else
  {
    // older volume version!!!
    if (sModality == "CT")
    {
      iMax = (bLevel == true)? 3095 : 4095;
    }
    else
    {
      vxVolumeHeader::VoxelTypeEnum val = header.GetVoxelTypeEnum();

      if (header.GetVoxelTypeEnum() == vxVolumeHeader::UINT1 || header.GetVoxelTypeEnum() == vxVolumeHeader::INT1)
      {
        iMax = 255;
      }
      else
      {
        iMax = 65535;
      }
    }
  }

  return iMax;
} // GetMaxValue() /


/**
 * Get maximum value for this modality and window/level
 * @param volume given intensity volume
 * @param bLevel flag
 * @return resulting short
 */
int4 vxIntensityConverter::GetMaxValue(const vxBlockVolume<uint2> & volume,
                                       const bool bLevel)
//*******************************************************************
{
  return GetMaxValue(volume.GetHeader(), bLevel);
} // GetMaxValue()


/**
 * Convert from short to ZeroOne
 * @param iInput input short
 * @param header volume header
 * @param bLevel flag
 * @return resulting ZeroOne
 */
ZeroOne vxIntensityConverter::FromShort(const int4 iInput,
                                        const vxVolumeHeader & header,
                                        const bool bLevel)
//*******************************************************************
{
  int4 iMax(GetMaxValue(header, false));
  if (iMax != 0)
  {
    return (iInput + GetBias(header, bLevel))/(float4)iMax;
  }

  return 0;
} // FromShort()


/**
 * Convert from short to ZeroOne
 * @param iInput input short
 * @param header volume header
 * @param bLevel flag
 * @return resulting short
 */
int4 vxIntensityConverter::ClampShort(const int4 iInput,
                                      const vxVolumeHeader & header,
                                      const bool bLevel)
//*******************************************************************
{
  return Bound((int4)GetMinValue(header, bLevel), iInput, (int4)GetMaxValue(header, bLevel));
} // ClampShort()


/**
 * Convert from ZeroOne to short
 * @param fInput ZeroOne
 * @param header volume header
 * @param bLevel flag
 * @return resulting short
 */
int4 vxIntensityConverter::ToShort(const ZeroOne fInput,
                                   const vxVolumeHeader & header,
                                   const bool bLevel)
//*******************************************************************
{
  return static_cast<int4>(0.5F + fInput*GetMaxValue(header, false) - GetBias(header, bLevel));
} // ToShort()


/**
* Initialize from short doing any necessary modality conversion
* @param header volume header
* @param bLevel level flag
* @return resulting value
*/
int4 vxIntensityConverter::GetBias(const vxVolumeHeader & header, 
                                   const bool bLevel)
//*******************************************************************
{
  if (bLevel == false)
  {
    return 0;
  }

  string sBias;
  header.GetHeaderEntry(vxVolumeHeader::Bias, sBias);
  return atoi(sBias.c_str());
} // GetBias()


/**
 * convert raw value to modality value
 * @param iRaw : raw value
 * @param header : vxvolume header
 * @return value in modality units
 */
int4 vxIntensityConverter::RawToModality(const int4 iRaw, const vxVolumeHeader & header)
//*******************************************************************
{
  return int4(iRaw - GetBias(header, true));

  //return vxIntensityConverter::ToShort(vxIntensityConverter::FromShort(iRaw, header, false), header, true );
} // RawToModality()


/**
 * convert modality value to raw value
 * @param iModality : modality value
 * @param header : vxvolume header
 * @return raw value
 */
int4 vxIntensityConverter::ModalityToRaw(const int4 iModality, const vxVolumeHeader & header)
//*******************************************************************
{
  return int4(iModality + GetBias(header, true));

  //return vxIntensityConverter::ToShort(vxIntensityConverter::FromShort(vxIntensityConverter::ClampShort(iModality, header, true), header, true), header);
} // ModalityToRaw()


#undef FILE_REVISION


// Revision History:
// $Log: vxIntensityConverter.inl,v $
// Revision 1.8  2007/03/09 16:27:20  dongqing
// fix the code review error
//
// Revision 1.7  2007/02/14 20:48:01  romy
// Threshold values are now using 'iSignificantBitsPerVoxel' instead of  'bitStored' value from the Volume Header. Issue# 5412
//
// Revision 1.6  2006/10/30 23:02:25  dongqing
// fix the RawToModality and ModalityToRaw bug
//
// Revision 1.5  2006/01/18 20:36:48  geconomos
// Initial effort to migrate to visual studio 2005 underway!
//
// Revision 1.4  2005/05/23 11:15:38  michael
// Reworked Scale and Bias to always be returned
//
// Revision 1.3  2005/05/10 20:00:24  michael
// Made BitStored part of the enum of header vars instead of having a dedicated
// access function.
//
// Revision 1.2  2005/04/20 12:55:48  michael
// Fixed the rounding problems when going to a short.
//
// Revision 1.1  2004/10/26 13:18:09  michael
// adding the inl file
//
// Revision 1.3  2004/08/31 15:55:13  michael
// Adjusted code to correclty load old 8 bit .sld/.slc into vxBlockVolume
//
// Revision 1.2  2004/03/24 21:51:06  frank
// fixed typo in modality to raw conversion
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.20  2004/01/22 15:42:35  wenli
// Add Raw and Modality conversion
//
// Revision 1.19  2003/12/23 21:34:18  michael
// checked in code to take into account the BIAS in the VarMap
//
// Revision 1.18  2003/12/22 14:16:20  michael
// fixed bug in converting ZeroOne into a short for a level value
//
// Revision 1.17  2003/12/19 00:36:19  michael
// Introduced a GetBias() to account for the bias for window/level and simplify
// the overall mechanism. Furthermore, it easily allows to implement a bias to
// account for different vendors.
//
// Revision 1.16  2003/07/03 17:24:47  michael
// added two new min/max functions, based on volume, not volume header
//
// Revision 1.15  2003/06/05 17:35:23  michael
// transition from short to int
//
// Revision 1.14  2003/06/05 13:31:20  michael
// fixed buyg in the intensity conversion (float cast for denominator)
//
// Revision 1.13  2003/06/04 18:32:39  michael
// extended the intensity converter to take a volume header so that not only
// modality but number of bits used can also be used.
//
// Revision 1.12  2003/05/27 14:12:00  michael
// completed code review
//
// Revision 1.11  2003/05/16 13:16:20  frank
// formatting
//
// Revision 1.10  2003/05/16 11:54:11  michael
// code review
//
// Revision 1.9  2003/05/15 20:32:34  dongqing
// code review
//
// Revision 1.8  2003/01/27 23:18:49  michael
// added annotation stuff: flexible text location (handle index), multiple text lines etc.
//
// Revision 1.7  2002/11/22 19:08:27  michael
// fixed return of max for non CT --> 4095
//
// Revision 1.6  2002/11/07 13:59:07  geconomos
// Fxied incorrect value in GetMaxValue (Michael).
//
// Revision 1.5  2002/10/24 01:23:21  michael
// added 0.5 for rounding, was missing
//
// Revision 1.4  2002/10/10 15:14:22  michael
// Extended intensity converter by min/max methods
//
// Revision 1.3  2002/10/08 19:04:19  michael
// fixed bug using () around the ? operation
//
// Revision 1.2  2002/10/07 22:28:55  michael
// extended intensity converter to clamp values
//
// Revision 1.1  2002/10/07 14:09:38  michael
// Added initial version to be used on the GUI level
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/vxIntensityConverter.inl,v 1.8 2007/03/09 16:27:20 dongqing Exp $
// $Id: vxIntensityConverter.inl,v 1.8 2007/03/09 16:27:20 dongqing Exp $
