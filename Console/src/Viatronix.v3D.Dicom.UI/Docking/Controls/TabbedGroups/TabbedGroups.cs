// $Id: TabbedGroups.cs,v 1.1 2005/10/11 14:41:43 mkontak Exp $
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
using System.Text;
using System.Data;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using Viatronix.v3D.Dicom.UI.Docking.Win32;
using Viatronix.v3D.Dicom.UI.Docking.Common;
using Viatronix.v3D.Dicom.UI.Docking.Controls;

namespace Viatronix.v3D.Dicom.UI.Docking.Controls
{
  [ToolboxBitmap(typeof(TabbedGroups))]
  public class TabbedGroups : UserControl, ISupportInitialize, IMessageFilter
  {
    public class DragProvider
    {
      protected object m_tag;
            
      public DragProvider()
      {
        m_tag = null;
      }
            
      public DragProvider(object tag)
      {
        m_tag = tag;
      }
            
      public object Tag
      {
        get { return m_tag; }
        set { m_tag = value; }
      }
    }
	
    public enum DisplayTabModes
    {
      HideAll,
      ShowAll,
      ShowActiveLeaf,
      ShowMouseOver,
      ShowActiveAndMouseOver
    }
	
    public enum CompactFlags
    {
      RemoveEmptyTabLeaf = 1,
      RemoveEmptyTabSequence = 2,
      ReduceSingleEntries = 4,
      ReduceSameDirection = 8,
      All = 15
    }
	
    // Instance fields
    protected int m_numLeafs;
    protected int m_defMinWidth;
    protected int m_defMinHeight;
    protected string m_closeMenuText;
    protected string m_prominentMenuText;
    protected string m_rebalanceMenuText;
    protected string m_movePreviousMenuText;
    protected string m_moveNextMenuText;
    protected string m_newVerticalMenuText;
    protected string m_newHorizontalMenuText;
    protected ImageList m_imageList;
    protected bool m_dirty;
    protected bool m_autoCalculateDirty;
    protected bool m_saveControls;
    protected bool m_initializing;
    protected bool m_atLeastOneLeaf;
    protected bool m_autoCompact;
    protected bool m_compacting;
    protected bool m_resizeBarLock;
    protected int m_resizeBarVector;
    protected Color m_resizeBarColor;
    protected Shortcut m_closeShortcut;
    protected Shortcut m_prominentShortcut;
    protected Shortcut m_rebalanceShortcut;
    protected Shortcut m_movePreviousShortcut;
    protected Shortcut m_moveNextShortcut;
    protected Shortcut m_splitVerticalShortcut;
    protected Shortcut m_splitHorizontalShortcut;
    protected Shortcut m_nextTabShortcut;
    protected CompactFlags m_compactOptions;
    protected DisplayTabModes m_displayTabMode;
    protected TabGroupLeaf m_prominentLeaf;
    protected TabGroupLeaf m_activeLeaf;
    protected TabGroupSequence m_root;
    protected VisualStyle m_style;
	
    // Delegates for events
    public delegate void TabControlCreatedHandler(TabbedGroups tg, Controls.TabControl tc);
    public delegate void PageCloseRequestHandler(TabbedGroups tg, TGCloseRequestEventArgs e);
    public delegate void PageContextMenuHandler(TabbedGroups tg, TGContextMenuEventArgs e);
    public delegate void GlobalSavingHandler(TabbedGroups tg, XmlTextWriter xmlOut);
    public delegate void GlobalLoadingHandler(TabbedGroups tg, XmlTextReader xmlIn);
    public delegate void PageSavingHandler(TabbedGroups tg, TGPageSavingEventArgs e);
    public delegate void PageLoadingHandler(TabbedGroups tg, TGPageLoadingEventArgs e);
    public delegate void ExternalDropHandler(TabbedGroups tg, TabGroupLeaf tgl, Controls.TabControl tc, DragProvider dp);
	
    // Instance events
    public event TabControlCreatedHandler TabControlCreated;
    public event PageCloseRequestHandler PageCloseRequest;
    public event PageContextMenuHandler PageContextMenu;
    public event GlobalSavingHandler GlobalSaving;
    public event GlobalLoadingHandler GlobalLoading;
    public event PageSavingHandler PageSaving;
    public event PageLoadingHandler PageLoading;
    public event EventHandler ProminentLeafChanged;
    public event EventHandler ActiveLeafChanged;
    public event EventHandler DirtyChanged;
    public event ExternalDropHandler ExternalDrop;
	
    public TabbedGroups()
    {
      InternalConstruct(VisualStyle.IDE);
    }
            
    public TabbedGroups(VisualStyle style)
    {
      InternalConstruct(style);
    }
        
    protected void InternalConstruct(VisualStyle style)
    {
      // Prevent flicker with double buffering and all painting inside WM_PAINT
      SetStyle(ControlStyles.DoubleBuffer, true);
      SetStyle(ControlStyles.AllPaintingInWmPaint, true);
		
      // We want to act as a drop target
      this.AllowDrop = true;
		
      // Remember parameters
      m_style = style;
		    
      // Define initial state
      m_numLeafs = 0;
      m_compacting = false;
      m_initializing = false;
		    
      // Create the root sequence that always exists
      m_root = new TabGroupSequence(this);
		    
      // Define default settings
      ResetProminentLeaf();
      ResetResizeBarVector();
      ResetResizeBarColor();
      ResetResizeBarLock();
      ResetCompactOptions();
      ResetDefaultGroupMinimumWidth();
      ResetDefaultGroupMinimumHeight();
      ResetActiveLeaf();
      ResetAutoCompact();
      ResetAtLeastOneLeaf();
      ResetCloseMenuText();
      ResetProminentMenuText();
      ResetRebalanceMenuText();
      ResetMovePreviousMenuText();
      ResetMoveNextMenuText();
      ResetNewVerticalMenuText();
      ResetNewHorizontalMenuText();
      ResetCloseShortcut();
      ResetProminentShortcut();
      ResetRebalanceShortcut();
      ResetMovePreviousShortcut();
      ResetMoveNextShortcut();
      ResetSplitVerticalShortcut();
      ResetSplitHorizontalShortcut();
      ResetImageList();
      ResetDisplayTabMode();
      ResetSaveControls();
      ResetAutoCalculateDirty();
      ResetDirty();
            
      // Add ourself to the application filtering list 
      // (to snoop for shortcut combinations)
      Application.AddMessageFilter(this);
            
    }
        
