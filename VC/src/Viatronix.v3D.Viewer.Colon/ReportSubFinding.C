// $Id: ReportSubFinding.C,v 1.6.2.4 2010/02/05 15:19:11 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Description: Work area for other findings
// Author: George Economos

#include "stdafx.h"
#include "viatron.h"
#include "ReportSubFinding.h"
#include "resource.h"
#include "ReportMiscellaneous.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int INCREMENT = 20;

using namespace WinUtils;
using namespace ColonReport;

/**
 * Constructor
 *
 * @param pParent   parent window
 */
ReportSubFinding::ReportSubFinding(CWnd* pParent) : ReportWndBase(0, pParent)
{
	//{{AFX_DATA_INIT(ReportSubFinding)
	//}}AFX_DATA_INIT
} // ReportSubFinding( pParent )


/**
 * Dialog data variable initialization.
 *
 *  @param pDX   Dialog data exchange instance.
 */
void ReportSubFinding::DoDataExchange(CDataExchange* pDX)
{
	ReportWndBase::DoDataExchange(pDX);
} // DoDataExchange( pDX )


BEGIN_MESSAGE_MAP(ReportSubFinding, ReportWndBase)
END_MESSAGE_MAP()


/**
* Init dialog procedure
* 
* @return   Create success
*/
BOOL ReportSubFinding::OnInitDialog() 
{
  ReportWndBase::OnInitDialog();
  
  return TRUE;
} // OnInitDialog()


/**
 * animates sliding window
 *
 *  @param ptFinal   final location of the window
 *  @param pWnd      window to slide
 */
void ReportSubFinding::SlideOut(const CPoint& ptFinal, CWnd* pWnd)
{
  CRect rcRegion;
  GetClientRect(rcRegion);

  CPoint ptBegin;
  ptBegin.x = ptFinal.x - rcRegion.Width();
  ptBegin.y = ptFinal.y;   

  rcRegion.left = rcRegion.right;

  ptBegin.x += INCREMENT;
  rcRegion.left -= INCREMENT;
  
  while(ptBegin.x < ptFinal.x)
  {
    
    CRgn rgn;
    rgn.CreateRectRgn(rcRegion.left, rcRegion.top, rcRegion.right, rcRegion.bottom);
    
    SetWindowRgn(reinterpret_cast<HRGN>(rgn.Detach()), TRUE); 
    
    SetWindowPos(pWnd, ptBegin.x, ptFinal.y, 0, 0, SWP_SHOWWINDOW|SWP_NOACTIVATE|SWP_NOSIZE); 

    ptBegin.x += INCREMENT;
    rcRegion.left -= INCREMENT;
    
    RedrawWindow();
    
    Sleep(10);
  }  
} // SlideOut( ptFinal, pWnd )


/**
 * animates sliding window
 *
 *  @param ptFinal   final location of the window
 *  @param pWnd      window to slide
 */
void ReportSubFinding::SlideIn(const CPoint& ptFinal, CWnd* pWnd)
{
  CRect rcRegion;
  GetClientRect(rcRegion);

  CRect rcParent;
  pWnd->GetWindowRect(rcParent);
  GetParent()->ScreenToClient(rcParent);  
  
  CRect rcSlider;
  GetWindowRect(rcSlider);
  GetParent()->ScreenToClient(rcSlider);
  
  CPoint ptBegin;
  ptBegin.x = rcSlider.left;
  ptBegin.y = rcSlider.top;
  
  ptBegin.x -= INCREMENT;
  rcRegion.left += INCREMENT;
  rcSlider.left += INCREMENT;
  
  while(ptBegin.x > ptFinal.x)
  {
    CRgn rgn;
    rgn.CreateRectRgn(rcRegion.left, rcRegion.top, rcRegion.right, rcRegion.bottom);
    
    SetWindowRgn(reinterpret_cast<HRGN>(rgn.Detach()), FALSE); 
   
    SetWindowPos(pWnd, ptBegin.x, ptFinal.y, 0, 0, SWP_SHOWWINDOW|SWP_NOACTIVATE|SWP_NOSIZE); 

    ptBegin.x -= INCREMENT;
    rcRegion.left += INCREMENT;    
    
    Sleep(10);
  }  
} // SlideIn( ptFinal, pWnd )


/**
 * Called when a subview is being closed
 **/
