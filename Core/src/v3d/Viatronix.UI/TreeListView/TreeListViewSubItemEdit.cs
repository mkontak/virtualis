// $Id: TreeListViewSubItemEdit.cs,v 1.2 2005/09/19 11:43:58 mkontak Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]


using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace Viatronix.UI
{

  #region CustomEdit

  /// <summary>
  /// Implements a cutom edit control
  /// </summary>
	internal class CustomEdit : NativeWindow, IWin32Window
	{

    #region fields

    /// <summary>
    /// Tree List Voew Control Handle
    /// </summary>
    private TreeListViewItemEditControlHandle m_editorHandle;

    /// <summary>
    /// Edit item informations
    /// </summary>
    private EditItemInformation m_editInformation;

    /// <summary>
    /// Editor control
    /// </summary>
    private Control m_editor;

    #endregion

		#region properties

    /// <summary>
    /// Handle
    /// </summary>
		new public IntPtr Handle
		{
			get{return base.Handle;}
		} // Handle

		private TreeListView m_treeListView;

		#endregion

    #region constructor and destructor

		private CustomEdit() {}

		public CustomEdit(IntPtr handle, TreeListView treelistview, Control editor)
		{
			m_treeListView = treelistview;
			m_editInformation = m_treeListView.EditedItem;
			if(editor == null) m_editor = new TextBox();
			else m_editor = editor;
			m_editor.Hide();
			if(!m_treeListView.Controls.Contains(m_editor))
				m_treeListView.Controls.Add(m_editor);
			m_editorHandle = new TreeListViewItemEditControlHandle(m_treeListView, m_editor, this);
			AssignHandle(handle);
		}

		#endregion

		#region methods

    /// <summary>
    /// Shows the edit control
    /// </summary>
		public void ShowEditControl()
		{

      ///////////////////////////////////////////////////////
      // Do not show the control if the item is not in focus
      ///////////////////////////////////////////////////////
      if(m_treeListView.FocusedItem == null) 
      {
        return;
      } // END ... If the item is not in focus

			ListViewItem item = (ListViewItem) m_treeListView.EditedItem.Item;

			Rectangle rec = m_treeListView.EditedItem.ColumnIndex > 0 ?
				m_treeListView.GetSubItemRect(item.Index, m_treeListView.EditedItem.ColumnIndex) :
				m_treeListView.GetItemRect(item.Index, ItemBoundsPortion.Label);
			m_editor.Size = rec.Size;
			m_editor.Location = rec.Location;
			m_editor.Top--;
			m_editor.Show();
			m_editor.Text = item.SubItems[m_treeListView.EditedItem.ColumnIndex].Text;
			m_editor.Focus();

		} // ShowEditControl()

    /// <summary>
    /// Hides the edit control
    /// </summary>
		public void HideEditControl()
		{
			m_editor.Hide();
			ReleaseHandle();
			m_editorHandle.ReleaseHandle();
		} // HideEditControl

    #region WndProc

    /// <summary>
    /// Send window message
    /// </summary>
    /// <param name="m"></param>
    public void SendMessage(ref Message m)
    {
      WndProc(ref m);
    } 

    /// <summary>
    /// Override to the WndProc method
    /// </summary>
    /// <param name="m"></param>
    protected override void WndProc(ref Message m)
    {
      switch(m.Msg)
      {
        case (int)Win32.WindowMessages.WM_SHOWWINDOW:
          bool show = m.WParam != IntPtr.Zero;
          if(show) ShowEditControl();
          else HideEditControl();
          return;
      }
    } // WndProc()

    #endregion

		#endregion

	} // class CustomEdit


  #endregion

  #region TreeListViewItemEditControlHandle

  /// <summary>
  /// Implements a TreeListVoewEditCOntrolHandle class
  /// </summary>
	internal class TreeListViewItemEditControlHandle : NativeWindow, IWin32Window
	{

    #region fields

    /// <summary>
    /// Cutome edit 
    /// </summary>
    private CustomEdit m_customEdit;

    /// <summary>
    /// Control
    /// </summary>
    private Control m_control;

    /// <summary>
    /// Tree List View
    /// </summary>
    private TreeListView m_treeListView;


    #endregion

		#region properties

    /// <summary>
    /// Constrol handle
    /// </summary>
		new public IntPtr Handle
		{
			get{return base.Handle;}
		} // Handle

		#endregion

    #region constructor and destructor

    /// <summary>
    /// Constructor for the TreeListViewEditControlHandle
    /// </summary>
    /// <param name="treelistview"></param>
    /// <param name="control"></param>
    /// <param name="customedit"></param>
		public TreeListViewItemEditControlHandle(TreeListView treelistview, Control control, CustomEdit customedit)
		{
			m_control = control;
			m_treeListView = treelistview;
			m_customEdit = customedit;
			if(!control.Created) control.CreateControl();
			AssignHandle(control.Handle);
		} // TreeListViewItemEditControlHandle()

		#endregion

    #region methods

    /// <summary>
    /// End edit for control
    /// </summary>
    /// <param name="Cancel"></param>
		private void EndEdit(bool Cancel)
		{
			if(!m_treeListView.InEdit) return;

			m_treeListView.ExitEdit(Cancel, m_control.Text);

		} // EndEdit()

    /// <summary>
    /// On kill focus
    /// </summary>
    /// <param name="m"></param>
    /// <returns></returns>
		private bool OnKillFocus(Message m)
		{
      /////////////////////////////////////////////////////////////////////////////
			// If the control is a combobox don't end edit if the handle is a handle
			// of one of the sub controls of the combobox
      ////////////////////////////////////////////////////////////////////////////
			if(!(m_control is ComboBox)) return true;

			Win32.PCOMBOBOXINFO info = new Win32.PCOMBOBOXINFO();

			info.cbSize = (uint)System.Runtime.InteropServices.Marshal.SizeOf(typeof(Win32.PCOMBOBOXINFO));

			if( ! Win32.User32.GetComboBoxInfo(m_control.Handle, ref info)) return true;

			if(m.WParam == info.hwndCombo || m.WParam == info.hwndItem || m.WParam == info.hwndList)
			{
				ReleaseHandle();
				AssignHandle(m.WParam);
				return false;
			}

			return true;

		} // OnKillFocus()

		#region Wndproc

    /// <summary>
    /// Overrode for the WndProc messagehandler
    /// </summary>
    /// <param name="m"></param>
		protected override void WndProc(ref Message m)
		{
			switch(m.Msg)
			{
				case (int) Win32.WindowMessages.WM_KEYDOWN:
					Keys key = (Keys)(int) m.WParam;
					if(key != Keys.Return && key != Keys.Escape) break;
					bool Cancel = key != Keys.Enter;
					EndEdit(Cancel);
					return;
				case (int) Win32.WindowMessages.WM_KILLFOCUS:
					if(OnKillFocus(m))
					{
						EndEdit(!(m_control is ComboBox && m_treeListView.EditedItem.Label != m_control.Text));
						return;
					}
					break;
			}
			base.WndProc(ref m);
		} // WndProc()

    /// <summary>
    /// 
    /// </summary>
    /// <param name="Param"></param>
    /// <returns></returns>
		private int HighOrder(IntPtr Param)
		{
			int intparam = Param.ToInt32();
			return (intparam >> 16) & 0x0000ffff;
		} // HoghOrder(0

    /// <summary>
    /// Low order
    /// </summary>
    /// <param name="Param"></param>
    /// <returns></returns>
		private int LowOrder(IntPtr Param)
		{
			int intparam = Param.ToInt32();
			return intparam & 0x0000ffff;
		} /// LowOrder()

		#endregion

    #endregion

	} // class TreeListViewItemEditControlHandle

  #endregion

}

#region revision history

// $Log: TreeListViewSubItemEdit.cs,v $
// Revision 1.2  2005/09/19 11:43:58  mkontak
// Coding standard and minor changes
//
// Revision 1.1  2005/05/11 11:53:43  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/TreeListView/TreeListViewSubItemEdit.cs,v 1.2 2005/09/19 11:43:58 mkontak Exp $
// $Id: TreeListViewSubItemEdit.cs,v 1.2 2005/09/19 11:43:58 mkontak Exp $

#endregion

