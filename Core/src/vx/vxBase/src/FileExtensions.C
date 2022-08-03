// $Id: FileExtensions.C,v 1.2 2005/08/05 13:02:46 geconomos Exp $
// 
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Jeff Meade (jmeade@viatronix.com)

#include "stdafx.h"
#define FILE_EXTENSIONS_C
#include "FileExtensions.h"

using namespace vx;

/**
 * Constructor 
 *
 * @param s   File extension string.
 * @param e   File type classification.
 */
FileExtension::FileExtension(const std::string & s, const FileClassificationEnum e)
  : m_sExt(InterpretAsExtension(tolower(s))), m_eFileClass(e), m_iExtLen(m_sExt.length()-1)
{
}


/**
 * Returns true if the given filename has the extension (left-hand side of ==).
 *
 * @param sRHSFilename   Filename to test.
 * @return               True if filename ends with the given (LHS) extension.
 */
bool FileExtension::operator==(const std::string & sRHSFilename) const
{
  // compare last n-1 characters of both
  return (m_sExt.compare(1, min(m_iExtLen, sRHSFilename.length()), tolower(sRHSFilename), sRHSFilename.length() - m_iExtLen, m_iExtLen) == 0);
  //return (m_sExt.compare(1, m_iExtLen, tolower(sRHSFilename), sRHSFilename.length() - m_iExtLen, m_iExtLen) == 0);
}

/**
 * Determine if the filename corresponds to a supine dataset.
 * It expects a file prefix that looks like "NAME1P" or "NAME1S"
 *
 * @param sFileTitle    file title
 * @return              true if it is supine file
 */
bool IsSupineFilePrefix(const std::string & sFileTitle)
{
  bool bSupine(false);

  if (sFileTitle.length() > 1)
  {
    int2 iSorP(toupper(sFileTitle[sFileTitle.length()-1]));
    if (iSorP == 'S')
    {
      bSupine = true;
    }
    else if (iSorP == 'P')
    {
      bSupine = false;
    }
    else
    {
      //wrn << "Unable to determine if the model is supine or prone." << endl;
    } // primary dataset is supine or prone
  } // enough characters in string
  
  return bSupine;
} // IsSupineFileTitle()


/**
 * Convert prone file title to supine.
 *
 * @param sProneFileTitle   prone file title
 * @return                  supine file title
 */
std::string GetSupineFilePrefixFromProne(const std::string & sProneFileTitle)
{
  std::string sSupine(sProneFileTitle);
  const uint4 uLength( static_cast< uint4 >( sProneFileTitle.length() ));

  if (uLength > 1)
  {
    sSupine[uLength-1] = 'S';
  } // if enough characters to process

  return sSupine;
} // GetSupineFilePrefixFromProne()


/**
 * Convert supine file title to prone.
 *
 * @param sSupineFileTitle   supine file title
 * @return                   prone file title
 */
std::string GetProneFilePrefixFromSupine(const std::string & sSupineFileTitle)
{
  std::string sProne(sSupineFileTitle);
  const uint4 uLength( static_cast< uint4>( sSupineFileTitle.length()) );

  if (uLength > 1)
  {
    sProne[uLength-1] = 'P';
  } // enough characters to process

  return sProne;
} // GetProneFilePrefixFromSupine()


/**
 * returns the last period and all following characters of <s>
 *
 * @param s   string to be interpreted as an extension
 * @return    the last '.' character and all trailing characters in <s>
 */
const std::string FileExtension::InterpretAsExtension(const std::string & s) const
{
  const std::string::size_type iPos(s.find_last_of('.'));
  return (iPos != std::string::npos) ? s.substr(iPos) : ('.' + s);
}

// $Log: FileExtensions.C,v $
// Revision 1.2  2005/08/05 13:02:46  geconomos
// added extensions for pdf
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.5  2003/07/08 14:31:37  jmeade
// Code standards, comments.
//
// Revision 3.4  2003/06/05 16:27:31  jmeade
// code review.
//
// Revision 3.3  2003/05/16 13:08:19  frank
// code formatting
//
// Revision 3.2  2003/05/13 19:09:08  michael
// code review
//
// Revision 3.1  2003/01/06 15:03:17  ingmar
// moved .xml and .txt from static instance to static member function
//
// Revision 3.0  2001/10/14 23:02:15  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:47  ingmar
// moved from pvcs
// 
//    Rev 2.1   Sep 19 2001 14:29:56   liwei
// No change.
// 
//    Rev 2.0   Sep 16 2001 23:46:16   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:33:32   ingmar
// Initial revision.
// Revision 1.1  2001/09/01 00:08:29  soeren
// renamed utils directory
//
// Revision 1.2  2001/04/30 02:20:04  kevin
// First part of ReaderGlobal Re-organize for improved object
// encapsulation before the Code Walkthrough
//
// Revision 1.1  2001/04/12 17:00:25  jmeade
// FileExtensions.C implementation file; further simplified implementation
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/FileExtensions.C,v 1.2 2005/08/05 13:02:46 geconomos Exp $
// $Id: FileExtensions.C,v 1.2 2005/08/05 13:02:46 geconomos Exp $
