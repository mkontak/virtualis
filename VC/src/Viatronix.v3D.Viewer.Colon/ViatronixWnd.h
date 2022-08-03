// $Id: ViatronixWnd.h,v 1.9.2.13 2010/04/17 05:17:12 dongqing Exp $
//
// Copyright© 2000-2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ViatronixWnd.h
// Description: Interface for GLWnd with Viatronix-built controls
// Owner(s): Jeffrey Meade  jmeade@viatronix.net


#if !defined(AFX_VIATRONIXWND_H__C0B5C950_EE66_4424_8387_703EBFD8DB6D__INCLUDED_)
#define AFX_VIATRONIXWND_H__C0B5C950_EE66_4424_8387_703EBFD8DB6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GLWnd.h"
#include "SlidingPaneView.h"

namespace ColonViews
{

  namespace Layouts
  {

    namespace V3DS
    {
      /**
       * Enums to access view class atom map
       */
      enum V3DViewClassTypeEnum
      {
        eV3DCLASSPOS_ENDOLUMINAL = 0,
        eV3DCLASSPOS_OVERVIEW,
        eV3DCLASSPOS_SLICEVIEW,
        eV3DCLASSPOS_PATIENTINFO,
        eV3DCLASSPOS_PATIENTINFO_HORIZONTAL_SMALL,
        eV3DCLASSPOS_CONTROLS,
        eV3DCLASSPOS_CONTROLS_DASHBOARD_SMALL,
        eV3DCLASSPOS_CONTROLS_DASHBOARD_LARGE,
        eV3DCLASSPOS_CONTROLS_PANE_SMALL,
        eV3DCLASSPOS_CONTROLS_PANE_LARGE,
        eV3DCLASSPOS_CONTROLS_WIDESCREEN_SMALL,
        eV3DCLASSPOS_CONTROLS_WIDESCREEN_LARGE,
        eV3DCLASSPOS_NOTATIONS,
        eV3DCLASSPOS_OVERVIEWTOOLBAR,
        eV3DCLASSPOS_MISSEDREGIONS,
        eV3DCLASSPOS_BUTTON,
        eV3DCLASSPOS_COLORPROGRESSCTRL,
        eV3DCLASSPOS_SLICESCROLLBAR,
        eV3DCLASSPOS_SLIDINGPANEVIEW,
        eV3DCLASSPOS_VIATRONCHILDWND,
        eV3DCLASSPOS_SEGMENT2DVIEW,
        eV3DCLASSPOS_SEGMENT3DVIEW,
        eV3DCLASSPOS_SEGMENTCYLINDRICALVIEW,
        eV3DCLASSPOS_PANEHEADER,
        eV3DCLASSPOS_ADJUSTTRANSLUCENCY,
        eV3DCLASSPOS_CADRESULTS,
        eV3DCLASSPOS_UDSEGMENTSMAUI,
        eV3DCLASSPOS_CONTROLCONTAINERGROUPBOX,
        eNUM_V3D_CLASSES
      }; // V3DViewClassTypeEnum
      /// 
      typedef std::map<ATOM, V3DViewClassTypeEnum> ATOMtoV3DViewClassTypeEnumMap;
      /// 
      static ATOMtoV3DViewClassTypeEnumMap v3DClassAtomIDMap;


