// $Id: DynamicColumnHeader.cs,v 1.5 2006/02/06 20:07:12 gdavidson Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Reflection;
using System.Collections;
using System.Windows.Forms;
using System.Xml;

namespace Viatronix.UI
{
	/// <summary>
	/// Column generated from xml and populates the subitems using reflection.
	/// </summary>
	public abstract class DynamicColumnHeader : ColumnHeader, ICloneable
	{

    #region fields

    /// <summary>
    /// The id of the column.
    /// </summary>
    private string m_id = string.Empty;

    /// <summary>
    /// Special formatting for the column.
    /// </summary>
    private string m_formatting = string.Empty;

    /// <summary>
    /// Property reference by the column.
    /// </summary>
    private string m_property = string.Empty;

    /// <summary>
    /// Determines if the column is default visible.
    /// </summary>
    private bool m_isDefault = false;

    /// <summary>
    /// A custom IComparer for column sorting.
    /// </summary>
    private string m_comparerType = string.Empty;

    private SortOrder m_sortOrder = SortOrder.None;

    #endregion

    #region properties

    /// <summary>
    /// Returns the ID for the column.
    /// </summary>
    public string ID
    {
      get { return m_id; }
    } // ID


    /// <summary>
    /// Returns the formatting used by the column.
    /// </summary>
    public string Formatting
    {
      get { return m_formatting; }
    } // Formatting


    /// <summary>
    /// Returns the property string used by the column.
    /// </summary>
    public string Property
    {
      get { return m_property; }
    } // Property


    /// <summary>
    /// Returns if the column is visible by default.
    /// </summary>
    public bool Default
    {
      get { return m_isDefault; }
    } // Default


    public SortOrder SortOrder
    {
      get { return m_sortOrder; }
      set { m_sortOrder = value; }
    } // SortOrder

    #endregion

    #region methods

    /// <summary>
    /// Returns the value for the column from the provided object.
    /// </summary>
    /// <param name="obj">The object containing the value.</param>
    /// <returns>The value of the object.</returns>
    public abstract object GetValue( object obj );


    public string GetString( object obj )
    {
      return ConvertToString( GetValue( obj ));
    } // GetString( obj )


    public IComparer CreateComparer()
    {
      return CreateComparer( true );
    } // CreateComparer()


    public IComparer CreateComparer( bool flip )
    {
      if( flip )
      {
        if( m_sortOrder == SortOrder.Descending || m_sortOrder == SortOrder.None )
          m_sortOrder = SortOrder.Ascending;
        else
          m_sortOrder = SortOrder.Descending;
      }

      if( m_comparerType == string.Empty )
        return new DynamicListViewDefaultComparer( m_sortOrder, this.Index );

      return (IComparer) Activator.CreateInstance( Type.GetType( m_comparerType ));
    } // CreateComparer()


    /// <summary>
    /// Allows for custom initialization of the column header.
    /// </summary>
    /// <param name="node">XmlNode containing custom configuration.</param>
    public virtual void Initialize( XmlNode node )
    {
      m_id = node.Attributes[ "id" ].Value;

      if( node.Attributes[ "text" ] != null )
        this.Text = node.Attributes[ "text" ].Value;
      else
        this.Text = string.Empty;
      
      if( node.Attributes[ "width" ] != null )
        this.Width = Convert.ToInt32( node.Attributes[ "width" ].Value );

      if( node.Attributes[ "align" ] != null )
        this.TextAlign = (HorizontalAlignment) Enum.Parse( typeof( HorizontalAlignment ), node.Attributes[ "align" ].Value, true );

      if( node.Attributes[ "default" ] != null )
        SetDefault( bool.Parse( node.Attributes[ "default" ].Value ));

      if( node.Attributes[ "formatting" ] != null )
        m_formatting = node.Attributes[ "formatting" ].Value;

      m_property = node.Attributes[ "property" ].Value;

      if( node.Attributes[ "comparer" ] != null )
        m_comparerType = node.Attributes[ "comparer" ].Value;

      if( node.Attributes[ "sortOrder" ] != null )
        m_sortOrder = (SortOrder) Enum.Parse( typeof( SortOrder ), node.Attributes[ "sortOrder" ].Value, true );
    } // Initialize( node )


    /// <summary>
    /// Generates the output for the specified object.
    /// </summary>
    /// <param name="obj">The object being outputed.</param>
    /// <returns>A string representation of the object.</returns>
    public string ConvertToString( object obj )
    {
      if( obj == null )
        return string.Empty;

      Type type = obj.GetType();
      if( this.Formatting == string.Empty )
      {
        MethodInfo method = type.GetMethod( "ToString", new Type[0] );
        return (string) method.Invoke( obj, new object[0] );
      }
      else
      {
        MethodInfo method = type.GetMethod( "ToString", new Type[] { typeof( string )} );
        return (string) method.Invoke( obj, new object[] { this.Formatting });
      }
    } // ConvertToString( obj )


