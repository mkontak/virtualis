// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ReportingUtils.h
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#pragma once

// includes
#include <vector>
#include <memory>
#include "ReaderGlobal.h"



#if defined(_DEBUG)
# import "..\..\..\..\Libraries\com\ReportLibd.dll" named_guids
#elif defined(FINAL_RELEASE)
# import "..\..\..\..\Libraries\com\ReportLib.dll" named_guids
#else
# import "..\..\..\..\Libraries\com\ReportLibdr.dll" named_guids
#endif

OPEN_UTILITIES_NS


namespace Reporting
{
  /// collection of bitmaps
  typedef std::shared_ptr< Gdiplus::Bitmap > Snapshot;
  typedef std::vector< Snapshot > SnapshotList;

  /// opens the report
  bool OpenReport( const std::string sReportFilename, ReportLib::IReportPtr & spReport );

  /// extract the report images
  bool ExtractSnapshots( ReportLib::IReportRevisionPtr spRevision, SnapshotList & images );

  /// create pdf
  bool CreatePdf( ReportLib::IReportRevisionPtr spRevision, std::shared_ptr<pdf::Pdf> & pPdf );

  /// mark Report Completed
  void MarkComplete( const bool & bIsComplete );

  /// is Report Completed
  bool IsComplete();

  /// send to pacs options
  enum PacsSendOptions
  {
    PACS_SEND_NONE      = 0x0,
    PACS_SEND_SNAPSHOTS = 0x1,
    PACS_SEND_REPORT    = 0x2,
  };

  /// send to pacs
  void SendToPacs( ReportLib::IReportRevisionPtr spRevision, const std::string & uid, PacsSendOptions eOptions );

}; // namespace Reporting

CLOSE_UTILITIES_NS