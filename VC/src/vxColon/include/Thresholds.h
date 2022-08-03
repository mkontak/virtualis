// $Id: Thresholds.h,v 1.3 2006/02/09 14:04:17 geconomos Exp $
//
// Copyright(c) 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author:     [Ingmar Bitter] [ingmar@viatronix.net]
//
//
// Complete History at bottom of file.


#ifndef Thresholds_h
#define Thresholds_h

/**
 *  class to hold the results of the Thresholder
 */
class Thresholds
{
public:
  /// default contructor
  Thresholds();

  /// copy contructor
  Thresholds(const Thresholds & src);

  /// read thresholds vector from disk
  void Read (const std::string & fileName);
  /// write thresholds vector to disk
  bool Write(std::string fileName);

  /// allow to direct threshold values to an output stream.
  std::ostream & AsOstream(std::ostream & os) const;

  friend std::ostream & operator << (std::ostream & os, const Thresholds & t) {
    return (t.AsOstream(os));
  }

private:
  /// Builds ascii header containing the Thresholds.
  std::string BuildAsciiHeader();
  /// Reads the header containing the Thresholds as ascii text.
  bool ReadAsciiHeader(const int1 * headerPtr);

public: /// data members
  /// this vector holds the actualy threshold values
  std::vector<int4> m_vThresholds;
  /// The pixels of this PNGImage icon is set by the Thresholder to display the Histogram
  //PNGImage m_icon;

}; // endThresholds

#endif // Thresholds_h


// $Log: Thresholds.h,v $
// Revision 1.3  2006/02/09 14:04:17  geconomos
// removed inclusion of pngimage.h
//
// Revision 1.2  2006/02/03 19:27:53  frank
// removed png library
//
// Revision 1.1  2005/08/05 12:54:57  geconomos
// moved from vx repository
//
// Revision 3.2  2002/02/20 22:46:01  michael
// removed include of Hazards which isn't used
//
// Revision 3.1  2001/12/21 17:50:10  ingmar
// added class Sec_50_dataStruct to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:28  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:48  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Oct 01 2001 16:10:20   soeren
// removed error class
// Resolution for 1022: Remove Error class
// 
//    Rev 2.0   Sep 16 2001 23:49:16   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:36:46   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:07:24  soeren
// renamed datastruct directory
//
// Revision 1.12  2001/05/25 14:50:26  lihong
// coding standard
//
// Revision 1.11  2001/05/24 22:36:57  lihong
// changed most variables for coding standard
//
// Revision 1.10  2001/04/12 18:52:39  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.9  2001/04/12 15:50:50  dave
// added namespace vx for exceptions
//
// Revision 1.8  2001/04/12 14:38:13  dmitry
// Sec_6_dataStruct base class added
//
// Revision 1.7  2001/04/05 15:45:50  dave
// updated header
//
// Revision 1.6  2001/03/29 16:18:21  dmitry
// Updated datatypes, implemented proper casting
//
// Revision 1.5  2000/12/11 19:55:08  dave
// new libpng, exception, file versions etc
//
// Revision 1.4  2000/11/22 06:44:22  dave
// Updated PNGImage class
//
// Revision 1.3  2000/10/31 13:53:10  frank
// Merged in new skeleton and segment structure
//
// Revision 1.2.14.1  2000/10/28 19:31:07  ingmar
// merged in changes from v1k-newSegments branch
//
// Revision 1.2  2000/09/26 01:17:49  kevin
// Result of StaticLibraries branch Merge
//
// Revision 1.1.1.1.2.1  2000/09/25 18:56:36  kevin
// Made Static Libraries
//
// Revision 1.1.1.1  2000/08/11 22:15:51  ingmar
// unit tests complete
//
//
// 10    9/04/00 11:10p Ingmar
// now works with new naming sceme
//
// 9     8/31/00 4:12p Lihong
// changed public variable m_vThresholds as m_thresholds
//
// 8     8/22/00 5:50p Jay
// Thresholds now reads/writes in the PNG file format and has an icon for
// the thresholds and histogram found by the thresholder
//
// 7     8/22/00 4:56p Kevin
// THRESHOLDS -> VOLUME
//
// 6     8/21/00 5:42p Antonio
// updated header
//
// *****************  Version 5  *****************
// User: Sarang       Date: 8/20/00    Time: 6:49p
// Updated in $/v2k/src/Thresholds
// The whole pipeline now works under linux+gcc. Removed tons of bugs and
// syntax errors found by gcc.
//
// *****************  Version 4  *****************
// User: Antonio      Date: 8/11/00    Time: 4:08p
// Updated in $/v2k/src/Thresholds
// added header and history to end of file.
//
// **********************
// Label: last_void_*_volume_version
// User: Jay             Date:  8/11/00  Time:  1:19a
// Labeled 'last_void_*_volume_version'
// Label comment:
//   last void * volume version
// about to replace with template volume
//
// *****************  Version 3  *****************
// User: Lihong          Date:  7/28/00  Time:  2:35p
// Checked in $/v2k/src/Thresholds
// Comment:
//   made Thresholds class work
//
// *****************  Version 2  *****************
// User: Ingmar          Date:  7/28/00  Time: 12:35p
// Checked in $/v2k/src/Thresholds
// Comment:
//   added Thresholds class to hold the threshold vector
//
// *****************  Version 1  *****************
// User: Ingmar          Date:  7/28/00  Time: 10:47a
// Created Thresholds.h
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/Thresholds.h,v 1.3 2006/02/09 14:04:17 geconomos Exp $
// $Id: Thresholds.h,v 1.3 2006/02/09 14:04:17 geconomos Exp $
