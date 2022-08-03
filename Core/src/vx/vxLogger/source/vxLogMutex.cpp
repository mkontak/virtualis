// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: vxLogMutex.cpp
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#include "stdafx.h"
#include "vxLogMutex.h"

/**
 * Constructor
 *
 * @param sName   Mutex name
 */
vxLogMutex::vxLogMutex(const std::string & sName) :
m_sMutexName(sName),
m_hMutex((HANDLE)(__nullptr))
{


} // vxLogMutex()


/**
 * Destructor : close the mutex sHandle
 */
vxLogMutex::~vxLogMutex()
{
  if ( m_hMutex != (HANDLE)(__nullptr) )
  {

    BOOL status = ReleaseMutex(m_hMutex);

    status = CloseHandle ( m_hMutex );

    if ( status == FALSE )
      vxLogUtility::LogMessage(util::Strings::Format("Failed to close mutex %s [%d]", m_sMutexName.c_str(), GetLastError()));

    m_hMutex = (HANDLE)(__nullptr);

  }

} // ~vxLogMutex()

/**
 * Waits for the mutex
 */
void vxLogMutex::Wait()
{
  if ( m_hMutex != (HANDLE)(__nullptr) )
    if ( WaitForSingleObject( m_hMutex, INFINITE ) != WAIT_OBJECT_0 )
       vxLogUtility::LogMessage(util::Strings::Format("Failed wait of mutex %s, no locking is used [%d]", m_sMutexName.c_str(), GetLastError()));

} // Wait()


void vxLogMutex::Release()
{
  if ( m_hMutex != (HANDLE)(__nullptr) )
    ReleaseMutex(m_hMutex);

} 

/**
 * Creates the mutex
 */
std::shared_ptr<vxLogMutex> vxLogMutex::Create(const std::string & sName)
{

  std::shared_ptr<vxLogMutex> pMutex = std::shared_ptr<vxLogMutex>(new vxLogMutex(sName));

  if ( pMutex == __nullptr )
    throw vxLogException(LogRec("Failed to allocate vxLogMutex","vxLogMutex","Create"));


  pMutex->Create();

  return pMutex;


}


void vxLogMutex::Create()
{
  if ( m_hMutex == (HANDLE)(__nullptr) )
  {
    //Provide SD for CreateMutex to avoid access level crash.
    SECURITY_ATTRIBUTES attributes;
    RtlZeroMemory( &attributes, sizeof( attributes ) );

    attributes.nLength = sizeof( attributes );
    attributes.bInheritHandle = FALSE; 
    SECURITY_DESCRIPTOR SD;
    InitializeSecurityDescriptor( &SD, SECURITY_DESCRIPTOR_REVISION );


    // give the security descriptor a Null Dacl for access to all
    SetSecurityDescriptorDacl( &SD, TRUE, (PACL)NULL , FALSE );

    // Make the security attributes point to the security descriptor
    attributes.lpSecurityDescriptor = &SD;

    // create the mutex
    m_hMutex = CreateMutex( &attributes, FALSE, m_sMutexName.c_str() );
  
    if ( m_hMutex == (HANDLE)(__nullptr) )
      throw vxLogException(LogRec("Failed to create/open mutex", "vxLogMutex", "vxLogMutex"));
  }

}