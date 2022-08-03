// $Id: DynamicListView.cs,v 1.19 2006/02/16 21:47:51 gdavidson Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
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
using System.Reflection;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.Xml;
using System.Text;
using System.Runtime.InteropServices;

namespace Viatronix.UI
{
  /// <summary>
  /// A listview which can be configured using xml and maintains its state between executions.
  /// </summary>
  public class DynamicListView : GroupListView, IComparer
  {

    #region fields

    /// <summary>
    /// version of the listview's xml
    /// </summary>
    public const string Version = "1.0";

    /// <summary>
    /// collection of sorting images
    /// </summary>
    private static ImageList m_sortImages = null;

    /// <summary>
    /// Collection of defined columns for the listview.
    /// </summary>
    private ArrayList m_definedColumns = new ArrayList();

    /// <summary>
    /// Maintains a reference to an image column.
    /// </summary>
    private IDynamicImageColumnHeader m_imageHeader = null;

    /// <summary>
    /// Reference to the column which was sorted.
    /// </summary>
    private DynamicColumnHeader m_sortedColumn = null;

    /// <summary>
    /// determines if a "Show in Groups" menu item is displayed.
    /// </summary>
    private bool m_allowGroupToggle = false;

    /// <summary>
    /// determines if the selected it should be tracked
    /// </summary>
    private bool m_trackSelection = false;

    #endregion

    #region construction

    /// <summary>
    /// static constructor
    /// </summary>
    static DynamicListView()
    {
      m_sortImages = new ImageList();
      m_sortImages.ImageSize = new Size( 12, 12 );

      Assembly assembly = Assembly.GetExecutingAssembly();
      m_sortImages.Images.Add( Image.FromStream( assembly.GetManifestResourceStream( "Viatronix.UI.res.sort-up.png" )));
      m_sortImages.Images.Add( Image.FromStream( assembly.GetManifestResourceStream( "Viatronix.UI.res.sort-down.png" )));
    } // DynamicListView()


    /// <summary>
    /// Creates a new instance of a DynamicListView control.
    /// </summary>
    public DynamicListView()
    {
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.SupportsTransparentBackColor, true );
    } // DynamicListView()

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets if a menu item is displayed that alllows grouping to be activated/deactivated.
    /// </summary>
    public bool AllowGroupToggle
    {
      get { return m_allowGroupToggle; }
      set { m_allowGroupToggle = value; }
    } // AllowGroupToggle


    /// <summary>
    /// Gets or sets the SmallImageList
    /// </summary>
    /// <remarks>When the base class sets the SmallImageList the header's image list is also set to the new image list.
    /// This implementation resets the header's image list after the SmallImageLIst is set.</remarks>
    public new ImageList SmallImageList
    {
      get { return base.SmallImageList; }
      set
      {
        base.SmallImageList = value;

        // reset the header's image list
        uint handle = Win32.User32.SendMessage( this.Handle, (int) Win32.LVM.LVM_GETHEADER, (uint) 0, (uint) 0 );
        Win32.User32.SendMessage( new IntPtr( handle ), (int) Win32.HeaderControlMessages.HDM_SETIMAGELIST, 0, m_sortImages.Handle );
      }
    } // SmallImageList

    #endregion

    #region override methods

    /// <summary>
    /// Processes Windows messages.
    /// </summary>
    /// <param name="m">The Windows Message to process.</param>
    protected override void WndProc( ref Message m )
    {  
      // block context menu
      if( m.Msg == (int) Win32.Msgs.WM_CONTEXTMENU ) 
        return;

      // scroll the listview
      else if( m.Msg == (int) Win32.Msgs.WM_VSCROLL || m.Msg == (int) Win32.Msgs.WM_MOUSEWHEEL )
        m_trackSelection = false;
 
      // right click on the column header
      else if( m.Msg == (int) Win32.Msgs.WM_NOTIFY )
      {
        Win32.NMHDR nmhdr = (Win32.NMHDR) Marshal.PtrToStructure( m.LParam, typeof( Win32.NMHDR ));
        if( nmhdr.code == (int) Win32.WindowsNotifyConstants.NM_RCLICK )
        {
          // construct menuitems for each defined column
          ArrayList items = new ArrayList();
          foreach( DynamicColumnHeader column in m_definedColumns )
          {
            MenuItemEx item = new MenuItemEx( column.Text );
            item.Checked = ( this.Columns.Contains( column ));
            item.Data = column;
            item.Click += new EventHandler( this.OnMenuItemClickHandler );

            items.Add( item );
          }

          items.Sort( this );

          if( this.AllowGroupToggle )
          {
            MenuItemEx seperator = new MenuItemEx( "-" );
            items.Add( seperator );

            MenuItemEx grouping = new MenuItemEx( "Show in Groups" );
            grouping.Checked = this.EnableGroups;
            grouping.Click += new EventHandler( this.OnGroupingClickHandler );
            items.Add( grouping );
          }

          Viatronix.UI.ContextMenu menu = new Viatronix.UI.ContextMenu();
          menu.MenuItems.AddRange( (MenuItemEx[]) items.ToArray( typeof( MenuItemEx )));
          menu.Show( this, PointToClient( Control.MousePosition ));
        }
      }

      base.WndProc( ref m );

            // on right click display context menu
      if( m.Msg == (int) Win32.Msgs.WM_RBUTTONDOWN )
      {
        if( this.ContextMenu != null )
        {
          this.ContextMenu.Show( this, PointToClient( Control.MousePosition ));
          Application.DoEvents();
        }
      }
    } // WndProc( m )


