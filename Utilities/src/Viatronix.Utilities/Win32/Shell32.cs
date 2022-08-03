using System;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Security.Permissions;
using System.Security;
using System.Threading;

namespace Viatronix.Utilities.Win32
{
  [SuppressUnmanagedCodeSecurity]
  static class Shell32
  {
    // Methods
    [DllImport("shell32.dll", CharSet = CharSet.Auto)]
    public static extern IntPtr SHBrowseForFolder([In] BROWSEINFO lpbi);
    [DllImport("shell32.dll")]
    public static extern int SHGetMalloc([Out, MarshalAs(UnmanagedType.LPArray)] IMalloc[] ppMalloc);
    [DllImport("shell32.dll", CharSet = CharSet.Auto)]
    public static extern bool SHGetPathFromIDList(IntPtr pidl, IntPtr pszPath);
    [DllImport("shell32.dll")]
    public static extern int SHGetSpecialFolderLocation(IntPtr hwnd, int csidl, ref IntPtr ppidl);
  }

}
