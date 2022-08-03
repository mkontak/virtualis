// $Id: FileNameFilter.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: dave (dave@viatronix.com)

#ifndef FileNameFilter_h
#define FileNameFilter_h

// class definition: FileNameFilter
class VX_BASE_DLL FileNameFilter
{
// functions
public:

  /// constructor
  FileNameFilter(const std::string & s = std::string(), const int4 i = 0);

  /// destructor
  virtual ~FileNameFilter();

  /// returns true if the filename matches the filter
  virtual bool Accept(const std::string & sDir, const std::string & sName) const = 0;

  /// returns the filename wildcard for the filter (e.g. '*' for all, '*.ext' for a particular extension)
  virtual std::string GetNameWildcard() const = 0;

// data
protected:

  /// stored length of filter
  const int4 m_iFilterLen;

  /// filter
  const std::string m_sFilter;
}; // FileNameFilter


// class definition: Example instance of FileNameFilter that accepts names with matching prefix.
class VX_BASE_DLL PrefixFilter : public FileNameFilter
{
// functions
public:

  /// constructor
  PrefixFilter(const std::string & sInFilePrefix);

  /// returns true if the filename matches the filter
  virtual bool Accept(const std::string & sDir, const std::string & sName) const;

  /// returns the filename wildcard for the filter (e.g. '*' for all, '*.ext' for a particular extension)
  virtual std::string GetNameWildcard() const;
}; // PrefixFilter


// class definition: Example instance of FileNameFilter that accepts names with matching suffix.
class VX_BASE_DLL SuffixFilter : public FileNameFilter
{
// functions
public:

  /// constructor
  SuffixFilter(const std::string & sInFileSuffix);

  /// returns true if the filename matches the filter
  virtual bool Accept(const std::string & sDir, const std::string & sName) const;

  /// returns the filename wildcard for the filter (e.g. '*' for all, '*.ext' for a particular extension)
  virtual std::string GetNameWildcard() const;
}; // SuffixFilter


/**
 * File filter that accepts all filenames that contain the given string.
 */
class VX_BASE_DLL ContainsFilter : public FileNameFilter
{
// functions
public:

  /// constructor
  ContainsFilter(const std::string & sInText);

  /// returns true if the filename matches the filter
  virtual bool Accept(const std::string & sDir, const std::string & sName) const;

  /// returns the filename wildcard for the filter (e.g. '*' for all, '*.ext' for a particular extension)
  virtual std::string GetNameWildcard() const;
}; // ContainsFilter


/**
 * Default Filter that accepts all files.
 */
class VX_BASE_DLL AcceptAllFilter : public FileNameFilter
{
// functions
public:

  /// returns true if the filename matches the filter
  virtual bool Accept(const std::string & sDir, const std::string & sName) const { return true; }

  /// returns the filename wildcard for the filter (e.g. '*' for all, '*.ext' for a particular extension)
  virtual std::string GetNameWildcard() const { return std::string("*"); }
}; // AcceptAllFilter


#endif // FileNameFilter_h


// $Log: FileNameFilter.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.7  2003/05/16 13:08:19  frank
// code formatting
//
// Revision 3.6  2003/05/13 19:09:08  michael
// code review
//
// Revision 3.5  2003/05/12 16:31:48  michael
// code review
//
// Revision 3.4  2002/07/19 15:17:47  mkontak
// MoreVXDLL
//
// Revision 3.3  2002/07/18 15:11:55  mkontak
// Add VXDLL to class definitions
//
// Revision 3.2  2002/01/17 22:24:30  jmeade
// Removed differing memory allocations in derived classes;
// Use system commands for file name (filter) matching.
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
// Revision 1.6  2001/05/20 20:14:32  dave
// more standards compliant
//
// Revision 1.5  2001/04/19 20:57:37  dave
// updated variable names
//
// Revision 1.4  2001/04/12 15:56:38  jmeade
// use case-insensitive comparisons
//
// Revision 1.3  2001/04/11 18:40:32  dmitry
// Sec_1_utils inheritance added
//
// Revision 1.2  2001/04/08 21:38:20  dave
// updated doc headers
//
// Revision 1.1  2001/02/13 02:57:23  dave
// added FileNameFilter as new file, listRecusive in File.h
//
// $Id: FileNameFilter.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
