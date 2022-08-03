using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace Viatronix.Utilities.Tests.ProcessorTest
{
  /// <summary>
  /// Holds the main entry point.
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
      Application.Run(new MainForm());
    }
    
  } // class Program
} // namespace Viatronix.Utilities.Tests.ProcessorTest
