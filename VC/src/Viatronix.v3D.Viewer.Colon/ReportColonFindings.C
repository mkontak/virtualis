// $Id: ReportColonFindings.C,v 1.20.2.28 2010/10/26 20:05:03 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Description: Work area for colon findings
// Owner: George Economos

#include "stdafx.h"
#include "ReportColonFindings.h"
#include "ReportVariant.h"
#include "ReaderGlobal.h"
#include "V1KMessages.h"
#include "ApplicationSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#undef Normal

#define FILE_REVISION "$Revision: 1.20.2.28 $"
using namespace ReaderLib;
using namespace WinUtils;
using namespace ColonReport;


//defines
#define FILE_REVISION "$Revision: 1.20.2.28 $"


////////////////////////////////////////////////////////////////////////////
// ReportColonFindings

IMPLEMENT_DYNAMIC(ReportColonFindings, CDialog)

/**
 * Constructor
 *
 * @param pParent   Parent window for this window.
 */
ReportColonFindings::ReportColonFindings(CWnd* pParent) : ReportWndBase(ReportColonFindings::IDD, pParent),
  m_szImage(256,256),
  m_pDragImage(NULL),
  m_iDropItem(-1),
  m_bExtraColonic(false),
  m_pWndSubFinding(NULL),
  m_bUserClickedFindingDesc(false)
{
} // constructor( pParent )


/**
 * Dialog data variable initialization.
 *
 * @param pDX   Dialog data exchange instance.
 */
void ReportColonFindings::DoDataExchange(CDataExchange* pDX)
{
  ReportWndBase::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(ReportColonFindings)
  DDX_Control(pDX, IDC_OTHER_COMMENTS, m_wndOtherComments);
  DDX_Control(pDX, IDC_GROUP_SHAPE, m_wndGroupShape);
  DDX_Control(pDX, IDC_GROUP_LOCATION, m_wndGroupLocation);
  DDX_Control(pDX, IDC_GROUP_FEATURES, m_wndGroupFeatures);
  DDX_Control(pDX, IDC_RCF_GROUP_ORGAN, m_wndGroupOrgan);
  DDX_Control(pDX, IDC_RCF_GROUP_FINDINGDESC, m_wndGroupFindingDesc);
  DDX_Control(pDX, IDC_ENTRIES, m_wndEntries);
  DDX_Control(pDX, IDC_IMAGES, m_wndImages);
  DDX_Radio(pDX, IDC_FINDING_NORMAL, m_iFindingType);
  DDX_Control(pDX, IDC_RCF_FLEXURE_CHECK, m_wndcheckFlexure);
  //}}AFX_DATA_MAP
} // DoDataExchange( pDX )


BEGIN_MESSAGE_MAP(ReportColonFindings, ReportWndBase)
  //{{AFX_MSG_MAP(ReportColonFindings)
  ON_NOTIFY(LVN_BEGINDRAG, IDC_ENTRIES, OnBeginDragEntries)
  ON_NOTIFY(LVN_BEGINDRAG, IDC_IMAGES, OnBeginDragImages)
  ON_NOTIFY(LVN_DELETEITEM, IDC_IMAGES, OnDeleteItemImages)
  ON_NOTIFY(LVN_DELETEITEM, IDC_ENTRIES, OnDeleteItemEntries)
  ON_NOTIFY(LVN_ITEMCHANGED, IDC_ENTRIES, OnItemChangedEntries)
  ON_NOTIFY(LVN_ITEMCHANGED, IDC_RCF_GROUP_ORGAN, OnItemChangedOrgan)
  ON_NOTIFY(LVN_ITEMCHANGED, IDC_RCF_GROUP_FINDINGDESC, OnItemChangedFindingDesc)
  ON_WM_LBUTTONUP()
  ON_WM_MOUSEMOVE()
  ON_NOTIFY(LVN_ITEMCHANGED, IDC_GROUP_FINDING, OnItemChangedGroupFinding)
  ON_NOTIFY(NM_RCLICK, IDC_IMAGES, OnRClickImages)
  ON_NOTIFY(NM_RCLICK, IDC_ENTRIES, OnRClickEntries)
  ON_COMMAND(IDM_RPT_JUMPTOFINDINGVIEW, OnRptJumpToFindingView)
  ON_COMMAND(IDM_RPT_DELETE_ENTRY, OnRptDeleteEntry)
  ON_COMMAND(IDM_RPT_DELETE_ALL_FINDINGS, OnRptDeleteAllFindings)
  ON_COMMAND(IDM_RPT_DISTANCE_UNCERTAIN, OnRptDistanceUncertain)
  ON_COMMAND(IDM_RPT_DELETE_IMAGE, OnRptDeleteImage)
  ON_BN_CLICKED(IDC_FINDING_NORMAL, OnFindingNormal)
  ON_BN_CLICKED(IDC_FINDING_POLYP, OnFindingColonicPolyp)
  ON_BN_CLICKED(IDC_FINDING_DIVERTCULUM, OnFindingColonicDivertculum)
  ON_BN_CLICKED(IDC_FINDING_STENOSIS, OnFindingColonicStenosis)
  ON_BN_CLICKED(IDC_FIDNING_OTHER, OnFindingColonicOther)
  ON_BN_CLICKED(IDC_FINDING_EXTRA_COLONIC, OnFindingExtraColonic)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/**
 * WM_INITDIALOG handler - sent to the dialog box during the Create, CreateIndirect, or DoModal calls
 *
 * @return   TRUE upon success
 */
BOOL ReportColonFindings::OnInitDialog()
{
  ReportWndBase::OnInitDialog();

  // set control fonts
  m_wndImages.SetFont(&m_fontSmall);
  GetDlgItem(IDC_ENTRY_NAME)->SetFont(&m_fontName);

  // subclass the spin edit controls
  CreateSpinEdit(IDC_MAXIMUM_SIZE, m_wndMaximumSize, 0.0f, 99.0f, 0.0f, 0.1f, 1);
  CreateSpinEdit(IDC_PERP_SIZE, m_wndPerpendicularSize, 0.0f, 99.0f, 0.0f, 0.1f, 1);

  CreateRadioGroups();

  // store ReportListCtrl's rectangle for drag and drop
  m_wndEntries.GetWindowRect(m_rcEntries);
  ScreenToClient(m_rcEntries);

  // set the background color for the two list controls
  m_wndImages.SetBkColor(m_clrBack);
  m_wndEntries.SetBkColor(m_clrBack);

  m_iFindingType = 0; // Normal
  UpdateData(FALSE);

  return TRUE;
} // OnInitDialog()


/**
 * Empty override so that user cannot dismiss dialog
 */
void ReportColonFindings::OnOK()
{
  // By default, the CDialog::OnOK() base class method will close
  //  the dialog, when, for example, the user hits the enter key
  // So, we override this and do nothing

  return;
} // OnOK()


/**
 * Empty override so that user cannot dismiss dialog
 */
void ReportColonFindings::OnCancel()
{
  // By default, the CDialog::OnCancel() base class method will close
  //  the dialog, when, for example, the user hits the ESC-key
  // So, we override this and do nothing

  return;
} // OnCancel()


/**
 * initializes the RadioGroup custom controls for the form
 */
void ReportColonFindings::CreateRadioGroups()
{
  // Shape radios
  m_wndGroupShape.SetBackgroundImage(GetChildBackground(m_wndGroupShape.GetDlgCtrlID()));
  m_wndGroupShape.SetSelectedBold(TRUE);
  m_wndGroupShape.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_SESSILE),
    static_cast<DWORD>(ReportLib::Sessile));
  m_wndGroupShape.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_PEDUNCULATED),
    static_cast<DWORD>(ReportLib::PedunculatedStalkSeen));
  m_wndGroupShape.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_FLAT),
    static_cast<DWORD>(ReportLib::Flat));
  m_wndGroupShape.SelectItem(static_cast<DWORD>(ReportLib::Sessile));


  // Location
  m_wndGroupLocation.SetBackgroundImage(GetChildBackground(m_wndGroupLocation.GetDlgCtrlID()));
  m_wndGroupLocation.SetSelectedBold(TRUE);
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_CECUM),
    static_cast<DWORD>(ReportLib::Cecum));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_ASCENDING),
    static_cast<DWORD>(ReportLib::Ascending));
  //m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_HEPATIC_FLEXURE),
  //  static_cast<DWORD>(ReportLib::HepaticFlexure));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_TRANSVERSE),
    static_cast<DWORD>(ReportLib::Transverse));
  //m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_SPLENIC_FLEXURE),
  //  static_cast<DWORD>(ReportLib::SplenicFlexure));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_DESCENDING),
    static_cast<DWORD>(ReportLib::Descending));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_SIGMOID),
    static_cast<DWORD>(ReportLib::Sigmoid));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_RECTUM),
    static_cast<DWORD>(ReportLib::Rectum));
  m_wndGroupLocation.SelectItem(static_cast<DWORD>(ReportLib::Rectum));


  // Features radios
  m_wndGroupFeatures.SetBackgroundImage(GetChildBackground(m_wndGroupFeatures.GetDlgCtrlID()));
  m_wndGroupFeatures.SetSelectedBold(TRUE);

  CString sIndeterminate;
  sIndeterminate.Format("%s (%s)", LoadResourceString(IDS_INDETERMINATE), LoadResourceString(IDS_NO_REPORT));
  m_wndGroupFeatures.InsertGroupItem(ReportGroupCtrl::NO_GROUP, sIndeterminate,
    static_cast<DWORD>(ReportLib::Indeterminate));
  // new option: report this indeterminate feature.
  sIndeterminate.Format("%s (%s)", LoadResourceString(IDS_INDETERMINATE), LoadResourceString(IDS_REPORT));
  m_wndGroupFeatures.InsertGroupItem(ReportGroupCtrl::NO_GROUP, sIndeterminate,
    static_cast<DWORD>(ReportLib::Indeterminate + 5));

  m_wndGroupFeatures.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceString(IDS_SIMILAR_TO_COLON_WALL),
    static_cast<DWORD>(ReportLib::SimilarToColonWall));

  CString sDensityChange;
  sDensityChange.Format(IDS_DENSITY_CHANGE_FORMAT, LoadResourceString(IDS_GRADUAL));
  m_wndGroupFeatures.InsertGroupItem(ReportGroupCtrl::NO_GROUP, sDensityChange,
    static_cast<DWORD>(ReportLib::GradualDensityChange));
  sDensityChange.Format(IDS_DENSITY_CHANGE_FORMAT, LoadResourceString(IDS_ABRUPT));
  m_wndGroupFeatures.InsertGroupItem(ReportGroupCtrl::NO_GROUP, sDensityChange,
    static_cast<DWORD>(ReportLib::AbruptDensityChange));
  sDensityChange.Format(IDS_DENSITY_CHANGE_FORMAT, LoadResourceString(IDS_INTERMEDIATE));
  m_wndGroupFeatures.InsertGroupItem(ReportGroupCtrl::NO_GROUP, sDensityChange,
    static_cast<DWORD>(ReportLib::IntermediateDensityChange));
  m_wndGroupFeatures.SelectItem(static_cast<DWORD>(ReportLib::Indeterminate));

  /// map of all groups
  ReportOtherFindings::FillGroupMaps(m_mapGroupNames, m_mapSubFindings);

  // (Extra-colonic) Organs
  m_wndGroupOrgan.SetBackgroundImage(GetChildBackground(m_wndGroupOrgan.GetDlgCtrlID()));
  m_wndGroupOrgan.SetSelectedBold(TRUE);
  for (TOrganGroupMap::const_iterator it = m_mapGroupNames.begin(); it != m_mapGroupNames.end(); it++)
  {
    m_wndGroupOrgan.InsertGroupItem(ReportGroupCtrl::NO_GROUP, it->second, it->first);
  }
  m_wndGroupOrgan.SelectItem(m_mapGroupNames.begin()->first);

  // (Extra-colonic finding) description
  m_wndGroupFindingDesc.SetBackgroundImage(GetChildBackground(m_wndGroupFindingDesc.GetDlgCtrlID()));
  m_wndGroupFindingDesc.SetSelectedBold(TRUE);
  m_wndGroupFindingDesc.DeleteAllItems();

} // CreateRadioGroups()


