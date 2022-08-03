// $Id: SelectSeriesDialog.C,v 1.7.2.1 2008/11/13 17:46:17 jmeade Exp $
//
// Copyright© 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: George Economos (george@viatronix.com)

// includes
#include "stdafx.h"
#include "Viatron.h"
#include "SelectSeriesDialog.h"
#include "File.h"
#include "vxDOMDocument.h"
#include "vxDOMAttribute.h"
#include "vxDOMElement.h"
#include "vxDOMNamedNodeMap.h"
#include "vxDOMNodeList.h"
#include "ReaderGlobal.h"

#define FILE_REVISION "$Revision: 1.7.2.1 $"

// namespaces
using namespace ReaderLib;
using namespace ColonViews::Dialogs;

/**
 * Constructor
 */
SelectSeriesDialog::SelectSeriesDialog( CWnd * pParent ) : CDialog(SelectSeriesDialog::IDD, pParent)
{
} // SelectSeriesDialog()


/**
 * Destructor
 */
SelectSeriesDialog::~SelectSeriesDialog()
{
} // ~SelectSeriesDialog()


BEGIN_MESSAGE_MAP(SelectSeriesDialog, CDialog)
  ON_BN_CLICKED(IDC_PRIMARY_VOLUME_SERIES_SELECT, OnClickedPrimaryVolumeSeries)
  ON_BN_CLICKED(IDC_PRIMARY_PREPROCESSED_SERIES_SELECT, OnClickedPrimaryPreprocessedSeries)
  ON_BN_CLICKED(IDC_PRIMARY_CAD_SERIES_SELECT, OnClickedPrimaryCadSeries)
  ON_BN_CLICKED(IDC_SECONDARY_VOLUME_SERIES_SELECT, OnClickedSecondaryVolumeSeries)
  ON_BN_CLICKED(IDC_SECONDARY_PREPROCESSED_SERIES_SELECT, OnClickedSecondaryPreprocessedSeries)
  ON_BN_CLICKED(IDC_SECONDARY_CAD_SERIES_SELECT, OnClickedSecondaryCadSeries)
  ON_BN_CLICKED(IDC_SESSION_SERIES_SELECT, OnClickedSessionSeries)
  ON_BN_CLICKED(IDOK, OnClickedOk)
  ON_COMMAND(ID_SET_ALL, OnSetAll)
  ON_COMMAND(ID_SET_BROWSE_LOCATION, OnSetBrowseLocation)
  ON_COMMAND_RANGE(ID_SET_PRIMARY_VOLUME,ID_SET_SESSION_SERIES, OnContextMenuItem)
  ON_NOTIFY(TCN_SELCHANGE, IDC_SERIES_TABCTL, OnTabChanged)
  ON_NOTIFY(TCN_SELCHANGING, IDC_SERIES_TABCTL, OnTabChanging)
  ON_NOTIFY(TVN_SELCHANGED, IDC_SELSER_TREE, OnFilesSelectionChanged)
  ON_LBN_SELCHANGE(IDC_SELSER_FILELIST, OnItemChangedFileList)
END_MESSAGE_MAP()


/**
 * Called by the framework to exchange and validate dialog data
 *
 * @param   pDX     CDataExchange instance
 */
void SelectSeriesDialog::DoDataExchange( CDataExchange * pDX )
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_SERIES_TABCTL, m_tabs);
  DDX_Control(pDX, IDC_SELSER_FILELIST, m_list);
} // DoDataExchange( CDataExchange * pDX )


/**
 * Called in response to the WM_INITDIALOG message
 *
 * @return TRUE when explicitly setting focus to one of the controls
 */