    /// <summary>
    /// Sets windows Xp properties to eliminate flickering.
    /// </summary>
    /// <param name="args">An EventArgs that contains the event data.</param>
    protected override void OnHandleCreated( EventArgs args )
    {
      base.OnHandleCreated( args );

      Win32.LVS_EX styles = (Win32.LVS_EX) Win32.User32.SendMessage( this.Handle, (int) Win32.LVM.LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0 );
      styles |= Win32.LVS_EX.LVS_EX_DOUBLEBUFFER | Win32.LVS_EX.LVS_EX_BORDERSELECT;
      Win32.User32.SendMessage( this.Handle, (int) Win32.LVM.LVM_SETEXTENDEDLISTVIEWSTYLE, 0, (int) styles );

      uint handle = Win32.User32.SendMessage( this.Handle, (int) Win32.LVM.LVM_GETHEADER, (uint) 0, (uint) 0 );
      Win32.User32.SendMessage( new IntPtr( handle ), (int) Win32.HeaderControlMessages.HDM_SETIMAGELIST, 0, m_sortImages.Handle );
    } // OnHandleCreated( args )


    /// <summary>
    /// Handles the sorting when a column is clicked.
    /// </summary>
    /// <param name="args">An ColumnClickEventArgs that contains the event data.</param>
    protected override void OnColumnClick( ColumnClickEventArgs args )
    {
      base.OnColumnClick( args );

      if( this.EnableGroups == false )
      {
        if( m_sortedColumn != null )
          m_sortedColumn.ShowSortIcon( false );

        m_sortedColumn = (DynamicColumnHeader) this.Columns[ args.Column ];
        this.ListViewItemSorter = m_sortedColumn.CreateComparer();
        m_sortedColumn.ShowSortIcon( true );
      }
    } // OnColumnClick( args )


    /// <summary>
    /// Handles the selected index changed event.
    /// </summary>
    /// <param name="args">An EventArgs that contains the event data.</param>
    protected override void OnSelectedIndexChanged( EventArgs args )
    {
      m_trackSelection = ( this.SelectedItems.Count > 0 );
      base.OnSelectedIndexChanged( args );
    } // OnSelectedIndexChanged( args )


    #endregion

    #region methods

    /// <summary>
    /// Adds a ListViewItem for the provided object.
    /// </summary>
    /// <param name="obj">The object being displayed by the listview.</param>
    /// <returns>A ListViewItem representing the provided object.</returns>
    public GroupListViewItem AddItem( object obj )
    {
      return AddItem( obj, null );
    } // AddItem( obj )


    /// <summary>
    /// Adds a ListViewItem for the provided object.
    /// </summary>
    /// <param name="obj">The object being displayed by the listview.</param>
    /// <param name="group">Group containing the object.</param>
    /// <returns>A ListViewItem representing the provided object.</returns>
    public GroupListViewItem AddItem( object obj, ListViewGroup group )
    {
      GroupListViewItem item = new GroupListViewItem();
      UpdateItem( obj, item );      

      if( group != null )
        group.Items.Add( item );
      else
        this.Items.Add( item );

      return item;
    } // AddItem( obj, group )

    
    /// <summary>
    /// Binds the contents of the provided dataset to the listview.
    /// </summary>
    /// <param name="dataSet">Containing data that needs to be displayed.</param>
    /// <param name="tableName">The name of the table in the dataset.</param>
    public void SetDataBinding( DataSet dataSet, string tableName )
    {
      SetDataBinding( dataSet, tableName, new string[ 0 ]);
    } // SetDataBinding( dataSet, tableName )


