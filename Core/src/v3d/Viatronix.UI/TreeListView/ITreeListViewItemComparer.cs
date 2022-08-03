

// $Id: ITreeListViewItemComparer.cs,v 1.1 2005/05/11 11:53:50 mkontak Exp $
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
	/// Interface ITreeListViewItemComparer
	/// </summary>
	public interface ITreeListViewItemComparer : System.Collections.IComparer
	{

    #region properties

		/// <summary>
		/// Sort order
		/// </summary>
		SortOrder SortOrder
		{
			get;
			set;
		}
		/// <summary>
		/// Column for the comparison
		/// </summary>
		int Column
		{
			get;
			set;
		}

    #endregion

	} // interface ITreeListViewComparer

}


#region revision history

// $Log: ITreeListViewItemComparer.cs,v $
// Revision 1.1  2005/05/11 11:53:50  mkontak
// Initial checkin
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/TreeListView/ITreeListViewItemComparer.cs,v 1.1 2005/05/11 11:53:50 mkontak Exp $
// $Id: ITreeListViewItemComparer.cs,v 1.1 2005/05/11 11:53:50 mkontak Exp $

#endregion
