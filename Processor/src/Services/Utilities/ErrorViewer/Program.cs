using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ErrorViewer
{
  static class Program
  {
    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [STAThread]
    static void Main(string [] args)
    {
      if (args.Length == 0)
        return;

      string errorText = args[0];
      string title = "Error";

      if ( args.Length > 1)
      {
        title = args[0];
        errorText = args[1];
      }

      Application.EnableVisualStyles();
      Application.SetCompatibleTextRenderingDefault(false);
      Application.Run(new ErrorForm(title, errorText));
    }
  }
}
