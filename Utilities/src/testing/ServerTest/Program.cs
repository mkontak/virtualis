using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace Viatronix.Utilities.Dicom
{
  /// <summary>
  /// Program class to start main
  /// </summary>
  static class Program
  {

    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [STAThread]
    static void Main()
    {
      Application.EnableVisualStyles();
      Application.SetCompatibleTextRenderingDefault(false);
      Application.Run(new ServerTestForm());
    } // Main()

  } // class Program

} // namespace Viatronix.Utilities.Dicom
