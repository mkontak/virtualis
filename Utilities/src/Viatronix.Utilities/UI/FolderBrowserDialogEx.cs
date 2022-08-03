

using System;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Security.Permissions;
using System.Security;
using System.Threading;
using Viatronix.Utilities.Win32;

namespace Viatronix.Utilities.UI
{


  ///
  public class FolderBrowserDialogEx : System.Windows.Forms.CommonDialog
  {
  
    #region fields

    private static readonly int MAX_PATH = 260;

    private BrowseFolderCallbackProc m_callback;
    private string m_descriptionText;
    private Environment.SpecialFolder m_rootFolder;
    private string m_selectedPath;
    private bool m_selectedPathNeedsCheck;
    private bool m_showNewFolderButton;
    private bool m_showEditBox;
    private bool m_showBothFilesAndFolders;
    private bool m_newStyle = true;
    private bool m_showFullPathInEditBox = true;
    private bool m_dontIncludeNetworkFoldersBelowDomainLevel;
    private int m_uiFlags;
    private IntPtr m_hwndEdit;
    private IntPtr m_rootFolderLocation;

    #endregion

    #region events 

    /// <summary>
    /// HelperRequest event
    /// </summary>
    public new event EventHandler HelpRequest
    {
      add     { base.HelpRequest += value; }
      remove  { base.HelpRequest -= value; }
    } // HelpRequest

    #endregion

    #region contructors

    /// <summary>
    /// Constructors
    /// </summary>
    public FolderBrowserDialogEx()
    {
      Reset();
    } // FolderBrowserDialogEx()

    #endregion


    #region methods

 
    /// <summary>
    /// Creates a printer browser
    /// </summary>
    /// <returns>Returns the new dialog</returns>
    public static FolderBrowserDialogEx PrinterBrowser()
    {
      FolderBrowserDialogEx folderBorowser = new FolderBrowserDialogEx();
      // avoid MBRO comppiler warning when passing m_rootFolderLocation as a ref:
      folderBorowser.BecomePrinterBrowser();
      return folderBorowser;
    } // FolderBrowserDialogEx PrinterBrowser()

    /// <summary>
    /// Creates a new computer folder dialog
    /// </summary>
    /// <returns>Newly create </returns>
    public static FolderBrowserDialogEx ComputerBrowser()
    {
      FolderBrowserDialogEx folderBorowser = new FolderBrowserDialogEx();
	      // avoid MBRO comppiler warning when passing m_rootFolderLocation as a ref:
      folderBorowser.BecomeComputerBrowser();
      return folderBorowser;
    } // ComputerBrowser()


	  // Helpers
	  private void BecomePrinterBrowser()
	  {
      m_uiFlags += BrowseFlags.BIF_BROWSEFORPRINTER;
      Description = "Select a printer:";
      Shell32.SHGetSpecialFolderLocation(IntPtr.Zero, CSIDL.PRINTERS, ref this.m_rootFolderLocation);
      ShowNewFolderButton = false;
      ShowEditBox = false;
	  }       

	  private void BecomeComputerBrowser()
	  {
      m_uiFlags += BrowseFlags.BIF_BROWSEFORCOMPUTER;
      Description = "Select a computer:";
      Shell32.SHGetSpecialFolderLocation(IntPtr.Zero, CSIDL.NETWORK, ref this.m_rootFolderLocation);
      ShowNewFolderButton = false;
      ShowEditBox = false;
    }


 
    /// <summary>
    /// Callback function
    /// </summary>
    /// <param name="hwnd"></param>
    /// <param name="msg"></param>
    /// <param name="lParam"></param>
    /// <param name="lpData"></param>
    /// <returns></returns>
    private int FolderBrowserCallback(IntPtr hwnd, int msg, IntPtr lParam, IntPtr lpData)
    {
      switch (msg)
      {
      case BrowseForFolderMessages.BFFM_INITIALIZED:
          if (m_selectedPath.Length != 0)
          {
            User32.SendMessage(new HandleRef(null, hwnd), BrowseForFolderMessages.BFFM_SETSELECTIONW, 1, m_selectedPath);
            if (this.m_showEditBox && this.m_showFullPathInEditBox)
            {
              // get handle to the Edit box inside the Folder Browser Dialog
              m_hwndEdit = User32.FindWindowEx(new HandleRef(null, hwnd), IntPtr.Zero, "Edit", null);
              User32.SetWindowText(m_hwndEdit, m_selectedPath);
            }
          }
          break;

      case BrowseForFolderMessages.BFFM_SELCHANGED:
          IntPtr pidl = lParam;
          if (pidl != IntPtr.Zero)
          {
            if (((m_uiFlags & BrowseFlags.BIF_BROWSEFORPRINTER) == BrowseFlags.BIF_BROWSEFORPRINTER) ||
                ((m_uiFlags & BrowseFlags.BIF_BROWSEFORCOMPUTER) == BrowseFlags.BIF_BROWSEFORCOMPUTER))
            {
              // we're browsing for a printer or computer, enable the OK button unconditionally.
              User32.SendMessage(new HandleRef(null, hwnd), BrowseForFolderMessages.BFFM_ENABLEOK, 0, 1);
            }
            else
            {
              IntPtr pszPath = Marshal.AllocHGlobal(MAX_PATH * Marshal.SystemDefaultCharSize);
              bool haveValidPath = Shell32.SHGetPathFromIDList(pidl, pszPath);
              String displayedPath = Marshal.PtrToStringAuto(pszPath);
              Marshal.FreeHGlobal(pszPath);
              // whether to enable the OK button or not. (if file is valid)
              User32.SendMessage(new HandleRef(null, hwnd), BrowseForFolderMessages.BFFM_ENABLEOK, 0, haveValidPath ? 1 : 0);

              // Maybe set the Edit Box text to the Full Folder path
              if (haveValidPath && !String.IsNullOrEmpty(displayedPath))
              {
                if (m_showEditBox && m_showFullPathInEditBox)
                {
                  if (m_hwndEdit != IntPtr.Zero)
                    User32.SetWindowText(m_hwndEdit, displayedPath);
                }

                if ((m_uiFlags & BrowseFlags.BIF_STATUSTEXT) == BrowseFlags.BIF_STATUSTEXT)
                    User32.SendMessage(new HandleRef(null, hwnd), BrowseForFolderMessages.BFFM_SETSTATUSTEXT, 0, displayedPath);
              }
            }
          }
          break;
      }
      return 0;
    } // FolderBrowserCallback(IntPtr hwnd, int msg, IntPtr lParam, IntPtr lpData)
   