    [Category("TabbedGroups")]
    [DefaultValue(typeof(VisualStyle), "IDE")]
    public VisualStyle Style
    {
      get { return m_style; }
            
      set
      {
        if (m_style != value)
        {   
          m_style = value;
                    
          // Propogate to all children
          Notify(TabGroupBase.NotifyCode.StyleChanged);
        }
      }
    }

    public void ResetStyle()
    {
      Style = VisualStyle.IDE;
    }
        
    [Browsable(false)]
    public TabGroupSequence RootSequence
    {
      get { return m_root; }
    }

    [Category("TabbedGroups")]
    [DefaultValue(-1)]
    public int ResizeBarVector
    {
      get { return m_resizeBarVector; }
            
      set
      {
        if (m_resizeBarVector != value)
        {
          m_resizeBarVector = value;
                    
          // Propogate to all children
          Notify(TabGroupBase.NotifyCode.ResizeBarVectorChanged);
        }
      }
    }
        
    public void ResetResizeBarVector()
    {
      ResizeBarVector = -1;
    }
        
    [Category("TabbedGroups")]
    public Color ResizeBarColor
    {
      get { return m_resizeBarColor; }
            
      set
      {
        if (!m_resizeBarColor.Equals(value))
        {
          m_resizeBarColor = value;
                    
          // Propogate to all children
          Notify(TabGroupBase.NotifyCode.ResizeBarColorChanged);
        }
      }
    }

    protected bool ShouldSerializeResizeBackColor()
    {
      return m_resizeBarColor != base.BackColor;
    }
        
    public void ResetResizeBarColor()
    {
      ResizeBarColor = base.BackColor;
    }
        
    [Category("TabbedGroups")]
    [DefaultValue(false)]
    public bool ResizeBarLock
    {
      get { return m_resizeBarLock; }
      set { m_resizeBarLock = value; }
    }
        
    public void ResetResizeBarLock()
    {
      ResizeBarLock = false;
    }
        
    [Category("TabbedGroups")]
    public TabGroupLeaf ProminentLeaf
    {
      get { return m_prominentLeaf; }
            
      set
      {
        if (m_prominentLeaf != value)
        {
          m_prominentLeaf = value;

          // Mark layout as dirty
          if (m_autoCalculateDirty)
            m_dirty = true;

          // Propogate to all children
          Notify(TabGroupBase.NotifyCode.ProminentChanged);
                    
          OnProminentLeafChanged(EventArgs.Empty);
        }
      }
    }
        
    public void ResetProminentLeaf()
    {
      ProminentLeaf = null;
    }
        
    [Category("TabbedGroups")]
    [DefaultValue(typeof(CompactFlags), "All")]
    public CompactFlags CompactOptions
    {
      get { return m_compactOptions; }
      set { m_compactOptions = value; }
    }

    public void ResetCompactOptions()
    {
      CompactOptions = CompactFlags.All;
    }
        
    [Category("TabbedGroups")]
    [DefaultValue(4)]
    public int DefaultGroupMinimumWidth 
    {
      get { return m_defMinWidth; }
            
      set
      {
        if (m_defMinWidth != value)
        {
          m_defMinWidth = value;
                    
          // Propogate to all children
          Notify(TabGroupBase.NotifyCode.MinimumSizeChanged);
        }
      }
    }        
        
    public void ResetDefaultGroupMinimumWidth()
    {
      DefaultGroupMinimumWidth = 4;
    }
        
    [Category("TabbedGroups")]
    [DefaultValue(4)]
    public int DefaultGroupMinimumHeight
    {
      get { return m_defMinHeight; }
            
      set
      {
        if (m_defMinHeight != value)
        {
          m_defMinHeight = value;
                    
          // Propogate to all children
          Notify(TabGroupBase.NotifyCode.MinimumSizeChanged);
        }
      }
    }        
        
    public void ResetDefaultGroupMinimumHeight()
    {
      DefaultGroupMinimumHeight = 4;
    }

    [Localizable(true)]
    [Category("Text String")]
    [DefaultValue("&Close")]
    public string CloseMenuText
    {
      get { return m_closeMenuText; }
      set { m_closeMenuText = value; }
    }
        
    public void ResetCloseMenuText()
    {
      CloseMenuText = "&Close";
    }

    [Localizable(true)]
    [Category("Text String")]
    [DefaultValue("Pro&minent")]
    public string ProminentMenuText
    {
      get { return m_prominentMenuText; }
      set { m_prominentMenuText = value; }
    }

    public void ResetProminentMenuText()
    {
      ProminentMenuText = "Pro&minent";
    }

    [Localizable(true)]
    [Category("Text String")]
    [DefaultValue("&Rebalance")]
    public string RebalanceMenuText
    {
      get { return m_rebalanceMenuText; }
      set { m_rebalanceMenuText = value; }
    }

    public void ResetRebalanceMenuText()
    {
      RebalanceMenuText = "&Rebalance";
    }

    [Localizable(true)]
    [Category("Text String")]
    [DefaultValue("Move to &Previous Tab Group")]
    public string MovePreviousMenuText
    {
      get { return m_movePreviousMenuText; }
      set { m_movePreviousMenuText = value; }
    }

    public void ResetMovePreviousMenuText()
    {
      MovePreviousMenuText = "Move to &Previous Tab Group";
    }

    [Localizable(true)]
    [Category("Text String")]
    [DefaultValue("Move to &Next Tab Group")]
    public string MoveNextMenuText
    {
      get { return m_moveNextMenuText; }
      set { m_moveNextMenuText = value; }
    }
        