void ReportSubFinding::OnReportViewUnactivate()
{
  try
  {
    switch(m_spFinding->Finding)
    {
      case ReportLib::Lung_Nodule:
      case ReportLib::Lung_Granuloma:
      case ReportLib::Liver_Mass:
      case ReportLib::Liver_Cyst:
      case ReportLib::Adrenal_Mass:
      case ReportLib::Renal_Mass:
      case ReportLib::Renal_Cyst:
      case ReportLib::Renal_Stones:
      case ReportLib::Spleen_Mass:
      case ReportLib::Spleen_Granuloma:
      {
        m_spFinding->Location = m_wndGroupLocation.GetSelectedItem();
        m_spFinding->MaximumSize = m_wndMaximumSize.GetValue();
        m_spFinding->PerpendicularSize = m_wndPerpendicularSize.GetValue();
        break;
      }
      case ReportLib::Renal_Scar:
      case ReportLib::GI_InguinalHernia:
      {
        m_spFinding->Location = m_wndGroupLocation.GetSelectedItem();
        break;
      }
      case ReportLib::Renal_Hydronephrosis:
      {
        m_spFinding->Location = m_wndGroupLocation.GetSelectedItem();
        m_spFinding->Degree = m_wndGroupLevel.GetSelectedItem();
        break;
      }  
      case ReportLib::Lung_Pneomuthorax:
      {
        m_spFinding->Location = m_wndGroupLocation.GetSelectedItem();
        m_spFinding->Size = m_wndGroupSize.GetSelectedItem();
        break;
      }
      case ReportLib::Vascular_AAA:
      {
        m_spFinding->Location = m_wndGroupLocation.GetSelectedItem();
        m_spFinding->Length = m_wndLength.GetValue();
        m_spFinding->MaximumSize = m_wndMaximumSize.GetValue();
        m_spFinding->PerpendicularSize = m_wndPerpendicularSize.GetValue();
        break;
      }  
      case ReportLib::Vascular_ArteralCalcification:
      {
        m_spFinding->Degree = m_wndGroupLevel.GetSelectedItem();
        break;
      }
      default:
      {
        break;
      }
    }

    CString s;
    GetDlgItemText(IDC_COMMENTS, s);
    m_spFinding->Comment = static_cast<LPCTSTR>(s);

    m_spFinding->CRADSCategory = m_wndComboCRADS.GetCurSel();

    ReportMiscellaneous::UpdateKludgedExtraColonicCRadsText(m_spRevision);
  }
  catch(_com_error& e)
  {
    DisplayReportError(IDS_UPDATE_FINDING, e.Error());
  }
} // OnReportViewUnactivate()


/**
 * Initializes the different finding categories
 *
 * @param spFinding   finding for which to initial display
 */
void ReportSubFinding::SetFinding(ReportLib::IReportOtherFindingPtr& spFinding)
{
  try
  {
    m_spFinding = spFinding;
    switch(m_spFinding->Finding)
    {
      case ReportLib::Lung_Nodule:
      case ReportLib::Lung_Granuloma:
      {
        OnInitLung1();
        break;
      }
      case ReportLib::Lung_Pneomuthorax:
      {
        OnInitLung2();
        break;
      }
      case ReportLib::Liver_Mass:
      case ReportLib::Liver_Cyst:
      {
        OnInitLiver1();
        break;
      }
      case ReportLib::Adrenal_Mass:
      {
        OnInitAdrenal1();
        break;
      }
      case ReportLib::Renal_Mass:
      case ReportLib::Renal_Cyst:
      case ReportLib::Renal_Stones:
      {
        OnInitRenal1();
        break;
      }
      case ReportLib::Renal_Scar:
      {
        OnInitRenal2();
        break;
      }
      case ReportLib::Renal_Hydronephrosis:
      {
        OnInitRenal3();
        break;
      }
      case ReportLib::Vascular_AAA:
      {
        OnInitVascular1();
        break;
      }
      case ReportLib::Vascular_ArteralCalcification:
      {
        OnInitVascular2();
        break;
      }
      case ReportLib::Spleen_Mass:
      case ReportLib::Spleen_Granuloma:
      {
        OnInitSpleen1();
        break;
      }
      case ReportLib::GI_InguinalHernia:
      {
        OnInitGI1();
        break;
      }
      default:
      {
        break;
      }
    }

    m_wndComboCRADS.SubclassDlgItem(IDC_RPT_COMBO_CRADSCAT, this);
    for (DWORD iCat(0); iCat <= 4; iCat++)
    {
      m_wndComboCRADS.AddString(("E"+ToAscii(iCat)).c_str());
    }
    m_wndComboCRADS.SetCurSel(m_spFinding->CRADSCategory);

    SetDlgItemText(IDC_COMMENTS, m_spFinding->Comment);
  }
  catch(_com_error& e)
  {
    DisplayReportError(IDS_RETRIEVE_FINDING, e.Error());
  }
} // SetFinding( spFinding )