    /// <summary>
    /// Binds the contents of the provided dataset to the listview.
    /// </summary>
    /// <param name="dataset">Containing data that needs to be displayed.</param>
    /// <param name="tableName">The name of the table in the dataset.</param>
    /// <param name="uniqueColumns">A colelction of uniqueColumns used to maintain selection.</param>
    public void SetDataBinding( DataSet dataset, string tableName, string[] uniqueColumns )
    {
      BeginUpdate();
      DataTable table = dataset.Tables[ tableName ].Copy();

      ArrayList primaryKeys = new ArrayList();
      foreach( string col in uniqueColumns )
        primaryKeys.Add( table.Columns[ col ]);

      ArrayList existingRows = new ArrayList();
      table.PrimaryKey = (DataColumn[]) primaryKeys.ToArray( typeof( DataColumn ));
      foreach( GroupListViewItem item in this.Items )
      {
        DataRow oldRow = (DataRow) item.Tag;
        ArrayList columnValues = new ArrayList();
        foreach( string col in uniqueColumns )
          columnValues.Add( oldRow[ col ]);

        if( table.Rows.Contains( columnValues.ToArray() ))
        {
          DataRow newRow = table.Rows.Find( columnValues.ToArray() );
          UpdateItem( newRow, item );
          existingRows.Add( newRow );
        }
        else
          item.Remove();
      }
      
      foreach( DataRow row in table.Rows )
      {
        if( existingRows.Contains( row ))
          continue;

        ListViewItem item = AddItem( row );

        //Display Image. 
        //precondition: The dataset should have IMAGE_INDEX column to display this
        if (  row.Table.Columns.Contains( "IMAGE_INDEX" )  && !row.IsNull("IMAGE_INDEX") && this.SmallImageList != null )
        {
          int imageIndex = System.Convert.ToInt32( row[ "IMAGE_INDEX" ]);

          if ( imageIndex < this.SmallImageList.Images.Count )
            item.ImageIndex = imageIndex;
        }    
      }

      this.Tag = table;

      if( m_trackSelection && this.SelectedItems.Count > 0 )
        this.SelectedItems[0].EnsureVisible();

      EndUpdate();
    } // SetDataBinding( dataSet, tableName, uniqueColumns )
    

    /// <summary>
    /// Loads the columns for the control from the specified configuration file.
    /// </summary>
    /// <param name="configuration">Path to the configuration file.</param>
    public void Load( string configuration )
    {
      XmlDocument doc = new XmlDocument();
      doc.Load( configuration );

      Load( doc.DocumentElement );
    } // Load( configuration )


    /// <summary>
    /// Loads the columns for the control from the specified configuration file.
    /// </summary>
    /// <param name="node">XmlNode containing configuration information.</param>
    public void Load( XmlNode node )
    {
      Load( node, true );
    } // Load( node )


    /// <summary>
    /// Loads the columns for the control from the configuration file and displays the columns listed in the userPreferences file.
    /// </summary>
    /// <param name="configuration">Path to the configuration file.</param>
    /// <param name="userPreferences">Path to the user preferences.</param>
    public void Load( string configuration, string userPreferences )
    {
      XmlDocument configDoc = new XmlDocument();
      configDoc.Load( configuration );

      try
      {
        XmlDocument userDoc = new XmlDocument();
        userDoc.Load( userPreferences );
        Load( configDoc.DocumentElement, userDoc.DocumentElement );
      }
      catch
      {
        Reset();
        Load( configDoc.DocumentElement );
        return;
      }      
    } // Load( configuration, userPreferences )


    /// <summary>
    /// Loads the columns for the control from the configuration file and displays the columns listed in the userPreferences file.
    /// </summary>
    /// <param name="configuration">Path to the configuration file.</param>
    /// <param name="node">XmlNode containing user preference's information..</param>
    public void Load( string configuration, XmlNode node )
    {
      XmlDocument configDoc = new XmlDocument();
      configDoc.Load( configuration );

      Load( configDoc.DocumentElement, node );
    } // Load( configuration, node )


