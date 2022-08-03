// $Id: FunctionProgress.h,v 1.1 2005/08/05 12:59:44 geconomos Exp $
//
// Copyright© 2000-2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner(s): Jeffrey Meade  jmeade@viatronix.net


#if !defined(AFX_FUNCTIONPROGRESS_H__365E2860_7EC3_4738_A9DF_6BD5B61629D0__INCLUDED_)
#define AFX_FUNCTIONPROGRESS_H__365E2860_7EC3_4738_A9DF_6BD5B61629D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "typedef.h"

/**
 * Helper class for function text and progress (messages
 *  WM_APP_DISPLAY_FUNCTION_TEXT and WM_APP_SET_FUNCTION_PROGRESS)
 */
class FunctionProgress
{
// Messages
public:
  /// Message to display text during function progress: WPARAM(function string ID), LPARAM(step string ID)
  static const uint4 WM_REG_DISPLAY_FUNCTION_TEXT;
  /// Message to display percentage of function completion: WPARAM(0-100)
  static const uint4 WM_REG_SET_FUNCTION_PROGRESS;

public:
  /// Default constructor, sends messages to main application window
  FunctionProgress();
  /// Constructor, sends messages to specified application window
  FunctionProgress(HWND hAppWnd);
  /// Destructor
  virtual ~FunctionProgress();

  /// Sends message to display function/step
  void Text(const uint4 iFunctionStrID, const uint4 iStepStrID);

  /// Sends message to display function
  void Function(const uint4 iFunctionStrID);

  /// Sends message to display step
  void Step(const uint4 iStepStrID);

  /// Sends message to display progress
  void Progress(const uint4 iProgress);

private:
  /// Window that will receive progress messages
  HWND m_hMainWnd;

  /// Class instance reference counter
  static uint8 m_uRefCount;
}; // end class FunctionProgress

#endif // !defined(AFX_FUNCTIONPROGRESS_H__365E2860_7EC3_4738_A9DF_6BD5B61629D0__INCLUDED_)

// $Log: FunctionProgress.h,v $
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.1.2.1  2003/07/07 20:30:38  jmeade
// Code standards.
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/FunctionProgress.h,v 1.1 2005/08/05 12:59:44 geconomos Exp $
// $Id: FunctionProgress.h,v 1.1 2005/08/05 12:59:44 geconomos Exp $
