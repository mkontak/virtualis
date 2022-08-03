// $Id: ViatronChildView.h,v 1.8.2.2 2010/04/17 05:16:20 dongqing Exp $
//
// Copyright© 2000, 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: ViatronChildView.h
// Owner(s): Jeffrey Meade  jmeade@viatronix.net

#if !defined(AFX_VIATRONCHILDVIEW_H__FFE53149_EC54_43C6_8D30_956BC15A5F0D__INCLUDED_)
#define AFX_VIATRONCHILDVIEW_H__FFE53149_EC54_43C6_8D30_956BC15A5F0D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GLChildView.h"
#include "ViatronixWnd.h"
#include "ReaderGlobal.h"
#include "Dataset.h"

#define FILE_REVISION "$Revision: 1.8.2.2 $"

namespace ColonViews
{

  namespace ChildViews
  {

    class ViatronChildView : public GLChildView
    {
    public:
      /// Constructor
      ViatronChildView(const ViewWndTypeEnum eGLViewType = eGVT_NONE, HWND hParent = HWND(NULL),
        const ReaderLib::DatasetOrientations::TypeEnum eDatasetOrientation = ReaderLib::DatasetOrientations::ePRIMARY);
      /// Destructor
      virtual ~ViatronChildView();
      /// Menu/control/accelerator command handler
      virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

      /// Gets the default stored dataset
      inline ReaderLib::Dataset& GetDatasetRef() const;
      /// Gets the opposite of the stored dataset
      inline ReaderLib::Dataset& GetOppositeDatasetRef() const;
      /// Gets the default stored dataset
      inline ReaderLib::Dataset* GetDatasetPtr() const;
      /// Gets the opposite of the stored dataset
      inline ReaderLib::Dataset* GetOppositeDatasetPtr() const;
      /// Gets proper type info
      virtual const type_info& GetClassTypeInfo() { return typeid(*this); }

      /// Override to handle changes in view modes
      virtual void OnViewModeChange(const uint4 uOldMode, const uint4 uNewMode) {}
      /// Override to handle measurement update.
      virtual void OnUpdateMeasurement(const CPoint) {}

      /// Gets a dataset based on orientation
      static inline ReaderLib::Dataset& GetDatasetRef(const ReaderLib::DatasetOrientations::TypeEnum eDatasetOrientation);

    private:
      /// Get opposite dataset orientation
      inline ReaderLib::DatasetOrientations::TypeEnum
        GetOppositeOrientation(const ReaderLib::DatasetOrientations::TypeEnum eDatasetOrientation) const;

    private:
      /// Stored dataset
      ReaderLib::Dataset ** m_pDataset;
      ReaderLib::Dataset & m_dataset;

      /// Stored opposite dataset
      ReaderLib::Dataset ** m_pOppositeDataset;
      ReaderLib::Dataset & m_oppositeDataset;
    };

  } // namespace ChildViews

  namespace Layouts
  {

    class ViatronixChildWnd : public ViatronixWnd
    {
    public:
      /// Constructor
      ViatronixChildWnd(const uint4 uViewLayoutResourceID);

      /// Create the child views for this view window (always the one specified for this child window control)
      /// Calculate the view rectangles (the one view will fill the window)

      /// Type cast operator
      inline operator ChildViews::GLChildViewRef() { return GetChildViewByPos(0); }

      // Overrides
      // ClassWizard generated virtual function overrides
      //{{AFX_VIRTUAL(ViatronixChildWnd)
    protected:
      //}}AFX_VIRTUAL

      // Generated message map functions
    protected:
      //{{AFX_MSG(ViatronixChildWnd)
      afx_msg void OnNcLButtonDblClk(UINT uHitTest, CPoint point);
      //}}AFX_MSG
      afx_msg LRESULT OnMessageSizingQuery(WPARAM wProposedSize, LPARAM);
      /// on mouse move
      afx_msg virtual void OnMouseMove(UINT uFlags, CPoint point);

