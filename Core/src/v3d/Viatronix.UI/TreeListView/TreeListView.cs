// $Id: TreeListView.cs,v 1.3 2006/06/16 20:47:49 mkontak Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]


using System;
using System.Threading;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.Diagnostics;

namespace Viatronix.UI
{

  #region TreeListView

	/// <summary>
	/// TreeListView control (simulates a TreeView in a ListView)
	/// </summary>
	public class TreeListView : System.Windows.Forms.ListView
	{



    #region delegates

    /// <summary>
    /// Int Handler Delegate
    /// </summary>
    private delegate int IntHandler();

    /// <summary>
    /// IItem arrsy handlet delegate used to d
    /// </summary>
    private delegate TreeListViewItem[] ItemArrayHandler();

    #endregion

    
    #region fields

    /// <summary>
    /// Color of the lines if ShowPlusMinus property is enabled
    /// </summary>
    private Color m_plusMinusLineColor = Color.DarkGray;

    /// <summary>
    /// Flag used to determine whether the control is draw with XP-Style highlight color
    /// </summary>
    private bool m_useXPHighLightStyle = true;

    /// <summary>
    /// Path seperator
    /// </summary>
    private string m_pathSeparator = "\\";

    /// <summary>
    /// Show Plus Minus flag
    /// </summary>
    private bool m_showPlusMinus = true;

    /// <summary>
    /// Flag to indicate whether the item is currently edited
    /// </summary>
    private bool m_inEdit;

    /// <summary>
    /// informations of the current edited item
    /// </summary>
    internal EditItemInformation m_editedItem = new EditItemInformation();

    /// <summary>
    /// Flag indicating whether the marquee selection tool is curently being used
    /// </summary>
    private bool m_hasMarquee = false;

    /// <summary>
    /// Selection mark
    /// </summary>
    private TreeListViewItem m_selectionMark = null;

    /// <summary>
    /// Updating flag
    /// </summary>
    private bool m_updating = false;

    /// <summary>
    /// Flag used to indicate that the constrol is to skip the mouse down event
    /// </summary>
    private bool m_skipMouseDownEvent = false;

    /// <summary>
    /// Check direction
    /// </summary>
    private CheckDirections m_checkDirection = CheckDirections.None;

    /// <summary>
    /// COMCTL32 version
    /// </summary>
    internal int m_comctl32Version;

    /// <summary>
    /// Last time a double click event occurred
    /// </summary>
    private DateTime m_lastDoubleClick;

    /// <summary>
    /// Last item clicked 
    /// </summary>
    private EditItemInformation m_lastItemClicked;

    /// <summary>
    /// Custom Edit
    /// </summary>
    private CustomEdit m_customEdit;

    /// <summary>
    /// Tree list view items collection
    /// </summary>
    private TreeListViewItemCollection m_items;

    /// <summary>
    /// Conponents
    /// </summary>
    private System.ComponentModel.IContainer components;

    /// <summary>
    /// Image list
    /// </summary>
    private System.Windows.Forms.ImageList m_imageList;

    /// <summary>
    /// Freeze check boxes flag
    /// </summary>
    private bool m_freezeCheckBoxes = false;

    /// <summary>
    /// Mouse scroll position
    /// </summary>
    private Point m_mouseScrollPosition = new Point(0, 0);

    /// <summary>
    /// Last time the doulbleclick event occurred
    /// </summary>
    private DateTime m_doubleClickTime = DateTime.Now;

    /// <summary>
    /// Plus-Minus image list
    /// </summary>
    private System.Windows.Forms.ImageList m_plusMinusImageList;

    /// <summary>
    /// Flag indicating whether a scroll bar is added to the control when there is not enough room to display all items
    /// </summary>
    private CheckBoxTypes m_checkBoxes = CheckBoxTypes.None;

    /// <summary>
    /// Expand Methods
    /// </summary>
    private TreeListViewExpandMethod m_expandMethod = TreeListViewExpandMethod.EntireItemDbleClick;

    /// <summary>
    /// Small image list
    /// </summary>
    private ImageList m_smallImageList = null;


    /// <summary>
    /// Sorting Order
    /// </summary>
    private SortOrder m_sortingOrder = SortOrder.Ascending;

    #endregion fields


    #region events

    /// <summary>
    /// Occurs when the label for an item is edited by the user.
    /// </summary>
    [Description("Occurs when the label for an item is edited by the user.")]
    public new event TreeListViewLabelEditEventHandler AfterLabelEdit;
    /// <summary>
    /// Occurs when the user starts editing the label of an item.
    /// </summary>
    [Description("Occurs when the user starts editing the label of an item."), Browsable(true)]
    public new event TreeListViewBeforeLabelEditEventHandler BeforeLabelEdit;
    /// <summary>
    /// Occurs before the tree node is collapsed.
    /// </summary>
    [Description("Occurs before the tree node is collapsed")]
    public event TreeListViewCancelEventHandler BeforeExpand;
    /// <summary>
    /// Occurs before the tree node is collapsed.
    /// </summary>
    [Description("Occurs before the tree node is collapsed")]
    public event TreeListViewCancelEventHandler BeforeCollapse;
    /// <summary>
    /// Occurs after the tree node is expanded
    /// </summary>
    [Description("Occurs after the tree node is expanded")]
    public event TreeListViewEventHandler AfterExpand;
    /// <summary>
    /// Occurs after the tree node is collapsed
    /// </summary>
    [Description("Occurs after the tree node is collapsed")]
    public event TreeListViewEventHandler AfterCollapse;


    
    /// <summary>
    /// Raises the AfterLabelEdit event.
    /// </summary>
    /// <param name="e"></param>
    protected virtual void OnAfterLabelEdit(TreeListViewLabelEditEventArgs e)
    {
      if(AfterLabelEdit != null)
      {
        AfterLabelEdit(this, e);
      }
    } // OnAfterLabelEdit(TreeListViewLabelEditEventArgs e)

    /// <summary>
    /// Please use OnAfterLabelEdit(TreeListViewLabelEditEventArgs e)
    /// </summary>
    /// <param name="e"></param>
    protected override void OnAfterLabelEdit(LabelEditEventArgs e)
    {
      throw new Exception("Please use OnAfterLabelEdit(TreeListViewLabelEditEventArgs e)");
    } // OnAfterLabelEdit(LabelEditEventArgs e)

    /// <summary>
    /// Raises the BeforeLabelEdit event.
    /// </summary>
    /// <param name="e"></param>
    protected virtual void OnBeforeLabelEdit(TreeListViewBeforeLabelEditEventArgs e)
    {
      if(BeforeLabelEdit != null) 
      {
        BeforeLabelEdit(this, e);
      }
    } // OnBeforeLabelEdit(TreeListViewBeforeLabelEditEventArgs e)

    /// <summary>
    /// Please use OnBeforeLabelEdit(TreeListViewLabelEditEventArgs e)
    /// </summary>
    /// <param name="e"></param>
    protected override void OnBeforeLabelEdit(LabelEditEventArgs e)
    {
      throw new Exception("Please use OnBeforeLabelEdit(TreeListViewLabelEditEventArgs e)");
    } // OnBeforeLabelEdit(LabelEditEventArgs e)

    /// <summary>
    /// Raises the BeforeExpand event.
    /// </summary>
    /// <param name="e"></param>
    protected virtual void OnBeforeExpand(TreeListViewCancelEventArgs e)
    {
      if(BeforeExpand != null) 
      {
        BeforeExpand(this, e);
      }
    } // OnBeforeExpand(TreeListViewCancelEventArgs e)

    /// <summary>
    /// Raises the AfterExpand event.
    /// </summary>
    /// <param name="e"></param>
    protected virtual void OnAfterExpand(TreeListViewEventArgs e)
    {
      if(AfterExpand != null) 
      { 
        AfterExpand(this, e);
      }
    } // OnAfterExpand(TreeListViewEventArgs e)

    /// <summary>
    /// Raises the BeforeCollapse event.
    /// </summary>
    /// <param name="e"></param>
    protected virtual void OnBeforeCollapse(TreeListViewCancelEventArgs e)
    {
      if(BeforeCollapse != null) 
      {
        BeforeCollapse(this, e);
      }

    } // OnBeforeCollapse(TreeListViewCancelEventArgs e)

    /// <summary>
    /// Raises the AfterCollapse event.
    /// </summary>
    /// <param name="e"></param>
    protected virtual void OnAfterCollapse(TreeListViewEventArgs e)
    {
      if( AfterCollapse != null) 
      {
        AfterCollapse(this, e);
      }
    } // OnAfterCollapse(TreeListViewEventArgs e)


    /// <summary>
    /// Raises the before expand event
    /// </summary>
    /// <param name="e"></param>
    internal void RaiseBeforeExpand(TreeListViewCancelEventArgs e)
    {
      OnBeforeExpand(e);
    } // RaiseBeforeExpand(TreeListViewCancelEventArgs e)

    /// <summary>
    /// Raises the before collapse event
    /// </summary>
    /// <param name="e"></param>
    internal void RaiseBeforeCollapse(TreeListViewCancelEventArgs e)
    {
      OnBeforeCollapse(e);
    } // RaiseBeforeCollapse(TreeListViewCancelEventArgs e)

    /// <summary>
    /// Raises the after expand event
    /// </summary>
    /// <param name="e"></param>
    internal void RaiseAfterExpand(TreeListViewEventArgs e)
    {
      OnAfterExpand(e);
    } // RaiseAfterExpand(TreeListViewEventArgs e)