/**
 * Initializes sub-dialog
 */
void ReportSubFinding::OnInitLung1()
{
  CreateSpinEdit(IDC_PERP_SIZE, m_wndPerpendicularSize, 0.0f, 999.0f,  0.0f, 0.1f, 1);
  CreateSpinEdit(IDC_MAXIMUM_SIZE, m_wndMaximumSize, 0.0f, 999.0f,  0.0f, 0.1f, 1);

  m_wndGroupLocation.SubclassDlgItem(IDC_LIST_LOCATION, this);
  m_wndGroupLocation.SetSelectedBold(true);
  m_wndGroupLocation.SetBackgroundImage(GetChildBackground(m_wndGroupLocation.GetDlgCtrlID()));
  
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_RIGHT_UPPER_LOBE),
		static_cast<DWORD>(ReportLib::RightLung_UpperLobe));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_RIGHT_MIDDLE_LOBE),
		static_cast<DWORD>(ReportLib::RightLung_MiddleLobe));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_RIGHT_LOWER_LOBE),
		static_cast<DWORD>(ReportLib::RightLung_LowerLobe));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_LEFT_UPPER_LOBE),
		static_cast<DWORD>(ReportLib::LeftLung_UpperLobe));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_LEFT_LOWER_LOBE),
		static_cast<DWORD>(ReportLib::LeftLung_LowerLobe));

  m_wndGroupLocation.SelectItem(m_spFinding->Location); 
  m_wndMaximumSize.SetValue(m_spFinding->MaximumSize);

  m_wndPerpendicularSize.SetValue(m_spFinding->PerpendicularSize);
  m_wndPerpendicularSize.ShowWindow(m_spFinding->PerpendicularSize > 0.0f);
} // OnInitLung1()


/**
 * Initializes sub-dialog
 */
void ReportSubFinding::OnInitLung2()
{
  m_wndGroupLocation.SubclassDlgItem(IDC_LIST_LOCATION, this);
  m_wndGroupLocation.SetSelectedBold(TRUE);
  m_wndGroupLocation.SetBackgroundImage(GetChildBackground(m_wndGroupLocation.GetDlgCtrlID()));
  
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_RIGHT_LUNG),
		static_cast<DWORD>(ReportLib::RightLung));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_LEFT_LUNG),
		static_cast<DWORD>(ReportLib::LeftLung));

  m_wndGroupSize.SubclassDlgItem(IDC_LIST_SIZE, this);
  m_wndGroupSize.SetSelectedBold(TRUE);
  m_wndGroupSize.SetBackgroundImage(GetChildBackground(m_wndGroupSize.GetDlgCtrlID()));
  
  m_wndGroupSize.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_SMALL),
		static_cast<DWORD>(ReportLib::Small));
  m_wndGroupSize.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_MEDIUM),
		static_cast<DWORD>(ReportLib::Medium));
  m_wndGroupSize.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_LARGE),
		static_cast<DWORD>(ReportLib::Large));
  m_wndGroupSize.SelectItem(static_cast<DWORD>(ReportLib::Small));  

  m_wndGroupLocation.SelectItem(m_spFinding->Location); 
  m_wndGroupSize.SelectItem(m_spFinding->Size);
} // OnInitLung2()


/**
 * Initializes sub-dialog
 */
void ReportSubFinding::OnInitLiver1()
{
  CreateSpinEdit(IDC_PERP_SIZE, m_wndPerpendicularSize, 0.0f, 999.0f,  0.0f, 0.1f, 1);
  CreateSpinEdit(IDC_MAXIMUM_SIZE, m_wndMaximumSize, 0.0f, 999.0f,  0.0f, 0.1f, 1);

  m_wndGroupLocation.SubclassDlgItem(IDC_LIST_LOCATION, this);
  m_wndGroupLocation.SetSelectedBold(TRUE);
  m_wndGroupLocation.SetBackgroundImage(GetChildBackground(m_wndGroupLocation.GetDlgCtrlID()));
  
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_RIGHT_LOBE_ANTERIOR),
		static_cast<DWORD>(ReportLib::RightLobe_AnteriorSegment));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_RIGHT_LOBE_POSTERIOR),
		static_cast<DWORD>(ReportLib::RightLobe_PosteriorSegment));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_LEFT_LOBE_MEDIAL),
		static_cast<DWORD>(ReportLib::LeftLobe_MedialSegment));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_LEFT_LOBE_LATERAL),
		static_cast<DWORD>(ReportLib::LeftLobe_LateralSegment));
    
  m_wndGroupLocation.SelectItem(m_spFinding->Location); 
  m_wndMaximumSize.SetValue(m_spFinding->MaximumSize);

  m_wndPerpendicularSize.SetValue(m_spFinding->PerpendicularSize);
  m_wndPerpendicularSize.ShowWindow(m_spFinding->PerpendicularSize > 0.0f);
} // OnInitLiver1()


