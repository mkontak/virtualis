// $Id: DataGrid.cs,v 1.2 2005/04/15 18:35:43 romy Exp $
//
// Copyright(c) 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: [Mark Kontak] mkontak@viatronix.com

using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

namespace Viatronix.UI
{
  /// <summary>
  /// Common class for Console list display.
  /// </summary>
  public class DataGrid : System.Windows.Forms.DataGrid
  {
    #region variables


    /// <summary>
    /// Last row index
    /// </summary>
    private int m_lastRowIndex = -1;

    /// <summary>
    /// Custom Viatronix popup menu
    /// </summary>
    private Viatronix.UI.PopupMenu m_vxPopupMenu;


    /// <summary>
    /// Whether multiple rows can be selected at once
    /// </summary>
    private bool m_bMultiSelRows = true;


    /// <summary>
    /// Event: mouse pointer has entered the data grid
    /// </summary>
    public event DataGridEnterEventHandler DataGridEnterEvent;
    
    #endregion

    #region properties


    public int LastRowIndex
    {
      get { return m_lastRowIndex; }
    }

    /// <summary>
    /// Get/set VxPopupMenu
    /// </summary>
    public virtual Viatronix.UI.PopupMenu PopUpMenu
    {
      get { return m_vxPopupMenu; }
      set { m_vxPopupMenu = value; }
    } // PopUpMenu


    /// <summary>
    /// Returns the number of rows (items) in the data grid
    /// </summary>
    public int RowCount
    {
      get 
      {
        int iCount = 0;
        if ( this.DataSource.GetType() == typeof(DataViewManager) )
        {                                                  
          DataViewManager dataViewManager = (DataViewManager) this.DataSource;
          iCount = dataViewManager.CreateDataView(dataViewManager.DataSet.Tables[0]).Count;
        }
        else
        {
          iCount = ( (DataSet) this.DataSource ).Tables[0].Rows.Count;
        }

        return iCount;
      }
    } // RowCount

    /// <summary>
    /// Returns the selectedrow count
    /// </summary>
    public int SelectedRowCount
    {
      get
      {
        int selectedRow = 0;
        for ( int row = 0; row < RowCount; row++ )
        {
          selectedRow += ( this.IsSelected(row) ? 1 : 0 );
        }

        return selectedRow;
      }
    }


    /// <summary>
    /// MultiSelectRows
    /// </summary>
    public bool MultiSelectRows
    {
      get { return m_bMultiSelRows; }
      set { m_bMultiSelRows = value; }
    } // MultiSelectRows

    /// <summary>
    /// ContextMenu
    /// </summary>
    [Browsable(false)]
    public override System.Windows.Forms.ContextMenu ContextMenu
    {
       get { return base.ContextMenu; }
    } // ContextMenu
    
    #endregion

    #region constructor
    
    /// <summary>
    /// Constructor
    /// </summary>
    public DataGrid()
    {
      this.ReadOnly = true;
    } // DataGrid()
    
    
    #endregion

    #region overrides
    

    protected override void OnKeyUp( KeyEventArgs args )
    {
      if ( args.KeyCode == Keys.Up )
      {
        ProcessUpKey();
      }
      else if ( args.KeyCode == Keys.Down )
      {
        ProcessDownKey();
      }

      base.OnKeyUp(args);
    }

    /// <summary>
    /// Overridden MouseDown event handler
    /// </summary>
    /// <param name="args">event data</param>
    protected override void OnMouseDown( MouseEventArgs args )
    {
      if ( this.DataSource == null)
        return;

      m_lastRowIndex = CurrentRowIndex;

      System.Windows.Forms.DataGrid.HitTestInfo hitTest = this.HitTest( args.X, args.Y );

      if ( Control.ModifierKeys == Keys.Shift && MultiSelectRows )
      {
        int low  = Math.Min(hitTest.Row, CurrentRowIndex);
        int high = Math.Max(hitTest.Row, CurrentRowIndex);

        for ( int index = 0, count = RowCount; index < count; ++index )
        {
          if ( index < low || index > high )
          {
            this.UnSelect( index );
          }
          else
          {
            this.Select( index );
          }
        }

        return;
      }
      else if ( ( Control.ModifierKeys != Keys.Control ) || !MultiSelectRows )
      {
        for ( int index = 0, count = RowCount; index < count; ++index )
        {
          if ( this.IsSelected(index) )
            this.UnSelect( index );
        }
      }

      if ( hitTest.Type == System.Windows.Forms.DataGrid.HitTestType.Cell )
      {
        this.Select( hitTest.Row );
        this.CurrentRowIndex = hitTest.Row;
      }
      else
        base.OnMouseDown( args );
    } // OnMouseDown()


    /// <summary>
    /// Overridden MouseUp event handler
    /// </summary>
    /// <param name="args">event data</param>
    protected override void OnMouseUp( MouseEventArgs args )
    {
      System.Windows.Forms.DataGrid.HitTestInfo hitTest = this.HitTest( args.X, args.Y );

      if( args.Button == MouseButtons.Right )
      {
        if( hitTest.Type == System.Windows.Forms.DataGrid.HitTestType.Cell )
        {
          // Show popup menu on right mouse up on an item
          if ( this.PopUpMenu != null )
          {
            // Viatronix menus use screen coordinates (which, btw, is the correct way to view a popup menu)
            this.PopUpMenu.Show(this, PointToScreen(new Point( args.X, args.Y )));
          }
        }
      }
      base.OnMouseUp( args);
    } // OnMouseUp()

