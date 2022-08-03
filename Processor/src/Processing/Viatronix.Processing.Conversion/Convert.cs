// $Id: Convert.cs,v 1.5.4.2 2009/04/07 12:19:12 mkontak Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]


using System;
using System.Threading;
using System.Collections.Generic;
using Viatronix.Dicom;
using Viatronix.Logging;
using Viatronix.Conversion;
using System.Net.Sockets;
using System.Text;
using System.IO;
using System.Xml;
using Viatronix.Enterprise.Processing;

namespace Viatronix.Processing
{

	/// <summary>
	/// Implements a class that will perform the conversion from DICOM slices to V3D Volume file.
	/// </summary>
	internal class Convert 
	{

    #region constants

    /// <summary>
    /// Version of conversion executable
    /// </summary>
    private const string m_version = "3.6";

    /// <summary>
    /// System name we are running on
    /// </summary>
    private static string m_systemName = System.Windows.Forms.SystemInformation.ComputerName;

    /// <summary>
    /// Module name
    /// </summary>
    private const string m_moduleName = "conversion";

    #endregion

    #region fields

 
    /// <summary>
    /// Messaging manager
    /// </summary>
    private Viatronix.Messaging.MessagingManager m_manager = null;

    /// <summary>
    /// Status message
    /// </summary>
    private Viatronix.Messaging.ProcessingMessage m_message = null;


    /// <summary>
    /// Error Code
    /// </summary>
    private int m_errorCode = 0;

    /// <summary>
    /// Down Sample Large Datasets
    /// </summary>
    private bool m_downSampleLargeDatasets = true;
   
    /// <summary>
    /// Flag to ignore project images (MIPS)
    /// </summary>
    private bool m_ignoreProjectionImages = false;

 
    /// <summary>
    /// Sub type being processed
    /// </summary>
    private string m_subType = string.Empty;

    /// <summary>
    /// ID of source
    /// </summary>
    private int m_sourceID = 0;

    /// <summary>
    /// Job id
    /// </summary>
    private string m_jobId = string.Empty;


    /// <summary>
    /// Default application
    /// </summary>
    private string m_defaultApplication = string.Empty;


    /// <summary>
    /// Source file path (Location of DICOM files)
    /// </summary>
    private string m_sourceFilePath = "";

    /// <summary>
    /// Destination file path (Destination of volume file). 
    /// </summary>
    private string m_destinationFilePath = "";

    /// <summary>
    /// Interpolation value
    /// </summary>
    private int m_interpolateValue = -1;

    /// <summary>
    /// Ignore Acquisition Number flag
    /// </summary>
    private bool m_splitByAcquisitionNumber = false;

    /// <summary>
    /// Delta value retrospective gating
    /// </summary>
    private float m_delta = 0.0001f;

    /// <summary>
    /// Retrospective flag
    /// </summary>
    private bool m_retrospective = false;

    /// <summary>
    /// Maximum detectors allowed
    /// </summary>
    private int m_maxDetectors = 32;

    /// <summary>
    /// Default pixel padding value
    /// </summary>
    private int m_pixelPadding = 0;

    /// <summary>
    /// Volume type to be generated
    /// </summary>
    private string m_volumeType;

    /// <summary>
    /// Logging level
    /// </summary>
    private int m_logLevel;

    /// <summary>
    /// Logger Type
    /// </summary>
    private MessageFormat m_logMessageFormat = MessageFormat.Text;

    /// <summary>
    /// Log file name
    /// </summary>
    private string m_logFilePath;

    /// <summary>
    /// Command line arguments
    /// </summary>
    private Dictionary<string, string> m_commands = new Dictionary<string, string>();

    /// <summary>
    /// Conversion model
    /// </summary>
    private ConversionModel m_model = new ConversionModel();

    /// <summary>
    /// Connection to server for status
    /// </summary>
    private TcpClient m_client = new TcpClient();

    /// <summary>
    /// unit test flag: 1 -- to execute unit test; 0 -- not to execute
    /// </summary>
    private int m_unittest = 0;

  
    #endregion

    #region properties

    /// <summary>
    /// Gets the error code
    /// </summary>
    public int ErrorCode
    {
      get { return m_errorCode;  }
    } // ErrorCode

    /// <summary>
    /// Returns the log file path
    /// </summary>
    public string LogFilePath 
    {
      get { return m_logFilePath; }
    }

