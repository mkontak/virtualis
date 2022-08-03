// $Id: ReportColonFindings.h,v 1.6.2.7 2010/10/26 20:05:03 dongqing Exp $
//
// Copyright© 2000, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Description: Work area for colon findings
// Owner: George Economos

#ifndef ReportColonFindings_h_
#define ReportColonFindings_h_

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif


#include "ReportWndBase.h"
#include "ReportControls.h"
#include "ReportGroupCtrl.h"
#include "ReportOtherFindings.h"

#include "resource.h"

namespace ColonReport
{
  /**
   * isplay colonic findings
   */
  class ReportColonFindings : public ReportWndBase
  {
    DECLARE_DYNAMIC(ReportColonFindings)

  // enums and constants
  public:

    /// types of findings
    enum ColonFindingSubEnum { COLONIC_NORMAL = 0, COLONIC_OTHER = 1, COLONIC_EXTRA = 2};

    //{{AFX_DATA(ReportColonFindings)
    enum { IDD = IDD_RPT_COLON_FINDINGS };

  protected:

    /// Value of the finding-type radio buttons
    enum FindingRadioValue {NORMAL = 0, POLYP = 1, DIVERTICULUM = 2, STENOSIS = 3, OTHER = 4, EXTRACOLONIC = 5 };

  // construction
  public:

    /// constructor
    ReportColonFindings(CWnd* pParent = NULL);

  // member functions
  public:

    /// sets the revision for the form
    virtual void SetReport(ReportLib::IReportRevisionPtr& spRevision);

    /// called when form is deactivated
    virtual void OnReportViewUnactivate();

    /// resets form to default state
    void Reset();

    /// Determines whether a finding is supine or prone, -1 if indeterminate (from the name, as the position was unfortunately included in report entry design)
    static ReportLib::DatasetPositionEnumProp MapFindingNameToDatasetPosition(const std::string& sFindingName);

    //{{AFX_VIRTUAL(ReportColonFindings)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

  protected:

    //{{AFX_MSG(ReportColonFindings)
    afx_msg void OnBeginDragEntries(NMHDR* pNMHDR, LRESULT* pResult);

    /// handles begin to drag images
    afx_msg void OnBeginDragImages(NMHDR* pNMHDR, LRESULT* pResult);

    /// handles delete of images
    afx_msg void OnDeleteItemImages(NMHDR* pNMHDR, LRESULT* pResult);

    /// handle init of dialog
    virtual BOOL OnInitDialog();

    /// Enter/OnOK
    virtual void OnOK();

    /// Esc/OnCancel
    virtual void OnCancel();

    /// handle delete of item entries
    afx_msg void OnDeleteItemEntries(NMHDR* pNMHDR, LRESULT* pResult);

    /// handle item changed entries
    afx_msg void OnItemChangedEntries(NMHDR* pNMHDR, LRESULT* pResult);

    /// handle item changed organ
    afx_msg void OnItemChangedOrgan(NMHDR* pNMHDR, LRESULT* pResult);

    /// handle item changed finding description
    afx_msg void OnItemChangedFindingDesc(NMHDR* pNMHDR, LRESULT* pResult);

    /// handle left button up
    afx_msg void OnLButtonUp(UINT uFlags, CPoint point);

    /// handle mouse move
    afx_msg void OnMouseMove(UINT uFlags, CPoint point);

    /// handle item changed group
    afx_msg void OnItemChangedGroupFinding(NMHDR* pNMHDR, LRESULT* pResult);

    /// handle right click on images
    afx_msg void OnRClickImages(NMHDR* pNMHDR, LRESULT* pResult);

    /// handle right click on entries
    afx_msg void OnRClickEntries(NMHDR* pNMHDR, LRESULT* pResult);

    /// handle rpt jump to finding view
    afx_msg void OnRptJumpToFindingView();

    /// handle rpt delete entry
    afx_msg void OnRptDeleteEntry();

    /// handle rpt distance uncertain
    afx_msg void OnRptDistanceUncertain();

    /// handle rpt delete image
    afx_msg void OnRptDeleteImage();

    /// handle rpt delete all findings
    afx_msg void OnRptDeleteAllFindings();

    /// handle rpt sort findings
    afx_msg void OnRptSortFindingsByDistance();

    /// handle finding normal
    afx_msg void OnFindingNormal();

    /// handle finding colonic polyp
    afx_msg void OnFindingColonicPolyp();

    /// handle finding colonic divertculum
    afx_msg void OnFindingColonicDivertculum();

    /// handle finding colonic stenosis
    afx_msg void OnFindingColonicStenosis();

    /// handle finding colonic other
    afx_msg void OnFindingColonicOther();

    /// handle finding extra colonic
    afx_msg void OnFindingExtraColonic();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    /// initializes all radio button controls on the form
    void CreateRadioGroups();

