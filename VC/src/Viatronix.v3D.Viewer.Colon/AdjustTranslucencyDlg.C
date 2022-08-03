// $Id: AdjustTranslucencyDlg.C,v 1.16.2.17 2010/01/07 18:40:44 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: AdjustTranslucencyDlg.C   
// Description: Dialog to adjust colors of translucent rendering 
// Owner: Jeff Meade [jmeade@viatronix.com]
//

#include "stdafx.h"
#include "AdjustTranslucencyDlg.h"
#include "HUconverter.h"
#include "MainAppFrame.h"
#include "User.h"
#include "TextPromptWnd.h"
#include "vxDOMNodeList.h"
#include "vxDOMNamedNodeMap.h"
#include "Viatron.h"
#include "ApplicationSettings.h"
#include "ServiceLocator.h"
#include "vcUtils.h"


#include <vector>

using namespace ColonViews::Dialogs;
using namespace ReaderLib;
using namespace WinUtils;

IMPLEMENT_DYNAMIC(AdjustTranslucencyDlg, CDialog)

#define FILE_REVISION "$Revision: 1.16.2.17 $"

const std::string SYSTEM_DEFAULT_KEY( "System Default" );
const std::string SYSTEM_DEFAULT_ID( "\x7FSystem Default" );


/**
 * Constructor
 *
 * @param pParent   Parent window for dialog
 */
AdjustTranslucencyDlg::AdjustTranslucencyDlg(CWnd* pParent /*=NULL*/, const uint4 uDlgBackgroundResourceID /*=IDB_ENDOVIEW_HORIZ_DLGBAR*/)
  : ViatronDialog(AdjustTranslucencyDlg::IDD, pParent, uDlgBackgroundResourceID)
{
  //{{AFX_DATA_INIT(AdjustTranslucencyDlg)
  //}}AFX_DATA_INIT

  // Initialize gradient entries
  GradientList& grad = m_gradientCtrl.GetGradient();

  const TransferFunctionVC& func = rdrGlobal.m_pCurrDataset->m_biopsyTransferFunction;
  for (int i(1); i < func.m_vfBiopsyThresholds.size()-1; i++)
  {
    grad.AddPeg(RGB(0xff,0xff,0), func.m_vfBiopsyThresholds[i].m_fThreshold / func.GetResolution());
  }
} // AdjustTranslucencyDlg::AdjustTranslucencyDlg


/**
 * Destructor
 */
AdjustTranslucencyDlg::~AdjustTranslucencyDlg()
{
} // ~AdjustTranslucencyDlg


/**
 * called before the current layout changes
 */
void AdjustTranslucencyDlg::OnLayoutChanging()
{
  WriteSettings();
} // OnLayoutChanging()


/**
 * called after the current layout changes
 */
void AdjustTranslucencyDlg::OnLayoutChanged()
{
} // OnLayoutChanged()


/**
 * stores the currently selected set of thresholds in the registry
 */
void AdjustTranslucencyDlg::SaveCurrentSettingsToRegistry()
{
  // Save translucency settings
  std::string sSettings;
  const std::vector<TransferFunctionVC::LinearTFParams>& vThresh = rdrGlobal.m_pCurrDataset->m_biopsyTransferFunction.m_vfBiopsyThresholds;

  for (int4 i(0); i < vThresh.size(); i++)
  {
    sSettings += ToAscii(vThresh[i].m_fThreshold) + " ";
    sSettings += ToAscii(vThresh[i].m_prevColor.A()) + " " + ToAscii(vThresh[i].m_nextColor.A()) + " ";
    sSettings += ToAscii(RGB(vThresh[i].m_prevColor.R(), vThresh[i].m_prevColor.G(), vThresh[i].m_prevColor.B())) + " ";
    sSettings += ToAscii(RGB(vThresh[i].m_nextColor.R(), vThresh[i].m_nextColor.G(), vThresh[i].m_nextColor.B())) + " ";
  }

  ApplicationSettings::UserSettings().Set( APP_SETTING_TRANSLUCENCY_SETTINGS, sSettings );
} // SaveCurrentSettingsToRegistry()


/**
 * Dialog data variable initialization.
 *
 *  @param pDX   Dialog data exchange instance.
 */
void AdjustTranslucencyDlg::DoDataExchange(CDataExchange* pDX)
{
  ViatronDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(AdjustTranslucencyDlg)
  DDX_Control(pDX, IDC_AJT_GRADIENTCTRL, m_gradientCtrl);
  //}}AFX_DATA_MAP
  DDX_Control(pDX, IDC_AJT_COLOR_BTN_PREV, m_btnPrev);
  DDX_Control(pDX, IDC_AJT_COLOR_BTN_NEXT, m_btnNext);
  DDX_Control(pDX, IDC_AJT_EDITSELECTED, m_btnEditSel);
  DDX_Control(pDX, IDC_AJT_DELETESELECTED, m_btnDeleteSel);
  DDX_Control(pDX, IDC_AJT_SAVENEW, m_btnSaveNew);
  DDX_Control(pDX, IDC_AJT_PRESETS_COMBO, m_cbxPresets);
} // DoDataExchange


BEGIN_MESSAGE_MAP(AdjustTranslucencyDlg, ViatronDialog)
  //{{AFX_MSG_MAP(AdjustTranslucencyDlg)
  ON_WM_CREATE()
  ON_WM_SETCURSOR()
  ON_BN_CLICKED(IDC_AJT_EDITSELECTED, OnEditSelected)
  ON_BN_CLICKED(IDC_AJT_SAVESELECTED, OnSaveSelected)
  ON_BN_CLICKED(IDC_AJT_CANCELEDIT, OnCancelEditSelected)
  ON_WM_SHOWWINDOW()
  ON_WM_SIZE()
  //}}AFX_MSG_MAP
  ON_NOTIFY(GC_SELCHANGE, IDC_AJT_GRADIENTCTRL, OnNotifyChangeSelPeg)
  ON_NOTIFY(GC_PEGMOVE, IDC_AJT_GRADIENTCTRL, OnNotifyPegMove)
  ON_NOTIFY(GC_PEGMOVED, IDC_AJT_GRADIENTCTRL, OnNotifyPegMoved)
  ON_NOTIFY(GC_EDITPEG, IDC_AJT_GRADIENTCTRL, OnNotifyEditPeg)
  ON_NOTIFY(WM_MOUSEMOVE, IDC_AJT_GRADIENTCTRL, OnNotifyMouseMoveGradientCtrl)
  ON_BN_CLICKED(IDC_AJT_SAVENEW, OnSaveNewSettings)
  ON_BN_CLICKED(IDC_AJT_COLOR_BTN_PREV, OnClickColorBtnPrev)
  ON_BN_CLICKED(IDC_AJT_COLOR_BTN_NEXT, OnClickColorBtnNext)
  ON_CBN_SELCHANGE(IDC_AJT_PRESETS_COMBO, OnSelChangePresets)
  ON_BN_CLICKED(IDC_AJT_DELETESELECTED, OnClickedDelete)
  ON_WM_DESTROY()
