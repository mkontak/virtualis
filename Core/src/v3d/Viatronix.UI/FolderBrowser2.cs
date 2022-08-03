// $Id: FolderBrowser2.cs,v 1.2 2005/03/08 19:11:06 frank Exp $
//
// Copyright(c) 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: George Economos (george@viatronix.com)

using System;
using System.ComponentModel;
using System.Windows.Forms;
using System.Windows.Forms.Design;
using System.Runtime.InteropServices;
using Win32;

namespace Viatronix.UI
{

  /// <summary>
  /// Wrapper for folder browser withing the folder name editor object
  /// </summary>
  public class FolderBrowser2 : FolderNameEditor
  {
    
    #region fields

    /// <summary>
    /// Folder browser (Protected member of FolderNameEditor object)
    /// </summary>
    private FolderNameEditor.FolderBrowser m_browser = null;

    /// <summary>
    /// Title for folder browser
    /// </summary>
    private string m_title;

    #endregion

    #region initialization

    /// <summary>
    /// Constructor
    /// </summary>
    public FolderBrowser2()
    {
      m_title = "Select folder";
      m_browser = new FolderNameEditor.FolderBrowser();
      m_browser.Style = FolderNameEditor.FolderBrowserStyles.ShowTextBox;
    } // FolderBrowser2()

    #endregion

    #region properties

    /// <summary>
    /// Set/Gets the title of the folder browser
    /// </summary>
    public string Title
    {
      get { return m_title; }
      set { m_title = value; }
    } // Title


    /// <summary>
    /// Returns the directory path
    /// </summary>
    public string DirectoryPath
    {
      get{return this.m_browser.DirectoryPath;}
    } // DirectoryPath

    #endregion

    #region methods

    /// <summary>
    /// Show the folder browser
    /// </summary>
    /// <returns></returns>
    public DialogResult ShowDialog()
    {
      m_browser.Description = m_title;
      return m_browser.ShowDialog();
    } // ShowDialog()

    #endregion

  } // class FolderBrowser2	  
} // namespace Viatronix.UI

#region revision history

// $Log: FolderBrowser2.cs,v $
// Revision 1.2  2005/03/08 19:11:06  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.1.1.1.6.1  2005/03/02 16:50:34  gdavidson
// Code review
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2003/09/05 14:58:32  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 1.1.2.1  2003/08/21 21:20:43  mkontak
// Created a new folder browser
//
// Revision 1.4  2003/05/25 18:12:07  michael
// .net --> .com
//
// Revision 1.3  2003/04/25 21:06:44  geconomos
// Endless code reviews.
//
// Revision 1.2  2003/04/17 19:41:05  geconomos
// Some more code cleanup.
//
// Revision 1.1  2003/01/09 19:00:49  geconomos
// Folder browser move from xp project.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/FolderBrowser2.cs,v 1.2 2005/03/08 19:11:06 frank Exp $
// $Id: FolderBrowser2.cs,v 1.2 2005/03/08 19:11:06 frank Exp $

#endregion