/**
 * saves current state and releases revision object
 */
void ReportColonFindings::Reset()
{
  // clears all entries
  OnEntryChanging(CString());
  
  // release revision
  m_spRevision = NULL;

  ResetForm();
} // Reset()


/**
 * resets all child controls to a default state
 */
void ReportColonFindings::ResetForm()
{
  // reset all controls to default state
  m_wndEntries.DeleteAllItems();
  ClearImages();
  SetDlgItemText(IDC_ENTRY_NAME, CString());
  SetDlgItemText(IDC_OTHER_COMMENTS, CString());

  m_iFindingType = 0; // Normal
  UpdateData(FALSE);

  m_wndGroupShape.SelectItem(static_cast<DWORD>(ReportLib::Sessile));
  m_wndGroupLocation.SelectItem(static_cast<DWORD>(ReportLib::Cecum));
  m_wndcheckFlexure.SetCheck(0);
  m_wndGroupFeatures.SelectItem(static_cast<DWORD>(ReportLib::Indeterminate));
  m_wndMaximumSize.SetValue(0.0F);
  m_wndPerpendicularSize.SetValue(0.0F);  m_wndPerpendicularSize.ShowWindow(SW_HIDE);
} // ResetForm()


/**
 * delete the sub-finding window
 */
void ReportColonFindings::DeleteSubFindingDialog()
{
  if ( m_pWndSubFinding == NULL )
    return;

  if ( IsWindow(*m_pWndSubFinding) )
  {
    OnSubFindingDialogUnactivate();
    m_pWndSubFinding->ShowWindow(SW_HIDE);
    m_pWndSubFinding->DestroyWindow();
  }

  delete m_pWndSubFinding;
  m_pWndSubFinding = NULL;

} // DeleteSubFindingDialog()


/**
 * on sub-finding window un-activate
 */
void ReportColonFindings::OnSubFindingDialogUnactivate()
{
  if ( ( m_pWndSubFinding == NULL ) || !IsWindow(*m_pWndSubFinding) )
  {
    return;
  }

  try
  {
    m_spRevision->OtherFindings->GetItem(m_pWndSubFinding->GetCurrentFinding()->ID);
    m_pWndSubFinding->OnReportViewUnactivate();
  }
  catch(_com_error&)
  {
		LogDbg("No need to report this com exception", "ReportColonFindings", "OnSubFindingDialogUnactivate");
    ; // presumably, an error will occur for the GetItem call above if the entry has been deleted; this is normal, just move on
  }

} // OnSubFindingDialogUnactivate()


/**
 * determines which dataset position is referenced in the finding name
 *
 * @param sFindingName   name of the finding to map
 * @return               scan position identified in the finding name
 */
ReportLib::DatasetPositionEnumProp ReportColonFindings::MapFindingNameToDatasetPosition(const std::string& sFindingName)
{
  // is it a supine, prone, other, or indeterminate (old version)?
  const int4 viFindName[] = { IDS_SUPINE, IDS_PRONE, IDS_DECUBITUS, IDS_DECUBITUS_L, IDS_DECUBITUS_R, IDS_DECUB_L, IDS_DECUB_R };
  const ReportLib::DatasetPositionEnumProp vePos[]
    = { ReportLib::Supine, ReportLib::Prone, ReportLib::Decubitus, ReportLib::DecubitusLeft, ReportLib::DecubitusRight, ReportLib::DecubitusLeft, ReportLib::DecubitusRight };

  for (int4 i(0); i < sizeof(viFindName)/sizeof(int4); i++ )
  {
    if (sFindingName.find(LPCTSTR('('+LoadResourceStringWithUpperHead(viFindName[i])+')')) != std::string::npos)
    {
      return vePos[i];
    }
  }

  // {{ this is a hack for the possible case where reports were created in English and read in another language
  int MISSING_LANGUAGE_TRANSLATIONS_FOR_ITALIAN_DECUBITUS;
  const std::string vsFindName[2][7] =
  {
    { "(Supine)",     "(Prone)",      "(Decubitus)",  "(Decubitus Left)",    "(Decubitus Right)", "(DecubL)", "(DecubR)" }, // Italian (has yet to be translated)
    { "(Décubitus)",  "(Procubitus)", "(Décubitus)",  "(Décubitus Gauche)",  "(Décubitus Droit)", "(DécubG)", "(DécubD)" }  // French
  };

  for (int4 i(0); i < sizeof(viFindName)/sizeof(int4); i++ )
  {
    if ((sFindingName.find(vsFindName[0][i]) != std::string::npos)
     || (sFindingName.find(vsFindName[1][i]) != std::string::npos))
    {
      return vePos[i];
    }
  }
  // }}

  LogErr("Could not match finding name to an appropriate dataset position.  Contact service for assistance with possible language translation error.", "ReportColonFindings", "MapFindingNameToDatasetPosition");

  // old version: unable to find a dataset position name ID
  return ReportLib::DatasetPositionEnumProp(-1);  // indeterminate: for old version.
} // MapFindingNameToDatasetPosition( sFindingName )


/**
 * called when a new revision is opened
 *
 * @param spRevision    smart pointer to new revision object
 */
void ReportColonFindings::SetReport(ReportLib::IReportRevisionPtr& spRevision)
{
  ReportWndBase::SetReport(spRevision);

  try
  {
    m_spRevision = spRevision;
    int4 iNumEntries = m_spRevision->Entries->Count;

    // for each entry in the revision
    for (int4 iEntry(0); iEntry < iNumEntries; iEntry++)
    {
      // grab the entry from revision
      ReportLib::IReportEntryPtr spEntry = m_spRevision->Entries->GetItem((long)iEntry);

      LVITEM lvi;
      lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
      lvi.iItem     = m_wndEntries.GetItemCount(); // insert at end of control
      lvi.iSubItem = 0;
      lvi.iImage = 0; // first image index

      // set this name from the entry name (we need this tom foolery because of MFC's lack of support for BSTRs)
      const std::string sName = spEntry->Name;

      int4 iImageNum(0);
      int4 iStrID(0);
      // is it a supine, prone, or indeterminate (old version)?
      switch (MapFindingNameToDatasetPosition(sName))
      {
        case ReportLib::Supine:
        {
          iStrID = IDS_SUPINE;
          iImageNum = 1; // supine
          break;
        }
        case ReportLib::Prone:
        {
          iStrID = IDS_PRONE;
          iImageNum = 2; // prone
          break;
        }
        case ReportLib::Decubitus:
        {
          iStrID = IDS_DECUBITUS;
          iImageNum = 0; // other
          break;
        }
        case ReportLib::DecubitusLeft:
        {
          iStrID = IDS_DECUBITUS_L;
          iImageNum = 0; // other
          break;
        }
        case ReportLib::DecubitusRight:
        {
          iStrID = IDS_DECUBITUS_R;
          iImageNum = 0; // other
          break;
        }
        default:
        {
          iStrID = 0;
          iImageNum = 0; // other
          break;
        }
      }

      const bool bDistanceUncertain = (sName[0] == '-');
      CString strName;
      if (bDistanceUncertain || (iStrID == 0) || (spEntry->OrigDistanceToRectum <= 0.0F) )
      {
        strName = LoadResourceStringWithUpperHead(IDS_COLONFINDING_DISTANCE_UNCERTAIN); //" Distance uncertain";
      }
      else
      {
        strName.Format( LoadResourceString( IDS_DISTANCE_FROM_RECTUM_FORMAT ), spEntry->OrigDistanceToRectum/10.0F);
      }

      lvi.pszText = const_cast<LPTSTR>( static_cast<LPCTSTR>(strName) );
      // allocate and populate List Item data and associate with item's data
      EntryListData* p = new EntryListData;
      p->m_sEntryID = static_cast<LPCTSTR>(spEntry->ID);
      lvi.lParam  = reinterpret_cast<LPARAM>(p);
      lvi.iImage = iImageNum;

      // insert the item
      m_wndEntries.InsertItem(&lvi);

    } // end for each entry

    // select the first item in the entries list
    m_wndEntries.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, 0x000f);
  }
  catch(_com_error& e)
  {
	  LogDbg("No need to report this com exception", "ReportColonFindings", "SetReport");

    DisplayReportError(IDS_READ_COLON_FINDING_ENTRY, e.Error());
  }
  catch( ... )
  {
    AfxMessageBox( IDS_ERROR_UNSPECIFIED, MB_ICONSTOP|MB_OK);
    LogErr( "Unhandled exception caught.", "ReportColonFindings", "SetReport" );
  }
} // SetReport( spRevision )


/**
 * removes an image from the list control and the revision object
 *
 * @param pszImageID   unique ID of image object
 * @return             TRUE upon success
 */
BOOL ReportColonFindings::DeleteImage(LPCTSTR pszImageID)
{
  try
  {
    // remove image from entry  based on Image ID
    m_spRevision->Entries->GetItem(static_cast<LPCTSTR>(m_sCurrentEntry))->Images->Remove(pszImageID);

    // find index of image in image's list control
    int4 iItem(FindImage(pszImageID));

    // remove the item from the list control
    m_wndImages.DeleteItem(iItem);

    // if only the overview image exists, that is,  there is only one image left, delete the entry
    if( ( !m_bExtraColonic && m_wndImages.GetItemCount() == 1 ) || ( m_bExtraColonic && m_wndImages.GetItemCount() == 0 ) )
    {
      // remove from revision
      DeleteEntry(m_sCurrentEntry);

      // select the first item in the entries list control
      m_wndEntries.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, 0x000f);
    }
  }
  catch(_com_error& e)
  {
	  LogDbg("No need to report this com exception", "ReportColonFindings", "DeleteImage");

    DisplayReportError(IDS_DELETE_COLON_FINDING, e.Error());
    return FALSE;
  }
  catch( ... )
  {
    AfxMessageBox( IDS_ERROR_UNSPECIFIED, MB_ICONSTOP|MB_OK);
    LogErr( "Unhandled exception caught.", "ReportColonFindings", "DeleteImage" );
    return FALSE;
  }

  return TRUE;
} // DeleteImage( pszImageID )


/**
 * finds an image within the image control
 *
 * @param pszImageID    unique ID of image object
 * @return              position of image within control, -1 if not found
 */
int4 ReportColonFindings::FindImage(LPCTSTR pszImageID) const
{
  // get number of images in list control
  int4 iNumImages(m_wndImages.GetItemCount());

  // for each image in the list control
  for(int4 iImage(0); iImage < iNumImages; iImage++)
  {
    // get the item's associated data
    ImageListData* pd = reinterpret_cast<ImageListData*>(m_wndImages.GetItemData(iImage));
    ASSERT(pd);

    // if the two IDs match return the index of the image
    if(!_stricmp(pszImageID, pd->m_sImageID))
    {
      return iImage;
    }
  } // end for each image

  return -1; // not found
} // FindImage( pszImageID )


/**
 * removes an entry from the list control and the revision object
 *
 * @param pszEntryID    unique ID of entry object
 * @return              TRUE upon success
 */
