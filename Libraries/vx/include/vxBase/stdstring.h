// $Id: stdstring.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Jeff M. (jmeade@viatronix.com)

#ifndef Std_Strings_h
#define Std_Strings_h

// includes

// forward declarations
inline std::string tolower(const std::string & sIn);
inline std::string toupper(const std::string & sIn);
inline int compare_nocase(const std::string & sLHS, const std::string & sRHS);
inline std::string operator+(const std::string & sLHS, const std::string & sRHS);


/** 
 * Converts string to lower cases
 * @param sIn input string
 * @return output string
 */
inline std::string tolower(const std::string & sIn)
{
  std::string sOut(sIn);
  std::string::iterator strIter(sOut.begin());
  std::string::iterator strEnd(sOut.end());

  for (; strIter != strEnd; ++strIter)
  {
    *strIter = tolower(*strIter);
  }

  return sOut;
} // tolower()


/** 
 * Converts string to upper cases
 * @param sIn input string
 * @return output string
 */
inline std::string toupper(const std::string & sIn)
{
  std::string sOut(sIn);
  std::string::iterator strIter(sOut.begin());
  std::string::iterator strEnd(sOut.end());

  for (; strIter != strEnd; ++strIter)
  {
    *strIter = toupper(*strIter);
  }

  return sOut;
} // toupper()


/** 
 * Compare strings ignoring lower/upper casing
 * @param sLHS input string 1
 * @param sRHS input string 2
 * @return result
 */
inline int compare_nocase(const std::string & sLHS, const std::string & sRHS)
{
  return (tolower(sLHS)).compare(tolower(sRHS));
}


/** 
 * Concatenate strings
 * @param sLHS input string 1
 * @param sRHS input string 2
 * @return output string
 */
inline std::string operator+(const std::string & sLHS, const std::string & sRHS)
{
  return std::string(sLHS) += sRHS;
}


#endif // ifndef Std_Strings_h


// Revision History:
// $Log: stdstring.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.3  2003/05/29 17:50:56  michael
// completed code review
//
// Revision 3.2  2003/05/13 16:30:36  michael
// code review
//
// Revision 3.1  2001/12/20 14:59:48  ingmar
// added class Sec_03_utils to structure the doxygen documentation
//
// Revision 3.0  2001/10/14 23:02:16  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
// 
//    Rev 2.1   Sep 19 2001 14:30:00   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:46:32   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:33:40   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:08:30  soeren
// renamed utils directory
//
// Revision 1.4  2001/04/12 19:06:45  jmeade
// corrected + operator
//
// Revision 1.3  2001/04/12 16:53:11  jmeade
// corrected header
//
// Revision 1.2  2001/04/12 15:57:36  jmeade
// string + string operator
//
// Revision 1.1  2001/04/12 15:23:56  jmeade
// helper functions for functionality missing from std::basic_string
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/stdstring.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: stdstring.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