    public void ResetMoveNextMenuText()
    {
      MoveNextMenuText = "Move to &Next Tab Group";
    }

    [Localizable(true)]
    [Category("Text String")]
    [DefaultValue("New &Vertical Tab Group")]
    public string NewVerticalMenuText
    {
      get { return m_newVerticalMenuText; }
      set { m_newVerticalMenuText = value; }
    }

    public void ResetNewVerticalMenuText()
    {
      NewVerticalMenuText = "New &Vertical Tab Group";
    }

    [Localizable(true)]
    [Category("Text String")]
    [DefaultValue("New &Horizontal Tab Group")]
    public string NewHorizontalMenuText
    {
      get { return m_newHorizontalMenuText; }
      set { m_newHorizontalMenuText = value; }
    }
        
    public void ResetNewHorizontalMenuText()
    {
      NewHorizontalMenuText = "New &Horizontal Tab Group";
    }

    [Category("Shortcuts")]
    public Shortcut CloseShortcut
    {
      get { return m_closeShortcut; }
      set { m_closeShortcut = value; }
    }

    protected bool ShouldSerializeCloseShortcut()
    {
      return !m_closeShortcut.Equals(Shortcut.CtrlShiftC);
    }

    public void ResetCloseShortcut()
    {
      CloseShortcut = Shortcut.CtrlShiftC;
    }
        
    [Category("Shortcuts")]
    public Shortcut ProminentShortcut
    {
      get { return m_prominentShortcut; }
      set { m_prominentShortcut = value; }
    }

    protected bool ShouldSerializeProminentShortcut()
    {
      return !m_prominentShortcut.Equals(Shortcut.CtrlShiftT);
    }

    public void ResetProminentShortcut()
    {
      ProminentShortcut = Shortcut.CtrlShiftT;  
    }
        
    [Category("Shortcuts")]
    public Shortcut RebalanceShortcut
    {
      get { return m_rebalanceShortcut; }
      set { m_rebalanceShortcut = value; }
    }

    protected bool ShouldSerializeRebalanceShortcut()
    {
      return !m_rebalanceShortcut.Equals(Shortcut.CtrlShiftR);
    }

    public void ResetRebalanceShortcut()
    {
      RebalanceShortcut = Shortcut.CtrlShiftR;
    }

    [Category("Shortcuts")]
    public Shortcut MovePreviousShortcut
    {
      get { return m_movePreviousShortcut; }
      set { m_movePreviousShortcut = value; }
    }

    protected bool ShouldSerializeMovePreviousShortcut()
    {
      return !m_movePreviousShortcut.Equals(Shortcut.CtrlShiftP);
    }

    public void ResetMovePreviousShortcut()
    {
      MovePreviousShortcut = Shortcut.CtrlShiftP;
    }
        
    [Category("Shortcuts")]
    public Shortcut MoveNextShortcut
    {
      get { return m_moveNextShortcut; }
      set { m_moveNextShortcut = value; }
    }

    protected bool ShouldSerializeMoveNextShortcut()
    {
      return !m_moveNextShortcut.Equals(Shortcut.CtrlShiftN);
    }

    public void ResetMoveNextShortcut()
    {
      MoveNextShortcut = Shortcut.CtrlShiftN;
    }
        
    [Category("Shortcuts")]
    public Shortcut SplitVerticalShortcut
    {
      get { return m_splitVerticalShortcut; }
      set { m_splitVerticalShortcut = value; }
    }

    protected bool ShouldSerializeSplitVerticalShortcut()
    {
      return !m_splitVerticalShortcut.Equals(Shortcut.CtrlShiftV);
    }

    public void ResetSplitVerticalShortcut()
    {
      SplitVerticalShortcut = Shortcut.CtrlShiftV;
    }
        
    [Category("Shortcuts")]
    public Shortcut SplitHorizontalShortcut
    {
      get { return m_splitHorizontalShortcut; }
      set { m_splitHorizontalShortcut = value; }
    }

    protected bool ShouldSerializeSplitHorizontalShortcut()
    {
      return !m_splitHorizontalShortcut.Equals(Shortcut.CtrlShiftH);
    }

    public void ResetSplitHorizontalShortcut()
    {
      SplitHorizontalShortcut = Shortcut.CtrlShiftH;
    }
        
    [Category("TabbedGroups")]
    public ImageList ImageList
    {
      get { return m_imageList; }
            
      set 
      { 
        if (m_imageList != value)
        {
          // Propogate to all children
          Notify(TabGroupBase.NotifyCode.ImageListChanging);

          m_imageList = value;
                    
          // Propogate to all children
          Notify(TabGroupBase.NotifyCode.ImageListChanged);
        }
      }
    }
        
    protected bool ShouldSerializeImageList()
    {
      return m_imageList != null;
    }
        
    public void ResetImageList()
    {
      ImageList = null;
    }
        
    [Category("TabbedGroups")]
    [DefaultValue(typeof(DisplayTabModes), "ShowAll")]
    public DisplayTabModes DisplayTabMode
    {
      get { return m_displayTabMode; }
            
      set
      {
        if (m_displayTabMode != value)
        {
          m_displayTabMode = value;
                    
          // Propogate to all children
          Notify(TabGroupBase.NotifyCode.DisplayTabMode);
        }
      }
    }
        
    public void ResetDisplayTabMode()
    {
      DisplayTabMode = DisplayTabModes.ShowAll;
    }

    [Category("TabbedGroups")]
    [DefaultValue(true)]
    public bool SaveControls
    {
      get { return m_saveControls; }
      set { m_saveControls = value; }
    }
        
    public void ResetSaveControls()
    {
      SaveControls = true;
    }

    [Category("TabbedGroups")]
    public bool Dirty
    {
      get { return m_dirty; }
            
      set 
      {
        if (m_dirty != value)
        {
          m_dirty = value;
                    
          OnDirtyChanged(EventArgs.Empty);
        }
      }
    }
        
    protected bool ShouldSerializeDirty()
    {
      return false;
    }
        
