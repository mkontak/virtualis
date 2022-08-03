// $Id: AdjustTranslucencyDlg.h,v 1.9.2.12 2010/01/18 13:25:55 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: AdjustTranslucencyDlg.h
// Description: Dialog to adjust colors of translucent rendering 
// Owner: Jeff Meade [jmeade@viatronix.com]
//

#if !defined(AFX_ADJUSTTRANSLUCENCY_H__3CA71NFL_FC59_41NBA_849A_MLB1934NHL8B__INCLUDED_)
#define AFX_ADJUSTTRANSLUCENCY_H__3CA71NFL_FC59_41NBA_849A_MLB1934NHL8B__INCLUDED_

#pragma once

#include "ViatronDialog.h"
#include "GradientCtrl.h"
#include "resource.h"
#include "TransferFunctionVC.h"

namespace ColonViews
{

  /// translucency setting structure
  class TranslucencySetting
  {
  public:
    /// list of settings
    std::vector<TransferFunctionVC::LinearTFParams> m_vThresholds;
    /// settings name
    std::string m_sName;
    /// username, creator of the settings
    std::string m_sOwner;
  }; // class TranslucencySetting

  /// The list of translucency settings
  typedef std::vector< std::pair< std::string, std::pair<int4,int4> > > WindowLevelVector;
  class TransSettingsMap : public std::map< std::string, TranslucencySetting >
  {
  public:
    /// reads settings
    bool ReadSettings();
    /// writes settings
    bool WriteSettings();
    /// searches window level vector by saved name
    int4 FindWLSetting(const std::string &sName);
  public:
    /// list of user-saved window level settings
    WindowLevelVector m_vWL;
    /// we are fixing a maximum number of user-saved window/level settings
    static const int4 iMaxWLVectorSize = 5;
  }; // class TransSettingsMap

  namespace Dialogs
  {

    /**
     * Interface used to adjust the look of the translucency features (pseudo-color in 2D and virtual biopsy in 3D)
     */
    class AdjustTranslucencyDlg : public ViatronDialog
    {
      /// dynamic creation
      DECLARE_DYNAMIC(AdjustTranslucencyDlg)

    public:
      /// Dialog Data
      enum { IDD = IDD_ADJUST_TRANSLUCENCY, IDD_LARGE = IDD_ADJUST_TRANSLUCENCY_LARGE, IDD_WIDESMALL = IDD_ADJUST_TRANSLUCENCY_WIDESMALL, IDD_WIDE = IDD_ADJUST_TRANSLUCENCY_WIDESCREEN  };

    public:
      /// Constructor
      AdjustTranslucencyDlg(CWnd* pParent = NULL, const uint4 uDlgBackgroundResourceID = IDB_ENDOVIEW_HORIZ_DLGBAR);
      /// Destructor
      virtual ~AdjustTranslucencyDlg();

      /// called before the current layout changes
      virtual void OnLayoutChanging();

      /// called after the current layout changes
      virtual void OnLayoutChanged();

      /// stores the currently selected set of thresholds in the registry
      static void SaveCurrentSettingsToRegistry();

    protected:
      /// Overrides
      /// ClassWizard generated virtual function overrides
      //{{AFX_VIRTUAL(AdjustTranslucencyDlg)
      // DDX/DDV support
      virtual void DoDataExchange(CDataExchange* pDX);
      /// dialog init
      virtual BOOL OnInitDialog();
      /// dialog destroy
      virtual void OnDestroy();
      //}}AFX_VIRTUAL

      //{{AFX_MSG(AdjustTranslucencyDlg)
      /// dialog create
      afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
      /// set cursor msg
      afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT uHitTest, UINT uMessage);
      /// edit the selected settings
      afx_msg void OnEditSelected();
      /// save the selected settings
      afx_msg void OnSaveSelected();
      /// save the selected settings
      afx_msg void OnCancelEditSelected();
      /// show window message
      afx_msg void OnShowWindow(BOOL bShow, UINT uStatus);
      //}}AFX_MSG

