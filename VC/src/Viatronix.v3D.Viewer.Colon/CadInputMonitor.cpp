// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: CadInputMonitor.cpp
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#include "StdAfx.h"
#include "CADInputMonitor.h"
#include "CadUtils.h"
#include "ReaderGlobal.h"
#include "V1KMessages.h"
#include "WinUtils.h"
#include "ViatronViewWnd.h"


using namespace ReaderLib;


/**
 * constructor
 */
CadInputMonitor::CadInputMonitor( ColonViews::Layouts::ColonViatronixWnd * pLayout, bool bEnabled ) :
  m_pLayout( pLayout ),
  m_bEnabled( bEnabled ),
  m_bActive( rdrGlobal.m_supine.IsCadFindingsLoaded() || rdrGlobal.m_prone.IsCadFindingsLoaded() )
{
} // CadInputMonitor


/**
 * destructor
 */
CadInputMonitor::~CadInputMonitor()
{
} // ~CadInputMonitor()


/**
 * Monitors the keyboard input
 *
 * @param uChar
 * @param uRenCnt
 * @param uFlags
 */
bool CadInputMonitor::Monitor( UINT uChar, UINT uRepCnt, UINT uFlags )
{
  if( m_bEnabled )
  {
    switch( uChar )
    {
    case 'C':
      Utilities::CAD::ToggleVisibility();
      return true;

    case VK_SPACE:
      if( rdrGlobal.m_bDisplayCadFindings )
      {
        if( WinUtils::IsKeyStateDown( VK_SHIFT ))
          Utilities::CAD::PreviousFinding( *rdrGlobal.m_pCurrDataset );
        else
          Utilities::CAD::NextFinding( *rdrGlobal.m_pCurrDataset );
        return true;
      }
      break;
    }
  }

  return false;
} // Monitor( uChar, uRepCnt, uFlags )