BOOL SelectSeriesDialog::OnInitDialog()
{
  CDialog::OnInitDialog();

  // initialize the shell tree control
  m_tree.SubclassDlgItem( IDC_SELSER_TREE, this );
  m_tree.SetContextMenuId( IDM_SERIES_SELECT );
  
  // initialize the tab control
  m_tabs.InsertItem( 0, "Series" );
  m_tabs.InsertItem( 1, "My Computer" );

  // read the most recently used settings
  ReadUserSettings();

  // refresh the shell tree
  RefreshTree();
  
  return TRUE;
} // OnInitDialog()


/**
 * Click handler for the select primary volume series button
 */
void SelectSeriesDialog::OnClickedPrimaryVolumeSeries()
{
  BrowseForFolder( IDC_PRIMARY_VOLUME_SERIES );
} // OnClickedPrimaryVolumeSeries()


/**
 * Click handler for the select primary preprocessed series button
 */
void SelectSeriesDialog::OnClickedPrimaryPreprocessedSeries()
{
  BrowseForFolder( IDC_PRIMARY_PREPROCESSED_SERIES );
} // OnClickedPrimaryPreprocessedSeries()


/**
 * Click handler for the select primary cad series button
 */
void SelectSeriesDialog::OnClickedPrimaryCadSeries()
{
  BrowseForFolder( IDC_PRIMARY_CAD_SERIES );
} // OnClickedPrimaryCadSeries()


/**
 * Click handler for the select secondary volume series button
 */
void SelectSeriesDialog::OnClickedSecondaryVolumeSeries()
{
  BrowseForFolder( IDC_SECONDARY_VOLUME_SERIES );
} // OnClickedSecondaryVolumeSeries()


/**
 * Click handler for the select secondary preprocessed series button
 */
void SelectSeriesDialog::OnClickedSecondaryPreprocessedSeries()
{
  BrowseForFolder( IDC_SECONDARY_PREPROCESSED_SERIES );
} // OnClickedSecondaryPreprocessedSeries()


/**
 * Click handler for the select secondary cad series button
 */
void SelectSeriesDialog::OnClickedSecondaryCadSeries()
{
  BrowseForFolder( IDC_SECONDARY_CAD_SERIES );
} // OnClickedSecondaryCadSeries()


/**
 * Click handler for the select session series button
 */
void SelectSeriesDialog::OnClickedSessionSeries()
{
  BrowseForFolder( IDC_SESSION_SERIES );
} // OnClickedSessionSeries()


/**
 * Handler for the set all series context menu item
 */
void SelectSeriesDialog::OnSetAll()
{
  std::string sSelectedPath( m_tree.GetSelectedPath() );
  
  GetDlgItem( IDC_PRIMARY_VOLUME_SERIES )->SetWindowText( sSelectedPath.c_str() );
  GetDlgItem( IDC_PRIMARY_PREPROCESSED_SERIES )->SetWindowText( sSelectedPath.c_str() );
  GetDlgItem( IDC_PRIMARY_CAD_SERIES )->SetWindowText( sSelectedPath.c_str() );
  GetDlgItem( IDC_SECONDARY_VOLUME_SERIES )->SetWindowText( sSelectedPath.c_str() );
  GetDlgItem( IDC_SECONDARY_PREPROCESSED_SERIES )->SetWindowText( sSelectedPath.c_str() );
  GetDlgItem( IDC_SECONDARY_CAD_SERIES )->SetWindowText( sSelectedPath.c_str() );
  GetDlgItem( IDC_SESSION_SERIES )->SetWindowText( sSelectedPath.c_str() );
} // OnSetAll()


/**
 * Handler for the set browse location context menu item
 */
void SelectSeriesDialog::OnSetBrowseLocation()
{
  m_sUserBrowsePath = m_tree.GetSelectedPath();
  if( m_tabs.GetCurSel() != 0 )
    m_tabs.SetCurSel( 0 );

  RefreshTree();
} // OnSetBrowseLocation()


/**
 * Handler for the context menu items
 *
 * @param   uMenuId     context menu id
 */