/**
 * Initializes sub-dialog
 */
void ReportSubFinding::OnInitAdrenal1()
{
  CreateSpinEdit(IDC_PERP_SIZE, m_wndPerpendicularSize, 0.0f, 999.0f,  0.0f, 0.1f, 1);
  CreateSpinEdit(IDC_MAXIMUM_SIZE, m_wndMaximumSize, 0.0f, 999.0f,  0.0f, 0.1f, 1);

  m_wndGroupLocation.SubclassDlgItem(IDC_LIST_LOCATION, this);
  m_wndGroupLocation.SetSelectedBold(TRUE);
  m_wndGroupLocation.SetBackgroundImage(GetChildBackground(m_wndGroupLocation.GetDlgCtrlID()));
  
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_RIGHT),
		static_cast<DWORD>(ReportLib::Right));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_LEFT),
		static_cast<DWORD>(ReportLib::Left));

  m_wndGroupLocation.SelectItem(m_spFinding->Location); 
  m_wndMaximumSize.SetValue(m_spFinding->MaximumSize);

  m_wndPerpendicularSize.SetValue(m_spFinding->PerpendicularSize);
  m_wndPerpendicularSize.ShowWindow(m_spFinding->PerpendicularSize > 0.0f);
} // OnInitAdrenal1()


/**
 * Initializes sub-dialog
 */
void ReportSubFinding::OnInitRenal1()
{
  CreateSpinEdit(IDC_PERP_SIZE, m_wndPerpendicularSize, 0.0f, 999.0f,  0.0f, 0.1f, 1);
  CreateSpinEdit(IDC_MAXIMUM_SIZE, m_wndMaximumSize, 0.0f, 999.0f,  0.0f, 0.1f, 1);

  m_wndGroupLocation.SubclassDlgItem(IDC_LIST_LOCATION, this);
  m_wndGroupLocation.SetSelectedBold(TRUE);
  m_wndGroupLocation.SetBackgroundImage(GetChildBackground(m_wndGroupLocation.GetDlgCtrlID()));
  
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_RIGHT_UPPER_POLE),
		static_cast<DWORD>(ReportLib::Right_UpperPole));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_RIGHT_MIDDLE),
		static_cast<DWORD>(ReportLib::Right_Middle));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_RIGHT_LOWER_POLE),
		static_cast<DWORD>(ReportLib::Right_LowerPole));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_LEFT_UPPER_POLE),
		static_cast<DWORD>(ReportLib::Left_UpperPole));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_LEFT_MIDDLE),
		static_cast<DWORD>(ReportLib::Left_Middle));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_LEFT_LOWER_POLE),
		static_cast<DWORD>(ReportLib::Left_LowerPole));
  
  m_wndGroupLocation.SelectItem(m_spFinding->Location); 
  m_wndMaximumSize.SetValue(m_spFinding->MaximumSize);

  m_wndPerpendicularSize.SetValue(m_spFinding->PerpendicularSize);
  m_wndPerpendicularSize.ShowWindow(m_spFinding->PerpendicularSize > 0.0f);
} // OnInitRenal1()


/**
 * Initializes sub-dialog
 */
void ReportSubFinding::OnInitRenal2()
{
  m_wndGroupLocation.SubclassDlgItem(IDC_LIST_LOCATION, this);
  m_wndGroupLocation.SetSelectedBold(TRUE);
  m_wndGroupLocation.SetBackgroundImage(GetChildBackground(m_wndGroupLocation.GetDlgCtrlID()));
  
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_RIGHT_UPPER_POLE),
		static_cast<DWORD>(ReportLib::Right_UpperPole));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_RIGHT_MIDDLE),
		static_cast<DWORD>(ReportLib::Right_Middle));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_RIGHT_LOWER_POLE),
		static_cast<DWORD>(ReportLib::Right_LowerPole));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_LEFT_UPPER_POLE),
		static_cast<DWORD>(ReportLib::Left_UpperPole));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_LEFT_MIDDLE),
		static_cast<DWORD>(ReportLib::Left_Middle));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_LEFT_LOWER_POLE),
		static_cast<DWORD>(ReportLib::Left_LowerPole));

  m_wndGroupLocation.SelectItem(m_spFinding->Location); 
} // OnInitRenal2()