    /// <summary>
    /// Raises the after collapse event
    /// </summary>
    /// <param name="e"></param>
    internal void RaiseAfterCollapse(TreeListViewEventArgs e)
    {
      OnAfterCollapse(e);
    } // RaiseAfterCollapse(TreeListViewEventArgs e)

    #endregion events


		#region properties

    /// <summary>
    /// Sets/Gets the updating flag
    /// </summary>
    [Browsable(false), Description("Gets the flag indicating that the control is to skip the mouse down event")]
    internal bool SkipMouseDownEvent
    {
      get { return m_skipMouseDownEvent; }
    } // SkipMouseDownEvent

    /// <summary>
    /// Sets/Gets the updating flag
    /// </summary>
    [Browsable(false), Description("Gets the flag indicating that the control is updating")]
    internal bool Updating
    {
      get { return m_updating; }
    } // Updating

    /// <summary>
    /// Sets/Gets the check direction
    /// </summary>
    [Browsable(false), Description("Gets or sets the check direction")]
    internal CheckDirections CheckDirection
    {
      get { return m_checkDirection; }
      set { m_checkDirection = value; }
    } // CheckDirection


    /// <summary>
    /// Sets/Gets the freeze check boxes flag
    /// </summary>
    [Browsable(false), Description("Gets or sets the flag to freeze check boxes")]
    internal bool FreezeCheckBoxes
    {
      get { return m_freezeCheckBoxes; }
      set { m_freezeCheckBoxes = value; }
    } // FreezeCheckBoxes


    /// <summary>
    /// Sets/Gets the plus minus image list
    /// </summary>
    [Browsable(false), Description("Gets or sets the plus minus image list")]
    internal System.Windows.Forms.ImageList PlusMinusImageList
    {
      get { return m_plusMinusImageList; }
      set { m_plusMinusImageList = value; }
    } // PlusMinusImageList

    /// <summary>
    /// Sets/Gets the plus minus image list
    /// </summary>
    [Browsable(false), Description("Gets or sets the last ietem clicked")]
    internal EditItemInformation LastItemClicked
    {
      get { return m_lastItemClicked; }
      set { m_lastItemClicked = value; }
    } // LastItemClicked

    /// <summary>
    /// Gets or sets a value indicating whether a check box appears next to each item in the control
    /// </summary>
    [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible), DefaultValue(typeof(CheckBoxTypes), "None")]
    [Browsable(true), Description("Gets or sets a value indicating whether a check box appears next to each item in the control")]
    new public CheckBoxTypes CheckBoxes
    {
      get { return m_checkBoxes; }
      set
      {

        /////////////////////////////////////////////////////////////////////
        // Do not continu if the value is the same as the current value
        /////////////////////////////////////////////////////////////////////
        if(m_checkBoxes == value) 
        {
          return;
        } // END ... If the value is the came

        m_checkBoxes      = value;
        m_checkDirection  = (value == CheckBoxTypes.Recursive ? CheckDirections.All : CheckDirections.None);
        base.CheckBoxes   = (value  == CheckBoxTypes.None ? false : true);

        if(Created)
        {
          Invalidate();
        } // END ... If the control is created
      }
    } // CheckBoxes

    /// <summary>
    /// Gets or sets a value indicating whether clicking an item selects all its subitems
    /// </summary>
    [Browsable(true), Description("Gets or sets a value indicating whether clicking an item selects all its subitems"), DefaultValue(true)]
    new public bool FullRowSelect
    {
      get { return base.FullRowSelect; }
      set { base.FullRowSelect = value; }
    } // FullRowSelect

    /// <summary>
    /// Not supported
    /// </summary>
    [Browsable(false)]
    new public ImageList StateImageList
    {
      get { return base.StateImageList; }
      set { base.StateImageList = value; }
    } // StateImageList

    /// <summary>
    /// Not supported
    /// </summary>
    [Browsable(false)]
    new public ImageList LargeImageList
    {
      get { return base.LargeImageList; }
      set { base.LargeImageList = value; }
    } // LargeImageList