      /**
       * Class names for v3D Controls
       */
      static const int1 * sV3DClassNames[eNUM_V3D_CLASSES] =
      {
        "Viatronix.v3D.Colon.Controls.EndoscopicView",
          "Viatronix.v3D.Colon.Controls.Overview",
          "Viatronix.v3D.Colon.Controls.Sliceview",
          "Viatronix.v3D.Colon.Controls.PatientInfo",
          "Viatronix.v3D.Colon.Controls.PatientInfo.Horizontal.Small",
          "Viatronix.v3D.Colon.Controls.ControlsDialog",
          "Viatronix.v3D.Colon.Controls.ControlsDialog.Dashboard.Small",
          "Viatronix.v3D.Colon.Controls.ControlsDialog.Dashboard.Large",
          "Viatronix.v3D.Colon.Controls.ControlsDialog.Pane.Small",
          "Viatronix.v3D.Colon.Controls.ControlsDialog.Pane.Large",
          "Viatronix.v3D.Colon.Controls.ControlsDialog.Widescreen.Small",
          "Viatronix.v3D.Colon.Controls.ControlsDialog.Widescreen.Large",
          "Viatronix.v3D.Colon.Controls.NotationsDialog",
          "Viatronix.v3D.Colon.Controls.OverviewToolbar",
          "Viatronix.v3D.Colon.Controls.MissedRegionsDialog",
          "Viatronix.v3D.Colon.Controls.Button",
          "Viatronix.v3D.Colon.Controls.ColorProgressCtrl",
          "Viatronix.v3D.Colon.Controls.SliceScrollbar",
          "Viatronix.v3D.Colon.Controls.SlidingPaneView",
          "Viatronix.v3D.Colon.Controls.ViatronChildWnd",
          "Viatronix.v3D.Colon.Controls.Segment2Dview",
          "Viatronix.v3D.Colon.Controls.Segment3Dview",
          "Viatronix.v3D.Colon.Controls.SegmentCylindricalView",
          "Viatronix.v3D.Colon.Controls.PaneHeader",
          "Viatronix.v3D.Colon.Controls.AdjustTranslucencyDialog",
          "Viatronix.v3D.Colon.Controls.CadResultsDialog",
          "Viatronix.v3D.Colon.Controls.UDSegmentsMaui",
          "Viatronix.v3D.Colon.Controls.ControlContainerGroupbox"
      }; // sV3DClassNames


      /**
       * V3D Control Styles; Note: Reuse nibbles for mutually-exclusive styles
       */
      enum DatasetOrientEnum { eSUPINE = 0x0, ePRONE = 0x1, ePRIMARY = 0x2, eSECONDARY = 0x3, eDatasetOrientBits = 0x3 };
      enum LateralOrientation { eHORIZONTAL = 0x0, eVERTICAL = 0x1, eWIDESCREEN = 0x2, ePORTRAIT = 0x20 };
      // Note: Don't use value (0x4), as it seems to always be set in return from GetExStyle()
      enum ChildClassTypeEnum { eViatronChildView = 0x0, eViatronChildWnd = 0x8, eChildClassBits = 0x8 };

      enum SliceOrientEnum   { eAXIAL = 0x00, eSAGITTAL = 0x10, eCORONAL = 0x20, eCROSS_SECTION = 0x30, eVIEWDIR = 0x40,
        eSliceOrientBits = 0x70 };
      enum EndoluminalStyles { eVERTICAL_LUMENCOVERAGE = 0x10 };
      enum BufferOverviewImageEnum { eNOBUFFERIMAGE = 0x00, eBUFFERIMAGE = 0x10 };
      enum PatientInfo       { eSINGLE_DATASET_DISPLAY = 0x10 };
      enum FullControls      { eABBREVIATED_CONTROLS = 0x10 };
      enum WindowStylesEnum  { eCAPTION = 0x80 };
      enum ButtonStyle       { eSKINBUTTON = 0x0, eSKINAUTOCHECKBUTTON = 0x1 };
      enum ColorProgressStyle { eCPS_VERTICAL = 0x1 };

      // & with this nibble to find position of (slice) child view for scrollbar
      enum SliceScrollID     { eSLICESCROLLPARENTPOS = 0xF };

      // Bit used to specify the sliding pane for a pane view????
      enum SlidingPaneIDLocEnum { eSLIDINGPANEID_BITPOS = 0xF00 }; // pretty sure this is unused

      enum BackgroundTypeEnum { eBKGD_DIALOG = 0x0, eBKGD_PANE = 0x20 };

      enum WindowSizeOptions { eFIXED_DIMENSIONS = 0x10000, eFIXED_DIM_MAXWIDTH = 0x20000, eFIXED_DIM_MAXHEIGHT = 0x30000,
        eWINDOWSZ_OPTION_BITS = 0xF0000 };

      // Bits used to specify an ID used to link a pane header and it's accompanying window
      enum PaneHeaderIDLocEnum { ePANEHEADERIDPOS = 0xF000 };

    } // namespace V3DS

    /////////////////////////////////////////////////////////////////////////////////////////////////////////


    /**
     * ViatronixWnd class
     */
    class ViatronixWnd : public GLWnd  
    {
      // Methods
    public:
      /// Default constructor
      ViatronixWnd(const uint4 uBackgroundImageBitmapID = 0);

      /// virtual destructor
      virtual ~ViatronixWnd() {}

