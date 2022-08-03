// $Id: vxProgressBar.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)


// includes
#include "stdafx.h"
#include "vxProgressBar.h"


// static member initialization
VX_BASE_DLL vxProgressBar::SetProgressPercentageFuncPtr vxProgressBar::m_pSetProgressPercentageFunc( NULL );
VX_BASE_DLL vxProgressBar::SetProgressMessageFuncPtr vxProgressBar::m_pSetProgressMessageFunc( NULL );


/**
 * Update the progress bar to the given progress percent and string
 *
 * @param fProgress the percent progress in the range [0,100]
 * @param sProgressMessage message text
 */
void vxProgressBar::SetProgress( float fProgress, const std::string & sProgressMessage )
{
  SetProgressPercentage(fProgress);
  SetProgressMessage(sProgressMessage);
} // SetProgress()


/**
 * Update the progress bar to the given progress percent
 *
 * @param fProgress the percent progress in the range [0,100]
 */
void vxProgressBar::SetProgressPercentage( float fProgress )
{

  if ( m_pSetProgressPercentageFunc != NULL )
    ( * m_pSetProgressPercentageFunc ) ( fProgress );

} // SetProgressPercentage()


/**
 * Update the progress message
 *
 * @param sProgressMessage message text
 */
void vxProgressBar::SetProgressMessage( const std::string & sProgressMessage )
{

  if ( m_pSetProgressMessageFunc != NULL )
    ( * m_pSetProgressMessageFunc ) ( sProgressMessage );

} // SetProgressMessage()


/** 
 * Initialize
 *
 * @param function pointer to progress bar function
 * @param function pointer to progress messag function
 */
void vxProgressBar::Initialize( SetProgressPercentageFuncPtr pSetProgressPercentageFunction,
                                SetProgressMessageFuncPtr pSetProgressMessageFunction )
{

  m_pSetProgressPercentageFunc = pSetProgressPercentageFunction;
  m_pSetProgressMessageFunc    = pSetProgressMessageFunction;

} // Initialize()


/**
 * Constructor
 *
 * @param fThreshold the threshold above which a change should be shown on the GUI
 */
ProgressBarUpdater::ProgressBarUpdater( const float4 fThreshold )
: m_fThreshold( fThreshold ),
m_fLastPercent( -1000.0F )
{
} // ProgressBarUpdater()


/**
 * Update the GUI given a new percent
 *
 * @param fPercent the new percent [0.0F,100.0F]
 */
void ProgressBarUpdater::Update( const float4 fPercent )
{

  // see if the percent has changed enough from last time to justify updating the GUI
  const float4 fDifferenceFromLast( static_cast< float4 >( fabs( fPercent - m_fLastPercent ) ));
  if ( fPercent >= 100.0F || fDifferenceFromLast > m_fThreshold )
  {

    m_fLastPercent = fPercent;
    vxProgressBar::SetProgressPercentage( fPercent );

  } // if changed significantly

} // Update()


// $Log: vxProgressBar.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.9  2003/05/20 15:21:57  frank
// Added progress bar updater class so as to update the GUI conservatively and not to overload it with updates which slow down the actual process.
//
// Revision 1.8  2003/05/13 14:43:20  frank
// code review
//
// Revision 1.7  2003/05/12 16:31:08  michael
// code review
//
// Revision 1.6  2003/04/14 16:37:50  frank
// Separated progress percent and progress message interface
//
// Revision 1.5  2003/04/09 20:17:17  geconomos
// Updated progress bar to have status text.
//
// Revision 1.4  2003/04/09 19:50:10  michael
// added comment correction ')'
//
// Revision 1.3  2003/04/09 17:56:20  michael
// put code into source file (more than a simple assignment/return)
//
// Revision 1.2  2003/02/13 17:27:03  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.1.2.1  2003/02/12 22:51:21  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.1  2003/02/11 23:41:46  frank
// Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/vxBase/src/vxProgressBar.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: vxProgressBar.C,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
