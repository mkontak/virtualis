// $Id: CommandlineDlg.C,v 1.1 2007/03/01 20:40:11 geconomos Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: Romy Sreedharan( mailto:romy@viatronix.com )

#include "stdafx.h"
#include "CommandlineDlg.h"
#include <shlobj.h>

#define _MAX_PATH   260 


/*
 * Handles the cancel click handler
 */
LRESULT CommandlineDlg::OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
  EndDialog(wID);
  return 0;
}//OnClickedCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)


/*
 *OnInit Dialog method
 */
LRESULT CommandlineDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
  CAxDialogImpl<CommandlineDlg>::OnInitDialog(uMsg, wParam, lParam, bHandled);
  bHandled = TRUE;
  return 1;  // Let the system set the focus
}//OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)


/*
 *Handles the OK Click handler
 */
LRESULT CommandlineDlg::OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
  /// Get the file name
  char strAppid[1000];
  GetDlgItemText( IDC_APPID_EDIT,  strAppid, 1000 );
  m_args.SetPluginAppId( strAppid );

  char strSourceFilePath[1000];
  GetDlgItemText( IDC_SRC_FILE_PATH_EDIT, strSourceFilePath, 1000 );
  m_args.SetVolumeFilePath( strSourceFilePath );

  char strDICOMFilePath[1000];
  GetDlgItemText( IDC_DICOM_FILE_PATH_EDIT, strDICOMFilePath, 1000 );
  m_args.SetDicomDirectory( strDICOMFilePath );

  char strDestFilePath[1000];
  GetDlgItemText( IDC_DEST_FILE_PATH_EDIT, strDestFilePath, 1000 );
  m_args.SetDestinationFilePath( strDestFilePath );

  char strVolumeFilePath[1000];
  GetDlgItemText( IDC_VOLUME_FILE_PATH_EDIT, strVolumeFilePath, 1000 );
  m_args.SetVolumeFilePath( strVolumeFilePath );

  UINT state = IsDlgButtonChecked( IDC_DICOMONLY_CHECK );
  if ( state == 0 )
    m_args.SetDicomOnly( false );
  else
    m_args.SetDicomOnly( true );

  EndDialog(wID);
  return IDOK;
}//OnClickedOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)



/*
 *UI which Browses for a folder
 */
std::string CommandlineDlg::BrowseForFolder( const uint4 uId )
{
  BROWSEINFO bi;
  memset( &bi, 0, sizeof( BROWSEINFO ) );
  bi.hwndOwner = m_hWnd;

  LPITEMIDLIST pidl = SHBrowseForFolder( &bi );
  if( pidl != NULL )
  { 
    char sDirectory[ _MAX_PATH ];
    SHGetPathFromIDList( pidl, sDirectory );
    // GetDlgItem( uId ).SetWindowText( sDirectory );
    CoTaskMemFree( pidl );

    return sDirectory;
  }
} // BrowseForFolder( const uint4 uId )


/*
 * UI Which browses for a file name
 */
std::string CommandlineDlg::GetFileName( std::string extension )
{
  OPENFILENAME ofn;
  char szFileName[_MAX_PATH] = "";

  ZeroMemory(&ofn, sizeof(ofn));

  ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
  ofn.hwndOwner = m_hWnd;
  std::string temp = "(" + extension + ")";
  ofn.lpstrFilter = temp.c_str();
  ofn.lpstrFile = szFileName;
  ofn.nMaxFile  = _MAX_PATH;
  ofn.Flags     = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
  ofn.lpstrDefExt = extension.c_str();

  GetOpenFileName(&ofn);
  return ofn.lpstrFile;
}//GetFileName()


/*
 * Source Browse button click handler
 */
LRESULT CommandlineDlg::OnSourceBrowseClick(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  std::string slectedFolder =  GetFileName();
  SetDlgItemText( IDC_SRC_FILE_PATH_EDIT, slectedFolder.c_str() );

  return 0;
}//OnSourceBrowseClick


/*
 *Dicom Browse Button Click handler
 */
LRESULT CommandlineDlg::OnDicomBrowseButtonClick(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  std::string slectedFolder =  BrowseForFolder( IDC_DICOM_BROWSE );
  SetDlgItemText( IDC_DICOM_FILE_PATH_EDIT, slectedFolder.c_str() );

  return 0;
}//OnDicomBrowseButtonClick


/*
 *Destination Browse Button click handler
 */
LRESULT CommandlineDlg::OnDestinationBrowseButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  std::string slectedFolder =  BrowseForFolder( IDC_DEST_BROWSE );
  SetDlgItemText( IDC_DEST_FILE_PATH_EDIT, slectedFolder.c_str() );

  return 0;
}//OnDestinationBrowseButton

/*
 *Volume Browse Button Click handler
 */
LRESULT CommandlineDlg::OnVolBrowseButtonClick(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  

  std::string slectedFile =  GetFileName( "All Files (*.*\")"); //"Vol Files (*.vol)\", \"*.VOL\"" ) ;
  std::string file = "";

  //Get the substring by cutting our the .vol
  if ( slectedFile.length() > 4 )
    file = slectedFile.substr( 0, slectedFile.length() - 4 );

  SetDlgItemText( IDC_VOLUME_FILE_PATH_EDIT, file.c_str() );

  return 0;
}//OnVolBrowseButtonClick


// $Log: CommandlineDlg.C,v $
// Revision 1.1  2007/03/01 20:40:11  geconomos
// changed extension from .cpp to .C
//
// Revision 1.3  2006/12/18 22:15:14  romy
// bug fix
//
// Revision 1.2  2006/12/18 21:54:03  romy
// modified captions
//
// Revision 1.1  2006/12/18 21:27:47  romy
// commandline dlg Initial version
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Processor.Plugins.CAD/CommandlineDlg.C,v 1.1 2007/03/01 20:40:11 geconomos Exp $
// $Id: CommandlineDlg.C,v 1.1 2007/03/01 20:40:11 geconomos Exp $