      /// on peg selection changed
      afx_msg void OnNotifyChangeSelPeg(NMHDR *pNotifyStruct, LRESULT *pResult);
      /// on peg moved
      afx_msg void OnNotifyPegMove(NMHDR *pNotifyStruct, LRESULT *pResult);
      /// after peg moved
      afx_msg void OnNotifyPegMoved(NMHDR *pNotifyStruct, LRESULT *pResult);
      /// on peg edit
      afx_msg void OnNotifyEditPeg(NMHDR * pNotifyStruct, LRESULT *result);
      /// mouse moved over gradient control
      afx_msg void OnNotifyMouseMoveGradientCtrl(NMHDR * pNotifyStruct, LRESULT *result);
      /// store new settings
      afx_msg void OnSaveNewSettings();
      /// edit peg colours
      afx_msg void OnEditColors();
      /// previous peg colour button clicked
      afx_msg void OnClickColorBtnPrev();
      /// next peg colour button clicked
      afx_msg void OnClickColorBtnNext();
      /// presets combobox selchange
      afx_msg void OnSelChangePresets();

      afx_msg void OnRButtonDownEditSel();
      afx_msg void OnClickedDelete();
      DECLARE_MESSAGE_MAP()

      /// on size msg
      afx_msg virtual void OnSize(UINT uType, int cx, int cy);

    private:
      /// update entry colors and positions from current biopsy thresholds
      void InitGradientDisplayValues();

      /// Sends messages and call methods to update viewer displays throughout application
      void UpdateVolumeDisplay();

      /// Handles clicks on either of the set-color buttons
      void OnClickColorButton(const bool bNext);

      /// previous/next peg colour pair
      typedef std::pair<TransferFunctionVC::LinearTFParams*,TransferFunctionVC::LinearTFParams*> LinearTFParamsPair;
      /// Get the corresponding pair of colors associated with a peg index
      LinearTFParamsPair PegIndexToColorPair(int4 iIndex);

      /// Update the color buttons
      void SetColorButtons(const int4 iIndex);

      /// Read translucency settings file
      bool ReadSettings();

      /// Write translucency settings file
      bool WriteSettings();

      /// select a settings preset
      void SetCurrentPreset(const int4 iSel);

      /// saves current (if sName exists) or new list of settings
      void SaveSettings(const std::string& sName);

    private:
      /// Flat button with a single solid color
      class ColorButton : public CButton
      {
      public:
        /// Constructor
        ColorButton() : m_colour(0x0) { }
        /// Set button colour
        void SetColor(COLORREF clr) { m_colour = clr; if (IsWindow(*this)) Invalidate(TRUE); }
        /// Get button colour
        COLORREF GetColor() { return m_colour; }

      private:
        /// Draw the button
        virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItem)
        {
          CDC &dc = *CDC::FromHandle(lpDrawItem->hDC);
          CRect rect;
          GetClientRect(rect);
          if (IsWindowEnabled())
          {
            dc.FillSolidRect(rect, m_colour);
            dc.Draw3dRect(rect, 0x0, 0x0);
            dc.Draw3dRect(rect - CPoint(1,1), RGB(255,255,255), RGB(255,255,255));
          }
          else
          {
            dc.FillSolidRect(rect, 0x808080);
          }
        } // DrawItem

      private:
        /// Button colour
        COLORREF m_colour;
      }; // ColorButton

    private:
      /// Previous range end-color button
      ColorButton m_btnPrev;
      /// Next range start-color button
      ColorButton m_btnNext;

      /// save new settings
      WinUtils::SkinButton m_btnSaveNew;
      /// edit current settings
      WinUtils::SkinButton m_btnEditSel;
      /// delete current settings
      WinUtils::SkinButton m_btnDeleteSel;

      /// Gradient control
      WinUtils::GradientCtrl m_gradientCtrl;
      /// Store the thresholds when first entering the adjustment dialog to revert to if requested
      /// The list of translucency settings
      TransSettingsMap m_mapSettings;

      /// Presets combo box
      CComboBox m_cbxPresets;
    };

  } // namespace Dialogs

} // namespace ColonViews

