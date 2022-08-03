// $Id: FunctionProgress.C,v 1.1 2005/08/05 12:59:44 geconomos Exp $
//
// Copyright© 2000-2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner(s): Jeffrey Meade  jmeade@viatronix.net

// includes
#include "stdafx.h"
#include "FunctionProgress.h"

// static members

/**
 * Class instance reference counter
 */
uint8 FunctionProgress::m_uRefCount(0);

/**
 * Message to display text during function progress: WPARAM(function string ID), LPARAM(step string ID)
 */
const uint4 FunctionProgress::WM_REG_DISPLAY_FUNCTION_TEXT(RegisterWindowMessage(TEXT("Viatronix.v3D.Message.WM_APP_DISPLAY_FUNCTION_TEXT")));

/**
 * Message to display percentage of function completion: WPARAM(0-100)
 */
const uint4 FunctionProgress::WM_REG_SET_FUNCTION_PROGRESS(RegisterWindowMessage(TEXT("Viatronix.v3D.Message.WM_APP_SET_FUNCTION_PROGRESS")));


/**
 * Default constructor
 */
FunctionProgress::FunctionProgress()
{
  m_hMainWnd = AfxGetMainWnd()->GetSafeHwnd();
  m_uRefCount++;
}


/**
 * Constructor 
 *
 * @param hAppWnd   Main application window.
 */
FunctionProgress::FunctionProgress(HWND hAppWnd) : m_hMainWnd(hAppWnd)
{
  m_uRefCount++;
}


/**
 * Destructor
 */
FunctionProgress::~FunctionProgress()
{
  if (--m_uRefCount == 0)
  {
    Text(0,0); Progress(0);
  }
}


/**
 * Sends message to display function/step
 *
 * @param iFunctionStrID   Display string ID for the function.
 * @param iStepStrID       Display string ID of the step within the function.
 */
void FunctionProgress::Text(const uint4 iFunctionStrID, const uint4 iStepStrID)
{
  ::PostMessage(m_hMainWnd, WM_REG_DISPLAY_FUNCTION_TEXT, iFunctionStrID, iStepStrID);
}


/**
 * Sends message to display function
 *
 * @param iFunctionStrID   Display string ID for the function.
 */
void FunctionProgress::Function(const uint4 iFunctionStrID)
{
  ::PostMessage(m_hMainWnd, WM_REG_DISPLAY_FUNCTION_TEXT, iFunctionStrID, 0);
}


/**
 * Sends message to display step
 *
 * @param iStepStrID   Display string ID of the step within the function.
 */
void FunctionProgress::Step(const uint4 iStepStrID)
{
  ::PostMessage(m_hMainWnd, WM_REG_DISPLAY_FUNCTION_TEXT, 0, iStepStrID);
}


/**
 * Sends message to display progress
 *
 * @param iProgress   Percentage progress.
 */
void FunctionProgress::Progress(const uint4 iProgress)
{
  ::PostMessage(m_hMainWnd, WM_REG_SET_FUNCTION_PROGRESS, iProgress, 0);
}


// $Log: FunctionProgress.C,v $
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.1.2.1  2003/07/07 20:30:38  jmeade
// Code standards.
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/FunctionProgress.C,v 1.1 2005/08/05 12:59:44 geconomos Exp $
// $Id: FunctionProgress.C,v 1.1 2005/08/05 12:59:44 geconomos Exp $