    public void ResetDirty()
    {
      Dirty = false;
    }

    [Category("TabbedGroups")]
    [DefaultValue(true)]
    public bool AutoCalculateDirty
    {
      get { return m_autoCalculateDirty; }
      set { m_autoCalculateDirty = value; }
    }
        
    public void ResetAutoCalculateDirty()
    {
      AutoCalculateDirty = true;
    }

    [Category("TabbedGroups")]
    public TabGroupLeaf ActiveLeaf
    {
      get { return m_activeLeaf; }
            
      set
      {
        if (m_activeLeaf != value)
        {
          // Mark layout as dirty
          if (m_autoCalculateDirty)
            m_dirty = true;

          // Remove selection highlight from old leaf
          if (m_activeLeaf != null)
          {
            // Get access to the contained tab control
            TabControl tc = m_activeLeaf.GroupControl as Controls.TabControl;
                        
            // Remove bold text for the selected page
            tc.BoldSelectedPage = false;
                        
            m_activeLeaf = null;
          }

          // Set selection highlight on new active leaf            
          if (value != null)
          {
            // Get access to the contained tab control
            TabControl tc = value.GroupControl as Controls.TabControl;
                        
            // Remove bold text for the selected page
            tc.BoldSelectedPage = true;
                        
            m_activeLeaf = value;
          }

          // Is the tab mode dependant on the active leaf value
          if ((m_displayTabMode == DisplayTabModes.ShowActiveLeaf) ||
            (m_displayTabMode == DisplayTabModes.ShowActiveAndMouseOver))
          {
            // Yes, better notify a change in value so it can be applied
            Notify(TabGroupBase.NotifyCode.DisplayTabMode);
          }
                        
          OnActiveLeafChanged(EventArgs.Empty);
        }
      }
    }
        
    public void ResetActiveLeaf()
    {
      ActiveLeaf = null;
    }

    [Category("TabbedGroups")]
    public bool AtLeastOneLeaf
    {
      get { return m_atLeastOneLeaf; }
            
      set
      {
        if (m_atLeastOneLeaf != value)
        {
          m_atLeastOneLeaf = value;
                    
          // Do always need at least one leaf?
          if (m_atLeastOneLeaf)
          {
            // Is there at least one?
            if (m_numLeafs == 0)
            {
              // No, create a default entry for the root sequence
              m_root.AddNewLeaf();

              // Mark layout as dirty
              if (m_autoCalculateDirty)
                m_dirty = true;
            }
          }
          else
          {
            // Are there some potential leaves not needed
            if (m_numLeafs > 0)
            {
              // Use compaction so only needed ones are retained
              if (m_autoCompact)
                Compact();
            }
          }
        }
      }
    }
        
    public void ResetAtLeastOneLeaf()
    {
      AtLeastOneLeaf = true;
    }

    [Category("TabbedGroups")]
    [DefaultValue(true)]
    public bool AutoCompact
    {
      get { return m_autoCompact; }
      set { m_autoCompact = value; }
    }

    public void ResetAutoCompact()
    {
      m_autoCompact = true;
    }

    public void Rebalance()
    {
      m_root.Rebalance(true);
    }

    public void Rebalance(bool recurse)
    {
      m_root.Rebalance(recurse);
    }
        
    public void Compact()
    {
      Compact(m_compactOptions);
    }
        
    public void Compact(CompactFlags flags)
    {
      // When  entries are removed because of compacting this may cause the container object 
      // to start a compacting request. Prevent this recursion by using a simple varible.
      if (!m_compacting)
      {
        // We never compact when loading/initializing the contents
        if (!m_initializing)
        {
          m_compacting = true;
          m_root.Compact(flags);
          m_compacting = false;
                    
          EnforceAtLeastOneLeaf();
        }
      }
    }
        
    public TabGroupLeaf FirstLeaf()
    {
      return RecursiveFindLeafInSequence(m_root, true);
    }

    public TabGroupLeaf LastLeaf()
    {
      return RecursiveFindLeafInSequence(m_root, false);
    }

    public TabGroupLeaf NextLeaf(TabGroupLeaf current)
    {
      // Get parent of the provided leaf
      TabGroupSequence tgs = current.Parent as TabGroupSequence;
            
      // Must have a valid parent sequence
      if (tgs != null)
        return RecursiveFindLeafInSequence(tgs, current, true);
      else
        return null;
    }
        
    public TabGroupLeaf PreviousLeaf(TabGroupLeaf current)
    {
      // Get parent of the provided leaf
      TabGroupSequence tgs = current.Parent as TabGroupSequence;
            
      // Must have a valid parent sequence
      if (tgs != null)
        return RecursiveFindLeafInSequence(tgs, current, false);
      else
        return null;
    }

    internal void MoveActiveToNearestFromLeaf(TabGroupBase oldLeaf)
    {
      // Must have a reference to begin movement
      if (oldLeaf != null)
      {
        // Find the parent sequence of leaf, remember that a 
        // leaf must be contained within a sequence instance
        TabGroupSequence tgs = oldLeaf.Parent as TabGroupSequence;
                
        // Must be valid, but had better check anyway
        if (tgs != null)
        {
          // Move relative to given base in the sequence
          MoveActiveInSequence(tgs, oldLeaf);
        }
      }
    }
        
    internal void MoveActiveToNearestFromSequence(TabGroupSequence tgs)
    {
      // Is active leaf being moved from root sequence
      if (m_root == tgs)
      {
        // Then make nothing active
        ActiveLeaf = null;
      }
      else
      {
        // Find the parent sequence of given sequence
        TabGroupSequence tgsParent = tgs.Parent as TabGroupSequence;
            
        // Must be valid, but had better check anyway
        if (tgs != null)
        {
          // Move relative to given base in the sequence
          MoveActiveInSequence(tgsParent, tgs);
        }
      }
    }
        
