// $Id: GroupListView.cs,v 1.1 2006/01/09 19:42:11 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.IO;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.Xml;
using System.Text;
using System.Runtime.InteropServices;

namespace Viatronix.UI
{
  /// <summary>
  /// A listview control which provides grouping functionality.
  /// </summary>
  public class GroupListView : System.Windows.Forms.ListView
  {

    #region fields

    /// <summary>
    /// Reference to a Viatronix.UI.GroupGroupListViewItemCollection object.
    /// </summary>
    private Viatronix.UI.GroupListView.GroupListViewItemCollection m_items = null;

    /// <summary>
    /// Reference to a collection of ListViewGroups.
    /// </summary>
    private Viatronix.UI.GroupListView.ListViewGroupCollection m_groups = null;

    /// <summary>
    /// enables\disables grouping
    /// </summary>
    private bool m_enableGroups = false;

    /// <summary>
    /// header font for the group
    /// </summary>
    private Font m_groupFont = null;

    #endregion

    #region construction
  
    /// <summary>
    /// Default constructor
    /// </summary>
    public GroupListView()
    {
      SetStyle( ControlStyles.DoubleBuffer, true );

      m_items = new Viatronix.UI.GroupListView.GroupListViewItemCollection( this );
      m_groups = new ListViewGroupCollection( this );
    } // GroupListView()

    #endregion

    #region properties

    /// <summary>
    /// Returns the listview item collection.
    /// </summary>
    public new Viatronix.UI.GroupListView.GroupListViewItemCollection Items
    {
      get { return m_items; }
    } // Items


    /// <summary>
    /// Retruns a collection of listview groups.
    /// </summary>
    public new ListViewGroupCollection Groups
    {
      get { return m_groups; }
    } // Groups


    /// <summary>
    /// Gets or sets if the listview will display groups
    /// </summary>
    public bool EnableGroups
    {
      get { return m_enableGroups; }
      set 
      {
        m_enableGroups = value;

        if( this.IsHandleCreated )
          Win32.User32.SendMessage( this.Handle, (int) Win32.LVM.LVM_ENABLEGROUPVIEW, ((m_enableGroups) ? 1 : 0), 0 );
      }
    } // EnableGroups


    /// <summary>
    /// Returns the font for the group headers.
    /// </summary>
    private Font GroupFont
    {
      get 
      {
        if( m_groupFont == null )
          m_groupFont = new Font( this.Font, FontStyle.Bold );
        
        return m_groupFont;
      }
    } // GroupFont

    #endregion

    #region methods


//    protected override void WndProc( ref Message m )
//    {
//      if( m.Msg == (int) Win32.Msgs.WM_NOTIFY )
//      {
//        Win32.NMLISTVIEW nmListView = (Win32.NMLISTVIEW) Marshal.PtrToStructure( m.LParam, typeof( Win32.NMLISTVIEW ));
//        if( nmListView.nmhdr.code == (int) Win32.WindowsNotifyConstants.NM_CUSTOMDRAW )
//        {
//          Win32.NMLVCUSTOMDRAW nmLV = (Win32.NMLVCUSTOMDRAW) m.GetLParam( typeof( Win32.NMLVCUSTOMDRAW ));
//          if( nmLV.dwItemType == (int) Win32.LVCDI.LVCDI_GROUP )
//          {
//            if( nmLV.nmcd.dwDrawStage == (int) Win32.CDDS.CDDS_PREPAINT )
//              m.Result = new IntPtr( (int) Win32.CDRF.CDRF_NOTIFYITEMDRAW );
//
//            if( nmLV.nmcd.dwDrawStage == (int) Win32.CDDS.CDDS_ITEMPREPAINT )
//            {
//              Win32.Gdi32.SelectObject( nmLV.nmcd.hdc, this.GroupFont.ToHfont() );
//              m.Result = new IntPtr( (int) Win32.CDRF.CDRF_NEWFONT );
//            }
//          }
//          else
//            m.Result = new IntPtr( (int) Win32.CDRF.CDRF_DODEFAULT );
//        }
//      }
//
//      base.WndProc( ref m );
//    }


