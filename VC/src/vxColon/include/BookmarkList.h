// $Id: BookmarkList.h,v 1.2.10.1 2008/11/13 02:16:33 jmeade Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeffrey Meade (mailto:jmeade@viatronix.net)


// pragmas
#pragma once


// includes
#include "Typedef.h"
#include "Triple.h"
#include "Point.h"
#include "Normal.h"
#include "FileExtensions.h"
#include "StudyLoad.h"
#include "Bookmark.h"
#include <string>
#include <list>
#include <ios>


// defines
#define BOOKMARK_FILE_ID              "V1KBookmarksFile"


namespace ReaderLib
{

  class Dataset;
  class BookmarkList : public std::list<Bookmark>
  {
  public:
    enum { eBOOKMARK_FILE_CURRENT_VERSION = 0x002 };

  public:
    /// Constructor
    BookmarkList(const vx::FileExtension& fileExt);
    /// File read constructor.
    BookmarkList(const std::string & filePathTitle, const vx::FileExtension& fileExt);
    /// Destructor.
    virtual ~BookmarkList();

    /// Clears the bookmark list.
    void Reset();

    /// Returns true if index is valid.
    bool IsValidIndex(const int4 & iIndex) const;

    /// Adds bookmark to the list.
    void AddBookmark(Bookmark bookmark);
    /// Jumps the current world coordinates to the specified bookmark and stops flying.
    bool StopAtBookmark(const int4 & iIndex, Dataset & rDataset);
    /// Retrieves the specified bookmark.
    Bookmark & operator[](const int4 & iIndex);
    /// Retrieves the specified bookmark.
    const Bookmark & operator[](const int4 & iIndex) const;

    /// Deletes the specified bookmark.
    bool DeleteBookmark(const int4 & iIndex);

    /// Outputs list to the specified file.
    bool WriteFile(const std::string & sFilePathTitle);
    /// Initializes list from the specified file.
    bool ReadFile(const std::string & sFilePathTitle);

    /// Input stream operator
    friend std::istream& operator>>(std::istream& is, BookmarkList& bmlist)
    {
      // Read header
      std::string sFileID;
      int4 iVersion;
      int1 inStringTerm(0);
      InputTermFmtString(is, sFileID);
      is >> iVersion >> inStringTerm;

      // set the version of the class that was saved with the file
      Bookmark::g_iBookmarkClassVersion = iVersion;

      bmlist.clear();

      // Read bookmarks
      int4 iNumBookmarks;
      is >> iNumBookmarks >> inStringTerm;

      for (Bookmark bmIn; iNumBookmarks > 0; is >> bmIn, bmlist.push_back(bmIn), iNumBookmarks--);

      // reset the current file version
      Bookmark::g_iBookmarkClassVersion = BookmarkList::eBOOKMARK_FILE_CURRENT_VERSION;
      return is;
    }
    /// Output stream operator
    friend std::ostream& operator<<(std::ostream& os, const BookmarkList& bmlist)
    {
      // Write header
      OutputTermFmtString(os, std::string(BOOKMARK_FILE_ID));
      os << BookmarkList::eBOOKMARK_FILE_CURRENT_VERSION << Bookmark::STRING_TERMINATOR;

      // Write bookmarks
      os << bmlist.size() << Bookmark::STRING_TERMINATOR;
      for (BookmarkList::const_iterator curr = bmlist.begin(); curr != bmlist.end(); os << *curr, curr++);
      return os;
    }

  private:
    /// Retrieves the indicated bookmark.
    iterator GetBookmark(int4 iIndex);
    /// Retrieves the indicated bookmark.
    const_iterator GetBookmark(int4 index) const;
    /// Serializes bookmark file.
    bool SerializeFile(const std::string & sFileTitle, const std::ios::open_mode & mode);

  private:
    /// File extension for saving instances of the file
    const vx::FileExtension& m_fileExt;
  };

} // namespace ReaderLib


