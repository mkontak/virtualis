// $Id: SingleLock.h,v 1.2 2007/03/01 18:47:05 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (mailto:geconomos@viatronix.com)

#pragma once

// class definition
template< typename SyncObject >
class  SingleLock
{
// member functions
public:

  /**
   * Constructor
   *
   * @param   pSyncObject   object to provide access for
   * @param   bInitialLock  specifies whether to initially attempt to access the supplied object
   */
  inline SingleLock( SyncObject * pSyncObject, bool bInitialLock ) : m_pSyncObject( pSyncObject ), m_bAcquired( false )
  {
    if( bInitialLock )
    {
      Lock();
    }
  } // Constructor


  /**
   * non-virtual destructor 
   */
  inline ~SingleLock()
  {
    Unlock();
  } // ~SingleLock()


  /**
   * Waits on a synchronization object
   */
  inline void Lock() 
  {    
    if( !m_bAcquired )
    {
      m_pSyncObject->Lock();
      m_bAcquired = true;
    }
  } // Lock()

  
  /**
   * Releases a synchronization object
   */
  inline void Unlock() 
  { 
    if( m_bAcquired )
    {
      m_pSyncObject->Unlock(); 
      m_bAcquired = false;
    }
  } // Unlock()


  /**
   * Determines if the object is locked
   *
   * @return true if object is locked
   */
  inline bool IsLocked() 
  { 
    return m_bAcquired; 
  } // IsLocked() 


// member variables
private:

  /// private sync object
  SyncObject * m_pSyncObject;

  /// indicates if the lock has been acquired
  bool m_bAcquired;

}; // CriticalSection



// $Log: SingleLock.h,v $
// Revision 1.2  2007/03/01 18:47:05  geconomos
// code review preparation
//
// Revision 1.1  2006/03/29 19:34:21  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/SingleLock.h,v 1.2 2007/03/01 18:47:05 geconomos Exp $
// $Id: SingleLock.h,v 1.2 2007/03/01 18:47:05 geconomos Exp $
