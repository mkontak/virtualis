// $Id: PersonName.C,v 1.3 2007/11/03 00:02:37 jmeade Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Dave (dave@viatronix.com)


// includes
#include "stdafx.h"
#include "PersonName.h"


// namespaces
using namespace std;


/**
 * Defaul constructor
 */
PersonName::PersonName()
: m_sLast(""), m_sFirst(""), m_sMiddle(""), m_sPrefix(""), m_sSuffix("")
{
}


/**
 * Constructor
 * @param last name
 * @param first name
 * @param middle name
 * @param prefix
 * @param suffix
 */
PersonName::PersonName(const string & sLast, const string & sFirst,
                       const string & sMiddle, const string & sPrefix,
                       const string & sSuffix)
:m_sLast(sLast),
m_sFirst(sFirst),
m_sMiddle(sMiddle), 
m_sPrefix(sPrefix), 
m_sSuffix(sSuffix)
{
}


/**
 * Constructor
 */
PersonName::PersonName(const string & sDicomName)
{
  SetDicomName(sDicomName);
} // Constructor


/**
 * Set DICOM name
 * @param name
 */
void PersonName::SetDicomName(const std::string & sDicomName)
{
  //////////////////////
  // Remove the MOD:
  /////////////////////
  string name = toupper(sDicomName);
  std::string sReplace("");
  std::string sSearch("MOD:");

  std::string::size_type idx = name.find(sSearch);
  if ( idx != std::string::npos )
  {
    name.replace(idx, sSearch.size(), sReplace);     
  }

  ////////////////////////////////////////////////
  // make sure separators are all ^ (not ,)
  ///////////////////////////////////////////////
  uint4 k = 0;
  for (uint4 i(0); i < name.size(); ++i ) 
  {
    if ( name[i] == '*' )
    {
      if ( k == i ) 
      {
        ++k;
      }
      else
      {
        name[i] = ' ';
      }
    }
    else if ( name[i] == ',' )
    {
      name[i] = '^';
    }
    else if ( name[i] == ' ' )
    {
      if ( k == i )
      {
        ++k;
      }
      else
      {
        name[i] = '^';
      }
    }
  } // END .... for each character in the name

  ////////////////////////////////////////////////
  // Trim spaces and '*' from beginning of name
  ///////////////////////////////////////////////
  if ( k > 0 )
  {
    name = name.substr(k, name.size()-k);
  } // END ... If there are characters to trim

  ///////////////////////////////////////////////////////////////////////
  // could convert through dicom library, then must include it in V1k
  // write shitty parse function instead
  //////////////////////////////////////////////////////////////////////
  m_sLast   = ParseNext(name);
  m_sFirst  = ParseNext(name);
  m_sMiddle = ParseNext(name);
  m_sPrefix = ParseNext(name);
  m_sSuffix = ParseNext(name);
} // SetDicomName()


/**
 * Comparison operator
 * @param person name object
 * @return true if "equal", based on name only
 */
bool PersonName::operator==(const PersonName & other) const
{
  return GetFullName() == other.GetFullName();
} // operator==


/**
 * Inequality operator
 * @param person name object
 * @return true if "inequal", based on name only
 */
bool PersonName::operator!=(const PersonName & other) const
{
  return !(*this == other);
}


/**
 * Helper function: returns the full name formatted as "prefix first middle last suffix"
 * @return name
 */
string PersonName::GetFullName() const
{
  string sFullName(m_sPrefix);

  if (m_sFirst != "")
  {
    if (sFullName != "")
    {
      sFullName += " ";
    }
    sFullName += m_sFirst;
  }

  if (m_sMiddle != "")
  {
    if (sFullName != "") 
    {
      sFullName += " ";
    }
    sFullName += m_sMiddle;
  }

  if (m_sLast != "")
  {
    if (sFullName != "") 
    {
      sFullName += " ";
    }
    sFullName += m_sLast;
  }

  if (m_sSuffix != "")
  {
    if (sFullName != "") 
    {
      sFullName += " ";
    }
    sFullName += m_sSuffix;
  }

  return sFullName;
} // GetFullName()


/**
 * Helper function: returns the full name formatted as "last^first middle"
 * @return name
 */
string PersonName::GetLastFirstName() const
{
  string sFullName(m_sLast);

  if ( m_sFirst  != "" )
  {
    sFullName += "^";
    sFullName += m_sFirst;
  }
  if ( m_sMiddle != "" )
  {
    sFullName += "^" + m_sMiddle;
  }

  return sFullName;
}