END_MESSAGE_MAP()


/**
 * delete button clicked
 */
void AdjustTranslucencyDlg::OnClickedDelete()
{
  CString sName;
  m_cbxPresets.GetLBText(m_cbxPresets.GetCurSel(), sName);
  TransSettingsMap::iterator iter = m_mapSettings.find(LPCTSTR(sName));
  if (iter == m_mapSettings.end())
    return;

  m_mapSettings.erase(iter);
  if (m_cbxPresets.DeleteString(m_cbxPresets.FindString(-1, sName)) >= 0)
  {
    m_cbxPresets.SetCurSel(0);
    SetCurrentPreset(0);
  }
} // OnClickedDelete()

////////////////////////// AdjustTranslucencyDlg message handlers //////////////////////////

/**
 * WM_CREATE handler.
 *
 * @param lpCreateStruct   Create window parameters.
 * @return                 -1 if failed, 0 otherwise.
 */
int AdjustTranslucencyDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (CDialog::OnCreate(lpCreateStruct) == -1)
    return -1;

  // Initialize gradient control  
  GradientList& grad = m_gradientCtrl.GetGradient();
  grad.SetPegOrderFixed(true);
  grad.SetBackgroundColour(RGB(0xff,0,0));
  grad.SetInterpolationMethod(GradientList::LINEAR);
  grad.SetDiscreteColorRangesMode(true);

  m_gradientCtrl.SetOrientation(GradientCtrl::FORCEHORIZONTAL);
  m_gradientCtrl.EnableKeyboardEdit(false);
  m_gradientCtrl.SetPegSide(true, false); // no pegs on right (top)
  m_gradientCtrl.SetPegSide(false, true); // pegs on left (bottom)
  m_gradientCtrl.ShowTooltips(FALSE);

  InitGradientDisplayValues();
  return 0;
} // OnCreate


/**
 * Init dialog procedure
 * 
 * @return   Create success
 */
BOOL AdjustTranslucencyDlg::OnInitDialog()
{
  ViatronDialog::OnInitDialog();
  SetColorButtons(GradientList::NONE);

  SetButtonIcon(m_btnSaveNew, IDB_COLORMAP_RESET_SYSTEM);
  SetButtonIcon(m_btnEditSel, IDB_COLORMAP_RESET_USER);
  SetButtonIcon(m_btnDeleteSel, IDB_DELETEALL_BUTTON);

  // Enable tool tips
  const BOOL bTipsEnabled(EnableToolTips(TRUE));

  ReadSettings();

  // brute force way of determining which is the set 'settings', since the name isn't being saved
  const std::vector<TransferFunctionVC::LinearTFParams>& vSetThresh = rdrGlobal.m_pCurrDataset->m_biopsyTransferFunction.m_vfBiopsyThresholds;

  int4 iSetting(0); bool bSet(false);
  for (TransSettingsMap::iterator iter = m_mapSettings.begin(); iter != m_mapSettings.end(); iter++, iSetting++)
  {
    const TranslucencySetting it = iter->second;
    const std::vector<TransferFunctionVC::LinearTFParams>& vItThresh = iter->second.m_vThresholds;
    bool bMatch(true);

    for (int4 iCt(0); iCt < vItThresh.size() && iCt < vSetThresh.size(); iCt++)
    {
      if ( (int4(vSetThresh[iCt].m_fThreshold) != int4(vItThresh[iCt].m_fThreshold)) ||
        (vSetThresh[iCt].m_nextColor != vItThresh[iCt].m_nextColor) ||
        (vSetThresh[iCt].m_prevColor != vItThresh[iCt].m_prevColor) )
      {
        bMatch = false;
        break;
      }
    }

    if (bMatch)
    {
      m_cbxPresets.SetCurSel(iSetting);
      SetCurrentPreset(iSetting);
      bSet = true;
      break;
    }
  }

  if (!bSet) // if not found, set system default
  {
    m_cbxPresets.SetCurSel(m_mapSettings.size()-1);
    SetCurrentPreset(m_mapSettings.size()-1);
  }
  //}} brute force, damn is this ugly

  m_gradientCtrl.EnableWindow(FALSE);

  return TRUE;  // return TRUE unless you set the focus to a control
                // EXCEPTION: OCX Property Pages should return FALSE
} // OnInitDialog


/**
 * Called when window is resized
 *
 * @param uType   Specifies the type of resizing requested. 
 * @param iCX     Width
 * @param iCY     Height
 */
void AdjustTranslucencyDlg::OnSize( UINT uType, int iCX, int iCY ) 
{
  ViatronDialog::OnSize(uType, iCX, iCY);

  if ( iCX < 1 || iCY < 1 )
    return;
return;
  //
  // custom resizing code
  //

  CWnd *pControl = NULL;

  // layout the buttons
  const int4 iGap = 5;
  const int4 iButtonSize = 62;
  const int4 iButtonRevertX = iCX - iGap - iButtonSize;
  const int4 iButtonY = iCY - iGap - iButtonSize;
  const int4 iButtonDefaultX = iButtonRevertX - iGap - iButtonSize;
  if ((pControl = GetDlgItem( IDC_AJT_SAVENEW ))!= NULL )       pControl->SetWindowPos( NULL, iButtonRevertX, iButtonY, iButtonSize, iButtonSize, SWP_NOZORDER | SWP_NOSIZE );
  if ((pControl = GetDlgItem( IDC_AJT_EDITSELECTED ))!= NULL ) pControl->SetWindowPos( NULL, iButtonDefaultX, iButtonY, iButtonSize, iButtonSize, SWP_NOZORDER | SWP_NOSIZE );

  // preset combobox
  int4 iComboX(0);
  CRect rect;
  if ( (pControl = GetDlgItem( IDC_AJT_PRESETS_COMBO )) != NULL )
  {
    pControl->GetWindowRect(rect);
    this->ScreenToClient(rect);
    rect.MoveToX(iButtonDefaultX - iGap*3 - rect.Width());
    pControl->SetWindowPos( NULL, iComboX = rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER );
  }
  // delete button
  if ( (pControl = GetDlgItem( IDC_AJT_DELETESELECTED )) != NULL )
  {
    pControl->GetWindowRect(rect);
    this->ScreenToClient(rect);
    rect.MoveToX(iComboX - iGap*3 - rect.Width());
    pControl->SetWindowPos( NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOZORDER | SWP_NOSIZE);
  }

  // layout the gradient
  const int4 iGradientWidth = iCX - 2 * iGap;
  const int4 iGradientHeight = iCY - 3 * iGap - iButtonSize;
  if ( (pControl = GetDlgItem( IDC_AJT_GRADIENTCTRL )) != NULL ) pControl->SetWindowPos( NULL, iGap, iGap, iGradientWidth, iGradientHeight, SWP_NOZORDER );

  // leave the rest where they are

} // OnSize()