    /// <summary>
    /// Displays an icon indicating the sort order or nothing.
    /// </summary>
    /// <param name="show">true to display an icon; otherwise false</param>
    public void ShowSortIcon( bool show )
    {
      // get the display order for the column
      int count = this.ListView.Columns.Count;
      int[] order = new int[ count ];
      Win32.User32.SendMessage( this.ListView.Handle, (int) Win32.LVM.LVM_GETCOLUMNORDERARRAY, count, order );

      int index = 0;
      for( int i = 0; i < count; ++i )
      {
        if( order[i] == this.Index )
        {
          index = i;
          break;
        }
      }


      // create a LVCOLUMN with the proper formatting
      Win32.LVCOLUMN column = new Win32.LVCOLUMN();
      column.mask = (int) ( Win32.LVCF.LVCF_FMT );
      column.fmt = (int) this.TextAlign;

      // setup the LVCOLUMN for an image 
      if( show )
      {
        column.mask |= (int) Win32.LVCF.LVCF_IMAGE;
        column.fmt |= (int) Win32.LVCFMT.LVCFMT_IMAGE;
        if( this.TextAlign != HorizontalAlignment.Right )
          column.fmt |= (int) Win32.LVCFMT.LVCFMT_BITMAP_ON_RIGHT;

        column.iImage = ( m_sortOrder == SortOrder.Ascending ) ? 0 : 1;
      }
        
      Win32.User32.SendMessage( this.ListView.Handle, (int) Win32.LVM.LVM_SETCOLUMNA, index, ref column );
    } // ShowSortIcon()


    /// <summary>
    /// Sets the status oof the default member of the column header.
    /// </summary>
    /// <param name="isDefault">true if the column is visible by default; otherwise false</param>
    protected void SetDefault( bool isDefault )
    {
      m_isDefault = isDefault;
    } // SetDefault( isDefault )

    #endregion

    #region ICloneable Members

    /// <summary>
    /// Creates a new object that is a copy of the current instance.
    /// </summary>
    /// <returns>A new object that is a copy of this instance.</returns>
    public new abstract object Clone();


    /// <summary>
    /// Provides the ability to copy the members of one column header to another.
    /// </summary>
    /// <param name="column">The destination of the copy.</param>
    protected void CopyTo( DynamicColumnHeader column )
    {
      column.m_id = this.ID;
      column.m_formatting = this.Formatting;
      column.m_isDefault = this.Default;
      column.m_property = this.Property;

      column.Width = this.Width;
      column.TextAlign = this.TextAlign;
      column.Text = this.Text;
    } // CopyTo( column )

    #endregion

  } // class DynamicColumnHeader

  #region DynamicListViewDefaultComparer class

  public class DynamicListViewDefaultComparer : IComparer
  {

    #region fields

    private SortOrder m_sortOrder = SortOrder.Ascending;

    private int m_index = 0;

    private Comparer m_comparer = new Comparer( System.Globalization.CultureInfo.CurrentCulture );

    #endregion

    #region construction

    public DynamicListViewDefaultComparer( SortOrder order, int index )
    {
      m_sortOrder = order;
      m_index = index;
    } // DynamicListViewComparer( order )

    #endregion

    #region IComparer Members

    public int Compare( object x, object y )
    {
      int sortOrder = ( m_sortOrder == SortOrder.Ascending ) ? 1 : -1;

      ListViewItem itemX = (ListViewItem) x;
      ListViewItem itemY = (ListViewItem) y;

      object objX = null;
      object objY = null;

      try
      {
//        if( m_index == 0 )
//        {
//          objX = itemX.Tag;
//          objY = itemY.Tag;
//        }
//        else
        {
          objX = ((DynamicListViewSubItem) itemX.SubItems[ m_index ]).Tag;
          objY = ((DynamicListViewSubItem) itemY.SubItems[ m_index ]).Tag;
        }

        return sortOrder * m_comparer.Compare( objX, objY );
      }
      catch( ArgumentException )
      {
        return 0;
      }
    } // Compare( x, y )

    #endregion
    
  } // class DynamicListViewDefaultComparer


  #endregion

  #region IDynamicImageColumnHeader interface

  /// <summary>
  /// Provides the interface for the image column in a DynamicListView.
  /// </summary>
  public interface IDynamicImageColumnHeader
  {

    /// <summary>
    /// Returns a list of small images.
    /// </summary>
    ImageList SmallImages
    {
      get;
    } // SmallImages


    /// <summary>
    /// Returns a list of large images.
    /// </summary>
    ImageList LargeImages
    {
      get;
    } // LargeImages


    /// <summary>
    /// Returns the index of the image for the specified object.
    /// </summary>
    /// <param name="obj">The object being displayed in the DynamicListView.</param>
    /// <returns>Index of the image.</returns>
    int GetIndex( object obj );

  } // interface IDynamicImageColumnHeader

  #endregion

} // namespace Viatronix.UI


#region revision history

// $Log: DynamicColumnHeader.cs,v $
// Revision 1.5  2006/02/06 20:07:12  gdavidson
// Added an icon indicating the sort ordering
//
// Revision 1.4  2005/10/18 17:54:53  gdavidson
// Added sorting of columns.
//
// Revision 1.3  2005/06/22 14:57:03  gdavidson
// Added IDynamicImageColumnHeader interface.
//
// Revision 1.2  2005/06/03 14:46:57  gdavidson
// Added functionality for a dataset.
//
// Revision 1.1  2005/05/26 17:27:18  gdavidson
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/DynamicColumnHeader.cs,v 1.5 2006/02/06 20:07:12 gdavidson Exp $
// $Id: DynamicColumnHeader.cs,v 1.5 2006/02/06 20:07:12 gdavidson Exp $

#endregion
