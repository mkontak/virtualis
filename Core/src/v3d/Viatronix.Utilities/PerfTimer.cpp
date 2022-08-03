// $Id: PerfTimer.cpp,v 1.2 2007/03/01 19:13:08 geconomos Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos (mailto:geconomos@viatronix.com)

#include "StdAfx.h"
#include "PerfTimer.h"

USING_UTILITIES_NS

/**
 * Constructor
 */
PerfTimer::PerfTimer()
{
  m_pTimer = new Timer();
} // Timer()


/**
 * Destructor
 */
PerfTimer::!PerfTimer()
{
  delete m_pTimer;
} // ~Timer()


/**
 * Reset the timer to zero and activate at the same time
 */
void PerfTimer::Reset()
{
  m_pTimer->Reset();
} // Reset()


/**
 * Stop the timer (and freeze the elapsed time)
 */
void PerfTimer::Stop()
{
  m_pTimer->Stop();
} // Stop()


/**
 * Continiue the timer from where Stop() had frozen it
 */
void PerfTimer::Continue()
{
  m_pTimer->Continue();
} // Continue()


/**
 * Gets the elapsed time 
 *
 * @return elapsed time in seconds
 */
float PerfTimer::ElapsedTime::get()
{
  return m_pTimer->ElapsedTime();
} // get_ElaspsedTime()

