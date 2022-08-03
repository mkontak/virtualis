// $Id: FileSystemListView.cs,v 1.4 2005/05/26 17:18:31 gdavidson Exp $
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
using System.Collections.Specialized;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.Globalization;
using System.Text.RegularExpressions;


namespace Viatronix.UI
{

	/// <summary>
	/// Summary description for FileSystemListView.
	/// </summary>
	public class FileSystemListView : ListView
	{

    #region fields

    private string m_rootPath = string.Empty;
    private DirectoryInfo m_root = null;
    private DirectoryInfo m_current = null;

    private Color m_columnColor = SystemColors.Control;

    private ColumnHeader m_nameColumn = null;
    private ColumnHeader m_sizeColumn = null;
    private ColumnHeader m_typeColumn = null;
    private ColumnHeader m_dateColumn = null;

    private bool m_parseFileSystem = false;

    private StringCollection m_exclusions = new StringCollection(); 

    private bool m_showFiles = true;

    private bool m_showDirectories = true;

    private bool m_showParentFolder = true;

    private bool m_displayName = true;
    private bool m_displaySize = true;
    private bool m_displayType = true;
    private bool m_displayDate = true;

    private const string DateFormat = "M/d/yyyy h:mm tt";

    #endregion

    #region construction

		public FileSystemListView()
		{
      SetStyle( ControlStyles.AllPaintingInWmPaint, true );
//      SetStyle( ControlStyles.UserPaint, true );
      SetStyle( ControlStyles.DoubleBuffer, true );
      SetStyle( ControlStyles.SupportsTransparentBackColor, true );

      this.SuspendLayout();

      this.SmallImageList = SystemImageManager.Instance.SmallImageList;
      this.LargeImageList = SystemImageManager.Instance.LargeImageList;

      GenerateDefaultColumns();
      
      this.ResumeLayout( true );
		}

    #endregion

    #region properties

    public bool ParseFileSystem
    {
      get { return m_parseFileSystem; }
      set 
      { 
        m_parseFileSystem = value; 
        if( m_parseFileSystem )
          Reset();
      }
    }


    public DirectoryInfo[] SelectedDirectories
    {
      get
      {
        ArrayList list = new ArrayList();
        foreach( ListViewItem item in this.SelectedItems )
        {
          if( item.Tag is DirectoryInfo )
            list.Add( item.Tag );
        }

        return (DirectoryInfo[])list.ToArray( typeof( DirectoryInfo ));
      }
    }


    public FileInfo[] SelectedFiles
    {
      get
      {
        ArrayList list = new ArrayList();
        foreach( ListViewItem item in this.SelectedItems )
        {
          if( item.Tag is FileInfo )
            list.Add( item.Tag );
        }

        return (FileInfo[])list.ToArray( typeof( FileInfo ));
      }
    }


    public FileSystemInfo[] SelectedFilesAndDirectories
    {
      get
      {
        ArrayList list = new ArrayList();
        foreach( ListViewItem item in this.SelectedItems )
        {
          if( item.Tag is FileSystemInfo )
            list.Add( item.Tag );
        }

        return (FileSystemInfo[])list.ToArray( typeof( FileSystemInfo ));
      }
    }


    public Color CustomColumnColor
    {
      get { return m_columnColor; }
      set { m_columnColor = value; }
    }


    public StringCollection Exclusions
    {
      get { return m_exclusions; }
    }


    public string RootDirectory
    {
      get { return m_rootPath; }
      set 
      {
        bool exists = Directory.Exists( value );
        m_rootPath = ( exists ) ? value : string.Empty; 
        m_root = ( exists ) ? new DirectoryInfo( m_rootPath ) : null;
        m_current = m_root;
        if( this.ParseFileSystem )
          Reset();
      }
    }


    public bool ShowFiles
    {
      get { return m_showFiles; }
      set { m_showFiles = value; }
    }


    public bool ShowDirectories
    {
      get { return m_showDirectories; }
      set { m_showDirectories = value; }
    }


    public bool DisplayName
    {
      get { return m_displayName; }
      set { m_displayName = value; }
    }


    public bool DisplaySize
    {
      get { return m_displaySize; }
      set { m_displaySize = value; }
    }


    public bool DisplayType
    {
      get { return m_displayType; }
      set { m_displayType = value; }
    }


    public bool DisplayDate
    {
      get { return m_displayDate; }
      set { m_displayDate = value; }
    }