      /// returns the resource ID for indicated window orientation
      virtual uint4 GetLandscapeLayoutID()  { return m_uViewLayoutResID; }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetWidescreenLayoutID() { return m_uViewLayoutResID; }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetPortraitLayoutID()   { return m_uViewLayoutResID; }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetPortraitLargeLayoutID() { return m_uViewLayoutResID; }

      DECLARE_MESSAGE_MAP()

    private:
      /// template resource ID
      const uint4 m_uViewLayoutResID;
    };

  } // namespace Layouts

  ////////////////////////////////////////////////////////////////////////////////////////////////////

  namespace ChildViews
  {
    /// get reference to current dataset
    inline ReaderLib::Dataset& ViatronChildView::GetDatasetRef() const
    {
      return (m_pDataset ? **m_pDataset : m_dataset);
    } // GetDatasetRef()

    /// get reference to other dataset
    inline ReaderLib::Dataset& ViatronChildView::GetOppositeDatasetRef() const
    {
      return (m_pOppositeDataset ? **m_pOppositeDataset : m_oppositeDataset);
    } // GetOppositeDatasetRef()


    /// get pointer to current dataset
    inline ReaderLib::Dataset* ViatronChildView::GetDatasetPtr() const
    {
      return (m_pDataset ? *m_pDataset : &m_dataset);
    } // GetDatasetPtr()

    /// get pointer to other dataset
    inline ReaderLib::Dataset* ViatronChildView::GetOppositeDatasetPtr() const
    {
      return (m_pOppositeDataset ? *m_pOppositeDataset : &m_oppositeDataset);
    } // GetOppositeDatasetPtr()

    /// get reference to dataset by type
    inline ReaderLib::Dataset& ViatronChildView::GetDatasetRef(const ReaderLib::DatasetOrientations::TypeEnum eDatasetOrientation)
    {
      switch (eDatasetOrientation)
      {
      case ReaderLib::DatasetOrientations::eSUPINE:
        return ReaderLib::rdrGlobal.m_supine;
        break;

      case ReaderLib::DatasetOrientations::ePRONE:
        return ReaderLib::rdrGlobal.m_prone;
        break;

      case ReaderLib::DatasetOrientations::ePRIMARY:
        return *ReaderLib::rdrGlobal.m_pCurrDataset;
        break;

      case ReaderLib::DatasetOrientations::eSECONDARY:
        return *ReaderLib::rdrGlobal.m_pSecondaryDataset;
        break;

      default:
        // Illegal argument
        ex::IllegalArgumentException(LogRec("Invalid DatasetOrientationsEnum case", "ViatronChildView", "GetDataset"));
        return ReaderLib::rdrGlobal.m_nullDataset;
        break;
      }
    } // GetDatasetRef()


    inline ReaderLib::DatasetOrientations::TypeEnum
      ViatronChildView::GetOppositeOrientation(const ReaderLib::DatasetOrientations::TypeEnum eDatasetOrientation) const
    {
      switch (eDatasetOrientation)
      {
      case ReaderLib::DatasetOrientations::eSUPINE:
        return ReaderLib::DatasetOrientations::ePRONE;
        break;

      case ReaderLib::DatasetOrientations::ePRONE:
        return ReaderLib::DatasetOrientations::eSUPINE;
        break;

      case ReaderLib::DatasetOrientations::ePRIMARY:
        return ReaderLib::DatasetOrientations::eSECONDARY;
        break;

      case ReaderLib::DatasetOrientations::eSECONDARY:
        return ReaderLib::DatasetOrientations::ePRIMARY;
        break;

      default:
        // Illegal argument
        ex::IllegalArgumentException(LogRec("Invalid DatasetOrientationsEnum case", "ViatronChildView", "GetOppositeOrientation"));
        return ReaderLib::DatasetOrientations::eDEFAULT;
        break;
      } // switch (orientation)
    } // GetOppositeOrientation()

  } // namespace ChildViews

} // namespace ColonViews


#undef FILE_REVISION

#endif // !defined(AFX_VIATRONCHILDVIEW_H__FFE53149_EC54_43C6_8D30_956BC15A5F0D__INCLUDED_)