/**
 * update entry colors and positions from current biopsy thresholds
 */
void AdjustTranslucencyDlg::InitGradientDisplayValues()
{
  const TransferFunctionVC& func = rdrGlobal.m_pCurrDataset->m_biopsyTransferFunction;
  GradientList& grad = m_gradientCtrl.GetGradient();
  std::vector<TransferFunctionVC::LinearTFParams>& vThresh = rdrGlobal.m_pCurrDataset->m_biopsyTransferFunction.m_vfBiopsyThresholds;

  const int4 iMaxVal(func.m_vfBiopsyThresholds.size()-1);
  grad.SetStartPegColour(RGB(vThresh[0].m_prevColor.R(), vThresh[0].m_prevColor.G(), vThresh[0].m_prevColor.B()));
  grad.SetEndPegColour(RGB(vThresh[iMaxVal].m_nextColor.R(), vThresh[iMaxVal].m_nextColor.G(), vThresh[iMaxVal].m_nextColor.B()));

  for (int4 i(1); i < iMaxVal; i++)
  {
    Peg peg = grad.GetPeg(i-1);
    peg.SetPosition(vThresh[i].m_fThreshold / func.GetResolution());
    peg.SetPrevEndColor(RGB(vThresh[i-1].m_nextColor.R(), vThresh[i-1].m_nextColor.G(), vThresh[i-1].m_nextColor.B()));
    peg.SetColor(RGB(vThresh[i].m_prevColor.R(), vThresh[i].m_prevColor.G(), vThresh[i].m_prevColor.B()));
    grad.SetPeg(i-1, peg);
  }

  if (IsWindow(m_gradientCtrl))
    m_gradientCtrl.Invalidate(TRUE);
} // InitGradientDisplayValues


/**
 * Notification message: GC_PEGMOVED
 *
 * @param pNotifyStruct   Notification message information
 * @param pResult         Out result
 */
void AdjustTranslucencyDlg::OnNotifyPegMove(NMHDR *pNotifyStruct, LRESULT *pResult)
{
  const PegNMHDR* pPegNotifyStruct = (PegNMHDR*)pNotifyStruct;
  const int4 iPeg(pPegNotifyStruct->iIndex);
  const Peg peg = m_gradientCtrl.GetGradient().GetPeg(iPeg);

  TransferFunctionVC& func = rdrGlobal.m_pCurrDataset->m_biopsyTransferFunction;
  const float4 fVal = peg.GetPosition() * func.GetResolution();
  func.m_vfBiopsyThresholds[iPeg+1].m_fThreshold = fVal;

  UpdateVolumeDisplay();


#ifndef FINAL_RELEASE
  int iMax(255);
  Image<PixelRGB<uint1> > image(Triple<uint4>(256+20, 30, 0), PixelRGB<uint1>(0, 100, 0));

  for (int4 j(0); j<=iMax; j++)
  {
    uint2 uR(func.m_pfRed[j]);
    uint2 uG(func.m_pfGreen[j]);
    uint2 uB(func.m_pfBlue[j]);
    uint2 uA(func.m_pfOpacity[j]);

    for (int4 k(0); k<10; ++k)
    {
      image.SetPixel(Point2D<uint2>(10+j, 10+k), PixelRGB<uint1>(uR, uG, uB));
    }
  }
  image.Write("D:/Colormap1dFull.ppm");
#endif

} // OnNotifyPegMove


/**
 * Notification message: GC_EDITPEG
 *
 * @param pNotifyStruct   Notification message information
 * @param pResult         Out result
 */
void AdjustTranslucencyDlg::OnNotifyEditPeg(NMHDR * pNotifyStruct, LRESULT *result)
{
} // OnNotifyEditPeg


/**
 * Notification message: GC_SELCHANGE
 *
 * @param pNotifyStruct   Notification message information
 * @param pResult         Out result
 */
void AdjustTranslucencyDlg::OnNotifyChangeSelPeg(NMHDR *pNotifyStruct, LRESULT *pResult)
{
  const PegNMHDR* pPegNotifyStruct = (PegNMHDR*)pNotifyStruct;
  SetColorButtons(pPegNotifyStruct->iIndex);
  if ((pPegNotifyStruct->iIndex == GradientList::NONE) || (pPegNotifyStruct->iIndex == GradientList::BACKGROUND))
  {
    return;
  }


  const Peg peg = m_gradientCtrl.GetGradient().GetPeg(pPegNotifyStruct->iIndex);
  const TransferFunctionVC& func = rdrGlobal.m_pCurrDataset->m_biopsyTransferFunction;

  const float4 fVal = peg.GetPosition() * func.GetResolution();
  SetDlgItemText(IDC_AJT_PEG_VALUE_STATIC, ToAscii(HUconverter::Uchar2HU(uint1(fVal))).c_str());
} // OnNotifyChangeSelPeg


/**
 * Sets the previous and next buttons to the same color of the given threshold
 *
 * @param iIndex   Threshold for button color values
 */
void AdjustTranslucencyDlg::SetColorButtons(const int4 iIndex)
{
  const LinearTFParamsPair paramPair = PegIndexToColorPair(iIndex);

  if (paramPair.first != NULL)
    m_btnPrev.SetColor(RGB(paramPair.first->m_nextColor.R(), paramPair.first->m_nextColor.G(), paramPair.first->m_nextColor.B()));
  m_btnPrev.EnableWindow(paramPair.first != NULL);

  if (paramPair.second != NULL)
    m_btnNext.SetColor(RGB(paramPair.second->m_prevColor.R(), paramPair.second->m_prevColor.G(), paramPair.second->m_prevColor.B()));
  m_btnNext.EnableWindow(paramPair.second != NULL);
} // SetColorButtons


/**
 * read settings file
 *
 * @return   success/failure
 **/
