using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Viatronix.Dicom;
using System.Xml;
using System.Configuration;

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

      string level = "SERIES";                    // -l
      string medicalId = string.Empty;            // -m
      string patientName = string.Empty;          // -n
      string modality = string.Empty;             // -mod
      string studyUid = string.Empty;             // -study
      string seriesUid = string.Empty;            // -series
      string instanceUid = string.Empty;          // -instance
      string physician = string.Empty;            // -phy
      string studyDate = string.Empty;            // -std
      string studyTime = string.Empty;            // -stt
      string seriesDate = string.Empty;           // -sed
      string seriesTime = string.Empty;           // -set
      string dob = string.Empty;                  // -dob
      string tob = string.Empty;                  // -tob
      string studyId = string.Empty;              // -id
      string blocking = string.Empty;             // -b 
      string logFile = string.Empty;
      string destination = string.Empty;
      bool wait = (ConfigurationManager.AppSettings["wait"] != null ? Boolean.Parse(ConfigurationManager.AppSettings["wait"]) : false);
      string description = "Sends a DICOM C-MOVE command to the server specified. The server will send the images to the specified destination. The destination aetitle must be known by the server.";

      try
      {

 
        // Initialize the logger
        Viatronix.Logging.Log.Initialize();


        XmlNode clientNode = (XmlNode)System.Configuration.ConfigurationManager.GetSection("client");

        Viatronix.Dicom.CommandLineParameters parameters = CommandLineParameters.Create("Push.exe", description, clientNode);

        parameters.Verbose = (ConfigurationManager.AppSettings["verbose"] != null ? Boolean.Parse(ConfigurationManager.AppSettings["verbose"]) : parameters.Verbose);
        logFile = (ConfigurationManager.AppSettings["log"] != null ? ConfigurationManager.AppSettings["log"] : string.Empty);
        destination = (ConfigurationManager.AppSettings["destination"] != null ? ConfigurationManager.AppSettings["destination"] : string.Empty);
        blocking = (ConfigurationManager.AppSettings["blocking"] != null ? ConfigurationManager.AppSettings["blocking"] : string.Empty);

        parameters.Update(args);

        wait = parameters.Wait;


        parameters.TryGetParameter<string>("-d", ref destination);


        // Make sure there is a destination
        if ( string.IsNullOrEmpty(destination ) )
          throw new System.Exception("Missing destination");

        parameters.TryGetParameter<string>("-log", ref logFile);
        parameters.TryGetParameter<string>("-study", ref studyUid);
        parameters.TryGetParameter<string>("-series", ref seriesUid);
        parameters.TryGetParameter<string>("-instance", ref instanceUid);
        parameters.TryGetParameter<string>("-level", ref level);
        parameters.TryGetParameter<string>("-studyId", ref studyId);
        parameters.TryGetParameter<string>("-mod", ref modality);
        parameters.TryGetParameter<string>("-physician", ref physician);
        parameters.TryGetParameter<string>("-studyDate", ref studyDate);
        parameters.TryGetParameter<string>("-studyTime", ref studyTime);
        parameters.TryGetParameter<string>("-seriesDate", ref seriesDate);
        parameters.TryGetParameter<string>("-seriesTime", ref seriesTime);
        parameters.TryGetParameter<string>("-dob", ref dob);
        parameters.TryGetParameter<string>("-tob", ref tob);
        parameters.TryGetParameter<string>("-patientId", ref medicalId);
        parameters.TryGetParameter<string>("-medicalId", ref medicalId);
        parameters.TryGetParameter<string>("-patientName", ref patientName);
        parameters.TryGetParameter<string>("-blocking", ref blocking);

        parameters.Validate();


       
        if (  string.IsNullOrEmpty(medicalId) && string.IsNullOrEmpty(patientName) && 
              string.IsNullOrEmpty(studyUid) && string.IsNullOrEmpty(seriesUid) && string.IsNullOrEmpty(instanceUid)) 
          throw new System.Exception("Missing search parameters");



        /// Create the move based on the input
        CMove move = CMove.Create(parameters.AETitle, parameters.Server.ToXml());

        if (string.Compare(blocking, "false", true) == 0)
          move.BlockingMode = BlockingModes.NONBLOCKING;
        else
          move.BlockingMode = BlockingModes.BLOCKING;

        /// Setup event handlers
        move.Started    += new ClientEventHandler(Move_OnStarted);
        move.Aborted    += new ClientEventHandler(Move_OnAborted);
        move.Failed     += new ClientEventHandler(Move_OnFailed);
        move.Progress   += new ClientEventHandler(Move_OnProgress);
        move.Completed  += new ClientEventHandler(Move_OnCompleted);

        CMoveParameters moveParameters;

        // =======================================================================================
        // Perform the PUSH based on the arguments UIDs take precedence over patient retrievals.
        // =========================================================================================
        if ( ! string.IsNullOrEmpty(studyUid) || ! string.IsNullOrEmpty(seriesUid) || ! string.IsNullOrEmpty(instanceUid) )
          moveParameters = move.AddParameters(CMoveParameters.Create(destination, studyUid, seriesUid, instanceUid));
        else
          moveParameters = move.AddParameters(CMoveParameters.Create(destination, medicalId, patientName));


        moveParameters.LogFile = logFile;

        Console.WriteLine("C-MOVE Started ");
        Console.WriteLine("");
        Console.WriteLine("AE Title:         " + move.AETitle);
        Console.WriteLine("Called AE Title:  " + move.Server.AETitle);
        Console.WriteLine("Server:           " + move.Server.Server);
        Console.WriteLine("Port:             " + move.Server.Port.ToString());
        Console.WriteLine("Destination:      " + destination);
        Console.WriteLine("");

        m_start = DateTime.Now;

        move.Start();

        move.Wait();

        /// Give time for cleanup
        System.Threading.Thread.Sleep(500);

      }
      catch (DicomException exp)
      {
        m_errorCode = -1;
        WriteError("Push failed : " + exp.Message);
      }
      catch (Exception ex)
      {
        m_errorCode = -1;
        WriteError("Push failed : " + ex.Message);
      }

      //==================================================================================
      // If an enter to continue is true then wait for the user to enter before exiting
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
      Viatronix.Logging.Log.Error(message, "Push", "WriteError");
      Console.WriteLine(message);
    } // WriteError(string message)



    #region event handlers

    static void Move_OnStarted(object sender, ClientEventArgs args)
    {

    }

    static void Move_OnProgress(object sender, Viatronix.Dicom.ClientEventArgs args)
    {

       Console.WriteLine(string.Format("Move image {0} of {1}", args.Completed, (args.Completed + args.Remaining)));
    }

    static void Move_OnFailed(object sender, Viatronix.Dicom.ClientEventArgs arg)
    {
      m_errorCode = 1;
      Console.WriteLine("");
      Console.Error.WriteLine(string.Format("C-MOVE Failed ({0}/{1}) : {2}", arg.Remaining, arg.Completed, arg.Message));
      m_completeEvent.Set();

    }

    static void Move_OnCompleted(object sender, Viatronix.Dicom.ClientEventArgs arg)
    {
      Console.WriteLine("");
      Console.WriteLine(string.Format("C-MOVE Completed : It took {0} seconds to push {1} images", DateTime.Now.Subtract(m_start).TotalSeconds , arg.Completed.ToString()) );

      m_completeEvent.Set();
    }

    static void Move_OnAborted(object sender, Viatronix.Dicom.ClientEventArgs arg)
    {
      Console.WriteLine("");
      Console.WriteLine(string.Format("C-MOVE Aborted ({0}/{1}) : {2}", arg.Remaining, arg.Completed, arg.Message));
      m_completeEvent.Set();

    }

    #endregion

  }
}