    /// <summary>
    /// Performs the ShGetMalloc
    /// </summary>
    /// <returns></returns>
    private static IMalloc GetSHMalloc()
    {
      IMalloc[] ppMalloc = new IMalloc[1];
      Shell32.SHGetMalloc(ppMalloc);
      return ppMalloc[0];
    } // GetSHMalloc()

    /// <summary>
    /// Resets flags
    /// </summary>
    public override void Reset()
    {
      m_rootFolder = (Environment.SpecialFolder)0;
      m_descriptionText = string.Empty;
      m_selectedPath = string.Empty;
      m_selectedPathNeedsCheck = false;
      m_showNewFolderButton = true;
      m_showEditBox = true;
      m_newStyle = true;
      m_dontIncludeNetworkFoldersBelowDomainLevel = false;
      m_hwndEdit = IntPtr.Zero;
      m_uiFlags = 0;
      m_rootFolderLocation = IntPtr.Zero;
    } // Reset()

    /// <summary>
    /// Runs the dialog
    /// </summary>
    /// <param name="hWndOwner">Window Ownerss Handle</param>
    /// <returns></returns>
    protected override bool RunDialog(IntPtr hWndOwner)
    {
      bool result = false;
      if (m_rootFolderLocation == IntPtr.Zero)
      {
        Shell32.SHGetSpecialFolderLocation(hWndOwner, (int)this.m_rootFolder, ref m_rootFolderLocation);
        if (m_rootFolderLocation == IntPtr.Zero)
        {
          Shell32.SHGetSpecialFolderLocation(hWndOwner, 0, ref m_rootFolderLocation);
          if (m_rootFolderLocation == IntPtr.Zero)
          {
            throw new InvalidOperationException("FolderBrowserDialogNoRootFolder");
          }
        }
      } // END ... If the folder location contains nothing

      m_hwndEdit = IntPtr.Zero;

      if (m_dontIncludeNetworkFoldersBelowDomainLevel)
        m_uiFlags += BrowseFlags.BIF_DONTGOBELOWDOMAIN;
      if (this.m_newStyle)
        m_uiFlags += BrowseFlags.BIF_NEWDIALOGSTYLE;
      if (!this.m_showNewFolderButton)
        m_uiFlags += BrowseFlags.BIF_NONEWFOLDERBUTTON;
      if (this.m_showEditBox)
        m_uiFlags += BrowseFlags.BIF_EDITBOX;
      if (this.m_showBothFilesAndFolders)
        m_uiFlags += BrowseFlags.BIF_BROWSEINCLUDEFILES;


      if (Control.CheckForIllegalCrossThreadCalls && (Application.OleRequired() != ApartmentState.STA))
      {
        throw new ThreadStateException("DebuggingException: ThreadMustBeSTA");
      }

      IntPtr pidl = IntPtr.Zero;
      IntPtr hglobal = IntPtr.Zero;
      IntPtr pszPath = IntPtr.Zero;

      try
      {
        BROWSEINFO browseInfo = new BROWSEINFO();
        hglobal = Marshal.AllocHGlobal(MAX_PATH * Marshal.SystemDefaultCharSize);
        pszPath = Marshal.AllocHGlobal(MAX_PATH * Marshal.SystemDefaultCharSize);
        m_callback = new BrowseFolderCallbackProc(this.FolderBrowserCallback);
        browseInfo.pidlRoot = m_rootFolderLocation;
        browseInfo.Owner = hWndOwner;
        browseInfo.pszDisplayName = hglobal;
        browseInfo.Title = this.m_descriptionText;
        browseInfo.Flags = m_uiFlags;
        browseInfo.callback = m_callback;
        browseInfo.lParam = IntPtr.Zero;
        browseInfo.iImage = 0;
        pidl = Shell32.SHBrowseForFolder(browseInfo);
        if (((m_uiFlags & BrowseFlags.BIF_BROWSEFORPRINTER) == BrowseFlags.BIF_BROWSEFORPRINTER) ||
        ((m_uiFlags & BrowseFlags.BIF_BROWSEFORCOMPUTER) == BrowseFlags.BIF_BROWSEFORCOMPUTER))
        {
          this.m_selectedPath = Marshal.PtrToStringAuto(browseInfo.pszDisplayName);
          result = true;
        }
        else
        {
          if (pidl != IntPtr.Zero)
          {
            Shell32.SHGetPathFromIDList(pidl, pszPath);
            this.m_selectedPathNeedsCheck = true;
            this.m_selectedPath = Marshal.PtrToStringAuto(pszPath);
            result = true;
          }
        }
      }
      finally
      {
        IMalloc sHMalloc = GetSHMalloc();
        sHMalloc.Free(m_rootFolderLocation);

        m_rootFolderLocation = IntPtr.Zero;

        if (pidl != IntPtr.Zero)
           sHMalloc.Free(pidl);
     
        if (pszPath != IntPtr.Zero)
          Marshal.FreeHGlobal(pszPath);
       
        if (hglobal != IntPtr.Zero)
          Marshal.FreeHGlobal(hglobal);
        

        m_callback = null;
      }

      return result;

    } // RunDialog()

