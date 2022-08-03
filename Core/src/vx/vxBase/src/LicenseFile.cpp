// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: LicenseFile.cpp
//
// Owner: Mark Kontak  mailto:mkontak@viatronix.com


#include "StdAfx.h"
#include "LicenseFile.h"
#include <strstream>
#include "Exception.h"
#include "crypto\CryptStream.h"




/**
 *  create
 */
void LicenseFile::Create()
{
  m_systemID = sys::Wmi::GetSystemId();
} // Create


/**
 *  open
 *
 *  @param input
 */
void LicenseFile::Open( std::istream & input )
{
  if( !input )
    throw ex::RuntimeException( LogRec( "Unable to read license file.", "LicenseFile", "Open" ));

  m_systemID = crypto::CryptStream::Decrypt( input );
} // Open


/**
 *  save
 *
 *  @param output
 */
void LicenseFile::Save( std::ostream & output )
{
  if( !output )
    throw ex::RuntimeException( LogRec( "Unable to write license file.", "LicenseFile", "Save" ));

  std::stringstream stream;
  stream << m_systemID;

  crypto::CryptStream::Encrypt( output, m_systemID );
} // Save


/**
 *  check if license is valid
 *
 *  @return true if it is valid
 */
bool LicenseFile::IsValid()
{
  return sys::Wmi::GetSystemId() == m_systemID;
} // IsValid


/**
 *  generate
 *
 *  @param sFile
 */
void LicenseFile::Generate( const std::string & sFile ) 
{ 
  LicenseFile license;
  license.Create();
  license.Save( sFile );

  SetFileAttributes( sFile.c_str(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY );
} // Generate


/**
 *  check
 *
 *  @param sFile
 *  @return true if license is valid
 */
bool LicenseFile::Check( const std::string & sFile )
{
  try
  {
    LicenseFile license;
    license.Open( sFile );

    return license.IsValid();
  }
  catch( ex::VException & )
  {
    return false;
  }
} // Check