    /// <summary>
    /// Raises the HandleCreated event. Also inserts items and groups.
    /// </summary>
    /// <param name="args">An EventArgs that contains the event data.</param>
    protected override void OnHandleCreated( EventArgs args )
    {
      base.OnHandleCreated( args );

      if( this.EnableGroups )
      {
        Win32.User32.SendMessage( this.Handle, (int) Win32.LVM.LVM_ENABLEGROUPVIEW, 1, 0 );
        foreach( ListViewGroup group in this.Groups )
          InsertGroup( group );
      }
    } // OnHandleCreated( args )


    /// <summary>
    /// Inserts the managed group into the listview control.
    /// </summary>
    /// <param name="group">Managed ListViewGroup.</param>
    internal void InsertGroup( ListViewGroup group )
    {
      Win32.LVGROUP lvGroup = group.CreateLVGROUP();
      int result = (int) Win32.User32.SendMessage( this.Handle, (int) Win32.LVM.LVM_INSERTGROUP, -1, ref lvGroup );
      if( result == -1 )
        throw new InvalidOperationException( "Unable to insert group into listview. [HRESULT=" + result.ToString() + "]" );

      group.Index = result;
      group.SetListView( this );
    } // InsertGroup( group )


    /// <summary>
    /// Removes the managed group from the listview control.
    /// </summary>
    /// <param name="group">Managed ListViewGroup.</param>
    internal void RemoveGroup( ListViewGroup group )
    {
      int result = (int) Win32.User32.SendMessage( this.Handle, (int) Win32.LVM.LVM_REMOVEGROUP, group.GroupID, null );
      if( result == -1 )
        throw new InvalidOperationException( "Unable to insert group into listview. [HRESULT=" + result.ToString() + "]" );

      foreach( Viatronix.UI.GroupListViewItem item in group.Items )
        this.Items.Remove( item );
    } // RemoveGroup( group )


    /// <summary>
    /// Adds an item to a listview group.
    /// </summary>
    /// <param name="itemIndex">Item's index in the control.</param>
    /// <param name="groupID">Group identification.</param>
    internal void AddItemToGroup( int itemIndex, int groupID )
    {
      Win32.LVITEM lvItem = new Win32.LVITEM();
      lvItem.mask = (int) Win32.LVIF.LVIF_GROUPID;
      lvItem.iItem = itemIndex;
      lvItem.iGroupId = groupID;

      int result = (int) Win32.User32.SendMessage( this.Handle, (int) Win32.LVM.LVM_SETITEMW, 0, ref lvItem );
      if( result == 0 )
        throw new InvalidOperationException( "Unable to add item to group. [INDEX=" + itemIndex + "]" );
    } // AddItemToGroup( itemIndex, groupID )
                                                                                         
    #endregion

    #region ListViewGroupCollection class

    /// <summary>
    /// Collection of ListViewgroup objects.
    /// </summary>
    public class ListViewGroupCollection : CollectionBase
    {

      #region fields

      /// <summary>
      /// Reference to the parent ListView control.
      /// </summary>
      private Viatronix.UI.GroupListView m_listview = null;

      #endregion

      #region construction

      /// <summary>
      /// Creates a new instance of a ListViewGroupCollection.
      /// </summary>
      /// <param name="group">Parent ListView.</param>
      public ListViewGroupCollection( Viatronix.UI.GroupListView listview )
      {
        m_listview = listview;
      } // ListViewGroupCollection( listview )


      #endregion

      #region properties

      /// <summary>
      /// Returns the ListViewGroup at the specified index.
      /// </summary>
      public ListViewGroup this[ int index ]
      {
        get { return (ListViewGroup) this.List[ index ]; }
      } // Indexer


      /// <summary>
      /// Returns the ListViewGroup matching the text.
      /// </summary>
      public ListViewGroup this[ string text ]
      {
        get
        {
          string textVal = text.ToLower();
          foreach( ListViewGroup group in this.List )
          {
            if( group.Text.ToLower() == textVal )
              return group;
          }
          return null;
        }
      } // Indexer

      #endregion

      #region methods

      /// <summary>
      /// Adds the specified group to the collection.
      /// </summary>
      /// <param name="item">ListViewGroup</param>
      public void Add( ListViewGroup group )
      {
        this.List.Add( group );
      } // Add( group )