    /// <summary>
    /// Gets or sets the ImageList to use when displaying items as small icons in the control (must be filled in)
    /// </summary>
    [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible), DefaultValue(null)]
    [Browsable(true), Description("Gets or sets the ImageList to use when displaying items as small icons in the control")]
    new public ImageList SmallImageList
    {
      get { return m_smallImageList; }
      set { m_smallImageList = value; }
    }// SmallImageList
   
    /// <summary>
    /// Get or set the sort order
    /// </summary>
    [DesignerSerializationVisibility(DesignerSerializationVisibility.Visible)]
    [Browsable(true), Description("Get or Set the sort order"), DefaultValue(typeof(SortOrder), "Ascending")]
    new public SortOrder Sorting
    {
      get { return m_sortingOrder; }
      set
      {
        if(m_sortingOrder == value) 
        {
          return;
        }

        m_sortingOrder = value;

        Items.SortOrderRecursively = value;

      }
    } // SortingOrder


    /// <summary>
    /// Get or set the expand method
    /// </summary>
    [Browsable(true), DefaultValue(typeof(TreeListViewExpandMethod), "EntireItemDbleClick"),
    Description("Get or Set the expand method")]
    public TreeListViewExpandMethod ExpandMethod
    {
      get { return m_expandMethod; }
      set { m_expandMethod = value; }
    } // ExpandMethod

    /// <summary>
    /// View (always Details)
    /// </summary>
    [DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
    [Browsable(false)]
    public new View View
    {
      get { return base.View; }
      set { base.View = View.Details; }
    } // View


    /// <summary>
    /// Items of the TreeListView
    /// </summary>
    //		[Browsable(true),
    //		Editor(typeof(TreeListViewItemsEditor), typeof(System.Drawing.Design.UITypeEditor))]
    [Browsable(false)]
    [Description("Items of the TreeListView")]
    new public TreeListViewItemCollection Items
    {
      get { return m_items; }
    } // Items


    /// <summary>
    /// Get currently selected items
    /// </summary>
    [Browsable(false)]
    new public SelectedTreeListViewItemCollection SelectedItems
    {
      get { return new SelectedTreeListViewItemCollection(this); }
    } // SelectedItems

    /// <summary>
    /// Get currently checked items
    /// </summary>
    [Browsable(false)]
    public new TreeListViewItem[] CheckedItems
    {
      get  { return (TreeListViewItem[])Invoke(new ItemArrayHandler(GetCheckedItems)); }
    } // CheckedItems


    /// <summary>
    /// Gets the item in the control that currently has focus.
    /// </summary>
    [Browsable(false)]
    new public TreeListViewItem FocusedItem
    {
      get { return (TreeListViewItem)base.FocusedItem; }
    } // FocusedItem
  

		/// <summary>
		/// Gets whether the marquee selection tool is curently being used
		/// </summary>
		[Browsable(false)]
		public bool HasMarquee
		{
			get { return m_hasMarquee;	}
		} // HasMarquee


		/// <summary>
		/// Gets the informations of the current edited item
		/// </summary>
		[Browsable(false)]
		public EditItemInformation EditedItem
		{
			get { return m_editedItem; }
		} // EditedItem


		/// <summary>
		/// Gets whether an item is currently edited
		/// </summary>
		[Browsable(false)]
		public bool InEdit
		{
			get { return m_inEdit;	}
		} // InEdit


 		/// <summary>
		/// Get the number of items recursively
		/// </summary>
		[Browsable(false)]
		public int ItemsCount
		{
			get
			{

				TreeListViewItem[] items = m_items.ToArray();

				int count = items.Length;

        foreach(TreeListViewItem item in items) 
        {
          count += item.ChildrenCount;
        } // END ... For each intem in treelistview

				return count;
			}
		} // ItemsCount


		/// <summary>
		/// Get or set the comparer
		/// </summary>
		[Browsable(false)]
		public ITreeListViewItemComparer Comparer
		{
			get  {return(Items.Comparer); }
			set { Items.Comparer = value; }
		} // Comparer
    

    /// <summary>
		/// Gets or sets a value indicating whether plus-sign (+) and minus-sign (-) buttons are displayed next to TreeListView that contain child TreeListViews
		/// </summary>
		[DesignerSerializationVisibility(DesignerSerializationVisibility.Visible), DefaultValue(true)]
		[Browsable(true), Description("Gets or sets a value indicating whether plus-sign (+) and minus-sign (-) buttons are displayed next to TreeListView that contain child TreeListViews")]
		public bool ShowPlusMinus
		{
			get {	return m_showPlusMinus;	}
			set
			{
        // If the value is the same just return
        if( m_showPlusMinus == value) 
        {
          return;
        } // END ... If the new value is the same as the current value

				m_showPlusMinus = value;

        if(Created) 
        {
          Invoke(new MethodInvoker(VisibilityChange));
        } // END ... If the constrol is created

			} // set
		} // ShowPlusMinus


		/// <summary>
		/// Gets or Sets the color of the lines if ShowPlusMinus property is enabled
		/// </summary>
		[DesignerSerializationVisibility(DesignerSerializationVisibility.Visible), DefaultValue(typeof(Color), "DarkGray")]
		[Browsable(true), Description("Gets or Sets the color of the lines if ShowPlusMinus property is enabled")]
		public Color PlusMinusLineColor
		{
			get { return m_plusMinusLineColor;	}
			set
			{
				m_plusMinusLineColor = value;

        /////////////////////////////////////////////////////////////////////
        // If the constrol is created invalidate it so that it is redrawn
        //////////////////////////////////////////////////////////////////////
        if(Created) 
        {
          Invalidate();
        } // END ... If Ccontrol is created
			}
		} // PlusMinusLineColor


		/// <summary>
		/// Gets or Sets whether the control draw XP-Style highlight color
		/// </summary>
		[DesignerSerializationVisibility(DesignerSerializationVisibility.Visible), DefaultValue(true)]
		[Browsable(true), Description("Gets or Sets whether the control draw XP-Style highlight color")]
		public bool UseXPHighlightStyle
		{
			get { return m_useXPHighLightStyle;	}
			set	
      {	
        m_useXPHighLightStyle = value; 

        //////////////////////////////////////////////////////////
        // If created we ned to invalidate if toe be redrawn
        //////////////////////////////////////////////////////////
        if(Created) 
        {
          Invalidate();
        } // END ... If the control is created
			}
		} // UseXPHighlightStyle


		/// <summary>
		/// Gets or sets the delimiter string that the TreeListViewItem path uses
		/// </summary>
		[DesignerSerializationVisibility(DesignerSerializationVisibility.Visible), DefaultValue("\\")]
		[Browsable(true), Description("Gets or sets the delimiter string that the TreeListViewItem path uses")]
		public string PathSeparator
		{
			get { return m_pathSeparator; }
			set {	m_pathSeparator = value; }
		} // PathSeparator


		#endregion  properties

    #region constructors and destructors

		/// <summary>
		/// Create a new instance of a TreeListView
		/// </summary>
		public TreeListView()
		{
			InitializeComponent();

      if( !IsHandleCreated ) 
      {
        CreateHandle();
      }

			m_items           = new TreeListViewItemCollection(this);
			m_items.SortOrder = m_sortingOrder;
			m_comctl32Version = Win32.ComCtl32.GetMajorVersion();

			int style = Win32.User32.SendMessage(Handle, (int) Win32.ListViewMessages.LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0);
			style |= (int) (Win32.ListViewExtendedStyles.LVS_EX_INFOTIP | Win32.ListViewExtendedStyles.LVS_EX_LABELTIP);
			Win32.User32.SendMessage(Handle, (int) Win32.ListViewMessages.LVM_SETEXTENDEDLISTVIEWSTYLE, 0, style);
		} // TreeListView()


    /// <summary>
    /// Disposes of the object
    /// </summary>
    protected override void Dispose( bool disposing )
    {
      if( disposing )
      {
        if( components != null )
        {
          components.Dispose();
        }
      }

      base.Dispose( disposing );
    } // Dispose()

		#endregion constructors and destructors

    #region Component Designer generated code

    /// <summary>
    /// InitializeComponent
    /// </summary>
    private void InitializeComponent()
    {
      this.components = new System.ComponentModel.Container();
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(TreeListView));
      this.m_imageList = new System.Windows.Forms.ImageList(this.components);
      this.m_plusMinusImageList = new System.Windows.Forms.ImageList(this.components);
      this.SuspendLayout();
      // 
      // m_imageList
      // 
      this.m_imageList.ColorDepth = System.Windows.Forms.ColorDepth.Depth32Bit;
      this.m_imageList.ImageSize = new System.Drawing.Size(16, 16);
      this.m_imageList.TransparentColor = System.Drawing.Color.Transparent;
      // 
      // m_plusMinusImageList
      // 
      this.m_plusMinusImageList.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("m_plusMinusImageList.ImageStream")));
      this.m_plusMinusImageList.TransparentColor = System.Drawing.Color.Transparent;
      this.m_plusMinusImageList.Images.SetKeyName(0, "Plus.gif");
      this.m_plusMinusImageList.Images.SetKeyName(1, "Minus.gif");
      // 
      // TreeListView
      // 
      this.FullRowSelect = true;
      this.View = System.Windows.Forms.View.Details;
      this.ResumeLayout(false);

    } // InitializeComponent()

    #endregion

    #region methods


    /// <summary>
    /// Kill the focus of the control
    /// </summary>
    public void KillFocus()
    {
      Win32.User32.SendMessage( Handle, (int) Win32.WindowMessages.WM_KILLFOCUS, IntPtr.Zero, IntPtr.Zero);
    } // KillFocus()

    /// <summary>
    /// Sets the selected item range
    /// </summary>
    /// <param name="item1"></param>
    /// <param name="item2"></param>
    private void SetSelectedItemsRange(TreeListViewItem item1, TreeListViewItem item2)
    {
      if(InvokeRequired)
      {
        throw new Exception("Invoke required");
      }

      if(item1 == null || item2 == null) 
      {
        return;
      }

      if(!item1.Visible || !item2.Visible) 
      {
        return;
      }

      if(item1.Parent != item2.Parent) 
      {
        return;
      }

      TreeListViewItemCollection items = item1.Container;
      int index1 = items.GetIndexOf(item1);
      int index2 = items.GetIndexOf(item2);
      ListViewItem[] selItems = new ListViewItem[base.SelectedItems.Count];
      base.SelectedItems.CopyTo(selItems, 0);

      foreach(ListViewItem selItem in selItems)
      {
        int selItemIndex = items.GetIndexOf((TreeListViewItem)selItem);
        if(selItemIndex < Math.Min(index1, index2) ||
          selItemIndex > Math.Max(index1, index2))
        {
          selItem.Selected = false;
        }
      } // END ... For each selected item

      for(int i = Math.Min(index1, index2); i <= Math.Max(index1, index2); i++)
      {
        if(!items[i].Selected) 
        {
          items[i].Selected = true;
        }
      }

    } // SetSelectedItemsRange(TreeListViewItem item1, TreeListViewItem item2)


    /// <summary>
    /// Expands all the tree nodes
    /// </summary>
    public void ExpandAll()
    {
      if(InvokeRequired)
      {
        throw new Exception("Invoke required");
      }

      BeginUpdate();

      foreach(TreeListViewItem item in Items)
      {
        item.ExpandAllInternal();
      }

      EndUpdate();
    } // ExpandAll()

    /// <summary>
    /// Collapses all the tree nodes
    /// </summary>
    public void CollapseAll()
    {
      if(InvokeRequired)
      {
        throw new Exception("Invoke required");
      }

      BeginUpdate();

      foreach(TreeListViewItem item in Items)
      {
        item.CollapseAllInternal();
      }

      EndUpdate();

    } // CollapseAll()



    /// <summary>
    /// Exit the editing of a cell
    /// </summary>
    /// <param name="Cancel"></param>
    /// <param name="Text"></param>
    internal void ExitEdit(bool Cancel, string Text)
    {

      if( !InEdit || EditedItem.Item == null) 
      {
        return;
      }

      // Mouse position
      Point pos = ( EditedItem.Item != null ? EditedItem.Item.GetBounds(TreeListViewItemBoundsPortion.Icon).Location : new Point(0, 0) );
      pos.Offset(1,1);
      EditItemInformation editedItem = EditedItem;

      Message m = Message.Create(Handle, (int) Win32.WindowMessages.WM_LBUTTONDOWN, (IntPtr)1, (IntPtr) ((pos.Y << 16) + pos.X));
      m_skipMouseDownEvent = true;
      base.WndProc(ref m);
      m_skipMouseDownEvent = false;

      if( !Cancel )
      {

        TreeListViewLabelEditEventArgs e = new TreeListViewLabelEditEventArgs(EditedItem.Item, EditedItem.ColumnIndex, Text);

        OnAfterLabelEdit(e);

        if(!e.Cancel)
        {
          editedItem.Item.SubItems[editedItem.ColumnIndex].Text = Text;
        }
      }

      m_inEdit = false;

      m_editedItem = new EditItemInformation(null, 0, "");

    } // ExitEdit(bool Cancel, string Text)

    /// <summary>
    /// Retrieves the specified portion of the bounding rectangle for a specific item within the list view control
    /// </summary>
    /// <param name="index">The zero-based index of the item within the ListView.ListViewItemCollection whose bounding rectangle you want to return</param>
    /// <param name="portion">One of the TreeListViewItemBoundsPortion values that represents a portion of the TreeListViewItem for which to retrieve the bounding rectangle</param>
    /// <returns>A Rectangle that represents the bounding rectangle for the specified portion of the specified TreeListViewItem</returns>
    public Rectangle GetItemRect(int index, TreeListViewItemBoundsPortion portion)
    {
      if(index >= base.Items.Count || index < 0)
      {
        throw new Exception("Out of range exception");
      } 

      TreeListViewItem item = (TreeListViewItem) base.Items[index];
      return item.GetBounds(portion);
    } // GetItemRect(int index, TreeListViewItemBoundsPortion portion)


    /// <summary>
    /// Raises the ItemCheck event
    /// </summary>
    /// <param name="e">An ItemCheckEventArgs that contains the event data</param>
    protected override void OnItemCheck(System.Windows.Forms.ItemCheckEventArgs e)
    {
      base.OnItemCheck(e);

      ListView.ListViewItemCollection baseItems = base.Items;

      if(e.Index >= base.Items.Count || e.Index < 0)
      {
        return;
      }

      TreeListViewItem item = (TreeListViewItem) base.Items[e.Index];
      if( item == null || m_checkDirection == CheckDirections.None) 
      {
        return; 
      }

      CheckDirections oldDirection = m_checkDirection; 

      TreeListViewItem parentItem = item.Parent;

      if(parentItem != null && (oldDirection & CheckDirections.Upwards) == CheckDirections.Upwards)
      {
        m_checkDirection = CheckDirections.Upwards;
        while(parentItem != null)
        {
          if(e.NewValue == CheckState.Checked)
          {
            if(!parentItem.Checked)
            {
              parentItem.Checked = true;
              break;
            }
            else
            {
              bool allChecked = true;
              foreach(TreeListViewItem childItem in parentItem.Items)
              {
                if(childItem == item) 
                {
                  continue;
                }
                else if( !childItem.Checked )
                {
                  allChecked = false;
                  break;
                }
              }   // END ... For each shild item

              if( allChecked ) 
              {
                parentItem.Redraw();
              }

            }
          }
          else
          {
            bool allUnChecked = true;
            foreach(TreeListViewItem childItem in parentItem.Items)
            {
              if(childItem == item) 
              {
                continue;
              }
              else if(childItem.Checked)
              {
                allUnChecked = false;
                break;
              }
            } 

            if(allUnChecked && parentItem.Checked)
            {
              parentItem.Checked = false;
              break;
            }

          }

          parentItem = parentItem.Parent;

        }

      }

      if((oldDirection & CheckDirections.Downwards) == CheckDirections.Downwards)
      {
        m_checkDirection = CheckDirections.Downwards;
        foreach(TreeListViewItem childItem in item.Items)
        {
          childItem.Checked = e.NewValue == CheckState.Checked;
        }
      }

      m_checkDirection = oldDirection;

    } // OnItemCheck(System.Windows.Forms.ItemCheckEventArgs e)

    /// <summary>
    /// Raises the ColumnClick event
    /// </summary>
    /// <param name="e">A ColumnClickEventArgs that contains the event data</param>
    protected override void OnColumnClick(System.Windows.Forms.ColumnClickEventArgs e) 
    { 

      base.OnColumnClick(e);
      Cursor = Cursors.WaitCursor;
      ListViewItem[] selItems = new ListViewItem[base.SelectedItems.Count];
      base.SelectedItems.CopyTo(selItems, 0);

      // Must set ListView.checkDirection to CheckDirections.None. 
      // Forbid recursively checking. 
      CheckDirections oldDirection = m_checkDirection;
      m_checkDirection = CheckDirections.None;

      BeginUpdate();

      if(Comparer.Column == e.Column)
      {
        Sorting = (Sorting == SortOrder.Ascending ? SortOrder.Descending : SortOrder.Ascending);
      }
      else
      {
        Comparer.Column = e.Column;
        Items.SortOrderRecursivelyWithoutSort = SortOrder.Ascending;
        try{Items.Sort(true);}
        catch{}
      }

      if(FocusedItem != null) 
      {
        FocusedItem.EnsureVisible();
      }

      foreach(ListViewItem item in selItems)
      {
        if(item.Index > -1) item.Selected = true;
      }

      EndUpdate();

      // Reset ListView.checkDirection
      m_checkDirection = oldDirection;
      Cursor = Cursors.Default;

    } // OnColumnClick(System.Windows.Forms.ColumnClickEventArgs e) 

    /// <summary>
    /// Raises the VisibleChanged event
    /// </summary>
    /// <param name="e"></param>
    protected override void OnVisibleChanged(EventArgs e)
    {
      base.OnVisibleChanged(e);
      if(base.SmallImageList != m_smallImageList)
      {
        base.SmallImageList = m_smallImageList;
      }

      VisibilityChange();

    } // OnVisibleChanged(EventArgs e)

    /// <summary>
    /// Visibility changes
    /// </summary>
    internal void VisibilityChange()
    {

      /////////////////////////////////////
      // Do nothing if we are not visible
      /////////////////////////////////////
      if( ! Visible ) 
      {
        return;
      } // END ... If we are not visible

      // Do not redraw
      BeginUpdate();

      try
      {

        //////////////////////////////////////////////////////////////////////////
        // Loop through all the items in the list refreshing their indentation
        //////////////////////////////////////////////////////////////////////////
        foreach(TreeListViewItem item in this.Items)
        {
          item.RefreshIndentation(true);
        } // END ... For each item in list

      }
      catch
      {
        // Ignore
      }

      //////////////////////////////////////////////////////////////////
      // If there is a focused item make sure that item is visible
      //////////////////////////////////////////////////////////////////
      if( FocusedItem != null ) 
      {
        FocusedItem.EnsureVisible();
      } // END ... If there is a focused item

      // Allow redraw
      EndUpdate();

    } // VisibilityChange()

    /// <summary>
    /// Gets an item at the specified coordinates
    /// </summary>
    /// <param name="p">Mouse position</param>
    /// <returns></returns>
    public TreeListViewItem GetItemAt(Point p)
    {
      return GetItemAt(p.X, p.Y);
    } // GetItemAt(Point p)

    /// <summary>
    /// Gets an item at the specified coordinates (fullrow)
    /// </summary>
    /// <param name="p">Mouse position</param>
    /// <returns></returns>
    public TreeListViewItem GetItemAtFullRow(Point p)
    {

      if(FullRowSelect) 
      {
        return GetItemAt(p);
      }

      TreeListViewItemCollection items = GetVisibleItems();

      foreach(TreeListViewItem item in items)
      {
        if(item.GetBounds(TreeListViewItemBoundsPortion.Entire).Contains(p))
        {
          return item;
        }
      }

      return null;
    } // GetItemAtFullRow(Point p)

    /// <summary>
    /// Gets an item at the specified coordinates.
    /// </summary>
    /// <param name="x"></param>
    /// <param name="y"></param>
    /// <returns></returns>
    new public TreeListViewItem GetItemAt(int x, int y)
    {
      return (TreeListViewItem) base.GetItemAt(x, y);
    } // GetItemAt(int x, int y)

    /// <summary>
    /// Gets the TreeListViewItem from the ListView index of the item
    /// </summary>
    /// <param name="index">Index of the Item</param>
    /// <returns></returns>
    public TreeListViewItem GetTreeListViewItemFromIndex(int index)
    {
      
      if(base.Items.Count < index + 1) 
      {
        return(null);
      }

      return((TreeListViewItem) base.Items[index]);
    } // GetTreeListViewItemFromIndex(int index)

    /// <summary>
    /// Not supported (use items.Sort)
    /// </summary>
    new public void Sort()
    {
      if(InvokeRequired)
        throw(new Exception("Invoke required"));
      Items.Sort(true);
    } // Sort()


    /// <summary>
    /// Prevents the control from drawing until the EndUpdate method is called
    /// </summary>
    new public void BeginUpdate()
    {
      m_updating = true;
      base.BeginUpdate();
    } // BeginUpdate()

    /// <summary>
    /// Resumes drawing of the list view control after drawing is suspended by the BeginUpdate method
    /// </summary>
    new public void EndUpdate()
    {
      m_updating = false;
      base.EndUpdate();
    } // EndUpdate()



    /// <summary>
    /// Returns an array of check items
    /// </summary>
    /// <returns></returns>
    private TreeListViewItem[] GetCheckedItems()
    {
      if(InvokeRequired)
      {
        throw new Exception("Invoke required");
      }

      TreeListViewItemCollection items = new TreeListViewItemCollection();
      foreach(TreeListViewItem item in Items)
      {
        item.GetCheckedItems(ref items);
      } // END ... FOr each item in list

      return items.ToArray();

    } // GetCheckedItems()

    #region WndProc

		/// <summary>
		/// WndProc
		/// </summary>
		/// <param name="m"></param>
		protected override void WndProc(ref System.Windows.Forms.Message m)
		{
			#region View messages

			if(false)
			{
				string val;
				val = Enum.GetName(typeof(Win32.ListViewMessages), (Win32.ListViewMessages) m.Msg);
				if(val != "" && val != null)
					Debug.WriteLine(val);
				else
				{
					val = Enum.GetName(typeof(Win32.WindowMessages), (Win32.WindowMessages) m.Msg);
					if(val != "" && val != null)
					Debug.WriteLine(val);
				}
				if(val != "" && val != null)
					Debug.WriteLine(m.Msg.ToString());
				if(val == "LBUTTONDOWN")
				{
					int a= 0;
					a++;
				}
			}
			#endregion


			TreeListViewItem item = null; Rectangle rec;

			switch((Win32.WindowMessages) m.Msg)
			{

				case Win32.WindowMessages.WM_NOTIFY:
				case (Win32.WindowMessages) Win32.ReflectedMessages.OCM_NOTIFY:

					Win32.NMHDR nmhdr = (Win32.NMHDR) m.GetLParam(typeof(Win32.NMHDR));
					Win32.NMHEADER nmheader =(Win32.NMHEADER) m.GetLParam(typeof(Win32.NMHEADER));


          //
					switch((Win32.ListViewNotifications) nmhdr.code)
					{

            case Win32.ListViewNotifications.LVN_MARQUEEBEGIN:
            {
              if((MouseButtons & MouseButtons.Left) != MouseButtons.Left)
              {
                m.Result = (IntPtr)1;
              }
              else
              {
                m_hasMarquee = true;
              }
            }
            break;
						
            
            case Win32.ListViewNotifications.LVN_ITEMCHANGING:
            {

              Win32.NMLISTVIEW nmlistview = (Win32.NMLISTVIEW) m.GetLParam(typeof(Win32.NMLISTVIEW));

              if( nmlistview.iItem < 0 || (item = GetTreeListViewItemFromIndex(nmlistview.iItem)) == null) 
              {
                break;
              }

              bool cancel = false;

              if(nmlistview.Select)
              {
                if(m_selectionMark == null) 
                {
                  m_selectionMark = item;
                }
                else if(!m_selectionMark.Visible) 
                {
                  m_selectionMark = item;
                }

                if(HasMarquee) 
                {
                  item.Focused = true;
                }

              }
              else if(nmlistview.UnSelect && HasMarquee)
              {

                if(item.NextVisibleItem != null)
                {
                  if(item.NextVisibleItem.Selected)
                  {
                    item.NextVisibleItem.Focused = true;
                  }
                }

                if(item.PrevVisibleItem != null)
                {
                  if(item.PrevVisibleItem.Selected)
                  {
                    item.PrevVisibleItem.Focused = true;
                  }
                }

              }

              #region Select after dbl click
              // Disable the selection after a double click (normaly, if the control scrolls after
              // a collapse, the new item under the cursor is automatically selected...)
              if(m_doubleClickTime.AddMilliseconds(500).CompareTo(DateTime.Now) > 0 &&
                (nmlistview.Select || nmlistview.Focus) &&
                FocusedItem != item)
                cancel = true;
              #endregion
              #region Wrong Level Select
              if(((Win32.ListViewItemStates)nmlistview.uNewState & Win32.ListViewItemStates.LVIS_SELECTED) == Win32.ListViewItemStates.LVIS_SELECTED &&
                MultiSelect)
                if(SelectedIndices.Count > 0)
                  if(GetTreeListViewItemFromIndex(nmlistview.iItem).Parent != SelectedItems[0].Parent)
                    cancel = true;
              #endregion
              #region Check during selection
              // Disable check boxes check when :
              // - the Marquee selection tool is being used
              // - the Ctrl or Shift keys are down
              bool state = (nmlistview.uChanged & (uint)Win32.ListViewItemFlags.LVIF_STATE) == (uint)Win32.ListViewItemFlags.LVIF_STATE;
              bool ctrlKeyDown = (ModifierKeys & Keys.Control) == Keys.Control;
              bool shiftKeyDown = (ModifierKeys & Keys.Shift) == Keys.Shift;
              if((nmlistview.Check || nmlistview.UnCheck) &&
                (HasMarquee || ctrlKeyDown || shiftKeyDown))
              {
                //									MessageBox.Show(this,
                //										"uChanged = " + nmlistview->uChanged.ToString() + "\n\n" + 
                //										"uOld = " + nmlistview->uOldState.ToString() + "\n" + 
                //										"uNew = " + nmlistview->uChanged.ToString() + "\n\n" +
                //										"OldCheck : " + (oldCheck ? "true" : "false") + "\n" + 
                //										"NewCheck : " + (newCheck ? "true" : "false"));
                cancel = true;
              }
              #endregion
              if(cancel)
              {
                m.Result = (IntPtr)1;
                return;
              }
            }
            break;

						#region Win32.ListViewNotifications.BEGINLABELEDIT

            case Win32.ListViewNotifications.LVN_BEGINLABELEDIT:
            {
              // Cancel label edit if the message is sent just after a double click
              if(m_lastDoubleClick.AddMilliseconds(450) > DateTime.Now)
              {
                Message canceledit = Message.Create(Handle, (int) Win32.ListViewMessages.LVM_CANCELEDITLABEL, IntPtr.Zero, IntPtr.Zero);
                WndProc(ref canceledit);
                m.Result = (IntPtr) 1;
                return;
              }
              item = m_lastItemClicked.Item;
              item.EnsureVisible();
              // Add subitems if needed
              while(item.SubItems.Count-1 < m_lastItemClicked.ColumnIndex) 
              {
                item.SubItems.Add("");
              }

              TreeListViewBeforeLabelEditEventArgs beforeed = 
                  new TreeListViewBeforeLabelEditEventArgs(FocusedItem, m_lastItemClicked.ColumnIndex, item.SubItems[m_lastItemClicked.ColumnIndex].Text);
              
              OnBeforeLabelEdit(beforeed);

              if(beforeed.Cancel)
              {
                Message canceledit = Message.Create(Handle, (int) Win32.ListViewMessages.LVM_CANCELEDITLABEL, IntPtr.Zero, IntPtr.Zero);
                WndProc(ref canceledit);
                m.Result = (IntPtr) 1;
                return;
              }

              m_inEdit = true;

              // Get edit handle
              Message mess = Message.Create(Handle, (int)Win32.ListViewMessages.LVM_GETEDITCONTROL, IntPtr.Zero, IntPtr.Zero);
              WndProc(ref mess);
              IntPtr edithandle = mess.Result;
              m_customEdit = new CustomEdit(edithandle, this, beforeed.Editor);
              m_editedItem = new EditItemInformation(FocusedItem, beforeed.ColumnIndex, FocusedItem.SubItems[beforeed.ColumnIndex].Text);
              m.Result = IntPtr.Zero;
              return;
            }

						#endregion Win32.ListViewNotifications.BEGINLABELEDIT

						#region Win32.ListViewNotifications.ENDLABELEDIT
						case Win32.ListViewNotifications.LVN_ENDLABELEDIT:
							if(m_customEdit != null)
								m_customEdit.HideEditControl();
							m_customEdit = null;
							m_inEdit = false;
							m_editedItem = new EditItemInformation();
							m.Result = IntPtr.Zero;
							return;
						#endregion
						
						#region CUSTOMDRAW
						case (Win32.ListViewNotifications) Win32.NotificationMessages.NM_CUSTOMDRAW:
							base.WndProc(ref m);
							CustomDraw(ref m);
							return;
						#endregion

						#region BEGINSCROLL
						case Win32.ListViewNotifications.LVN_BEGINSCROLL:
							m_updating = true;
							break;
						#endregion
						#region ENDSCROLL
						case Win32.ListViewNotifications.LVN_ENDSCROLL:
							m_updating = false;
							// Disable display bug with vertical lines (slow...)
//							if(ShowPlusMinus)
//							{
//								DrawPlusMinusItemsLines();
//								DrawPlusMinusItems();
//							}
							break;
						#endregion

						#region Win32.HeaderControlNotifications.BEGINDRAG
						case (Win32.ListViewNotifications) Win32.HeaderControlNotifications.HDN_BEGINDRAG:
							nmheader =(Win32.NMHEADER) m.GetLParam(typeof(Win32.NMHEADER));
							if(nmheader.iItem == 0)
							{
								m.Result = (IntPtr)1;
								return;
							}
							break;
						#endregion
						#region Win32.HeaderControlNotifications.ENDDRAG
						case (Win32.ListViewNotifications) Win32.HeaderControlNotifications.HDN_ENDDRAG:
							nmheader =(Win32.NMHEADER) m.GetLParam(typeof(Win32.NMHEADER));
							// Get mouse position in header coordinates
							IntPtr headerHandle = (IntPtr) Win32.User32.SendMessage(Handle, (int) Win32.ListViewMessages.LVM_GETHEADER, IntPtr.Zero, IntPtr.Zero);
							Win32.POINTAPI pointapi = new Win32.POINTAPI(MousePosition);
							Win32.User32.ScreenToClient(headerHandle, ref pointapi);
							// HeaderItem Rect
							Win32.RECT headerItemRect = new Win32.RECT();
							Win32.User32.SendMessage(headerHandle, (int)Win32.HeaderControlMessages.HDM_GETITEMRECT, 0, ref headerItemRect);
							int headerItemWidth = headerItemRect.right - headerItemRect.left;
							// Cancel the drag operation if the first column is moved
							// or destination is the first column
							if(pointapi.x <= headerItemRect.left + headerItemWidth / 2 ||
								nmheader.iItem == 0)
							{
								m.Result = (IntPtr)1;
								return;
							}
							break;
						#endregion
						#region Win32.HeaderControlNotifications.TRACK / ENDTRACK
//						case (Win32.ListViewNotifications)Win32.HeaderControlNotifications.TRACK:
						case (Win32.ListViewNotifications)Win32.HeaderControlNotifications.HDN_ENDTRACK:
							Invalidate();
							break;
						#endregion
					}
					break;

				#endregion

				#region LBUTTONDOWN
					// Cancel the click on checkboxes if the item is not "checkable"
					case Win32.WindowMessages.WM_LBUTTONDOWN:
						if(Columns.Count == 0) break;
						// Set the clickeditem and column
						int colclicked = GetColumnAt(MousePosition);
						if(colclicked == -1) colclicked = 0;
						item = GetItemAtFullRow(PointToClient(MousePosition));
						m_lastItemClicked = new EditItemInformation(item, colclicked, "");
						if(m_selectionMark == null || !m_selectionMark.Visible) m_selectionMark = item;
						if(((Win32.KeyStatesMasks)(int)m.WParam & Win32.KeyStatesMasks.MK_SHIFT) != Win32.KeyStatesMasks.MK_SHIFT &&
							!(((Win32.KeyStatesMasks)(int)m.WParam & Win32.KeyStatesMasks.MK_CONTROL) == Win32.KeyStatesMasks.MK_CONTROL &&
							item.Parent != m_selectionMark.Parent))
							m_selectionMark = item;
						// Get where the mouse has clicked
						Win32.LVHITTESTINFO lvhittest = new Win32.LVHITTESTINFO();
						lvhittest.pt = new Win32.POINTAPI(PointToClient(MousePosition));
						Win32.User32.SendMessage(Handle, (Int32) Win32.ListViewMessages.LVM_HITTEST, 0, ref lvhittest);
						if(item == null) break;
						// Plus / Minus click
						if(item.GetBounds(TreeListViewItemBoundsPortion.PlusMinus).Contains(PointToClient(MousePosition)) &&
							ShowPlusMinus && item.Items.Count > 0 &&
							Columns[0].Width > (item.Level+1)*SystemInformation.SmallIconSize.Width)
						{
							Focus();
							if(item.IsExpanded) item.Collapse();
							else item.Expand();
							OnMouseDown(new MouseEventArgs(MouseButtons.Left, 1, PointToClient(MousePosition).X, PointToClient(MousePosition).Y, 0));
							return;
						}
						// Cancel mouse click if multiselection on a wrong item
						if(SelectedIndices.Count > 0 &&
							(((Win32.KeyStatesMasks)(int)m.WParam & Win32.KeyStatesMasks.MK_SHIFT) == Win32.KeyStatesMasks.MK_SHIFT ||
							((Win32.KeyStatesMasks)(int)m.WParam & Win32.KeyStatesMasks.MK_CONTROL) == Win32.KeyStatesMasks.MK_CONTROL) &&
							MultiSelect)
						{
							if(m_selectionMark.Parent == item.Parent &&
								((Win32.KeyStatesMasks)(int)m.WParam & Win32.KeyStatesMasks.MK_SHIFT) == Win32.KeyStatesMasks.MK_SHIFT)
							{
								m_updating = true;
								SetSelectedItemsRange(item, m_selectionMark);
								// Prevent all item at the wrong level of being selected
								m.WParam = (IntPtr) Win32.KeyStatesMasks.MK_CONTROL;
								base.WndProc(ref m);
								item.Selected = true;
								m_updating = false;
								DrawSelectedItemsFocusCues();
								return;
							}
						}
						break;
				#endregion

				#region LBUTTONDBLCLK
					// Disable this notification to remove the auto-check when
					// the user double-click on an item and append the expand / collapse function
				case Win32.WindowMessages.WM_LBUTTONDBLCLK:
					m_lastDoubleClick = DateTime.Now;
					if(FocusedItem != null)
					{
						item = FocusedItem;
						bool doExpColl = false;
						switch(ExpandMethod)
						{
							case TreeListViewExpandMethod.IconDbleClick:
								rec = item.GetBounds(ItemBoundsPortion.Icon);
								if(rec.Contains(PointToClient(MousePosition))) doExpColl = true;
								break;
							case TreeListViewExpandMethod.ItemOnlyDbleClick:
								rec = item.GetBounds(ItemBoundsPortion.ItemOnly);
								if(rec.Contains(PointToClient(MousePosition))) doExpColl = true;
								break;
							case TreeListViewExpandMethod.EntireItemDbleClick:
								rec = item.GetBounds(ItemBoundsPortion.Entire);
								if(rec.Contains(PointToClient(MousePosition))) doExpColl = true;
								break;
							default:
								break;
						}
						if(doExpColl)
						{
							m_doubleClickTime = DateTime.Now;
							Cursor = Cursors.WaitCursor;
							BeginUpdate();
							if(item.IsExpanded) item.Collapse();
							else item.Expand();
							EndUpdate();
							Cursor = Cursors.Default;
						}
					}
					OnDoubleClick(new EventArgs());
					return;
				#endregion

				#region MOUSEMOVE
				case Win32.WindowMessages.WM_MOUSEMOVE:
					if((MouseButtons & MouseButtons.Left) != MouseButtons.Left && HasMarquee)
						m_hasMarquee = false;
					break;
				#endregion

				#region UNICHAR, CHAR, KEYDOWN
				case Win32.WindowMessages.WM_UNICHAR:
				case Win32.WindowMessages.WM_CHAR:
					CharPressed((char) m.WParam);
					return;
				case Win32.WindowMessages.WM_KEYDOWN:
					OnKeyDown(new KeyEventArgs((Keys)(int) m.WParam));
					return;
				#endregion

				#region PAINT
				case Win32.WindowMessages.WM_PAINT:
					if(InEdit && EditedItem.Item != null)
					{

            Rectangle rectangle = EditedItem.Item.GetBounds(ItemBoundsPortion.Entire);

						Win32.RECT rect = new Win32.RECT();

            rect.left = rectangle.Left; 
            rect.top = rectangle.Top;
            rect.right = rectangle.Right; 
            rect.bottom = rectangle.Bottom;
						Win32.User32.ValidateRect(Handle, ref rect);
					}
					base.WndProc(ref m);
					DrawIntermediateStateItems();
					DrawSelectedItemsFocusCues();
					return;
				#endregion

				#region VSCROLL, HSCROLL, ENSUREVISIBLE
				case Win32.WindowMessages.WM_VSCROLL:
				case Win32.WindowMessages.WM_HSCROLL:
				case (Win32.WindowMessages)Win32.ListViewMessages.LVM_ENSUREVISIBLE:
					if(!Scrollable)
					{
						m.Result = (IntPtr)0;
						return;
					}
					break;
				#endregion

			} // END ... switch on message

			base.WndProc(ref m);
		} // WndProc(ref System.Windows.Forms.Message m)

		#region KeyFunction
			#region OnKeyDown
			/// <summary>
			/// Raises the KeyDown event
			/// </summary>
			/// <param name="e"></param>
			protected override void OnKeyDown(KeyEventArgs e)
			{
				Keys key = e.KeyCode;
				if(FocusedItem == null)
				{
					if(base.Items.Count > 0 &&
						(key == Keys.Down || key == Keys.Up || key == Keys.Left || key == Keys.Right))
					{
						base.Items[0].Selected = true;
						base.Items[0].Focused = true;
						base.Items[0].EnsureVisible();
					}
					base.OnKeyDown(e);
					return;
				}
				TreeListViewItem item = FocusedItem;
				switch(key)
				{
					case Keys.Down:
						if(item.NextVisibleItem != null)
						{
							TreeListViewItem nextitem = item.NextVisibleItem;
							if((Control.ModifierKeys & Keys.Shift) == Keys.Shift &&
								MultiSelect)
							{
								if(item.Parent != nextitem.Parent && item.Selected)
								{
									while((nextitem = nextitem.NextVisibleItem) != null)
										if(nextitem.Parent == item.Parent)
											break;
								}
								if(nextitem != null)
									SetSelectedItemsRange(m_selectionMark, nextitem);
								else
									nextitem = item.NextVisibleItem;
							}
							else if((Control.ModifierKeys & Keys.Control) != Keys.Control)
							{
								SetSelectedItemsRange(nextitem, nextitem);
								m_selectionMark = nextitem;
							}
							nextitem.Focused = true;
							nextitem.EnsureVisible();
						}
						break;
					case Keys.Up:
						if(item.PrevVisibleItem != null)
						{
							TreeListViewItem previtem = item.PrevVisibleItem;
							if((Control.ModifierKeys & Keys.Shift) == Keys.Shift &&
								MultiSelect)
							{
								if(item.Parent != previtem.Parent && item.Selected)
								{
									while((previtem = previtem.PrevVisibleItem) != null)
										if(previtem.Parent == item.Parent)
											break;
								}
								if(previtem != null)
									SetSelectedItemsRange(m_selectionMark, previtem);
								else
									previtem = item.PrevVisibleItem;
							}
							else if((Control.ModifierKeys & Keys.Control) != Keys.Control)
							{
								SetSelectedItemsRange(previtem, previtem);
								m_selectionMark = previtem;
							}
							previtem.Focused = true;
							previtem.EnsureVisible();
						}
						break;
					case Keys.Enter:
						base.SelectedItems.Clear();
						if(item.IsExpanded) item.Collapse();
						else item.Expand();
						item.Selected = true;
						item.EnsureVisible();
						break;
					case Keys.Left:
						if(item.IsExpanded)
						{
							base.SelectedItems.Clear();
							item.Selected = true;
							item.Collapse();
							item.EnsureVisible();
						}
						else if(item.Parent != null)
						{
							base.SelectedItems.Clear();
							item.Parent.Selected = true;
							item.Parent.Focused = true;
							item.Parent.EnsureVisible();
						}
						break;
					case Keys.Right:
						if(item.Items.Count == 0) break;
						if(!item.IsExpanded)
						{
							base.SelectedItems.Clear();
							item.Selected = true;
							item.Expand();
							item.EnsureVisible();
						}
						else
						{
							base.SelectedItems.Clear();
							item.Items[item.Items.Count-1].Selected = true;
							item.Items[item.Items.Count-1].Focused = true;
							item.Items[item.Items.Count-1].EnsureVisible();
						}
						break;
					case Keys.Space:
						if(base.CheckBoxes) item.Checked = !item.Checked;
						break;
				}
				base.OnKeyDown(e);
			}
			#endregion
			#region CharPressed
			private void CharPressed(char character)
			{
				Debug.Assert(!InvokeRequired);
				string begin = character.ToString().ToUpper();
				if(FocusedItem == null) return;
				TreeListViewItem item = FocusedItem;
				base.SelectedItems.Clear();
				item.Selected = true;
				// Select an item begining with the specified character
				if((begin.CompareTo("A") >= 0 && begin.CompareTo("Z") <= 0) || begin == " ")
				{
					// Get the collection in wich the item is
					TreeListViewItemCollection collection = item.Parent == null ? this.Items : item.Parent.Items;
					bool founded = false;
					// Search in the next items
					for(int i = collection.GetIndexOf(item) + 1 ; i < collection.Count ; i++)
						if(collection[i].Text.ToUpper().StartsWith(begin))
						{
							collection[i].Selected = true;
							collection[i].Focused = true;
							collection[i].EnsureVisible();
							founded = true;
							break;
						}
					// Search in the previous items
					if(!founded)
						for(int i = 0 ; i < collection.GetIndexOf(item) ; i++)
							if(collection[i].Text.ToUpper().StartsWith(begin))
							{
								collection[i].Selected = true;
								collection[i].Focused = true;
								collection[i].EnsureVisible();
								founded = true;
								break;
							}
				}
			}
			#endregion
		#endregion

		#endregion




    #region drawing

			#region CustomDraw

			private void CustomDraw(ref Message m)
			{

				int iRow, iCol; 
        bool bSelected;

				unsafe
				{

					Win32.NMLVCUSTOMDRAW * nmlvcd = (Win32.NMLVCUSTOMDRAW *)m.LParam.ToPointer();

					switch((Win32.CustomDrawDrawStateFlags)nmlvcd->nmcd.dwDrawStage)
					{
						case Win32.CustomDrawDrawStateFlags.CDDS_PREPAINT:
							m.Result = (IntPtr)Win32.CustomDrawReturnFlags.CDRF_NOTIFYITEMDRAW;
							break;
						case Win32.CustomDrawDrawStateFlags.CDDS_ITEMPREPAINT:
							m.Result = (IntPtr)Win32.CustomDrawReturnFlags.CDRF_NOTIFYSUBITEMDRAW;
							break;
            case Win32.CustomDrawDrawStateFlags.CDDS_ITEMPREPAINT |	Win32.CustomDrawDrawStateFlags.CDDS_SUBITEM:
            {
              iRow = (int)nmlvcd->nmcd.dwItemSpec;
              iCol = (int)nmlvcd->iSubItem;
              bSelected = base.Items[iRow].Selected;// && this.Focused;

              TreeListViewItem item = GetTreeListViewItemFromIndex(iRow);

              if(bSelected && m_useXPHighLightStyle)
              {

                Color color = Focused ? Win32.ColorUtil.VSNetSelectionColor : Win32.ColorUtil.VSNetSelectionUnfocusedColor;

                if(HideSelection && !Focused) 
                {
                  color = BackColor;
                }

                if(FullRowSelect || iCol == 0)
                {
                  nmlvcd->clrTextBk = (int)Win32.ColorUtil.RGB(color.R, color.G, color.B);
                }

                nmlvcd->nmcd.uItemState &= ~(int)Win32.CustomDrawItemStateFlags.CDIS_SELECTED;

                if( iCol == 0 ) 
                {
                  item.DrawFocusCues();
                }

              }

              if ( iCol == 0 )
              {
                item.DrawIntermediateState();
                item.DrawPlusMinusLines();
                item.DrawPlusMinus();
              }

              m.Result = (IntPtr)Win32.CustomDrawReturnFlags.CDRF_NEWFONT;
            }

            break;

					} // END ... switch

				} // END ... unsafe

			}

			#endregion

			#region Draw Items Parts

			internal void DrawIntermediateStateItems()
			{

				Debug.Assert(!InvokeRequired);

				if(CheckBoxes != CheckBoxTypes.Recursive) return;

				if(m_updating) return;

				TreeListViewItemCollection items = GetVisibleItems();

				Graphics g = Graphics.FromHwnd(Handle);

        foreach(TreeListViewItem item in items)
        {
          item.DrawIntermediateState(g);
        }

				g.Dispose();

			}

			internal void DrawSelectedItemsFocusCues()
			{

				if( m_updating ) return;

				if( (HideSelection && !Focused) || !m_useXPHighLightStyle ) return;

				Debug.Assert(!InvokeRequired);

				SelectedTreeListViewItemCollection items = SelectedItems;

        if(FocusedItem != null && Focused)
        {
          FocusedItem.DrawFocusCues();
        }

        foreach(TreeListViewItem temp in items)
        {
          temp.DrawFocusCues();
        }
			}

			internal void DrawPlusMinusItems()
			{

				if( m_updating ) return;

				Graphics g = Graphics.FromHwnd(Handle);

				TreeListViewItemCollection items = GetVisibleItems();

        foreach(TreeListViewItem item in items)
        {
          item.DrawPlusMinus(g);
        }

				g.Dispose();

			}

			internal void DrawPlusMinusItemsLines()
			{

				if( m_updating ) return;

				Graphics g = Graphics.FromHwnd(Handle);

				TreeListViewItemCollection items = GetVisibleItems();

        foreach(TreeListViewItem item in items)
        {
          item.DrawPlusMinusLines(g);
        }

				g.Dispose();
			}

			#endregion

		#endregion

    #region column order methods

    /// <summary>
    /// Get the index of the specified column from its physical position
    /// </summary>
    /// <param name="columnorder"></param>
    /// <returns></returns>
    public int GetColumnIndex(int columnorder)
    {
      if(columnorder < 0 || columnorder > Columns.Count - 1) return(-1);
      return Win32.User32.SendMessage(Handle, (int)Win32.HeaderControlMessages.HDM_ORDERTOINDEX, columnorder, 0);
    }
    /// <summary>
    /// Gets the order of a specified column
    /// </summary>
    /// <param name="columnindex"></param>
    /// <returns></returns>
    public int GetColumnOrder(int columnindex)
    {
      if(this.Columns.Count == 0) return(-1);
      if(columnindex < 0 || columnindex > this.Columns.Count - 1) return(-1);
      IntPtr[] colorderarray = new IntPtr[this.Columns.Count];
      Win32.User32.SendMessage(this.Handle, (int) Win32.ListViewMessages.LVM_GETCOLUMNORDERARRAY, (IntPtr) this.Columns.Count, ref colorderarray[0]);
      return((int) colorderarray[columnindex]);
    }
    /// <summary>
    /// Gets the columns order
    /// </summary>
    /// <returns>Example {3,1,4,2}</returns>
    public int[] GetColumnsOrder()
    {
      if(this.Columns.Count == 0) return(new int[] {});
      IntPtr[] colorderarray = new IntPtr[this.Columns.Count];
      try
      {
        Win32.User32.SendMessage(this.Handle, (int) Win32.ListViewMessages.LVM_GETCOLUMNORDERARRAY, (IntPtr) this.Columns.Count, ref colorderarray[0]);
      }
      catch{}
      int[] colorderarrayint = new int[this.Columns.Count];
      for(int i = 0 ; i < this.Columns.Count ; i ++)
        colorderarrayint[i] = (int) colorderarray[i];
      return(colorderarrayint);
    }
    /// <summary>
    /// Indicates the column order (for example : {0,1,3,2})
    /// </summary>
    /// <param name="colorderarray"></param>
    public void SetColumnsOrder(int[] colorderarray)
    {
      if(this.Columns.Count == 0) return;
      if(colorderarray.Length != this.Columns.Count) return;
      if(colorderarray[0] != 0) return;
      IntPtr[] colorderarrayintptr = new IntPtr[this.Columns.Count];
      for(int i = 0 ; i < this.Columns.Count ; i ++)
        colorderarrayintptr[i] = (IntPtr) colorderarray[i];
      try
      {
        Win32.User32.SendMessage(this.Handle, (int) Win32.ListViewMessages.LVM_SETCOLUMNORDERARRAY, (IntPtr) this.Columns.Count, ref colorderarrayintptr[0]);
      }
      catch{}
      Refresh();
    }

    private void _scroll()
    {
      while(MouseButtons == MouseButtons.Middle)
      {
        int dx = MousePosition.Y - m_mouseScrollPosition.Y;
        int dy = MousePosition.Y - m_mouseScrollPosition.Y;
        Scroll(
          dx,
          dy);
        Thread.Sleep(100);
      }
      Cursor = Cursors.Default;
    }
		
    /// <summary>
    /// Scrolls the control
    /// </summary>
    /// <param name="x"></param>
    /// <param name="y"></param>
    public void Scroll(int x, int y)
    {
      Win32.User32.SendMessage(Handle, (int) Win32.ListViewMessages.LVM_SCROLL, x, y);
    }
    /// <summary>
    /// Indicates the column order (for example : "3142")
    /// </summary>
    /// <param name="colorder"></param>
    public void SetColumnsOrder(string colorder)
    {
      if(colorder == null) return;
      int[] colorderarray = new int[colorder.Length];
      for(int i = 0 ; i < colorder.Length ; i++)
        colorderarray[i] = int.Parse(new String(colorder[i], 1));
      SetColumnsOrder(colorderarray);
    }
    #endregion

    #region item region methods

    /// <summary>
    /// Gets the items that are visible in the TreeListView
    /// </summary>
    /// <returns>A collection of items</returns>
    public TreeListViewItemCollection GetVisibleItems()
    {
      TreeListViewItemCollection visibleItems = new TreeListViewItemCollection();
      if(base.Items.Count == 0) return visibleItems;
      int firstItemIndex = TopItem.Index;
      int itemsPerPageCount = (int)Win32.User32.SendMessage(Handle, (int) Win32.ListViewMessages.LVM_GETITEMCOUNT, IntPtr.Zero, IntPtr.Zero);
      int lastVisibleItemIndex = firstItemIndex + itemsPerPageCount > base.Items.Count ?
        base.Items.Count : firstItemIndex + itemsPerPageCount;
      for(int i = firstItemIndex; i < lastVisibleItemIndex; i++)
        visibleItems.Add((TreeListViewItem) base.Items[i]);
      return visibleItems;
    }
    /// <summary>
    /// Gets the column at the specified position
    /// </summary>
    /// <param name="p">Point in client coordinates</param>
    /// <returns>The nul zero based index of the column (-1 if failed)</returns>
    public int GetColumnAt(Point p)
    {
      Win32.LVHITTESTINFO hittest = new Win32.LVHITTESTINFO();
      hittest.pt = new Win32.POINTAPI(PointToClient(MousePosition));
      Win32.User32.SendMessage(Handle, (Int32) Win32.ListViewMessages.LVM_SUBITEMHITTEST, 0 ,ref hittest);
      return(hittest.iSubItem);
    }
    /// <summary>
    /// Get SubItem rectangle
    /// </summary>
    /// <param name="item"></param>
    /// <param name="column"></param>
    /// <returns></returns>
    public Rectangle GetSubItemRect(TreeListViewItem item, int column)
    {
      ListViewItem lvitem = (ListViewItem) item;
      return GetSubItemRect(lvitem.Index, column);
    }
    /// <summary>
    /// Get SubItem rectangle
    /// </summary>
    /// <param name="row"></param>
    /// <param name="col"></param>
    /// <returns></returns>
    public Rectangle GetSubItemRect(int row, int col)
    {
      Win32.RECT rc = new Win32.RECT();
      rc.top = col;
      rc.left = (int)Win32.ListViewSubItemPortion.LVIR_BOUNDS;
      Win32.User32.SendMessage(Handle, (int)Win32.ListViewMessages.LVM_GETSUBITEMRECT,  row, ref rc);
			
      if ( col == 0 )
      {
        // The LVM_GETSUBITEMRECT message does not give us the rectangle for the first subitem
        // since it is not considered a subitem
        // obtain the rectangle for the header control and calculate from there
        Rectangle headerRect = GetHeaderItemRect(col);
        return new Rectangle((int)rc.left, (int)rc.top, (int)headerRect.Width, (int)(rc.bottom-rc.top));
      }
			
      return new Rectangle((int)rc.left, (int)rc.top, (int)(rc.right-rc.left), (int)(rc.bottom-rc.top));
    }
    /// <summary>
    /// Get HeaderItem text
    /// </summary>
    /// <param name="index"></param>
    /// <returns></returns>
    public string GetHeaderItemText(int index)
    {
      Win32.HDITEM hdi = new Win32.HDITEM();
      hdi.mask = Win32.HeaderItemFlags.HDI_TEXT;
      hdi.cchTextMax =  255;
      hdi.pszText = System.Runtime.InteropServices.Marshal.AllocHGlobal(255);
      Win32.User32.SendMessage(Handle, Win32.HeaderControlMessages.HDM_GETITEMW, index, ref hdi);
      string text = System.Runtime.InteropServices.Marshal.PtrToStringAuto(hdi.pszText);
      return text;
    }
    /// <summary>
    /// Get HeaderItem rect
    /// </summary>
    /// <param name="index"></param>
    /// <returns></returns>
    protected Rectangle GetHeaderItemRect(int index)
    {
      Win32.RECT rc = new Win32.RECT();
      IntPtr header = Win32.User32.GetDlgItem(Handle, 0);
      Win32.User32.SendMessage(header, (int)Win32.HeaderControlMessages.HDM_GETITEMRECT, index, ref rc);
      return new Rectangle((int)rc.left, (int)rc.top, (int)(rc.right-rc.left), (int)(rc.bottom-rc.top));
    }
    /// <summary>
    /// Get row rect
    /// </summary>
    /// <param name="row"></param>
    /// <returns></returns>
    public Rectangle GetRowRect(int row)
    {
      Win32.RECT rc = new Win32.RECT();
      rc.top = 0;
      rc.left = (int)Win32.ListViewSubItemPortion.LVIR_BOUNDS;
      Win32.User32.SendMessage(Handle, (int)Win32.ListViewMessages.LVM_GETSUBITEMRECT,  row, ref rc);
      return new Rectangle((int)rc.left, (int)rc.top, (int)(rc.right-rc.left), (int)(rc.bottom-rc.top));
    }
    #endregion




    #region event handlers

    /// <summary>
    /// Raises the MouseDown event
    /// </summary>
    /// <param name="e">A MouseEventArgs that contains the event data</param>
    protected override void OnMouseDown(MouseEventArgs e)
    {
      if( ! m_skipMouseDownEvent )
      {
        base.OnMouseDown(e);
      }
    } // OnMouseDown(MouseEventArgs e)

    #endregion


	} // class TreeListView

  #endregion TreeListView

  #region CheckBoxTypes Enumerator

  /// <summary>
  /// Check boxes types for TreeListView control
  /// </summary>
  [Serializable]
  public enum CheckBoxTypes
  {
    /// <summary>
    /// No CheckBoxes
    /// </summary>
    None,
    /// <summary>
    /// Simple CheckBoxes
    /// </summary>
    Simple,
    /// <summary>
    /// Check children recursively and set indeterminate state for the parents
    /// </summary>
    Recursive,
  }

  #endregion


	#region TreeListViewItemCheckDirections Enum

	/// <summary>
	/// Check boxes direction in the TreeListView
	/// </summary>
	[Flags]
	[Serializable]
	internal enum CheckDirections
	{
		/// <summary>
		/// Simply check the item
		/// </summary>
		None = 0,
		/// <summary>
		/// Set the indeterminate state to the parent items
		/// </summary>
		Upwards = 1,
		/// <summary>
		/// Check children items recursively
		/// </summary>
		Downwards = 2,
		/// <summary>
		/// Upwards + Downwards
		/// </summary>
		All = 3,
	}
	#endregion 

	#region EditItemInformation

	/// <summary>
	/// Class that contains all informations on an edited item
	/// </summary>
	public struct EditItemInformation
	{

    #region fields

    /// <summary>
    ///  Creation date 
    /// </summary>
    internal DateTime CreationTime;

    /// <summary>
    ///  Label for the edit box
    /// </summary>
    private string m_label;

    /// <summary>
    ///  Item beging edited
    /// </summary>
    private TreeListViewItem m_item;

    /// <summary>
    /// Column index of edited item
    /// </summary>
    private int m_columnIndex;

    #endregion

		#region properties

		/// <summary>
		/// Gets the label of the subitem
		/// </summary>
		public string Label
		{
			get { return m_label; }
		} // Label


		/// <summary>
		/// Gets the item being edited
		/// </summary>
		public TreeListViewItem Item
		{
			get { return m_item; }
		} // Item

		/// <summary>
		/// Gets the number of the subitem
		/// </summary>
		public int ColumnIndex
		{
			get{return m_columnIndex;}
		} // ColumnIndex

		#endregion

		#region constructor

		/// <summary>
		/// Creates a new instance of EditItemInformation
		/// </summary>
		/// <param name="item"></param>
		/// <param name="column"></param>
		/// <param name="label"></param>
		public EditItemInformation(TreeListViewItem item, int column, string label)
		{
			m_item = item; 
      m_columnIndex = column; 
      m_label = label; 
      CreationTime = DateTime.Now;
		} // EditItemInformation

		#endregion

	} // EditItemInformation

	#endregion  EditItemInformation


	#region TreeListViewAction
	/// <summary>
	/// TreeListView actions
	/// </summary>
	[Serializable]
	public enum TreeListViewAction
	{
		/// <summary>
		/// By Keyboard
		/// </summary>
		ByKeyboard,
		/// <summary>
		/// ByMouse
		/// </summary>
		ByMouse,
		/// <summary>
		/// Collapse
		/// </summary>
		Collapse,
		/// <summary>
		/// Expand
		/// </summary>
		Expand,
		/// <summary>
		/// Unknown
		/// </summary>
		Unknown
	}
	#endregion

	#region TreeListViewExpandMethod
	/// <summary>
	/// Expand / Collapse method
	/// </summary>
	[Serializable]
	public enum TreeListViewExpandMethod
	{
		/// <summary>
		/// Expand when double clicking on the icon
		/// </summary>
		IconDbleClick,
		/// <summary>
		/// Expand when double clicking on the entire item
		/// </summary>
		EntireItemDbleClick,
		/// <summary>
		/// Expand when double clicking on the item only
		/// </summary>
		ItemOnlyDbleClick,
		/// <summary>
		/// None
		/// </summary>
		None
	}
	#endregion

}

#region revision history

// $Log: TreeListView.cs,v $
// Revision 1.3  2006/06/16 20:47:49  mkontak
// no message
//
// Revision 1.2  2005/09/19 11:43:58  mkontak
// Coding standard and minor changes
//
// Revision 1.1  2005/05/11 11:53:50  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/TreeListView/TreeListView.cs,v 1.3 2006/06/16 20:47:49 mkontak Exp $
// $Id: TreeListView.cs,v 1.3 2006/06/16 20:47:49 mkontak Exp $

#endregion
