// $Id: v3D_IntensityConverter.h,v 1.3 2006/10/04 18:29:05 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (meissner@viatronix.com)

#pragma once


// includes
#include "vxIntensityConverter.h"
#include "v3D_VolumeHeader.h"


// namespaces
OPEN_VISUALIZATION_NS


// class definition
public ref class IntensityConverter
{
// functions
public:
  
  /// constructor
  IntensityConverter() {};
  
  /// destructor
  virtual ~IntensityConverter() {};

// properties
public:

  /// returns the minimum value
  static int GetMinValue(VolumeHeader ^ mpHeader);

  /// returns the minimum value
  static int GetMinValue(VolumeHeader ^ mpHeader, bool bLevel);

  /// returns the maximum value
  static int GetMaxValue(VolumeHeader ^ mpHeader);

  /// returns the maximum value
  static int GetMaxValue(VolumeHeader ^ mpHeader, bool bLevel);

  /// returns the window value
  static float FromShort(int iInput, VolumeHeader ^ mpHeader);

  /// returns the window value
  static float FromShort(int iInput, VolumeHeader ^ mpHeader, bool bLevel);
  
  /// clamp a short value short
  static int ClampShort(int iInput, VolumeHeader ^ mpHeader);

  /// clamp a short value short
  static int ClampShort(int iInput, VolumeHeader ^ mpHeader, bool bLevel);

  /// sets the window value
  static int ToShort(float fInput, VolumeHeader ^ mpHeader);

  /// sets the window value
  static int ToShort(float fInput, VolumeHeader ^ mpHeader, bool bLevel);

  /// returns the units for the specified modality
  static System::String ^ GetUnits(VolumeHeader ^ mpHeader);

  /// returns the units for the specified modality
  static System::String ^ GetUnits(System::String ^ sModality);

  /// convert raw value to modality value
  static int RawToModality( int iRaw, VolumeHeader ^ mpHeader );

  /// convert modality value to raw value
  static int ModalityToRaw( int iModality, VolumeHeader ^ mpHeader );

}; // IntensityConverter


CLOSE_VISUALIZATION_NS 


// $Log: v3D_IntensityConverter.h,v $
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
// Revision 1.10  2003/06/05 17:35:23  michael
// transition from short to int
//
// Revision 1.9  2003/06/04 18:32:39  michael
// extended the intensity converter to take a volume header so that not only
// modality but number of bits used can also be used.
//
// Revision 1.8  2003/05/16 13:11:04  frank
// formatting
//
// Revision 1.7  2003/05/13 13:28:45  michael
// code review
//
// Revision 1.6  2003/05/06 18:51:41  geconomos
// Coding standards
//
// Revision 1.5  2003/04/09 15:56:05  geconomos
// Issue# 3037: Display units for window level.
//
// Revision 1.4  2002/10/10 15:14:22  michael
// Extended intensity converter by min/max methods
//
// Revision 1.3  2002/10/07 22:27:49  michael
// extended intensity converter to clamp values
//
// Revision 1.2  2002/10/07 21:02:58  geconomos
// Made the ToXXX functions static.
//
// Revision 1.1  2002/10/07 14:05:00  michael
// Added initial version to be used on the GUI level
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_IntensityConverter.h,v 1.3 2006/10/04 18:29:05 gdavidson Exp $
// $Id: v3D_IntensityConverter.h,v 1.3 2006/10/04 18:29:05 gdavidson Exp $