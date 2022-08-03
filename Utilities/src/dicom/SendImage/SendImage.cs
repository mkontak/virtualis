using System;
using System.Configuration;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using Viatronix.Dicom;
using Viatronix.Logging;


namespace Viatronix.Utilities.Dicom
{

  /// <summary>
  /// Class to send images to a dicom server
  /// </summary>
  class SendImage
  {

    #region fields

    /// <summary>
    /// Error code of 0 indicates no error
    /// </summary>
    private static int m_errorCode = 0;

    /// <summary>
    /// Parameters passed in on the command line
    /// </summary>
    private static Parameters m_parameters = null;

    /// <summary>
    /// Completed wait handle
    /// </summary>
    private static System.Threading.ManualResetEvent m_completeEvent = new System.Threading.ManualResetEvent(false);
   
    #endregion


    #region methods

    static int Main(string[] args)
    {

      try
      {

        XmlNode clientNode = (XmlNode)System.Configuration.ConfigurationManager.GetSection("client");

        // Create the client parameters from the app.config's xml (These are the defaults)
        m_parameters = Parameters.Create(clientNode);

        m_parameters.Verbose = (ConfigurationManager.AppSettings["verbose"] != null ? Boolean.Parse(ConfigurationManager.AppSettings["verbose"]) : m_parameters.Verbose);
        m_parameters.Wait = (ConfigurationManager.AppSettings["wait"] != null ? Boolean.Parse(ConfigurationManager.AppSettings["wait"]) : m_parameters.Wait);


        /// Creates all the logs in the section
        Viatronix.Logging.Log.Initialize();


        // Override the defaults with command line arguments
        m_parameters.Update(args);



        Viatronix.Dicom.CStore store = Viatronix.Dicom.CStore.Create(m_parameters.AETitle, m_parameters.Server.ToXml());

        store.LogSource = "SendImage";

        Console.WriteLine("C-STORE Started ");
        Console.WriteLine("");
        Console.WriteLine("AE Title:         " + store.AETitle);
        Console.WriteLine("Called AE Title:  " + store.Server.AETitle);
        Console.WriteLine("Server:           " + store.Server.Server);
        Console.WriteLine("Port:             " + store.Server.Port.ToString());


        store.Started   += new ClientEventHandler(Store_OnStarted);
        store.Aborted   += new ClientEventHandler(Store_OnAborted);
        store.Failed    += new ClientEventHandler(Store_OnFailed);
        store.Progress  += new ClientEventHandler(Store_OnProgress);
        store.Completed += new ClientEventHandler(Store_OnCompleted);

        // ==========================
        // Add al files to the store
        // ==========================
        foreach (string file in m_parameters.Files)
        {
          store.AddFile(file);
        }


        /// Start the store
        store.Start();

        /// Wait for it to complete
        store.Wait();

        // Give some time for cleanup
        System.Threading.Thread.Sleep(500);


      }
      catch (DicomException exp)
      {
        m_errorCode = -1;
        Console.WriteLine("SendImage failed : " + exp.Message);
      }
      catch (Exception ex)
      {
        m_errorCode = -1;
        Console.WriteLine("SendImage failed : " + ex.Message);
      }

      //==================================================================================
      // If an enter to continue is true then wait for the user to enter before exiting
      //===================================================================================
      if (m_parameters.Wait)
        Console.ReadLine();


      /// Returns the error code
      return m_errorCode;

     }

    #region event handlers

    static void Store_OnStarted(object sender, ClientEventArgs arg)
    {
      Console.WriteLine("Remaining:        " + arg.Remaining.ToString());
      Console.WriteLine("");
         
    }

    static void Store_OnProgress(object sender, Viatronix.Dicom.ClientEventArgs arg)
    {
      //======================================================
      // Only display image sending progress if we are verbose
      //======================================================
      if ( m_parameters.Verbose )
        Console.WriteLine(string.Format("Sent image {0} of {1} ... {2}", arg.Completed, (arg.Completed + arg.Remaining), arg.Message));
    }

    static void Store_OnFailed(object sender, Viatronix.Dicom.ClientEventArgs arg)
    {
      m_errorCode = 1;
      Console.WriteLine("");
      Console.Error.WriteLine(string.Format("C-STORE Failed ({0}/{1}) : {2}",arg.Remaining, arg.Completed,arg.Message ));
      m_completeEvent.Set();

    }

    static void Store_OnCompleted(object sender, Viatronix.Dicom.ClientEventArgs arg)
    {
      Console.WriteLine("");
      Console.WriteLine("C-STORE Completed " + arg.Completed.ToString() + " images sent");

      m_completeEvent.Set();
    }

    static void Store_OnAborted(object sender, Viatronix.Dicom.ClientEventArgs arg)
    {
      Console.WriteLine("");
      Console.WriteLine(string.Format("C-STORE Aborted ({0}/{1}) : {2}",arg.Remaining, arg.Completed,arg.Message ));
      m_completeEvent.Set();

    }

    #endregion


    #endregion

  } // class Program

}