BOOL ReportColonFindings::DeleteEntry(LPCTSTR pszEntryID)
{
  try
  {
    // remove the associated other finding if it exists
    _bstr_t sOtherID = m_spRevision->Entries->GetItem(static_cast<LPCTSTR>(pszEntryID))->OtherFindingID;

		if ( sOtherID.length() > 0 )
		{
		  try
		  {
			  m_spRevision->OtherFindings->Remove(sOtherID);
		  }
		  catch(_com_error&)
		  {
				LogErr("Delete entry com exception thrown.", "ReportColonFindings", "DeleteEntry");
		  }
		} // if

    // remove entry from revision based on Entry ID
    m_spRevision->Entries->Remove(pszEntryID);

    // find entry in entries list control
    int4 iItem(FindEntry(pszEntryID));

    // set that no entry is current
    m_sCurrentEntry = _T("");

    // delete the entry from the list control
    m_wndEntries.DeleteItem(iItem);

    // if there aren't anymore entries reset the form controls
    if(m_wndEntries.GetItemCount() == 0)
      ResetForm();
  }
  catch(_com_error& e)
  {
	  LogErr("Delete entry throws exception", "ReportColonFindings", "DeleteEntry");

    DisplayReportError(IDS_DELETE_COLON_FINDING, e.Error());
    return FALSE;
  }
  catch( ... )
  {
    AfxMessageBox( IDS_ERROR_UNSPECIFIED, MB_ICONSTOP|MB_OK);
    LogErr( "Unhandled exception caught.", "ReportColonFindings", "DeleteEntry" );
    return FALSE;
  }
  return TRUE;
} //DeleteEntry( pszEntryID )


/**
 * finds an entry within the list control
 *
 * @param pszImageID    unique ID of entry object
 * @return              position of entry within control, -1 if not found
 */
int4 ReportColonFindings::FindEntry(LPCTSTR pszEntryID) const
{
  // get number of entries in the list control
  int4 iNumEntries(m_wndEntries.GetItemCount());

  // for each entry in the list control
  for(int4 iEntry(0); iEntry < iNumEntries; iEntry++)
  {
    // get the entries associated data
    EntryListData* pd = reinterpret_cast<EntryListData*>(m_wndEntries.GetItemData(iEntry));
    ASSERT(pd);

    // if the two IDs match return the index of the entry
    if(!_stricmp(pszEntryID, pd->m_sEntryID))
      return iEntry;
  } //  end for each entry

  return -1; //  not found
} // FindEntry( pszEntryID )


/**
 * selects an entry within the list control
 *
 * @param pszImageID    unique ID of entry object
 * @return              TRUE upon success
 */
BOOL ReportColonFindings::SelectEntry(LPCTSTR pszEntryID)
{
  // find entry based on Entry ID
  int4 iItem(FindEntry(pszEntryID));
  if(iItem == -1)
  {
    return FALSE; // item not found
  }

  // select the item
  m_wndEntries.SetItemState(iItem, LVIS_SELECTED|LVIS_FOCUSED, 0x000f);

  // redraw
  m_wndEntries.Update(iItem);

  return TRUE;
} // SelectEntry( pszEntryID )


/**
 * Message: LVN_DELETEITEM, Called when an item's entries have been deleted.
 *
 * @param pNMHDR    Notification message header.
 * @param pResult   Success/failure.
 */
void ReportColonFindings::OnDeleteItemEntries(NMHDR* pNMHDR, LRESULT* pResult)
{
  NM_LISTVIEW* plv = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);

  // get item's associated data and free it
  EntryListData* p = reinterpret_cast<EntryListData*>(plv->lParam);
  if(p)
  {
    delete p;
  }

  *pResult = 0;
} // OnDeleteItemEntries( pNMHDR, pResult )


/**
 * Message: LVN_ITEMCHANGED, Called when an item's entries have changed.
 *
 * @param pNMHDR    Notification message header.
 * @param pResult   Success/failure.
 */
void ReportColonFindings::OnItemChangedEntries(NMHDR* pNMHDR, LRESULT* pResult)
{
  NM_LISTVIEW* plv = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);

  // only interested in a state change
  if(plv->uChanged & LVIF_STATE)
  {
    // if item state is changing to selected
    if(!(plv->uOldState&LVIS_SELECTED) && (plv->uNewState & LVIS_SELECTED))
    {
      // get item's associated data
      EntryListData* p = reinterpret_cast<EntryListData*>(plv->lParam);

      // inform that we are about to change
      OnEntryChanging(m_sCurrentEntry);

      // now do the change
      m_sCurrentEntry = p->m_sEntryID;
      OnEntryChanged(m_sCurrentEntry);
    }
  }

  *pResult = 0;
} // OnItemChangedEntries( pNMHDR, pResult )


/**
 * Message: LVN_ITEMCHANGED, Called when an item's entries have changed.
 *
 * @param pNMHDR    Notification message header.
 * @param pResult   Success/failure.
 */
void ReportColonFindings::OnItemChangedOrgan(NMHDR* pNMHDR, LRESULT* pResult)
{
  NM_LISTVIEW* plv = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);

  // only interested in a state change
  if ( (!(plv->uChanged & LVIF_STATE)) || !(!(plv->uOldState&LVIS_SELECTED) && (plv->uNewState & LVIS_SELECTED)) )
  {
    return;
  }

  m_wndGroupFindingDesc.DeleteAllItems();
  const uint4 uOrganID(m_wndGroupOrgan.GetSelectedItem());
  for (TSubFindingMap::const_iterator it = m_mapSubFindings.begin(); it != m_mapSubFindings.end(); it++)
  {
    if (it->second->eGroupID == uOrganID)
    {
      m_wndGroupFindingDesc.InsertGroupItem(ReportGroupCtrl::NO_GROUP, it->second->sName, it->first);
    }
  }

  if ( m_bUserClickedFindingDesc )
  {
    m_wndGroupFindingDesc.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, 0x000F);
  }
  else
  {
    m_wndGroupFindingDesc.SetItemState(-1, LVIS_SELECTED|LVIS_FOCUSED, 0x000F);
    DeleteSubFindingDialog();
  }

} // OnItemChangedOrgan( pNMHDR, pResult )


/**
 * Message: LVN_ITEMCHANGED, Called when an item's entries have changed.
 *
 * @param pNMHDR    Notification message header.
 * @param pResult   Success/failure.
 */
void ReportColonFindings::OnItemChangedFindingDesc(NMHDR* pNMHDR, LRESULT* pResult)
{
  NM_LISTVIEW* plv = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);

  // only interested in a state change
  if (!(plv->uChanged & LVIF_STATE) || (m_spRevision == NULL) )
  {
    return;
  }

  // if item state is changing to selected
  if (!(plv->uOldState&LVIS_SELECTED) && (plv->uNewState & LVIS_SELECTED))
  {
    DeleteSubFindingDialog();
    m_pWndSubFinding = new ReportSubFinding;

    TSubFindingMap::iterator itSub = m_mapSubFindings.find(ReportLib::OtherFindingsEnum(m_wndGroupFindingDesc.GetSelectedItem()));
    m_pWndSubFinding->Create(itSub->second->uDlgID, this);
    CRect rct(WinUtils::GetWindowRect(GetDlgItem(IDC_RCF_GROUPBOX_FINDINGDESC)));
    ScreenToClient(rct);
    m_pWndSubFinding->SetWindowPos(this, rct.right + 5, rct.top, 0,0, SWP_NOSIZE|SWP_NOZORDER);
    m_pWndSubFinding->ShowWindow(SW_SHOW);

    ReportLib::IReportEntryPtr spEntry = m_spRevision->Entries->GetItem(static_cast<LPCTSTR>(m_sCurrentEntry));
    ReportLib::IReportOtherFindingPtr spFoundFinding = NULL;
    for (long i(0); i < m_spRevision->OtherFindings->Count; i++)
    {
      ReportLib::IReportOtherFindingPtr spFinding = m_spRevision->OtherFindings->GetItem(i);
      if ( spFinding->ID == spEntry->OtherFindingID )
      {
        spFoundFinding = spFinding;
        break;
      }
    }

    if (spFoundFinding == NULL)
    {
      spFoundFinding = m_spRevision->OtherFindings->Add();
      spEntry->OtherFindingID = spFoundFinding->ID;
    }

    spFoundFinding->Finding = itSub->first;
    m_pWndSubFinding->SetFinding(spFoundFinding);
    m_pWndSubFinding->m_spRevision = m_spRevision;

  }
} // OnItemChangedFindingDesc( pNMHDR, pResult )


/**
 * Message: , Called to validate an entry change.
 *
 * @param pszEntryID   Entry identifier.
 * @return             TRUE to enable the change.
 */
BOOL ReportColonFindings::OnEntryChanging(LPCTSTR pszEntryID)
{
  try
  {
    // make sure that we have an entry and that the revision is not read only
    if(*pszEntryID == '\0' || m_spRevision->IsReadOnly == VARIANT_TRUE)
    {
      return TRUE;
    }

    // update the entry object in the report file from the values on the form
    ReportLib::IReportEntryPtr spEntry = m_spRevision->Entries->GetItem(pszEntryID);

    UpdateData(TRUE);
    switch (m_iFindingType)
    {
      case NORMAL:
      {
	    if( spEntry->ColonFinding == static_cast<ReportLib::ColonFindingEnum>(ReportLib::Other_Extracolonic) )
		{ // This entry is flipped back from an extra-colonic finding. Need to update the other finding list
		  RemoveFlippedItemFromOtherFindingList( spEntry );
		} // if
        spEntry->ColonFinding = static_cast<ReportLib::ColonFindingEnum>(ReportLib::Other_Normal);
        break;
      }
      case POLYP:
      {
	    if( spEntry->ColonFinding == static_cast<ReportLib::ColonFindingEnum>(ReportLib::Other_Extracolonic) )
		{ // This entry is flipped back from an extra-colonic finding. Need to update the other finding list
		  RemoveFlippedItemFromOtherFindingList( spEntry );
		} // if
        spEntry->ColonFinding = static_cast<ReportLib::ColonFindingEnum>(ReportLib::Polyp);
        break;
      }
      case DIVERTICULUM:
      {
	    if( spEntry->ColonFinding == static_cast<ReportLib::ColonFindingEnum>(ReportLib::Other_Extracolonic) )
		{ // This entry is flipped back from an extra-colonic finding. Need to update the other finding list
		  RemoveFlippedItemFromOtherFindingList( spEntry );
		} // if
        spEntry->ColonFinding = static_cast<ReportLib::ColonFindingEnum>(ReportLib::Diverticulum);
        break;
      }
      case STENOSIS:
      {
	    if( spEntry->ColonFinding == static_cast<ReportLib::ColonFindingEnum>(ReportLib::Other_Extracolonic) )
		{ // This entry is flipped back from an extra-colonic finding. Need to update the other finding list
		  RemoveFlippedItemFromOtherFindingList( spEntry );
		} // if
        spEntry->ColonFinding = static_cast<ReportLib::ColonFindingEnum>(ReportLib::Stenosis);
        break;
      }
      case OTHER:
      {
	    if( spEntry->ColonFinding == static_cast<ReportLib::ColonFindingEnum>(ReportLib::Other_Extracolonic) )
		{ // This entry is flipped back from an extra-colonic finding. Need to update the other finding list
		  RemoveFlippedItemFromOtherFindingList( spEntry );
		} // if
        spEntry->ColonFinding = static_cast<ReportLib::ColonFindingEnum>(ReportLib::Other_Colonic);
        break;
      }
      case EXTRACOLONIC:
      default: // else
      {
        spEntry->ColonFinding = static_cast<ReportLib::ColonFindingEnum>(ReportLib::Other_Extracolonic);

		// need to add the extra colonic finding back to the other finding list.
		ReportLib::IReportOtherFindingPtr spFoundFinding = NULL;
		for (long i(0); i < m_spRevision->OtherFindings->Count; i++)
		{
		  ReportLib::IReportOtherFindingPtr spFinding = m_spRevision->OtherFindings->GetItem(i);
		  if ( spFinding->ID == spEntry->OtherFindingID )
		  {
			spFoundFinding = spFinding;
			break;
		  }
		} // for

		if (spFoundFinding == NULL)
		{ /// all string property has to be initialized. Otherwise, it may cause crash at certain circumference.
		  spFoundFinding = m_spRevision->OtherFindings->Add();
		  spFoundFinding->Finding = ReportLib::Misc_Other;
		  spEntry->OtherFindingID = spFoundFinding->ID;
		}
        break;
      }
    }

    int4 iLocation = m_wndGroupLocation.GetSelectedItem() + (m_wndcheckFlexure.GetCheck()!=0 ? ReportLib::NearFlexure : 0);
    spEntry->Location = static_cast<ReportLib::LocationEnum>(iLocation);

    if (spEntry->ColonFinding == ReportLib::Polyp)
    {
      spEntry->Shape = static_cast<ReportLib::ShapeEnum>(m_wndGroupShape.GetSelectedItem());
      spEntry->Translucency = static_cast<ReportLib::TranslucencyEnum>(m_wndGroupFeatures.GetSelectedItem());
      spEntry->Maxiumum = m_wndMaximumSize.GetValue();
      spEntry->Perpendicular = m_wndPerpendicularSize.GetValue();
    }
    else if ( spEntry->ColonFinding == ReportLib::Other_Extracolonic )
    {
      OnSubFindingDialogUnactivate();
    }

    CString sComments;
    GetDlgItemText(IDC_OTHER_COMMENTS, sComments);
    spEntry->Comments = static_cast<LPCTSTR>(sComments);
    // end update
  }
  catch(_com_error& e)
  {
	  LogErr("Change entry throwing an exception", "ReportColonFindings", "OnEntryChanging");

    DisplayReportError(IDS_READ_COLON_FINDING_ENTRY, e.Error());
    return FALSE;
  }
  catch( ... )
  {
    AfxMessageBox( IDS_ERROR_UNSPECIFIED, MB_ICONSTOP|MB_OK);
    LogErr( "Unhandled exception caught.", "ReportColonFindings", "OnEntryChanging" );
    return FALSE;
  }

  return TRUE;
} // OnEntryChanging( pNMHDR, pResult )


