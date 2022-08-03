// $Id: BookmarkList.C,v 1.6.2.1 2008/11/13 02:16:33 jmeade Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Jeffrey Meade  jmeade@viatronix.com


// includes
#include "stdafx.h"
#include "BookmarkList.h"
#include "ReaderGlobal.h"


// defines
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define FILE_REVISION "$Revision: 1.6.2.1 $"


// namespaces
using namespace ReaderLib;


/**
 * Default constructor
 */
BookmarkList::BookmarkList(const vx::FileExtension& fileExt) : m_fileExt(fileExt)
{
  // Initialize variables
  Reset();
}


/**
 * File read constructor.
 *
 * @param const std::string & sFileTitle   File to open.
 */
BookmarkList::BookmarkList(const std::string & sFileTitle, const vx::FileExtension& fileExt) : m_fileExt(fileExt)
{
  ReadFile(sFileTitle);
}


/**
 * Destructor 
 */
BookmarkList::~BookmarkList()
{
  Reset();
}


/**
 * Clears the bookmark list.
 */
void BookmarkList::Reset()
{
  clear();
}


/**
 * Returns true if index is valid.
 *
 * @param const int4 & iIndex   Index to test.
 * @return bool                 True if index is valid, false otherwise.
 */
bool BookmarkList::IsValidIndex(const int4 & iIndex) const
{
  return ((iIndex >= 0) && (iIndex < size()));
}


/**
 * Adds bookmark to the list.
 *
 * @param Bookmark bookmark   Bookmark to add to list.
 */
void BookmarkList::AddBookmark(Bookmark bookmark)
{
  if (bookmark.m_sName.empty())
  {
    bookmark.m_sName = "---";
  }

  push_back(bookmark);
}


/**
 * Deletes the specified bookmark.
 *
 * @param const int4 & iIndex   Bookmark to delete.
 * @return bool                 True if bookmark index is valid.
 */
bool BookmarkList::DeleteBookmark(const int4 & iIndex)
{
  iterator delMarkIt = GetBookmark(iIndex);
  if (delMarkIt != end())
  {
    erase(delMarkIt);
    return true;
  }

  return false;
}


/**
 * Moves the current world coordinates to the specified bookmark, stopping any current flight.
 *
 * @param const int4 & iIndex   Index of bookmark in list.
 * @param Dataset & rDataset    Dataset whose world position will change.
 * @return bool                 True if index is valid, false otherwise.
 */
bool BookmarkList::StopAtBookmark(const int4 & iIndex, Dataset & rDataset)
{

  try
  {
    // check for a valid index
    bool bValid = IsValidIndex( iIndex );
    if ( bValid )
    {

      // get the other dataset
      Dataset & rSecondary = (&rDataset == &rdrGlobal.m_supine) ? rdrGlobal.m_prone : rdrGlobal.m_supine;

      // go to the bookmark in one or both datasets
      ( * this )[ iIndex ].GoTo( rDataset );
      if ( rdrGlobal.m_bSPLocked )
      {
        rdrGlobal.UpdateWorldPosition( rDataset, rSecondary );
      }

      // stop both datasets
      rDataset.m_submarine.FullStop();
      rSecondary.m_submarine.FullStop();
    }
    return bValid;
  } // try
  catch ( ... )
  {
    LogWrn( "unhandled exception", "BookmarkList", "StopAtBookmark" );
    return false;
  }

} // StopAtBookmark()


/**
 * Returns the indicated bookmark.
 *
 * @param int4 iIndex         List index of bookmark.
 * @return BookmarkIterator   Found bookmark, or NULL if index not valid.
 */
BookmarkList::iterator BookmarkList::GetBookmark(int4 iIndex)
{
  if (!IsValidIndex(iIndex))
  {
    return end();
  }

  iterator bookmarkIter = begin();
  for (; iIndex > 0; bookmarkIter++, iIndex--)
  {
    if (bookmarkIter == end())
    {
      return end();
    }
  }

  return bookmarkIter;
}