    /// <summary>
    /// Overridden Previews a keyboard message 
    /// </summary>
    /// <param name="msg">windows message</param>
    /// <returns>message was processed by the control</returns>
    protected override bool ProcessKeyPreview( ref System.Windows.Forms.Message msg )
    {
      // preprocess the keyboard enter key and fire our DataGridEnterEvent
      if (msg.WParam.ToInt32() == (int) Keys.Enter) 
      {
        if (DataGridEnterEvent != null)
          DataGridEnterEvent( this, new System.EventArgs() );

        return true;
      }
      else if ( msg.WParam.ToInt32() == (int) Keys.Up )
      {
        OnKeyUp( new KeyEventArgs(Keys.Up));

        return true;
      }
      else if ( msg.WParam.ToInt32() == (int) Keys.Down )
      {
        OnKeyUp( new KeyEventArgs(Keys.Down));

        return true;
      }


      return base.ProcessKeyPreview( ref msg );
    } // ProcessKeyPreview()


    /// <summary>
    /// Preprocesses input messages within the message loop 
    /// </summary>
    /// <param name="msg">message to process</param>
    /// <returns>message was processed by the control</returns>
    public override bool PreProcessMessage( ref System.Windows.Forms.Message msg )
    {
      // preprocess the keyboard enter key and fire our DataGridEnterEvent
      if (msg.WParam.ToInt32() == (int) Keys.Enter) 
      {
        if (DataGridEnterEvent != null)
          DataGridEnterEvent( this, new System.EventArgs() );

        return true;
      }

      return false;
    } // PreProcessMessage()


    /// <summary>
    /// DataGrid delegate
    /// </summary>
    public delegate void DataGridEnterEventHandler( object sender, System.EventArgs arg );

    
    /// <summary>
    /// Returns the first selected item
    /// </summary>
    /// <returns>The first selected item, or -1 if none</returns>
    public int GetFirstSelected()
    {
      return GetNextSelected(0);
    } // GetFirstSelected()
    
    /// <summary>
    /// Returns the next selected item (index-inclusive)
    /// </summary>
    /// <param name="index">First item to search</param>
    /// <returns>The next selected item, or -1 if none</returns>
    public int GetNextSelected(int index)
    {
      for (int i=index, count = RowCount; i < count; i++)
      {
        if (IsSelected(i))
        {
          return i;
        }
      }
      return -1;
    } // GetNextSelected()


    /// <summary>
    /// Gets a list of indexes of the selected grid items.
    /// </summary>
    /// <returns>List of indexes of the selected grid items.</returns>
    public ArrayList GetSelectedList()
    {
      ArrayList selList = new ArrayList();
      for (int iSel = GetFirstSelected(); iSel >= 0; iSel = GetNextSelected(iSel+1))
      {
        selList.Add(iSel);
      }

      return selList;
    } // GetSelectedList()


    /// <summary>
    /// Sets the selected items by item index.
    /// </summary>
    /// <param name="list">List of indexes of items to be selected.</param>
    public void SetSelectedList(ArrayList list)
    {
      // Clear current selections
      for (int i=0, count = RowCount; i < count; i++)
      {
        this.UnSelect(i);
      }

      // Set new selections
      for (IEnumerator it = list.GetEnumerator(); it.MoveNext(); )
      {
        int iSel = (int) it.Current;
        if ((iSel >= 0)&&(iSel < RowCount))
          this.Select(iSel);
      }
    }
    
    #endregion 

    #region methods

    private void ProcessUpKey()
    {
      int lastRow = (this.RowCount - 1);

      if ( CurrentRowIndex == 0 )
      {
        m_lastRowIndex = -1;
      }
      else
      {
        if ( CurrentRowIndex > 0 )
        {
          UnSelect(CurrentRowIndex);
          CurrentRowIndex--;
          Select(CurrentRowIndex);
        }
      }
    }

    private void ProcessDownKey()
    {
      int lastRow = (this.RowCount - 1);

      if ( CurrentRowIndex == lastRow )
      {
        m_lastRowIndex = RowCount;
      }
      else
      {
        if ( CurrentRowIndex < lastRow )
        {
          UnSelect(CurrentRowIndex);
          CurrentRowIndex++;
          Select(CurrentRowIndex);
        }
      }
    }

    #endregion
  } // class DataGrid
} // namespace Viatronix.UI

#region Revision History
// $Log: DataGrid.cs,v $
// Revision 1.2  2005/04/15 18:35:43  romy
// Fixed Ambiguity with UI Elements
//
// Revision 1.1  2005/04/15 18:10:27  romy
// Moved from Console to v3D.GUI
//

// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/DataGrid.cs,v 1.2 2005/04/15 18:35:43 romy Exp $
// $Id: DataGrid.cs,v 1.2 2005/04/15 18:35:43 romy Exp $
#endregion