/**
 *  Removing finding from the extra-colonic finding list after user flips the associated
 *  extra-colonic finding entry back to a colonic finding.
 *
 * @param spEntry the pointer to the updated finding entry.
 */
void ReportColonFindings::RemoveFlippedItemFromOtherFindingList( ReportLib::IReportEntryPtr & spEntry )
{
	for (long i(0); i < m_spRevision->OtherFindings->Count; i++)
	{
	  ReportLib::IReportOtherFindingPtr spFinding = m_spRevision->OtherFindings->GetItem(i);
	  if ( spFinding->ID == spEntry->OtherFindingID )
	  {
		m_spRevision->OtherFindings->Remove(i);
		break;
	  }
	} // for

} // RemoveFlippedItemFromOtherFindingList



/**
 * Called after an entry has changed.
 *
 * @param pszEntryID   Entry identifier.
 * @return             TRUE.
 */
BOOL ReportColonFindings::OnEntryChanged(LPCTSTR pszEntryID)
{
  CWaitCursor cursor;

  try
  {
    // transfer data from entry to form
    ClearImages();
    ReportLib::IReportEntryPtr spEntry = m_spRevision->Entries->GetItem(pszEntryID);

    std::string str(spEntry->Name);
    if (str[0] == '-')
    {
      if (str.find(LoadResourceString(IDS_SUPINE_EXTRA)) != std::string::npos)
      {
        str = (LoadResourceString(IDS_SUPINE_EXTRA) + " " + LowerCaseString(IDS_COLONFINDING_DISTANCE_UNCERTAIN));
      }
      else if (str.find(LoadResourceString(IDS_PRONE_EXTRA)) != std::string::npos)
      {
        str = (LoadResourceString(IDS_PRONE_EXTRA) + " " + LowerCaseString(IDS_COLONFINDING_DISTANCE_UNCERTAIN));
      }
      else
      {
        str = LoadResourceStringWithUpperHead(IDS_COLONFINDING_DISTANCE_UNCERTAIN);
      }
    }
    SetDlgItemText(IDC_ENTRY_NAME, str.c_str());
    SetDlgItemText(IDC_OTHER_COMMENTS, static_cast<LPCTSTR>(spEntry->Comments));

    m_bExtraColonic = false;
    switch (static_cast<DWORD>(spEntry->ColonFinding))
    {
      case ReportLib::Polyp:
      {
        m_iFindingType = POLYP;
        SetFinding(spEntry->ColonFinding);
        break;
      }
      case ReportLib::Diverticulum:
      {
        m_iFindingType = DIVERTICULUM;
        SetFinding(spEntry->ColonFinding);
        break;
      }
      case ReportLib::Stenosis:
      {
        m_iFindingType = STENOSIS;
        SetFinding(spEntry->ColonFinding);
        break;
      }
      case ReportLib::Other_Normal:
      {
        m_iFindingType = NORMAL;
        SetFinding(ReportLib::Other_Normal, ColonReport::ReportColonFindings::COLONIC_NORMAL);
        break;
      }
      case ReportLib::Other_Colonic:
      {
        m_iFindingType = OTHER;
        SetFinding(ReportLib::Other_Colonic, ColonReport::ReportColonFindings::COLONIC_OTHER);
        break;
      }
      case ReportLib::Other_Extracolonic:
      {
        m_iFindingType = EXTRACOLONIC;
        SetFinding(ReportLib::Other_Extracolonic, ColonReport::ReportColonFindings::COLONIC_EXTRA);
        m_bExtraColonic = true;
        break;
      }
      default: 
      {
        break;
      }
    } // set up 'ColonFinding'
    UpdateData(FALSE);

    if (!m_bExtraColonic)
    {
      m_wndGroupLocation.SelectItem(static_cast<DWORD>(spEntry->Location & ~ReportLib::NearFlexure));
    }
    m_wndcheckFlexure.SetCheck(( (spEntry->Location & ReportLib::NearFlexure) == ReportLib::NearFlexure) ? 1 : 0);

    if (spEntry->ColonFinding == ReportLib::Polyp)
    {
      switch (static_cast<DWORD>(spEntry->Shape))
      {
        // backwards compatibility: old 'Round' and 'Oval' classifications are obsolete
        case ReportLib::Round:
        case ReportLib::Oval:
        {
          m_wndGroupShape.SetSelectionAll(false);
          break;
        }
        // backwards compatibility: unseen stalk is the same as sessile
        case ReportLib::PedunculatedStalkUnseen:
        {
          m_wndGroupShape.SelectItem(static_cast<DWORD>(ReportLib::Sessile));
          break;
        }
        default:
        {
          m_wndGroupShape.SelectItem(static_cast<DWORD>(spEntry->Shape));
          break;
        }
      }

      m_wndGroupFeatures.SelectItem(static_cast<DWORD>(spEntry->Translucency));
    }
    m_wndMaximumSize.SetValue(spEntry->Maxiumum);
    m_wndPerpendicularSize.SetValue(spEntry->Perpendicular);
    m_wndPerpendicularSize.ShowWindow((spEntry->ColonFinding == ReportLib::Polyp) && (spEntry->Perpendicular > 0.0f) ? SW_SHOW : SW_HIDE);
    
    AddImages(m_bExtraColonic);
    
    // end transfer
  }
  catch(_com_error& e)
  {
	  LogErr("Entry change throws an exception", "ReportColonFindings", "OnEntryChanged");

    DisplayReportError(IDS_READ_COLON_FINDING_ENTRY, e.Error());
		return FALSE;
  }
  catch( ... )
  {
    AfxMessageBox( IDS_ERROR_UNSPECIFIED, MB_ICONSTOP|MB_OK);
    LogErr( "Unhandled exception caught.", "ReportColonFindings", "OnEntryChanged" );
  }

  return TRUE;
} // OnEntryChanged( pszEntryID )


/**
 * Removes all images from list.
 *
 * @return     TRUE.
 */
BOOL ReportColonFindings::ClearImages()
{
  // remove images from list control
  m_wndImages.DeleteAllItems();

  // create the new image list
  HIMAGELIST himl = ImageList_Create(m_szImage.cx, m_szImage.cy, ILC_COLOR24, 0, 2);
  if(!himl)
  {
    return FALSE;
  }

  // attach MFC wrapper
  CImageList objImageList;
  objImageList.Attach(himl);

  // set the new image list and detach from MFC wrapper
  CImageList* piml = m_wndImages.SetImageList(&objImageList, LVSIL_NORMAL);
  objImageList.Detach();

  // free the previous image list
  if(piml)
  {
    piml->DeleteImageList();
  }

  return TRUE;
} // ClearImages


/**
 * Adds images to the list control.
 *
 * @param bExtraColonic   Whether or not the entry is an extra-colonic finding.
 */
void ReportColonFindings::AddImages(const bool bExtraColonic/* =false */)
{
  try
  {
    ReportLib::IReportImagesPtr spImages = NULL;
    try
    {
      spImages = m_spRevision->Entries->GetItem(static_cast<LPCTSTR>(m_sCurrentEntry))->Images;
    }
    catch(_com_error&)
    {
	    LogErr("Add image throws an exception", "ReportColonFindings", "AddImages");

      return;
    }

    CRect rcClient;
    m_wndImages.GetClientRect(rcClient);

    const int4 iNumImages(spImages->Count);

    CSize szSpacing(0, 0);

    // set the horizontal spacing for the images
    if(iNumImages == 2)
    {
      szSpacing.cx = ((rcClient.Width() -  (2* m_szImage.cx)) / 3) + m_szImage.cx;
    }
    else
    {
      szSpacing.cx = ((rcClient.Width() -  (3* m_szImage.cx)) / 4) + m_szImage.cx;
    }

    m_wndImages.SetIconSpacing(szSpacing.cx, szSpacing.cy);

    // set the work area so image scroll horizontally
    CRect rcWorkArea(0, 0, iNumImages*szSpacing.cx, m_szImage.cy);
    if(rcClient.Width() > rcWorkArea.Width())
    {
      rcWorkArea = rcClient;
    }

    // set the vertical offset
    CSize szOffset;
    szOffset.cx = (szSpacing.cx - m_szImage.cx) / 2;

    if(iNumImages > 3)
    {
      szOffset.cy = (rcClient.Height() - m_szImage.cy - GetSystemMetrics(SM_CYHSCROLL)) / 2;
    }
    else
    {
      szOffset.cy = (rcClient.Height() - m_szImage.cy) / 2;
    }

    rcWorkArea.OffsetRect(szOffset);

    m_wndImages.SetWorkAreas(1, &rcWorkArea);

    // for each image in the entry
    for (int4 iImage(0); iImage < iNumImages; iImage++)
    {
      int4 iIndex(-1);

      // get the image from the entry
      ReportLib::IImagePtr spImage = spImages->GetItem((long)iImage);

      // according to the finding selection & label, display this image or not.
      if (bExtraColonic)
      {
        // test: if we need to display this image.
        COleVariant vPixels = spImage->Pixels;
        LPBYTE pPixels;
        SafeArrayAccessData(vPixels.parray, reinterpret_cast<void**>(&pPixels));
        if (iImage == 0) // hide overview for extra-colonic
        // ((pPixels[0]==0x0F && pPixels[2]==0x0F) || (pPixels[0]==26 && pPixels[2]==26))
        {
          // '26': in most cases, the overview image has value '26' at first pixel.
          SafeArrayUnaccessData(vPixels.parray);
          continue;
        }
        else
        {
          SafeArrayUnaccessData(vPixels.parray);
        }
      }

      // resize the image
      spImage->Resize(256, 256);

      // add it to the image list
      CImageList* piml = m_wndImages.GetImageList(LVSIL_NORMAL);
      iIndex = ImageList_Add(
        piml->GetSafeHandle(),
        reinterpret_cast<HBITMAP>(spImage->Bitmap),
        NULL
      );

      // create a new data item
      ImageListData* p = new ImageListData;

      p->m_sImageID = static_cast<LPCTSTR>(spImage->ID);

      // insert the item intothe list control
      LVITEM lvi;
      lvi.mask      = LVIF_IMAGE|LVIF_PARAM;
      lvi.iItem     = m_wndImages.GetItemCount();
      lvi.iSubItem  = 0;
      lvi.iImage    = iIndex;
      lvi.lParam    = reinterpret_cast<LPARAM>(p);

      m_wndImages.InsertItem(&lvi);
    }
  }
  catch(_com_error& e)
  {
	  LogErr("Add image throws an exception", "ReportColonFindings", "AddImages");

    DisplayReportError(IDS_ADD_REPORT_IMAGE, e.Error());
  }
  catch( ... )
  {
    AfxMessageBox( IDS_ERROR_UNSPECIFIED, MB_ICONSTOP|MB_OK);
    LogErr( "Unhandled exception caught.", "ReportColonFindings", "AddImages" );
  }
} // AddImages()