    #endregion


    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="args">Command line arguments</param>
		public Convert(string [] args)
		{

      m_splitByAcquisitionNumber = false;

      // Parse the command line arguments into local fields
      ParseCommandLine(args);


      m_model.Updated   += new PipelineEventHandler(OnConversionUpdated);
      m_model.Completed += new PipelineEventHandler(OnConversionCompleted);
      m_model.Failed    += new PipelineEventHandler(OnConversionFailed);

		} // Convert

    #endregion


    #region event handlers

    /// <summary>
    /// Handles the conversion update event
    /// </summary>
    /// <param name="args"></param>
    void OnConversionUpdated( PipelineEventArgs args )
    {

      Log.Debug(String.Format("Conversion Updated ({0},{1},{2})", args.Remaining, args.Completed, args.Message), "Convert", "OnConversionUpdated");


      // ==============================================================================================
      // Check for a Writing text in the message indicating that we need to change the module state 
      // from converting to WRITING VOLUME.
      // ==============================================================================================
      if (args.Message.StartsWith("Writing"))
      {
        SendState(args.Progress, "Writing Volume");
      }
      else
      {
        SendState(args.Progress, "Converting");
      }

    } // OnConversionUpdated
   

    /// <summary>
    /// Handles the conversion completed event
    /// </summary>
    /// <param name="args"></param>
    void OnConversionCompleted( PipelineEventArgs args )
    {
      Log.Debug("Conversion Completed", "Convert", "OnConversionCompleted");

      SendState(100, "Converting");

    } // OnConversionCompleted


    /// <summary>
    /// Handles the conversion failed event
    /// </summary>
    /// <param name="args"></param>
    void OnConversionFailed( PipelineEventArgs args )
    {
      Log.Debug(String.Format("Conversion Failed : {0}", args.Message), "Convert", "OnConversionFailed");

      m_errorCode = -1;

    } // OnConversionFailed


    /// <summary>
    /// Handles the conversion started event
    /// </summary>
    /// <param name="args"></param>
    void OnConversionStarted( PipelineEventArgs args )
    {
      Log.Debug("Conversion Started", "Convert", "OnConversionUpdated");

      SendState(1, "Converting");

    } // OnConversionStarted


    #endregion

    #region methods

 
    /// <summary>
    /// Sends state back to the WQ handler
    /// </summary>
    /// <param name="overallProgress">Overall progress of the conversion</param>
    /// <param name="stageProgress">Module state's progress</param>
    private void SendState(float progress, string stage = "")
    {

      try
      {
        // =========================================================================================
        // If there is no manager and no message then we cannot send back the state
        //    - No Manager means that the messageHost and/or the  messagePort was not supplied.
        //    - No Message means that the JOB ID was not supplied
        // =========================================================================================
        if (m_manager != null && m_message != null)
        {
            m_message.Progress = progress;
            m_message.Stage    = stage;
 
            m_manager.Send(m_message);
         } // if

      }
      catch (Exception ex)
      {
        Log.Warning("Failed to send message to work queue : " + ex.Message, "Convert", "SendState");
      }
 
    } // SendState()