bool ColonViews::TransSettingsMap::ReadSettings()
{
  // clear map
  clear();

  //{{ read settings file
  try
  {
    
    std::string sXml( ServiceLocator::GetConfigurationService()->GetConfiguration( ViatronApp::GetViatronApp()->GetContextId(), CONFIGURATION_TRANSLUCENCY ));
    if( Utilities::Xml::IsEmpty( sXml ))
      sXml = ServiceLocator::GetConfigurationService()->GetDefaultConfiguration( ViatronApp::GetViatronApp()->GetContextId(), CONFIGURATION_TRANSLUCENCY );

    //if(vx::File::Exists(sFile))
    if( !sXml.empty() )
    {
      vxDOMDocument * pDocSettings;
      //pDocSettings = new vxDOMDocument(vxDOMDocument::FromFile( sFile ));
      pDocSettings = new vxDOMDocument(vxDOMDocument::FromXml( sXml ));

      if (!pDocSettings->IsValid())
        throw ex::FileNotFoundException(LogRec("Could not open settings file.", "AdjustTranslucencyDlg", "ReadSettings"));

      vxDOMElement elem = pDocSettings->GetDocumentElement();

      vxDOMNodeList listSettings = elem.GetElementsByTagName("ThresholdsSetting");
      for (int4 iSetting(0); iSetting < listSettings.GetLength(); iSetting++)
      {
        TranslucencySetting setg;

        // get name and owner attributes
        vxDOMNamedNodeMap nodemap = listSettings.Item(iSetting).GetAttributes();
        setg.m_sName = nodemap.GetNamedItem("Name").GetNodeValue();
        setg.m_sOwner = nodemap.GetNamedItem("Owner").GetNodeValue();

        vxDOMElement elemInner = listSettings.Item(iSetting).ToElement();
        vxDOMNodeList listThresh = elemInner.GetElementsByTagName("Threshold");
        setg.m_vThresholds.resize(listThresh.GetLength());
        for (int4 iThresh(0); iThresh < listThresh.GetLength(); iThresh++)
        {
          vxDOMNode itemThresh = listThresh.Item(iThresh);
          TransferFunctionVC::LinearTFParams& currparam = setg.m_vThresholds[iThresh];
          currparam.m_fThreshold = atof(itemThresh.GetAttributes().GetNamedItem("Value").GetNodeValue().c_str());

          vxDOMNodeList listPrev = itemThresh.ToElement().GetElementsByTagName("Previous");
          vxDOMNamedNodeMap mapPrev = listPrev.Item(0).GetAttributes();
          currparam.m_prevColor.R() = atof(mapPrev.GetNamedItem("Red").GetNodeValue().c_str());
          currparam.m_prevColor.G() = atof(mapPrev.GetNamedItem("Green").GetNodeValue().c_str());
          currparam.m_prevColor.B() = atof(mapPrev.GetNamedItem("Blue").GetNodeValue().c_str());
          currparam.m_prevColor.A() = atof(mapPrev.GetNamedItem("Opacity").GetNodeValue().c_str());

          vxDOMNodeList listNext = itemThresh.ToElement().GetElementsByTagName("Next");
          vxDOMNamedNodeMap mapNext = listNext.Item(0).GetAttributes();
          currparam.m_nextColor.R() = atof(mapNext.GetNamedItem("Red").GetNodeValue().c_str());
          currparam.m_nextColor.G() = atof(mapNext.GetNamedItem("Green").GetNodeValue().c_str());
          currparam.m_nextColor.B() = atof(mapNext.GetNamedItem("Blue").GetNodeValue().c_str());
          currparam.m_nextColor.A() = atof(mapNext.GetNamedItem("Opacity").GetNodeValue().c_str());
        } // for thresholds

        // add settings to map
        (*this)[setg.m_sName] = setg;
      } // for list of thresholds

      m_vWL.clear();
      vxDOMNodeList listWLs = elem.GetElementsByTagName("WindowLevelSetting");
      for (int4 iWL(0); (iWL < listWLs.GetLength()) && (iWL < iMaxWLVectorSize); iWL++)
      {
        vxDOMNode itemWL = listWLs.Item(iWL);
        vxDOMNamedNodeMap attMap = itemWL.GetAttributes();
        std::string sName = attMap.GetNamedItem("Name").GetNodeValue().c_str();
        m_vWL.push_back(std::make_pair(sName,
          std::make_pair(atoi(attMap.GetNamedItem("Window").GetNodeValue().c_str()), atoi(attMap.GetNamedItem("Level").GetNodeValue().c_str()))));
      } // for window/level settings
    }
    else
    {
      //LogDbg(sFile + " not found", "AdjustTranslucencyDlg", "ReadSettings");
      LogDbg( "TranslucencySettings not found", "AdjustTranslucencyDlg", "ReadSettings");
    }
  }
  catch (...)
  {

  }
  //}} read settings file

  return true;
} // ReadSettings()


/**
 * read settings from a file
 *
 * @return  success/failure
 **/
bool AdjustTranslucencyDlg::ReadSettings()
{
  m_mapSettings.ReadSettings();

  // Add settings names to the presets combo
  for (TransSettingsMap::iterator iter = m_mapSettings.begin(); iter != m_mapSettings.end(); iter++)
  {
    m_cbxPresets.AddString(iter->second.m_sName.c_str());
  }

  //{{ store system default settings
  TranslucencySetting setting;
  setting.m_sName = SYSTEM_DEFAULT_ID;
  TransferFunctionVC::GetDefaultBiopsyThresholds(setting.m_vThresholds);
  m_mapSettings[setting.m_sName] = setting;
  //}} store system default settings

  // add system default name to bottom of list
  m_cbxPresets.AddString( SYSTEM_DEFAULT_KEY.c_str() );
  m_cbxPresets.SetCurSel(-1);

  return true;
} // ReadSettings


/**
 * write settings file
 *
 * @return   success/failure
 **/
bool AdjustTranslucencyDlg::WriteSettings()
{
  try
  {
    SaveCurrentSettingsToRegistry();

    // window/level map may have been updated outside of the adjust translucency dialog, so renew it just in case
    ColonViews::TransSettingsMap tmap; tmap.ReadSettings();
    m_mapSettings.m_vWL.clear();
    m_mapSettings.m_vWL = tmap.m_vWL;

    m_mapSettings.WriteSettings();
  }
  catch (...)
  {
    return false;
  }

  return true;
} // WriteSettings()


