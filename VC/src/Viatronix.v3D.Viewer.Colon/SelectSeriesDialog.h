// $Id: SelectSeriesDialog.h,v 1.4.2.1 2008/11/13 17:46:17 jmeade Exp $
//
// Copyright© 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: George Economos (george@viatronix.com)

// pragma declarations
#pragma once

/// include declarations
#include "resource.h"
#include "ShellTree.h"
#include "StudyLoadParamsEx.h"
#include "vxDOMNode.h"

namespace ColonViews
{

  namespace Dialogs
  {

    // select a study series
    class SelectSeriesDialog : public CDialog
    {
    // enums
    public:

      /// dilaog resource template id
      enum { IDD = IDD_SELECTSERIESDIALOG };

    // member functions
    public:
    	
      /// constructor
      SelectSeriesDialog( CWnd* pParent = NULL );
    	
      /// desturctor
      virtual ~SelectSeriesDialog();

      /// gets the load parameters
      StudyLoadParamsEx & GetStudyLoadParams() { return m_params; }

    protected:
    	
      /// called by the framework to exchange and validate dialog data
      virtual void DoDataExchange( CDataExchange * pDX );

      /// called in response to the WM_INITDIALOG message
      virtual  BOOL OnInitDialog( );

	    /// mfc message map
      DECLARE_MESSAGE_MAP()
      
      /// click handler for the select primary volume series button
      void OnClickedPrimaryVolumeSeries();
      
      /// click handler for the select primary preprocessed series button
      void OnClickedPrimaryPreprocessedSeries();
      
      /// click handler for the select primary cad series button
      void OnClickedPrimaryCadSeries();
      
      /// click handler for the select secondary volume series button
      void OnClickedSecondaryVolumeSeries();
      
      /// click handler for the select secondary preprocessed series button
      void OnClickedSecondaryPreprocessedSeries();
      
      /// click handler for the select secondary cad series button
      void OnClickedSecondaryCadSeries();
      
      /// click handler for the select session series button
      void OnClickedSessionSeries();

      /// handler for TCN_SELCHANGE
      void OnTabChanged( NMHDR * pNMHDR, LRESULT * pResult );

      /// handler for TCN_SELCHANGING
      void OnTabChanging( NMHDR *pNMHDR, LRESULT * pResult );

      /// handler for TVN_SELCHANGE
      void OnFilesSelectionChanged( NMHDR * pNMHDR, LRESULT * pResult );

      /// current selection in filelist changed
      void OnItemChangedFileList( );

      /// click handler for the "OK" button
      void OnClickedOk();

      /// handler for the context menu items
      void OnContextMenuItem( UINT uId );

      /// handler for the set all series context menu item
      void OnSetAll();

      /// handler for the set browse location context menu item
      void OnSetBrowseLocation();

    private:

      /// displays the "BrowseForFolder" dialog and sets the text for the specified window
      void BrowseForFolder( const uint4 uEditId );

      // forces the shell tree to refresh
      void RefreshTree();

      ///  reads the most recetly used settings from the user's configurations file
      void ReadUserSettings();

      ///  stores the most recetly used settings from the user's configurations file
      void StoreUserSettings();

      /// adds an attribute to the specified DOM node
      void AddAttribute( vxDOMNode & node, const std::string & sName, uint4 uDlgItemId );

      /// adds an attribute to the specified DOM node
      void AddAttribute( vxDOMNode & node, const std::string & sName, const std::string & sValue );

      /// helper function for retrieving the text for a control
      std::string GetDlgItemText( uint4 uDlgItemId );

    // member variables
    private:

      /// directories shell tree control
      ShellTree m_tree;

      /// tab control
      CTabCtrl m_tabs;

      /// filenames listbox
      CListBox m_list;

      /// return results
      StudyLoadParamsEx m_params;

      /// user's custom browse location
      std::string m_sUserBrowsePath;
      
    }; // class SelectSeriesDialog

  } // namespace Dialogs

} // namespace ColonViews

// $Log: SelectSeriesDialog.h,v $
// Revision 1.4.2.1  2008/11/13 17:46:17  jmeade
// display information on file selected in list.
//
// Revision 1.4  2007/03/09 15:53:30  jmeade
// code standards.
//
// Revision 1.3  2006/07/10 20:40:30  jmeade
// adding a directory file listing (to aid in directory selection).
//
// Revision 1.2  2005/11/16 14:12:10  geconomos
// added context menu to set all series to the slected directory in the shell tree
//
// Revision 1.1  2005/11/15 16:15:36  geconomos
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/SelectSeriesDialog.h,v 1.4.2.1 2008/11/13 17:46:17 jmeade Exp $
// $Id: SelectSeriesDialog.h,v 1.4.2.1 2008/11/13 17:46:17 jmeade Exp $