void SelectSeriesDialog::OnContextMenuItem( UINT uMenuID )
{
  uint4 uDlgItemId( 0 );
  switch( uMenuID )
  {
  case ID_SET_PRIMARY_VOLUME:
    uDlgItemId = IDC_PRIMARY_VOLUME_SERIES;
    break;
  case ID_SET_PRIMARY_PREPROCESSED:
    uDlgItemId = IDC_PRIMARY_PREPROCESSED_SERIES;
    break;
  case ID_SET_PRIMARY_CAD:
    uDlgItemId = IDC_PRIMARY_CAD_SERIES;
    break;
  case ID_SET_SECONDARY_VOLUME:
    uDlgItemId = IDC_SECONDARY_VOLUME_SERIES;
    break;
  case ID_SET_SECONDARY_PREPROCESSED:
    uDlgItemId = IDC_SECONDARY_PREPROCESSED_SERIES;
    break;
  case ID_SET_SECONDARY_CAD:
    uDlgItemId = IDC_SECONDARY_CAD_SERIES;
    break;
  case ID_SET_SESSION_SERIES:
    uDlgItemId = IDC_SESSION_SERIES;
    break;
  default:
    LogWrn( "Unknown menu id", "SelectSeriesDialog", "OnContextMenuItem" );
    break;
  }

  if( uDlgItemId != 0 )
    GetDlgItem( uDlgItemId )->SetWindowText( m_tree.GetSelectedPath().c_str() );
} // OnContextMenuItem( UINT uID )


/**
 * Displays the "BrowseForFolder" dialog and sets the text for the specified window
 *
 * @param   uId   Dialog id of windows to set text
 */
void SelectSeriesDialog::BrowseForFolder( const uint4 uId )
{
  BROWSEINFO bi;
  memset( &bi, 0, sizeof( BROWSEINFO ) );
  bi.hwndOwner = GetSafeHwnd();

  LPITEMIDLIST pidl = SHBrowseForFolder( &bi );
  if( pidl != NULL )
  { 
    char sDirectory[ _MAX_PATH ];
    SHGetPathFromIDList( pidl, sDirectory );
    GetDlgItem( uId )->SetWindowText( sDirectory );
    CoTaskMemFree( pidl );
  }
} // BrowseForFolder( const uint4 uId )


/**
 * Forces the shell tree to refresh
 */
void SelectSeriesDialog::RefreshTree()
{
  USES_CONVERSION;

  LPITEMIDLIST pidl( NULL );
  if( m_tabs.GetCurSel() == 0 )
  {
    DWORD dw( 0 );
    SHILCreateFromPath( A2OLE( m_sUserBrowsePath.c_str() ), &pidl, &dw );
  }

  m_tree.Initialize( pidl, false );
} // RefreshTree()


/**
 * Handler for TCN_SELCHANGE
 *
 * @param   pNMHDR      NMHDR containing information related to this message
 * @param   pResult     resultant code
 */
void SelectSeriesDialog::OnTabChanged( NMHDR * pNMHDR, LRESULT * pResult )
{
  RefreshTree();
  *pResult = 0;
} // OnTabChanged( NMHDR * pNMHDR, LRESULT * pResult )


/**
 * Handler for TCN_SELCHANGING
 *
 * @param   pNMHDR      NMHDR containing information related to this message
 * @param   pResult     resultant code
 */
void SelectSeriesDialog::OnTabChanging( NMHDR *pNMHDR, LRESULT * pResult )
{
  *pResult = 0;
} // OnTabChanging( NMHDR *pNMHDR, LRESULT * pResult )


/**
 * handler for TVN_SELCHANGE
 *
 * @param   pNMHDR      NMHDR containing information related to this message
 * @param   pResult     resultant code
 */