/**
 * Initializes sub-dialog
 */
void ReportSubFinding::OnInitRenal3()
{
  m_wndGroupLocation.SubclassDlgItem(IDC_LIST_LOCATION, this);
  m_wndGroupLocation.SetSelectedBold(TRUE);
  m_wndGroupLocation.SetBackgroundImage(GetChildBackground(m_wndGroupLocation.GetDlgCtrlID()));
  
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_RIGHT_KIDNEY),
		static_cast<DWORD>(ReportLib::RightKidney));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_LEFT_KIDNEY),
		static_cast<DWORD>(ReportLib::LeftKidney));

  m_wndGroupLevel.SubclassDlgItem(IDC_LIST_LEVEL, this);
  m_wndGroupLevel.SetSelectedBold(TRUE);
  m_wndGroupLevel.SetBackgroundImage(GetChildBackground(m_wndGroupLevel.GetDlgCtrlID()));
  
  m_wndGroupLevel.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_MILD),
		static_cast<DWORD>(ReportLib::Mild));
  m_wndGroupLevel.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_MODERATE),
		static_cast<DWORD>(ReportLib::Moderate));
  m_wndGroupLevel.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_SEVERE),
		static_cast<DWORD>(ReportLib::Severe));
  
  m_wndGroupLocation.SelectItem(m_spFinding->Location); 
  m_wndGroupLevel.SelectItem(m_spFinding->Degree);
} // OnInitRenal3()


/**
 * Initializes sub-dialog
 */
void ReportSubFinding::OnInitVascular1()
{ 
  CreateSpinEdit(IDC_LENGTH_SIZE, m_wndLength, 0.0f, 999.0f, 0.0f, 0.1f, 1);
  CreateSpinEdit(IDC_PERP_SIZE, m_wndPerpendicularSize, 0.0f, 999.0f,  0.0f, 0.1f, 1);
  CreateSpinEdit(IDC_MAXIMUM_SIZE, m_wndMaximumSize, 0.0f, 999.0f,  0.0f, 0.1f, 1);

  m_wndLength.SetValue(m_spFinding->Length);
  m_wndMaximumSize.SetValue(m_spFinding->MaximumSize);

  m_wndPerpendicularSize.SetValue(m_spFinding->PerpendicularSize);
  m_wndPerpendicularSize.ShowWindow(m_spFinding->PerpendicularSize > 0.0f);

  m_wndGroupLocation.SubclassDlgItem(IDC_LIST_LOCATION, this);
  m_wndGroupLocation.SetSelectedBold(TRUE);
  m_wndGroupLocation.SetBackgroundImage(GetChildBackground(m_wndGroupLocation.GetDlgCtrlID()));
  
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_SUPRARENAL),
		static_cast<DWORD>(ReportLib::Suprarenal));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_JUXTARENAL),
		static_cast<DWORD>(ReportLib::Juxtarenal));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_INFRARENAL),
		static_cast<DWORD>(ReportLib::Infrarenal));
  
  m_wndGroupLocation.SelectItem(m_spFinding->Location); 
} // OnInitVascular1()


/**
 * Initializes sub-dialog
 */
void ReportSubFinding::OnInitVascular2()
{
  m_wndGroupLevel.SubclassDlgItem(IDC_LIST_LEVEL, this);
  m_wndGroupLevel.SetSelectedBold(TRUE);
  m_wndGroupLevel.SetBackgroundImage(GetChildBackground(m_wndGroupLevel.GetDlgCtrlID()));
  
  m_wndGroupLevel.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_MILD),
		static_cast<DWORD>(ReportLib::Mild));
  m_wndGroupLevel.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_MODERATE),
		static_cast<DWORD>(ReportLib::Moderate));
  m_wndGroupLevel.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_SEVERE),
		static_cast<DWORD>(ReportLib::Severe));
  
  m_wndGroupLevel.SelectItem(m_spFinding->Degree); 
} // OnInitVascular2()


/**
 * Initializes sub-dialog
 */
