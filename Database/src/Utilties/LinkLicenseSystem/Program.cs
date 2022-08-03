using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace Viatronix.Utilities.Database
{

  static class Program
  {
    //
    [DllImport("kernel32.dll")]
    static extern bool AttachConsole(int dwProcessId);


    private const int ATTACH_PARENT_PROCESS = -1;

 
    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [STAThread]
    static int Main(string [] args)
    {


      if (args.Length == 0)
      {
        Application.EnableVisualStyles();
        Application.SetCompatibleTextRenderingDefault(false);
        Application.Run(new LinkLicenseSystem());

        return 0;
      }
      else
      {

        if ( args.Contains("-gui") )
        {
          Application.EnableVisualStyles();
          Application.SetCompatibleTextRenderingDefault(false);
          Application.Run(new LinkLicenseSystem(args));

          return 0;
        }
        else
        {
          try
          {
            LinkLicenseSystem linker = new LinkLicenseSystem(args);
            linker.Link();
            return 0;
          }
          catch (Exception ex)
          {
            AttachConsole(ATTACH_PARENT_PROCESS);
            Console.WriteLine(string.Format("{0}Link failed : {1}{2}", System.Environment.NewLine, ex.Message, System.Environment.NewLine));
            return -1;
          }
        }
      }

    }

  }

}