    /// <summary>
    /// Loads the columns for the control from the configuration file and displays the columns listed in the userPreferences file.
    /// </summary>
    /// <param name="node">XmlNode containing configuration information.</param>
    /// <param name="userPreferences">Path to the user preferences.</param>
    public void Load( XmlNode node, string userPreferences )
    {
      try
      {
        XmlDocument userDoc = new XmlDocument();
        userDoc.Load( userPreferences );
        Load( node, userDoc.DocumentElement );
      }
      catch( Exception  )
      {
        Reset();
        Load( node );
        return;
      }
    } // Load( node, userPreferences )


    /// <summary>
    /// Loads the columns for the control from the configuration file and displays the columns listed in the userPreferences file.
    /// </summary>
    /// <param name="configDoc">XmlNode containing configuration information.</param>
    /// <param name="userDoc">XmlNode containing user preference's information.</param>
    public void Load( XmlNode configDoc, XmlNode userDoc )
    {
      Load( configDoc, false );

      string activeID = string.Empty;
      if( userDoc.Attributes[ "active" ] != null )
        activeID = userDoc.Attributes[ "active" ].Value;

      if( this.AllowGroupToggle )
      {
        if( userDoc.Attributes[ "allowGroups" ] != null )
          this.AllowGroupToggle = bool.Parse( userDoc.Attributes[ "allowGroups" ].Value );
  
        if( userDoc.Attributes[ "enableGroups" ] != null )
        {
          this.EnableGroups = bool.Parse( userDoc.Attributes[ "enableGroups" ].Value );
                this.HeaderStyle = ( this.EnableGroups ) ? ColumnHeaderStyle.Nonclickable : ColumnHeaderStyle.Clickable;
        }
      }

      // display the columns specified in the user preferences file
      foreach( XmlNode node in userDoc.SelectNodes( "column" ))
      {
        DynamicColumnHeader header = GetColumn( node.Attributes[ "id" ].Value );
        
        if( node.Attributes[ "width" ] != null )
          header.Width = Convert.ToInt32( node.Attributes[ "width" ].Value );

        if( node.Attributes[ "align" ] != null )
          header.TextAlign = (HorizontalAlignment) Enum.Parse( typeof( HorizontalAlignment ), node.Attributes[ "align" ].Value, true );

        if( node.Attributes[ "sortOrder" ] != null )
          header.SortOrder = (SortOrder) Enum.Parse( typeof( SortOrder ), node.Attributes[ "sortOrder" ].Value, true );
        
        this.Columns.Add( header );

        if( header.ID == activeID )
        {
          this.ListViewItemSorter = header.CreateComparer( false );      
          m_sortedColumn = header;
          m_sortedColumn.ShowSortIcon( !this.EnableGroups );
        }
      }
    } // Load( configuration, userPreferences )


    /// <summary>
    /// Loads the columns in the configuration file.
    /// </summary>
    /// <param name="doc">XmlNode containing configuration information.</param>
    /// <param name="insert">true inserts the default columns; otherwise false</param>
    private void Load( XmlNode doc, bool insert )
    {
      Type columnType = Type.GetType( doc.Attributes[ "type" ].Value );
      DynamicColumnHeader baseHeader = (DynamicColumnHeader) Activator.CreateInstance( columnType, true );

      foreach( XmlNode node in doc.SelectNodes( "column" ))
      {
        DynamicColumnHeader header = (DynamicColumnHeader) baseHeader.Clone();
        header.Initialize( node );

        m_definedColumns.Add( header );

        if( insert && header.Default )
          this.Columns.Add( header );
      }
    } // Load( configuration, insert )


    /// <summary>
    /// Generates a user preferences file at the specified location.
    /// </summary>
    /// <param name="filename">The path to the user preferences file.</param>
    public void Save( string filename )
    {
      XmlTextWriter writer = new XmlTextWriter( filename, Encoding.UTF8 );
      writer.WriteStartElement( "columns" );
      writer.WriteAttributeString( "version", Version );
      writer.WriteAttributeString( "allowGroups", this.AllowGroupToggle.ToString() );
      writer.WriteAttributeString( "enableGroups", this.EnableGroups.ToString() );

      if( m_sortedColumn != null )
        writer.WriteAttributeString( "active", m_sortedColumn.ID );

      foreach( int pos in GetColumnOrder() )
      {
        DynamicColumnHeader header = (DynamicColumnHeader) this.Columns[ pos ];
        writer.WriteStartElement( "column" );
        writer.WriteAttributeString( "id", header.ID );
        writer.WriteAttributeString( "width", header.Width.ToString() );
        writer.WriteAttributeString( "align", header.TextAlign.ToString() );
        writer.WriteAttributeString( "sortOrder", header.SortOrder.ToString() );
        writer.WriteEndElement();
      }

      writer.WriteEndElement();
      writer.Close();
    } // Save( filename )