/**
 * Message: LVN_DELETEITEM, Deletes an item's images.
 *
 * @param pNMHDR    Notification message header.
 * @param pResult   Success/failure.
 */
void ReportColonFindings::OnDeleteItemImages(NMHDR* pNMHDR, LRESULT* pResult)
{
  NM_LISTVIEW* plv = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);

  // get items's associated data and free it
  ImageListData* p = reinterpret_cast<ImageListData*>(plv->lParam);
  if(p)
  {
    delete p;
  }

  *pResult = 0;
} // OnDeleteItemImages( pNMHDR, pResult )


/**
 * Message: LVN_BEGINDRAG, Notification that an entry drag-n-drop has begun.
 *
 * @param pNMHDR    Notification message header.
 * @param pResult   Success/failure.
 */
void ReportColonFindings::OnBeginDragEntries(NMHDR* pNMHDR, LRESULT* pResult)
{
  // nothing to drag to if there is not multiple items
  if (m_wndEntries.GetItemCount() <= 1)
  {
    return;
  }

  NM_LISTVIEW* plv = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);

  // create the drag (overview) image
  CPoint pt;
  m_pDragImage = CreateDragImage(0, pt);
  if (!m_pDragImage)
  {
    return;
  }

  // begin the D&D operation
  m_pDragImage->BeginDrag(0, CPoint(0, 0));
  MapWindowPoints(NULL, &plv->ptAction, 1);
  m_pDragImage->DragEnter(CWnd::GetDesktopWindow(), plv->ptAction);
  SetCapture();
  // end begin D&D operation

  *pResult = 0;

} // OnBeginDragEntries( pNMHDR, pResult )


/**
 * Message: LVN_BEGINDRAG, Notification that an image drag-n-drop has begun.
 *
 * @param pNMHDR    Notification message header.
 * @param pResult   Success/failure.
 */
void ReportColonFindings::OnBeginDragImages(NMHDR* pNMHDR, LRESULT* pResult)
{
  CWaitCursor cursor;

  NM_LISTVIEW* plv = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);

  // can't drag the Overview image
  if (!m_bExtraColonic && plv->iItem == 0)
  {
    *pResult = 1;
    return;
  }

  // create the drag image
  CPoint pt;
  m_pDragImage = CreateDragImage(plv->iItem, pt);
  if (!m_pDragImage)
  {
    return;
  }

  // begin the D&D operation
  m_pDragImage->BeginDrag(0, CPoint(0, 0));
  MapWindowPoints(NULL, &plv->ptAction, 1);
  m_pDragImage->DragEnter(CWnd::GetDesktopWindow(), plv->ptAction);
  SetCapture();
  // end begin D&D operation

  *pResult = 0;
} // OnBeginDragImages( pNMHDR, pResult )


/**
 * Creates the image displayed in the drag-n-drop operation.
 *
 * @param iItem         Item for the image.
 * @param pt            Client point.
 * @return CImageList*  Image list for the dragged image.
 */
CImageList* ReportColonFindings::CreateDragImage(int4 iItem, CPoint& pt)
{
  // create a smaller 64X64 image list
  HIMAGELIST himl = ImageList_Create(64, 64, ILC_COLOR24|ILC_MASK, 0, 1);
  if(!himl)
  {
    return NULL;
  }

  CImageList* pDragImage = NULL;

  try
  {
    // get image's associated data
    ImageListData* pd = reinterpret_cast<ImageListData*>(m_wndImages.GetItemData(iItem));

    // get the image from the entry
    ReportLib::IImagePtr spImage =
          m_spRevision->Entries->GetItem((LPCTSTR)m_sCurrentEntry)->Images->GetItem(static_cast<LPCTSTR>(pd->m_sImageID));

    // resize the image
    spImage->Resize(64, 64);

    // add the image to the image list
    int4 iIndex = ImageList_AddMasked(himl, reinterpret_cast<HBITMAP>(spImage->Bitmap), RGB(255,0,255));
    if(iIndex == -1)
    {
      ImageList_Destroy(himl);
      return NULL;
    }

    // create new image list
    pDragImage = new CImageList;
    if(!pDragImage)
    {
      ImageList_Destroy(himl);
      return NULL;
    }

    // attach to MFC wrapper
    pDragImage->Attach(himl);
  }
  catch(_com_error&)
  {
	  LogErr("CreateDragImage throws an exception", "ReportColonFindings", "CreatDragImage");

    delete pDragImage;
    return NULL;
  }
  catch( ... )
  {
    LogErr( "Unhandled exception", "ReportColonFindings", "CreateDragImage" );
    AfxMessageBox( IDS_ERROR_UNSPECIFIED, MB_ICONSTOP|MB_OK);
  }

  // return the new image list
  return pDragImage;
} // CreateDragImage( iItem, pt )


/**
 * Message: WM_LBUTTONUP, Button up message handler.
 *
 * @param uFlags   Virtual key flags.
 * @param point    Point of event.
 */
void ReportColonFindings::OnLButtonUp(UINT uFlags, CPoint point)
{
  // are we dragging
  if(m_pDragImage)
  {
    // stop image list drag operation
    m_pDragImage->EndDrag();
    delete m_pDragImage;
    m_pDragImage = NULL;

    // is there a drop item?
    if(m_iDropItem != -1)
    {
      // get the data associated with the drop item
      EntryListData* ped = reinterpret_cast<EntryListData*>(m_wndEntries.GetItemData(m_iDropItem));
      ASSERT(ped);

      // get the data associated with the selected item
      ImageListData* pid = reinterpret_cast<ImageListData*>(m_wndImages.GetItemData(m_wndImages.GetNextItem(-1, LVNI_SELECTED)));
      ASSERT(pid);

      // do the drop
      OnDropEntries(ped->m_sEntryID, pid->m_sImageID);

      // set the drop item as selected
      m_wndEntries.SetItemState(m_iDropItem, 0, LVIS_DROPHILITED);
      m_wndEntries.Update(m_iDropItem);
      m_iDropItem = -1;
    }

    ReleaseCapture();
  }

  ReportWndBase::OnLButtonUp(uFlags, point);
} // OnLButtonUp( uFlags, point )


/**
 * Message: WM_MOUSEMOVE, Mouse move event.
 *
 * @param uFlags   Virtual keys identifier.
 * @param point    Point of event.
 */
void ReportColonFindings::OnMouseMove(UINT uFlags, CPoint point)
{
  // are we dragging?
  if(m_pDragImage)
  {
    // convert point and move drag image
    CPoint pt(point);
    MapWindowPoints(NULL, &pt, 1);
    m_pDragImage->DragMove(pt);

    // is the cursor over the entries list control
    if(m_rcEntries.PtInRect(point))
    {
      // get the item that the cursor is over
      int4 iItem = m_wndEntries.HitTest(point);

      // make sure we have an item and it isn't the drop item
      if(iItem != -1 && iItem != m_iDropItem)
      {
        // show the item as a drop target
        m_pDragImage->DragShowNolock(FALSE);
        if(m_iDropItem != -1)
        {
          m_wndEntries.SetItemState(m_iDropItem, 0, LVIS_DROPHILITED);
          m_wndEntries.Update(m_iDropItem);
        }

        m_wndEntries.SetItemState(iItem, LVIS_DROPHILITED, LVIS_DROPHILITED);
        m_wndEntries.Update(iItem);
        m_iDropItem = iItem;
        m_pDragImage->DragShowNolock(TRUE);

        // end show
      }
      else if(iItem == -1) // not over any item
      {
        // reset drop item if any
        if(m_iDropItem != -1)
        {
          // reset drop item
          m_pDragImage->DragShowNolock(FALSE);
          m_wndEntries.SetItemState(m_iDropItem, 0, LVIS_DROPHILITED);
          m_wndEntries.Update(m_iDropItem);
          m_iDropItem = -1;
          m_pDragImage->DragShowNolock(TRUE);
          // end reset
        }
      }
    }
  }

  ReportWndBase::OnMouseMove(uFlags, point);
} // OnMouseMove( uFlags, point )


/**
 * Indicates a drag-n-drop operation has ended with a drop.
 *
 * @param pszTargetID   ID of drop target.
 * @param pszImageID    Image identifier.
 * @return              Whether the drop operation is allowed and successful.
 */
BOOL ReportColonFindings::OnDropEntries(LPCTSTR pszTargetID, LPCTSTR pszImageID)
{
  // get the select image
  int4 iItem = m_wndImages.GetNextItem(-1, LVNI_SELECTED);

  // get associated data of image
  ImageListData* pd = reinterpret_cast<ImageListData*>(m_wndImages.GetItemData(iItem));

  // if the two are the same return
  if(!_stricmp(pszTargetID, m_sCurrentEntry))
  {
    return FALSE;
  }

  try
  {
    ReportLib::IReportEntriesPtr spEntries      = m_spRevision->Entries;
    ReportLib::IReportImagesPtr  spSourceImages = spEntries->GetItem((LPCTSTR)m_sCurrentEntry)->Images;
    ReportLib::IReportImagesPtr  spTargetImages = spEntries->GetItem((LPCTSTR)pszTargetID)->Images;

    // determine if the user wants to move the image or the entire finding
    int4 iUsrPrompt(0);
    const bool bLast( spSourceImages->Count <= 2 );
    if ( (iUsrPrompt = AfxMessageBox(bLast?IDS_REPORT_DRAG_LAST_IMAGE_PROMPT :IDS_REPORT_DRAG_IMAGE_PROMPT,
      MB_ICONQUESTION | (bLast?MB_OKCANCEL:MB_YESNOCANCEL))) == IDCANCEL )
    {
      return FALSE;
    }

    CWaitCursor cursor;


    if ( iUsrPrompt == IDYES || iUsrPrompt == IDOK ) // Move the single image
    {
      // get image based on image ID
      ReportLib::IImagePtr spImage = spSourceImages->GetItem(pszImageID);

      // findings that were created from orphaned OtherFindings may be imageless; add the overview before adding the moved image
      if ( spTargetImages->Count <= 0 )
      {
        ReportLib::IImagePtr spOvImage = spSourceImages->GetItem(long(0));
        spTargetImages->Add(spOvImage);
      }

      // copy (add) the image to the new entry
      spTargetImages->Add(spImage);

      // remove the image from it's current location
      spSourceImages->Remove(pszImageID);
    }
    else // ( iUsrPrompt == IDNO ) // Move ALL images from Source to Target
    {
      while ( spSourceImages->Count > 0 )
      {
        spTargetImages->Add(spSourceImages->GetItem(0L));
        spSourceImages->Remove(0L);
      }
    }

    // delete image from current list
    m_wndImages.DeleteItem(iItem);

    // if only the overview exists, remove the entry
    if ( /*((spSourceImages->Count == 1) && m_bExtraColonic) ||*/ (spSourceImages->Count <= 1) )
    {
      // delete entry
      DeleteEntry(m_sCurrentEntry);
      m_sCurrentEntry.Empty();

      // select the new entry
      SelectEntry(pszTargetID);
    }
  }
  catch(_com_error& )
  {
	  LogErr("OnDropEntries throws an exception", "ReportColonFindings", "OnDropEntries");

    return FALSE;
  }
  catch( ... )
  {
    AfxMessageBox( IDS_ERROR_UNSPECIFIED, MB_ICONSTOP|MB_OK);
    LogErr( "Unhandled exception caught.", "ReportColonFindings", "OnDropEntries" );
    return FALSE;
  }

  return TRUE;
} // OnDropEntries( pszTargetID, pszImageID )


/**
 * Message: LVN_ITEMCHANGED
 *
 *  @param pNMHDR       Notification msg structure
 *  @param pResult      result
 */
