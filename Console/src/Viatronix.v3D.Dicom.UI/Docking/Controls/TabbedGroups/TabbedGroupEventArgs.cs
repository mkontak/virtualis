// $Id: TabbedGroupEventArgs.cs,v 1.1 2005/10/11 14:41:43 mkontak Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]

using System;
using System.IO;
using System.Xml;
using System.Windows.Forms;
using Viatronix.v3D.Dicom.UI.Docking.Menus;
using Viatronix.v3D.Dicom.UI.Docking.Controls;

namespace Viatronix.v3D.Dicom.UI.Docking.Controls
{
  public class TGCloseRequestEventArgs
  {
    protected TabGroupLeaf m_tgl;
    protected Controls.TabControl m_tc;
    protected Controls.TabPage m_tp;
    protected bool m_cancel;
	
    public TGCloseRequestEventArgs(TabGroupLeaf tgl, Controls.TabControl tc, Controls.TabPage tp)
    {
      // Definie initial state
      m_tgl = tgl;
      m_tc = tc;
      m_tp = tp;
      m_cancel = false;
    }
		
    public TabGroupLeaf Leaf
    {
      get { return m_tgl; }
    }
    
    public Controls.TabControl TabControl
    {
      get { return m_tc; }
    }

    public Controls.TabPage TabPage
    {
      get { return m_tp; }
    }
        
    public bool Cancel
    {
      get { return m_cancel; }
      set { m_cancel = value; }
    }
  }

  public class TGContextMenuEventArgs : TGCloseRequestEventArgs
  {
    protected PopupMenu m_contextMenu;
	
    public TGContextMenuEventArgs(TabGroupLeaf tgl, Controls.TabControl tc, 
      Controls.TabPage tp, PopupMenu contextMenu)
      : base(tgl, tc, tp)
    {
      // Definie initial state
      m_contextMenu = contextMenu;
    }
		
    public PopupMenu ContextMenu
    {
      get { return m_contextMenu; }
    }    
  }
    
  public class TGPageLoadingEventArgs
  {
    protected Controls.TabPage m_tp;
    protected XmlTextReader m_xmlIn;
    protected bool m_cancel;
        
    public TGPageLoadingEventArgs(Controls.TabPage tp, XmlTextReader xmlIn)
    {
      // Definie initial state
      m_tp = tp;
      m_xmlIn = xmlIn;
      m_cancel = false;
    }
        
    public Controls.TabPage TabPage
    {
      get { return m_tp; }
    }
        
    public XmlTextReader XmlIn
    {
      get { return m_xmlIn; }
    }
        
    public bool Cancel
    {
      get { return m_cancel; }
      set { m_cancel = value; }
    }
  }    

  public class TGPageSavingEventArgs
  {
    protected Controls.TabPage m_tp;
    protected XmlTextWriter m_xmlOut;
        
    public TGPageSavingEventArgs(Controls.TabPage tp, XmlTextWriter xmlOut)
    {
      // Definie initial state
      m_tp = tp;
      m_xmlOut = xmlOut;
    }
        
    public Controls.TabPage TabPage
    {
      get { return m_tp; }
    }
        
    public XmlTextWriter XmlOut
    {
      get { return m_xmlOut; }
    }
  }    
}


#region revision history

// $Log: TabbedGroupEventArgs.cs,v $
// Revision 1.1  2005/10/11 14:41:43  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Controls/TabbedGroups/TabbedGroupEventArgs.cs,v 1.1 2005/10/11 14:41:43 mkontak Exp $
// $Id: TabbedGroupEventArgs.cs,v 1.1 2005/10/11 14:41:43 mkontak Exp $

#endregion

