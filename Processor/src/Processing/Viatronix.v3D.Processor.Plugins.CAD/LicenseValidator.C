// $Id: LicenseValidator.C,v 1.2 2006/01/27 20:31:37 jmeade Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )

/// includes
#include "StdAfx.h"
#include "LicenseValidator.h"

/// defines
#define FILE_REVISION "$Revision: 1.2 $"


/**
 * Constructor
 */
LicenseValidator::LicenseValidator()
{
  /// create abort event
  m_hAbortRenewalThread = CreateEvent( NULL, TRUE, FALSE, NULL );
} // LicenseValidator()


/**
 * Destructor
 */
LicenseValidator::~LicenseValidator()
{
  Release();

  if( m_hAbortRenewalThread )
    CloseHandle( m_hAbortRenewalThread );

} // ~LicenseValidator()


/**
 * Requests the specified license
 *
 * @param   requestInfo           contains information about the license being request
 * @param   bStartRenewalThread   indicates if the renewal thread should be started
 * @return  true if license was granted; otherwise false  
 */
bool LicenseValidator::Request( const LicenseRequestInfo & requestInfo, bool bStartRenewalThread )
{
  HRESULT hr( S_OK );
    
  try
  {
    // release the licensing if it is already instaniaited
    Release();

    // create an instance of the licensing client
    if( FAILED( hr = m_spLicenseClient.CoCreateInstance( CLSID_UnmanagedLicenseClient ) ) )
    {
      LogErr( std::string( "Unable to create instance of licensing client. HRESULT= " ) + ToAscii( hr ), "LicenseValidator", "Request" );
      return false;
    }

    // marshal the pointer into an IStream
    if( FAILED( hr = AtlMarshalPtrInProc( m_spLicenseClient,  __uuidof( _UnmanagedLicenseClient ), &m_spMarshalStream ) ) )
    {
      LogErr(std::string( "Unable to marshal license client interface. HRESULT=" ) + ToAscii( hr ), "LicenseValidator", "Request" );
      return false;
    }

    // set the required information on the licensing client
    if(  FAILED( hr = m_spLicenseClient->put_ProductID ( CComBSTR( requestInfo.GetProductId().c_str() ) ) )
      || FAILED( hr = m_spLicenseClient->put_UserID    ( CComBSTR( requestInfo.GetUserId().c_str()    ) ) )
      || FAILED( hr = m_spLicenseClient->put_SeriesID  ( CComBSTR( requestInfo.GetSeriesId().c_str()  ) ) ) )
    {
      LogLastError( "Could not put License ID information", "Request", hr );
      return false;
    }

    // request a license
    VARIANT_BOOL bResult( VARIANT_FALSE );
    if( FAILED( hr = m_spLicenseClient->RequestLicense( &bResult ) ) || ( bResult == VARIANT_FALSE  ) )
    {
      LogLastError( "Request for license " + requestInfo.GetProductId() + " failed", "Request", hr );
      return false;
    }

    // do we need to start the renewal thread
    if( bStartRenewalThread )
      Start();
  }
  catch( ... )
  {
    LogErr( "Unhandled exception", "LicenseValidator", "Request" );
    return false;
  }
  return true;
} // Request( const LicenseRequestInfo & requestInfo, bool bStartRenewalThread )


/** 
 * Releases the license
 * 
 * @return    true if license was rleased successfully; false otherwsie
 */
bool LicenseValidator::Release()
{
  /*
  try
  {
    HRESULT hr( S_OK );

    // has the license validator been initilized?
    if( !m_spLicenseClient )
      return false;

    // is the renewal thread running?
    if( IsRunning() )
    {

      // indicate that we wish to abort the renewal thread
      SetEvent( m_hAbortRenewalThread );
    
      // wait for the thread to terminate
      Wait();
    }

    // release the license
    VARIANT_BOOL bResult( VARIANT_FALSE );
    if( ( FAILED( hr = m_spLicenseClient->ReleaseLicense( &bResult ))) || ( bResult == VARIANT_FALSE ) )
    {
      LogLastError( "Could not release software license", "Release", hr );
      return false;
    }

    // release the licensing client
    m_spLicenseClient = NULL;
  }
  catch (...)
  {
    LogErr( "Unhandled exception", "LicenseValidator", "Release" );
    return false;
  }
	*/
  return true;
} // Release()


