// $Id: vxModifiedQueue.C,v 1.14 2007/01/31 15:07:11 romy Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Dmitry Gritsayenko (mailto:dmitry@viatronix.com)


// includes
#include "stdafx.h"
#include "vxModifiedQueue.h"
#include "vxModifiedSet.h"
#include "vxObserver.h"
#include <algorithm>
#include "utilities.h"
#include "Timer.h"
#include "vxVisualizationPreferences.h"
#include "vxVisualizationGraph.h"

// namespace declarations
using namespace std;


// defines
#define FILE_REVISION "$Revision: 1.14 $"


// static member initialization
std::vector<vxModifiedSet *> vxModifiedQueue::m_messages;
std::vector<vxModifiedSet *> vxModifiedQueue::m_buffer;
vxVisualizationGraph * vxModifiedQueue::m_pVisualizationGraph = NULL;

bool vxModifiedQueue::m_bCreatingMovies( false );

/**
 * Sends all of the modified messages to the sink clients
 */
void vxModifiedQueue::Flush() 
//*******************************************************************
{
  try
  {
    if ( m_bCreatingMovies )
      GetVisualizationGraph().InternalFlush();
    else
      GetVisualizationGraph().Run();
  }
  catch( ... )
  {
    LogDbg( "+++ UNHANDLED EXCEPTION CAUGHT +++", "vxModifiedQueue", "Flush" );
  }
} // Flush()


/**
 * Clears all of the modified messages without forewarding to the sink clients
 */
void vxModifiedQueue::Clear()
//*******************************************************************
{
  if( GetVisualizationGraph().IsFlushing() == false)
  { // release the ModifiedSet objects
		const int4 iN = m_messages.size();

		if( iN > 0 )
		{
			for( int4 i=0; i<iN; i++ )
			{
				if( m_messages[i] != nullptr )
					delete m_messages[i];
			} // for i

			m_messages.clear();
		} // if
  }
} // Clear()


/**
 * Appends the modified message to the others associated with this sink client
 * @param pClient  pointer to a client
 * @param info     modified message
 */
void vxModifiedQueue::Push(vxObserver * pClient, const vxModifiedInfo & info) 
//*******************************************************************
{ 
	int4 iIndex = -1;
	bool bMessage( true );
	register int4 i;

  if( GetVisualizationGraph().IsFlushing() == true )
  {
		for( i=0; i<m_buffer.size(); ++i )
    {
      if( m_buffer[i]->GetClient() == pClient )
      {
        iIndex = i;
				break;
      }
    } // for i

		bMessage = false;
  }
  else
  {
    for( i=0; i<m_messages.size(); ++i )
    {
      if( m_messages[i]->GetClient() == pClient )
      {
        iIndex = i;
				break;
      }
    } // for i
  }
	
	if( bMessage )
  {
		if( iIndex == -1)
		{ // create a new client record
			m_messages.push_back(new vxModifiedSet(pClient, info));
		} // if
		else
		{ // append the existing client record
			m_messages[iIndex]->AddMessage( info );
		} // else
  }  // if
  else
  {
    if( iIndex == -1 )
    { // create a new client record
      m_buffer.push_back(new vxModifiedSet(pClient, info));
    }
    else
    { // append the existing client record
			m_buffer[iIndex]->AddMessage(info);
    } // else
  } // else
} // Push()


/**
 * Removes all stored messages for the disconnected client
 * @param pClient   pointer to a client
 */
void vxModifiedQueue::Disconnected(vxObserver * pClient)
//*******************************************************************
{
	int4 iIndex = -1;
	bool bMessage( true );
	register int4 i;

  // search for an existing client
  if( GetVisualizationGraph().IsFlushing() == true )
  {
		for( i=0; i<m_buffer.size(); ++i )
    {
      if( m_buffer[i]->GetClient() == pClient )
      {
        iIndex = i;
				break;
      }
    } // for i

		bMessage = false;
  }
  else
  {
    for( i=0; i<m_messages.size(); ++i )
    {
      if( m_messages[i]->GetClient() == pClient )
      {
        iIndex = i;
				break;
      }
    } // for i
  }
  
  // delete messages
  if( bMessage && iIndex >= 0 )
  {
    delete m_messages[iIndex];
		m_messages.erase( m_messages.begin() + iIndex );
  }
} // Disconnected()


// Revision History:
// $Log: vxModifiedQueue.C,v $
// Revision 1.14  2007/01/31 15:07:11  romy
// Added a flag that will perform a flush when creating movies
//
// Revision 1.13  2007/01/29 20:37:47  romy
// Added a flush flag to skip the GetVisualizationGraph Run Process. This fixes the VAS movie creation issue
//
// Revision 1.12  2007/01/19 15:28:49  geconomos
// reorganized visualization preferences
//
// Revision 1.11  2007/01/08 17:03:40  gdavidson
// Refactored vxVisualizationGraph code
//
// Revision 1.10  2006/10/20 20:48:11  geconomos
// added support for global controllers
//
// Revision 1.9  2006/10/05 17:03:35  geconomos
// add try/catch block in Flush
//
// Revision 1.8  2006/09/20 14:29:15  geconomos
// corrected check to see whether or not viewer needs to be redrawn
//
// Revision 1.7  2006/09/14 20:14:35  geconomos
// added concept of local controller support
//
// Revision 1.6  2006/09/01 13:41:09  geconomos
// added check to ensure that there is at least one viewer before processing
//
// Revision 1.5  2006/08/31 14:13:42  geconomos
// hacked in vxGlobalControllerDatasets
//
// Revision 1.4  2006/08/23 18:01:18  geconomos
// cleanup
//
// Revision 1.3  2006/08/23 18:00:03  geconomos
// moved original Flush code to InternalFlush
//
// Revision 1.2  2006/08/03 14:17:36  geconomos
// wired in the rendering manager
//
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2003/12/16 14:07:23  michael
// took out the logging message when flusihing the pipeline. Should not have
// been checked in in the first place.
//
// Revision 1.5  2003/12/16 13:25:07  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.4  2003/09/25 12:54:43  michael
// added more comments
//
// Revision 1.3  2003/05/07 21:00:47  dongqing
// code walkthrough
//
// Revision 1.2  2003/05/06 15:11:39  michael
// initial code review
//
// Revision 1.1  2002/07/30 20:21:04  dmitry
// Moved from 74_vxUtils.
//
// Revision 1.8  2002/07/25 17:43:08  dmitry
// Changed vxModifiedInfo to be passed as a reference not a pointer.
//
// Revision 1.7  2002/07/15 15:21:30  geconomos
// Removed virtually inherited classes.
//
// Revision 1.6  2002/06/04 16:13:00  dmitry
// Coding Standard
//
// Revision 1.5  2002/05/30 18:30:53  dmitry
// Fixed sort on buffering.
//
// Revision 1.4  2002/05/10 21:14:36  michael
// replaced typedef
//
// Revision 1.3  2002/05/08 18:23:02  dmitry
// try catch placed around client modified
//
// Revision 1.2  2002/05/01 13:49:01  michael
// Started code review related stuff ...
//
// Revision 1.1  2002/04/17 19:23:03  dmitry
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxModifiedQueue.C,v 1.14 2007/01/31 15:07:11 romy Exp $
// $Id: vxModifiedQueue.C,v 1.14 2007/01/31 15:07:11 romy Exp $