    public virtual void OnTabControlCreated(Controls.TabControl tc)
    {
      // Remember how many leafs there are
      m_numLeafs++;
        
      // Define default values
      tc.Appearance = Viatronix.v3D.Dicom.UI.Docking.Controls.TabControl.VisualAppearance.MultiDocument;
      tc.BoldSelectedPage = false;
      tc.IDEPixelBorder = true;
      tc.ImageList = m_imageList;
      tc.Style = m_style;

      // Apply the current display tab mode setting            
      switch(m_displayTabMode)
      {
        case TabbedGroups.DisplayTabModes.ShowAll:
          tc.HideTabsMode = Viatronix.v3D.Dicom.UI.Docking.Controls.TabControl.HideTabsModes.ShowAlways;
          break;
        case TabbedGroups.DisplayTabModes.HideAll:
          tc.HideTabsMode = Viatronix.v3D.Dicom.UI.Docking.Controls.TabControl.HideTabsModes.HideAlways;
          break;
      }
            
      // Has anyone registered for the event?
      if (TabControlCreated != null)
        TabControlCreated(this, tc);
    }
        
    public virtual void OnPageCloseRequested(TGCloseRequestEventArgs e)
    {
      // Has anyone registered for the event?
      if (PageCloseRequest != null)
        PageCloseRequest(this, e);
                
    }

    public virtual void OnPageContextMenu(TGContextMenuEventArgs e)
    {
      // Has anyone registered for the event?
      if (PageContextMenu != null)
        PageContextMenu(this, e);
    }

    public virtual void OnGlobalSaving(XmlTextWriter xmlOut)
    {
      // Has anyone registered for the event?
      if (GlobalSaving != null)
        GlobalSaving(this, xmlOut);
    }
        
    public virtual void OnGlobalLoading(XmlTextReader xmlIn)
    {
      // Has anyone registered for the event?
      if (GlobalLoading != null)
        GlobalLoading(this, xmlIn);
    }
        
    public virtual void OnPageSaving(TGPageSavingEventArgs e)
    {
      // Has anyone registered for the event?
      if (PageSaving != null)
        PageSaving(this, e);
    }
        
    public virtual void OnPageLoading(TGPageLoadingEventArgs e)
    {
      // Has anyone registered for the event?
      if (PageLoading != null)
        PageLoading(this, e);
    }

    public virtual void OnProminentLeafChanged(EventArgs e)
    {
      // Has anyone registered for the event?
      if (ProminentLeafChanged != null)
        ProminentLeafChanged(this, e);
    }
        
    public virtual void OnActiveLeafChanged(EventArgs e)
    {
      // Has anyone registered for the event?
      if (ActiveLeafChanged != null)
        ActiveLeafChanged(this, e);
    }
        
    public virtual void OnDirtyChanged(EventArgs e)
    {
      // Has anyone registered for the event?
      if (DirtyChanged != null)
        DirtyChanged(this, e);
    }

    public virtual void OnExternalDrop(TabGroupLeaf tgl, Controls.TabControl tc, DragProvider dp)
    {
      // Has anyone registered for the event?
      if (ExternalDrop != null)
        ExternalDrop(this, tgl, tc, dp);
    }

    public void BeginInit()
    {
      m_initializing = true;
    }
        
    public void EndInit()
    {
      m_initializing = false;
            
      // Inform the root sequence to reposition itself
      m_root.Reposition();
    }
        
    public bool Initializing
    {
      get { return m_initializing; }
    }

    public byte[] SaveConfigToArray()
    {
      return SaveConfigToArray(Encoding.Unicode);	
    }

    public byte[] SaveConfigToArray(Encoding encoding)
    {
      // Create a memory based stream
      MemoryStream ms = new MemoryStream();
			
      // Save into the file stream
      SaveConfigToStream(ms, encoding);

      // Must remember to close
      ms.Close();

      // Return an array of bytes that contain the streamed XML
      return ms.GetBuffer();
    }

    public void SaveConfigToFile(string filename)
    {
      SaveConfigToFile(filename, Encoding.Unicode);
    }

    public void SaveConfigToFile(string filename, Encoding encoding)
    {
      // Create/Overwrite existing file
      FileStream fs = new FileStream(filename, FileMode.Create);
			
      // Save into the file stream
      SaveConfigToStream(fs, encoding);		

      // Must remember to close
      fs.Close();
    }

    public void SaveConfigToStream(Stream stream, Encoding encoding)
    {
      XmlTextWriter xmlOut = new XmlTextWriter(stream, encoding); 

      // Use indenting for readability
      xmlOut.Formatting = Formatting.Indented;
			
      // Always begin file with identification and warning
      xmlOut.WriteStartDocument();
      xmlOut.WriteComment(" Magic, The User Interface library for .NET (www.dotnetmagic.com) ");
      xmlOut.WriteComment(" Modifying this generated file will probably render it invalid ");

      // Associate a version number with the root element so that future version of the code
      // will be able to be backwards compatible or at least recognise out of date versions
      xmlOut.WriteStartElement("TabbedGroups");
      xmlOut.WriteAttributeString("FormatVersion", "1");
            
      if (m_activeLeaf != null)
        xmlOut.WriteAttributeString("ActiveLeaf", m_activeLeaf.Unique.ToString());
      else
        xmlOut.WriteAttributeString("ActiveLeaf", "-1");

      // Give handlers chance to embed custom data
      xmlOut.WriteStartElement("CustomGlobalData");
      OnGlobalSaving(xmlOut);
      xmlOut.WriteEndElement();

      // Save the root sequence
      m_root.SaveToXml(xmlOut);

      // Terminate the root element and document        
      xmlOut.WriteEndElement();
      xmlOut.WriteEndDocument();

      // This should flush all actions and close the file
      xmlOut.Close();			
            
      // Saved, so cannot be dirty any more
      if (m_autoCalculateDirty)
        m_dirty = false;
            
    }

    public void LoadConfigFromArray(byte[] buffer)
    {
      // Create a memory based stream
      MemoryStream ms = new MemoryStream(buffer);
			
      // Save into the file stream
      LoadConfigFromStream(ms);

      // Must remember to close
      ms.Close();
    }