bool ColonViews::TransSettingsMap::WriteSettings()
{
  MSXML2::IXMLDOMDocument2Ptr pXmlDoc;
  HRESULT hRes = pXmlDoc.CreateInstance(__uuidof(MSXML2::DOMDocument30));
  pXmlDoc->preserveWhiteSpace = VARIANT_TRUE;

  try
  {
    // Add xml data
    {
      //MSXML2::IXMLDOMProcessingInstructionPtr pInstr;
      //pInstr = pXmlDoc->createProcessingInstruction("xml", "version='1.0'");
      //pXmlDoc->appendChild(pInstr);

      MSXML2::IXMLDOMElementPtr pElement, pElTop, pElThresh, pChild;
      MSXML2::IXMLDOMAttributePtr pAttr;

      std::strstream sstr;
      pElTop   = XMLHelper::AppendAttributeChild(pXmlDoc, pXmlDoc, "translucency", "Version", variant_t(1.0));

      for (TransSettingsMap::const_iterator iter = begin(); iter != end(); iter++)
      {
        const TranslucencySetting& it = iter->second;

        if (it.m_sName == SYSTEM_DEFAULT_ID )
          continue;

        pElThresh = XMLHelper::AppendAttributeChild(pXmlDoc, pXmlDoc->documentElement, "ThresholdsSetting", "Name", variant_t(it.m_sName.c_str()));
        pAttr = pXmlDoc->createAttribute("Owner");   pAttr->value = variant_t(it.m_sOwner.c_str());
        pElThresh->setAttributeNode(pAttr);

        for (int4 i(0); i < iter->second.m_vThresholds.size(); i++)
        {
          pElement = XMLHelper::AppendAttributeChild(pXmlDoc, pElThresh, "Threshold", "Value", it.m_vThresholds[i].m_fThreshold);

          pChild = XMLHelper::AppendAttributeChild(pXmlDoc, pElement, "Previous", "Red", variant_t(it.m_vThresholds[i].m_prevColor.R()));
          pAttr = pXmlDoc->createAttribute("Green");   pAttr->value = variant_t(it.m_vThresholds[i].m_prevColor.G());   pChild->setAttributeNode(pAttr);
          pAttr = pXmlDoc->createAttribute("Blue");    pAttr->value = variant_t(it.m_vThresholds[i].m_prevColor.B());   pChild->setAttributeNode(pAttr);
          pAttr = pXmlDoc->createAttribute("Opacity"); pAttr->value = variant_t(it.m_vThresholds[i].m_prevColor.A());   pChild->setAttributeNode(pAttr);

          pChild = XMLHelper::AppendAttributeChild(pXmlDoc, pElement, "Next", "Red", variant_t(it.m_vThresholds[i].m_nextColor.R()));
          pAttr = pXmlDoc->createAttribute("Green");   pAttr->value = variant_t(it.m_vThresholds[i].m_nextColor.G());   pChild->setAttributeNode(pAttr);
          pAttr = pXmlDoc->createAttribute("Blue");    pAttr->value = variant_t(it.m_vThresholds[i].m_nextColor.B());   pChild->setAttributeNode(pAttr);
          pAttr = pXmlDoc->createAttribute("Opacity"); pAttr->value = variant_t(it.m_vThresholds[i].m_nextColor.A());   pChild->setAttributeNode(pAttr);
        }
      }

      for (WindowLevelVector::iterator liter = m_vWL.begin(); liter != m_vWL.end(); liter++)
      {
        pElThresh = XMLHelper::AppendAttributeChild(pXmlDoc, pXmlDoc->documentElement, "WindowLevelSetting", "Name", variant_t(liter->first.c_str()));
        pAttr = pXmlDoc->createAttribute("Window");     pAttr->value = variant_t(liter->second.first);
        pElThresh->setAttributeNode(pAttr);
        pAttr = pXmlDoc->createAttribute("Level");     pAttr->value = variant_t(liter->second.second);
        pElThresh->setAttributeNode(pAttr);
      }
    }

    const std::string sXml( pXmlDoc->Getxml() );
    ServiceLocator::GetConfigurationService()->SetConfiguration( ViatronApp::GetViatronApp()->GetContextId(), sXml );

    //if (FAILED(pXmlDoc->save( (rdrGlobal.GetModulePath() + "config/TranslucencySettings.xml").c_str() )))  { throw false; }
  }
  catch (...)
  {
    return false;
  }

  return true;
} // WriteSettings()


/**
 * searches window level vector by saved name
 *
 * @param sName   name of setting for which to search
 * @return        index of found setting or -1 if not found
 */
int4 ColonViews::TransSettingsMap::FindWLSetting(const std::string &sName)
{
  for (int4 iSt(0); iSt < m_vWL.size(); iSt++)
  {
    if (sName == m_vWL[iSt].first)
      return iSt;
  }

  return -1;
} // FindWLSetting


/**
 * Notification message: GC_PEGMOVED
 *
 * @param pNotifyStruct   Notification message information
 * @param pResult         Out result
 */
void AdjustTranslucencyDlg::OnNotifyPegMoved(NMHDR *pNotifyStruct, LRESULT *pResult)
{
} // OnNotifyPegMoved()


/**
 * Notification message: WM_MOUSEMOVE
 *
 * @param pNotifyStruct   Notification message information
 * @param pResult         Out result
 */
void AdjustTranslucencyDlg::OnNotifyMouseMoveGradientCtrl(NMHDR * pNotifyStruct, LRESULT *result)
{
  CPoint pt;
  GetCursorPos(&pt);
  ScreenToClient(&pt);

  const TransferFunctionVC& func = rdrGlobal.m_pCurrDataset->m_biopsyTransferFunction;
  const float4 fVal = m_gradientCtrl.GetPosAtPoint(pt) * func.GetResolution();

  SetDlgItemText(IDC_AJT_PEG_VALUE_STATIC, ToAscii(HUconverter::Uchar2HU(uint1(fVal))).c_str());
} // OnNotifyMouseMoveGradientCtrl()


/**
 * Called to set mouse cursor icon
 * 
 * @param pWnd       Pointer to window containing cursor
 * @param uHitTest   Area within dialog where cursor is location (e.g. HT_BORDER)
 * @param uMessage   Mouse message number
 * @return           TRUE if cursor was set
 */
BOOL AdjustTranslucencyDlg::OnSetCursor(CWnd* pWnd, UINT uHitTest, UINT uMessage) 
{
  SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
  return TRUE;
} // OnSetCursor()


/**
 * Enables edit functionality
 */