    /// deletes an entry from the revision
    BOOL DeleteEntry(LPCTSTR pszEntryID);

    /// finds an entry within the entries list control
    int4 FindEntry(LPCTSTR pszEntryID) const;

    /// selects an entry within the entries list control
    BOOL SelectEntry(LPCTSTR pszEntryID);

    /// deletes an entry within the entries list control
    BOOL DeleteImage(LPCTSTR pszEntryID);

    /// finds an image within the images list control
    int4 FindImage(LPCTSTR pszEntryID) const;

    /// sets the current finding and updates the form accordingly
    void SetFinding(const ReportLib::ColonFindingEnum eFinding, const ColonFindingSubEnum eSubFinding = COLONIC_NORMAL);

    /// adds all images for a given entry to the image list control
    void AddImages(const bool bExtraColonic=false);

    /// clears all images from the images list control
    BOOL ClearImages();

    /// creates drag image for drag and drop operation
    CImageList* CreateDragImage(int4 iItem, CPoint& pt);

    /// called new entry in entries list control is selected
    BOOL OnEntryChanged(LPCTSTR pszEntryID);

    // called before entries change in the entries list control
    BOOL OnEntryChanging(LPCTSTR pszEntryID);

    /// Removing finding from the extra-colonic finding list after user flips the associated
    /// extra-colonic finding entry back to a colonic finding.
    void RemoveFlippedItemFromOtherFindingList( ReportLib::IReportEntryPtr & spEntry );

    /// called when an image is dropped and entry in entries list control
    BOOL OnDropEntries(LPCTSTR pszTargetEntryID, LPCTSTR pszSourceImageID);

    /// reset the form do a default state
    void ResetForm();

    /// updates the location radio button group.
    void UpdateLocationRadios(const ReportLib::LocationEnum eLocation, const bool bExtraColonic);

    /// List item compare function (for sorting)
    static int CALLBACK EntriesListItemCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

  private:
    /// delete the sub-finding window
    void DeleteSubFindingDialog();
    /// on sub-finding window un-activate
    void OnSubFindingDialogUnactivate();

  // member variables
  public:

    CEdit m_wndOtherComments;
    int4 m_iFindingType;
    //}}AFX_DATA

  protected:
    /// maximum size spin edit control 
    ReportSpinEdit m_wndMaximumSize;

    /// Perpendicular size spin edit
    ReportSpinEdit m_wndPerpendicularSize;

    /// currently selected entry's ID
    CString m_sCurrentEntry; 

    /// associated data for Image items
    struct ImageListData
    {
      CString m_sImageID;
    };

    /// associated data for Entry items
    struct EntryListData
    {
      CString m_sEntryID;
    };

    /// entries list control
    ReportListCtrl m_wndEntries;

    /// images list control
    CListCtrl m_wndImages;

    /// default image size for image item
    CSize m_szImage;

    /// drag image list
    CImageList* m_pDragImage;

    /// currently selected drop target
    int4 m_iDropItem;

    /// bounding rectangle of entries list control
    CRect m_rcEntries;

    /// location radio button control
    ReportGroupCtrl m_wndGroupLocation;

    /// shapes radio button control
    ReportGroupCtrl m_wndGroupShape;

    /// translucency radio button control
    ReportGroupCtrl m_wndGroupFeatures;

    /// extra-colonic finding, organ specification control
    ReportGroupCtrl m_wndGroupOrgan;

    /// extra-colonic finding, description control
    ReportGroupCtrl m_wndGroupFindingDesc;

    /// If we need to clear/add images.
    bool m_bExtraColonic;

    /// Check box for near-flexure location value
    CButton m_wndcheckFlexure;

    /// map of all groups
    TOrganGroupMap m_mapGroupNames;
    /// map of findings
    TSubFindingMap m_mapSubFindings;

private:

    /// extra-colonic information
    ReportSubFinding * m_pWndSubFinding;

    /// differentiate between user and code selection of finding description
    bool m_bUserClickedFindingDesc;


    /// delimiter for c-rads and exam quality kludges
    static const char m_sHiddenTextDelimiter[3];
    /// newlines within the hidden text
    static const char m_sHiddenTextNewline[3];

  };

} // namespace ColonReport

#endif