    public void LoadConfigFromFile(string filename)
    {
      // Open existing file
      FileStream fs = new FileStream(filename, FileMode.Open);
			
      // Load from the file stream
      LoadConfigFromStream(fs);		

      // Must remember to close
      fs.Close();
    }

    public void LoadConfigFromStream(Stream stream)
    {
      XmlTextReader xmlIn = new XmlTextReader(stream); 

      // Ignore whitespace, not interested
      xmlIn.WhitespaceHandling = WhitespaceHandling.None;

      // Moves the reader to the root element.
      xmlIn.MoveToContent();

      // Double check this has the correct element name
      if (xmlIn.Name != "TabbedGroups")
        throw new ArgumentException("Root element must be 'TabbedGroups'");

      // Load the format version number
      string version = xmlIn.GetAttribute(0);
      string rawActiveLeaf = xmlIn.GetAttribute(1);

      // Convert format version from string to double
      int formatVersion = (int)System.Convert.ToDouble(version);
      int activeLeaf = System.Convert.ToInt32(rawActiveLeaf);
            
      // We can only load 1 upward version formats
      if (formatVersion < 1)
        throw new ArgumentException("Can only load Version 1 and upwards TabbedGroups Configuration files");

      try
      {
        // Prevent compacting and reposition of children
        BeginInit();
                
        // Remove all existing contents
        m_root.Clear();
                
        // Read to custom data element
        if (!xmlIn.Read())
          throw new ArgumentException("An element was expected but could not be read in");

        if (xmlIn.Name != "CustomGlobalData")
          throw new ArgumentException("Expected 'CustomData' element was not found");

        bool finished = xmlIn.IsEmptyElement;

        // Give handlers chance to reload custom saved data
        OnGlobalLoading(xmlIn);

        // Read everything until we get the end of custom data marker
        while(!finished)
        {
          // Check it has the expected name
          if (xmlIn.NodeType == XmlNodeType.EndElement)
            finished = (xmlIn.Name == "CustomGlobalData");

          if (!finished)
          {
            if (!xmlIn.Read())
              throw new ArgumentException("An element was expected but could not be read in");
          }
        } 

        // Read the next well known lement
        if (!xmlIn.Read())
          throw new ArgumentException("An element was expected but could not be read in");

        // Is it the expected element?
        if (xmlIn.Name != "Sequence")
          throw new ArgumentException("Element 'Sequence' was expected but not found");
                
        // Reload the root sequence
        m_root.LoadFromXml(xmlIn);

        // Move past the end element
        if (!xmlIn.Read())
          throw new ArgumentException("Could not read in next expected node");

        // Check it has the expected name
        if (xmlIn.NodeType != XmlNodeType.EndElement)
          throw new ArgumentException("EndElement expected but not found");
      }
      finally
      {
        TabGroupLeaf newActive = null;
            
        // Reset the active leaf correctly
        TabGroupLeaf current = FirstLeaf();
                
        while(current != null)
        {
          // Default to the first leaf if we cannot find a match
          if (newActive == null)
            newActive = current;
                        
          // Find an exact match?
          if (current.Unique == activeLeaf)
          {
            newActive = current;
            break;
          }
                    
          current = NextLeaf(current);
        }
                
        // Reinstate the active leaf indication
        if (newActive != null)
          ActiveLeaf = newActive;
            
        // Allow normal operation
        EndInit();
      }
                        
      xmlIn.Close();			
            
      // Just loaded, so cannot be dirty
      if (m_autoCalculateDirty)
        m_dirty = false;
    }
        
    protected TabGroupLeaf RecursiveFindLeafInSequence(TabGroupSequence tgs, bool forwards)
    {
      int count = tgs.Count;
        
      for(int i=0; i<count; i++)
      {
        // Index depends on which direction we are processing
        int index = (forwards == true) ? i : (tgs.Count - i - 1);
                
        // Is this the needed leaf node?
        if (tgs[index].IsLeaf)
          return tgs[index] as TabGroupLeaf;
        else
        {
          // Need to make a recursive check inside group
          TabGroupLeaf leaf = RecursiveFindLeafInSequence(tgs[index] as TabGroupSequence, forwards);

          if (leaf != null)
            return leaf;
        }
      }
            
      // Still no luck
      return null;
    }

    protected TabGroupLeaf RecursiveFindLeafInSequence(TabGroupSequence tgs, TabGroupBase tgb, bool forwards)
    {
      int count = tgs.Count;
      int index = tgs.IndexOf(tgb);
        
      // Are we look for entries after the provided one?
      if (forwards)
      {
        for(int i=index+1; i<count; i++)
        {
          // Is this the needed leaf node?
          if (tgs[i].IsLeaf)
            return tgs[i] as TabGroupLeaf;
          else
          {
            TabGroupLeaf leaf = RecursiveFindLeafInSequence(tgs[i] as TabGroupSequence, forwards);
                    
            if (leaf != null)
              return leaf;
          }
        }
      }
      else
      {
        // Now try each entry before that given
        for(int i=index-1; i>=0; i--)
        {
          // Is this the needed leaf node?
          if (tgs[i].IsLeaf)
            return tgs[i] as TabGroupLeaf;
          else
          {
            TabGroupLeaf leaf = RecursiveFindLeafInSequence(tgs[i] as TabGroupSequence, forwards);
                    
            if (leaf != null)
              return leaf;
          }
        }
      }
                        
      // Still no luck, try our own parent
      if (tgs.Parent != null)
        return RecursiveFindLeafInSequence(tgs.Parent as TabGroupSequence, tgs, forwards);
      else
        return null;
    }

