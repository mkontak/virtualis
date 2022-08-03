// $Id: DataGridCheckboxColumn.cs,v 1.5 2006/04/27 20:59:59 romy Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan (mailto:romy@viatronix.com)

using System;
using System.Data;
using System.Windows.Forms;
using System.Drawing;

namespace Viatronix.UI
{

  /// <summary>
  /// Checkbox datagrid column.
  /// </summary>
  public class DataGridCheckBoxColumn : DataGridColumnStyle 
  {

    #region fields

    /// <summary>
    /// Size of the column.
    /// </summary>
    private const int size = 16;

    /// <summary>
    /// Checkbox contained in the column.
    /// </summary>
    private System.Windows.Forms.CheckBox m_checkbox = new CheckBox();
    
    /// <summary>
    /// Allows editting of the column.
    /// </summary>
    private bool m_editing;

    private EventHandler m_checkedChangedHandler = null; 
    
    #endregion

    #region construction

    /// <summary>
    /// Creates a new instance of a DataGridCheckBoxColumn;
    /// </summary>
    public DataGridCheckBoxColumn() : base() 
    {
      //m_checkbox.FlatStyle = FlatStyle.System;
      m_checkbox.Visible = false;
      m_checkedChangedHandler = new EventHandler( OnCheckedChangedHandler );
    } // DataGridCheckBoxColumn()

    #endregion

    #region override methods


    /// <summary>
    /// Initiates a request to interrupt an edit procedure.
    /// </summary>
    /// <param name="row">The row number upon which an operation is being interrupted.</param>
    protected override void Abort( int row )
    {
      m_editing = false;
      m_checkbox.CheckedChanged -= m_checkedChangedHandler;
      Invalidate();
    } // Abort( row )


    /// <summary>
    /// Initiates a request to complete an editing procedure.
    /// </summary>
    /// <param name="dataSource">The CurrencyManager for the DataGridColumnStyle.</param>
    /// <param name="row">The number of the row being edited.</param>
    /// <returns>true if the editing procedure committed successfully; otherwise, false.</returns>
    protected override bool Commit( CurrencyManager dataSource, int row ) 
    {
      m_checkbox.Bounds = Rectangle.Empty;
         
      m_checkbox.CheckedChanged -= m_checkedChangedHandler;

      if( !m_editing )
        return true;

      m_editing = false;

      try 
      {
        SetColumnValueAtRow( dataSource, row, m_checkbox.Checked );
      } 
      catch( Exception ) 
      {
        Abort( row );
        return false;
      }

      Invalidate();
      return true;
    } // Commit( dataSource, row ) 


    /// <summary>
    /// Prepares the cell for editing a value.
    /// </summary>
    /// <param name="source">The CurrencyManager for the DataGridColumnStyle.</param>
    /// <param name="row">The row number in this column which is being edited.</param>
    /// <param name="bounds">The Rectangle in which the control is to be sited.</param>
    /// <param name="readOnly">A value indicating whether the column is a read-only. true if the value is read-only; otherwise, false.</param>
    /// <param name="instantText">The text to display in the control.</param>
    /// <param name="cellIsVisible">A value indicating whether the cell is visible. true if the cell is visible; otherwise, false.</param>
    protected override void Edit( CurrencyManager source, int row, Rectangle bounds, 
                                  bool readOnly, string instantText, bool cellIsVisible ) 
    {
      if( cellIsVisible ) 
      {
        int x = bounds.Left + ( int )( ( bounds.Width - size ) / 2.0F + 0.5F );
        int y = bounds.Top + ( int )( ( bounds.Height - size ) / 2.0F + 0.5F );

        m_checkbox.Bounds = new Rectangle( x, y, size, size );
        m_checkbox.Checked = (bool)GetColumnValueAtRow( source, row );
        m_checkbox.Visible = true;
        m_checkbox.CheckedChanged += m_checkedChangedHandler;
      } 
      else 
      {
        m_checkbox.Checked = (bool)GetColumnValueAtRow( source, row );
        m_checkbox.Visible = false;
      }

      if( m_checkbox.Visible )
        DataGridTableStyle.DataGrid.Invalidate( bounds );
    } // Edit( ... )


    /// <summary>
    /// Gets the width and height of the specified value. The width and height are used when the user 
    /// navigates to DataGridTableStyle using the DataGridColumnStyle.
    /// </summary>
    /// <param name="gfx">A Graphics object.</param>
    /// <param name="value">An object value for which you want to know the screen height and width.</param>
    /// <returns>A Size that contains the dimensions of the cell.</returns>
    protected override Size GetPreferredSize( Graphics gfx, object value ) 
    {
      return new Size( 100, m_checkbox.Height + 4 );
    } // GetPreferredSize( gfx, value ) 


    /// <summary>
    /// Gets the minimum height of a row.
    /// </summary>
    /// <returns>The minimum height of a row.</returns>
    protected override int GetMinimumHeight() 
    {
      return m_checkbox.Height;
    } // GetMinimumHeight()


    /// <summary>
    /// Gets the height used for automatically resizing columns.
    /// </summary>
    /// <param name="gfx">A Graphics object.</param>
    /// <param name="value">A object value for which you want to know the screen height and width.</param>
    /// <returns>The height used for auto resizing a cell.</returns>
    protected override int GetPreferredHeight( Graphics gfx, object value ) 
    {
      return m_checkbox.Height;
    } // GetPreferredHeight( gfx, value ) 


