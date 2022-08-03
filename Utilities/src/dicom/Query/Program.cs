using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
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
    /// Start 
    /// </summary>
    private static DateTime m_start;

    /// Destination
    private static string m_destination = string.Empty;

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
      string logFile = string.Empty;
      bool wait = (ConfigurationManager.AppSettings["wait"] != null ? Boolean.Parse(ConfigurationManager.AppSettings["wait"]) : false);

      string description = "Sends a DICOM C-FIND command to the server specified. The server will send the images to the specified destination aetitle. The destination aetitle must be known by the server.";


      try
      {

        // Initialize the logger
        Viatronix.Logging.Log.Initialize();

        XmlNode clientNode = (XmlNode)System.Configuration.ConfigurationManager.GetSection("client");

        Viatronix.Dicom.CommandLineParameters parameters = CommandLineParameters.Create("Query.exe", description, clientNode);

        parameters.Verbose  = (ConfigurationManager.AppSettings["verbose"] != null ? Boolean.Parse(ConfigurationManager.AppSettings["verbose"]) : parameters.Verbose);
        wait                = parameters.Wait;
        logFile             = (ConfigurationManager.AppSettings["log"] != null ? ConfigurationManager.AppSettings["log"] : string.Empty);


        parameters.Update(args);

        parameters.TryGetParameter("-log", ref logFile);
        parameters.TryGetParameter("-study", ref studyUid);
        parameters.TryGetParameter("-series", ref seriesUid);
        parameters.TryGetParameter("-instance", ref instanceUid);
        parameters.TryGetParameter("-level", ref level);
        parameters.TryGetParameter("-studyId", ref studyId);
        parameters.TryGetParameter("-mod", ref modality);
        parameters.TryGetParameter("-physician", ref physician);
        parameters.TryGetParameter("-studyDate", ref studyDate);
        parameters.TryGetParameter("-studyTime", ref studyTime);
        parameters.TryGetParameter("-seriesDate", ref seriesDate);
        parameters.TryGetParameter("-seriesTime", ref seriesTime);
        parameters.TryGetParameter("-dob", ref dob);
        parameters.TryGetParameter("-tob", ref tob);
        parameters.TryGetParameter("-patientId", ref medicalId);
        parameters.TryGetParameter("-medicalId", ref medicalId);
        parameters.TryGetParameter("-patientName", ref patientName);


        parameters.Validate();



        if (string.IsNullOrEmpty(medicalId) && string.IsNullOrEmpty(patientName) &&
              string.IsNullOrEmpty(studyUid) && string.IsNullOrEmpty(seriesUid) &&
              string.IsNullOrEmpty(instanceUid) && string.IsNullOrEmpty(physician) &&
              string.IsNullOrEmpty(studyDate) && string.IsNullOrEmpty(studyTime) &&
              string.IsNullOrEmpty(seriesDate) && string.IsNullOrEmpty(seriesTime) &&
              string.IsNullOrEmpty(dob) && string.IsNullOrEmpty(tob) &&
              string.IsNullOrEmpty(modality) &&
              string.IsNullOrEmpty(instanceUid) && string.IsNullOrEmpty(physician) &&
              string.IsNullOrEmpty(studyId))
          Console.WriteLine("No parameters specified for query, this may not work");




        CFind find = CFind.Create(parameters.AETitle, parameters.Server.ToXml());

        find.Started += new ClientEventHandler(Find_OnStarted);
        find.Aborted += new ClientEventHandler(Find_OnAborted);
        find.Failed += new ClientEventHandler(Find_OnFailed);
        find.Progress += new ClientEventHandler(Find_OnProgress);
        find.Completed += new ClientEventHandler(Find_OnCompleted);


        CFindParameters findParameters = CFindParameters.Create();

        findParameters.InstanceUids = instanceUid;
        findParameters.PatientNames = patientName;
        findParameters.Modalities = modality;
        findParameters.SeriesUids = seriesUid;
        findParameters.StudyDate = studyDate;
        findParameters.SeriesDate = seriesDate;
        findParameters.StudyUids = studyUid;
        findParameters.PatientDob = dob;
        findParameters.PatientTob = tob;
        findParameters.StudyId = studyId;
        findParameters.Level = level;
        findParameters.MedicalId = medicalId;
        findParameters.Physician = physician;
        findParameters.LogFile = logFile;

        find.AddParameters(findParameters);

        m_start = DateTime.Now;

        Console.WriteLine("C-FIND Started ");
        Console.WriteLine("");
        Console.WriteLine("AE Title:         " + find.AETitle);
        Console.WriteLine("Called AE Title:  " + find.Server.AETitle);
        Console.WriteLine("Server:           " + find.Server.Server);
        Console.WriteLine("Port:             " + find.Server.Port.ToString());
        Console.WriteLine("");

 
        find.Start();

        find.Wait();

        /// Give time for cleanup
        System.Threading.Thread.Sleep(500);

      }
      catch (DicomException exp)
      {
        m_errorCode = -1;
        WriteError("Query failed : " + exp.Message);
      }
      catch (Exception ex)
      {
        m_errorCode = -1;
        WriteError("Query failed : " + ex.Message);
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
      Viatronix.Logging.Log.Error(message, "Query", "WriteError");
      Console.WriteLine(message);
    } // WriteError(string message)


 

    #region event handlers

    static void Find_OnStarted(object sender, ClientEventArgs args)
    {
 
    }

    static void Find_OnProgress(object sender, Viatronix.Dicom.ClientEventArgs args)
    {


      if (!string.IsNullOrEmpty(m_destination))
      {

        FileStream fs = new FileStream(m_destination, FileMode.Append, FileAccess.Write);
        StreamWriter writer = new StreamWriter(fs);
        writer.WriteLine(args.Image.ToXml());
        fs.Close();

      }
      else
      {
        Console.WriteLine("---------------------------------------------------------------------");
        Console.WriteLine(args.Image.ToXml());
      }

    }

    static void Find_OnFailed(object sender, Viatronix.Dicom.ClientEventArgs args)
    {
      m_errorCode = 1;
      Console.WriteLine("");
      Console.Error.WriteLine(string.Format("C-FIND Failed ({0}/{1}) : {2}", args.Remaining, args.Completed, args.Message));
      m_completeEvent.Set();

    }

    static void Find_OnCompleted(object sender, Viatronix.Dicom.ClientEventArgs args)
    {
      Console.WriteLine("");
      Console.WriteLine(string.Format("C-FIND Completed : It took {0} seconds to push {1} images", DateTime.Now.Subtract(m_start).TotalSeconds, args.Completed));

      m_completeEvent.Set();
    }

    static void Find_OnAborted(object sender, Viatronix.Dicom.ClientEventArgs args)
    {
      Console.WriteLine("");
      Console.WriteLine(string.Format("C-FIND Aborted ({0}/{1}) : {2}", args.Remaining, args.Completed, args.Message));
      m_completeEvent.Set();

    }

    #endregion

  }
}