    private DirectoryInfo Current
    {
      get { return m_current; }
    }


    private DirectoryInfo Root
    {
      get { return m_root; }
    }

    #endregion

    #region override methods

    protected override void OnResize( EventArgs args )
    {
      base.OnResize( args );

      Invalidate();
    }


    protected override void OnPaint( PaintEventArgs args )
    {
      base.OnPaint( args );

//      Graphics gfx = args.Graphics;
//
//      int y = 20;
//      foreach( ListViewItem item in this.Items )
//      {
//        int x = 0;
//        foreach( int col in GetColumnOrder() )
//        {                
//          ColumnHeader header = this.Columns[ col ];
//
//          System.Windows.Forms.ListViewItem.ListViewSubItem subItem = item.SubItems[ col ];
//
//          SizeF stringMeasure = gfx.MeasureString( subItem.Text, subItem.Font, header.Width, StringFormat.GenericDefault );
//          using( Brush brush = new SolidBrush( subItem.ForeColor ))
//            gfx.DrawString( subItem.Text, subItem.Font, brush, new RectangleF( x, y, stringMeasure.Width, this.FontHeight ), GetStringFormat( header ));
//            
//          x += header.Width;
//        }
//
//        y += this.FontHeight;
//      }

    }




    protected override void OnPaintBackground( PaintEventArgs args )
    {
      base.OnPaintBackground( args );

      if( m_nameColumn == null )
        return;

      int x = 0;
      foreach( int pos in GetColumnOrder() )
      {
        ColumnHeader header = this.Columns[ pos ];
        if( header == m_nameColumn )
          break;

        x += header.Width;
      }

      using( Brush brush = new SolidBrush( this.CustomColumnColor ))
        args.Graphics.FillRectangle( brush, x, args.ClipRectangle.Y, m_nameColumn.Width, args.ClipRectangle.Height );
    }


    protected override void OnDoubleClick( EventArgs args )
    {
      base.OnDoubleClick( args );

      if( this.SelectedItems.Count == 0 )
        return;

      DirectoryInfo[] selectedDirs = this.SelectedDirectories;
      if( selectedDirs.Length != 0 )
        OnDirectoryOpened( selectedDirs[0] );
      else
        OnFileOpened( this.SelectedFiles[0] );
    }


    protected override void OnKeyDown( KeyEventArgs args )
    {
      base.OnKeyDown( args );

      if( this.SelectedItems.Count == 0 || (( args.KeyCode & Keys.Enter ) != Keys.Enter ))
        return;

      DirectoryInfo[] selectedDirs = this.SelectedDirectories;
      if( selectedDirs.Length != 0 )
        OnDirectoryOpened( selectedDirs[0] );
      else
        OnFileOpened( this.SelectedFiles[0] );
    }


    protected virtual void OnDirectoryOpened( DirectoryInfo directory )
    {
      m_current = directory;

      this.Items.Clear();
      RefreshView();

      // raise event
    }


    protected virtual void OnFileOpened( FileInfo file )
    {
      // raise event
    }

    #endregion

    #region methods

    public void GenerateDefaultColumns()
    {
      if( this.DisplayName )
      {
        m_nameColumn = CreateHeader( "Name", HorizontalAlignment.Left, 225 );
        this.Columns.Add( m_nameColumn );
      }

      if( this.DisplaySize )
      {
        m_sizeColumn = CreateHeader( "Size", HorizontalAlignment.Right, 100 );
        this.Columns.Add( m_sizeColumn );
      }

      if( this.DisplayType )
      {
        m_typeColumn = CreateHeader( "Type", HorizontalAlignment.Left, 175 );
        this.Columns.Add( m_typeColumn );
      }

      if( this.DisplayDate )
      {
        m_dateColumn = CreateHeader( "Date Modified", HorizontalAlignment.Left, 125 );
        this.Columns.Add( m_dateColumn );
      }
    } // GenerateDefaultColumns()

    
    public void AddFiles( params string[] paths )
    {
      foreach( string path in paths )
      {
        if( Directory.Exists( path ))
          AddDirectory( new DirectoryInfo( path ));
        else if( File.Exists( path ))
          AddFile( new FileInfo( path ));        
      }
    }


    public void Reset()
    {
      this.Items.Clear();
      if( this.ParseFileSystem )
        RefreshView();
    }