// $Log: BookmarkList.h,v $
// Revision 1.2.10.1  2008/11/13 02:16:33  jmeade
// fix bug where go-to-bookmark functionality failed when prone loaded as 1st dataset.
//
// Revision 1.2  2005/09/28 12:03:26  vxconfig
// split out bookmark class from bookmark list files
//
// Revision 1.1  2005/09/13 13:00:10  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.14.2.3.2.1  2005/03/25 13:58:51  frank
// Issue #3990: Fixed bookmark and annotation jumping with linked datasets
//
// Revision 3.14.2.3  2005/02/15 17:04:34  jmeade
// merge from vc1.4 branch.
//
// Revision 3.14.2.2.2.1  2004/12/06 22:42:47  jmeade
// Stop flight upon jumping to bookmark.
//
// Revision 3.14.2.2  2003/04/14 16:41:18  jmeade
// Unneeded method.
//
// Revision 3.14.2.1  2003/02/21 17:51:32  jmeade
// Bookmark string cast operator (returns name).
//
// Revision 3.14  2003/01/22 22:25:46  ingmar
// added reading of triples
//
// Revision 3.13  2002/12/24 03:38:52  jmeade
// Put file version enum into class header.
//
// Revision 3.12  2002/09/27 19:03:03  jmeade
// Coding standards, function name change, comments.
//
// Revision 3.11  2002/07/23 16:25:33  jmeade
// Use the compiler-created copy operator (no need to define it).
//
// Revision 3.10  2002/07/15 15:51:53  jmeade
// A user-defined data field (not serialized, for use during a single execution).
//
// Revision 3.9  2002/07/03 03:23:41  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_07-02-2002)
//
// Revision 3.8  2002/06/26 17:49:55  jmeade
// Moving more hard-coded strings to the string table; Code standards.
//
// Revision 3.7  2002/06/10 22:39:24  jmeade
// Merged with VC_1-2_CS-1 branch, tag:  Joined_On_06-10-2002
//
// Revision 3.6  2002/06/04 18:37:10  jmeade
// Merged with VC_1-2_CS-1 Branch, tag: Joined_On_06-03-2002
//
// Revision 3.5  2002/05/29 15:42:02  jmeade
// Comments.
//
// Revision 3.4  2002/04/12 02:10:17  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.3  2002/03/15 03:26:16  jmeade
// Returning to local versions of serialize functions for Triple derivatives (too many errors
//  using vxBase serializing); Code standards.
//
// Revision 3.2  2002/03/13 21:56:29  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.1.2.3  2002/07/02 23:03:25  jmeade
// Exposing serialization functions for class use versatility.
//
// Revision 3.1.2.2  2002/06/04 19:22:29  jmeade
// Issue 2270:  Allow class user to specify file extension.
//
// Revision 3.1.2.1  2002/05/23 15:56:49  jmeade
// Issue 2270:  Visible flag (used primarily for annotations); const's where appropriate;
// Derive from (instead of including) std::list<>, for easier access to enumeration functions.
//
// Revision 3.1  2001/11/11 19:55:31  kevin
// Fixes for rdrGlobal.m_pStudy when renderin 2 endoscopic views in registration mode
//
// Revision 3.0  2001/10/14 23:01:53  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:40:06   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:26:56   ingmar
// Initial revision.
// Revision 1.9  2001/05/15 17:42:02  kevin
// No need to use (void *) in bookmark goto functions (would have liked to
// use Study &, but can't quite get it compiling, so it's Study * for now
//
// Revision 1.8  2001/04/20 20:10:38  binli
// fixed a bug: bookmark picking in S&P view or Verify view.
//
// Revision 1.7  2000/11/09 19:08:01  jeff
// Using std::list<> instead of an array to store bookmarks (planned necessary improvement)
//
// Revision 1.6  2000/11/08 21:14:33  jeff
// Reduced size of bookmark list's pre-allocated array
//
// Revision 1.5  2000/11/03 01:52:40  jeff
// Added method Bookmark::GoTo()
//
// Revision 1.4  2000/10/25 17:46:36  jeff
// bookmark priority; helpful if I put some code in Reset() function;
// boomark list ValidIndex() accounts also for array size;
// default bookmark name simply "Bookmark";
// get bookmark in list functions return reference (for updating);
//
// Revision 1.3  2000/10/03 16:35:09  jeff
// Reverted back to older version of Bookmark and BookmarkList,
// because the past update to conform to coding standards introduced
// MANY errors!
//
// Revision 1.1.1.1  2000/09/11 00:33:25  ingmar
// unit tests complete
//
// 
// 7     8/28/00 2:05p Antonio
// updated header and footer
// 
// **********************
// Label: Version 0.1
// User: Jeff            Date:  8/15/00  Time:  5:43p
// Labeled 'Version 0.1'
// Label comment:
//   First half-decent working version
// 
// *****************  Version 6  *****************
// User: Jeff            Date:  8/02/00  Time:  7:52p
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Drawing bookmarks (can't get the colors right though, and they're
// kinda big)
// 
// Ensuring the thread is resumed before continuing when calling ResumeThread()
// 
// *****************  Version 5  *****************
// User: Jeff            Date:  8/01/00  Time:  7:05p
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Version 1 Bookmark file
// 
// *****************  Version 4  *****************
// User: Jeff            Date:  7/28/00  Time:  5:53p
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Added functions for serializing bookmark list and for adding
// bookmarks thru GUI
// 
// *****************  Version 3  *****************
// User: Jeff            Date:  7/27/00  Time:  6:11p
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Nightly checkin
// 
// *****************  Version 2  *****************
// User: Jeff            Date:  7/24/00  Time:  7:52p
// Checked in $/Viatron1000/src/Viatron
// Comment:
//   Added bookmark class and associated hooks,
// (initially) only for jumping to fixed precomputed positions
// 
// *****************  Version 1  *****************
// User: Jeff            Date:  7/24/00  Time:  6:22p
// Created BookmarkList.h
// Comment:
//   Initial version
// 
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/BookmarkList.h,v 1.2.10.1 2008/11/13 02:16:33 jmeade Exp $
// $Id: BookmarkList.h,v 1.2.10.1 2008/11/13 02:16:33 jmeade Exp $