void AdjustTranslucencyDlg::OnEditSelected()
{
  m_gradientCtrl.ShowWindow(SW_HIDE);
  m_gradientCtrl.EnableWindow(TRUE);
  m_gradientCtrl.Invalidate(TRUE);
  m_gradientCtrl.ShowWindow(SW_SHOW);
  m_gradientCtrl.UpdateWindow();

  m_cbxPresets.EnableWindow(FALSE);
  m_btnDeleteSel.EnableWindow(FALSE);

  m_btnEditSel.SetDlgCtrlID(IDC_AJT_SAVESELECTED);
  m_btnSaveNew.SetDlgCtrlID(IDC_AJT_CANCELEDIT);
} // OnEditSelected


/**
 * Save current selections on settings
 */
void AdjustTranslucencyDlg::OnSaveSelected()
{
  // call save on current settings
  CString sName;
  m_cbxPresets.GetLBText(m_cbxPresets.GetCurSel(), sName);
  if (sName.IsEmpty())
    return;

  SaveSettings(LPCTSTR(sName));

  m_gradientCtrl.ShowWindow(SW_HIDE);
  m_gradientCtrl.EnableWindow(FALSE);
  m_gradientCtrl.Invalidate(TRUE);
  m_gradientCtrl.ShowWindow(SW_SHOW);
  m_gradientCtrl.UpdateWindow();

  m_cbxPresets.EnableWindow(TRUE);
  m_btnDeleteSel.EnableWindow(TRUE);

  m_btnEditSel.SetDlgCtrlID(IDC_AJT_EDITSELECTED);
  m_btnSaveNew.SetDlgCtrlID(IDC_AJT_SAVENEW);

} // OnSaveSelected()


/**
 * Cancel current edit on settings
 */
void AdjustTranslucencyDlg::OnCancelEditSelected()
{
  m_gradientCtrl.ShowWindow(SW_HIDE);
  m_gradientCtrl.EnableWindow(FALSE);
  m_gradientCtrl.Invalidate(TRUE);
  m_gradientCtrl.ShowWindow(SW_SHOW);
  m_gradientCtrl.UpdateWindow();

  m_cbxPresets.EnableWindow(TRUE);

  m_btnEditSel.SetDlgCtrlID(IDC_AJT_EDITSELECTED);
  m_btnSaveNew.SetDlgCtrlID(IDC_AJT_SAVENEW);

  // TODO: get saved settings for current selection
  OnSelChangePresets();

} // OnCancelEditSelected()


/**
 * Updates the volume displays to the currently-set threshold values
 */
void AdjustTranslucencyDlg::UpdateVolumeDisplay()
{
  rdrGlobal.m_pCurrDataset->m_biopsyTransferFunction.SetBiopsy();
  rdrGlobal.m_pCurrDataset->UpdateBiopsyColormap();
  rdrGlobal.m_pCurrDataset->m_fields.m_volumeRender.FlushAccumulationBuffer();

  // copy thresholds to the other dataset
  if (rdrGlobal.m_bDualDataLoaded)
  {
    const TransferFunctionVC& funcCurr = rdrGlobal.m_pCurrDataset->m_biopsyTransferFunction;
    TransferFunctionVC& funcScnd = rdrGlobal.m_pSecondaryDataset->m_biopsyTransferFunction;
    funcScnd.m_vfBiopsyThresholds.resize(funcCurr.m_vfBiopsyThresholds.size());
    for (int i(0); i < funcCurr.m_vfBiopsyThresholds.size(); i++)
    {
      funcScnd.m_vfBiopsyThresholds[i] = funcCurr.m_vfBiopsyThresholds[i];
    }

    rdrGlobal.m_pSecondaryDataset->m_biopsyTransferFunction.SetBiopsy();
    rdrGlobal.m_pSecondaryDataset->UpdateBiopsyColormap();
    rdrGlobal.m_pSecondaryDataset->m_fields.m_volumeRender.FlushAccumulationBuffer();
  }

  MainAppFrame::GetMainFrame()->PostMessage(WM_APP_GLCHILDVIEW_VIEWTYPE_INVALIDATE,
    WPARAM(GLChildView::eGVT_SLICE | GLChildView::eGVT_NAVIGATION), 0);
} // UpdateVolumeDisplay()


/**
 * Update window's visibility state.
 *
 * @param bShow     Whether or not the window is now visible.
 * @param uStatus   Source of the show window message.
 */
void AdjustTranslucencyDlg::OnShowWindow(BOOL bShow, UINT uStatus) 
{
  ViatronDialog::OnShowWindow(bShow, uStatus);
  
  // TODO: Add your message handler code here
  if (bShow)
  {
    ;
  }
  else
  {
    // copy thresholds to the other dataset
    if (rdrGlobal.m_bDualDataLoaded)
    {
      const TransferFunctionVC& funcCurr = rdrGlobal.m_pCurrDataset->m_biopsyTransferFunction;
      TransferFunctionVC& funcScnd = rdrGlobal.m_pSecondaryDataset->m_biopsyTransferFunction;
      funcScnd.m_vfBiopsyThresholds.resize(funcCurr.m_vfBiopsyThresholds.size());
      for (int i(0); i < funcCurr.m_vfBiopsyThresholds.size(); i++)
      {
        funcScnd.m_vfBiopsyThresholds[i] = funcCurr.m_vfBiopsyThresholds[i];
      }

      rdrGlobal.m_pSecondaryDataset->m_biopsyTransferFunction.SetBiopsy();
      rdrGlobal.m_pSecondaryDataset->UpdateBiopsyColormap();
      rdrGlobal.m_pSecondaryDataset->m_fields.m_volumeRender.FlushAccumulationBuffer();
      MainAppFrame::GetMainFrame()->PostMessage(WM_APP_GLCHILDVIEW_VIEWTYPE_INVALIDATE, WPARAM(GLChildView::eGVT_SLICE), 0);
    }

    WriteSettings();
  }
} // OnShowWindow()


/**
 * Revert back to saved translucency values
 */
void AdjustTranslucencyDlg::OnSaveNewSettings()
{
  std::string sName = TextPromptWnd::GetText(LPCTSTR(LoadResourceString(IDS_ENTERNEWSETTINGSNAME)), std::string(), TextPromptWnd::eSingleLine);
  if (sName.empty())
    return;

  m_btnEditSel.EnableWindow(TRUE);

  SaveSettings(sName);
} // OnSaveNewSettings()


/**
 * saves current (if sName exists) or new list of settings
 *
 * @param sName   name of new settings
 */