    public void RefreshView()
    {
      if( this.Current == null )
        return;

      foreach( DirectoryInfo dir in this.Current.GetDirectories() )
        AddDirectory( dir );

      foreach( FileInfo file in this.Current.GetFiles() )
        AddFile( file );

      if( this.Root != null && this.Current.FullName == this.Root.FullName )
        return;

      ListViewItem parent = CreateParentItem( this.Current.Parent );
      this.Items.Insert( 0, parent );
    }


    public ListViewItem AddDirectory( DirectoryInfo directory )
    {
      int imageIndex = SystemImageManager.Instance.GetFolderIndex( FolderStyle.Closed );
      ListViewItem item = CreateItem( directory, string.Empty, imageIndex );
     
      if( this.ShowDirectories )
        this.Items.Add( item );

      return item;
    }


    public ListViewItem AddFile( FileInfo file )
    {
      if( IsExcludedFile( file ))
        return null;

      NumberFormatInfo numberInfo = new CultureInfo( "en-US", false ).NumberFormat;
      numberInfo.NumberDecimalDigits = 0;

      string length = ( file.Length / 1000 ).ToString( "N", numberInfo ) + " KB";
      int imageIndex = SystemImageManager.Instance.GetImageIndex( file.FullName );

      ListViewItem item = CreateItem( file, length, imageIndex );

      if( this.ShowFiles )
        this.Items.Add( item );

      return item;
    }


    private ColumnHeader CreateHeader( string text, HorizontalAlignment align, int width )
    {
      ColumnHeader header = new ColumnHeader();
      header.Text = text;
      header.TextAlign = align;
      header.Width = width;

      return header;
    }


    private ListViewItem CreateItem( FileSystemInfo tag, string length, int imageIndex )
    {
      ArrayList subItems = new ArrayList();
      if( this.DisplayName )
        subItems.Add( tag.Name );

      if( this.DisplaySize )
        subItems.Add( length );

      if( this.DisplayType )
        subItems.Add( Viatronix.UI.Utilities.GetMimeTypeName( tag.FullName ));

      if( this.DisplayDate )
        subItems.Add( tag.LastAccessTime.ToString( DateFormat ));

      ListViewItem item = new ListViewItem( (string[]) subItems.ToArray( typeof( string )), imageIndex );
      item.Tag = tag;

      return item;
    }


    private ListViewItem CreateParentItem( DirectoryInfo directory )
    {
      int imageIndex = SystemImageManager.Instance.GetFolderIndex( FolderStyle.Open );
      string[] subitems = new string[] { "...", string.Empty, "Parent Folder", string.Empty };
      ListViewItem item = new ListViewItem( subitems, imageIndex, this.ForeColor, Color.LightGray, this.Font );
      item.Tag = directory;
      return item;
    }


    private int[] GetColumnOrder()
    {
      int count = this.Columns.Count;
      int[] order = new int[ count ];
      Win32.User32.SendMessage( this.Handle, (int) Win32.LVM.LVM_GETCOLUMNORDERARRAY, count, order );
      return order;
    }


    private StringFormat GetStringFormat( ColumnHeader header )
    {
      StringFormat format = new StringFormat();
      if( header.TextAlign == HorizontalAlignment.Center )
        format.Alignment = StringAlignment.Center;
      else if( header.TextAlign == HorizontalAlignment.Left )
        format.Alignment = StringAlignment.Near;
      else
        format.Alignment = StringAlignment.Far;

      return format;
    }


    private bool IsExcludedFile( FileInfo file )
    {
      foreach( string exclusion in this.Exclusions )
      {
        string pattern = exclusion.Replace( "*", ".*" );
        if( Regex.IsMatch( file.Name, pattern, RegexOptions.IgnoreCase ))
          return true;
      }

      return false;
    }

    #endregion

  } // class FileSystemListView
} // namespace Viatronix.UI


#region revision history

// $Log: FileSystemListView.cs,v $
// Revision 1.4  2005/05/26 17:18:31  gdavidson
// Added support for a transparent background.
//
// Revision 1.3  2005/04/20 15:36:27  gdavidson
// Added a check for null before checking if the root directory has changed.
//
// Revision 1.2  2005/03/29 01:03:16  gdavidson
// Reset the control if the RootDirectory is changed.
//
// Revision 1.1  2001/01/02 12:41:26  gdavidson
// Initial Revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/FileSystemListView.cs,v 1.4 2005/05/26 17:18:31 gdavidson Exp $
// $Id: FileSystemListView.cs,v 1.4 2005/05/26 17:18:31 gdavidson Exp $

#endregion