void SelectSeriesDialog::OnFilesSelectionChanged( NMHDR * pNMHDR, LRESULT * pResult )
{
  *pResult = 0;
  std::string sPath = m_tree.GetSelectedPath();

  TCHAR lpszOldPath[MAX_PATH];
  ::GetCurrentDirectory(MAX_PATH, lpszOldPath);

  ::SetCurrentDirectory(sPath.c_str());
  m_list.ResetContent();
  m_list.Dir(DDL_READWRITE|DDL_DIRECTORY, _T("*.*"));

  ::SetCurrentDirectory(lpszOldPath);
  SetDlgItemText(IDC_SELSER_FILEINFO, "");

} // OnFilesSelectionChanged()


/**
 * A new item in the filelist was selected
 */
void SelectSeriesDialog::OnItemChangedFileList( )
{
  CString sFilename;
  m_list.GetText(m_list.GetCurSel(), sFilename);
  
  std::string c(vx::ColonSessionXMLExt);

  if ( c == std::string(".")+vx::File(LPCTSTR(sFilename)).GetExtension() )
  {
    std::string sPath = m_tree.GetSelectedPath() + '/';
    sPath += vx::File(LPCTSTR(sFilename)).GetTitle();

    ColonSession sess;
    sess.Read(sPath, ReaderLib::rdrGlobal.m_supine);

    std::string sFileInfo("Session File Version: ");
    sFileInfo += ToAscii(sess.GetVersion()) + "\n";
    sFileInfo += ToAscii(sess.m_bookmarkList.size()) + " Bookmarks; ";
    sFileInfo += ToAscii(sess.m_2DAnnotationList.size()) + " Annotations; ";
    sFileInfo += ToAscii(sess.m_measureLineList.size()) + " Measurements\n";

    SetDlgItemText(IDC_SELSER_FILEINFO, sFileInfo.c_str());
  }
  else
    SetDlgItemText(IDC_SELSER_FILEINFO, "");
} // OnItemChangedFileList()


/**
 * Click handler for the "OK" button
 */
void SelectSeriesDialog::OnClickedOk()
{
  // store the current value to the user settings
  StoreUserSettings();

  m_params.GetPrimaryDataset().SetVolumeDirectory( GetDlgItemText( IDC_PRIMARY_VOLUME_SERIES ) );
  m_params.GetPrimaryDataset().SetPreprocessedDirectory( GetDlgItemText( IDC_PRIMARY_PREPROCESSED_SERIES) );
  m_params.GetPrimaryDataset().SetCadDirectory( GetDlgItemText( IDC_PRIMARY_CAD_SERIES) );
  m_params.GetPrimaryDataset().SetPrefix(  DatasetLoadParams::GeneratePrefix( m_params.GetPrimaryDataset().GetVolumeDirectory(), "s" ) );
  
  m_params.GetSecondaryDataset().SetVolumeDirectory( GetDlgItemText( IDC_SECONDARY_VOLUME_SERIES ) );
  m_params.GetSecondaryDataset().SetPreprocessedDirectory( GetDlgItemText( IDC_SECONDARY_PREPROCESSED_SERIES ) );
  m_params.GetSecondaryDataset().SetCadDirectory( GetDlgItemText( IDC_SECONDARY_CAD_SERIES ) );
  m_params.GetSecondaryDataset().SetPrefix(  DatasetLoadParams::GeneratePrefix( m_params.GetSecondaryDataset().GetVolumeDirectory(), "p" ) );
  
  m_params.SetSessionDirectory( GetDlgItemText( IDC_SESSION_SERIES ) );

  uint4 uFlags( 0 );
  if( m_params.GetPrimaryDataset().GetVolumeDirectory().length() && m_params.GetPrimaryDataset().GetPreprocessedDirectory().length() )
    uFlags |=  StudyLoadParams::PRIMARY_DATASET_VALID;

  if( m_params.GetSecondaryDataset().GetVolumeDirectory().length() && m_params.GetSecondaryDataset().GetPreprocessedDirectory().length() )
    uFlags |=  StudyLoadParams::SECONDARY_DATASET_VALID;

  m_params.SetFlags( uFlags );

  // base class
  EndDialog( IDOK );
} // OnClickedOk()