      /// <summary>
      /// Removes the ListViewGroup from the collection.
      /// </summary>
      /// <param name="item">ListViewGroup.</param>
      public void Remove( ListViewGroup group )
      {
        this.List.Remove( group );
      } // Remove( group )

      
      /// <summary>
      /// Validates the type of teh object being inserted.
      /// </summary>
      /// <param name="value">Object being inserted into collection.</param>
      protected override void OnValidate( object value )
      {
        if(( value is ListViewGroup ) == false )
          throw new ArgumentException( "The specified object is not of type Viatronix.UI.ListViewGroup" );
      } // OnValidate( object value )


      /// <summary>
      /// Provides custom functionality on insert operations.
      /// </summary>
      /// <param name="index">The index the object is inserted at.</param>
      /// <param name="value">The object being inserted.</param>
      protected override void OnInsert( int index, object value )
      {
        if( m_listview != null )
          m_listview.InsertGroup( (ListViewGroup) value );
      } // OnInsert( index, value )


      /// <summary>
      /// Provides custom functionality on insert operations.
      /// </summary>
      /// <param name="index">The index the object is inserted at.</param>
      /// <param name="value">The object being inserted.</param>
      protected override void OnRemove(int index, object value)
      {
        if( m_listview != null )
          m_listview.RemoveGroup( (ListViewGroup) value );
      } // OnInsert( index, value )


      #endregion

    } // class ListViewGroupCollection

    #endregion

    #region GroupListViewItemCollection class

    /// <summary>
    /// Extends the System.Windows.Forms.ListView.GroupListViewItemCollection with grouping functionality.
    /// </summary>
    public  class GroupListViewItemCollection : System.Windows.Forms.ListView.ListViewItemCollection
    {

      #region fields

      /// <summary>
      /// Reference to the parent ListView control.
      /// </summary>
      private Viatronix.UI.GroupListView m_listview = null;

      #endregion

      #region construction

      /// <summary>
      /// Creates a new instance of a GroupListViewItemCollection.
      /// </summary>
      /// <param name="listview">ListView control</param>
      public GroupListViewItemCollection( Viatronix.UI.GroupListView listview ) : base( listview )
      {
        m_listview = listview;
      } // GroupListViewItemCollection( listview )

      #endregion

      #region methods

      /// <summary>
      /// Adds the listview item to the collection.
      /// </summary>
      /// <param name="item">ListViewItem</param>
      /// <returns>ListViewItem</returns>
      public override System.Windows.Forms.ListViewItem Add( System.Windows.Forms.ListViewItem item )
      {
        System.Windows.Forms.ListViewItem addedItem = base.Add( item );

        if( m_listview.EnableGroups )
        {
          Viatronix.UI.GroupListViewItem v3dItem = addedItem as Viatronix.UI.GroupListViewItem;
          if( v3dItem != null && v3dItem.Group != null )
            m_listview.AddItemToGroup( v3dItem.Index, v3dItem.Group.GroupID );
        }

        return addedItem;
      } // Add( item )


      /// <summary>
      /// Inserts the listview item into the collection.
      /// </summary>
      /// <param name="displayIndex">The insert index.</param>
      /// <param name="item">ListViewItem</param>
      /// <returns>ListViewItem</returns>
      public new System.Windows.Forms.ListViewItem Insert( int displayIndex, System.Windows.Forms.ListViewItem item )
      {
        System.Windows.Forms.ListViewItem insertedItem = base.Insert( displayIndex, item );

        Viatronix.UI.GroupListViewItem v3dItem = insertedItem as Viatronix.UI.GroupListViewItem;
        if( v3dItem != null && v3dItem.Group != null )
          m_listview.AddItemToGroup( v3dItem.Index, v3dItem.Group.GroupID );

        return insertedItem;
      } // Insert( displayIndex, item )


      /// <summary>
      /// Clears the ListViewItems in the collection.
      /// </summary>
      public override void Clear()
      {
        base.Clear();

        foreach( ListViewGroup group in m_listview.Groups )
          group.Items.Clear();
      } // Clear()