/**
 * Helper function: returns full name as "last, first middle suffix"
 * @return name
 */
std::string PersonName::GetLastFirstMidSuffix() const
{
  string sFullName = m_sLast;
  if ( m_sFirst != "")  
  {
    sFullName += string(", ") + m_sFirst;
  }
  if ( m_sMiddle != "")  
  {
    sFullName += string(" ") + m_sMiddle;
  }
  if ( m_sSuffix != "")  
  {
    sFullName += string(" ") + m_sSuffix;
  }

  return sFullName;
} // GetLastFirstMidSuffix()


/**
 * Helper function: returns a Dicom formatted name string
 * @return name
 */
string PersonName::GetDicomName() const
{
  string sName(m_sLast);
  if (m_sFirst  != "") 
  { 
    sName += "^" + m_sFirst;
    if (m_sMiddle != "") 
    {
      sName += "^" + m_sMiddle;
      if (m_sPrefix != "") 
      {
        sName += "^" + m_sPrefix;
        if (m_sSuffix != "") 
        {
          sName += "^" + m_sSuffix;
        }
      }
    }
  }

  return sName;
} // GetDicomName()


/**
 * Parse next
 * @param name
 */
std::string PersonName::ParseNext(string & sName)
{
  string sRes;
  string::size_type pos;

  if ( sName == "" )
  {
    return sName;
  }
  else if ( (pos = sName.find_first_of("^")) == string::npos ) 
  {
    sRes  = sName;
    sName = "";
    return sRes;
  } 
  else 
  {
    sRes  = sName.substr(0, pos);
    sName = sName.substr(pos+1, sName.size());
    return sRes;
  }
} // ParseNext()


// $Log: PersonName.C,v $
// Revision 1.3  2007/11/03 00:02:37  jmeade
// Merge_from_CON_30_on_20071102
//
// Revision 1.2.2.1  2007/09/20 23:08:38  jmeade
// spelling in comment.
//
// Revision 1.2  2006/05/31 16:16:42  geconomos
// porting to visual studio 2005
//
// Revision 1.1.1.1  2004/03/02 03:58:41  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.5  2003/05/13 19:09:08  michael
// code review
//
// Revision 3.4  2003/04/22 13:58:05  mkontak
// Issue 3093
//
// Revision 3.3  2002/06/19 17:31:54  frank
// Fixed patient name formatting.
//
// Revision 3.2  2002/05/20 03:31:15  mkontak
// Added SetDicomName()
//
// Revision 3.1  2002/02/20 01:43:38  michael
// includes/spaces
//
// Revision 3.0  2001/10/14 23:02:15  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 19 2001 14:29:58   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:46:20   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:33:36   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:08:30  soeren
// renamed utils directory
//
// Revision 1.12  2001/08/10 18:30:26  mkontak
// Changed the person name to add a ^ between the first name and middle
// name.
//
// Revision 1.11  2001/05/20 20:14:32  dave
// more standards compliant
//
// Revision 1.10  2001/05/15 14:13:17  dave
// added comparison operators
//
// Revision 1.9  2001/04/08 21:38:20  dave
// updated doc headers
//
// Revision 1.8  2001/03/21 18:18:36  dmitry
// Updated datatypes as defined in Typedef.h
//
// Revision 1.7  2001/02/27 16:36:51  jmeade
// corrected comment on GetLastFirstName() (it does NOT return "last, first middle");
// added GetLastFirstMidSuffix() (which returns "last, first mid suffix")
//
// Revision 1.6  2001/02/26 23:25:40  dave
// added GetDicomName
//
// Revision 1.5  2001/02/26 18:24:18  jcohen
// Add GetLastFirstName() to return name as <Last, First MI>
//
// Revision 1.4  2001/02/26 01:26:54  ingmar
// added ste functions and operator <<
//
// Revision 1.3  2001/02/21 21:10:24  jcohen
// Allow comma or space as separator for raw dicom name in addition to ^
//
// Revision 1.2  2001/02/21 19:00:41  dave
// added string constuctor
//
// Revision 1.1  2001/01/18 21:30:08  dave
// added PersonName class
//
// $Header: /CVS/cvsRepository/V3D/src/vxBlockVolume/src/PersonName.C,v 1.3 2007/11/03 00:02:37 jmeade Exp $
// $Id: PersonName.C,v 1.3 2007/11/03 00:02:37 jmeade Exp $