    /// <summary>
    /// Parses the command line options and sets local data members.
    /// </summary>
    /// <param name="args">Command line argument list</param>
    private void ParseCommandLine( string [] args )
    {

      string command = string.Empty;
      foreach (string arg in args)
      {
        if (arg[0] == '-')
        {
          command = arg.Substring(1, arg.Length - 1);
          m_commands.Add(command, string.Empty);
        } // if
        else
          m_commands[command] = arg;

      } // for


      // Check for the process id
      if ( m_commands.ContainsKey("procId") )
        m_sourceID = System.Convert.ToInt32(m_commands["procId"]);       
      

      // Job Id
      if (m_commands.ContainsKey("job"))
      {
        m_jobId = m_commands["job"];
        m_message = Viatronix.Messaging.ProcessingMessage.Create(m_commands["job"], m_moduleName);
      } // job id


      // ==============================================
      // Gets the default apoplication id if specified
      // ==============================================
      if (m_commands.ContainsKey("app"))
        m_defaultApplication = m_commands["app"];


      //===================================================================
      // Check for the ignore projections flag
      //===================================================================
      if ( m_commands.ContainsKey("ignoreProjections") )
      { 
        try
        {
          m_ignoreProjectionImages = System.Convert.ToBoolean(m_commands["ignoreProjections"]);
        }
        catch ( Exception )
        {
          m_ignoreProjectionImages = ( System.Convert.ToInt32(m_commands["ignoreProjections"]) == 1 ? true : false );
        }
      } // END ... If the ignore projections flag is present

      // Check for the study type id, used when sending state messages 
      if ( m_commands.ContainsKey("subType") )
        m_subType = m_commands["subType"].ToString();

      //================================================================================================
      // Make sure the source file path is specified. This contains the SLICECOUNTS file which is no
      // longer used. I only use the directory of the SLICECOUNTS file and read all the DICOM files
      // from there.
      //================================================================================================
      if ( ! m_commands.ContainsKey("sourceFilePath") )
      {
        throw new ArgumentException("No source file path specified");
      } // END ... If the sourceFilePath is not present in the command line
      else
      {
        /// The source file path come in with the root on it
        m_sourceFilePath = System.Convert.ToString(m_commands["sourceFilePath"]);

        if ( string.Compare(System.IO.Path.GetFileName(m_sourceFilePath), "Dicom", true) != 0 )
        {
          string sourceFilePath = System.IO.Path.Combine(m_sourceFilePath, "Dicom");
          if ( System.IO.Directory.Exists(sourceFilePath) )
          {
            m_sourceFilePath = sourceFilePath;
          }
        } // if dicom sub directory exists

      } // END ... Else the sourceFilePath is present

      //======================================================================
      // Make sure the destination file path is present in the command line. 
      //=====================================================================
      if ( ! m_commands.ContainsKey("destFilePath") )
      {
        throw new ArgumentException("No destination file path specified");
      } // END ... If the destinationFilePath was not present
      else
      {
        m_destinationFilePath = System.Convert.ToString(m_commands["destFilePath"]);
      } // END ... Else the destinationFilePath was present


      //==========================================================================================
      // Check for the logFilePath argument. If present use the specified value as the log file
      // else append "DcmToVol" to the destintaionFilePath
      //==========================================================================================
      if ( ! m_commands.ContainsKey("logFilePath") )
      {
        m_logFilePath = string.Empty; 
      } // END ... If the logFilePath is not present
      else
      {
        m_logFilePath = System.Convert.ToString(m_commands["logFilePath"]);
      } // END ... Else the logFilePath is present


      //==========================
      // Check for the log level
      //==========================
      if ( m_commands.ContainsKey("logLevel") )
      {
        m_logLevel = System.Convert.ToInt32(m_commands["logLevel"]);
      } // END ... If the log level is present

      //==========================
      // Check for the logger type
      //==========================
      if (m_commands.ContainsKey("logType"))
      {
        uint logformat = System.Convert.ToUInt32(m_commands["logformat"]);

        if (logformat < 3)
          m_logMessageFormat = (MessageFormat)logformat;

      } // END ... If the log level is present

      //===========================================================================================
      // Check for the interpolation value. If > 0 and less then the current slice spacing then 
      // interpolation will be performed.
      //============================================================================================
      if ( m_commands.ContainsKey("interpolate") )
        m_interpolateValue = System.Convert.ToInt32(m_commands["interpolate"]);
 
      //============================================================================================
      // Check for the retrospective flag. If 1 (true) then retrospective gating is checked in the
      // dataset. 
      //============================================================================================
      if ( m_commands.ContainsKey("retrospective") )
      {
        try
        {
          m_retrospective = System.Convert.ToBoolean(m_commands["retrospective"]);
        }
        catch ( Exception )
        {
          m_retrospective = ( System.Convert.ToInt32(m_commands["retrospective"]) == 1 ? true : false );
        }
      } // END ... If retrospective is present

    
      //============================================================================================
      // Check for the ignore acquisition flag. If 1 (true) then acquisition # is ignored as part of 
      // the dataset key.
      //============================================================================================
      if ( m_commands.ContainsKey("ignoreAcquisitionNo") )
      {
        try
        {
          m_splitByAcquisitionNumber = ! System.Convert.ToBoolean(m_commands["ignoreAcquisitionNo"]);
        }
        catch ( Exception )
        {
          m_splitByAcquisitionNumber = ( System.Convert.ToInt32(m_commands["ignoreAcquisitionNo"]) == 1 ? false : true );
        }

      } // END ... If ignore acquisition number is present


      //=================================================================================================
      // Check for the delta value, which is used to determine retrospective gating. The default is .001
      // and it should suffice.
      //===================================================================================================
      if ( m_commands.ContainsKey("delta") )
      {
        m_delta = System.Convert.ToSingle(m_commands["delta"]);
      } // END ... If the delta value is present

      //=====================================================================================================
      // Check for max detectors. This is the maximum number of detectors allowed during the retrosepctive
      // gating procss.
      //=====================================================================================================
      if ( m_commands.ContainsKey("maxdetectors") )
      {
        m_maxDetectors = System.Convert.ToInt32(m_commands["maxdetectors"]);
      } // END ... If the maxdetectors is present

      //============================================================================================
      // Check for a volume type specifier. Determines the type of volume built BLOCK or LINEAR. 
      //============================================================================================
      if ( m_commands.ContainsKey("volumeType") )
      {
        m_volumeType = System.Convert.ToString(m_commands["volumeType"]).ToLower();
      } // END ... If the volume type specifier is present


      //======================================================================================================
      // Check for the default pixel padding. this is the default pixel padding value to use if not present
      // in the DICOM header.
      //======================================================================================================
      if ( m_commands.ContainsKey("defaultPixelPadding") )
      {
        m_pixelPadding = System.Convert.ToInt32(m_commands["defaultPixelPadding"]);
      } // END ... If the default pixel padding is present

      //======================================================================================================
      // Check for the default pixel padding. this is the default pixel padding value to use if not present
      // in the DICOM header.
      //======================================================================================================
      if ( m_commands.ContainsKey("downSampleLargeDatasets") )
      {
        try
        {
          m_downSampleLargeDatasets = System.Convert.ToBoolean(m_commands["downSampleLargeDatasets"]);
        }
        catch ( Exception )
        {
          m_downSampleLargeDatasets = ( System.Convert.ToInt32(m_commands["downSampleLargeDatasets"]) == 1 ? true : false );
        }
      } // END ... If the default pixel padding is present

      //======================================================================================================
      // Check for the unit test flag.
      //======================================================================================================
      if ( m_commands.ContainsKey("unittest") )
      {
        m_unittest = System.Convert.ToInt32(m_commands["unittest"]);
      } // unittest
   
      if ( m_logFilePath.Length == 0 )
      {
        m_logFilePath = m_destinationFilePath + "_DcmToVol";
      } //

    } // ParseCommadLine()