    /// <summary>
    /// Paints the a DataGridColumnStyle with the specified Graphics, Rectangle, CurrencyManager, and row number.
    /// </summary>
    /// <param name="gfx">The Graphics object to draw to.</param>
    /// <param name="bounds">The bounding Rectangle to paint into.</param>
    /// <param name="source">The CurrencyManager of the System.Windows.Forms.DataGrid control the column belongs to.</param>
    /// <param name="row">The number of the row in the underlying data being referred to.</param>
    protected override void Paint( Graphics gfx, Rectangle bounds, CurrencyManager source, int row ) 
    {
      //Paint( gfx, bounds, source, row, false );
    } // Paint( gfx, bounds, source, row ) 
    

    /// <summary>
    /// Paints a DataGridColumnStyle with the specified Graphics, Rectangle, CurrencyManager, row number, and alignment.
    /// </summary>
    /// <param name="gfx">The Graphics object to draw to.</param>
    /// <param name="bounds">The bounding Rectangle to paint into.</param>
    /// <param name="source">The CurrencyManager of the System.Windows.Forms.DataGrid control the column belongs to.</param>
    /// <param name="row">The number of the row in the underlying data being referred to.</param>
    /// <param name="alignToRight">A value indicating whether to align the column's content to the right. true if the content should be aligned to the right; otherwise false.</param>
    protected override void Paint( Graphics gfx, Rectangle bounds, CurrencyManager source, int row, bool alignToRight) 
    {
      //Paint( gfx, bounds, source, row, Brushes.Red, Brushes.Blue, alignToRight );
    } // Paint( gfx, bounds, source, row, alignToRight)
    

    /// <summary>
    /// Paints a DataGridColumnStyle with the specified Graphics, Rectangle, CurrencyManager, row number, background color, foreground color, and alignment.
    /// </summary>
    /// <param name="gfx">The Graphics object to draw to.</param>
    /// <param name="bounds">The bounding Rectangle to paint into.</param>
    /// <param name="source">The CurrencyManager of the System.Windows.Forms.DataGrid control the column belongs to.</param>
    /// <param name="row">The number of the row in the underlying data being referred to.</param>
    /// <param name="backBrush">A Brush used to paint the background color.</param>
    /// <param name="foreBrush">A Color used to paint the foreground color.</param>
    /// <param name="alignToRight">A value indicating whether to align the content to the right. true if the content is aligned to the right, otherwise, false.</param>
    protected override void Paint( Graphics gfx, Rectangle bounds, CurrencyManager source, int row, Brush backBrush, Brush foreBrush, bool alignToRight ) 
    {
      bool isChecked = (bool) GetColumnValueAtRow( source, row );

      int x = bounds.Left + ( int )( ( bounds.Width - size ) / 2.0F + 0.5F );
      int y = bounds.Top + ( int )( ( bounds.Height - size ) / 2.0F + 0.5F );

      gfx.FillRectangle( backBrush, bounds );

      ControlPaint.DrawCheckBox( gfx, x, y, size, size, isChecked? ButtonState.Checked : ButtonState.Normal );

      PlatformID platformId = Environment.OSVersion.Platform;
    } // Paint( gfx, bounds, source, row, backBrush, foreBrush, alignToRight ) 


    /// <summary>
    /// Sets the System.Windows.Forms.DataGrid control that this column belongs to.
    /// </summary>
    /// <param name="grid">The System.Windows.Forms.DataGrid control that this column belongs to.</param>
//    protected override void System.Windows.Forms.DataGridColumnStyle.SetDataGridInColumn( DataGrid grid ) 
//    {
//      base.SetDataGridInColumn( grid );
//      
//      if( m_checkbox.Parent != null ) 
//        m_checkbox.Parent.Controls.Remove( m_checkbox );
//
//      if( grid != null ) 
//        grid.Controls.Add(m_checkbox);
//    } // SetDataGridInColumn( grid )

    #endregion

    #region event handlers


    /// <summary>
    /// Handles the checked change event.
    /// </summary>
    /// <param name="sender">Th sender of the event.</param>
    /// <param name="args">EventArgs containing data specific to this event.</param>
    private void OnCheckedChangedHandler( object sender, EventArgs args ) 
    {
      this.m_editing = true;
      base.ColumnStartedEditing( m_checkbox );
    } // OnCheckedChangedHandler( sender, args ) 

    #endregion

  } // class DataGridCheckBoxColumn
} // namespace Viatronix.UI 

#region revision history

// $Log: DataGridCheckboxColumn.cs,v $
// Revision 1.5  2006/04/27 20:59:59  romy
// event handler fix
//
// Revision 1.4  2005/05/02 14:48:10  frank
// didn't compile as it was
//
// Revision 1.3  2005/04/15 18:35:43  romy
// Fixed Ambiguity with UI Elements
//
// Revision 1.2  2005/03/08 19:11:06  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.1.6.1  2005/03/02 16:47:14  gdavidson
// Code review
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/DataGridCheckboxColumn.cs,v 1.5 2006/04/27 20:59:59 romy Exp $
// $Id: DataGridCheckboxColumn.cs,v 1.5 2006/04/27 20:59:59 romy Exp $

#endregion