      #endregion

    } // class GroupListViewItemCollection

    #endregion

  } // class ListView

  #region GroupListViewItem

  /// <summary>
  /// Extends the System.Windows.Forms.ListViewItem with grouping functionalty.
  /// </summary>
  public class GroupListViewItem : System.Windows.Forms.ListViewItem
  {
    
    #region fields

    /// <summary>
    /// Reference to a ListViewGroup.
    /// </summary>
    private ListViewGroup m_group = null;

    #endregion

    #region construction

    /// <summary>
    /// default constructor
    /// </summary>
    public GroupListViewItem()
    { 
    } // GroupListViewItem()


    /// <summary>
    /// Initializes a new instance of the ListViewItem class with specified item text.
    /// </summary>
    /// <param name="text">The text to display for the item.</param>
    public GroupListViewItem( string text ) : base( text )
    {
    } // GroupListViewItem( text )


    /// <summary>
    /// Initializes a new instance of the ListViewItem class with an array of strings representing subitems.
    /// </summary>
    /// <param name="items">An array of strings that represent the subitems of the new item.</param>
    public GroupListViewItem( string[] items ) : base( items )
    {
    } // GroupListViewItem( items )


    /// <summary>
    /// Initializes a new instance of the ListViewItem class with an array of ListViewItem.ListViewSubItem objects and the image index position of the item's icon.
    /// </summary>
    /// <param name="subItems">An array of ListViewItem.ListViewSubItem objects that represent the subitems of the item.</param>
    /// <param name="imageIndex">The zero-based index of the image within the ImageList associated with the ListView control that contains the item. </param>
    public GroupListViewItem( System.Windows.Forms.ListViewItem.ListViewSubItem[] subItems, int imageIndex ) : base( subItems, imageIndex )
    {
    } // GroupListViewItem( subItems, imageIndex )
    

    /// <summary>
    /// Initializes a new instance of the ListViewItem class with specified item text and the image index position of the item's icon.
    /// </summary>
    /// <param name="text">The text to display for the item.</param>
    /// <param name="imageIndex">The zero-based index of the image within the ImageList associated with the ListView control that contains the item.</param>
    public GroupListViewItem( string text, int imageIndex ) : base( text, imageIndex )
    { 
    } // GroupListViewItem( text, imageIndex )


    /// <summary>
    /// Initializes a new instance of the ListViewItem class with an array of strings representing subitems and the image index position of the item's icon.
    /// </summary>
    /// <param name="items">An array of strings that represent the subitems of the new item.</param>
    /// <param name="imageIndex">The zero-based index of the image within the ImageList associated with the ListView control that contains the item.</param>
    public GroupListViewItem( string[] items, int imageIndex ) : base( items, imageIndex )
    {
    } // GroupListViewItem( items, imageIndex )


    /// <summary>
    /// Initializes a new instance of the ListViewItem class with an array of strings representing subitems, the image index position of the item's icon, and the foreground color, background color, and font of the item.
    /// </summary>
    /// <param name="items">An array of strings that represent the subitems of the new item.</param>
    /// <param name="imageIndex">The zero-based index of the image within the ImageList associated with the ListView control that contains the item.</param>
    /// <param name="foreColor">A Color that represents the foreground color of the item.</param>
    /// <param name="backColor">A Color that represents the background color of the item.</param>
    /// <param name="font">A Font that represents the typeface to display the item's text in.</param>
    public GroupListViewItem( string[] items, int imageIndex, Color foreColor, Color backColor, Font font ) : base( items, imageIndex, foreColor, backColor, font )
    {
    } // GroupListViewItem( items, imageIndex )

    #endregion

    #region properties

    /// <summary>
    /// Returns the parent ListViewGroup.
    /// </summary>
    public new ListViewGroup Group
    {
      get { return m_group; }
    } // Group

    #endregion

    #region methods

    /// <summary>
    /// Sets the ListViewGroup.
    /// </summary>
    /// <param name="group">ListViewGroup object</param>
    internal void SetGroup( ListViewGroup group )
    {
      m_group = group;

      if( this.ListView != null && m_group.ListView == this.ListView )
        m_group.ListView.AddItemToGroup( this.Index, group.GroupID );
    } // SetGroup( group )