void ReportColonFindings::OnItemChangedGroupFinding(NMHDR* pNMHDR, LRESULT* pResult)
{
  NM_LISTVIEW* plv = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);

  // update form based on new finding selection
  if(plv->uNewState & LVIS_SELECTED)
  {
    // SetFinding(static_cast<ReportLib::ColonFindingEnum>(plv->lParam));
  }

  *pResult = 0;
} // OnItemChangedGroupFinding( pNMHDR, pResult )


/**
 * sets the finding
 *  @param eFinding   finding category
 *  @param eSubEnum   sub-finding category
 */
void ReportColonFindings::SetFinding(const ReportLib::ColonFindingEnum eFinding, const ColonFindingSubEnum eSubEnum)
{
  if (m_spRevision == NULL || m_spRevision->Entries->Count <= 0)
  {
    return;
  }

  // show/hide the following controls if finding is a POLYP
  bool bPolyp = eFinding == ReportLib::Polyp;

  GetDlgItem(IDC_STATIC_SHAPE)->ShowWindow(bPolyp);
  m_wndGroupShape.ShowWindow(bPolyp);

  GetDlgItem(IDC_STATIC_TRANSLUCENCY)->ShowWindow(bPolyp);
  m_wndGroupFeatures.ShowWindow(bPolyp);

  const bool bExtraColonic(eSubEnum == ColonReport::ReportColonFindings::COLONIC_EXTRA);
  GetDlgItem(IDC_STATIC_SIZE)->ShowWindow(bPolyp);
  GetDlgItem(IDC_STATIC_SIZE_LABEL1)->ShowWindow(bPolyp);
  m_wndMaximumSize.ShowWindow(bPolyp);

  ReportLib::IReportEntryPtr spEntry = m_spRevision->Entries->GetItem(static_cast<LPCTSTR>(m_sCurrentEntry));
  m_wndPerpendicularSize.ShowWindow((bPolyp && (spEntry->Perpendicular > 0.0f)) ? SW_SHOW : SW_HIDE);
  GetDlgItem(IDC_STATIC_SIZE_LABEL2)->ShowWindow(bPolyp && (spEntry->Perpendicular > 0.0f));

  if ( bPolyp )
  {
    m_wndGroupShape.SelectItem(static_cast<DWORD>(ReportLib::PedunculatedStalkSeen));
    m_wndGroupFeatures.SelectItem(static_cast<DWORD>(ReportLib::SimilarToColonWall));
  }

  // added a new selection: Other - "Clear Location"
  m_wndGroupLocation.ShowWindow(!bExtraColonic);
  m_wndcheckFlexure.ShowWindow(!bExtraColonic);
  GetDlgItem(IDC_RCF_LOCATIONGROUPBOX)->ShowWindow(!bExtraColonic);
  GetDlgItem(IDC_OTHER_COMMENTS)->ShowWindow(!bExtraColonic);
  GetDlgItem(IDC_RCF_GROUPBOX_COMMENTS)->ShowWindow(!bExtraColonic);

  GetDlgItem(IDC_RCF_GROUPBOX_FINDINGDESC)->ShowWindow(bExtraColonic);
  m_wndGroupOrgan.ShowWindow(bExtraColonic);
  GetDlgItem(IDC_RCF_GROUPBOX_ORGAN)->ShowWindow(bExtraColonic);
  m_wndGroupFindingDesc.ShowWindow(bExtraColonic);

  // end show/hide

  if ( !bExtraColonic )
  { 
		DeleteSubFindingDialog();
  } // if

  if ( bExtraColonic )
  {
    ReportLib::IReportEntryPtr spEntry = m_spRevision->Entries->GetItem(static_cast<LPCTSTR>(m_sCurrentEntry));
    ReportLib::IReportOtherFindingPtr spFoundFinding = NULL;
    for (long i(0); i < m_spRevision->OtherFindings->Count; i++)
    {
      ReportLib::IReportOtherFindingPtr spFinding = m_spRevision->OtherFindings->GetItem(i);
      if ( spFinding->ID == spEntry->OtherFindingID )
      {
        spFoundFinding = spFinding;
        break;
      }
    }

    if (spFoundFinding == NULL)
    { /// all string property has to be initialized. Otherwise, it may cause crash at certain circumference.
      spFoundFinding = m_spRevision->OtherFindings->Add();
      spFoundFinding->Finding = ReportLib::Misc_Other;
      spEntry->OtherFindingID = spFoundFinding->ID;
    }

    DeleteSubFindingDialog();
    m_pWndSubFinding = new ReportSubFinding;

    TSubFindingMap::iterator itSub = m_mapSubFindings.find(spFoundFinding->Finding);
    m_pWndSubFinding->Create(itSub->second->uDlgID, this);
    CRect rct(WinUtils::GetWindowRect(GetDlgItem(IDC_RCF_GROUPBOX_FINDINGDESC)));
    ScreenToClient(rct);
    m_pWndSubFinding->SetWindowPos(this, rct.right + 5, rct.top, 0,0, SWP_NOSIZE|SWP_NOZORDER);
    m_pWndSubFinding->ShowWindow(SW_SHOW);

    m_pWndSubFinding->SetFinding(spFoundFinding);
    m_pWndSubFinding->m_spRevision = m_spRevision;

    m_bUserClickedFindingDesc = false;
    m_wndGroupOrgan.SelectItem(m_mapSubFindings[spFoundFinding->Finding]->eGroupID);
    m_wndGroupFindingDesc.SelectItem(spFoundFinding->Finding);
    m_bUserClickedFindingDesc = true;
  }

} // SetFinding( eFinding, eSubEnum )


/**
 * Message: NM_RCLICK
 *
 *  @param pNMHDR       Notification msg structure
 *  @param pResult      result
 */
void ReportColonFindings::OnRClickImages(NMHDR* pNMHDR, LRESULT* pResult)
{
  // get point at time of message
  DWORD dwPos = GetMessagePos();
  CPoint pt(LOWORD(dwPos), HIWORD(dwPos));

  CPoint ptHitTest(pt) ;
  m_wndImages.ScreenToClient(&ptHitTest);

  // make sure cursor is over an item
  int4 iItem = m_wndImages.HitTest(ptHitTest);
  if( !m_bExtraColonic && (  iItem < 1  || m_wndImages.GetItemCount() <= 1 ) )
  {
    return;
  }

  // load popup
  CMenu menu;
  VERIFY(menu.LoadMenu(IDM_REPORT_POPUPS));

  CMenu* pMenu = menu.GetSubMenu(1);

  // show menu and track
  pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON, pt.x, pt.y, this);
  pResult = 0;
} // OnRClickImages( pNMHDR, pResult )


/**
 * Message: NM_RCLICK
 *
 *  @param pNMHDR       Notification msg structure
 *  @param pResult      result
*/
void ReportColonFindings::OnRClickEntries(NMHDR* pNMHDR, LRESULT* pResult)
{
  // Do not show menu when there are no items
  if (m_wndEntries.GetItemCount() <= 0)
  {
    return;
  }

  // get point at time of message
  DWORD dwPos = GetMessagePos();
  CPoint pt(LOWORD(dwPos), HIWORD(dwPos));

  // load popup menu
  CMenu menu;
  VERIFY(menu.LoadMenu(IDM_REPORT_POPUPS));

  CMenu* pMenu = menu.GetSubMenu(0);

  // convert point
  CPoint ptHitTest(pt) ;
  m_wndEntries.ScreenToClient(&ptHitTest);

  // where did we hit?
  int4 iItem = m_wndEntries.HitTest(ptHitTest);

  // if the cursor is not over an entry disable entry-specific menu items
  if (iItem < 0)
  {
    pMenu->EnableMenuItem(IDM_RPT_DELETE_ENTRY, MF_BYCOMMAND|MF_GRAYED|MF_DISABLED);
    pMenu->EnableMenuItem(IDM_RPT_DISTANCE_UNCERTAIN, MF_BYCOMMAND|MF_GRAYED|MF_DISABLED);
    pMenu->EnableMenuItem(IDM_RPT_JUMPTOFINDINGVIEW, MF_BYCOMMAND|MF_GRAYED|MF_DISABLED);
  }
  else
  {
    int4 iItem = m_wndEntries.GetNextItem(-1, LVNI_SELECTED);
    EntryListData* pld = reinterpret_cast<EntryListData*>(m_wndEntries.GetItemData(iItem));
    ReportLib::IReportEntryPtr spEntry = m_spRevision->Entries->GetItem(static_cast<LPCTSTR>(pld->m_sEntryID));

    // Distance uncertain if entry name starts with '-'
    std::string strName = spEntry->Name;
    pMenu->CheckMenuItem(IDM_RPT_DISTANCE_UNCERTAIN, (strName[0] == '-') ? MF_CHECKED : MF_UNCHECKED);
  }

  // show menu and track
  pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON, pt.x, pt.y, this);
  *pResult = 0;
} // OnRClickEntries( pNMHDR, pResult )


/**
 * Message: IDM_RPT_DELETE_ALL_FINDINGS
 */
void ReportColonFindings::OnRptDeleteAllFindings()
{
  // verify that the user wants to delete all the entries
  CString s;
  s.Format(IDS_VERIFY_USER_OPERATION_ALL_FORMAT, LoadResourceString(IDS_DELETE), LoadResourceString(IDS_COLON_FINDINGS));
  if(AfxMessageBox(s, MB_ICONQUESTION|MB_YESNO) == IDNO)
  {
    return;
  }

  int4 iNumEntries = m_spRevision->Entries->Count;
  // for each entry
  for(int4 iEntry(0); iEntry < iNumEntries; iEntry++)
  {
		// remove the associated other finding if it exists
		_bstr_t sOtherID = m_spRevision->Entries->GetItem(0L)->OtherFindingID;
		if ( sOtherID.length() > 0 )
		{
		  try
		  {
			  m_spRevision->OtherFindings->Remove(sOtherID);
		  }
		  catch(_com_error&)
		  {
			  LogErr("Add image throws an exception", "ReportColonFindings", "OnRptDeleteAllFindings");
			  return;
		  }
		} // if

    // every time you delete entries collection is resorted, just remove the first item every time
    m_spRevision->Entries->Remove(0L);
  }

  // reset the form
  m_sCurrentEntry = _T("");
  ResetForm();
} // OnRptDeleteAllFindings()


/**
 * Message: IDM_RPT_JUMPTOFINDINGVIEW
 */
void ReportColonFindings::OnRptJumpToFindingView()
{
  int4 iItem = m_wndEntries.GetNextItem(-1, LVNI_SELECTED);
  if(iItem == -1)
  {
    return; // nothing selected
  }

  // Get entry data
  EntryListData* pld = reinterpret_cast<EntryListData*>(m_wndEntries.GetItemData(iItem));
  ReportLib::IReportEntryPtr spEntry = m_spRevision->Entries->GetItem(static_cast<LPCTSTR>(pld->m_sEntryID));
  const std::string sName = spEntry->Name;
  ReportLib::DatasetPositionEnumProp eStudyType = ReportColonFindings::MapFindingNameToDatasetPosition(sName);

  if ((spEntry->OrigDistanceToRectum == 0.0f) || (eStudyType == -1))
  {
    // if no distance to rectum value recorded, then the camera coordinates were not saved (i.e. from an older report version)
    AfxMessageBox(LoadResourceString(IDS_FINDING_LOC_UNKNOWN));
    return;
  }

  // get camera coordinates for entry
  ReportVariant dist;
  dist.Attach(spEntry->CameraCoordinates.Detach());
  Point<float4> pos; Normal3D<float4> orientation; Normal3D<float4> up;
  dist.asCameraCoordinates(pos, orientation, up);

  // Set the current dataset
  AfxGetMainWnd()->SendMessage(WM_APP_SET_MAIN_DATASET, WPARAM((eStudyType == ReportLib::Supine) ? DatasetOrientations::eSUPINE : DatasetOrientations::ePRONE), 0);

  // TODO: Check if the saved position is valid among the current segment list selections

  // Set the camera position
  Dataset& dataset = (eStudyType == ReportLib::Supine) ? rdrGlobal.m_supine : rdrGlobal.m_prone;
  dataset.SelectSegment(pos);
  dataset.m_submarine.SetPositionWorld(dataset.m_fields.m_volumeRender.GetVolumeToWorldMatrix() * pos);
//  dataset.m_submarine.SetOrientation(orientation);
//  dataset.m_submarine.SetUpDirection(up);
  dataset.m_submarine.UpdatePosition();
  dataset.m_fields.m_volumeRender.FlushAccumulationBuffer();

  // jump to the navigation view
  AfxGetMainWnd()->PostMessage(WM_APP_VIEW_SELECTION_CHANGED, WM_APP_VIEWSEL_NAVIGATION, 0);
} // OnRptJumpToFindingView()


