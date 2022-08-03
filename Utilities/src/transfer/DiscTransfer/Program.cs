using System;
using System.Windows;
using System.Diagnostics;
using System.Collections.Generic;

namespace Viatronix.Utilities.Transfer.DiscTransfer
{
  /// <summary>
  /// The main program.
  /// </summary>
  internal static class Program
  {

    #region methods

    /// <summary>
    /// The main entry point into the program.
    /// </summary>
    /// <param name="args"></param>
    [STAThread]
    internal static void Main(string[] args)
    {
      Process[] proc = Process.GetProcessesByName(Process.GetCurrentProcess().ProcessName);
      if (proc.Length > 1)
      {
        MessageBox.Show("          Another instance of this program is currently running.\nIf not found in the Taskbar, please use the TaskManager to close it.", "", MessageBoxButton.OK, MessageBoxImage.Warning);
        return;
      }

      App app = new App();
      app.Run(new MainWindow());
    } // Main( args )

    #endregion

  } // class Program
} // namespace Viatronix.Utilities.Transfer.DiscTransfer
