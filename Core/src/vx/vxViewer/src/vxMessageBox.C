// $Id: vxMessageBox.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
//
// Copyright © 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: george econmos (george@viatronix.com)


// includes
#include "stdafx.h"
#include "vxMessageBox.h"


// initialization of static members
vxMessageBox::ShowInfoFuncPtr vxMessageBox::m_infoFuncPtr = NULL;
vxMessageBox::ShowErrorFuncPtr vxMessageBox::m_errorFuncPtr = NULL;
vxMessageBox::ShowQuestionFuncPtr vxMessageBox::m_questionFuncPtr = NULL;
vxMessageBox::ShowModelessInfoFuncPtr vxMessageBox::m_modelessInfoFuncPtr = NULL;


/**
 * Displays an information message box.
 *
 * @param sMsg Message text to display.
 */
void vxMessageBox::ShowInfo( const std::string & sMessage )
{
  (*m_infoFuncPtr)(sMessage);
} // ShowInfo()


/**
 * Displays an error message box.
 *
 * @param sMsg Message text to display.
 */
void vxMessageBox::ShowError( const std::string & sMessage )
{
  (*m_errorFuncPtr)(sMessage);
} // ShowError()


/**
 * Displays a question message box.
 *
 * @param sMsg Message text to display.
 *
 * @return true if user selected Yes, false otherwise
 */
bool vxMessageBox::ShowQuestion( const std::string & sMessage )
{
  return (*m_questionFuncPtr)(sMessage);
} // ShowQuestion()


/**
 * Displays a modeless information message box.
 *
 * @param sMsg Message text to display.
 */
void vxMessageBox::ShowModelessInformation( const std::string & sMessage, const uint4 uSeconds )
{
  ( * m_modelessInfoFuncPtr ) ( sMessage, uSeconds );
} // ShowModelessInformation()


// $Log: vxMessageBox.C,v $
// Revision 1.1.1.1  2004/03/02 03:58:42  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2003/12/15 16:11:31  frank
// Working on showing a modeless information dialog box
//
// Revision 1.5  2003/05/12 16:31:08  michael
// code review
//
// Revision 1.4  2003/04/09 19:50:10  michael
// added comment correction ')'
//
// Revision 1.3  2003/04/09 18:04:07  michael
// put code into source file (more than a simple assignment/return)
//
// Revision 1.2  2003/04/08 15:21:03  michael
// added log information
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/src/vxMessageBox.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
// $Id: vxMessageBox.C,v 1.1.1.1 2004/03/02 03:58:42 geconomos Exp $
