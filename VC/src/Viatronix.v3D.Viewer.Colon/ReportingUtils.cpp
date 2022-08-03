// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ReportingUtils.cpp
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#include "StdAfx.h"
#include "ReportingUtils.h"
#include "ReaderGlobal.h"
#include "File.h"
#include "ApplicationController.h"
#include "VxReportCreator.h"
#include "ReportViewWnd.h"
#include "Pdf.h"
#include "ApplicationSettings.h"
#include "vxPacsLibrary.h"
#include "ServiceLocator.h"

#include "Viatron.h"
#include "CommandLineInfoEx.h"
#include "PacsInfo.h"

USING_UTILITIES_NS


/**
 * Opens a report
 *
 * @param     sReportFilename             report file name
 * @param     spReport                    report pointer [OUT]
 * @return                                true if successful; otherwise false
 */
bool Reporting::OpenReport( const std::string sReportFilename, ReportLib::IReportPtr & spReport )
{
  try
  {
    // initialize the com runtime
    CoInitialize( NULL );

    // convenience hr
    HRESULT hr( S_OK );

    // verify that the vrx file exists
    if( !vx::File( sReportFilename ).Exists() )
    {
      LogErr( "Report vrx file does not exist", "VxVCReport", "OutputSnapshots" );
      return false;
    }
    
    // create an instance of the report
    if (FAILED( hr = spReport.CreateInstance( __uuidof( ReportLib::Report ) ) ) ) 
    {      
      LogErr( "Unable to create report instance. hr=" + ToAscii( hr ), "VxVCReport", "OutputSnapshots" );
      return false;
    }

    // open the vrx file
    spReport->Open( sReportFilename.c_str() );

		CoUninitialize();

    return true;
  }
  catch( _com_error & e )
  {
    LogErr( e.ErrorMessage(), "VxVCReport", "OutputSnapshots" );
		CoUninitialize();
    return false;
  }
  catch( ... )
  {
    LogErr( "Unhandled exception occurred while outputing snapshots.", "VxVCReport", "OutputSnapshots" );
 		CoUninitialize();
    return false;
  }
} // OpenReport( sReportFilename, spReport )


/**
 * Extracts the snapshots from the report
 *
 * @param     spRevision                  report revision
 * @param     images                      collection of snapshots [OUT]
 * @return                                true if successful; otherwise false
 */
bool Reporting::ExtractSnapshots( ReportLib::IReportRevisionPtr spRevision, SnapshotList & images )
{
  try
  {   
    // extract entries
    const int4 iNumEntries( spRevision->Entries->Count );
    for( int4 iEntry = 0; iEntry < iNumEntries; ++iEntry )
    {
      // grab finding from report
      ReportLib::IReportEntryPtr spEntry = spRevision->Entries->GetItem( (long) iEntry );

      // get number of images in the finding
      int4 iNumImages = spEntry->Images->Count;
      for( int4 iImage(0); iImage < iNumImages; ++iImage )
      {
        Snapshot image( Gdiplus::Bitmap::FromHBITMAP( (HBITMAP) spEntry->Images->GetItem( (long) iImage )->Bitmap, HPALETTE( 0 )));
        if( image.get() )
          images.push_back( image );  
      }
    }

    // extract "Non-Distended" findings
    const int4 iNumFindings = spRevision->NonDistendedFindings->Count;
    for( int4 iFinding(0); iFinding <iNumFindings; ++iFinding )
    {
      // grab the "Non-Distended" finding
      ReportLib::IReportNonDistendedFindingPtr spFinding = spRevision->NonDistendedFindings->GetItem( (long) iFinding );
      Snapshot image( Gdiplus::Bitmap::FromHBITMAP( (HBITMAP) spFinding->Image->Bitmap, HPALETTE( 0 )));
      if( image.get() )
        images.push_back( image );  
    }

    return true;
  }
  catch( ... )
  {
    LogErr( "Unhandled exception occurred while extracting snapshots.", "VxVCReport", "ExtractSnapshots" );
    return false;
  }
} // ExtractSnapshots( spRevision, images )


/**
 * Creates a pdf
 *
 * @param     spRevision                  report revision
 * @param     pdf                         pdf [OUT]
 * @return                                true if successful; otherwise false
 */
bool Reporting::CreatePdf( ReportLib::IReportRevisionPtr spRevision, std::shared_ptr<pdf::Pdf> & pPdf )
{
  try
  {
    ConsoleLib::VxReportCreator creator( spRevision );
    pPdf = creator.CreatePdf();
    return true;
  }
  catch( ... )
  {
    LogErr( "Unhandled exception while creating pdf.", "vcUtils", "CreatePdf" );
    return false;
  }
} // CreatePdf( spRevision, pPdf )


/**
 * mark Report Completed
 *
 *  @param bIsComplete bool flag = true if you want to mark as complete
 */
void Reporting::MarkComplete( const bool & bIsComplete )
{
	if( bIsComplete )
	{
    ColonViews::Layouts::ReportLayout::SetMarkCompleteFlag( bIsComplete );
    Colon::ApplicationController::Notify( Colon::ReportArgs() );
	}
	else
  {
    Colon::ApplicationController::Notify( Colon::ReportArgs() );
		ColonViews::Layouts::ReportLayout::SetMarkCompleteFlag( bIsComplete );
	}
} // MarkCompleted()


/**
 * Is the report completed
 */
bool Reporting::IsComplete()
{
  return ( ColonViews::Layouts::ReportLayout::CanOpenReport() && ColonViews::Layouts::ReportLayout::GetReport() != nullptr && !ColonViews::Layouts::ReportLayout::CanModifyReport() );
} // IsCompleted()


/**
 * send to pacs
 *
 * @param       spRevision               report
 * @param       uid                      dicom uid
 * @param       eOptions                 pacs send options
 */
void Reporting::SendToPacs( ReportLib::IReportRevisionPtr spRevision,  const std::string & uid, PacsSendOptions eOptions )
{

  try
  {
    std::shared_ptr< pdf::Pdf > pdf( nullptr );
    SnapshotList images;

    if( eOptions & PACS_SEND_REPORT )
    {
      LogDbg( "Preparing report for pacs transfer.", "Reporting", "SendToPacs" );
      Utilities::Reporting::CreatePdf( spRevision, pdf );
    }

    if( eOptions & PACS_SEND_SNAPSHOTS )
    {
      LogDbg( "Preparing snapshots for pacs transfer.", "Reporting", "SendToPacs" );
      Utilities::Reporting::ExtractSnapshots( spRevision, images );
    }

    if( eOptions & PACS_SEND_REPORT || eOptions & PACS_SEND_SNAPSHOTS )
      ServiceLocator::GetPacsService()->Send( uid, pdf, images );

  }
  catch ( dcm::DcmException & ex )
  {
    LogErr("Failed to store to PACS : " + ex.GetTextMessage(), "Reporting", "SendToPacs");
    throw;
  }

} // SendToPacs( spRevision, uid, eOptions )