    /// <summary>
    /// Updates the contents of a ListViewItem.
    /// </summary>
    /// <param name="obj">The object being displayed by the listview.</param>
    /// <param name="item">The listview item</param>
    private void UpdateItem( object obj, GroupListViewItem item )
    {
      item.SubItems.Clear();

      bool isFirstColumn = true;
      foreach( DynamicColumnHeader header in this.Columns )
      {
        object value = header.GetValue( obj );
        string text = header.ConvertToString( value );
        //        if( m_imageHeader != null && m_imageHeader == header )
        //        {
        //          item.Text = string.Empty;
        //          item.ImageIndex = m_imageHeader.GetIndex( obj );
        //          isFirstColumn = false;
        //        }
        if( m_imageHeader == null && isFirstColumn )
        {
          DynamicListViewSubItem subItem = new DynamicListViewSubItem( item, text );
          subItem.Tag = value;
          item.SubItems[0] = subItem;
          //          item.Text = text;
          //          item.Tag = value;
          isFirstColumn = false;
        }
        else
        {
          DynamicListViewSubItem subItem = new DynamicListViewSubItem( item, text );
          subItem.Tag = value;
          item.SubItems.Add( subItem );
        }
      }

      item.Tag = obj;
    } // UpdateItem( obj, item )


    /// <summary>
    /// Updates the contents of the loaded ListViewItems.
    /// </summary>
    private void UpdateItems()
    {
      BeginUpdate();

      foreach( GroupListViewItem item in this.Items )
        UpdateItem( item.Tag, item );

      EndUpdate();
    } // UpdateItems()


    /// <summary>
    /// Returns the column order.
    /// </summary>
    /// <returns>Array of ordered columns.</returns>
    private int[] GetColumnOrder()
    {
      int count = this.Columns.Count;
      int[] order = new int[ count ];
      Win32.User32.SendMessage( this.Handle, (int) Win32.LVM.LVM_GETCOLUMNORDERARRAY, count, order );
      return order;
    } // GetColumnOrder()


    /// <summary>
    /// Retrieves the column based on id.
    /// </summary>
    /// <param name="id">The id of the column.</param>
    /// <returns>The column with the specified id.</returns>
    private DynamicColumnHeader GetColumn( string id )
    {
      foreach( DynamicColumnHeader column in m_definedColumns )
      {
        if( column.ID == id )
          return column;
      }

      return null;
    } // GetColumn( id )


    /// <summary>
    /// Determines if the arraylist contains the provided DataRow.
    /// </summary>
    /// <param name="oldRows">Collection of old rows.</param>
    /// <param name="newRow">New DataRow.</param>
    /// <param name="columns">Columns being compared.</param>
    /// <returns>true if the DataRow is in the old collection; otherwise false</returns>
    private bool CompareValues( ArrayList oldRows, DataRow newRow, string[] columns )
    {
      foreach( DataRow table in oldRows )
      {
        if( CompareValues( table, newRow, columns ))
          return true;
      }

      return false;
    } // CompareValues( oldRows, newRow, columns )


    /// <summary>
    /// Compares the provided DataRows based on the specified columns.
    /// </summary>
    /// <param name="oldRow">The old DataRow.</param>
    /// <param name="newRow">The new DataRow.</param>
    /// <param name="columns">Columns being compared.</param>
    /// <returns>true if the columns are the same; otherise false</returns>
    private bool CompareValues( DataRow oldRow, DataRow newRow, string[] columns )
    {
      try
      {
        foreach( string column in columns )
        {
          if( !oldRow[ column ].Equals( newRow[ column ] ))
            return false;
        }

        return true;
      }
      catch( Exception )
      {
        return false;
      }
    } // CompareValues( oldRow, newRow, columns )

    
    /// <summary>
    /// Resets the collections in the listview.
    /// </summary>
    private void Reset()
    {
      m_definedColumns.Clear();
      Clear();
    } // Reset()

    #endregion

    #region event handlers

    /// <summary>
    /// Inserts or removes the column selected from the context menu.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnMenuItemClickHandler( object sender, EventArgs args )
    {
      MenuItemEx item = (MenuItemEx) sender;
      if( item.Checked )
        this.Columns.Remove( (DynamicColumnHeader) item.Data );
      else
        this.Columns.Add( (DynamicColumnHeader) item.Data );

      UpdateItems();
    } // OnMenuItemClickHandler( sender, args )