/**
 * Helper function for retrieving the text for a control
 *
 * @param   uDlgItemId    Id of control whose text is to be retrieved
 */
std::string SelectSeriesDialog::GetDlgItemText( uint4 uDlgItemId )
{
  std::string sText;

  char sz[ _MAX_PATH ];
  GetDlgItem( uDlgItemId )->GetWindowText( sz, _MAX_PATH );
  sText = sz;

  return sText;
} // GetDlgItemText( uint4 uDlgItemId )


/**
 * Reads the most recetly used settings from the user's configurations file
 */
void SelectSeriesDialog::ReadUserSettings()
{
  int4 TODO_GET_PATH_FROM_USER_OBJECT;
  std::string sFile = "C:\\Documents and Settings\\All Users\\Application Data\\Viatronix\\vxservice\\vc_load_params.xml";

  try
  {
    vx::File file( sFile );
    if( !file.Exists() )
      return;

    vxDOMDocument doc( vxDOMDocument::FromFile( sFile ) );

    // check the version
    std::string sVersion( doc.GetDocumentElement().GetAttributeNode( "version" ).GetNodeValue() );
    if( sVersion != "1.0" )
    {
      LogErr( "invalid version for select series dialog", "SelectSeriesDialog", "ReadUserSettings" );
      return;
    }

    // get the user browse location
    m_sUserBrowsePath = doc.GetDocumentElement().GetAttributeNode( "user_path" ).GetNodeValue();

    GetDlgItem( IDC_SESSION_SERIES )->SetWindowText( doc.GetDocumentElement().GetAttributeNode( "session" ).GetNodeValue().c_str() );

    vxDOMNode primaryNode = doc.GetDocumentElement().GetElementsByTagName( "primary" ).Item( 0 );
    GetDlgItem( IDC_PRIMARY_VOLUME_SERIES )->SetWindowText( primaryNode.GetAttributes().GetNamedItem( "volume" ).GetNodeValue().c_str() );
    GetDlgItem( IDC_PRIMARY_PREPROCESSED_SERIES )->SetWindowText( primaryNode.GetAttributes().GetNamedItem( "preprocessed" ).GetNodeValue().c_str() );
    GetDlgItem( IDC_PRIMARY_CAD_SERIES )->SetWindowText( primaryNode.GetAttributes().GetNamedItem( "cad" ).GetNodeValue().c_str() );

    vxDOMNode secondaryNode = doc.GetDocumentElement().GetElementsByTagName( "secondary" ).Item( 0 );
    GetDlgItem( IDC_SECONDARY_VOLUME_SERIES )->SetWindowText( secondaryNode.GetAttributes().GetNamedItem( "volume" ).GetNodeValue().c_str() );
    GetDlgItem( IDC_SECONDARY_PREPROCESSED_SERIES )->SetWindowText( secondaryNode.GetAttributes().GetNamedItem( "preprocessed" ).GetNodeValue().c_str() );
    GetDlgItem( IDC_SECONDARY_CAD_SERIES )->SetWindowText( secondaryNode.GetAttributes().GetNamedItem( "cad" ).GetNodeValue().c_str() );
  }
  catch( ex::XmlException & e )
  {
    AfxMessageBox( e.GetLogRecord().GetTextMessage().c_str(), MB_ICONERROR, MB_OK );
    LogErr( e.GetLogRecord().GetTextMessage(), "SelectSeriesDialog", "ReadUserSettings" );
  }
} // ReadUserSettings()


/**
 * Stores the most recetly used settings from the user's configurations file
 */