#endif

// $Log: AdjustTranslucencyDlg.h,v $
// Revision 1.9.2.12  2010/01/18 13:25:55  dongqing
// 1280x1024 provisions
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.9.2.11  2010/01/18 11:02:39  dongqing
// 1280x1024 provisions
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.9.2.10  2010/01/07 18:40:44  dongqing
// translucency control for widescreen
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.9.2.9  2008/01/31 17:30:42  jmeade
// Issue 5928: limit of 5 user-defined window/level presets.
//
// Revision 1.9.2.8  2007/11/02 05:51:10  jmeade
// temporary delete button.
//
// Revision 1.9.2.7  2007/11/02 05:41:02  jmeade
// Issue 5583: delete setting.
//
// Revision 1.9.2.6  2007/11/02 03:09:53  jmeade
// Issue 5583: rework adjust translucency dialog interaction.
//
// Revision 1.9.2.5  2007/10/30 23:06:28  jmeade
// Issue 5863: prevent duplicates in user w/l list.
//
// Revision 1.9.2.4  2007/10/19 15:30:15  jmeade
// a vector for WL presets.
//
// Revision 1.9.2.3  2007/10/04 21:56:38  jmeade
// user option to save new window level settings.
//
// Revision 1.9.2.2  2007/10/04 20:00:57  jmeade
// add user window/levels to translucency settings xml.
//
// Revision 1.9.2.1  2007/04/04 18:51:03  jmeade
// Issue 5551: ensure current settings are saved if app is closed while AT pane is active.
//
// Revision 1.9  2007/03/08 22:21:42  romy
// code review - header files adjusted
//
// Revision 1.8  2007/03/07 22:33:26  jmeade
// code standards.
//
// Revision 1.7  2007/01/09 19:14:08  jmeade
// ensure settings gets output when changing layouts.
//
// Revision 1.6  2006/12/05 19:54:00  jmeade
// Issue 5047: store new settings; settings file i/o.
//
// Revision 1.5  2006/11/07 20:12:11  jmeade
// start of presets combobox; comments.
//
// Revision 1.4  2006/01/27 20:31:37  jmeade
// Merge from VC_2-0: 051121 to present
//
// Revision 1.3.4.1  2006/01/11 15:21:42  frank
// Issue #4596: Adjusted layout of adjust translucency dialog
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
// Revision 1.1.2.10  2005/02/15 17:02:47  jmeade
// merge from vc1.4 branch.
//
// Revision 1.1.2.9.2.2  2004/10/18 20:56:32  jmeade
// Update secondary dataset's translucency map on the fly.
//
// Revision 1.1.2.9.2.1  2004/10/14 18:13:11  vxconfig
// one-time compile header directives.
//
// Revision 1.1.2.9  2004/08/23 17:57:38  jmeade
// icon buttons for adjust translucency dialog.
//
// Revision 1.1.2.8  2004/07/02 00:47:44  jmeade
// Set HU whereever mouse hovers over gradient control.
//
// Revision 1.1.2.7  2004/06/29 01:20:20  jmeade
// Colors in the adjust-peg-color buttons.
//
// Revision 1.1.2.6  2004/06/25 23:30:24  jmeade
// Two buttons for each peg color instead of sliders/radios.
//
// Revision 1.1.2.5  2004/06/25 08:12:50  jmeade
// 2nd pass at color editing interface, with always-visible sliders, click-and-edit, and real-time updates.
//
// Revision 1.1.2.4  2004/06/18 04:17:39  jmeade
// Edit begin and end colors for each threshold.
//
// Revision 1.1.2.3  2004/06/17 17:20:41  jmeade
// revert saved button.
//
// Revision 1.1.2.2  2004/05/05 23:15:55  jmeade
// Incremental update.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/AdjustTranslucencyDlg.h,v 1.9.2.12 2010/01/18 13:25:55 dongqing Exp $
// $Id: AdjustTranslucencyDlg.h,v 1.9.2.12 2010/01/18 13:25:55 dongqing Exp $
