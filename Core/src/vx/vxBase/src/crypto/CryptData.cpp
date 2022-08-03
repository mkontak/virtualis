// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: CryptData.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net

#include "stdafx.h"
#include "crypto\CryptData.h"
#include "Exception.h"



using namespace crypto;


/**
 *  constructor
 */
CryptData::CryptData() :
m_iAllocatedSize(0)
{
  m_blob.cbData = 0;
  m_blob.pbData = __nullptr;
} // CryptData()


/**
 * constructor
 *
 *  @param sText
 */
CryptData::CryptData(const std::string & sText) :
m_iAllocatedSize(0)
{
  m_iAllocatedSize = sText.size() * 4;
  m_blob.cbData = sText.size();
  m_blob.pbData = new BYTE[m_iAllocatedSize];

  if ( m_blob.pbData == __nullptr )
    throw ex::CryptOutOfMemoryException( LogRec( "Failed to allocate data buffer", "CryptData", "CryptData" ));

  memcpy((char *)m_blob.pbData, sText.c_str(), sText.size());

} // CryptData


/**
 * constructor
 *
 *  @param iSize
 */
CryptData::CryptData(int iSize) :
m_iAllocatedSize(iSize)
{

  m_blob.cbData = iSize;
  m_blob.pbData = new BYTE[iSize];

  if ( m_blob.pbData == __nullptr )
    throw ex::CryptOutOfMemoryException( LogRec( "Failed to allocate data buffer", "CryptData", "CryptData" ));

} // CryptData(


/**
 *  destructor
 */
CryptData::~CryptData()
{

  if ( m_blob.pbData != __nullptr )
  {
    delete [] m_blob.pbData;
    m_blob.pbData = __nullptr;
    m_blob.cbData = 0;
    m_iAllocatedSize = 0;
  }

} // ~CryptData()


/**
 * conversion to string
 *
 *  @return the string
 */
std::string CryptData::ToString()
{
  std::string sReturn;

  for ( register int i(0); i < m_blob.cbData; i++ )
    sReturn.push_back(m_blob.pbData[i]);


  return sReturn;
} // ToString