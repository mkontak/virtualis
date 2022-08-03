// $Id: v3D_IntensityConverter.cpp,v 1.3 2006/10/04 18:29:05 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (meissner@viatronix.com)


// includes
#include "StdAfx.h"
#include "v3D_IntensityConverter.h"


// namespaces
USING_VISUALIZATION_NS


/**
 * Get the minimum value
 * @param modality
 * @return short output
 */
int IntensityConverter::GetMinValue(VolumeHeader ^ mpHeader)
{
  return vxIntensityConverter::GetMinValue(*reinterpret_cast<vxVolumeHeader*>(mpHeader->GetVolumeHeaderPtr().ToPointer()));
}


/**
 * Get the minimum value
 * @param modality
 * @param level?
 * @return short output
 */
int IntensityConverter::GetMinValue(VolumeHeader ^ mpHeader, bool bLevel)
{
  return vxIntensityConverter::GetMinValue(*reinterpret_cast<vxVolumeHeader*>(mpHeader->GetVolumeHeaderPtr().ToPointer()), bLevel);
}


/**
 * Get the maximum value
 * @param modality
 * @return short output
 */
int IntensityConverter::GetMaxValue(VolumeHeader ^ mpHeader)
{
  return vxIntensityConverter::GetMaxValue(*reinterpret_cast<vxVolumeHeader*>(mpHeader->GetVolumeHeaderPtr().ToPointer()));
}


/**
 * Get the maximum value
 * @param modality
 * @param level?
 * @return short output
 */
int IntensityConverter::GetMaxValue(VolumeHeader ^ mpHeader, bool bLevel)
{
  return vxIntensityConverter::GetMaxValue(*reinterpret_cast<vxVolumeHeader*>(mpHeader->GetVolumeHeaderPtr().ToPointer()), bLevel);
}


/**
 * Convert from short to ZeroOne, depending on modality
 * @param short input
 * @param modality
 * @return float output
 */
float IntensityConverter::FromShort(int iInput, VolumeHeader ^ mpHeader)
{
  return vxIntensityConverter::FromShort(iInput, *reinterpret_cast<vxVolumeHeader*>(mpHeader->GetVolumeHeaderPtr().ToPointer()));
}


/**
 * Convert from short to ZeroOne, depending on window/level and modality
 * @param short input
 * @param modality
 * @param level?
 * @return float output
 */
float IntensityConverter::FromShort(int iInput, VolumeHeader ^ mpHeader, bool bLevel)
{
  return vxIntensityConverter::FromShort(iInput, *reinterpret_cast<vxVolumeHeader*>(mpHeader->GetVolumeHeaderPtr().ToPointer()), bLevel);
}


/**
 * Clamp short, depending on modality
 * @param short input
 * @param modality
 * @return short output
 */
int IntensityConverter::ClampShort(int iInput, VolumeHeader ^ mpHeader)
{
  return vxIntensityConverter::ClampShort(iInput, *reinterpret_cast<vxVolumeHeader*>(mpHeader->GetVolumeHeaderPtr().ToPointer()));
}


/**
 * Clamp short, depening on modality and window/level
 * @param short input
 * @param modality
 * @param level?
 */
int IntensityConverter::ClampShort(int iInput, VolumeHeader ^ mpHeader, bool bLevel)
{
  return vxIntensityConverter::ClampShort(iInput, *reinterpret_cast<vxVolumeHeader*>(mpHeader->GetVolumeHeaderPtr().ToPointer()), bLevel);
}


/**
 * Convert from ZeroOne to short, depending on modality
 * @param float input
 * @param string
 * @return short output
 */
int IntensityConverter::ToShort(float fInput, VolumeHeader ^ mpHeader)
{
  return vxIntensityConverter::ToShort(fInput, *reinterpret_cast<vxVolumeHeader*>(mpHeader->GetVolumeHeaderPtr().ToPointer()));
}


/**
 * Convert from ZeroOne to short, depending on modality and window/level
 * @param float input
 * @param modality
 * @param level?
 * @return short output
 */
int IntensityConverter::ToShort(float fInput, VolumeHeader ^ mpHeader, bool bLevel)
{
  return vxIntensityConverter::ToShort(fInput, *reinterpret_cast<vxVolumeHeader*>(mpHeader->GetVolumeHeaderPtr().ToPointer()), bLevel);
}


/**
 * Returns the units for the specified modality.
 * @param modality
 * @return string for specified modality
 */
System::String ^ IntensityConverter::GetUnits(System::String ^ mpsModality)
{
  return gcnew System::String( vxIntensityConverter::GetUnit(ss(mpsModality)).c_str() );
} // GetUnits()


/**
* Returns the units for the specified modality.
* @param modality
* @return string for specified modality
*/
System::String ^ IntensityConverter::GetUnits(VolumeHeader ^ mpHeader)
{
  return gcnew System::String( vxIntensityConverter::GetUnit(*reinterpret_cast<vxVolumeHeader*>(mpHeader->GetVolumeHeaderPtr().ToPointer())).c_str() );
} // GetUnits()


/**
 * convert raw value to modality value
 * @param iRaw the input raw value
 * @param mpHeader the volume header
 * @return modality value
 */
int IntensityConverter::RawToModality( int iRaw, VolumeHeader ^ mpHeader )
{
  return vxIntensityConverter::RawToModality( iRaw, * reinterpret_cast< vxVolumeHeader * >( mpHeader->GetVolumeHeaderPtr().ToPointer() ) );
} // RawToModality()


/**
 * convert modality value to raw value
 * @param iModality the input modality value
 * @param mpHeader the volume header
 * @return raw value
 */
int IntensityConverter::ModalityToRaw( int iModality, VolumeHeader ^ mpHeader )
{
  return vxIntensityConverter::ModalityToRaw( iModality, * reinterpret_cast< vxVolumeHeader * >( mpHeader->GetVolumeHeaderPtr().ToPointer() ) );
} // ModalityToRaw()


// $Log: v3D_IntensityConverter.cpp,v $
// Revision 1.3  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.2  2004/03/24 21:50:34  frank
// fixed typo in modality to raw conversion
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.11  2003/06/05 17:35:23  michael
// transition from short to int
//
// Revision 1.10  2003/06/04 18:32:39  michael
// extended the intensity converter to take a volume header so that not only
// modality but number of bits used can also be used.
//
// Revision 1.9  2003/05/16 13:11:04  frank
// formatting
//
// Revision 1.8  2003/05/13 13:28:45  michael
// code review
//
// Revision 1.7  2003/05/06 21:27:04  michael
// completed code review
//
// Revision 1.6  2003/05/06 18:51:41  geconomos
// Coding standards
//
// Revision 1.5  2003/04/09 14:40:03  geconomos
// Issue# 3037: Display units for window level.
//
// Revision 1.4  2002/10/10 15:14:22  michael
// Extended intensity converter by min/max methods
//
// Revision 1.3  2002/10/08 19:02:00  michael
// fixed copy&paste error
//
// Revision 1.2  2002/10/07 22:27:49  michael
// extended intensity converter to clamp values
//
// Revision 1.1  2002/10/07 14:05:00  michael
// Added initial version to be used on the GUI level
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_IntensityConverter.cpp,v 1.3 2006/10/04 18:29:05 gdavidson Exp $
// $Id: v3D_IntensityConverter.cpp,v 1.3 2006/10/04 18:29:05 gdavidson Exp $
