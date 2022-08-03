// $Id: FileNameFilter.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: dave dave@viatronix

#include "stdafx.h"
#include "FileNameFilter.h"


/**
 * Constructor
 * 
 */
FileNameFilter::FileNameFilter(const std::string & s, const int4 i) : m_sFilter(s), m_iFilterLen(i)
{
}


/**
 * Destructor
 */
FileNameFilter::~FileNameFilter()
{
}


/**
 * Constructor
 */
PrefixFilter::PrefixFilter(const std::string & sInFilePrefix)
 : FileNameFilter(tolower(sInFilePrefix), static_cast< int4 >( sInFilePrefix.size()))
{ 
}


/**
 * Accepts all matching prefixes.
 */
bool PrefixFilter::Accept(const std::string & sDir, const std::string & sName) const
{
  if ( static_cast< uint4 >( m_iFilterLen ) > sName.size() )
    return false;
  else if (tolower(sName.substr(0, m_iFilterLen)) == m_sFilter)
    return true;
  else
    return false;
} // Accept


/**
 * Returns the filename wildcard for the filter (e.g. '*' for all, '*.ext' for a particular extension)
 *
 */
std::string PrefixFilter::GetNameWildcard() const
{
  return m_sFilter + '*';
}


/**
 * Constructor
 */
SuffixFilter::SuffixFilter(const std::string & sInFileSuffix) 
: FileNameFilter(tolower(sInFileSuffix), static_cast< int4 >( sInFileSuffix.size()))
{ 
}


/**
 * Accepts all w/matching suffix
 */
bool SuffixFilter::Accept(const std::string & sDir, const std::string & sName) const
{
  if ( static_cast< uint4 >( m_iFilterLen ) >= sName.size() )
    return false;

  if (tolower(sName).substr( sName.size() - m_iFilterLen) == m_sFilter)
    return true;
  else
    return false;
} // Accept


/**
 * Returns the filename wildcard for the filter (e.g. '*' for all, '*.ext' for a particular extension)
 *
 */
std::string SuffixFilter::GetNameWildcard() const
{
  return '*' + m_sFilter;
}


/**
 *
 */
ContainsFilter::ContainsFilter(const std::string & sInText) 
 : FileNameFilter(tolower(sInText), static_cast< int4 >( sInText.size()))
{
}
  

/**
 * Accept all that contain the specified text.
 */
bool ContainsFilter::Accept(const std::string & sDir, const std::string & sName) const
{
  if (tolower(sName).find(m_sFilter) != std::string::npos)
    return true;
  else
    return false;
}
  

/**
 * Returns the filename wildcard for the filter (e.g. '*' for all, '*.ext' for a particular extension)
 *
 */
std::string ContainsFilter::GetNameWildcard() const
{
  return '*' + m_sFilter + '*';
}

// $Log: FileNameFilter.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.3  2003/05/13 19:09:08  michael
// code review
//
// Revision 3.2  2003/05/12 16:31:48  michael
// code review
//
// Revision 3.1  2002/01/17 22:24:30  jmeade
// Removed differing memory allocations in derived classes;
// Use system commands for file name (filter) matching.
//
// Revision 3.0  2001/10/14 23:02:15  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 19 2001 14:29:56   liwei
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
// Revision 1.2  2001/05/20 20:14:32  dave
// more standards compliant
//
// Revision 1.1  2001/04/19 22:44:31  dave
// updated variable names
//
// $Id: FileNameFilter.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
