// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DicomPacsService.cpp
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#include "StdAfx.h"
#include "DicomPacsService.h"
#include "Pdf.h"
#include "ApplicationSettings.h"
#include "vxPacsLibrary.h"
#include "ServiceLocator.h"
#include "Viatron.h"


/**
 * destructor
 */
DicomPacsService::~DicomPacsService()
{
} // ~DicomPacsService()


/**
 * Acquires a context for the specified parameters. The newly acquired context will be returned in the sContextId parameters.
 *
 * @param     uid                 uid
 * @param     pdf                 pdf
 * @param     images              images
 */
void DicomPacsService::Send( const std::string & uid, std::shared_ptr<pdf::Pdf> & pdf, Images images )
{
  try
  {
    // create pacs client
    ApplicationSettings::ApplicationState state( ApplicationSettings::Create( CONFIGURATION_GLOBAL )); 

    std::string sAETitle(state.Get( APP_SETTING_PACS_AETITLE_SERVER ));

    std::shared_ptr<dcm::DcmHost> pDcmHost(dcm::DcmHost::Create( sAETitle, state.Get( APP_SETTING_PACS_AETITLE_SERVER ), state.Get( APP_SETTING_PACS_HOSTNAME ), state.Get<uint4>( APP_SETTING_PACS_PORT ) ) );
  
    pDcmHost->AddAttribute( DCM_SupportsEncapsulatedPdf, state.Get(APP_SETTING_PACS_SUPPORTS_ENCAPSULATED_PDF )  );

    std::shared_ptr< pacs::PacsClient > pacs( pacs::PacsClient::Create( state.Get( APP_SETTING_PACS_AETITLE_LOCAL ), pDcmHost ) );

    // retrieve dcm header
    std::string dcmHeader( ServiceLocator::GetRetrievalService()->GetDicomHeader( ViatronApp::GetViatronApp()->GetContextId(), uid ));

    // add pdf and images
    if( pdf != nullptr )
    {
      LogDbg( "Adding report to pacs.", "DicomPacsService", "Send" );
      pacs->AddPdf( dcmHeader, pdf );
    }

    if( !images.empty() )
    {
      LogDbg( "Adding snapshots to pacs.", "DicomPacsService", "Send" );
      pacs->AddSnapshots( dcmHeader, images ); 
    }
        
    LogDbg( "Starting pacs transfer.", "DicomPacsService", "Send" );

    pacs->Store( true, state.Get<bool>( APP_SETTING_PACS_SINGLE_SERIES ) );

    
    LogDbg( "PACS transfer completed.", "DicomPacsService", "Send" );

  }
  catch ( dcm::DcmException & ex )
  {
    LogErr("Failed to store to PACS : " + ex.GetTextMessage(), "DicomPacsService","Send");
    throw;
  }
  catch ( ... )
  {
    throw dcm::DcmException(LogRec("Failed to store to pacs : unspecified error", "DicomPacsService", "Send"));
  }
} // Login( sUser, sContextId, sPassword )

