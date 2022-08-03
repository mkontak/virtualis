// $Id: vxProgressBar.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: Frank Dachille (frank@viatronix.com)

#ifndef vxProgressBar_h
#define vxProgressBar_h

/**
 * Class to handle the progress bar.  The upper level will pass in functions
 * to manipulate the GUI and then these static functions can be called
 * throughout vxBase.
 */
class VX_BASE_DLL vxProgressBar
{
// typedefs
private:

  /// function pointer for progress bar
  typedef void ( * SetProgressPercentageFuncPtr )( float fProgress );

  /// function pointer for progress message
  typedef void ( * SetProgressMessageFuncPtr ) ( const std::string & sProgressMessage );

// member functions
public:

  /// initialize
  static void Initialize( SetProgressPercentageFuncPtr pSetProgressPercentageFunction, SetProgressMessageFuncPtr pSetProgressMessageFunction );

  /// update the progress bar to the given progress percent and message
  static void SetProgress( float fProgress, const std::string & sProgressMessage );

  /// update the progress bar to the given progress percent
  static void SetProgressPercentage( float fProgress );

  /// update the progress bar message
  static void SetProgressMessage( const std::string & sProgressMessage = "" );

// data
public:

  /// progress value function pointer
  static SetProgressPercentageFuncPtr m_pSetProgressPercentageFunc;

  /// progress message function pointer
  static SetProgressMessageFuncPtr m_pSetProgressMessageFunc;

}; // class vxProgressBar


/**
 * Class to help updating the progress bar only a few times so as not
 * to bog down the system with GUI updates.
 */
class VX_BASE_DLL ProgressBarUpdater
{
// Member Functions
public:

  /// Constructor
  ProgressBarUpdater( const float4 fThreshold = 10.0F );

  /// Update the GUI given a new percent [0.0F,100.0F]
  void Update( const float4 fPercent );

// Data Members
private:

  /// the threshold when to update the GUI
  float4 m_fThreshold;

  /// the last percentage drawn
  float4 m_fLastPercent;

}; // class ProgressBarUpdater


#endif // vxProgressBar_h


// $Log: vxProgressBar.h,v $
// Revision 1.1.1.1  2004/03/02 03:58:40  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.10  2003/05/20 17:09:04  frank
// exported ProgressBarUpdater
//
// Revision 1.9  2003/05/20 15:21:58  frank
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
// Revision 1.3  2003/04/09 17:56:19  michael
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
// $Header: /CVS/cvsRepository/V3D/src/vxBase/include/vxProgressBar.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
// $Id: vxProgressBar.h,v 1.1.1.1 2004/03/02 03:58:40 geconomos Exp $