// Revision History:
// $Log: ReportColonFindings.h,v $
// Revision 1.6.2.7  2010/10/26 20:05:03  dongqing
// Fix the OtherFinding list did not update after user change some
// entry from Extra colonic to colonic findings
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.6.2.6  2010/10/19 20:06:51  dongqing
// E-catagory change for extra-colonic finding display no longer needs
// to visit Patient Info pane for updating.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.6.2.5  2010/03/30 07:50:17  dongqing
// prevent dialog from disappearing
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.6.2.4  2009/07/30 17:58:20  kchalupa
// Code Walkthrough
//
// Revision 1.6.2.3  2008/03/14 01:16:43  jmeade
// Issue 6023: allow the 1st findings description entry to be set when a user is changing organ description.
//
// Revision 1.6.2.2  2008/02/14 09:51:56  jmeade
// Issue 5924: save other finding info when necessary.
//
// Revision 1.6.2.1  2008/02/12 20:32:25  jmeade
// Issue 5924: adding other-findings info to extra-colonic colon findings.
//
// Revision 1.6  2007/03/10 05:40:52  jmeade
// code standards.
//
// Revision 1.5  2007/03/10 05:06:25  jmeade
// code standards.
//
// Revision 1.4  2007/02/09 00:11:48  jmeade
// Issue 5219: Implementing C-RADS: morphology, other findings E0-E4, failed colonoscopy indication.
//
// Revision 1.3  2006/06/01 20:22:08  frank
// updated to visual studio 2005
//
// Revision 1.2  2005/09/13 17:21:07  jmeade
// ColonReport namespace.
//
// Revision 1.1  2005/08/05 12:59:45  geconomos
// moved from vx repository
//
// Revision 3.7.6.7  2005/02/15 19:46:33  jmeade
// merge from vc1.4 branch.
//
// Revision 3.7.6.6.2.1  2004/10/22 02:41:54  jmeade
// jump to colon finding view implemented.
//
// Revision 3.7.6.6  2004/07/17 02:18:43  jmeade
// Method to search a finding name to determine whether the finding is a supine or prone study finding.
//
// Revision 3.7.6.5  2004/05/14 17:25:17  jmeade
// Added a private def'n.
//
// Revision 3.7.6.4  2004/03/30 23:45:46  jmeade
// Issue 3590: Fix bug where more than one finding type could be highlighted.
//
// Revision 3.7.6.3  2004/03/27 00:38:42  jmeade
// Issue 3590: New shape classifications; replaced for location radios with ReportGroupCtrl.
//
// Revision 3.7.6.2  2003/04/14 16:47:12  jmeade
// Replaced hard-coded button vector.
//
// Revision 3.7.6.1  2003/04/09 22:03:18  jmeade
// Coding standards.
//
// Revision 3.7  2002/07/03 03:23:42  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_07-02-2002)
//
// Revision 3.6  2002/04/26 15:57:31  jmeade
// Merged with VC_1-2_CS-1 Branch (tag: Joined_On_04-18-2002)
//
// Revision 3.5  2002/03/28 18:57:52  jmeade
// Spelling errors; More strings to string table.
//
// Revision 3.4.2.2  2002/06/12 02:25:23  jmeade
// Code standards.
//
// Revision 3.4.2.1  2002/04/03 01:04:25  jmeade
// Catch crash-causing exception when no image and Extra-Colonic is selected.
//
// Revision 3.4  2002/01/15 20:49:06  binli
// issue 1424: option to show/not show distance in report
//
// Revision 3.3  2002/01/07 21:43:46  binli
// Report adjustment: no display of overview imge in ExtraColonicFinding
//
// Revision 3.2  2002/01/03 18:57:21  binli
// Report adjustment: locations on selection of extra-colonic and normal colonic
//
// Revision 3.1  2002/01/02 22:31:20  binli
// Report adjustment: added 'Extra-Colonic'
//
// Revision 3.0  2001/10/14 23:01:59  ingmar
// set revision of all files to 3.0 as part of pvcs to cvs move
//
// Revision 1.1.1.1  2001/10/14 22:05:46  ingmar
// moved from pvcs
//
// 
//    Rev 2.0   Sep 16 2001 23:41:40   ingmar
// assigned file version to 2.0 and hooked *.dsp files to pvcs
// 
//    Rev 1.0   Sep 16 2001 17:28:30   ingmar
// Initial revision.
// Revision 1.14  2001/05/30 14:23:21  jmeade
// coding standards to fix name clashes
//
// Revision 1.13  2001/05/10 21:04:39  geconomos
// Renamed V1kobj to ReportLib
//
// Revision 1.12  2001/04/18 15:49:32  geconomos
// coding standards
//
// Revision 1.11  2001/04/13 02:34:11  jmeade
// headers and footers
//
// Revision 1.10  2001/03/30 18:12:50  geconomos
// Added ability to delete all colon findings.
//
// Revision 1.9  2001/02/07 16:25:03  geconomos
// Added revisions to report file
//
// Revision 1.8  2001/02/01 16:35:21  geconomos
// Added comment headers and footers
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ReportColonFindings.h,v 1.6.2.7 2010/10/26 20:05:03 dongqing Exp $
// $Id: ReportColonFindings.h,v 1.6.2.7 2010/10/26 20:05:03 dongqing Exp $