/**
 * Returns the indicated bookmark.
 *
 * @param int4 iIndex         List index of bookmark.
 * @return BookmarkIterator   Found bookmark, or NULL if index not valid.
 */
BookmarkList::const_iterator BookmarkList::GetBookmark(int4 iIndex) const
{
  if (!IsValidIndex(iIndex))
  {
    return const_iterator( end() );
  }

  const_iterator bookmarkIter = begin();
  for (; iIndex > 0; bookmarkIter++, iIndex--)
  {
    if (bookmarkIter == end())
    {
      return const_iterator( end() );
    }
  }

  return bookmarkIter;
}


/**
 * Retrieves the specified bookmark.
 *
 * @param const int4 & iIndex   List index of bookmark.
 * @return Bookmark &           Found bookmark, or NULL if index not valid.
 */
Bookmark & BookmarkList::operator [](const int4 & iIndex)
{
  iterator rtnBmIter = GetBookmark(iIndex);
  if (rtnBmIter != iterator( end() ))
  {
    return *rtnBmIter;
  }
  else
  {
    static Bookmark nullBookmark;
    return (nullBookmark = Bookmark());
  }
}



/**
 * Bookmark list access operator.
 *
 * @param const int4 & iIndex   Index of bookmark to access.
 * @return const Bookmark &     Bookmark indicated by iIndex, or a null bookmark for invalid index.
 */
const Bookmark & BookmarkList::operator [](const int4 & iIndex) const
{
  const_iterator rtnBm = GetBookmark(iIndex);
  if (rtnBm != const_iterator( end() ))
  {
    return *rtnBm;
  }
  else
  {
    static Bookmark nullBookmark;
    return (nullBookmark = Bookmark());
  }
}


/**
 * Initializes list from the specified file.
 *
 * @param const std::string & sFileTitle   Title of bookmark file to read.
 * @return bool                            Success of reading file.
 */
bool BookmarkList::ReadFile(const std::string & sFileTitle)
{
  //TODO 0x20 is the code for std::ios::nocreate but I get an error when compiling that 
  //     it does not exists. We need to find the reason for this error.
  return SerializeFile(sFileTitle, std::ios::open_mode(std::ios::in | 0x20 ));
}


/**
 * Outputs list to the specified file.
 *
 * @param const std::string & sFileTitle   Title of file to write.
 * @return bool                            Success of writing file.
 */
bool BookmarkList::WriteFile(const std::string & sFileTitle)
{
  return SerializeFile(sFileTitle, std::ios::open_mode(std::ios::out | std::ios::trunc));
}


/**
 * Serializes bookmark file.
 *
 * @param sFileTitle   Title of file to serialize.
 * @param mode         Indicates whether file will be read or written.
 * @return             Success of serialization.
 */
bool BookmarkList::SerializeFile(const std::string & sFileTitle, const std::ios::open_mode & mode)
{
  // Check the file pathname
  if (sFileTitle.empty())
  {
    return false;
  }

  if (mode & std::ios::in)
  {
    // Attempt to open file
    std::ifstream file((sFileTitle + m_fileExt).c_str(), std::ios::binary | mode);
    if (!file.is_open())
    {
      return false;
    }
    file >> *this;
    file.close();
  }
  else if (mode & std::ios::out)
  {
    // Attempt to open file
    std::ofstream file((sFileTitle + m_fileExt).c_str(), std::ios::binary | mode);
    if (!file.is_open())
    {
      return false;
    }
    file << *this;
    file.close();
  }

  return true;
}


