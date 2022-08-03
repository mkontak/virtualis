using System;
using System.Collections.Generic;
using System.Linq;
using System.ServiceProcess;
using System.Text;

namespace Viatronix
{

  static class Program
  {

    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    static void Main()
    {

      ServiceBase[] ServicesToRun;

      FolderMonitoringService service = new FolderMonitoringService();

      ServicesToRun = new ServiceBase[] {	service };
			
      
#if !DEBUG

      ServiceBase.Run(ServicesToRun);
#else
      service.Start();
#endif


    }

  }

}
