// $Id: vxSyncData.C,v 1.1 2008/05/02 04:57:19 cqian Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Cheng Qian (mailto:qian@viatronix.com)


// includes
#include "stdafx.h"
#include "vxSyncData.h"
#include "vxEnvironment.h"


// defines
#define FILE_REVISION "$Revision: 1.1 $"


bool vxSyncData::m_bEnabled( false );

/**
 * Constructor
 */ 
vxSyncData::vxSyncData()
{
  m_bEnabled = false;
} // constructor


/**
 * Destructor
 */ 
vxSyncData::~vxSyncData()
//******************************************************************* 
{
} // destructor


/** 
 * Add a Environment to the synchronization 
 *
 * @param: pEnvironment : the new Environment to be added 
 */
void vxSyncData::AddEnvironment ( vxEnvironment * pEnvironment ) 
{
  for( int4 i = 0; i< m_environments.size(); ++i ) 
  {
    if( pEnvironment == m_environments[i] ) 
      return;
  }
  m_environments.push_back( pEnvironment ); 
}// addEnvironment 



/** 
 * Remove a Environment from the synchronization 
 *
 * @param: pEnvironment : the new Environment to be added 
 */
void vxSyncData::RemoveEnvironment( vxEnvironment * pEnvironment )
{
  for( int4 i = 0; i< m_environments.size(); ++i ) 
  {
    if( pEnvironment == m_environments[i] ) 
    {
       for( int4 j = i; j < m_environments.size()-1; ++j ) 
         m_environments[j] = m_environments[j+1];

       m_environments.pop_back(); 
       break;
    } 
  }
}// RemoveEnvironment 



// Revision History:
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxSyncData.C,v 1.1 2008/05/02 04:57:19 cqian Exp $
// $Id: vxSyncData.C,v 1.1 2008/05/02 04:57:19 cqian Exp $
