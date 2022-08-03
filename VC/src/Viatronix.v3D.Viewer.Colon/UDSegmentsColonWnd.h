// $Id: UDSegmentsColonWnd.h,v 1.2.2.2 2010/04/17 05:16:38 dongqing Exp $
//
// Copyright © 2000-2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Kevin Kreeger (kevin@viatronix.com)


// pragmas
#pragma once


// defines


// includes
#include "ViatronViewWnd.h"
#include "ChildViewFrame.h"
#include "ReaderGlobal.h"
#include "V1KMessages.h"


/////////////////////////////////////////////////////////////////////////////
// SegmentsCylindricalViewFrame frame


// namespaces
namespace ColonViews
{

  namespace Layouts
  {

    /**
     * new undistorted segments layout
     */
    class UDSegmentsLayoutFrame : public ChildLayoutFrame
    {
      DECLARE_DYNCREATE(UDSegmentsLayoutFrame)

    public:

      // protected constructor used by dynamic creation
      UDSegmentsLayoutFrame();           

      // Operations
    public:

      // Overrides
      // ClassWizard generated virtual function overrides
      //{{AFX_VIRTUAL(UDSegmentsViewFrame)
      //}}AFX_VIRTUAL

      // Implementation
    public:

      // Generated message map functions
      //{{AFX_MSG(UDSegmentsLayoutFrame)
      // NOTE - the ClassWizard will add and remove member functions here.
      //}}AFX_MSG
    };

    /////////////////////////////////////////////////////////////////////////////


    class UDSegmentsLayout : public ColonViatronixWnd
    {
      // functions
    public:

      /// Constructor
      UDSegmentsLayout();

    protected:

      /// Keyboard event handler: key pressed.
      void OnKeyDown(UINT uChar, UINT uRepCnt, UINT uFlags);

      /// called when the view is being resized
      afx_msg void OnSize( UINT uType, int iSizeX, int iSizeY );

      /// Renders the views

      //{{AFX_MSG(UDSegmentsLayout)
      //}}AFX_MSG
      DECLARE_MESSAGE_MAP()

      /// returns the resource ID for indicated window orientation
      virtual uint4 GetLandscapeLayoutID()  { return IDT_UDSEGMENTS_MAUI; }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetWidescreenLayoutID() { return IDT_UDSEGMENTS_MAUI; }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetPortraitLayoutID()   { return IDT_UDSEGMENTS_MAUI; }
      /// returns the resource ID for indicated window orientation
      virtual uint4 GetPortraitLargeLayoutID() { return IDT_UDSEGMENTS_MAUI; }

    }; // class SegmentsCylindricalLayout


    // namespaces
  } // Layouts

} // ColonViews


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


// $Log: UDSegmentsColonWnd.h,v $
// Revision 1.2.2.2  2010/04/17 05:16:38  dongqing
// small portrait display layouts
//
// Revision 1.2.2.1  2009/12/09 20:25:22  dongqing
// layouts will specify various IDs for different orientations -- somewhat akin to
//  the IDD= enum used in mfc dialog classes --
// and the baseclass will decide which of the orientations is needed.
//
// Revision 1.2  2007/03/01 01:18:57  jmeade
// for code reviews: class naming, comments.
//
// Revision 1.1  2007/01/03 05:34:42  jmeade
// future ud segments layout.
//
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/UDSegmentsColonWnd.h,v 1.2.2.2 2010/04/17 05:16:38 dongqing Exp $
// $Id: UDSegmentsColonWnd.h,v 1.2.2.2 2010/04/17 05:16:38 dongqing Exp $
