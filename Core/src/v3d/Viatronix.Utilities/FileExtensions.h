// $Id: FileExtensions.h,v 1.3 2006/02/09 14:10:37 geconomos Exp $
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/FileExtensions.h,v 1.3 2006/02/09 14:10:37 geconomos Exp $
//
// Copyright© 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Mark Kontak  mkontak@viatronix.com
//

// Pragmas
#pragma once

// Includes
#include "UtilityEnums.h"

using namespace System;

OPEN_IO_NS

/**
 * Implements file extensions identifying extensions we use within our applications
 */
public ref class FileExtensions
{
private:
  /// Constructor
  FileExtensions() {}

  /// Destructor
  ~FileExtensions() {}

  /// Returns 0 if the extension are the same < 1 or > 1
  static __forceinline int Compare(System::String ^ sFile, System::String ^ sExtension)
  { return System::String::Compare(sExtension, System::IO::Path::GetExtension(sFile), true ); }

public:

  static String ^ Lock = ".lck";
  static String ^ Vol = ".vol";
  static String ^ Volume = ".vol";
  static String ^ Ecv = ".ecv";
  static String ^ Dicom = ".dcm";
  static String ^ Iso = ".iso";
  static String ^ Pdf = ".pdf";
  static String ^ Png = ".png";
  static String ^ Bitmap = ".bmp";
  static String ^ Text = ".txt";
  static String ^ Xml = ".xml";
  static String ^ Jpeg = ".jpg";
  static String ^ Gif = ".gif";
  static String ^ Tiff = ".tiff";
  static String ^ Icon = ".ico";
  static String ^ Ct8 = ".ct8";
  static String ^ ECG = ".iri";
  static String ^ Config = ".config";
  static String ^ Log = ".log";
  static String ^ Queue = ".que";
  static String ^ Vrx = ".vrx";
  static String ^ VCReport = ".tiff";
  static String ^ Avi = ".avi";
  static String ^ Wmv = ".wmv";
  static String ^ Asf = ".asf";



};  // class FileExtensions
      
CLOSE_IO_NS


// Revision History:
// $Log: FileExtensions.h,v $
// Revision 1.3  2006/02/09 14:10:37  geconomos
// removed using namepace from header
//
// Revision 1.2  2005/11/08 17:46:49  mkontak
// Added features
//
// Revision 1.1  2005/08/30 13:42:49  mkontak
// Split from V3D_File
//
// Revision 1.13  2005/06/27 17:49:29  mkontak
// Merge  of Console 2.1 branch
//
// Revision 1.12  2005/05/13 18:02:01  mkontak
// Added VrxExt
//
// Revision 1.11  2005/02/22 14:24:22  mkontak
// Removed methods no longer needed
//
// Revision 1.10  2005/02/02 19:47:21  mkontak
// Fixes
//
// Revision 1.9  2004/11/01 16:10:51  mkontak
// Pacs Queue Server Changes
//
// Revision 1.8  2004/09/10 18:05:23  mkontak
// Moved to SystemSettings and usage of System::Microsoft::Win32::Registry
//
// Revision 1.7.2.4  2005/03/21 20:49:48  mkontak
// Fix to return SUCCESS when ignoring DICOM and missing  UID's
//
// Revision 1.7.2.3  2005/02/10 16:00:46  mkontak
// Fixes and additionas for the Dicom Server Patch
//
// Revision 1.10  2005/02/02 19:47:21  mkontak
// Fixes
//
// Revision 1.9  2004/11/01 16:10:51  mkontak
// Pacs Queue Server Changes
//
// Revision 1.8  2004/09/10 18:05:23  mkontak
// Moved to SystemSettings and usage of System::Microsoft::Win32::Registry
//
// Revision 1.7  2004/06/28 17:57:32  mkontak
// Added Rename method
//
// Revision 1.6  2004/05/18 13:08:06  mkontak
// Added LogExt
//
// Revision 1.5  2004/05/17 20:25:04  mkontak
// Minor changes for the new WQ handler
//
// Revision 1.4  2004/05/11 19:13:18  mkontak
// Changes for the WQ
//
// Revision 1.3  2004/04/26 20:31:11  mkontak
// Added config extension
//
// Revision 1.2  2004/04/05 18:26:53  mkontak
// Added GetLocalPath()
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2003/10/31 15:17:45  mkontak
// Added CopyFiles method to IO::File
//
// Revision 1.5  2003/10/31 13:24:34  mkontak
// Added remote registry access
//
// Revision 1.4  2003/10/30 14:30:29  mkontak
// Copied the ENUMS from BRL to utilities, added GetStudyPath to File and added
// some properties to DNSRegistery
//
// Revision 1.3  2003/06/20 15:09:36  mkontak
// Coding standards
//
// Revision 1.2  2003/05/22 18:41:47  dongqing
// code review
//
// Revision 1.1  2003/05/15 14:24:32  mkontak
// Cosmetic
//
// Revision 1.15  2003/05/15 12:29:12  mkontak
// Added extensions
//
// Revision 1.14  2003/05/05 12:23:30  mkontak
// Reorganized registry added new funtionality
//
// Revision 1.13  2003/04/22 14:00:21  mkontak
// Used #pragma once
//
// Revision 1.12  2003/04/21 20:40:49  mkontak
// CD Writing/Anonymization
//
// Revision 1.11  2003/03/26 21:50:26  mkontak
// Added CT8 extension
//
// Revision 1.10  2003/03/12 17:10:36  geconomos
// Moved Viatronix.Utilities.File to Viatronix.Utilities.IO.File
//
// Revision 1.9  2003/01/17 21:51:32  mkontak
// Added GetAbsolutePath()
//
// Revision 1.8  2002/11/22 18:30:51  mkontak
// Added extensions and additional methods
//
// Revision 1.7  2002/10/09 20:55:45  mkontak
// Added XmlExt
//
// Revision 1.6  2002/09/30 14:08:04  mkontak
// Added TEXT extension
//
// Revision 1.5  2002/08/27 21:09:48  mkontak
// Added additional file extensions
//
// Revision 1.4  2002/08/23 16:54:49  mkontak
// Added additaionl registry supprt
//
// Revision 1.3  2002/08/12 19:28:27  mkontak
// CD Spanning
//
// Revision 1.2  2002/08/05 20:53:40  mkontak
// Added CDR writing with some fixes to the File and Locker classes
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities/FileExtensions.h,v 1.3 2006/02/09 14:10:37 geconomos Exp $
// $Id: FileExtensions.h,v 1.3 2006/02/09 14:10:37 geconomos Exp $
