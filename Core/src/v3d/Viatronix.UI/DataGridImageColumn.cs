// $Id: DataGridImageColumn.cs,v 1.1 2005/04/15 18:10:27 romy Exp $
//
// Copyright© (2002), Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]

using System;
using System.Data;
using System.IO;
using System.Drawing;
using System.Windows.Forms;
using System.Reflection;

namespace Viatronix.UI
{
  /// <summary>
  /// Summary description for DataGridImageColumn.
  /// </summary>
  public class DataGridImageColumn : DataGridColumnStyle
  {

    private Bitmap m_image = null;
    private bool m_visible = true;
    private string m_imageColumn = string.Empty;

    public string ImageColumn
    {
      set { m_imageColumn = value; }
      get { return m_imageColumn; }
    }

    public bool Visible
    {
      set { m_visible = value; }
      get { return m_visible; }
    }

    public Bitmap Image 
    {
      set { m_image = value; }
      get { return m_image; }
    }

    /// <summary>
    /// Constructor
    /// </summary>
    public DataGridImageColumn()
    {
    } // DataGridImageColumn()

    #region Overrides

    /// <summary>
    /// Get minimum height
    /// </summary>
    /// <returns>height</returns>
    protected override int GetMinimumHeight()
    {
      return 16;  // pixels
    } // GetMinimumHeight()


    /// <summary>
    /// Get preferred height
    /// </summary>
    /// <param name="g">graphics object</param>
    /// <param name="value">get object</param>
    /// <returns>height</returns>
    protected override int GetPreferredHeight( Graphics g, object value )
    {
      return 16; // pixels
    } // GetPreferredHeight()


    /// <summary>
    /// Get preferred size
    /// </summary>
    /// <param name="g">graphics object</param>
    /// <param name="value">get object</param>
    /// <returns>preferred size</returns>
    protected override Size GetPreferredSize( Graphics g, object value )
    {
      return new Size( 16, 16 ); // pixels
    } // GetPreferredSize()


    /// <summary>
    /// Complete editing procedure
    /// </summary>
    /// <param name="dataSource">currency manager for data source</param>
    /// <param name="rowNum">number of row being edited</param>
    /// <returns></returns>
    protected override bool Commit( CurrencyManager dataSource, int rowNum )
    {
      return true;  // since we're read-only, return true;
    } // Commit()


    /// <summary>
    /// Prepares the cell for editing
    /// </summary>
    /// <param name="source">currency manager for data source</param>
    /// <param name="rowNum">number of row to edit</param>
    /// <param name="bounds">bounds of rectangle control sited</param>
    /// <param name="readOnly">indicates column is read-only</param>
    protected override void Edit( CurrencyManager source, int rowNum, Rectangle bounds, bool readOnly )
    {
      // do nothing - read-only
    } // Edit()


    /// <summary>
    /// Prepares the cell for editing
    /// </summary>
    /// <param name="source">currency manager for data source</param>
    /// <param name="rowNum">number of row to edit</param>
    /// <param name="bounds">bounds of rectangle control sited</param>
    /// <param name="readOnly">indicates column is read-only</param>
    /// <param name="instantText">text to display in control</param>
    /// <param name="cellsVisible">indicates cell is visible</param>
    protected override void Edit( CurrencyManager source, int rowNum, Rectangle bounds, bool readOnly, string instantText, bool cellsVisible )
    {
      // do nothing - read-only
    } // Edit()


    /// <summary>
    /// Interrupt an edit procedure
    /// </summary>
    /// <param name="rowNum">number of row being edit</param>
    protected override void Abort( int rowNum )
    {
      // do nothing - read-only
    } // Abort()


    /// <summary>
    /// Paints the column in a datagrid
    /// </summary>
    /// <param name="g">graphics object</param>
    /// <param name="bounds">bounds of rectangle to paint into</param>
    /// <param name="source">currency manager for data source</param>
    /// <param name="rowNum">The number of the row in the underlying data table being referred to</param>
    /// <param name="alignToRight">indicating whether to align the content to the right</param>
    protected override void Paint( Graphics g, Rectangle bounds, CurrencyManager source, int rowNum, bool alignToRight )
    {
      try
      {

        if ( ! m_visible ) return;
 
        // we want to paint a maximum of a 16x16 area even if the size of the cell's rectangle is larger than 16 pixels
        int imageWidth = Math.Min( bounds.Width, 16 );
        int imageHeight = Math.Min( bounds.Height, 16 );

        int textWidth = Math.Max( bounds.Width - 16, 0 );
        int textHeight = bounds.Height;
        int textX = (textWidth == 0 ? bounds.X : bounds.X + 17);
        int textY = bounds.Y;

        // because we're using a custom sized rectangle wee need to create it
        Rectangle imageRect = new Rectangle( bounds.X, bounds.Y, imageWidth, imageHeight );
        Rectangle textRect = new Rectangle( textX, textY, textWidth, textHeight);


        object obj = this.GetColumnValueAtRow( source, rowNum );

        if ( obj.GetType() == typeof(Bitmap) )
        {
          g.DrawImage( (Bitmap)obj, imageRect );
        }
        else
        {
          string text = (string)obj;

          if ( m_image != null )
          {
            g.DrawImage( m_image, imageRect );
            g.DrawString(text, this.DataGridTableStyle.DataGrid.Font, new SolidBrush(this.DataGridTableStyle.ForeColor), textRect, StringFormat.GenericDefault);
          }
          else if ( m_imageColumn != string.Empty )
          {

            if ( source.Bindings.Count > 0 )
            {
              DataRowView dv = (DataRowView)source.Bindings[0].BindingManagerBase.Current;
            
          
              try
              {
                object imageObj = dv.Row[m_imageColumn];

                if ( imageObj.GetType() != typeof(Bitmap) )
                {
                  throw new Exception("Invalid type");
                }


                g.DrawImage( (Bitmap)imageObj, imageRect );
                g.DrawString(text, this.DataGridTableStyle.DataGrid.Font, new SolidBrush(this.DataGridTableStyle.ForeColor), textRect, StringFormat.GenericDefault);

              }
              catch ( Exception )
              {
                g.DrawString(text, this.DataGridTableStyle.DataGrid.Font, new SolidBrush(this.DataGridTableStyle.ForeColor), bounds, StringFormat.GenericDefault);
              }

            }
            else
            {
              g.DrawString(text, this.DataGridTableStyle.DataGrid.Font, new SolidBrush(this.DataGridTableStyle.ForeColor), bounds, StringFormat.GenericDefault);
            }
          }
          else
          {
            g.DrawString(text, this.DataGridTableStyle.DataGrid.Font, new SolidBrush(this.DataGridTableStyle.ForeColor), bounds, StringFormat.GenericDefault);
          }
        }
      }
      catch( Exception e )
      {
        throw e;
      }
    } // Paint()


