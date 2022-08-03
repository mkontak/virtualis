// $Id: PersonName.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Dave (dave@viatronix.com)

/*
  Class for storing the name of a person.
  Created for parsing DICOM formatted names into a class.
*/

#ifndef PersonName_h
#define PersonName_h


// includes



// class definition
class VX_BLOCK_VOLUME_DLL PersonName
{
public:

  /// default constructor
  PersonName();

  /// constructor
  PersonName(const std::string & sLast, const std::string & sFirst,
             const std::string & sMiddle = "", const std::string & sPrefix = "",
             const std::string & sSuffix = "");

  /// constructor for Dicom formatted strings
  PersonName(const std::string & sDicomName);

  /// operator==
  bool operator==(const PersonName & other) const;

  /// operator!=
  bool operator!=(const PersonName & other) const;

  /// get first name
  const std::string & GetFirst() const { return m_sFirst; }

  /// set first name
  void SetFirst(const std::string & sFirst) { m_sFirst = sFirst; }

  /// get middle name
  const std::string & GetMiddle() const { return m_sMiddle; }

  /// set middle name
  void SetMiddle(const std::string & sMiddle) { m_sMiddle = sMiddle; }

  /// get last name
  const std::string & GetLast() const { return m_sLast; }

  /// set last name
  void SetLast(const std::string & sLast) {m_sLast = sLast; }

  /// get prefix
  const std::string & GetPrefix() const { return m_sPrefix; }

  /// set prefix
  void SetPrefix(const std::string & sPrefix) { m_sPrefix = sPrefix; }

  /// get suffix
  const std::string & GetSuffix() const { return m_sSuffix; }

  /// set suffix
  void SetSuffix(const std::string & sSuffix) { m_sSuffix = sSuffix; }

  /// helper function: returns the full name formatted as "prefix first middle last suffix"
  std::string GetFullName() const;

  /// helper function: returns the full name formatted as "last^first middle"
  std::string GetLastFirstName() const;

  /// helper function: returns full name as "last, first middle suffix"
  std::string GetLastFirstMidSuffix() const;

  /// helper function: returns a Dicom formatted name string
  std::string GetDicomName() const;

  /// initializes the class using the dicom name specified
  void SetDicomName(const std::string & sDicomname);

private:

  /// parse next
  std::string ParseNext(std::string & sName);

// data
protected:

  /// last name
  std::string m_sLast;

  /// first name
  std::string m_sFirst;

  /// middle name
  std::string m_sMiddle;

  /// prefix
  std::string m_sPrefix;

  /// suffix
  std::string m_sSuffix;
}; // PersonName


/** 
 * operator<<
 * @param ostream
 * @param person name object
 * @retun ostream
 */
inline std::ostream & operator << (std::ostream & os, PersonName & pn) 
{
  os << pn.GetDicomName();
  return os;
} // operator <<


/** 
 * operator>>
 * @param istream
 * @param person name object
 * @retun istream
 */
inline std::istream & operator >>(std::istream & is, PersonName & pn) 
{
  std::string name;
  is >> name;

  pn = PersonName(name);

  return is;
}


#endif // PersonName_h


// $Log: PersonName.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.7  2003/05/16 13:08:20  frank
// code formatting
//
// Revision 3.6  2003/05/16 11:53:11  michael
// code review
//
// Revision 3.5  2003/05/13 19:09:08  michael
// code review
//
// Revision 3.4  2002/07/10 21:24:19  geconomos
// exported class.
//
// Revision 3.3  2002/05/20 03:31:15  mkontak
// Added SetDicomName()
//
// Revision 3.2  2002/02/20 01:43:38  michael
// includes/spaces
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
//    Rev 2.1   Sep 19 2001 14:30:00   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:46:20   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:33:38   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:08:30  soeren
// renamed utils directory
//
// Revision 1.11  2001/05/20 20:14:32  dave
// more standards compliant
//
// Revision 1.10  2001/05/15 14:13:17  dave
// added comparison operators
//
// Revision 1.9  2001/04/11 18:40:32  dmitry
// Sec_1_utils inheritance added
//
// Revision 1.8  2001/04/08 21:38:20  dave
// updated doc headers
//
// Revision 1.7  2001/03/21 18:18:36  dmitry
// Updated datatypes as defined in Typedef.h
//
// Revision 1.6  2001/02/27 16:36:51  jmeade
// corrected comment on GetLastFirstName() (it does NOT return "last, first middle");
// added GetLastFirstMidSuffix() (which returns "last, first mid suffix")
//
// Revision 1.5  2001/02/26 23:25:40  dave
// added GetDicomName
//
// Revision 1.4  2001/02/26 18:24:19  jcohen
// Add GetLastFirstName() to return name as <Last, First MI>
//
// Revision 1.3  2001/02/26 01:26:54  ingmar
// added ste functions and operator <<
//
// Revision 1.2  2001/02/21 19:00:41  dave
// added string constuctor
//
// Revision 1.1  2001/01/18 21:30:08  dave
// added PersonName class
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/include/PersonName.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: PersonName.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
