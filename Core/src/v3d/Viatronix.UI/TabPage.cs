// $Id: TabPage.cs,v 1.4 2005/03/08 19:11:06 frank Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos ( mailto:george@viatronix.com )

using System;
using System.Drawing;
using System.Windows.Forms;

namespace Viatronix.UI
{
  /// <summary>
  /// Represents a TabPage than can be added to a TabControl control.
  /// </summary>
	public class TabPage : System.Windows.Forms.Panel
	{

    #region fields

    /// <summary>
    /// row in which tab resides
    /// </summary>
    private int m_row = 0;

    /// <summary>
    /// column in which tab resides
    /// </summary>
    private int m_column = 0;

    /// <summary>
    /// bounds of the button
    /// </summary>
    private Rectangle m_buttonBounds = new Rectangle();

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the row in which the tab resides.
    /// </summary>
    internal int Row
    {
      get { return m_row; }
      set { m_row = value; }
    } // Row

      
    /// <summary>
    /// Gets or sets the column in which the tab resides.
    /// </summary>
    internal int Column
    {
      get { return m_column; }
      set { m_column = value; }
    } // Column

    /// <summary>
    /// Gets or sets the bounds of the button.
    /// </summary>
    internal Rectangle ButtonBounds
    {
      get { return m_buttonBounds; }
      set { m_buttonBounds = value; }
    } // ButtonBounds

    #endregion

    #region initialization
    
    /// <summary>
    /// Initializes a TabPage instance with default values.
    /// </summary>
    public TabPage()
    {
    } // TabPage()

    
    /// <summary>
    /// Initializes a TabPage instance with the specified text.
    /// </summary>
    /// <param name="text">Text to be displayed on tab.</param>
    public TabPage( string text )
    {
      this.Text = text;
    } // TabPage( string text )
    
    #endregion
	
  } // class TabPage
} // namespace Viatronix.UI

#region revision history

// $Log: TabPage.cs,v $
// Revision 1.4  2005/03/08 19:11:06  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.3.6.1  2005/03/02 13:52:20  frank
// code review
//
// Revision 1.3  2004/07/09 18:32:36  geconomos
// Merged TabInfo with TabPage.
//
// Revision 1.2  2004/05/13 13:32:31  geconomos
// Initial code cleanup.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/TabPage.cs,v 1.4 2005/03/08 19:11:06 frank Exp $
// $Id: TabPage.cs,v 1.4 2005/03/08 19:11:06 frank Exp $

#endregion