void SelectSeriesDialog::StoreUserSettings()
{
  int4 TODO_GET_PATH_FROM_USER_OBJECT;
  std::string sFilename = "C:\\Documents and Settings\\All Users\\Application Data\\Viatronix\\vxservice\\vc_load_params.xml";

  // create the root element
  vxDOMDocument doc( "loadparam" );
  AddAttribute( doc.GetDocumentElement(), "version", "1.0" );
  AddAttribute( doc.GetDocumentElement(), "session", IDC_SESSION_SERIES );
  AddAttribute( doc.GetDocumentElement(), "user_path", m_sUserBrowsePath );

  vxDOMElement primary =  doc.CreateElement( "primary" );
  AddAttribute( primary, "volume",        IDC_PRIMARY_VOLUME_SERIES );
  AddAttribute( primary, "preprocessed",  IDC_PRIMARY_PREPROCESSED_SERIES );
  AddAttribute( primary, "cad",           IDC_PRIMARY_CAD_SERIES );
  
  vxDOMElement secondary =  doc.CreateElement( "secondary" );
  AddAttribute( secondary, "volume",        IDC_SECONDARY_VOLUME_SERIES );
  AddAttribute( secondary, "preprocessed",  IDC_SECONDARY_PREPROCESSED_SERIES);
  AddAttribute( secondary, "cad",           IDC_SECONDARY_CAD_SERIES );

  doc.GetDocumentElement().AppendChild( primary );
  doc.GetDocumentElement().AppendChild( secondary );

  vx::File(vx::File( sFilename ).GetParent() ).Mkdirs();
  doc.Save( sFilename );
} // StoreUserSettings()


/**
 * Adds an attribute to the specified DOM node
 *
 * @param   node          node to add attribute to
 * @param   sName         attribute name
 * @param   uDlgItemId    id of control whose value is to be the attributes'
 */
void SelectSeriesDialog::AddAttribute( vxDOMNode & node, const std::string & sName, uint4 uDlgItemId )
{
  char sWindowText[ _MAX_PATH ];
  GetDlgItem( uDlgItemId )->GetWindowText( sWindowText, _MAX_PATH );
  AddAttribute( node, sName, sWindowText );
} // AddAttribute( vxDOMNode & node, const std::string & sName, uint4 uDlgItemId )


/**
 * Adds an attribute to the specified DOM node
 *
 * @param   node      node to add attribute to
 * @param   sName     attribute name
 * @param   sValue    attribute value
 */
void SelectSeriesDialog::AddAttribute( vxDOMNode & node, const std::string & sName, const std::string & sValue )
{
  vxDOMAttribute attr =  node.GetOwnerDocument().CreateAttribute( sName );
  attr.SetNodeValue( sValue );
  node.GetAttributes().SetNamedItem( attr );
} // AddAttribute( vxDOMNode & node, const std::string & sName, const std::string & sValue )


// $Log: SelectSeriesDialog.C,v $
// Revision 1.7.2.1  2008/11/13 17:46:17  jmeade
// display information on file selected in list.
//
// Revision 1.7  2007/01/16 09:11:18  frank
// added the user preferences directory if it does not already exist - a real time saver!
//
// Revision 1.6  2006/07/10 20:40:30  jmeade
// adding a directory file listing (to aid in directory selection).
//
// Revision 1.5  2005/11/16 18:19:14  jmeade
// removed unused/unneeded IDs.
//
// Revision 1.4  2005/11/16 14:28:07  geconomos
// added the ability to hint at which volume ( .ecv or .vol ) to base the dataset prefix on
//
// Revision 1.3  2005/11/16 14:12:10  geconomos
// added context menu to set all series to the slected directory in the shell tree
//
// Revision 1.2  2005/11/15 18:52:17  geconomos
// reworked user to be compatible with the new console
//
// Revision 1.1  2005/11/15 16:15:36  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/SelectSeriesDialog.C,v 1.7.2.1 2008/11/13 17:46:17 jmeade Exp $
// $Id: SelectSeriesDialog.C,v 1.7.2.1 2008/11/13 17:46:17 jmeade Exp $