    protected void MoveActiveInSequence(TabGroupSequence tgs, TabGroupBase child)
    {
      int count = tgs.Count;
      int index = tgs.IndexOf(child);
        
      // First try each entry after that given
      for(int i=index+1; i<count; i++)
      {
        // Is this the needed leaf node?
        if (tgs[i].IsLeaf)
        {
          // Make it active, and finish
          ActiveLeaf = tgs[i] as TabGroupLeaf;
          return;  
        }
        else
        {
          // Need to make a recursive check inside group
          if (RecursiveActiveInSequence(tgs[i] as TabGroupSequence, true))
            return;
        }
      }
            
      // Now try each entry before that given
      for(int i=index-1; i>=0; i--)
      {
        // Is this the needed leaf node?
        if (tgs[i].IsLeaf)
        {
          // Make it active, and finish
          ActiveLeaf = tgs[i] as TabGroupLeaf;
          return;  
        }
        else
        {
          // Need to make a recursive check inside group
          if (RecursiveActiveInSequence(tgs[i] as TabGroupSequence, false))
            return;
        }
      }
            
      // Still no luck, try our own parent
      if (tgs.Parent != null)
        MoveActiveInSequence(tgs.Parent as TabGroupSequence, tgs);
    }
        
    protected bool RecursiveActiveInSequence(TabGroupSequence tgs, bool forwards)
    {
      int count = tgs.Count;
        
      for(int i=0; i<count; i++)
      {
        // Index depends on which direction we are processing
        int index = (forwards == true) ? i : (tgs.Count - i - 1);
                
        // Is this the needed leaf node?
        if (tgs[index].IsLeaf)
        {
          // Make it active, and finish
          ActiveLeaf = tgs[index] as TabGroupLeaf;
          return true;
        }
        else
        {
          // Need to make a recursive check inside group
          if (RecursiveActiveInSequence(tgs[index] as TabGroupSequence, forwards))
            return true;
        }
      }
            
      // Still no luck
      return false;
    }
        
    protected void Notify(TabGroupBase.NotifyCode notifyCode)
    {
      // Propogate change notification only is we have a root sequence
      if (m_root != null)
        m_root.Notify(notifyCode);
    }
        
    internal void EnforceAtLeastOneLeaf()
    {
      // Should not add items during compacting operation
      if (!m_compacting)
      {
        // Ensure we enfore policy of at least one leaf
        if (m_atLeastOneLeaf)
        {
          // Is there at least one?
          if (m_numLeafs == 0)
          {
            // No, create a default entry for the root sequence
            m_root.AddNewLeaf();
                        
            // Update the active leaf
            m_activeLeaf = FirstLeaf();

            // Mark layout as dirty
            if (m_autoCalculateDirty)
              m_dirty = true;
          }
        }
      }
    }
        
    internal void GroupRemoved(TabGroupBase tgb)
    {
      // Decrease count of leafs entries for each leaf that exists
      // which in the hierarchy that is being removed
            
      if (tgb.IsLeaf)
        m_numLeafs--;
      else
      {
        TabGroupSequence tgs = tgb as TabGroupSequence;
            
        // Recurse into processing each child item
        for(int i=0; i<tgs.Count; i++)
          GroupRemoved(tgs[i]);
      }
            
      // Mark layout as dirty
      if (m_autoCalculateDirty)
        m_dirty = true;
    }
         
    public bool PreFilterMessage(ref Message msg)
    {
      Form parentForm = this.FindForm();

      // Only interested if the Form we are on is activate (i.e. contains focus)
      if ((parentForm != null) && (parentForm == Form.ActiveForm) && parentForm.ContainsFocus)
      {		
        switch(msg.Msg)
        {
          case (int)Win32.Msgs.WM_KEYDOWN:
            // Ignore keyboard input if the control is disabled
            if (this.Enabled)
            {
              // Find up/down state of shift and control keys
              ushort shiftKey = User32.GetKeyState((int)Win32.VirtualKeys.VK_SHIFT);
              ushort controlKey = User32.GetKeyState((int)Win32.VirtualKeys.VK_CONTROL);

              // Basic code we are looking for is the key pressed
              int code = (int)msg.WParam;

              // Is SHIFT pressed?
              bool shiftPressed = (((int)shiftKey & 0x00008000) != 0);

              // Is CONTROL pressed?
              bool controlPressed = (((int)controlKey & 0x00008000) != 0);

              // Was the TAB key pressed?
              if ((code == (int)Win32.VirtualKeys.VK_TAB) && controlPressed)
              {
                if (shiftPressed)
                  return SelectPreviousTab();
                else
                  return SelectNextTab();
              }
              else
              {
                // Plus the modifier for SHIFT...
                if (shiftPressed)
                  code += 0x00010000;

                // Plus the modifier for CONTROL
                if (controlPressed)
                  code += 0x00020000;

                // Construct shortcut from keystate and keychar
                Shortcut sc = (Shortcut)(code);

                // Search for a matching command
                return TestShortcut(sc);
              }
            }
            break;
          case (int)Win32.Msgs.WM_SYSKEYDOWN:
            // Ignore keyboard input if the control is disabled
            if (this.Enabled)
            {
              if ((int)msg.WParam != (int)Win32.VirtualKeys.VK_MENU)
              {
                // Construct shortcut from ALT + keychar
                Shortcut sc = (Shortcut)(0x00040000 + (int)msg.WParam);
		
                // Search for a matching command
                return TestShortcut(sc);
              }
            }
            break;
          default:
            break;
        }
      }

      return false;
    }
        
