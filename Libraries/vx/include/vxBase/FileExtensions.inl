// $Id: FileExtensions.inl,v 1.2 2006/06/01 20:29:01 frank Exp $
//
// Copyright© 2000-2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner(s): Jeffrey Meade  jmeade@viatronix.net


/**
 * Appends a file extension on a string (typically a file title).
 *
 * @param sStr      String (e.g. file title).
 * @param fileExt   File extension to append.
 * @return          sStr.fileExt
 */
inline std::string operator+(const std::string & sStr, const vx::FileExtension & fileExt)
{
  if ( sStr.length() == 0 )
    return sStr;

  if (sStr[sStr.length() - 1] == '.')
  {
    return sStr + std::string(fileExt).substr(1, fileExt.GetExtensionLen());
  }
  else
  {
    return sStr + std::string(fileExt);
  }
}


/**
 * Determines whether the filename ends with the given extension.
 *
 * @param sFilename   Filename to test.
 * @param fileExt     File extension to test.
 * @return            True if the filename ends with the given extension.
 */
inline bool operator==(const std::string & sFilename, const vx::FileExtension & fileExt)
{
  return (fileExt == sFilename);
}


/**
 * Determines whether the filename ends with the given extension.
 *
 * @param sFilename   Filename to test.
 * @param fileExt     File extension to test.
 * @return            True if the filename DOES NOT end with the given extension.
 */
inline bool operator!=(const std::string & sFilename, const vx::FileExtension & fileExt)
{
  return !(fileExt == sFilename);
}


/**
 * Determines whether the filename ends with the given extension.
 *
 * @param sFilename   Filename to test.
 * @param fileExt     File extension to test.
 * @return            True if the filename ends with the given extension.
 */
inline bool operator==(const char * sFilename, const vx::FileExtension & fileExt)
{
  return (fileExt == sFilename);
}


/**
 * Determines whether the filename ends with the given extension.
 *
 * @param sFilename   Filename to test.
 * @param fileExt     File extension to test.
 * @return            True if the filename DOES NOT end with the given extension.
 */
inline bool operator!=(const char * sFilename, const vx::FileExtension & fileExt)
{
  return !(fileExt == sFilename);
}

// $Log: FileExtensions.inl,v $
// Revision 1.2  2006/06/01 20:29:01  frank
// updated to visual studio 2005
// buffer overrun problem fixed
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 3.2  2003/07/08 14:31:38  jmeade
// Code standards, comments.
//
// Revision 3.1  2003/06/05 16:27:32  jmeade
// code review.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/FileExtensions.inl,v 1.2 2006/06/01 20:29:01 frank Exp $
// $Id: FileExtensions.inl,v 1.2 2006/06/01 20:29:01 frank Exp $