/**
 * Renewal thread. 
 */
void LicenseValidator::Run()
{
  try
  {
    HRESULT hr( S_OK );

    /// unmarshal the interface pointer from the IStream
    CComPtr< _UnmanagedLicenseClient > spLicenseClient;
    if( FAILED( hr = AtlUnmarshalPtr( m_spMarshalStream, __uuidof( _UnmanagedLicenseClient ), reinterpret_cast< IUnknown** >( &spLicenseClient ) ) ) )
    {
      LogErr( std::string( "Unable to unmarshal license client interface. HRESULT= " ) + ToAscii( hr ), "LicenseValidator", "Run" );
      return;
    }

    // frequency to make renewal requests ( in milliseconds )
    static int4 RENEWAL_FREQUENCY( 60 * 1000 * 4 );

    // loop until abort handle is signaled or license cannot be renewed
    while( WaitForSingleObject( m_hAbortRenewalThread, RENEWAL_FREQUENCY ) != WAIT_OBJECT_0 )
    {        
      // attempt to renew the license
      VARIANT_BOOL bRenewed( VARIANT_FALSE );
      if( FAILED( hr = spLicenseClient->RenewLicense( &bRenewed ) ) )
      {
        LogErr( std::string( "Renew license call failed. HRESULT= " ) + ToAscii( hr ), "LicenseValidator", "Run" );
        return;
      }

      // were we granted a renewal?
      if( bRenewed == VARIANT_FALSE )
      {
        LogLastError( "The renewal of the license was not granted.", "Run", hr );
        return;
      }
    }
    LogDbg( "Renewal thread aborting", "LicenseValidator", "Run" );

  } // try
  catch( ... )
  {
    LogErr( "Unhandled exception", "LicenseValidator", "RenewalThread" );
  }
} // operator()()


/**
 * Outputs current error code and message to log file
 */
void LicenseValidator::LogLastError( const std::string & sMsg, const std::string & sMethod, const HRESULT hr )
{
  LogErr( sMsg + " Extended Message: " + GetLastErrorMessage(), "LicenseValidator", sMethod );
} // LogLastError( const std::string & sMsg, const std::string & sMethod, const HRESULT hr )


/**
 * Gets the last error message
 *
 * @return last error string
 */
std::string LicenseValidator::GetLastErrorMessage() const
{
  USES_CONVERSION;

  std::string sErrorMessage;

  // make sure that we instantiated the license client
  if( !m_spLicenseClient == false )
  {
    // get the last error message from the license server
    BSTR bsMsg;
    m_spLicenseClient->get_ErrorMessage( &bsMsg );
    
    // get the last error code
    long lResult;
    m_spLicenseClient->get_Result( &lResult );

    // format the message
    static char sTmp[ 1024 ];
    wsprintf( sTmp, "%s ( %d )", W2A( bsMsg ), lResult  );
    
    sErrorMessage = sTmp;
  }
  return sErrorMessage;
} // GetLastErrorMessage() const



// $Log: LicenseValidator.C,v $
// Revision 1.2  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.1.2.2  2006/01/16 15:46:12  geconomos
// modified renewal frequency to 4 minutes
//
// Revision 1.1.2.1  2006/01/16 15:42:28  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/LicenseValidator.C,v 1.2 2006/01/27 20:31:37 jmeade Exp $
// $Id: LicenseValidator.C,v 1.2 2006/01/27 20:31:37 jmeade Exp $
