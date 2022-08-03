// $Id: v3D_GraphTimerEx.h,v 1.1.2.2 2009/07/29 16:43:29 kchalupa Exp $
//
// Copyright © 2008, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:geconomos@viatronix.com)

// pragma declarations
#pragma once


// namespace declarations
OPEN_GRAPH_NS


/**
 * Represents a timer that can start stop and specify if running
 */
ref class GraphTimerEx
{
// construction
public:
  
  /// constructor
  GraphTimerEx();

// member functions
public:

  /// starts the timer
  void Start();

  /// stops the timer
  void Stop();

// properties
public:

  /// is the timer running
  property bool IsRunning
  {
    bool get() { return m_bRunning; }
  } // IsRunning

   /// returns the singleton instance
  static property GraphTimerEx ^ Instance
  {
    GraphTimerEx ^ get() { return m_mpInstance; }
  } // Instance

// event handlers
private:

  /// called when message queue is empty
  void OnApplicationIdleHandler( System::Object ^ mpSender, System::EventArgs ^ mpArgs );

// member variables
private:

  /// Application.Idle handler
  System::EventHandler ^ m_mpApplicationIdleHandler;

  /// indicates if the timer is running
  bool m_bRunning;

  /// singleton instance
  static GraphTimerEx ^ m_mpInstance = gcnew GraphTimerEx();

}; // ref class GraphTimerEx


CLOSE_GRAPH_NS


// $Log: v3D_GraphTimerEx.h,v $
// Revision 1.1.2.2  2009/07/29 16:43:29  kchalupa
// Code Walkthrough
//
// Revision 1.1.2.1  2009/01/27 17:26:19  gdavidson
// Initial revision - Moved from Colon plugin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_GraphTimerEx.h,v 1.1.2.2 2009/07/29 16:43:29 kchalupa Exp $
// $Id: v3D_GraphTimerEx.h,v 1.1.2.2 2009/07/29 16:43:29 kchalupa Exp $