    protected bool TestShortcut(Shortcut sc)
    {
      bool result = false;
        
      // Must have an active leaf for shortcuts to operate against
      if (m_activeLeaf != null)
      {
        Controls.TabControl tc = m_activeLeaf.GroupControl as Controls.TabControl;
            
        // Must have an active tab for these shortcuts to work against
        if (tc.SelectedTab != null)
        {
          // Close selected page requested?
          if (sc.Equals(m_closeShortcut))
          {
            m_activeLeaf.OnClose(m_activeLeaf, EventArgs.Empty);
            result = true;
          }

          // Toggle the prominence state?
          if (sc.Equals(m_prominentShortcut))
          {
            m_activeLeaf.OnToggleProminent(m_activeLeaf, EventArgs.Empty);
            result = true;
          }
                        
          // Move page to the next group?
          if (sc.Equals(m_moveNextShortcut))
          {
            m_activeLeaf.OnMoveNext(m_activeLeaf, EventArgs.Empty);
            result = true;
          }
                
          // Move page to the previous group?
          if (sc.Equals(m_movePreviousShortcut))
          {
            m_activeLeaf.OnMovePrevious(m_activeLeaf, EventArgs.Empty);
            result = true;
          }
                
          // Cannot split a group unless at least two entries exist                
          if (tc.TabPages.Count > 1)
          {
            bool allowVert = false;
            bool allowHorz = false;
                        
            if (m_root.Count <= 1)
            {
              allowVert = true;
              allowHorz = true;
            }
            else
            {
              if (m_root.Direction == Direction.Vertical)
                allowVert = true;
              else
                allowHorz = true;
            }
                    
            // Create two vertical groups
            if (allowHorz && sc.Equals(m_splitVerticalShortcut))
            {
              m_activeLeaf.NewHorizontalGroup(m_activeLeaf, false);
              result = true;
            }

            // Create two horizontal groups
            if (allowVert && sc.Equals(m_splitHorizontalShortcut))
            {
              m_activeLeaf.NewVerticalGroup(m_activeLeaf, false);
              result = true;
            }
          }
        }
                
        // Request to rebalance all spacing
        if (sc.Equals(m_rebalanceShortcut))
        {
          m_activeLeaf.OnRebalance(m_activeLeaf, EventArgs.Empty);
          result = true;
        }
      }

      return result;
    }
        
    protected bool SelectNextTab()
    {
      // If no active leaf...
      if (m_activeLeaf == null)
        SelectFirstPage();
      else
      {
        bool selectFirst = false;
        TabGroupLeaf startLeaf = m_activeLeaf;
        TabGroupLeaf thisLeaf = startLeaf;
                
        do
        {
          // Access to the embedded tab control
          Controls.TabControl tc = thisLeaf.GroupControl as Controls.TabControl;
                
          // Does it have any pages?
          if (tc.TabPages.Count > 0)
          {
            // Are we allowed to select the first page?
            if (selectFirst)
            {
              // Do it and exit loop
              tc.SelectedIndex = 0;
                            
              // Must ensure this becomes the active leaf
              if (thisLeaf != m_activeLeaf)
                ActiveLeaf = thisLeaf;
                                
              break;
            }
            else
            {
              // Is there another page after the selected one?
              if (tc.SelectedIndex < tc.TabPages.Count - 1)
              {
                // Select new page and exit loop
                tc.SelectedIndex = tc.SelectedIndex + 1;
                break;
              }         
            }           
          }
                    
          selectFirst = true;
                    
          // Find the next leaf in sequence
          thisLeaf = NextLeaf(thisLeaf);
                    
          // No more leafs, wrap back to first
          if (thisLeaf == null)
            thisLeaf = FirstLeaf();

          // Back at starting leaf?
          if (thisLeaf == startLeaf)
          {
            // If it was not the first page that we started from
            if (tc.SelectedIndex > 0)
            {
              // Then we have circles all the way around, select first page
              tc.SelectedIndex = 0;
            }
          }

        } while(thisLeaf != startLeaf);
      }
            
      return true;
    }

    protected bool SelectPreviousTab()
    {
      // If no active leaf...
      if (m_activeLeaf == null)
        SelectLastPage();
      else
      {
        bool selectLast = false;
        TabGroupLeaf startLeaf = m_activeLeaf;
        TabGroupLeaf thisLeaf = startLeaf;
                
        do
        {
          // Access to the embedded tab control
          Controls.TabControl tc = thisLeaf.GroupControl as Controls.TabControl;
                
          // Does it have any pages?
          if (tc.TabPages.Count > 0)
          {
            // Are we allowed to select the last page?
            if (selectLast)
            {
              // Do it and exit loop
              tc.SelectedIndex = tc.TabPages.Count - 1;
                            
              // Must ensure this becomes the active leaf
              if (thisLeaf != m_activeLeaf)
                ActiveLeaf = thisLeaf;
                                
              break;
            }
            else
            {
              // Is there another page before the selected one?
              if (tc.SelectedIndex > 0)
              {
                // Select previous page and exit loop
                tc.SelectedIndex = tc.SelectedIndex - 1;
                break;
              }         
            }           
          }
                    
          selectLast = true;
                    
          // Find the previous leaf in sequence
          thisLeaf = PreviousLeaf(thisLeaf);
                    
          // No more leafs, wrap back to first
          if (thisLeaf == null)
            thisLeaf = LastLeaf();

          // Back at starting leaf?
          if (thisLeaf == startLeaf)
          {
            // If it was not the first page that we started from
            if (tc.SelectedIndex == 0)
            {
              // Then we have circles all the way around, select last page
              tc.SelectedIndex = tc.TabPages.Count - 1;
            }
          }

        } while(thisLeaf != startLeaf);
      }
            
      return true;
    }

    protected void SelectFirstPage()
    {
      // Find the first leaf
      ActiveLeaf = FirstLeaf();
                    
      // Did we find a leaf?
      if (m_activeLeaf != null)
      {
        // Is there a page that can be selected?
        if (m_activeLeaf.TabPages.Count > 0)
          m_activeLeaf.TabPages[0].Selected = true;
      }
    }
        
    protected void SelectLastPage()
    {
      // Find the first leaf
      ActiveLeaf = LastLeaf();
                    
      // Did we find a leaf?
      if (m_activeLeaf != null)
      {
        // Is there a page that can be selected?
        if (m_activeLeaf.TabPages.Count > 0)
          m_activeLeaf.TabPages[m_activeLeaf.TabPages.Count - 1].Selected = true;
      }
    }
  }
}


#region revision history

// $Log: TabbedGroups.cs,v $
// Revision 1.1  2005/10/11 14:41:43  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/Docking/Controls/TabbedGroups/TabbedGroups.cs,v 1.1 2005/10/11 14:41:43 mkontak Exp $
// $Id: TabbedGroups.cs,v 1.1 2005/10/11 14:41:43 mkontak Exp $

#endregion