      /// Set UI information (e.g. status/help bars) when entering/leaving a view
      void SetChildViewMisc(const ChildViews::GLChildView &view, bool bEnter);

    protected:
      /// On idle
      virtual void GLIdle();

      /// Creates GL child views and other controls
      virtual void CreateChildViews(HWND hWndThis);

      /// Handles resizing of views/controls based on window size
      virtual void CalculateViewRects(const int4 iWidth, const int4 iHeight);

      /// Gets the original view rectangle of the specified child (scaled for current parent client area)
      Rect<int4> GetLayoutRect(const uint4 uChildViewControlID) const;

      DECLARE_MESSAGE_MAP()
      /// on mouse move
      afx_msg virtual void OnMouseMove( UINT uFlags, CPoint point );
      /// on key down
      afx_msg virtual void OnKeyDown( UINT uChar, UINT uRepCnt, UINT uFlags );
      /// invalidate
      afx_msg LRESULT OnAppGLChildViewTypeInvalidate( WPARAM wpGLChildViewType, LPARAM );
      /// on destroy
      afx_msg virtual void OnDestroy();


    private:
      /// Scales the rectangle (based on the ratio of dimension of the window's rectangle to its real parent rectangle)
      static void ScaleMyRect(const CRect & windowRect, const CRect & parentRect, Rect<int4> & scaledRect);
      /// An empty WndProc method used for WndClass registration
      static LRESULT CALLBACK DoNothingWNDPROC(HWND, UINT, WPARAM, LPARAM);
      /// Registers a v3D Window class
      static bool RegisterV3DClass(const std::string & sName, ATOM & outAtomID);

      // Structures
    private:
      /**
       * Structure to store information during creating of child views.
       */
      class CreateChildEnumStruct
      {
      public:
        /// Constructor
        CreateChildEnumStruct(std::list<ChildViews::GLChildViewPtr> & viewsListRef, std::list<CWnd*> & controlsListRef, const HWND hParent)
          : m_viewsList(viewsListRef), m_controlsList(controlsListRef), m_hParent(hParent), m_pPaneManager(NULL) {  }

          /// Creates a single child during an iteration of the child window enumeration
          static BOOL CALLBACK CreateProc(HWND hwnd, LPARAM lParam);

      public:
        /// Array of GL child views
        std::list<ChildViews::GLChildViewPtr> & m_viewsList;
        /// List of other windows controls, dialogs, etc
        std::list<CWnd *> & m_controlsList;

        /// Assume for now a single pane manager
        // TODO: Allow, using a list or map, for multiple pane views
        Panes::SlidingPaneManager * m_pPaneManager;

        /// Parent window
        const HWND m_hParent;
        /// Maps IDs for pane headers to same for sliding panes (children windows)
        typedef std::map<uint4,HWND> MapPaneIDToWnd;
        MapPaneIDToWnd m_mapPaneChildID;
      }; // CreateChildEnumStruct

    public:
      /// threshold defining a small application window
      static const CSize m_szSmallThreshold;

      /// the current size of the window (set in MainAppFrame::OnSize, since we can't detect at window creation time here)
      static CSize m_szAppWindow;

      // Members
    protected:
      /// Bit-pattern of GLChildView::ViewTypeEnum, child view types requiring refresh (between WM_PAINT calls)
      ///  (i.e. have been invalidated via WM_APP_GL_CHILDVIEW_INVALIDATE msg)
      static uint4 m_uInvalidChildViews;

      /// Resource IDs for template layouts of the window
      virtual uint4 GetLandscapeLayoutID() = 0;
      virtual uint4 GetWidescreenLayoutID() = 0;
      virtual uint4 GetPortraitLayoutID() = 0;
      virtual uint4 GetPortraitLargeLayoutID() = 0;

    private:
      /// Client rectangle (for sizing) of the window layout "template" dialog resource
      CRect m_layoutClientRect;

      class ChildViewOptions
      {
      public:
        Rect<int4> m_rctDisplay;
        bool m_bSquare;
      };

      /// Stores the window rectangles of each dialog control in layout dialog coordinates
      typedef std::map< uint4, ChildViewOptions > DlgCtlID_To_ChildOptionsMap;
      DlgCtlID_To_ChildOptionsMap m_childToOptionsMap;

