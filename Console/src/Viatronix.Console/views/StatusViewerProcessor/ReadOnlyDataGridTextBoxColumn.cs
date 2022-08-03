// $Id: ReadOnlyDataGridTextBoxColumn.cs,v 1.3 2007/03/06 16:33:20 mkontak Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )


#region usings

using System;
using System.Data;
using System.Windows.Forms;
using System.Drawing;

#endregion

namespace Viatronix.Console
{
	/// <summary>
	/// This class provides a ReadOnly Text Box Column which can be assocciated with a Grid Cell.
	/// </summary>
	public class ReadOnlyDataGridTextBoxColumn : DataGridTextBoxColumn
	{

    #region constructors
    
    /// <summary>
    /// Constructor
    /// </summary>
		public ReadOnlyDataGridTextBoxColumn()
		{
      //Set the Text Field as Read only
      this.ReadOnly = true;
		}//ReadOnlyDataGridTextBoxColumn

    #endregion


    #region overrides

    /// <summary>
    /// If Selected paint the cell with Blue else White. 
    /// </summary>
    /// <param name="gfx">Graphics Object</param>
    /// <param name="bounds">Bounds of the Column</param>
    /// <param name="source">CurrencyManager</param>
    /// <param name="rowNum">row index in grid</param>
    /// <param name="backBrush">Brush</param>
    /// <param name="foreBrush">Brush</param>
    /// <param name="alignToRight">bool</param>
    protected  override void Paint(System.Drawing.Graphics gfx, System.Drawing.Rectangle bounds, CurrencyManager source, int rowNum, System.Drawing.Brush backBrush, System.Drawing.Brush foreBrush, bool alignToRight)
    {
      Brush brushBack  = ( this.DataGridTableStyle.DataGrid.CurrentRowIndex == rowNum ) ? new SolidBrush( Color.FromArgb( 49,106,197 )):backBrush;
      Brush brushFront = ( this.DataGridTableStyle.DataGrid.CurrentRowIndex == rowNum ) ? new SolidBrush( Color.White ):foreBrush;
      
      base.Paint (gfx, bounds, source, rowNum, brushBack, brushFront, alignToRight);
    } // Paint(System.Drawing.Graphics gfx, System.Drawing.Rectangle bounds, CurrencyManager source, int rowNum, System.Drawing.Brush backBrush, System.Drawing.Brush foreBrush, bool alignToRight)

    
    #endregion 

	} // class ReadOnlyDataGridTextBoxColumn
} // namespace Viatronix.Console

#region revision history

// $Log: ReadOnlyDataGridTextBoxColumn.cs,v $
// Revision 1.3  2007/03/06 16:33:20  mkontak
// Coding standards
//
// Revision 1.2  2007/03/06 15:37:55  mkontak
// Coding standards
//
// Revision 1.1  2006/10/11 13:13:59  vxconfig
// modified filter name
//
// Revision 1.2  2005/12/02 02:24:27  romy
// coding standards
//
// Revision 1.1  2005/08/05 14:20:01  romy
// Rearranged Files
//
// Revision 1.2  2005/05/19 18:40:57  romy
// Added Header Information
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/views/StatusViewerProcessor/ReadOnlyDataGridTextBoxColumn.cs,v 1.3 2007/03/06 16:33:20 mkontak Exp $
// $Id: ReadOnlyDataGridTextBoxColumn.cs,v 1.3 2007/03/06 16:33:20 mkontak Exp $

#endregion