    #endregion

  } // class GroupListViewItem

  #endregion

  #region ListViewGroup

  /// <summary>
  /// Group information for a listview control.
  /// </summary>
  public class ListViewGroup
  {

    #region fields

    /// <summary>
    /// maintains a unique count for the group ids
    /// </summary>
    private static int NextGroupID = 0;

    /// <summary>
    /// Collection of ListViewItems added to this group.
    /// </summary>
    private ListViewGroupItemCollection m_items = null;

    /// <summary>
    /// Reference to a parent ListView control.
    /// </summary>
    private Viatronix.UI.GroupListView m_listview = null;

    /// <summary>
    /// Text displayed by the group.
    /// </summary>
    private string m_text = string.Empty;

    /// <summary>
    /// ListView Group Flags
    /// </summary>
    private Win32.LVGF m_flags = Win32.LVGF.LVGF_GROUPID;

    /// <summary>
    /// listView Group Alignment
    /// </summary>
    private Win32.LVGA m_align = Win32.LVGA.LVGA_HEADER_LEFT;

    /// <summary>
    /// the gourps id
    /// </summary>
    private int m_groupID = NextGroupID++;

    /// <summary>
    /// the groups index
    /// </summary>
    private int m_index = -1;

    #endregion

    #region construction

    /// <summary>
    /// Default constructor
    /// </summary>
    public ListViewGroup()
    {
      m_items = new ListViewGroupItemCollection( this );
    } // ListViewGroup()


    /// <summary>
    /// Creates a new instance of a ListViewGroup.
    /// </summary>
    /// <param name="text">Display text</param>
    public ListViewGroup( string text ) : this()
    {
      this.Text = text;
    } // ListViewGroup( text )

    #endregion

    #region properties

    /// <summary>
    /// Returns the parent ListView control.
    /// </summary>
    public Viatronix.UI.GroupListView ListView
    {
      get { return m_listview; }
    } // ListView


    /// <summary>
    /// Returns the ListViewItem collection.
    /// </summary>
    public ListViewGroupItemCollection Items
    {
      get { return m_items; }
    } // Items


    /// <summary>
    /// gets or sets the groups text.
    /// </summary>
    public string Text
    {
      get { return m_text; }
      set 
      { 
        m_text = value; 
        m_flags |= Win32.LVGF.LVGF_HEADER;      
      }
    } // Text


    /// <summary>
    /// Gets or sets the alignment.
    /// </summary>
    public HorizontalAlignment Align
    {
      get 
      { 
        switch( m_align )
        {
          case Win32.LVGA.LVGA_HEADER_LEFT:
            return HorizontalAlignment.Left;
          case Win32.LVGA.LVGA_HEADER_CENTER:
            return HorizontalAlignment.Center;
          case Win32.LVGA.LVGA_HEADER_RIGHT:
            return HorizontalAlignment.Right;
          default:
            return HorizontalAlignment.Left;
        }
      }
      set 
      { 
        switch( value )
        {
          case HorizontalAlignment.Left:
            m_align = Win32.LVGA.LVGA_HEADER_LEFT;
            break;
          case HorizontalAlignment.Center:
            m_align = Win32.LVGA.LVGA_HEADER_CENTER;
            break;
          case HorizontalAlignment.Right:
            m_align = Win32.LVGA.LVGA_HEADER_RIGHT;
            break;
        }
      
        m_flags |= Win32.LVGF.LVGF_ALIGN;
      }
    } // Align


    /// <summary>
    /// Return the group identification.
    /// </summary>
    internal int GroupID
    {
      get { return m_groupID; }
    } // GroupID


    /// <summary>
    /// Gets or sets the index of the group.
    /// </summary>
    internal int Index
    {
      get { return m_index; }
      set { m_index = value; }
    } // Index

    #endregion

    #region methods