      // indicates if the runtime instance is a leaf
      bool m_bIsLeafClass;
    }; // ViatronixWnd
  
  } // namespace Layouts

} // namespace ColonViews

#endif // !defined(AFX_VIATRONIXWND_H__C0B5C950_EE66_4424_8387_703EBFD8DB6D__INCLUDED_)

// Revision History:
// $Log: ViatronixWnd.h,v $
// Revision 1.9.2.13  2010/04/17 05:17:12  dongqing
// small portrait display layouts
//
// Revision 1.9.2.12  2010/04/17 04:53:06  dongqing
// small portrait display layouts
//
// Revision 1.9.2.11  2010/04/16 18:01:34  dongqing
// specify pane backgrounds in the resource file
//
// Revision 1.9.2.10  2010/04/10 23:55:40  dongqing
// centering controls dialog children on the fly
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.9.2.9  2010/04/09 02:34:41  dongqing
// portrait changes
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.9.2.8  2010/04/07 00:21:43  dongqing
// created a user control for progress ctrl, seeing as how -- strangely -- the basic control ignores the 'vertical' setting
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.9.2.7  2010/02/01 19:21:55  dongqing
// reload small/larger layouts if necessary while user is resizing
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.9.2.6  2010/01/31 02:13:55  dongqing
// support for vertical lumen coverage progress bar; toggle missed regions/cad dialogs
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.9.2.5  2010/01/14 14:48:05  dongqing
// displaying info of both datasets on dual-dataset layouts
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.9.2.4  2010/01/07 21:50:37  dongqing
// proper layout of 3D segments view
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.9.2.3  2009/12/09 19:39:45  dongqing
// layouts will specify various IDs for different orientations, and the baseclass will decide which orientation is needed.
// somewhat similar to the IDD= enum using in mfc dialogs
//
// Revision 1.9.2.2  2009/12/04 18:23:51  dongqing
// specifying fixed-square-size views in resource
//
// Revision 1.9.2.1  2009/11/24 23:01:27  dongqing
// localized all missed regions list functionality to a single control class.
//
// Revision 1.9  2007/03/07 23:07:03  jmeade
// code standards.
//
// Revision 1.8  2007/02/15 00:08:22  jmeade
// Merge_from_VC_2-0_on_070214
//
// Revision 1.7  2007/01/03 06:22:27  jmeade
// future ud segments layout.
//
// Revision 1.6  2005/11/15 16:17:18  geconomos
// modified loading process to use new multi-directory structure
//
// Revision 1.5.2.2  2006/05/22 12:39:15  geconomos
// virtual destructor
//
// Revision 1.5.2.1  2006/05/16 12:38:28  geconomos
// add WM_DESTROY handler to destroy non "view" based  children
//
// Revision 1.5  2005/10/28 17:48:33  geconomos
// renamed files from GLViewWnd => GLWnd
//
// Revision 1.4  2005/10/25 16:51:00  jmeade
// ParentViews ==> Layouts
//
// Revision 1.3  2005/09/14 14:41:53  jmeade
// cad results dialog, initial checkin.
//
// Revision 1.2  2005/08/10 23:48:55  jmeade
// namespaces for Layouts.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 1.1.2.6.4.3  2005/06/08 19:45:28  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 1.1.2.6.4.2  2005/06/03 15:05:26  jmeade
// abbreviated controls pane.
// Issue #4049: Added mouse text for overview images in sliding panes
//
// Revision 1.1.2.6  2004/08/19 18:21:09  jmeade
// Allow child windows to handle their own "Invalidate" calls.
//
// Revision 1.1.2.5  2004/05/05 23:20:44  jmeade
// Create adjust-translucency-dlg from resource.
//
// Revision 1.1.2.4  2003/05/05 21:40:25  jmeade
// Comments.
//
// Revision 1.1.2.3  2003/04/15 21:56:54  jmeade
// Issue 3001: Method to return the original scaled view rect from the layout resource.
//
// Revision 1.1.2.2  2003/02/27 03:34:50  jmeade
// Issue 2856:  More concise method for triggering child view updates.
//
// Revision 1.1.2.1  2003/02/22 12:21:10  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ViatronixWnd.h,v 1.9.2.13 2010/04/17 05:17:12 dongqing Exp $
// $Id: ViatronixWnd.h,v 1.9.2.13 2010/04/17 05:17:12 dongqing Exp $
