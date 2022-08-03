// $Id: SelectedTreeListViewItemCollection.cs,v 1.1 2005/05/11 11:53:50 mkontak Exp $
//
// Copyright © 20XX, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]


using System;
using System.Windows.Forms;

namespace Viatronix.UI
{

	/// <summary>
	/// Collection of selected items in a TreeListView
	/// </summary>
	public class SelectedTreeListViewItemCollection : ListView.SelectedListViewItemCollection
	{

		#region properties

		/// <summary>
		/// Gets a TreeListViewItem at the specified index
		/// </summary>
		new public TreeListViewItem this[int index]
		{
			get{return((TreeListViewItem) base[index]);}
		} // Indexer

		#endregion

    #region constructor

		/// <summary>
		/// Create a new instance of a SelectedTreeListViewItemCollection
		/// </summary>
		/// <param name="TreeListView"></param>
		public SelectedTreeListViewItemCollection(TreeListView TreeListView) : base((ListView) TreeListView)
		{

		} // SelectedTreeListViewItemCollection()

		#endregion

		#region methods

		/// <summary>
		/// Returns true if the specified item is in the collection
		/// </summary>
		/// <param name="item"></param>
		/// <returns></returns>
		public bool Contains(TreeListViewItem item)
		{
			return(base.Contains((ListViewItem) item));
		} // Contains()

		/// <summary>
		/// Index of an item
		/// </summary>
		/// <param name="item"></param>
		/// <returns></returns>
		public int IndexOf(TreeListViewItem item)
		{
			return(base.IndexOf((ListViewItem) item));
		} // IndexOf()

		#endregion

	}
}


#region revision history

// $Log: SelectedTreeListViewItemCollection.cs,v $
// Revision 1.1  2005/05/11 11:53:50  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/TreeListView/SelectedTreeListViewItemCollection.cs,v 1.1 2005/05/11 11:53:50 mkontak Exp $
// $Id: SelectedTreeListViewItemCollection.cs,v 1.1 2005/05/11 11:53:50 mkontak Exp $

#endregion
