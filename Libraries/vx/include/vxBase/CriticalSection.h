// $Id: CriticalSection.h,v 1.2 2006/03/29 19:37:36 geconomos Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Author: George Economos (george@viatronix.com)

#pragma once

// class definition
class CriticalSection
{

// member functions
public:
  
  /**
   * Constructor
   */
  inline CriticalSection() 
  {
    InitializeCriticalSection( &m_section );
  } // CriticalSection()

  
   /**
    * non-virtual destructor 
    */
  inline ~CriticalSection()
  {
    DeleteCriticalSection( &m_section );
  } // ~CriticalSection()


  /**
   * waits for ownership of the critical section object. The function
   * returns when the calling thread is granted ownership.
   */
  inline void Lock()
  {
    EnterCriticalSection( &m_section );
  } // Lock()

  
  /**
   * releases ownership of the critical section object
   */
  inline void Unlock()
  {
    LeaveCriticalSection( &m_section );
  } // Unlock()


 
// memeber variables
private:
    
  // window's critical section object
  mutable CRITICAL_SECTION m_section;

}; // CriticalSection


// $Log: CriticalSection.h,v $
// Revision 1.2  2006/03/29 19:37:36  geconomos
// code cleanup
//
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2003/05/16 13:07:42  frank
// code formatting
//
// Revision 1.3  2003/05/16 11:43:45  michael
// code review
//
// Revision 1.2  2003/05/12 16:31:08  michael
// code review
//
// Revision 1.1  2002/10/17 22:20:16  ingmar
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/CriticalSection.h,v 1.2 2006/03/29 19:37:36 geconomos Exp $
// $Id: CriticalSection.h,v 1.2 2006/03/29 19:37:36 geconomos Exp $