void ReportSubFinding::OnInitSpleen1()
{
  CreateSpinEdit(IDC_PERP_SIZE, m_wndPerpendicularSize, 0.0f, 999.0f, 0.0f, .1f, 1.0f);
  CreateSpinEdit(IDC_MAXIMUM_SIZE, m_wndMaximumSize, 0.0f, 999.0f, 0.0f, .1f, 1.0f);

  m_wndGroupLocation.SubclassDlgItem(IDC_LIST_LOCATION, this);
  m_wndGroupLocation.SetSelectedBold(TRUE);
  m_wndGroupLocation.SetBackgroundImage(GetChildBackground(m_wndGroupLocation.GetDlgCtrlID()));
  
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_SUPERIOR),
		static_cast<DWORD>(ReportLib::Superior));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_MIDDLE),
		static_cast<DWORD>(ReportLib::Middle));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_INFERIOR),
		static_cast<DWORD>(ReportLib::Inferior));
  
  m_wndGroupLocation.SelectItem(m_spFinding->Location); 
  m_wndMaximumSize.SetValue(m_spFinding->MaximumSize);

  m_wndPerpendicularSize.SetValue(m_spFinding->PerpendicularSize);
  m_wndPerpendicularSize.ShowWindow(m_spFinding->PerpendicularSize > 0.0f);
} // OnInitSpleen1()


/**
 * Initializes sub-dialog
 */
void ReportSubFinding::OnInitGI1()
{
  m_wndGroupLocation.SubclassDlgItem(IDC_LIST_LOCATION, this);
  m_wndGroupLocation.SetSelectedBold(TRUE);
  m_wndGroupLocation.SetBackgroundImage(GetChildBackground(m_wndGroupLocation.GetDlgCtrlID()));
  
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_RIGHT),
		static_cast<DWORD>(ReportLib::Right));
  m_wndGroupLocation.InsertGroupItem(ReportGroupCtrl::NO_GROUP, LoadResourceStringWithUpperHead(IDS_LEFT),
		static_cast<DWORD>(ReportLib::Left));
  m_wndGroupLocation.SelectItem(m_spFinding->Location); 
} // OnInitGI1()


// Revision History:
// $Log: ReportSubFinding.C,v $
// Revision 1.6.2.4  2010/02/05 15:19:11  dongqing
// update the hidden extra-colonic text when an extra-colonic entry changes
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.6.2.3  2009/07/30 17:58:20  kchalupa
// Code Walkthrough
//
// Revision 1.6.2.2  2008/02/12 20:32:25  jmeade
// Issue 5924: adding other-findings info to extra-colonic colon findings.
//
// Revision 1.6.2.1  2007/04/27 02:20:34  jmeade
// spacing.
//
// Revision 1.6  2007/02/09 04:26:13  jmeade
// Issue 5219: Implementing C-RADS: perpendicular field.
//
// Revision 1.5  2007/02/09 00:11:49  jmeade
// Issue 5219: Implementing C-RADS: morphology, other findings E0-E4, failed colonoscopy indication.
//
// Revision 1.4  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.3.2.1  2005/12/20 16:22:38  geconomos
// revisited SlideIn()
//
// Revision 1.3  2005/09/13 17:26:39  jmeade
// ColonReport namespace.
//
// Revision 1.2  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 3.1  2001/12/26 20:53:57  binli
// adjustment of Report.
//
// Revision 3.0  2001/10/14 23:01:59  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:41:50   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:42   ingmar
// Initial revision.
// Revision 1.13  2001/08/29 22:25:43  binli
// task 164: tab order is no good. changed (left to right, top to bottom)
//
// Revision 1.12  2001/05/10 21:04:39  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.11  2001/05/09 14:14:41  geconomos
// Addressed Item#000330 - Overall Impression
//
// Revision 1.10  2001/04/13 02:34:23  jmeade
// headers and footers
//
// Revision 1.9  2001/03/30 18:13:20  geconomos
// Partially fixed bug with reportspinedit control
//
// Revision 1.8  2001/02/02 08:07:38  jeff
// Moving strings to resource table
//
// Revision 1.7  2001/02/01 22:29:22  geconomos
// Fixed bug with not saving current view in other findings
//
// Revision 1.6  2001/02/01 16:35:21  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ReportSubFinding.C,v 1.6.2.4 2010/02/05 15:19:11 dongqing Exp $
// $Id: ReportSubFinding.C,v 1.6.2.4 2010/02/05 15:19:11 dongqing Exp $
