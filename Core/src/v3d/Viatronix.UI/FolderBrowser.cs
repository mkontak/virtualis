// $Id: FolderBrowser.cs,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
//
// Copyright(c) 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)

using System;
using System.ComponentModel;
using System.Windows.Forms;
using System.Windows.Forms.Design;
using System.Runtime.InteropServices;
using Win32;

namespace Viatronix.UI
{

  /// <summary>
  /// Wrapper class for the ShowFolder dialog.
  /// </summary>
  public class FolderBrowser :  Component
  {
    #region fields
    
    /// <summary>
    ///  Folder browser's text.
    /// </summary>
    private string m_title = String.Empty;
    
    /// <summary>
    /// Selected  directory.
    /// </summary>
    private string m_directoryPath;
    
    
    /// <summary>
    /// Selected display name.
    /// </summary>
    private string m_displayName;
    
    /// <summary>
    /// Browse flags.
    /// </summary>
    private BrowseFlags m_flags = BrowseFlags.BIF_DEFAULT;

    #endregion

    #region properties
    
    /// <summary>
    /// Gets or sets the title.
    /// </summary>
    public string Title
    {
      get{ return m_title; }
      set{ m_title = value;}
    } // Title

    /// <summary>
    /// Gets or sets the flags.
    /// </summary>
    public BrowseFlags Flags
    {
      get { return m_flags; }
      set{ m_flags = value; }
    } // Flags

    
    /// <summary>
    /// Gets the directory path.
    /// </summary>
    public string DirectoryPath
    {
      get{ return m_directoryPath; }
    } // DirectoryPath

    
    /// <summary>
    /// Gets the display name.
    /// </summary>
    public string DisplayName
    {
      get{ return m_displayName; }
    } // DisplayName
    
    
    #endregion

    #region methods

    /// <summary>
    /// Shows the "Browse For Folder" dialog.
    /// </summary>
    /// <returns>One of the DialogResult return codes.</returns>
    public DialogResult ShowDialog()
    {
      // allocate and set the BROWSEINFO structure
      BROWSEINFO browseInfo = new BROWSEINFO();
      browseInfo.pszDisplayName = IntPtr.Zero;
      browseInfo.lpfn = IntPtr.Zero;
      browseInfo.lParam = IntPtr.Zero;
      browseInfo.ulFlags = ( int ) m_flags;
      browseInfo.lpszTitle = m_title != String.Empty? m_title : "Select Folder";
      browseInfo.hwndOwner = System.Windows.Forms.Form.ActiveForm.Handle;
      
      IntPtr pidl = IntPtr.Zero;
      IntPtr folderPath = IntPtr.Zero;
      
      try
      {
        browseInfo.pszDisplayName = Marshal.AllocHGlobal( 256 );
        
        // call SHBrowseForFolder
        pidl = Win32.Shell32.SHBrowseForFolder( browseInfo );
        
        // check if the user cancelled out of the dialog or not.
        if( pidl == IntPtr.Zero )
          return DialogResult.Cancel;

        // allocate necssary memory buffer to receive the folder path.
        folderPath = Marshal.AllocHGlobal( 256 );
        
        // Call SHGetPathFromIDList to get folder path.
        bool bRet = Win32.Shell32.SHGetPathFromIDList( pidl, folderPath );
        
        // Convert the returned native poiner to string.
        m_directoryPath = Marshal.PtrToStringAuto( folderPath );
        
        m_displayName = Marshal.PtrToStringAuto( browseInfo.pszDisplayName );
      }
      catch( Exception  )
      {
        return DialogResult.Abort;
      } // catch( Exception e )
      finally
      {
        // Free the memory allocated by shell.
        if( pidl != IntPtr.Zero )
          Marshal.FreeHGlobal( pidl );
        
        if ( folderPath != IntPtr.Zero )
          Marshal.FreeHGlobal( folderPath );
        
        if( browseInfo.pszDisplayName != IntPtr.Zero )
          Marshal.FreeHGlobal( browseInfo.pszDisplayName );
      }
      return DialogResult.OK;
    } // SowDialog()

    #endregion
  
  } // class FolderBrowser
} // class FolderBrowser

#region revision history

// $Log: FolderBrowser.cs,v $
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.6  2003/09/05 14:58:32  michael
// merged the branch "XP1_1" back into the trunc
//
// Revision 1.5  2003/08/20 14:22:59  michael
// added "mailto:"
//
// Revision 1.4.2.1  2003/08/21 21:20:43  mkontak
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/FolderBrowser.cs,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $
// $Id: FolderBrowser.cs,v 1.1.1.1 2004/03/02 03:58:38 geconomos Exp $

#endregion
