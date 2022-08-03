// $Id: Mutex.h,v 1.3 2006/03/30 14:14:46 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (george@viatronix.com)

// pragma declarations
#pragma once

// class definition
class Mutex
{

// member functions
public:

  /**
   * Constructor
   */
  inline Mutex()
  {
    m_hMutex = CreateMutex( NULL, FALSE, NULL );
  } // Mutex()

   /**
    * Constructor
    */
  inline Mutex( const std::string & sName )
  {
    m_hMutex = CreateMutex( NULL, FALSE, sName.c_str() );
  } // Mutex()

  
  /**
   * non-virtual destructor 
   */
  inline  ~Mutex()
  {
    if( m_hMutex )
    {
      CloseHandle( m_hMutex );
    }
  } // ~Mutex()


  /**
   * Waits for ownership of the mutex object. The function
   * returns when the calling thread is granted ownership.
   */
  inline void Lock()
  {
    WaitForSingleObject( m_hMutex, INFINITE );
  } // Lock()
  

  /**
   * Releases ownership of the mutex object
   */
  inline void Unlock()
  {
    ReleaseMutex( m_hMutex ); 
  } // Unlock()
  
  
  /**
   * Determines if the calling threaded owns the mutex object
   *
   * @return  true if calling thread owns mutex object
   */
  inline bool IsLocked() 
  {
    return WaitForSingleObject( m_hMutex, 0 ) == WAIT_TIMEOUT;
  } // IsLocked()


// member variables
private:

  // window's mutex object
  HANDLE m_hMutex;
}; // Mutex



// $Log: Mutex.h,v $
// Revision 1.3  2006/03/30 14:14:46  geconomos
// added  ability to name mutex
//
// Revision 1.2  2006/03/29 19:40:51  geconomos
// converted all methods to inline
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2003/05/16 13:07:42  frank
// code formatting
//
// Revision 1.4  2003/05/12 16:31:08  michael
// code review
//
// Revision 1.3  2002/07/02 20:45:01  geconomos
// Exporting classes .
//
// Revision 1.2  2002/03/13 14:26:49  geconomos
// Coding conventions.
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/Mutex.h,v 1.3 2006/03/30 14:14:46 geconomos Exp $
// $Id: Mutex.h,v 1.3 2006/03/30 14:14:46 geconomos Exp $