    /// <summary>
    /// Paints the column in a datagrid
    /// </summary>
    /// <param name="g">graphics object</param>
    /// <param name="bounds">bounds of rectangle to paint into</param>
    /// <param name="source">currency manager for data source</param>
    /// <param name="rowNum">The number of the row in the underlying data table being referred to</param>
    protected override void Paint( Graphics g, Rectangle bounds, CurrencyManager source, int rowNum )
    {
      this.Paint( g, bounds, source, rowNum, false );
    } // Paint()


    /// <summary>
    /// Paints the column in a datagrid
    /// </summary>
    /// <param name="g">graphics object</param>
    /// <param name="bounds">bounds of rectangle to paint into</param>
    /// <param name="source">currency manager for data source</param>
    /// <param name="rowNum">The number of the row in the underlying data table being referred to</param>
    /// <param name="backBrush">used to paint the background co</param>
    /// <param name="foreBrush">used to paint the foreground color</param>
    /// <param name="alignToRight">value indicating whether to align the content to the right</param>
    protected override void Paint( Graphics g, Rectangle bounds, CurrencyManager source, int rowNum, Brush backBrush, Brush foreBrush, bool alignToRight )
    {
      try
      {
        if ( ! m_visible ) return;

        // paint background
        g.FillRegion( backBrush, new Region( bounds ));

        // we want to paint a maximum of a 16x16 area even if the size of the cell's rectangle is larger than 16 pixels
        int imageWidth = Math.Min( bounds.Width, 16 );
        int imageHeight = Math.Min( bounds.Height, 16 );

        int textWidth = Math.Max( bounds.Width - 16, 0 );
        int textHeight = bounds.Height;
        int textX = (textWidth == 0 ? bounds.X : bounds.X + 17);
        int textY = bounds.Y;

        // because we're using a custom sized rectangle wee need to create it
        Rectangle imageRect = new Rectangle( bounds.X, bounds.Y, imageWidth, imageHeight );
        Rectangle textRect = new Rectangle( textX, textY, textWidth, textHeight);
       
        object obj = this.GetColumnValueAtRow( source, rowNum );

        if ( obj.GetType() == typeof(Bitmap) )
        {
          g.DrawImage( (Bitmap)obj, imageRect );
        }
        else
        {
          string text = (string)obj;

          if ( m_image != null )
          {
            g.DrawImage( m_image, imageRect );
            g.DrawString(text, this.DataGridTableStyle.DataGrid.Font, new SolidBrush(this.DataGridTableStyle.ForeColor), textRect, StringFormat.GenericDefault);
          }
          else if ( m_imageColumn != string.Empty )
          {

            DataRowView dv = ((DataRowView)source.Current).DataView[rowNum];
                
            try
            {


              object imageObj = dv.Row[m_imageColumn];

              if ( ! System.Convert.IsDBNull(imageObj) && imageObj.GetType() != typeof(Bitmap) )
              {
                throw new Exception("Invalid type");
              }


              g.DrawImage( (Bitmap)imageObj, imageRect );
              g.DrawString(text, this.DataGridTableStyle.DataGrid.Font, new SolidBrush(this.DataGridTableStyle.ForeColor), textRect, StringFormat.GenericDefault);

            }
            catch ( Exception  )
            {
              g.DrawString(text, this.DataGridTableStyle.DataGrid.Font, new SolidBrush(this.DataGridTableStyle.ForeColor), bounds, StringFormat.GenericDefault);
            }

          }
          else
          {
            g.DrawString(text, this.DataGridTableStyle.DataGrid.Font, new SolidBrush(this.DataGridTableStyle.ForeColor), bounds, StringFormat.GenericDefault);
          }
        }
      }
      catch( Exception e )
      {
        throw e;
      }
    } // Paint()

    #endregion

    #region properties


    /// <summary>
    /// Format
    /// </summary>
    public string Format
    {
      get { return ""; }
      set { }
    } // Format


    /// <summary>
    /// FormatInfo
    /// </summary>
    public string FormatInfo
    {
      get { return ""; }
      set { }
    } // FormatInfo

    #endregion
	}
}
#region Revision History

// $Log: DataGridImageColumn.cs,v $
// Revision 1.1  2005/04/15 18:10:27  romy
// Moved from Console to v3D.GUI
//

// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/DataGridImageColumn.cs,v 1.1 2005/04/15 18:10:27 romy Exp $
// $Id: DataGridImageColumn.cs,v 1.1 2005/04/15 18:10:27 romy Exp $

#endregion