// Revision History:
// $Log: ViatronChildView.h,v $
// Revision 1.8.2.2  2010/04/17 05:16:20  dongqing
// small portrait display layouts
//
// Revision 1.8.2.1  2009/12/09 19:44:43  dongqing
// layouts will specify various IDs for different orientations -- somewhat akin to
//  the IDD= enum used in mfc dialog classes --
// and the baseclass will decide which of the orientations is needed.
//
// Revision 1.8  2007/03/07 23:07:03  jmeade
// code standards.
//
// Revision 1.7  2006/04/12 23:11:15  jmeade
// Merge from VC_2-0 branch: 060404
//
// Revision 1.6.2.1  2006/03/22 22:55:31  jmeade
// Issue 4667: restrict registration synchronization to views where both datasets are visible, and a link button is available.
//
// Revision 1.6  2005/10/25 16:51:00  jmeade
// ParentViews ==> Layouts
//
// Revision 1.5  2005/09/28 20:25:13  vxconfig
// renamed study files to dataset
//
// Revision 1.4  2005/08/12 21:48:54  jmeade
// namespaces for ReaderLib and WinUtils.
//
// Revision 1.3  2005/08/10 23:48:55  jmeade
// namespaces for Layouts.
//
// Revision 1.2  2005/08/09 20:45:28  jmeade
// namespaces for Dialogs and ChildViews.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.11.2.3.4.2  2005/06/08 19:45:28  geconomos
// result of merge of  VC_1-5 and VC_1-5_GEORGE
//
// Revision 3.11.2.3.4.1.2.1  2005/05/25 15:50:24  geconomos
// initial backup
//
// Revision 3.11.2.3.4.1  2005/04/01 23:33:22  jmeade
// Painting in the s/p panes.
//
// Revision 3.11.2.3  2003/07/25 21:45:45  jmeade
// Issue 3401: Common childview handler for main view window toggle.
//
// Revision 3.11.2.2  2003/02/27 03:34:50  jmeade
// Issue 2856:  More concise method for triggering child view updates.
//
// Revision 3.11.2.1  2003/02/22 12:23:51  jmeade
// Issue 2837:  Only one main view per 'screen' to handle messages (i.e. no child views derived from msg handling class).
//
// Revision 3.11  2002/11/25 23:28:12  jmeade
// No internal setting of pane text (let windows handle it themselves).
//
// Revision 3.10  2002/11/15 00:45:35  jmeade
// No hard-coding of # of child views; Hide child view storage (use access methods).
//
// Revision 3.9  2002/08/27 18:28:23  jmeade
// Sliding pane incremental checkin (using resource for child wind layout); Unused app messages.
//
// Revision 3.8  2002/08/22 13:20:02  jmeade
// Reverting back to before-Jeff-broke view wnd classes.
//
// Revision 3.6  2002/07/26 22:42:33  jmeade
// ViatronChildWnd, new control for a single GLChildView.
//
// Revision 3.5  2002/07/03 03:23:43  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_07-02-2002)
//
// Revision 3.4  2002/04/26 15:57:32  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.3  2002/04/12 02:17:57  jmeade
// Classes/enums/etc. misnamed as Study renamed to Dataset for clarity.
//
// Revision 3.2  2002/03/25 21:21:11  jmeade
// Cleanup of log record code; inline declarations on function definitions where appropriate.
//
// Revision 3.1.2.2  2002/06/19 20:58:11  jmeade
// Method to identify an instance's child view derivation.
//
// Revision 3.1.2.1  2002/03/28 22:32:32  jmeade
// Cleanup of log record code; inline declarations on function definitions where appropriate.
//
// Revision 3.1  2001/10/26 02:21:06  tushar
// removed errors due to changes made to logger class, updating again
//
// Revision 3.0  2001/10/14 23:02:01  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 1.1   Oct 08 2001 16:54:08   jmeade
// Consolidated code specifying study orientation; allow creator of NavigationView to specify study orientation
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ViatronChildView.h,v 1.8.2.2 2010/04/17 05:16:20 dongqing Exp $
// $Id: ViatronChildView.h,v 1.8.2.2 2010/04/17 05:16:20 dongqing Exp $