/*
inline std::istream & operator >>(std::istream & is, std::string & str) // (operator >>, string)
{
  uint4 iLen;
  int1 iStringTerm(0);
  is >> iStringTerm;

  if (iStringTerm != Bookmark::STRING_TERMINATOR)
  {
    return is;
  }

  is >> iLen >> iStringTerm;
  str.resize(iLen);

  for (uint4 i=0; i<iLen; is.get(str[i++]));
  is >> iStringTerm;
  return is;
}
inline std::ostream & operator <<(std::ostream & os, const std::string & str) // (operator <<, string)
{
  os << Bookmark::STRING_TERMINATOR << str.length() << Bookmark::STRING_TERMINATOR << str.c_str() << Bookmark::STRING_TERMINATOR;
  return os;
}
*/


// $Log: BookmarkList.C,v $
// Revision 1.6.2.1  2008/11/13 02:16:33  jmeade
// fix bug where go-to-bookmark functionality failed when prone loaded as 1st dataset.
//
// Revision 1.6  2006/06/01 20:25:50  frank
// updated to visual studio 2005
//
// Revision 1.5  2006/04/12 23:16:51  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.4  2006/02/27 18:42:02  jmeade
// missing return statement in a method.
//
// Revision 1.3  2005/11/16 04:52:06  jmeade
// Merge from Merge_VC_2-0_051115.
//
// Revision 1.2.2.2  2006/03/23 20:39:51  jmeade
// return call in method.
//
// Revision 1.2.2.1  2005/11/15 13:21:20  frank
// additional error handling
//
// Revision 1.2  2005/09/28 12:03:25  vxconfig
// split out bookmark class from bookmark list files
//
// Revision 1.1  2005/09/13 12:59:56  geconomos
// Moved from Viatron.vcproj
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.13.2.2.2.4  2005/04/11 15:11:01  jmeade
// Issue 2815: Disable annotations for oblique views.
//
// Revision 3.13.2.2.2.3  2005/04/01 03:46:43  frank
// Issue #4095: Actually stopped flying in 2D and in 3D at a bookmark
//
// Revision 3.13.2.2.2.2  2005/03/25 13:58:51  frank
// Issue #3990: Fixed bookmark and annotation jumping with linked datasets
//
// Revision 3.13.2.2.2.1  2005/03/24 16:18:21  frank
// Issue #3990: Jumped to bookmark on both supine and prone at the same time.
//
// Revision 3.13.2.2  2005/02/15 17:04:34  jmeade
// merge from vc1.4 branch.
//
// Revision 3.13.2.1.2.1  2004/12/06 22:42:47  jmeade
// Stop flight upon jumping to bookmark.
//
// Revision 3.13.2.1  2003/04/14 16:41:11  jmeade
// Unneeded method.
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
// Revision 3.7  2002/06/10 22:39:23  jmeade
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
// Revision 3.3  2002/03/15 03:26:15  jmeade
// Returning to local versions of serialize functions for Triple derivatives (too many errors
//  using vxBase serializing); Code standards.
//
// Revision 3.2  2002/03/13 21:56:28  mkontak
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
// Revision 1.20  2001/08/23 15:16:53  binli
// appendix to bug 509 fix: closed BookMark file after the study was closed.
// (Without this close fucntion call, I found the .mrk file was closed automatically,
//  Don;t know why in some cases it's still opened. Now it is forced to close, wish
// this bug will go.)
//
// Revision 1.19  2001/05/15 17:42:02  kevin
// No need to use (void *) in bookmark goto functions (would have liked to
// use Study &, but can't quite get it compiling, so it's Study * for now
//
// Revision 1.18  2001/05/02 20:12:09  ingmar
// BookmarkList.C
//
// Revision 1.17  2001/05/02 18:33:53  binli
// bug fixed: correct render planes when picked up a bookmark in S&PView/VerifyView.
//
// Revision 1.16  2001/04/30 02:20:02  kevin
// First part of ReaderGlobal Re-organize for improved object
// encapsulation before the Code Walkthrough
//
// Revision 1.15.2.1  2001/05/01 23:38:35  jmeade
// m_volume changed to m_volumeRender (April 26 - jmeade)
//
// Revision 1.15  2001/04/20 20:10:38  binli
// fixed a bug: bookmark picking in S&P view or Verify view.
//
// Revision 1.14  2001/04/13 00:35:42  jmeade
// file extensions
//
// Revision 1.13  2001/04/11 13:27:31  frank
// Moved volume data to Fields class.
//
// Revision 1.12  2001/03/14 16:44:34  jmeade
// Bug: No saving empty list of bookmarks
//
// Revision 1.11  2001/03/09 01:08:08  jmeade
// don't write empty bookmark list
//
// Revision 1.10  2000/11/20 14:38:03  frank
// Cleaned up motion blur
//
// Revision 1.9  2000/11/14 19:24:33  frank
// Flushed accumulation buffer during jumps
//
// Revision 1.8  2000/11/14 18:56:20  frank
// Changed study reference to pointer
//
// Revision 1.7  2000/11/09 19:08:01  jeff
// Using std::list<> instead of an array to store bookmarks (planned necessary improvement)
//
// Revision 1.6  2000/11/08 17:38:55  frank
// Added m_study to ReaderGlobal
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
// Revision 1.5  2000/09/21 20:51:55  frank
// Reflected new Submarine function names
//
// Revision 1.4  2000/09/21 19:27:50  kevin
// I dind't make any changes, why does this stupid program think that I
// need to check this back in
//
// Revision 1.3  2000/09/20 19:07:09  binli
// followed the change in class Submarine (member functions: Update(), Panto(), Replay())
//
// Revision 1.2  2000/09/19 17:28:43  binli
// Followed Frank's names
//
// Revision 1.1.1.1  2000/09/11 00:33:25  ingmar
// unit tests complete
//
// 
// 10    8/28/00 1:00p Antonio
// updated header and footer
//
//**********************
//Label: Version 0.1
//User: Jeff            Date:  8/15/00  Time:  5:43p
//Labeled 'Version 0.1'
//Label comment:
//  First half-decent working version
//
//*****************  Version 9  *****************
//User: Frank           Date:  8/08/00  Time:  9:40a
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Reflected function name change
//
//*****************  Version 8  *****************
//User: Jeff            Date:  8/02/00  Time:  7:52p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Drawing bookmarks (can't get the colors right though, and they're
//kinda big)
//
//Ensuring the thread is resumed before continuing when calling ResumeThread()
//
//*****************  Version 7  *****************
//User: Jeff            Date:  8/02/00  Time:  1:48p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Bug fix: Increased the length of input strings in
//operator>>(istream,std::string)
//
//*****************  Version 6  *****************
//User: Jeff            Date:  8/01/00  Time:  7:05p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Version 1 Bookmark file
//
//*****************  Version 5  *****************
//User: Jeff            Date:  7/31/00  Time:  5:36p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Added an enum class to GuiNavigation (GuiEnum) to:
//(1) Control access to mode vals (can only use = operator from inside class)
//(2) Reduce duplication (one each of Get,Set,operator ==,etc. methods)
//
//*****************  Version 4  *****************
//User: Jeff            Date:  7/28/00  Time:  5:53p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Added functions for serializing bookmark list and for adding
//bookmarks thru GUI
//
//*****************  Version 3  *****************
//User: Jeff            Date:  7/27/00  Time:  6:11p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Nightly checkin
//
//*****************  Version 2  *****************
//User: Jeff            Date:  7/24/00  Time:  7:52p
//Checked in $/Viatron1000/src/Viatron
//Comment:
//  Added bookmark class and associated hooks,
//(initially) only for jumping to fixed precomputed positions
//
//*****************  Version 1  *****************
//User: Jeff            Date:  7/24/00  Time:  6:22p
//Created BookmarkList.C
//Comment:
//  Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/src/BookmarkList.C,v 1.6.2.1 2008/11/13 02:16:33 jmeade Exp $
// $Id: BookmarkList.C,v 1.6.2.1 2008/11/13 02:16:33 jmeade Exp $
