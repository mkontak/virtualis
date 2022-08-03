using System;
using System.Diagnostics;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace Viatronix.Utilities.SelectiveManifestGenerator
{
  /// <summary>
  /// Contains the main entry point.
  /// </summary>
  internal static class Program
  {

    #region methods
    
    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [STAThread]
    internal static void Main()
    {
      string strModName = Process.GetCurrentProcess().MainModule.ModuleName;
      string strProcName = System.IO.Path.GetFileNameWithoutExtension(strModName);

      Process[] appProc = System.Diagnostics.Process.GetProcessesByName(strProcName);
      if (appProc.Length > 1)
      {
        MessageBox.Show(Messages.GetMessage("VX001"), Messages.GetMessage("VX002"), MessageBoxButtons.OK, MessageBoxIcon.Error);

        return;
      } // if( appProc.Length > 1 )

      try
      {
        MiniLogger.Initialize();
      }
      catch (Exception e)
      {
        MessageBox.Show(string.Format(Messages.GetMessage("VX003"), e.Message), Messages.GetMessage("VX004"), MessageBoxButtons.OK, MessageBoxIcon.Error);
        return;
      }

      Application.EnableVisualStyles();
      Application.SetCompatibleTextRenderingDefault(false);
      Application.Run(new MainForm());
    } // Main()

    #endregion

  } // class Program
} // namespace Viatronix.Utilities.SelectiveManifestGenerator
