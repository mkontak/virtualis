// $Id: TreeListViewEvents.cs,v 1.1 2005/09/19 11:43:58 mkontak Exp $
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

  #region event handlers

  /// <summary>
  /// TreeListViewBeforeLabelEditEventHandler delegate
  /// </summary>
  public delegate void TreeListViewBeforeLabelEditEventHandler(object sender, TreeListViewBeforeLabelEditEventArgs e);

  /// <summary>
  /// TreeListViewItemLabelEditHandler delegate
  /// </summary>
  public delegate void TreeListViewLabelEditEventHandler(object sender, TreeListViewLabelEditEventArgs e);

  /// <summary>
  /// TreeListViewCancelEventHandler delegate
  /// </summary>
  public delegate void TreeListViewCancelEventHandler(object sender, TreeListViewCancelEventArgs e);

  /// <summary>
  /// TreeListViewEventHandler delegate
  /// </summary>
  public delegate void TreeListViewEventHandler(object sender, TreeListViewEventArgs e);

  #endregion event handlers


  #region TreeListViewLabelEditEventArgs 

  /// <summary>
  /// Arguments of a TreeListViewLabelEdit event.
  /// </summary>
  [Serializable]
  public class TreeListViewLabelEditEventArgs : CancelEventArgs
  {

    #region fields

    /// <summary>
    /// Edit labl
    /// </summary>
    private string m_label;

    /// <summary>
    /// Item
    /// </summary>
    private TreeListViewItem m_item;

    /// <summary>
    /// Column index
    /// </summary>
    internal int m_columnIndex;

    #endregion fields

    #region constructors

    /// <summary>
    /// Creates a new instance of TreeListViewLabelEditEventArgs
    /// </summary>
    /// <param name="item"></param>
    /// <param name="column"></param>
    /// <param name="label"></param>
    public TreeListViewLabelEditEventArgs(TreeListViewItem item, int column, string label) : base()
    {
      m_item = item; 
      m_columnIndex = column; 
      m_label = label;
    } // TreeListViewLabelEditEventArgs

    #endregion  // Constructors

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
      get { return m_columnIndex; }
    } // ColumnIndex

    #endregion


  } // class TreeListViewLabelEditEventArgs


  #endregion TreeListViewLabelEditEventArgs


  #region TreeListViewBeforeLabelEditEventArgs

  /// <summary>
  /// Arguments of a TreeListViewBeforeLabelEdit event.
  /// </summary>
  [Serializable]
  public class TreeListViewBeforeLabelEditEventArgs : TreeListViewLabelEditEventArgs
  {

    #region fields

    /// <summary>
    /// Editor
    /// </summary>
    private Control m_editor;

    #endregion

    #region constructors

    /// <summary>
    /// Creates a new instance of TreeListViewBeforeLabelEditEventArgs
    /// </summary>
    /// <param name="item"></param>
    /// <param name="column"></param>
    /// <param name="label"></param>
    public TreeListViewBeforeLabelEditEventArgs(TreeListViewItem item, int column, string label) : base(item, column, label)
    {
    } // TreeListViewBeforeLabelEditEventArgs(TreeListViewItem item, int column, string label)

    #endregion  constructors


    #region properties

    /// <summary>
    /// Gets or sets the index of the subitem
    /// </summary>
    new public int ColumnIndex
    {
      get { return m_columnIndex; }
      set { m_columnIndex = value; }
    } // ColumnIndex

    /// <summary>
    /// Gets or sets the editor (a TextBox will be displayed if null)
    /// </summary>
    public Control Editor
    {
      get { return m_editor; }
      set { m_editor = value;}
    } // Editor

    #endregion properties


  } // class TreeListViewBeforeLabelEditEventArgs

  #endregion  TreeListViewBeforeLabelEditEventArgs

  #region TreeListViewEventArgs

  /// <summary>
  /// Arguments of a TreeListViewEvent
  /// </summary>
  [Serializable]
  public class TreeListViewEventArgs : EventArgs
  {

  
    #region fields

    /// <summary>
    /// Action
    /// </summary>
    private TreeListViewAction m_action;

    /// <summary>
    /// Item action is beging taken on
    /// </summary>
    private TreeListViewItem m_item;

    #endregion fields 

    /// <summary>
    /// Item that will be expanded
    /// </summary>
    public TreeListViewItem Item
    {
      get { return m_item; }
    } // Item

    /// <summary>
    /// Action returned by the event
    /// </summary>
    public TreeListViewAction Action
    {
      get { return m_action; }
    } // Action

    #region constructors

    /// <summary>
    /// Create a new instance of TreeListViewEvent arguments
    /// </summary>
    /// <param name="item"></param>
    /// <param name="action"></param>
    public TreeListViewEventArgs(TreeListViewItem item, TreeListViewAction action)
    {
      m_item = item;
      m_action = action;
    } // TreeListViewEventArgs

    #endregion constructors
  
  } // TreeListViewEventArgs

  #endregion TreeListViewEventArgs


  #region TreeListViewCancelEventArgs

  /// <summary>
  /// Arguments of a TreeListViewCancelEventArgs
  /// </summary>
  [Serializable]
  public class TreeListViewCancelEventArgs : TreeListViewEventArgs
  {

    #region fields

    /// <summary>
    /// Cancel flag
    /// </summary>
    private bool m_cancel = false;

    #endregion fields

    #region constructor

    /// <summary>
    /// Create a new instance of TreeListViewCancelEvent arguments
    /// </summary>
    /// <param name="item"></param>
    /// <param name="action"></param>
    public TreeListViewCancelEventArgs(TreeListViewItem item, TreeListViewAction action) :  base(item, action)
    {
    } // TreeListViewCancelEventArgs

    #endregion constructor

    #region properties

    /// <summary>
    /// True -> the operation is canceled
    /// </summary>
    public bool Cancel
    {
      get { return m_cancel; }
      set { m_cancel = value; }
    }

    #endregion properties

  }

  #endregion TreeListViewCancelEventArgs

}

#region revision history

// $Log: TreeListViewEvents.cs,v $
// Revision 1.1  2005/09/19 11:43:58  mkontak
// Coding standard and minor changes
//
// Revision 1.1  2005/05/11 11:53:50  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/TreeListView/TreeListViewEvents.cs,v 1.1 2005/09/19 11:43:58 mkontak Exp $
// $Id: TreeListViewEvents.cs,v 1.1 2005/09/19 11:43:58 mkontak Exp $

#endregion