    /// <summary>
    /// Runs the conversion process. Sets up the thread to perform the actual conversion, starts it and waits for
    /// is to complete.
    /// </summary>
    public void Run()
    {

      /// Creates the logger for the conversion (Use guid for unique name)
      Log.Create("conversion", m_logFilePath, (LogLevel)m_logLevel, m_logMessageFormat, true);     

      // Add the log message to identify the patch
      Log.Information("Dicom Conversion Module Version 3.6", "Convert", "Run");


      // Check for the messageHost, which is the hostname of the message receiver
      if (m_commands.ContainsKey("messageHost") && m_commands.ContainsKey("messagePort"))
      {
        m_manager = Viatronix.Messaging.MessagingManager.Create(m_commands["messageHost"], UInt32.Parse(m_commands["messagePort"]));
        Log.Information(string.Format("Messaging manager create ({0}/{1})", m_manager.Server, m_manager.Port), "Convert", "ParseCommand");
      }
      else
        Log.Information("Messaging manager could not be create, no host and/or port specified", "Convert", "ParseCommand");
 
      StringBuilder argumentList = new StringBuilder();
      foreach (KeyValuePair<string, string> command in m_commands)
      {
        argumentList.Append(command.Key + " " + command.Value + " ");
      }

      Log.Information(String.Concat("Conversion started : ", argumentList.ToString()), "Convert", "Run");

      string sourcePath = m_sourceFilePath;

      FileInfo info = new FileInfo(m_sourceFilePath);

      if ( info.Attributes != FileAttributes.Directory )
        sourcePath = System.IO.Path.Combine(System.IO.Path.GetDirectoryName(m_sourceFilePath), "dicom");


      m_model.JobId      = m_jobId;
      m_model.Path       = sourcePath;
      m_model.Pattern    = "*.dcm";
      m_model.VolumeFile = m_destinationFilePath + ".vol";

      Log.Debug(String.Format("Source dicom path : {0}", m_model.Path), "Convert", "Run");
      Log.Debug(String.Format("Destination volume file : {0}", m_model.VolumeFile), "Convert", "Run");

      try
      {
        /// Start the conversion
        m_model.CreateVolume();

        /// Wait for the conversion to complete
        m_model.CompletedEventHandle.WaitOne();

        /// execute unit test 
        if (m_unittest == 1)
          m_model.UnitTest();

        /// Complete
        SendState(100);

        Console.WriteLine(m_model.Results.ToXml());

      } // 
      catch ( Exception ex )
      {
        string sErrorMessage = String.Concat("Failed to create volume : ", ex.Message);

        Log.Error(sErrorMessage, "Convert", "Run");

        throw;
      }

    } // Run


