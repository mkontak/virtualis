

using System;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Security.Permissions;
using System.Security;
using System.Threading;
using Viatronix.Utilities.Win32;


namespace Viatronix.Utilities.Win32
{

  #region delgates

  public delegate int BrowseFolderCallbackProc(IntPtr hwnd, int msg, IntPtr lParam, IntPtr lpData);

  #endregion

  #region IMalloc

  [ComImport, Guid("00000002-0000-0000-c000-000000000046"), SuppressUnmanagedCodeSecurity, InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
  public interface IMalloc
  {
    [PreserveSig]
    IntPtr Alloc(int cb);
    [PreserveSig]
    IntPtr Realloc(IntPtr pv, int cb);
    [PreserveSig]
    void Free(IntPtr pv);
    [PreserveSig]
    int GetSize(IntPtr pv);
    [PreserveSig]
    int DidAlloc(IntPtr pv);
    [PreserveSig]
    void HeapMinimize();
  }

  #endregion

  #region BROWSEINFO

  [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
  public class BROWSEINFO
  {
    public IntPtr Owner;
    public IntPtr pidlRoot;
    public IntPtr pszDisplayName;
    public string Title;
    public int Flags;
    public BrowseFolderCallbackProc callback;
    public IntPtr lParam;
    public int iImage;
  }

  #endregion

  #region CSIDL

  public class CSIDL
  {
    public const int PRINTERS = 4;
    public const int NETWORK = 0x12;
  }

  #endregion

  #region BrowseFlags

  public class BrowseFlags
  {
    public const int BIF_DEFAULT = 0x0000;
    public const int BIF_BROWSEFORCOMPUTER = 0x1000;
    public const int BIF_BROWSEFORPRINTER = 0x2000;
    public const int BIF_BROWSEINCLUDEFILES = 0x4000;
    public const int BIF_BROWSEINCLUDEURLS = 0x0080;
    public const int BIF_DONTGOBELOWDOMAIN = 0x0002;
    public const int BIF_EDITBOX = 0x0010;
    public const int BIF_NEWDIALOGSTYLE = 0x0040;
    public const int BIF_NONEWFOLDERBUTTON = 0x0200;
    public const int BIF_RETURNFSANCESTORS = 0x0008;
    public const int BIF_RETURNONLYFSDIRS = 0x0001;
    public const int BIF_SHAREABLE = 0x8000;
    public const int BIF_STATUSTEXT = 0x0004;
    public const int BIF_UAHINT = 0x0100;
    public const int BIF_VALIDATE = 0x0020;
    public const int BIF_NOTRANSLATETARGETS = 0x0400;
  }

  #endregion

  #region BrowseForFolderMessages

  public static class BrowseForFolderMessages
  {
    // messages FROM the folder browser
    public const int BFFM_INITIALIZED = 1;
    public const int BFFM_SELCHANGED = 2;
    public const int BFFM_VALIDATEFAILEDA = 3;
    public const int BFFM_VALIDATEFAILEDW = 4;
    public const int BFFM_IUNKNOWN = 5;

    // messages TO the folder browser
    public const int BFFM_SETSTATUSTEXT = 0x464;
    public const int BFFM_ENABLEOK = 0x465;
    public const int BFFM_SETSELECTIONA = 0x466;
    public const int BFFM_SETSELECTIONW = 0x467;
  }


  #endregion

}