/**
 * Message: IDM_RPT_DELETE_ENTRY
 */
void ReportColonFindings::OnRptDeleteEntry()
{
  // get the selected entry
  const int4 iItem = m_wndEntries.GetNextItem(-1, LVNI_SELECTED);
  if(iItem == -1)
  {
    return; // nothing selected
  }

  // verify that the user wants to delete the selected entry
  CString str;
  str.Format(IDS_VERIFY_USER_OPERATION_FORMAT, LoadResourceString(IDS_DELETE), LoadResourceString(IDS_FINDING));
  if (AfxMessageBox(str, MB_YESNO|MB_ICONQUESTION) == IDNO)
  {
    return;
  }

  // get the entry's associated data
  EntryListData* pld = reinterpret_cast<EntryListData*>(m_wndEntries.GetItemData(iItem));

  // delete it
  DeleteEntry(pld->m_sEntryID);

  // select the first item in the list control
  m_wndEntries.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, 0x000F);
} // OnRptDeleteEntry()


/**
 * Message: IDM_RPT_DISTANCE_UNCERTAIN
 */
void ReportColonFindings::OnRptDistanceUncertain()
{
  // get the selected entry
  int4 iItem = m_wndEntries.GetNextItem(-1, LVNI_SELECTED);
  if(iItem == -1)
  {
    return; // nothing selected
  }

  try
  {
    EntryListData* pld = reinterpret_cast<EntryListData*>(m_wndEntries.GetItemData(iItem));
    ReportLib::IReportEntryPtr spEntry = m_spRevision->Entries->GetItem(static_cast<LPCTSTR>(pld->m_sEntryID));

    std::string strName(spEntry->Name);
    std::string strDist;
    bool bUndeterminedDistance = false;
    if (strName[0] == '-')
    {
      strName.erase(0,1);
      strDist = strName;
      spEntry->Name = strName.c_str();
      bUndeterminedDistance = false;
    }
    else
    {
      strDist = " " + LoadResourceStringWithUpperHead(IDS_COLONFINDING_DISTANCE_UNCERTAIN);
      strName.insert(0, "-");
      spEntry->Name = strName.c_str();
      bUndeterminedDistance = true;
    }

    // remove the old entry from the list control.
    m_wndEntries.DeleteItem(iItem);

    // is it a supine, prone, or indeterminate (old version)?
    int4 iImageNum;
    std::string strIcon(spEntry->Name);
    // if ( strIcon.find("(Supine)") != std::string::npos )
    if ( strIcon.find(LoadResourceString(IDS_SUPINE_EXTRA)) != std::string::npos )
    {
      if (!bUndeterminedDistance) strDist = strIcon.substr(strlen(LoadResourceString(IDS_SUPINE_EXTRA)));
      iImageNum = 1;  // supine.
    }
    // else if ( strIcon.find("(Prone)") != std::string::npos )
    else if ( strIcon.find(LoadResourceString(IDS_PRONE_EXTRA)) != std::string::npos )
    {
      if (!bUndeterminedDistance) strDist = strIcon.substr(strlen(LoadResourceString(IDS_PRONE_EXTRA)));
      iImageNum = 2;  // prone.
    }
    else // old version: indeterminate.
    {
      if (!bUndeterminedDistance) strDist = strIcon;
      iImageNum = 0;  // indeterminate: for old version.
    }

    EntryListData* pid = new EntryListData;
    pid->m_sEntryID = static_cast<LPCTSTR>(spEntry->ID);

    // add new entry to the list control.
    LVITEM lvi;
    lvi.mask     = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
    lvi.iItem    = iItem; // insert at current position.
    lvi.iSubItem = 0;
    lvi.iImage   = iImageNum; // first image indes
    lvi.pszText  = (LPTSTR)(LPCTSTR)strDist.c_str();
    lvi.lParam  = reinterpret_cast<LPARAM>(pid);

    // insert the item
    m_wndEntries.InsertItem(&lvi);

    // set up new 'Entry Name'
    {
      std::string str(spEntry->Name);
      if (str[0] == '-')
      {
        std::string sTitle;
        const std::string vsSearch[2]
          = { LPCTSTR('('+LoadResourceStringWithUpperHead(IDS_SUPINE)+')'), LPCTSTR('('+LoadResourceStringWithUpperHead(IDS_PRONE)+')') };
        if (str.find(vsSearch[0]) != std::string::npos)
        {
          sTitle = LoadResourceString(IDS_SUPINE_EXTRA) + " " + LowerCaseString(IDS_COLONFINDING_DISTANCE_UNCERTAIN);
        }
        else if (str.find(vsSearch[1]) != std::string::npos)
        {
          sTitle = (LoadResourceString(IDS_PRONE_EXTRA) + " " + LowerCaseString(IDS_COLONFINDING_DISTANCE_UNCERTAIN));
        }
        else
        {
          sTitle = LoadResourceStringWithUpperHead(IDS_COLONFINDING_DISTANCE_UNCERTAIN);
        }

        SetDlgItemText(IDC_ENTRY_NAME, sTitle.c_str());
      }
      else
      {
        SetDlgItemText(IDC_ENTRY_NAME, spEntry->Name);
      }
    }
  }
  catch(_com_error& e)
  {
	  LogErr("OnRptDistanceUncertain() throws an exception", "ReportColonFindings", "OnRptDistanceUncertain");

    DisplayReportError(IDS_DELETE_COLON_FINDING, e.Error());
  }
  catch( ... )
  {
    AfxMessageBox( IDS_ERROR_UNSPECIFIED, MB_ICONSTOP|MB_OK);
    LogErr( "Unhandled exception caught.", "ReportColonFindings", "OnRptDistanceUncertain" );
  }

} // OnRptDistanceUncertain()


/**
 * Message: IDM_RPT_DELETE_IMAGE
 */
void ReportColonFindings::OnRptDeleteImage()
{
  // get the select image
  int4 iItem = m_wndImages.GetNextItem(-1, LVNI_SELECTED);
  if(iItem == -1)
  {
    return; // nothing selected
  }

  // verify that the user wants to delete the selected image
  CString str;
  str.Format(IDS_VERIFY_USER_OPERATION_FORMAT, LoadResourceString(IDS_DELETE), LoadResourceString(IDS_IMAGE));
  if(AfxMessageBox(str, MB_YESNO|MB_ICONQUESTION) == IDNO)
  {
    return;
  }

  // get the image's associated data
  ImageListData* pld = reinterpret_cast<ImageListData*>(m_wndImages.GetItemData(iItem));

  // delete it
  DeleteImage(pld->m_sImageID);
} // OnRptDeleteImage()


/**
 * Message: IDC_FINDING_NORMAL, Entry identified as a normal finding.
 */
void ReportColonFindings::OnFindingNormal()
{
  SetFinding(ReportLib::Other_Normal);
  if (m_bExtraColonic)
  {
    ClearImages();
    AddImages(false);
    m_bExtraColonic = false;
  }
} // OnFindingNormal()


/**
 * Message: IDC_FINDING_POLYP, Entry identified as a colon finding.
 */
void ReportColonFindings::OnFindingColonicPolyp()
{
  SetFinding(ReportLib::Polyp);
  if (m_bExtraColonic)
  {
    ClearImages();
    AddImages(false);
    m_bExtraColonic = false;
  }
} // OnFindingColonicPolyp()


/**
 * Message: IDC_FINDING_DIVERTICULUM, Entry identified as a diverticulum colon finding.
 */
void ReportColonFindings::OnFindingColonicDivertculum()
{
  SetFinding(ReportLib::Diverticulum);
  if (m_bExtraColonic)
  {
    ClearImages();
    AddImages(false);
    m_bExtraColonic = false;
  }
} //OnFindingColonicDivertculum()


/**
 * Message: IDC_FINDING_STENOSIS, Entry identified as a stenosis finding.
 */
void ReportColonFindings::OnFindingColonicStenosis()
{
  SetFinding(ReportLib::Diverticulum);
  if (m_bExtraColonic)
  {
    ClearImages();
    AddImages(false);
    m_bExtraColonic = false;
  }
} // OnFindingColonicStenosis()


/**
 * Message: IDC_FINDING_OTHER, Entry identified as a nondescript colon finding.
 */
void ReportColonFindings::OnFindingColonicOther()
{
  SetFinding(ReportLib::Other_Colonic, ColonReport::ReportColonFindings::COLONIC_OTHER);
  if (m_bExtraColonic)
  {
    ClearImages();
    AddImages(false);
    m_bExtraColonic = false;
  }
} // OnFindingColonicOther()


/**
 * Message: IDC_FINDING_EXTRA_COLONIC, Entry identified as an extra-colonic finding.
 */
void ReportColonFindings::OnFindingExtraColonic()
{
  SetFinding(ReportLib::Other_Extracolonic, ColonReport::ReportColonFindings::COLONIC_EXTRA);
  if (!m_bExtraColonic)
  {
    ClearImages();
    AddImages(true);
    m_bExtraColonic = true;
  }
} // OnFindingExtraColonic()


/////////////////////////////////////////////////////////
// Note: The following code is a work around on JIRA issue #VC-1898
// The exception is when an extra-colonic finding was changed into a
// colonic or normal finding, the previous CRADs E-catagory may still
// display in the report. That needs to be fixed with more effort. 
// 10-19-2010 DQ

const char ReportColonFindings::m_sHiddenTextDelimiter[3] = { 13,13,0 };
const char ReportColonFindings::m_sHiddenTextNewline[3] = { 13,0 };


/**
 * called when view closed
 **/
void ReportColonFindings::OnReportViewUnactivate()
{
  OnEntryChanging(m_sCurrentEntry);  // This line is always needed.

  try
  {
    if( m_spRevision == NULL || m_spRevision->IsOpen == VARIANT_FALSE || m_spRevision->IsReadOnly == VARIANT_TRUE)
      return;

    ReportLib::IReportInfoPtr spInfo = m_spRevision->Info;
    
    CString sOtherRecmdtn = spInfo->OtherRecommendation.length()>0 ? static_cast<LPCTSTR>(spInfo->OtherRecommendation) : CString();
    // KLUDGE: Strip out C-RADS and XC-C-RADS
    int4 iCharRcm = sOtherRecmdtn.Find(m_sHiddenTextDelimiter);
    if (iCharRcm >= 0) sOtherRecmdtn = sOtherRecmdtn.Left(iCharRcm);

    spInfo->OtherRecommendation = static_cast<LPCTSTR>(sOtherRecmdtn);
  }
  catch(_com_error& e)
  {
    DisplayReportError(IDS_RETRIEVE_STUDY_INFORMATION, e.Error());
  }

} // OnReportViewUnactivate()





#undef FILE_REVISION


