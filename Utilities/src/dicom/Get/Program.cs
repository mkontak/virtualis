using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Configuration;
using Viatronix.Dicom;

namespace Push
{
  class Program
  {

    #region fields

    /// <summary>
    /// Error code of 0 indicates no error
    /// </summary>
    private static int m_errorCode = 0;

    /// <summary>
    /// Completed wait handle
    /// </summary>
    private static System.Threading.ManualResetEvent m_completeEvent = new System.Threading.ManualResetEvent(false);

    /// <summary>
    /// Starting time
    /// </summary>
    private static DateTime m_start;

    #endregion

    /// <summary>
    /// Main entry point
    /// </summary>
    /// <param name="args"></param>
    /// <returns></returns>
    static int Main(string[] args)
    {

      string medicalId = string.Empty;
      string patientName = string.Empty;
      string studyUid = string.Empty;
      string seriesUid = string.Empty;
      string instanceUid = string.Empty;
      string destination = string.Empty;
      string host = string.Empty;
      string logFile = string.Empty;
      string level = "SERIES";
      int destinationPort = -1;

      bool wait = (ConfigurationManager.AppSettings["wait"] != null ? Boolean.Parse(ConfigurationManager.AppSettings["wait"]) : false);
      string description = "Performa a GET by sending a DICOM C-MOVE command to the server specified. The server will send the images to the specified destination. The destination aetitle must be known by the server.";

      try
      {

        // Initialize the logger
        Viatronix.Logging.Log.Initialize();


        XmlNode clientNode = (XmlNode)System.Configuration.ConfigurationManager.GetSection("client");

        Viatronix.Dicom.CommandLineParameters parameters = CommandLineParameters.Create("Get.exe", description, clientNode);

        parameters.Verbose = (ConfigurationManager.AppSettings["verbose"] != null ? Boolean.Parse(ConfigurationManager.AppSettings["verbose"]) : parameters.Verbose);
        wait = parameters.Wait;
        logFile = (ConfigurationManager.AppSettings["log"] != null ? ConfigurationManager.AppSettings["log"] : string.Empty);


        parameters.Update(args);

        if (parameters.TryGetParameter<string>("-d", ref destination))
          throw new System.Exception("Missing destination");

        if (parameters.TryGetParameter<int>("-dp", ref destinationPort))
          throw new System.Exception("Missing destination port");


        parameters.TryGetParameter<string>("-log", ref logFile);
        parameters.TryGetParameter<string>("-study", ref studyUid);
        parameters.TryGetParameter<string>("-series", ref seriesUid);
        parameters.TryGetParameter<string>("-instance", ref instanceUid);
        parameters.TryGetParameter<string>("-level", ref level);
        parameters.TryGetParameter<string>("-patientId", ref medicalId);
        parameters.TryGetParameter<string>("-medicalId", ref medicalId);
        parameters.TryGetParameter<string>("-patientName", ref patientName);


        parameters.Validate();

       
        if (  string.IsNullOrEmpty(medicalId) && string.IsNullOrEmpty(patientName) && 
              string.IsNullOrEmpty(studyUid) && string.IsNullOrEmpty(seriesUid) && string.IsNullOrEmpty(instanceUid)) 
          throw new System.Exception("Missing search parameters");
         

        /// Create the move based on the input
        CGet get = CGet.Create(parameters.AETitle, parameters.Server);

        /// Setup event handlers
        get.Started += new ClientEventHandler(Get_OnStarted);
        get.Aborted += new ClientEventHandler(Get_OnAborted);
        get.Failed += new ClientEventHandler(Get_OnFailed);
        get.Progress += new ClientEventHandler(Get_OnProgress);
        get.Completed += new ClientEventHandler(Get_OnCompleted);


        // =======================================================================================
        // Perform the PUSH based on the arguments UIDs take precedence over patient retrievals.
        // =========================================================================================
        if ( ! string.IsNullOrEmpty(studyUid) || ! string.IsNullOrEmpty(seriesUid) || ! string.IsNullOrEmpty(instanceUid) )
          get.Add(destination, destinationPort, studyUid, seriesUid, instanceUid);
        else
          get.Add(destination, destinationPort, medicalId, patientName);


        m_start = DateTime.Now;

        get.Start();

        get.Wait();

        /// Give time for cleanup
        System.Threading.Thread.Sleep(500);

      }
      catch (DicomException exp)
      {
        m_errorCode = -1;
        WriteError("Get failed : " + exp.Message);
      }
      catch (Exception ex)
      {
        m_errorCode = -1;
        WriteError("Get failed : " + ex.Message);
      }

      //==================================================================================
      // If an enter to cointinue is true then wait for the user to enter before exiting
      //===================================================================================
      if (wait)
      {
        Console.Write(System.Environment.NewLine + "Press enter to continue ");
        Console.ReadLine();
      }


      /// Returns the error code
      return m_errorCode;


    } // Main()


    /// <summary>
    /// Write the error to the log and console
    /// </summary>
    /// <param name="?"></param>
    static void WriteError(string  message)
    {
      Viatronix.Logging.Log.Error(message, "Get", "WriteError");
      Console.WriteLine(message);
    } // WriteError(string message)




    #region event handlers

    static void Get_OnStarted(object sender, ClientEventArgs arg)
    {
      Console.WriteLine("C-GET Started ");
      Console.WriteLine("");
      Console.WriteLine("AE Title:         " + arg.CallingAETitle);
      Console.WriteLine("Called AE Title:  " + arg.CalledAETitle);
      Console.WriteLine("Server:           " + arg.ServerName);
      Console.WriteLine("Remaining:        " + arg.Remaining.ToString());
      Console.WriteLine("");

    }

    static void Get_OnProgress(object sender, Viatronix.Dicom.ClientEventArgs arg)
    {
      Console.WriteLine(string.Format("Get image {0} of {1}", arg.Completed, (arg.Completed + arg.Remaining)));
    }

    static void Get_OnFailed(object sender, Viatronix.Dicom.ClientEventArgs arg)
    {
      m_errorCode = 1;
      Console.WriteLine("");
      Console.Error.WriteLine(string.Format("C-GET Failed ({0}/{1}) : {2}", arg.Remaining, arg.Completed, arg.Message));
      m_completeEvent.Set();

    }

    static void Get_OnCompleted(object sender, Viatronix.Dicom.ClientEventArgs arg)
    {
      Console.WriteLine("");
      Console.WriteLine(string.Format("C-GET Completed : It took {0} seconds to push {1} images", DateTime.Now.Subtract(m_start).TotalSeconds , arg.Completed.ToString()) );

      m_completeEvent.Set();
    }

    static void Get_OnAborted(object sender, Viatronix.Dicom.ClientEventArgs arg)
    {
      Console.WriteLine("");
      Console.WriteLine(string.Format("C-GET Aborted ({0}/{1}) : {2}", arg.Remaining, arg.Completed, arg.Message));
      m_completeEvent.Set();

    }

    #endregion

  }
}
