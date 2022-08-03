// $Id: CmdLineSwitches.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Created:
// Author: [Name] [e-mail]
//
// Complete history at bottom of file.


#ifndef CmdLineSwitches_h     // prevent multiple includes
#define CmdLineSwitches_h

//includes

/**
 * Command line management class.
 *
 * switches can be any length, but have to start with a dash (-)
 * they can have
 * no ( -h )
 * one ( -px 50 ) or
 * multiple ( -bg "1 1 0" ) arguments
 */
class VX_BASE_DLL CmdLineSwitches 
{
 public:
  /// Runs a test execution of the CmdLineSwitches class
  static void Demo (int argc, char *argv[]);
  /// Variable argument length constructor.
  CmdLineSwitches(const char * , ... );

  /// destructor
  ~CmdLineSwitches();

  /// returns the whole commandline in one string
  inline std::string GetCommandLine();

  void ReadCommandLine(int argc, char * argv[]);
  void ReadParameterFile(const std::string & parameterFileName);
  
  void Hide (const std::string & sId);
  void Show (const std::string & sId);
  void SetSwitchValue(const std::string & idStr, std::string valueStr);
  
  /// return defined type casting of a switch value
  inline std::string     ToString   (const std::string & id);
  inline bool            ToBool     (const std::string & id);
  inline int4            ToInt      (const std::string & id);
  inline float8          ToDouble   (const std::string & id);
  inline uint8           ToULong    (const std::string & id);
  
  /// returns the description for an argument name or NULL
  const char * DescriptionStr(const std::string & idStr);

  std::ostream & ToOstream(std::ostream & os) const;
  /**
   * @param os
   * @param s
   * @return
   */
  inline friend std::ostream & operator << (std::ostream & os, const CmdLineSwitches & s)
  {
    return s.ToOstream(os);
  } // <<

  // returns vector of "exName triple1 triple2, etc"
  std::vector<std::string> ToVector() const;

  ///
  void Usage();

  /// write switch file
  void WriteSwitchFile(const std::string & fileName);
 
 protected:  
   /// returns index value of switch
   int IdIndex(const std::string & id);
 
 protected:
  /**
   * Contains all switch information.
   */
  class Switch 
  {
  public:
	  Switch (): m_bHide(false) {};
  public:
    std::string m_id;
    std::string m_value;
    std::string m_description;
	  bool m_bHide;
  };

  std::string m_argv0;                 // executable name
  std::vector<Switch>  m_switchMatrix; // contains all switches
}; // class CmdLineSwitches

#include "CmdLineSwitches.inl"

#endif  // CmdLineSwitches_h


// $Log: CmdLineSwitches.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.3  2003/05/13 21:09:18  dongqing
// code review
//
// Revision 3.2  2002/09/30 20:45:08  ingmar
// moved ToAscii() to 03_utils/utilities.h
//
// Revision 3.1  2001/12/20 15:00:15  ingmar
// added class Sec_03_utils to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:14  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 19 2001 14:29:52   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:46:08   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:33:26   ingmar
// Initial revision.
// Revision 1.2  2001/09/13 21:34:33  ingmar
// removed ToStrstream( )
//
// Revision 1.1  2001/09/01 00:08:28  soeren
// renamed utils directory
//
// Revision 1.21  2001/04/25 19:48:39  dmitry
// Created Hide and Show methods.
//
// Revision 1.20  2001/04/12 18:50:09  ingmar
// vx::Exception -> ex::Exception
//
// Revision 1.19  2001/04/12 15:50:50  dave
// added namespace vx for exceptions
//
// Revision 1.18  2001/04/11 18:40:32  dmitry
// Sec_1_utils inheritance added
//
// Revision 1.17  2001/04/09 22:23:15  dave
// changed from abort to exceptions
//
// Revision 1.16  2001/04/08 21:38:19  dave
// updated doc headers
//
// Revision 1.15  2001/04/05 17:59:53  dmitry
// updated datatypes
//
// Revision 1.14  2001/03/15 18:03:22  soeren
// Added new Function GetCommandLine()
//
// Revision 1.13  2001/03/15 17:04:18  soeren
// changed WriteSwitchFile Function from protected to public
//
// Revision 1.12  2001/02/28 21:06:56  dmitry
// Replaced all (char *) with std::string (except for the constructor),
// rewrote ReadParameterFile method to use istrstream
//
// Revision 1.11  2001/02/28 00:10:04  ingmar
// Error::Abrt willnow be called upon using a nonexisting switch
//
// Revision 1.10  2001/02/26 15:50:28  dmitry
// Changed method names to prog. conventions, added comments.
//
// Revision 1.9  2001/02/23 20:09:36  dmitry
// Replaced char ** array by std::string
//
// Revision 1.8  2001/02/22 16:17:23  ingmar
// oops forgot to make it compile
//
// Revision 1.7  2001/02/22 16:13:33  ingmar
// new internal switch class
//
// Revision 1.6  2000/12/19 20:38:33  dave
// added some stuff
//
// Revision 1.5  2000/10/23 18:01:33  ingmar
// added asBool member function
//
// Revision 1.4  2000/10/23 16:34:14  dave
// asBool added to CmdLine
//
// Revision 1.3  2000/09/26 02:58:13  kevin
// Result of StaticLibraries branch Merge
//
// Revision 1.2  2000/09/26 01:17:48  kevin
// Result of StaticLibraries branch Merge
//
// Revision 1.1.1.1.2.1  2000/09/25 18:56:35  kevin
// Made Static Libraries
//
// Revision 1.1.1.1  2000/08/11 22:15:50  ingmar
// unit tests complete
// Revision 1.1.1.1.2.1  2000/09/25 18:56:35  kevin
// Made Static Libraries
//
// Revision 1.1.1.1  2000/08/11 22:15:50  ingmar
// unit tests complete
//
//
// 8     9/07/00 11:27a Lihong
// added destructor to delete allocated memory
//
// 7     8/29/00 12:24p Antonio
// updated header
//
// 6     8/21/00 12:25a Sarang
// modified header
//
// *****************  Version 5  *****************
// User: Sarang       Date: 8/20/00    Time: 6:49p
// Updated in $/v2k/src/Utils
// The whole pipeline now works under linux+gcc. Removed tons of bugs and
// syntax errors found by gcc.
//
// *****************  Version 4  *****************
// User: Antonio      Date: 8/11/00    Time: 10:17a
// Updated in $/vx/src/Utils
// updated header of file.
//
// *****************  Version 3  *****************
// User: Antonio      Date: 8/02/00    Time: 10:11p
// Updated in $/vx/src/Utils
// added header and complete history to end of file
//
// *****************  Version 2  *****************
// User: Ingmar          Date:  7/19/00  Time: 10:30a
// Checked in $/v2k/src/Utils
// Comment:
//   added CmdLineSwitches to Utils library
//
// *****************  Version 1  *****************
// User: Ingmar          Date:  7/19/00  Time:  9:04a
// Created CmdLineSwitches.h
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/CmdLineSwitches.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: CmdLineSwitches.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