void AdjustTranslucencyDlg::SaveSettings(const std::string& sName)
{
  const TransferFunctionVC& func = rdrGlobal.m_pCurrDataset->m_biopsyTransferFunction;

  TranslucencySetting newStg;
  newStg.m_sName = sName;
  newStg.m_sOwner = User::GetCurrentUser().GetFullName();

  newStg.m_vThresholds.resize(func.m_vfBiopsyThresholds.size());
  for (int i(0); i < func.m_vfBiopsyThresholds.size(); i++)
  {
    newStg.m_vThresholds[i] = func.m_vfBiopsyThresholds[i];
  }

  if (m_mapSettings.find(sName) != m_mapSettings.end())
  {
    m_cbxPresets.SetCurSel(m_cbxPresets.FindString(-1, sName.c_str()));
  }
  else
  {
    const int4 iCt = m_cbxPresets.GetCount();
    m_cbxPresets.SetCurSel(m_cbxPresets.InsertString(iCt-1, sName.c_str()));

    SetCurrentPreset(iCt-1);
    OnEditSelected();
  }

  m_mapSettings[sName] = newStg;
} // OnSaveNewSettings()


/**
 * set preceding color button clicked
 */
void AdjustTranslucencyDlg::OnClickColorBtnPrev()
{
  OnClickColorButton(false);
} // OnClickColorBtnPrev()


/**
 * set following color button clicked
 */
void AdjustTranslucencyDlg::OnClickColorBtnNext()
{
  OnClickColorButton(true);
} // OnClickColorBtnNext()


/**
 * combobox selection changed
 */
void AdjustTranslucencyDlg::OnSelChangePresets()
{
  SetCurrentPreset(m_cbxPresets.GetCurSel());
} // OnSelChangePresets()


/**
 * select a settings preset
 *
 * @param iSel   value of preset
 */
void AdjustTranslucencyDlg::SetCurrentPreset(const int4 iSel)
{
  if (iSel >= m_mapSettings.size())
  {
    return;
  }

  TransferFunctionVC& func = rdrGlobal.m_pCurrDataset->m_biopsyTransferFunction;

  CString sName;
  m_cbxPresets.GetLBText(iSel, sName);
  if( strcmp( sName, SYSTEM_DEFAULT_KEY.c_str() ) == 0 )
    sName = SYSTEM_DEFAULT_ID.c_str();

  TransSettingsMap::const_iterator iter = m_mapSettings.find(LPCTSTR(sName));
  if (iter == m_mapSettings.end())
    return;

  const TranslucencySetting it = iter->second;

  func.m_vfBiopsyThresholds.resize(it.m_vThresholds.size());
  for (int i(0); i < it.m_vThresholds.size(); i++)
  {
    func.m_vfBiopsyThresholds[i] = it.m_vThresholds[i];
  }

  m_gradientCtrl.EnableWindow(FALSE);
  m_gradientCtrl.Invalidate(TRUE);
  m_gradientCtrl.UpdateWindow();

  m_btnEditSel.EnableWindow(iSel < m_mapSettings.size() - 1);
  m_btnDeleteSel.EnableWindow(iSel < m_mapSettings.size() - 1);

  UpdateVolumeDisplay();
  InitGradientDisplayValues();
} // OnSelChangePresets()


/**
 * Maps a peg index to its corresponding preceding/following color values
 *
 * @param iIndex   index of peg
 * @return         corresponding pair of color values
 */
AdjustTranslucencyDlg::LinearTFParamsPair AdjustTranslucencyDlg::PegIndexToColorPair(const int4 iIndex)
{
  std::vector<TransferFunctionVC::LinearTFParams> & vThresh = rdrGlobal.m_pCurrDataset->m_biopsyTransferFunction.m_vfBiopsyThresholds;
  LinearTFParamsPair paramPair(__nullptr, __nullptr);
  switch (iIndex)
  {
    case GradientList::NONE:
    case GradientList::BACKGROUND:
      break;

    case GradientList::STARTPEG:
      paramPair.second = &vThresh[0];
      break;

    case GradientList::ENDPEG:
      paramPair.first = &vThresh[vThresh.size()-1];
      break;

    default:
      paramPair.first = &vThresh[iIndex];
      paramPair.second = &vThresh[iIndex+1];
      break;
  }

  return paramPair;
} // PegIndexToColorPair()


/**
 * Handler for color-setting functionality
 *
 * @param bNext   Whether the following (true) or preceding (false) button was selected
 */
void AdjustTranslucencyDlg::OnClickColorButton(const bool bNext)
{
  const int4 iIndex = m_gradientCtrl.GetSelIndex();
  CColorDialog clrdlg;

  LinearTFParamsPair paramPair = PegIndexToColorPair(iIndex);

  if (bNext && (paramPair.second!=NULL))
    clrdlg.m_cc.rgbResult = RGB(paramPair.second->m_prevColor.R(), paramPair.second->m_prevColor.G(), paramPair.second->m_prevColor.B());
  else if (paramPair.first!=NULL)
    clrdlg.m_cc.rgbResult = RGB(paramPair.first->m_nextColor.R(), paramPair.first->m_nextColor.G(), paramPair.first->m_nextColor.B());

  clrdlg.m_cc.Flags |= CC_RGBINIT | CC_SOLIDCOLOR | CC_FULLOPEN;
  if (clrdlg.DoModal() == IDOK)
  {
    if (bNext && (paramPair.second!=NULL))
    {
      paramPair.second->m_prevColor.R()   = GetRValue(clrdlg.m_cc.rgbResult);
      paramPair.second->m_prevColor.G() = GetGValue(clrdlg.m_cc.rgbResult);
      paramPair.second->m_prevColor.B()  = GetBValue(clrdlg.m_cc.rgbResult);
    }
    else if (paramPair.first!=NULL)
    {
      paramPair.first->m_nextColor.R()   = GetRValue(clrdlg.m_cc.rgbResult);
      paramPair.first->m_nextColor.G() = GetGValue(clrdlg.m_cc.rgbResult);
      paramPair.first->m_nextColor.B()  = GetBValue(clrdlg.m_cc.rgbResult);
    }
  }

  SetColorButtons(iIndex);
  UpdateVolumeDisplay();
  InitGradientDisplayValues();
} // OnClickColorButton()


/**
 * called when window is being deleted
 **/
void AdjustTranslucencyDlg::OnDestroy()
{
//  WriteSettings();
} // OnDestroy()