    /// <summary>
    /// Sets the parent ListView control.
    /// </summary>
    /// <param name="listview">ListView control</param>
    internal void SetListView( Viatronix.UI.GroupListView listview )
    {
      m_listview = listview;

      foreach( Viatronix.UI.GroupListViewItem item in this.Items )
      {
        if( m_listview.Items.Contains( item ))
          m_listview.AddItemToGroup( item.Index, this.GroupID );
        else
          m_listview.Items.Add( item );
      }
    } // SetListView( listview )

    
    /// <summary>
    /// Creates an LVGROUP struct.
    /// </summary>
    /// <returns>LVGROUP struct</returns>
    internal Win32.LVGROUP CreateLVGROUP()
    {
      Win32.LVGROUP group = new Win32.LVGROUP();
      
      group.mask = (int) m_flags;  
      group.pszHeader = this.Text; 
      group.cchHeader = this.Text.Length; 
      group.uAlign = (int) m_align;
      group.iGroupId = this.GroupID;
      group.cbSize = Marshal.SizeOf( group ); 

      return group;
    } // CreateLVGROUP()


    #endregion

    #region ListViewGroupItemCollection class

    /// <summary>
    /// Collection of ListViewItems in a group.
    /// </summary>
    public class ListViewGroupItemCollection : CollectionBase
    {

      #region fields

      /// <summary>
      /// Reference to the parent ListViewGroup.
      /// </summary>
      private ListViewGroup m_parent = null;

      #endregion

      #region construction

      /// <summary>
      /// Creates a new instance of a ListViewGroupItemCollection.
      /// </summary>
      /// <param name="group">Parent ListViewGroup.</param>
      public ListViewGroupItemCollection( ListViewGroup group )
      {
        m_parent = group;
      } // ListViewGroupItemCollection( group )


      #endregion

      #region methods

      /// <summary>
      /// Adds the specified item to the collection.
      /// </summary>
      /// <param name="item">ListViewItem</param>
      public void Add( ListViewItem item )
      {
        this.List.Add( item );
      } // Add( item )


      /// <summary>
      /// Removes the ListViewItem from the collection.
      /// </summary>
      /// <param name="item">ListViewItem.</param>
      public void Remove( ListViewItem item )
      {
        this.List.Remove( item );
      } // Remove( item )

      
      /// <summary>
      /// Validates the type of teh object being inserted.
      /// </summary>
      /// <param name="value">Object being inserted into collection.</param>
      protected override void OnValidate( object value )
      {
        if(( value is ListViewItem ) == false )
          throw new ArgumentException( "The specified object is not of type Viatronix.UI.GroupListViewItem" );
      } // OnValidate( object value )


      /// <summary>
      /// Provides custom functionality for insert operations.
      /// </summary>
      /// <param name="index">The index the object is inserted at.</param>
      /// <param name="value">The object being inserted.</param>
      protected override void OnInsert( int index, object value )
      {
        Viatronix.UI.GroupListViewItem item = (Viatronix.UI.GroupListViewItem) value;
        item.SetGroup( m_parent );

        if( m_parent.ListView != null )
          m_parent.ListView.Items.Add( item );
      } // OnInsert( index, value )


      /// <summary>
      /// Provides custom functionality for removal operations.
      /// </summary>
      /// <param name="index">The index the object is removed at.</param>
      /// <param name="value">The object being removed.</param>
      protected override void OnRemove( int index, object value )
      {
        if( m_parent.ListView != null )
          m_parent.ListView.Items.Remove( (Viatronix.UI.GroupListViewItem) value );
      } // OnRemove( index, value )


      /// <summary>
      /// Provides custom functionality for the clear operation.
      /// </summary>
      protected override void OnClear()
      {
        if( m_parent.ListView != null )
        {
          foreach( Viatronix.UI.GroupListViewItem item in this.List )
            m_parent.ListView.Items.Remove( item );
        }
      } // OnClear()

      #endregion

    } // class ListViewGroupItemCollection

    #endregion

  } // class ListViewGroup

  #endregion

} // namespace Viatronix.UI

#region revision history

// $Log: GroupListView.cs,v $
// Revision 1.1  2006/01/09 19:42:11  gdavidson
// Initial Revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/GroupListView.cs,v 1.1 2006/01/09 19:42:11 gdavidson Exp $
// $Id: GroupListView.cs,v 1.1 2006/01/09 19:42:11 gdavidson Exp $

#endregion
