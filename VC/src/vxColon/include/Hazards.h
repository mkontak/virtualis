// $Id: Hazards.h,v 1.1.12.1 2007/04/03 20:35:21 jmeade Exp $
//
// Copyright© 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file 
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: Hazards.h
// Author: Frank Dachille (frank@viatronix.net)


#ifndef Hazards_h
#define Hazards_h

/// details possible hazards to an examination found during the scan
class Hazards 
{
public:
  /// enums for various types of hazards
  enum HazardEnum {
    INCOMPLETE_PROCESSING = 0,
    POOR_INFLATION_OR_MISSING_SEGMENT,
    PATIENT_MOVEMENT,
    FLUID_PUDDLE_MISSED_OR_INSUFFICIENTLY_TAGGED,
    WRONG_SCAN_PROTOCOL,
    UNUSUAL_AIR_HIGH_THRESHOLD_DETECTED,
    UNUSUAL_TISSUE_LOW_THRESHOLD_DETECTED,
    UNUSUAL_TISSUE_HIGH_THRESHOLD_DETECTED,
	  THICK_SLICE_INTERVAL,
  }; // enum HazardEnum

  /// The list of Hazards.
  static std::list<HazardEnum> m_hazardList;

// Member Functions
public:

  /// Read from disk.
  static bool Read(const std::string & sFilePrefix);

  /// Write to disk.
  static bool Write(const std::string & sFilePrefix);

  /// Add a warning.
  static void AddHazard(const HazardEnum eHazard);

}; // class Hazards
 
#endif 

// $Log: Hazards.h,v $
// Revision 1.1.12.1  2007/04/03 20:35:21  jmeade
// comments.
//
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 1.2  2005/06/09 14:00:30  dongqing
// add slice interval too thick warning
//
// Revision 1.1  2005/04/14 12:49:46  geconomos
// initial revision
//
// Revision 1.1  2002/03/08 22:34:04  michael
// added Hazards to 20_image to avoid forward dependencies
//
// Revision 3.4  2002/02/25 18:13:53  michael
// re-added Hazards class
//
// Revision 3.1  2001/12/20 15:00:15  ingmar
// added class Sec_03_utils to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:15  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.2   Oct 01 2001 16:10:14   soeren
// removed error class
// Resolution for 1022: Remove Error class
// 
//    Rev 2.1   Sep 19 2001 14:29:58   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:46:18   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:33:34   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:08:29  soeren
// renamed utils directory
//
// Revision 1.10  2001/05/21 13:00:23  frank
// Removed extra function.
//
// Revision 1.9  2001/04/13 00:31:09  jmeade
// file extensions, coding standards
//
// Revision 1.8  2001/04/11 18:40:32  dmitry
// Sec_1_utils inheritance added
//
// Revision 1.7  2001/04/09 18:02:57  dmitry
// Removed Revision History from Top of File
//
// Revision 1.6  2001/03/21 18:18:36  dmitry
// Updated datatypes as defined in Typedef.h
//
// Revision 1.5  2001/02/14 20:14:54  frank
// Added threshold hazards.
//
// Revision 1.4  2001/02/02 16:44:19  dave
// hazards updated on v2k side
//
// Revision 1.3  2001/02/01 17:47:09  frank
// Added a conversion function to get actual strings.
//
// Revision 1.2  2001/02/01 16:24:05  frank
// Added fluid puddle warning.
//
// Revision 1.1  2001/02/01 14:24:25  frank
// Renamed from Warnings
//
// Revision 1.1  2001/01/31 19:36:10  frank
// New class for enumerated warning messages (internationalization support)
//
// 
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/Hazards.h,v 1.1.12.1 2007/04/03 20:35:21 jmeade Exp $
// $Id: Hazards.h,v 1.1.12.1 2007/04/03 20:35:21 jmeade Exp $
