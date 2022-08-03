using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace DatabasePatch351
{
  static class Program
  {
    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [STAThread]
    static int Main(string [] args)
    {

      try
      {
        Application.EnableVisualStyles();
        Application.SetCompatibleTextRenderingDefault(false);
        Application.Run( DatabasePatcher.Create(args));

        return 0;
      }
      catch (Exception)
      {
        return 1;
      }
    }
  }
}