    /// <summary>
    /// Handles the grouping context menu item.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="args">An EventArgs that contains the event data.</param>
    private void OnGroupingClickHandler( object sender, EventArgs args )
    {
      MenuItemEx item = (MenuItemEx) sender;
      this.EnableGroups = !item.Checked;
      this.HeaderStyle = ( this.EnableGroups ) ? ColumnHeaderStyle.Nonclickable : ColumnHeaderStyle.Clickable;

      if( m_sortedColumn != null )
        m_sortedColumn.ShowSortIcon( !this.EnableGroups );      

      UpdateItems();
    } // OnGroupingClickHandler( sender, args )

    #endregion

    #region IComparer Members

    /// <summary>
    /// Compares two objects and returns a value indicating whether one is less than, equal to or greater than the other.
    /// </summary>
    /// <param name="x">First object to compare.</param>
    /// <param name="y">Second object to compare. </param>
    /// <returns>value indicating whether one is less than, equal to or greater than the other</returns>
    public int Compare( object x, object y )
    {
      MenuItemEx item1 = (MenuItemEx) x;
      MenuItemEx item2 = (MenuItemEx) y;

      return item1.Text.CompareTo( item2.Text );
    } // Compare( x, y )

    #endregion

  } // class DynamicListView

  #region DynamicListViewSubItem class

  public class DynamicListViewSubItem : ListViewItem.ListViewSubItem
  {
    
    #region fields

    private object m_tag = null;

    #endregion

    #region construction

    public DynamicListViewSubItem( ListViewItem item, string text ) : base( item, text )
    {
    }

    #endregion

    #region properties

    public new object Tag
    {
      get { return m_tag; }
      set { m_tag = value; }
    } // Tag

    #endregion

  }

  #endregion

} // namespace Viatronix.UI

#region revision history

// $Log: DynamicListView.cs,v $
// Revision 1.19  2006/02/16 21:47:51  gdavidson
// odified the SetDataBindings method to update all items
//
// Revision 1.18  2006/02/16 16:44:19  gdavidson
// Added an implementaion for the SmallImageList property to handle the bug that causes the header's image list to be set to the image list provided to the listview items.
//
// Revision 1.17  2006/02/15 20:00:55  gdavidson
// Modified the SetDataBindings method
//
// Revision 1.16  2006/02/06 20:17:42  gdavidson
// Maintains sorted column when switch between showing groups.
//
// Revision 1.15  2006/02/06 20:07:03  gdavidson
// Added an icon indicating the sort ordering
//
// Revision 1.14  2006/01/30 16:06:18  romy
// fix for Image_index column check
//
// Revision 1.13  2006/01/27 20:55:29  romy
// added Image colum for list view- a temp implementation
//
// Revision 1.12  2006/01/09 19:42:35  gdavidson
// Extended GroupListView
//
// Revision 1.11  2005/10/26 19:56:32  gdavidson
// Added code to maintain the sorted column.
//
// Revision 1.10  2005/10/18 17:54:53  gdavidson
// Added sorting of columns.
//
// Revision 1.9  2005/08/31 16:40:49  gdavidson
// Allowed row seletion before displaying context menu.
//
// Revision 1.8  2005/08/31 14:39:51  gdavidson
// Blocked listview context menu when right click on header.
//
// Revision 1.7  2005/06/22 15:05:18  gdavidson
// Added a Reset method.
//
// Revision 1.6  2005/06/10 19:36:56  gdavidson
// Added code to maintain the selection between Databindings.
//
// Revision 1.5  2005/06/07 15:11:16  romy
// Added the Win XP styles to the List View - Solves the flickering.
//
// Revision 1.4  2005/06/03 19:58:27  gdavidson
// Modified the SetDataBinding method to maintain selection.
//
// Revision 1.3  2005/06/03 14:46:57  gdavidson
// Added functionality for a dataset.
//
// Revision 1.2  2005/05/26 19:34:40  gdavidson
// Added overloads to the Load method.
//
// Revision 1.1  2005/05/26 17:27:18  gdavidson
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/DynamicListView.cs,v 1.19 2006/02/16 21:47:51 gdavidson Exp $
// $Id: DynamicListView.cs,v 1.19 2006/02/16 21:47:51 gdavidson Exp $

#endregion
