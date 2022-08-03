#include "stdafx.h"
#include "rdgen.h"
#include "MainDialog.h"
#include "Crypt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


MainDialog::MainDialog(CWnd* pParent) : CDialog(MainDialog::IDD, pParent)
{
}


BEGIN_MESSAGE_MAP(MainDialog, CDialog)
  ON_BN_CLICKED(IDOK, &MainDialog::OnClickedGenerate)
  ON_BN_CLICKED(IDC_OUTPUT_FILE, &MainDialog::OnShowFileOpen)
END_MESSAGE_MAP()


BOOL MainDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

  SetDlgItemText( IDC_EDIT_DISCLAIMER_TEXT, "For * clinical trial ONLY" );
  SetDlgItemText( IDC_EDIT_OUTPUT_FILE, "c:\\rdt.reg" );
	return TRUE;
}

void MainDialog::OnClickedGenerate()
{
  CString sDisclaimerText;
  GetDlgItemText(  IDC_EDIT_DISCLAIMER_TEXT, sDisclaimerText );
  if( sDisclaimerText.GetLength() < 1 )
    AfxMessageBox( "Disclaimer text is required" );


  CString sOutputFile;
  GetDlgItemText(  IDC_EDIT_OUTPUT_FILE, sOutputFile );
  if( sOutputFile.GetLength() < 1 )
    AfxMessageBox( "Output file is required" );

  try
  {
    GenerateRegistryFile( sOutputFile, sDisclaimerText,GetDlgItem( IDC_CHECK_64BIT )->SendMessage( BM_GETCHECK, 0, 0 ) > 0 );

    AfxMessageBox( "File created" );
  }
  catch( ... )
  {
    AfxMessageBox( "Foobar" );
  }
}


void MainDialog::GenerateRegistryFile( const CString & sFilename, const CString & sDisclaimerText, bool b64bit )
{
  Crypt crypt;
  std::string sEncryptedDisclaimer = static_cast< LPCTSTR >( sDisclaimerText );
  crypt.EncryptReadable( sEncryptedDisclaimer );

  std::fstream file( static_cast< LPCTSTR >( sFilename ), std::ios::out );

  file << "Windows Registry Editor Version 5.00\n\n";
  if( b64bit )
  {
    file << "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Viatronix\\Viatronix v3D System]\n";
  }
  else
  {
    file << "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Viatronix\\Viatronix v3D System]\n";
  }

  file << "\"RDT\"=\""<<  sEncryptedDisclaimer << "\"\n";

  file.close();
}



void MainDialog::OnShowFileOpen()
{
  CFileDialog dlg( TRUE );
  dlg.m_ofn.Flags |= OFN_EXPLORER;
  if( dlg.DoModal() == IDOK )
  {
    SetDlgItemText( IDC_EDIT_OUTPUT_FILE, dlg.m_pOFN->lpstrFile );
  }
  
}