    #endregion

    #region properties

    /// <summary>
    /// Sets/Gets the description
    /// </summary>
    public string Description
    {
      get { return m_descriptionText; }
      set { m_descriptionText = (value == null) ? string.Empty : value; }
    } // Description

    /// <summary>
    /// Sets/Gets the root folder
    /// </summary>
    public Environment.SpecialFolder RootFolder
    {
      get { return this.m_rootFolder; }
      set
      {
        if ( !Enum.IsDefined(typeof(Environment.SpecialFolder), value) )
        {
            throw new InvalidEnumArgumentException("value", (int)value, typeof(Environment.SpecialFolder));
        }
        m_rootFolder = value;
      }
    } // RootFolder

 
    /// <summary>
    /// Set/Gets the SelectedPath 
    /// </summary>
    public string SelectedPath
    {
      get
      {
        if (( (m_selectedPath != null) && (m_selectedPath.Length != 0) ) && m_selectedPathNeedsCheck )
        {
            new FileIOPermission(FileIOPermissionAccess.PathDiscovery, m_selectedPath).Demand();
            m_selectedPathNeedsCheck = false;
        }
        return m_selectedPath;
      }
      set
      {
        m_selectedPath = (value == null) ? string.Empty : value;
        m_selectedPathNeedsCheck = true;
      }
    } // SelectedPath

 
    /// <summary>
    /// Sets/Gets the ShowNewFolderButton flag
    /// </summary>
    public bool ShowNewFolderButton
    {
      get { return this.m_showNewFolderButton; }
      set { m_showNewFolderButton = value; }
    } // ShowNewFolderButton

    /// <summary>
    /// Sets/Gets the ShowEditBox flag
    /// </summary>
    public bool ShowEditBox
    {
      get { return this.m_showEditBox; }
      set { m_showEditBox = value; }
    } // ShowEditBox

    /// <summary>
    /// Set whether to use the New Folder Browser dialog style.
    /// </summary>
    /// <remarks>
    /// The new style is resizable and includes a "New Folder" button.
    /// </remarks>
    public bool NewStyle
    {
      get { return this.m_newStyle;}
      set { m_newStyle = value;}
    } // NewStyle


    /// <summary>
    /// Sets/Gets the DontIncludeNetworkFoldersBelowDomainLevel flag
    /// </summary>
    public bool DontIncludeNetworkFoldersBelowDomainLevel
    {
      get { return m_dontIncludeNetworkFoldersBelowDomainLevel; }
      set { m_dontIncludeNetworkFoldersBelowDomainLevel = value; }
    } // DontIncludeNetworkFoldersBelowDomainLevel

    /// <summary>
    /// Sets/Gets the ShowFullPathInEditBox flag
    /// </summary>
    public bool ShowFullPathInEditBox
    {
      get { return m_showFullPathInEditBox; }
      set { m_showFullPathInEditBox = value; }
    } // ShowFullPathInEditBox

    /// <summary>
    /// Gets/Sets the ShowBothFilesAndFolders flag
    /// </summary>
    public bool ShowBothFilesAndFolders
    {
      get { return m_showBothFilesAndFolders; }
      set { m_showBothFilesAndFolders = value; }
    } // ShowBothFilesAndFolders

  
    #endregion

  } // class FolderBrowserDialogEx




}
