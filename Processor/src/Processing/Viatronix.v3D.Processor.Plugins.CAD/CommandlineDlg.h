// $Id: CommandlineDlg.h,v 1.2 2006/12/18 21:54:03 romy Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Romy Sreedharan( mailto:romy@viatronix.com )


#pragma once

#include "resource.h"       // main symbols

#include <atlhost.h>
#include <atlconv.h>
#include "stdafx.h"

#include "ColonEnvironmentArgs.h"


// CommandlineArgs

class CommandlineDlg : public CAxDialogImpl<CommandlineDlg>
{

public:
  //Constructor
	CommandlineDlg(){}

  //Destrcutor
  ~CommandlineDlg(){}

  ///Returns the Colon Arguments
  const ColonEnvironmentArgs& GetColonEventArgs() const { return m_args; }
	
public:
  ///OK Button click handler
 	LRESULT OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

  ///OnInitDialog method
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

  //Handles the Cancel click handler
	LRESULT OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

  ///Check button click handler
  LRESULT OnBnClickedCheck1(WORD , WORD , HWND , BOOL& );

  //Source browse button click handler
  LRESULT OnSourceBrowseClick(WORD , WORD , HWND , BOOL& );

  ///DICOM Browse button click handler
  LRESULT OnDicomBrowseButtonClick(WORD , WORD , HWND , BOOL& );

  ///Destination Browse button click handler
  LRESULT OnDestinationBrowseButton(WORD , WORD , HWND , BOOL& );

  ///Volume FIle Browse Button Handler
  LRESULT OnVolBrowseButtonClick(WORD , WORD , HWND , BOOL& );


enum { IDD = IDD_COMMANDLINEARGS };
BEGIN_MSG_MAP(CommandlineArgs)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	COMMAND_HANDLER(IDOK, BN_CLICKED, OnClickedOK)
	COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnClickedCancel)
 // COMMAND_HANDLER(IDC_CHECK1, BN_CLICKED, OnBnClickedCheck1)
  COMMAND_HANDLER(IDC_SRC_BROWSE, BN_CLICKED, OnSourceBrowseClick)
  COMMAND_HANDLER(IDC_DICOM_BROWSE, BN_CLICKED, OnDicomBrowseButtonClick)
  COMMAND_HANDLER(IDC_DEST_BROWSE, BN_CLICKED, OnDestinationBrowseButton)
  COMMAND_HANDLER(IDC_VOL_BROWSE, BN_CLICKED, OnVolBrowseButtonClick)
  CHAIN_MSG_MAP(CAxDialogImpl<CommandlineDlg>)
END_MSG_MAP()


private:
  ///Browse for folders
  std::string BrowseForFolder( const uint4 uId );

  //Browse for file name
  std::string GetFileName( std::string extension= "*.*" );

private:
  ///Colon Event Args
  ColonEnvironmentArgs m_args;
};




// $Log: CommandlineDlg.h,v $
// Revision 1.2  2006/12/18 21:54:03  romy
// modified captions
//
// Revision 1.1  2006/12/18 21:27:47  romy
// commandline dlg Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/CommandlineDlg.h,v 1.2 2006/12/18 21:54:03 romy Exp $
// $Id: CommandlineDlg.h,v 1.2 2006/12/18 21:54:03 romy Exp $