    #endregion

  } // Convert class

} // Viatronix.Processing namespace


#region revision history

// $Log: Convert.cs,v $
// Revision 1.5.4.2  2009/04/07 12:19:12  mkontak
// Added log message idication verion 3.2.1.1
//
// Revision 1.5.4.1  2009/04/06 18:55:01  mkontak
// Added log message idication verion 3.2.1.1
//
// Revision 1.5  2006/04/07 12:32:46  vxconfig
// removed licening reference
//
// Revision 1.4  2005/09/28 11:58:26  mkontak
// Removal of old registry reads
//
// Revision 1.3  2005/09/22 16:50:09  mkontak
// Modified to account for changes in the sourceFilePath arguement
//
// Revision 1.2  2005/09/01 18:10:27  mkontak
// Fixes for Utilities changes
//
// Revision 1.1  2005/06/14 18:07:21  mkontak
// Moved from DicomToVolume
//
// Revision 1.13  2005/06/08 17:27:52  mkontak
// Changes tp messaging and now build in vxProcessor\\Conversion
//
// Revision 1.12  2005/06/08 13:41:23  mkontak
// Change message communication with executables
//
// Revision 1.11  2005/05/19 17:58:24  mkontak
// Fixes
//
// Revision 1.10  2005/04/14 18:14:37  mkontak
// Fix
//
// Revision 1.9  2005/02/10 21:33:10  mkontak
// Downsampling
//
// Revision 1.8  2005/01/05 18:48:31  mkontak
// Added parameter for down sampling
//
// Revision 1.7  2004/05/25 17:37:30  mkontak
// Clenup of the sendmessage object
//
// Revision 1.6  2004/05/18 19:01:12  mkontak
// Fix fir linear volumes
//
// Revision 1.5  2004/05/17 20:28:14  mkontak
// Minor changes for the WQ
//
// Revision 1.4  2004/04/08 12:42:53  mkontak
// Cleanup logging after 7.1 conversion and return a 3 if we have a license error
//
// Revision 1.3  2004/04/05 18:26:35  mkontak
// Changes to new licenses manager
//
// Revision 1.2  2004/03/24 21:32:55  frank
// removed hard-coded paths to installation, log, license, and config directories
//
// Revision 1.1.1.1  2004/03/02 03:58:37  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.16  2004/02/25 14:31:43  mkontak
// Merge with CONSOLE 2.0
//
// Revision 1.15.2.3  2004/02/25 14:30:54  mkontak
// Fix ignore projections arguement translation
//
// Revision 1.15.2.2  2004/02/13 15:06:35  mkontak
// *** empty log message ***
//
// Revision 1.15.2.1  2004/02/13 14:41:48  mkontak
// Angio license
//
// Revision 1.15  2004/01/07 20:16:13  mkontak
// Fixed license check
//
// Revision 1.14  2004/01/05 14:53:43  mkontak
// Added license checking into the conversion program
//
// Revision 1.13  2003/09/19 18:17:55  mkontak
// no message
//
// Revision 1.12  2003/09/19 18:02:57  mkontak
// no message
//
// Revision 1.11  2003/09/12 20:01:05  mkontak
// Log message
//
// Revision 1.10  2003/09/05 19:49:09  mkontak
// Fix to compile
//
// Revision 1.9  2003/07/16 14:18:25  mkontak
// Added logging of the parameters before starting the conversion
//
// Revision 1.8  2003/07/15 14:50:02  mkontak
// Additional arguements
//
// Revision 1.7  2003/07/03 11:50:25  mkontak
// Added threading to help in the callback messaging and also coding standards
//
// Revision 1.6  2003/06/26 20:28:16  mkontak
// Added SaveDicomFile and CreatePreview
//
// Revision 1.5  2003/06/18 18:45:24  mkontak
// Dicom coding standard
//
// Revision 1.4  2003/05/19 15:27:24  mkontak
// Coding standards
//
// Revision 1.3  2003/05/16 15:05:15  mkontak
// Fixed dicom to volume
//
// Revision 1.2  2003/05/08 13:22:07  mkontak
// coding standards fix
//
// Revision 1.1  2003/05/08 13:06:52  mkontak
// Initial check-in
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Processing.Conversion/Convert.cs,v 1.5.4.2 2009/04/07 12:19:12 mkontak Exp $
// $Id: Convert.cs,v 1.5.4.2 2009/04/07 12:19:12 mkontak Exp $

#endregion

