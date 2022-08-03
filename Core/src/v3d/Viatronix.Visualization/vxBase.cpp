// $Id: vxBase.cpp,v 1.6 2006/10/04 18:29:07 gdavidson Exp $
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
#include <windows.h>
#include "resource.h"
#include "vxMouseCursor.h"
#include "ResourceImage.h"
#include "vxBase.h"
#include "vxMessageBox.h"
#include "vxProgressBar.h"
#include "v3D_Events.h"
#include "vxISerializeFactoryManager.h"
#include "vxISerializeFactoryXP.h"
#include "vxDOM.h"
#include "vxUndoActionStack.h"


// defines
#ifdef MessageBox
#undef MessageBox
#endif


/// static function declarations
void ShowError( const std::string & sMsg ){ Viatronix::UI::MessageBox::ShowError( gcnew System::String( sMsg.c_str() )); }
void ShowInfo( const std::string & sMsg ){ Viatronix::UI::MessageBox::ShowInfo( gcnew System::String( sMsg.c_str() )); }
bool ShowQuestion( const std::string & sMsg ){ return Viatronix::UI::MessageBox::ShowQuestion( gcnew System::String( sMsg.c_str() )); }
void ShowModelessInfo( const std::string & sMsg, const uint4 uSeconds ){ Viatronix::UI::MessageBox::ShowModelessInformation( gcnew System::String( sMsg.c_str() ), uSeconds ); }
void UpdateProgressPercentage( float fProgress ) { Viatronix::Visualization::Events::RaiseProgressPercentage( fProgress ); }
void UpdateProgressMessage( const std::string & sProgressMessage ) { Viatronix::Visualization::Events::RaiseProgressMessage( gcnew System::String( sProgressMessage.c_str() )); }

void UndoStackModified() { Viatronix::Visualization::Events::RaiseUndoActionSignal(); }

#pragma unmanaged

/** 
 * DllMain
 * @param module handle
 * @param calling reason
 * @param unused
 * @return true/false
 */
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
  if ( ul_reason_for_call == DLL_PROCESS_ATTACH )
  {
    ResourceImage::SetModuleHandle( reinterpret_cast< HMODULE >( hModule ) );
    vxMouseCursor::SetModuleHandle( reinterpret_cast< HMODULE >( hModule ) );

    vxMessageBox::m_infoFuncPtr = &ShowInfo;
    vxMessageBox::m_errorFuncPtr = &ShowError;
    vxMessageBox::m_questionFuncPtr = &ShowQuestion;
    vxMessageBox::m_modelessInfoFuncPtr = &ShowModelessInfo;

    vxProgressBar::m_pSetProgressPercentageFunc = & UpdateProgressPercentage;
    vxProgressBar::m_pSetProgressMessageFunc = & UpdateProgressMessage;

    vxUndoActionStack::SetModifiedEventFunctionPtr( & UndoStackModified );

    vxEventTimer::Initialize();

    vxDOMDocument::Initialize();

    // create the factory for XP
    vxISerializeFactoryManager::AddFactory(new vxISerializeFactoryXP());
  }

  else if(  ul_reason_for_call == DLL_PROCESS_DETACH )
  {
     vxDOMDocument::Terminate();
  }
  return TRUE;
} // DllMain()


// $Log: vxBase.cpp,v $
// Revision 1.6  2006/10/04 18:29:07  gdavidson
// Ported to C++/CLI
//
// Revision 1.5  2006/09/26 15:17:38  romy
// modified the Initialize function name
//
// Revision 1.4  2006/04/25 20:23:20  romy
// StackModifiedFuncPtr added
//
// Revision 1.3  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.2  2005/04/12 15:23:59  geconomos
// changed xml library to the vxDOM classes
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.13  2003/12/15 20:10:13  frank
// Working on showing a modeless information dialog box
//
// Revision 1.12  2003/05/16 13:14:29  frank
// formatting
//
// Revision 1.11  2003/05/13 17:10:52  geconomos
// Coding conventions.
//
// Revision 1.10  2003/05/12 16:31:48  michael
// code review
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/vxBase.cpp,v 1.6 2006/10/04 18:29:07 gdavidson Exp $
// $Id: vxBase.cpp,v 1.6 2006/10/04 18:29:07 gdavidson Exp $