// Revision History:
// $Log: ReportColonFindings.C,v $
// Revision 1.20.2.28  2010/10/26 20:05:03  dongqing
// Fix the OtherFinding list did not update after user change some
// entry from Extra colonic to colonic findings
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.20.2.27  2010/10/19 20:06:51  dongqing
// E-catagory change for extra-colonic finding display no longer needs
// to visit Patient Info pane for updating.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.20.2.26  2010/03/30 07:49:54  dongqing
// prevent dialog from disappearing
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.20.2.25  2010/02/05 15:19:10  dongqing
// update the hidden extra-colonic text when an extra-colonic entry changes
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.20.2.24  2009/08/24 22:02:00  dongqing
// Since Gerg finds out the correct fix to the report libray for Microsoft ATL
// update, we need to roll back Dongqing's temporary fix.
//
// Revision 1.20.2.23  2009/08/19 15:32:25  vxconfig
// Roll back the exception handle for last time _bstr_t null check
// Last time change does help anything
//
// Revision 1.20.2.22  2009/08/14 19:21:05  dongqing
// add extracColonic finding initialization for colonic finding. This should fix
// the null when delete colonic finding since it checks the extracolonic finding
// property
//
// Revision 1.20.2.21  2009/08/14 14:41:08  vxconfig
// Fix the _bstr_t exception throw problem in suboptimal way
// Need to revisit later with better knowledge
//
// Revision 1.20.2.20  2009/08/13 20:33:51  dongqing
// Add log info for all COM exception
//
// Revision 1.20.2.19  2009/08/13 18:43:32  dongqing
// add necessary initialization for extra-colonic finding. It could not be NULL
//
// Revision 1.20.2.18  2009/07/30 17:58:19  kchalupa
// Code Walkthrough
//
// Revision 1.20.2.17  2009/02/13 22:44:06  jmeade
// Issue 6250: added to hack code for determining dataset position from finding name.
//
// Revision 1.20.2.16  2008/11/18 01:00:34  jmeade
// report entries with no original distance to rectum value should read as indeterminate distance to anal verge.
//
// Revision 1.20.2.15  2008/07/01 19:43:12  jmeade
// hack to fix reading of translated report findings entry title text
//
// Revision 1.20.2.14  2008/05/14 01:29:50  jmeade
// french version.
//
// Revision 1.20.2.13  2008/04/03 00:54:46  jmeade
// Issue 6059: corrected show/hide on perpendicular editfield.
//
// Revision 1.20.2.12  2008/03/14 01:16:43  jmeade
// Issue 6023: allow the 1st findings description entry to be set when a user is changing organ description.
//
// Revision 1.20.2.11  2008/03/12 20:37:19  jmeade
// Issue 6028: Don't store overall extra-colonic c-rads category; that is begging for an out-of-date value.  Compute when needed.
//
// Revision 1.20.2.10  2008/03/07 20:01:43  jmeade
// Issue 6020: different prompt for single-image move (report entries).
//
// Revision 1.20.2.9  2008/02/16 02:33:39  jmeade
// Issue 5924: set a child-less xc finding to miscellaneous.
//
// Revision 1.20.2.8  2008/02/15 01:45:23  jmeade
// Issue 5931: prompt user to merge images or entire findings.
//
// Revision 1.20.2.7  2008/02/14 10:22:32  jmeade
// Issue 5924: delete each associated otherfinding when in loop for deleting all
//
// Revision 1.20.2.6  2008/02/14 09:51:56  jmeade
// Issue 5924: save other finding info when necessary.
//
// Revision 1.20.2.5  2008/02/14 06:54:30  jmeade
// Issue 5924: handle xc-finding delete; identify xc findings properly; create parents for orphaned otherfindings.
//
// Revision 1.20.2.4  2008/02/12 23:25:11  jmeade
// Issue 5924: save other findings info for xc findings.
//
// Revision 1.20.2.3  2008/02/12 20:32:25  jmeade
// Issue 5924: adding other-findings info to extra-colonic colon findings.
//
// Revision 1.20.2.2  2007/04/27 02:19:34  jmeade
// Issue 5608: one overall extra-colonic c-rads category in the miscellaneous view.
//
// Revision 1.20.2.1  2007/03/27 22:27:24  jmeade
// Issue 5502: reflect changes made in colon viewer report into the console.
//
// Revision 1.20  2007/03/10 05:40:52  jmeade
// code standards.
//
// Revision 1.19  2007/02/15 18:35:48  jmeade
// Issue 5411: check for valide entries before accessing in SetFinding.
//
// Revision 1.18  2007/02/09 04:56:06  jmeade
// Issue 5219: Implementing C-RADS: optional (near) flexure field.
//
// Revision 1.17  2007/02/09 04:37:11  jmeade
// Issue 5219: Implementing C-RADS: perpendicular field.
//
// Revision 1.16  2007/02/09 00:11:48  jmeade
// Issue 5219: Implementing C-RADS: morphology, other findings E0-E4, failed colonoscopy indication.
//
// Revision 1.15  2006/10/04 23:10:59  jmeade
// enum warnings.
//
// Revision 1.14  2006/09/22 21:50:29  jmeade
// Issue 4946: display of decubitus patient positions in report.
//
// Revision 1.13  2006/08/25 21:14:18  jmeade
// Issue 4946: strings for decubitus studies.
//
// Revision 1.12  2006/06/01 20:22:08  frank
// updated to visual studio 2005
//
// Revision 1.11  2006/02/07 21:40:28  dongqing
// code walk through
//
// Revision 1.10  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.9.2.1  2006/01/12 19:31:37  geconomos
// issue# 4600: corrected dragging images when extra colonic
//
// Revision 1.9  2005/10/28 17:29:18  geconomos
// Issue# 4493: Unable to move last image if finding is extra colonic
//
// Revision 1.8  2005/10/24 20:33:12  jmeade
// Issue 4470: error loading in search strings.
//
// Revision 1.7  2005/09/13 17:21:07  jmeade
// ColonReport namespace.
//
// Revision 1.6  2005/09/09 15:21:22  geconomos
// Issue# 4424: ability to delete first image when finding is extra colonic
//
// Revision 1.5  2005/09/08 12:38:44  geconomos
// Issue# 4424: Unable to delete first image if finding is "extra colonic".
//
// Revision 1.4  2005/08/23 16:31:45  geconomos
// Corrected merge from vx repository.
//
// Revision 1.3  2005/08/19 13:00:09  vxconfig
// merged incremental changes from vx VC_1-6 to v3d
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.16.2.9.8.1  2005/06/29 00:08:01  jmeade
// Issue 4180: Skip overview image for extra-colonic findings.
//
// Revision 3.16.2.9  2005/02/15 19:46:33  jmeade
// merge from vc1.4 branch.
//
// Revision 3.16.2.8.2.2  2004/10/22 02:57:17  jmeade
// jump to colon finding view implemented.
//
// Revision 3.16.2.8.2.1  2004/10/22 02:41:54  jmeade
// jump to colon finding view implemented.
//
// Revision 3.16.2.8  2004/08/11 16:46:31  geconomos
// Report images are always at least 512x512
//
// Revision 3.16.2.7  2004/07/17 02:18:43  jmeade
// Method to search a finding name to determine whether the finding is a supine or prone study finding.
//
// Revision 3.16.2.6  2004/06/17 18:42:29  vxconfig
// static cast on library enum.
//
// Revision 3.16.2.5  2004/03/30 23:45:46  jmeade
// Issue 3590: Fix bug where more than one finding type could be highlighted.
//
// Revision 3.16.2.4  2004/03/27 00:38:42  jmeade
// Issue 3590: New shape classifications; replaced for location radios with ReportGroupCtrl.
//
// Revision 3.16.2.3  2003/04/14 16:47:12  jmeade
// Replaced hard-coded button vector.
//
// Revision 3.16.2.2  2003/04/09 22:01:36  jmeade
// Fixed a faulty string construction; Comments, coding standards.
//
// Revision 3.16.2.1  2003/03/07 04:11:33  jmeade
// Issue 2886:  Replace unused delete format strings.
//
// Revision 3.16  2003/01/21 19:41:13  jmeade
// Duplicate: Revision : 3.10.2.3: Removed hard-coded text.
//
// Revision 3.15  2002/09/09 19:26:05  jmeade
// Merged with XP_1-0 branch (tag: XP_1-0_Joined_On_09-06-2002)
//
// Revision 3.14.2.1  2002/08/05 15:55:38  jmeade
// Tabs to spaces.
//
// Revision 3.14  2002/04/26 15:57:31  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.13  2002/03/28 18:57:52  jmeade
// Spelling errors; More strings to string table.
//
// Revision 3.12  2002/03/13 22:01:41  mkontak
// Merge VC 1.2/CS 1.0
//
// Revision 3.11  2002/02/04 20:18:14  binli
// Improved image quality in report file
//
// Revision 3.10.2.2  2002/04/03 01:04:25  jmeade
// Catch crash-causing exception when no image and Extra-Colonic is selected.
//
// Revision 3.10.2.1  2002/02/11 16:37:19  binli
// Improved image quality in report file
//
// Revision 3.10  2002/01/17 18:55:38  binli
// 1> adjustment of captions
// 2> hardcoding of some const strings.
//
// Revision 3.9  2002/01/15 20:49:06  binli
// issue 1424: option to show/not show distance in report
//
// Revision 3.8  2002/01/09 22:06:45  binli
// Report adjustment: separate display of Non-Extra colon findings & Extra colon findings.
//
// Revision 3.7  2002/01/07 21:43:46  binli
// Report adjustment: no display of overview imge in ExtraColonicFinding
//
// Revision 3.6  2002/01/03 18:57:21  binli
// Report adjustment: locations on selection of extra-colonic and normal colonic
//
// Revision 3.5  2002/01/02 22:26:48  binli
// Report adjustment: added 'Extra-Colonic'
//
// Revision 3.4  2002/01/02 15:32:58  binli
// Adjustment: Added Other_Location/Clear_Location
//
// Revision 3.3  2001/12/26 20:51:38  binli
// adjustment of Report.
//
// Revision 3.2  2001/11/09 01:17:30  jmeade
// Merge with 1.1 - 2001-11-08
//
// Revision 3.1.2.2  2001/11/05 22:07:51  jmeade
// Coding standards.
//
// Revision 3.1.2.1  2001/11/04 05:13:00  jmeade
// Issue 1287, Item 4; Code standards.
//
// Revision 3.1  2001/10/18 18:12:47  binli
// issue 1208: read Max/Perpendicular from spEntry even though no polyp
//
// Revision 3.0  2001/10/14 23:01:59  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.1   Sep 21 2001 11:04:20   jmeade
// Popup menu naming
// 
//    Rev 2.0   Sep 16 2001 23:41:40   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:30   ingmar
// Initial revision.
// Revision 1.24  2001/09/10 22:39:21  binli
// track ID: 000672: added (Supine)/(Prone) to caption in Colon-Findings and NonDistendedAreas.
//
// Revision 1.23  2001/09/04 18:02:20  binli
// task 173 (track ID 000485): added new option: report inderterminate translucent feature or not.
//
// Revision 1.22  2001/08/22 21:34:12  binli
// bug pt1001: colonfinds, default is "Normal", not "Polyp"
//
// Revision 1.21  2001/05/30 14:23:21  jmeade
// coding standards to fix name clashes
//
// Revision 1.20  2001/05/10 21:04:38  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.19  2001/04/18 15:49:32  geconomos
// coding standards
//
// Revision 1.18  2001/04/13 02:34:11  jmeade
// headers and footers
//
// Revision 1.17  2001/03/30 18:12:50  geconomos
// Added ability to delete all colon findings.
//
// Revision 1.16  2001/03/07 20:04:11  geconomos
// Changed OtherColonFinding to Normal
//
// Revision 1.15  2001/02/26 22:00:08  geconomos
// Added Other to ColonFinding enum
//
// Revision 1.14  2001/02/07 16:25:03  geconomos
// Added revisions to report file
//
// Revision 1.13  2001/02/05 14:32:12  geconomos
// Implementation of ReportVariant
//
// Revision 1.12  2001/02/02 21:11:09  jeff
// stenosis misspelled
//
// Revision 1.11  2001/02/02 07:33:16  jeff
// Moving strings to resource table
//
// Revision 1.10  2001/02/01 16:35:21  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ReportColonFindings.C,v 1.20.2.28 2010/10/26 20:05:03 dongqing Exp $
// $Id: ReportColonFindings.C,v 1.20.2.28 2010/10/26 20:05:03 dongqing Exp $