// $Log: AdjustTranslucencyDlg.C,v $
// Revision 1.16.2.17  2010/01/07 18:40:44  dongqing
// translucency control for widescreen
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.16.2.16  2009/11/25 22:13:56  dongqing
// widescreen
//
// Revision 1.16.2.15  2008/01/31 17:30:42  jmeade
// Issue 5928: limit of 5 user-defined window/level presets.
//
// Revision 1.16.2.14  2008/01/24 21:47:48  jmeade
// fixing button sizing.
//
// Revision 1.16.2.13  2007/12/20 17:32:44  jmeade
// Issue 5940: clear WL list before read.
//
// Revision 1.16.2.12  2007/11/06 20:34:43  jmeade
// Issue 5878: interaction bug fixes.
//
// Revision 1.16.2.11  2007/11/06 03:38:27  jmeade
// placement of controls.
//
// Revision 1.16.2.10  2007/11/02 05:51:10  jmeade
// temporary delete button.
//
// Revision 1.16.2.9  2007/11/02 05:41:01  jmeade
// Issue 5583: delete setting.
//
// Revision 1.16.2.8  2007/11/02 03:09:53  jmeade
// Issue 5583: rework adjust translucency dialog interaction.
//
// Revision 1.16.2.7  2007/10/30 23:06:28  jmeade
// Issue 5863: prevent duplicates in user w/l list.
//
// Revision 1.16.2.6  2007/10/19 15:30:15  jmeade
// a vector for WL presets.
//
// Revision 1.16.2.5  2007/10/04 22:38:28  jmeade
// user option to save new window level settings.
//
// Revision 1.16.2.4  2007/10/04 21:56:38  jmeade
// user option to save new window level settings.
//
// Revision 1.16.2.3  2007/10/04 20:00:57  jmeade
// add user window/levels to translucency settings xml.
//
// Revision 1.16.2.2  2007/05/08 15:35:23  jmeade
// Issue 5647: set combobox selection when resetting system default.
//
// Revision 1.16.2.1  2007/04/04 18:51:03  jmeade
// Issue 5551: ensure current settings are saved if app is closed while AT pane is active.
//
// Revision 1.16  2007/03/14 23:36:14  jmeade
// code standards.
//
// Revision 1.15  2007/03/13 19:52:55  jmeade
// code standards.
//
// Revision 1.14  2007/03/08 22:21:42  romy
// code review - header files adjusted
//
// Revision 1.13  2007/03/07 22:33:26  jmeade
// code standards.
//
// Revision 1.12  2007/01/25 04:06:14  jmeade
// Issue 5047: correct errors with reading; replace settings if same name used.
//
// Revision 1.11  2007/01/24 22:01:16  jmeade
// Issue 5336: use correct map item for settings.
//
// Revision 1.10  2007/01/24 21:34:13  jmeade
// Issue 5336: use correct map item for settings.
//
// Revision 1.9  2007/01/09 19:14:08  jmeade
// ensure settings gets output when changing layouts.
//
// Revision 1.8  2006/12/15 14:50:45  dongqing
// add header file needing to be included
//
// Revision 1.7  2006/12/05 19:54:00  jmeade
// Issue 5047: store new settings; settings file i/o.
//
// Revision 1.6  2006/11/07 20:12:11  jmeade
// start of presets combobox; comments.
//
// Revision 1.5  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.4.2.1  2006/01/11 15:21:42  frank
// Issue #4596: Adjusted layout of adjust translucency dialog
//
// Revision 1.4  2005/10/19 21:32:45  jmeade
// update navigation view when changing translucency.
//
// Revision 1.3  2005/08/12 21:48:53  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.2  2005/08/09 20:45:28  jmeade
// namespaces for Dialogs and ChildViews.
//
// Revision 1.1  2005/08/05 12:59:44  geconomos
// moved from vx repository
//
// Revision 1.1.2.20  2005/02/15 17:02:47  jmeade
// merge from vc1.4 branch.
//
// Revision 1.1.2.19.2.2  2004/10/18 22:39:24  jmeade
// enable tooltips.
//
// Revision 1.1.2.19.2.1  2004/10/18 20:56:32  jmeade
// Update secondary dataset's translucency map on the fly.
//
// Revision 1.1.2.19  2004/08/23 17:57:38  jmeade
// icon buttons for adjust translucency dialog.
//
// Revision 1.1.2.18  2004/07/02 00:47:44  jmeade
// Set HU whereever mouse hovers over gradient control.
//
// Revision 1.1.2.17  2004/06/29 01:20:20  jmeade
// Colors in the adjust-peg-color buttons.
//
// Revision 1.1.2.16  2004/06/25 23:30:24  jmeade
// Two buttons for each peg color instead of sliders/radios.
//
// Revision 1.1.2.15  2004/06/25 08:09:34  jmeade
// 2nd pass at color editing interface, with always-visible sliders, click-and-edit, and real-time updates.
//
// Revision 1.1.2.14  2004/06/25 04:58:23  jmeade
// Fixed bug: previously not updating entire range of threshold values.
//
// Revision 1.1.2.13  2004/06/23 18:29:21  jmeade
// Display discrete color ranges (to match thresholds) in gradient display.
//
// Revision 1.1.2.12  2004/06/21 16:08:27  jmeade
// Update other study upon closing dialog.
//
// Revision 1.1.2.11  2004/06/18 04:17:39  jmeade
// Edit begin and end colors for each threshold.
//
// Revision 1.1.2.10  2004/06/17 23:05:05  jmeade
// implemented color selection changes to colormap.
//
// Revision 1.1.2.9  2004/06/17 20:16:06  vxconfig
// ordering of includes created a compile error.
//
// Revision 1.1.2.8  2004/06/17 17:44:52  jmeade
// revert saved implementation.
//
// Revision 1.1.2.7  2004/06/17 17:20:40  jmeade
// revert saved button.
//
// Revision 1.1.2.6  2004/06/17 00:21:35  jmeade
// Trigger slice views to update when translucency map changes.
//
// Revision 1.1.2.5  2004/06/17 00:00:48  jmeade
// HU display of values in adjust transluncency dialog.
//
// Revision 1.1.2.4  2004/05/19 13:09:12  geconomos
// Added calls to update the vxColormap in vxSliceRendererData anytime the biopsy transfer function is modified.
//
// Revision 1.1.2.3  2004/05/15 00:35:18  jmeade
// Allow storage/adjustment of all biopsy/transluncency values.
//
// Revision 1.1.2.2  2004/05/05 23:15:54  jmeade
// Incremental update.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/AdjustTranslucencyDlg.C,v 1.16.2.17 2010/01/07 18:40:44 dongqing Exp $
// $Id: AdjustTranslucencyDlg.C,v 1.16.2.17 2010/01/07 18:40:44 dongqing Exp $
