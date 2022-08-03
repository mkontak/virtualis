// $Id: Utils.cs,v 1.1.2.11 2011/05/19 16:02:42 kchalupa Exp $
//
// Copyright © 2009, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Kyle R Chalupa] [kchalupa@viatronix.com]

using System;
using System.IO;
using System.Configuration;
using System.ComponentModel;
using System.Windows.Forms;
using System.Collections.Generic;
using System.Text;

using Viatronix.Enterprise.Entities;
using Viatronix.Dicom;
using Viatronix.Logging;
using Viatronix.Utilities;
using Viatronix.Utilities.Registry;
using Viatronix.Utilities.Importer.Data;
using Viatronix.Utilities.Importer.Base;

namespace Viatronix.Utilities.Importer
{
  /// <summary>
  /// Types of log messages.
  /// </summary>
  public enum LogTypes
  {
    Debug,  // Test message.
    Error,  // Failure message.
    Info,   // Informational message.
    Warning // Warning message.
  }; // LogTypes

 
  /// <summary>
  /// Utility functions to assist in performing the task at hand.
  /// </summary>
  public static class Utils
  {

    #region fields

    /// <summary>
    /// The intermediate log file name.
    /// </summary>
    private static string m_logFileName;

    #endregion

    #region properties

    /// <summary>
    /// Gets the configurable base log directory.
    /// </summary>
    public static string LogDirectory
    { 
      get { return SystemSettings.CurrentSystemSettings.ParseExpression(ConfigurationManager.AppSettings["LogPath"]); }
    } // LogDirectory

    #endregion

    #region methods

    /// <summary>
    /// Clears the read only attribute.
    /// </summary>
    /// <param name="filePath">File path to set.</param>
    private static void RemoveReadOnlyAttribute(string filePath)
    {
      File.SetAttributes(filePath, File.GetAttributes(filePath) & ~FileAttributes.ReadOnly);
    } // RemoveReadOnlyAttribute( filePath )


    /// <summary>
    /// Clears the Read only file attribute, intended for importing from CD.
    /// </summary>
    /// <param name="filePath">The desired file path.</param>
    public static void ClearReadOnlyAttributeRecursively(string filePath)
    {
      if (Directory.Exists(filePath))
      {
        foreach (string directory in Directory.GetDirectories(filePath))
        {
          RemoveReadOnlyAttribute(directory);
          ClearReadOnlyAttributeRecursively(directory);
        }

        foreach (string file in Directory.GetFiles(filePath))
        {
          RemoveReadOnlyAttribute(file);
        }
      }
      else if(File.Exists(filePath))
      {
        RemoveReadOnlyAttribute(filePath);
      }
    } // ClearReadOnlyAttributeRecursively(String mpFilePath)


    /// <summary>
    /// Performs the Import.
    /// </summary>
    /// <param name="ioArgs">Gives the source, destination and what actions to perform in terms of copying
    /// database insert and whether or not the session should be included in them.</param>
    /// <returns></returns>
    public static void Import(ImporterArgs args)
    {
      // Reinitialize before each import.
      Global.Initialize();

      IImporter manager = null;
      Filters.IFilter filter = null;
      
      List<string> directories = new List<string>(Directory.GetDirectories(args.Source));
      directories.Sort();

      // Determine whether the copy should take place.

      string sourceLC = args.Source.ToLower() + "\\";
      string destinationLC = args.Destination.ToLower();
      string cacheLC = SystemSettings.CurrentSystemSettings.CacheBasePath.ToLower();

      // GUI Level has protection, prevent the user from the Cmd Line.
      if( sourceLC == destinationLC || sourceLC.StartsWith(cacheLC) )
      {
        // Log a message that the destination was not valid.
        if( sourceLC == destinationLC )
        {
          Log.Warning("Specified source and destination folders are equal, a file copy will not take place", "Utils", "Import");
        } // if(ioArgs.Source != ioArgs.Destination)
        else if( sourceLC.StartsWith(cacheLC) )
        {
          Log.Warning("Specified source directory is the Viatronix Cache directory.", "Utils", "Import");
        } // else

        // Don't perform copy, only update database.
        args.PerformCopy = false;
      }

      Log.Information(">>>>>>>> Beginning to import directory: " + args.Source + " <<<<<<<<", "Utils", "Import");

      if (Directory.GetFiles(args.Source, "study.xml").Length > 0)
      {
        // User chose the study directory as the top level. Log a failure and return.

        Global.LogPatientImportFailed(args.Source);
        Utils.LogMessage(LogTypes.Error, "User selected 3.0 study directory, import failed!");
        Log.Error("User selected study directory, import failed!", "Utils", "Import");

        return;
      }
      else if (Directory.GetFiles(args.Source, "series.xml").Length > 0)
      {
        // User chose the series directory as the top level. Log a failure and return.

        Global.LogPatientImportFailed(args.Source);
        Utils.LogMessage(LogTypes.Error, "User selected 3.0 series directory, import failed!");
        Log.Error("User selected series directory, import failed!", "Utils", "Import");

        return;
      }

      filter = Filters.FilterFactory.Create(args);
      if( filter != null )
      {
        Global.LogDirectoryNoticed(args.Source);

        DateTime start = DateTime.Now;
        Global.NotifyProgress("Parsing Directory", 0);
        
        manager = filter.Parse(args);
        
        DateTime end = DateTime.Now;
        Global.NotifyProgress("Parse Completed", 5);
        Log.Debug(string.Format("Took {0} to parse input directory.  [DIRECTORY={1}]", (end - start).TotalSeconds, args.Source), "Utils", "Import");

        // Check to make sure the directory could be parsed.
        if( manager != null )
        {
          Global.CheckForCancel();

          try
          {
            // Initiate copy cascade.
            Global.NotifyProgress("Beginning To Copy Directory", 5);
            manager.Copy(args);
          }
          catch(Exception)
          {
            if (directories.Count == 0)
            {
              Global.LogPatientImportFailed(args.Source);
            }
          }
        } // if( manager != null )
        else
        {
          Log.Error("Failed to parse directory contents.  Directory: " + args.Source, "Utils", "Import");
          Utils.LogMessage(LogTypes.Error, "Failed to parse directory contents.  Directory: " + args.Source);

          if (directories.Count == 0)
          {
            Global.LogPatientImportFailed(args.Source);
          }

          if (Global.Cancelled)
          {
            Global.LogPatientImportFailed(args.Source);
            return;
          }
        }
      } // if(filter != null)
      else
      {
        Log.Error("Failed to determine a proper filter for directory contents.  Directory: " + args.Source, "Utils", "Import");
        Utils.LogMessage(LogTypes.Error, "Failed to determine a proper filter for directory contents.  Directory: " + args.Source);

        if (directories.Count == 0)
        {
          Global.LogPatientImportFailed(args.Source);
        }
      }

      Log.Information(">>>>>>>> End of processing directory: " + args.Source + " <<<<<<<<", "Utils", "Import");
            
      if( filter == null || manager == null )
      {
        Utils.LogMessage(LogTypes.Warning, " <" + args.Source + "> couldn't be parsed.  Descending one level.");

        // Couldn't parse the current directory so parse one sublevel down.
        foreach( string directory in directories )
        {
          if (Global.Cancelled)
          {
            return;
          }

          Log.Information(">>>>>>>> Beginning to import directory: " + directory + " <<<<<<<<", "Utils", "Import");

          // Set up the source parameter.
          args.Source = directory;

          Global.LogDirectoryNoticed(args.Source);

          manager = null;

          // Can the directory be parsed?  If so parse it!
          filter = Filters.FilterFactory.Create(args);
          if( filter != null )
          {
            DateTime start = DateTime.Now;
            Global.NotifyProgress("Parsing Directory", 0);

            manager = filter.Parse(args);

            DateTime end = DateTime.Now;
            Global.NotifyProgress("Parse Completed", 5);
            Log.Debug(string.Format("Took {0} to parse input directory.  [DIRECTORY={1}]", (end - start).TotalSeconds, args.Source), "Utils", "Import");
          } // if( filter != null )
          else
          {
            Global.LogPatientImportFailed(args.Source);
            Log.Error("Failed to determine a proper filter for directory contents.  Directory: " + args.Source, "Utils", "Import");
            Utils.LogMessage(LogTypes.Error, "Failed to determine a proper filter for directory contents.  Directory: " + args.Source);
          }

          // Does the manager contain useful data?
          if( manager != null )
          {
            Global.NotifyProgress("Beginning To Copy Directory", 5);

            manager.Copy(args);

          } // if(manager != null)
          else
          {
            Global.LogPatientImportFailed(args.Source);
            Log.Error("Failed to parse directory contents.  Directory: " + args.Source, "Utils", "Import");
            Utils.LogMessage(LogTypes.Error, "Failed to parse directory contents.  Directory: " + args.Source);
          } // else

          Log.Information(">>>>>>>> End of processing directory: " + args.Source + " <<<<<<<<", "Utils", "Import");

        } // foreach (string directory in directories)

      } // if(filter == null || manager == null)
    } // Import(ImporterArgs ioArgs)


    /// <summary>
    /// Verifies that the folder that we are dealing with has dicom info in it.
    /// 
    /// Either in a dcm, vol or ecv file.
    /// </summary>
    /// <param name="path">The path to examine.</param>
    /// <returns>Whether or not the folder contains dicom information.</returns>
    public static bool ContainsDicomInfo(string path)
    {
      return ((Directory.GetFiles(path, "*?.vol").Length > 0) || (Directory.GetFiles(path, "*?.ecv").Length > 0));
    } // ContainsDicomInfo(string path)


    /// <summary>
    /// Gets the dicom information from the folder that we are dealing with.
    /// 
    /// Either in a dcm, vol or ecv file.
    /// </summary>
    /// <param name="path">The path to examine.</param>
    /// <param name="dataSetToElementList">The map from datasets to element lists that this function will populate.</param>
    /// <param name="dataSetToImageCount">The map from datasets to image counts that this function will populate</param>
    /// <returns>An Element List that contains dicom information.</returns>
    public static void GetDicomInfo(string source, Dictionary<string, Image> dataSetToElementList, Dictionary<string, int> dataSetToImageCount)
    {
      string[] volFiles;
      Image image;

      // Because the dicom folders dicom files don't have the dataset in the name, create a dictionary to map the data set to the filename.
      Dictionary<string, List<string>> dataSetToDicomFiles = new Dictionary<string, List<string>>();
      List<string> volumeList = new List<string>();

      // List of datasets that had volume files without dicom info.
      List<string> dataSetsNoVolumeDicomInfo = new List<string>();

      // Add all .vol files into the list.
      if( ( volFiles = Directory.GetFiles(source, "*?.vol") ).Length > 0 )
      {
        volumeList.AddRange(volFiles);
      } // if( ( volFiles = Directory.GetFiles(path, "*?.vol") ).Length > 0 )

      // Add all .ecv files into the list.
      if( ( volFiles = Directory.GetFiles(source, "*?.ecv") ).Length > 0 )
      {
        volumeList.AddRange(volFiles);
      } // if( ( volFiles = Directory.GetFiles(path, "*?.ecv") ).Length > 0 )

      Global.CheckForCancel();

      // Get the list of Dicom Files in case we can't get dicom info from the volume files in a dataset.
      foreach(string file in Directory.GetFiles(source, "*?.dcm"))
      {
        Global.CheckForCancel();

        string dicomFileNameLC = Path.GetFileName(file).ToLower();
        string dataSet = GetDataSet(dicomFileNameLC);

        List<string> dicomFilesForDataSet = null;
        if(!dataSetToDicomFiles.TryGetValue(dataSet, out dicomFilesForDataSet))
        {
          dicomFilesForDataSet = new List<string>();
          dataSetToDicomFiles.Add(dataSet, dicomFilesForDataSet);
        } // if(!dataSetToDicomFiles.TryGetValue(dataSet, out dicomFilesForDataSet)

        if(!dicomFilesForDataSet.Contains(file))
        {
          dicomFilesForDataSet.Add(file);
        } // if(!dicomFilesForDataSet.Contains(file))
      } // foreach(string file in Directory.GetFiles(path, "*?.dcm"))

      Global.CheckForCancel();

      string[] subDirectories = Directory.GetDirectories(source);

      // Examine the subdirectories and add dicom files from ones with an _dcm in their name.
      foreach(string subDirectory in subDirectories)
      {
        Global.CheckForCancel();

        string subDirectoryLC = subDirectory.ToLower();

        // Files don't have datasets in their name, so take the dataset from the subdirectory.
        string dataSet = Utils.GetDataSet(subDirectoryLC);

        if(subDirectoryLC.EndsWith("_dcm"))
        {
          // Get the list of Dicom Files in case we can't get dicom info from the volume files in a dataset.
          foreach( string file in Directory.GetFiles(subDirectory, "*?.dcm") )
          {
            Global.CheckForCancel();

            string fileLC = Path.GetFileName(file).ToLower();

            List<string> dicomFilesForDataSet = null;
            if(!dataSetToDicomFiles.TryGetValue(dataSet, out dicomFilesForDataSet))
            {
              dicomFilesForDataSet = new List<string>();
              dataSetToDicomFiles.Add(dataSet, dicomFilesForDataSet);
            } // if(!dataSetToDicomFiles.TryGetValue(dataSet, out dicomFilesForDataSet)

            if(!dicomFilesForDataSet.Contains(file))
            {
              dicomFilesForDataSet.Add(file);
            } // if(!dicomFilesForDataSet.Contains(file))
          } // foreach(string file in Directory.GetFiles(path, "*?.dcm"))
        } // if(subDirectory.ToLower().EndsWith("_dcm"))
      } // foreach(string subDirectory in subDirectories)

      Global.CheckForCancel();

      // Loop through the conglomeration.
      foreach( string volumeFile in volumeList )
      {
        Global.CheckForCancel();

        string volFileNameLC = volumeFile.ToLower();
        string dataSet = Utils.GetDataSet(volFileNameLC);

        // Create the entry, if needed.
        if( !dataSetToElementList.TryGetValue(dataSet, out image) )
        {
          // Make sure the volume file has a dicom header, else log a message.

          try
          {
            image = (Viatronix.Dicom.Image)Viatronix.Dicom.ImageFactory.Create(BaseUtilities.ReadDicomHeaderFromVolumeFile(volumeFile), ParameterType.HEADER);
            dataSetToElementList.Add(dataSet, image);
            dataSetToImageCount.Add(dataSet, BaseUtilities.ReadVolumeDimensionsFromVolumeFile(volumeFile).Item3);

            // If the dataset was in the failed pile from a previous file, then remove it.
            if (dataSetsNoVolumeDicomInfo.Contains(dataSet))
            {
              dataSetsNoVolumeDicomInfo.Remove(dataSet);
            } // if(dataSetsNoVolumeDicomInfo.Contains(dataSet))
          } // try
          catch (Viatronix.Dicom.DicomException e)
          {
            // Log that the file already exists.
            Log.Warning("Error in retrieving Dicom Info from Volume File " + volumeFile + ".  Exception: " + e.Message, "Utils", "GetDicomInfo");

            if (!dataSetsNoVolumeDicomInfo.Contains(dataSet))
            {
              dataSetsNoVolumeDicomInfo.Add(dataSet);
            } // if(!dataSetsNoVolumeDicomInfo.Contains(dataSet))
          } // catch(Viatronix.v3D.Dicom.NotFoundException e)
          catch (Exception e)
          {
            Log.Error(string.Format("Failed to read dicom header from volume file.  [ERROR={0}]", e.Message), "Utils", "GetDicomInfo");
            throw e;
          }
        } // if(!dictionary.TryGetValue(dataSet, out eleList))
      } // foreach( string volumeFile in volumeList )

      Global.CheckForCancel();

      foreach( KeyValuePair<string, List<string>> keypair in dataSetToDicomFiles)
      {
        if( !dataSetToElementList.TryGetValue(keypair.Key, out image) )
        {
          foreach(string dicomFile in keypair.Value)
          {
            try
            {
              // Make sure the volume file has a dicom header, else log a message.

              image = (Viatronix.Dicom.Image)ImageFactory.Create(dicomFile, ParameterType.FILE);

              dataSetToElementList.Add(keypair.Key, image);

              // If the dataset was in the failed pile from a previous file, then remove it.
              if( dataSetsNoVolumeDicomInfo.Contains(keypair.Key) )
              {
                dataSetsNoVolumeDicomInfo.Remove(keypair.Key);
              } // if(dataSetsNoVolumeDicomInfo.Contains(dataSet))

              // Log a warning indicating that the image count will not be available.
              Log.Warning("Image count could not be determined because no volume files with " +
                "Dicom Headers in them could be found for dataset: " + keypair.Key + ".  In Directory: " + source, "Utils", "GetDicomInfo");

              // Created the element list, loop no longer needs to execute.
              break;
            } // try
            catch(DicomException e)
            {
              Log.Warning("Error obtaining dicom header from file: " + dicomFile + ".  Exception: " + e.Message, "Utils", "GetDicomInfo");
            } // catch(DicomException e)
          } // foreach(string dicomFile in keypair.Value)

        } // if( !dataSetToElementList.TryGetValue(dataSet, out eleList) )
      } // foreach(string dicomFile in dicomFiles)

      Global.CheckForCancel();

      // Output an error in the log for each dataSet that didn't get added.
      foreach(string dataSet in dataSetsNoVolumeDicomInfo)
      {
        string namedDataSet = dataSet;

        string[] dataSetFiles = null;

        if((dataSetFiles = Directory.GetFiles(source, "*" + dataSet + "*.*")).Length > 0)
        {
          namedDataSet = dataSetFiles[0].Substring(0, dataSetFiles[0].IndexOf(dataSet));
        } // if((dataSetFiles = Directory.GetFiles(source, "*" + dataSet + "*.*")).Length > 0)

        Utils.LogMessage(LogTypes.Error, "< " + source + " >; Import of Data Set Failed; " +
          "Unable to locate valid Dicom Header for Data Set: " + namedDataSet);

        Log.Error("< " + source + " >; Import of Data Set Failed; Unable to locate valid Dicom Header for Data Set: " + namedDataSet
          , "Utils", "GetDicomInfo");
      } // foreach(string dataSet in dataSetsNoVolumeDicomInfo)
    } // GetDicomInfo(string path)


    /// <summary>
    /// Gets the position the study had taken place on.
    /// </summary>
    /// <param name="fileName">The path to the file we are examining.</param>
    /// <returns>'p' for Prone or 's' for Supine.</returns>
    public static char GetPositon(string fileName)
    {
      string fileNameLC = Path.GetFileNameWithoutExtension(fileName.ToLower());

      // Search the array, back to front looking for a character sequence equaling _?(p or s)
      for (int i = fileNameLC.Length - 3; i >= 0; --i)
      {
        // Was this index satisfactory?
        if( fileNameLC[i].Equals('_') && ( fileNameLC[i + 2].Equals('p') || fileNameLC[i + 2].Equals('s') ) )
        {
          return fileNameLC[i + 2];
        } // if (searching[i].Equals('_') && (searching[i + 2].Equals('p') ...
      } // for (int i = searching.Length - 3; i >= 0 && studyPosition.Equals('\0'); --i)

      return 'u';
    } // GetPositon(string fileName)


    /// <summary>
    /// Finds the slice width of a given file from it's filename.
    /// </summary>
    /// <param name="fileName">Absolute path to the file.</param>
    /// <returns>An integer containing the slice width in mm.</returns>
    public static int GetSliceWidth(string fileName)
    {
      string fileNameLC = Path.GetFileNameWithoutExtension(fileName.ToLower());

      for (int i = fileNameLC.Length - 3; i >= 0; i--)
      {
        // Is there a number between either _ and (p or s)?
        if (fileNameLC[i].Equals('_') && (fileNameLC[i + 1] >= '0' && fileNameLC[i + 1] <= '9') && (fileNameLC[i + 2].Equals('p') || fileNameLC[i + 2].Equals('s')))
        {
          // If the sequence was found, return it.
          return System.Convert.ToInt32(fileNameLC.Substring(i + 1, 1));
        } // if (searching[i].Equals('_') && (searching[i + 1] >= '0'
      } // for (int i = searching.Length - 3; i >= 0 && result == -1; i--)

      // If the sequence wasn't found, return -1.
      return -1;
    } // GetSliceWidth(string fileName)


    /// <summary>
    /// Returns the dataset of a given filename.
    /// </summary>
    /// <param name="fileName">The file to examine.</param>
    /// <returns>The dataset eg "1p" and so on or "Unknown".</returns>
    public static string GetDataSet(string fileName)
    {
      string dataSet = GetSliceWidth(fileName).ToString() + GetPositon(fileName);

      return dataSet == "-1u" ? "Unknown" : dataSet;
    } // GetDataSet(string fileName)


    /// <summary>
    /// Creates the log file, and initializes the Logger.
    /// </summary>
    /// <exception cref="System.ApplicationException">If the log directory does not exist.</exception>
    public static void CreateLogFile()
    {
      if (!Directory.Exists(Utils.LogDirectory))
      {
        Directory.CreateDirectory(Utils.LogDirectory);
      } // if( !Directory.Exists(path) )

      m_logFileName = Path.Combine(Utils.LogDirectory, "Importer-" + (DateTime.Now.ToString("yyyy-MM-dd-hh-mm-ss") + ".txt"));

      if( File.Exists(m_logFileName) )
      {
        for( int i = 0; i < 4096; ++i )
        {
          string logFileNameCandidate = Path.Combine(Utils.LogDirectory, ( DateTime.Now.ToString("yyyy-MM-dd-hh-mm-ss") +
            "_" + i.ToString() + ".txt" ));

          if( !File.Exists(logFileNameCandidate) )
          {
            m_logFileName = logFileNameCandidate;
            break;
          }
        }
      }
    } // CreateLogFile()


    /// <summary>
    /// Logs a message out to a concise log and also uses the Viatronix.v3D.Logging.Logger.
    /// </summary>
    /// <param name="logType">The category of message that should be written.</param>
    /// <param name="message">The message that should be written out.</param>
    public static void LogMessage(LogTypes logType, string message)
    {
      if(m_logFileName != null)
      {
        try
        {
          string logDirectory = Path.GetDirectoryName(m_logFileName);
          if (!Directory.Exists(logDirectory))
          {
            Directory.CreateDirectory(logDirectory);
          }

          using( TextWriter writer = new StreamWriter(m_logFileName, true) )
          {
            writer.WriteLine(DateTime.Now.ToString() + ";" + logType.ToString() + ";" + message);
            writer.Flush();
          } // using( TextWriter writer = new StreamWriter(Utils.GetLogFileName(), true) )
        }
        catch(Exception)
        {
        }
      }
    } // LogMessage(LogTypes logType, string message)


    /// <summary>
    /// Logs message to intermediate log.
    /// </summary>
    /// <param name="logType">Type of message.</param>
    /// <param name="series">Series in question.</param>
    /// <param name="message">Message.</param>
    public static void LogMessage(LogTypes logType, Series series, string message)
    {
      LogMessage(logType, series.StudyUid.Replace(".", "_") + "; " + series.Uid.ToString() + "; " + message);
    } // LogMessage(LogTypes logType, Series series, string message)


    /// <summary>
    /// Logs message to intermediate log.
    /// </summary>
    /// <param name="logType">Type of message.</param>
    /// <param name="patient">Patient in question.</param>
    /// <param name="message">Message.</param>
    public static void LogMessage(LogTypes logType, Patient patient, string message)
    {
      LogMessage(logType, patient.Name.LastFirst + "; " + message);
    } // LogMessage(LogTypes logType, Patient patient, string message)


    /// <summary>
    /// Logs message to intermediate log.
    /// </summary>
    /// <param name="logType">Type of message.</param>
    /// <param name="patient">Patient in question.</param>
    /// <param name="message">Message.</param>
    /// <param name="exceptionMessage">Error message.</param>
    public static void LogMessage(LogTypes logType, Patient patient, string message, string exceptionMessage)
    {
      LogMessage(logType, patient.Name.LastFirst + "; " + message + "; Exception: " + exceptionMessage);
    } // LogMessage(LogTypes logType, Patient patient, string message, string exceptionMessage)


    /// <summary>
    /// Creates a patient from a dataset.
    /// </summary>
    /// <param name="image">The dataset for the study.</param>
    /// <returns>A patient from the dataset.</returns>
    public static Patient CreatePatient(Image image)
    {
      Patient patient = new Patient();

      try
      {
        patient.Dob = Viatronix.Dicom.Utility.ToDateTime(image.PatientDob, "");
      }
      catch ( Exception )
      {
        Log.Warning("No patient DOB found, will use patient default.", "Utils", "CreatePatient");
      }

      patient.Gender      = Patient.ConvertToGender(image.PatientSex);
      patient.Weight      = (int)image.PatientWeight;
      patient.MedicalId   = image.MedicalId;
      patient.Name        = new PersonName(image.PatientName);

      return patient;
    } // CreatePatient( dataset )


    /// <summary>
    /// Creates a study from a dataset.
    /// </summary>
    /// <param name="image">The image for the study.</param>
    /// <returns>A study from the dataset.</returns>
    public static Study CreateStudy(Image image)
    {
      Study study = new Study();

      study.Physician           = image.ReferringPhysician;
      study.AccessionNumber     = image.AccessionNumber;
      study.Description         = image.StudyDescription;
      study.Date                = Viatronix.Dicom.Utility.GetScanDate(image);
      study.Id                  = image.StudyId;
      study.Uid                 = image.StudyInstanceUid;

      return study;
    } // CreateStudy( image )


    /// <summary>
    /// Creates a series from a dataset.
    /// </summary>
    /// <param name="image">The image for the series.</param>
    /// <returns>A series from the dataset.</returns>
    public static Series CreateSeries(Image image)
    {
      Series series = new Series();

      //series.AcquisitionNumber  = dataset.AcquisitionNumber;

      try
      {
        series.Date  = image.SeriesDateTime;
      }
      catch ( Exception )
      {
        Log.Warning("Failed to set the series date, using the scan date", "Utils", "CreateSeries");
        series.Date = Viatronix.Dicom.Utility.GetScanDate(image);
      }

      DateTime today = DateTime.Now;

      //series.DateReceived       = today;
      //series.DateProcessed      = today;
      series.Description        = image.SeriesDescription;
      series.Number             = image.SeriesNumber;
      series.Modality           = image.Modality;
      series.PatientOrientation = Viatronix.Dicom.Utility.GetImageOrientation(image.ImageOrientation).ToString();
      series.Type               = SeriesTypes.Dicom;
      series.Process            = "Receiving";
      series.State              = States.None;
      //series.DateStoredToPacs   = series.Date;
      series.Uid                = image.InstanceUid;

      return series;
    } // CreateSeries( image )

    #endregion

  } // Utils
} // namespace Viatronix.Utilities.Importer

#region revision history

// $Log: Utils.cs,v $
// Revision 1.1.2.11  2011/05/19 16:02:42  kchalupa
// CONS-1640
// Make Importer specific log directory a static.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.10  2011/05/18 17:58:02  kchalupa
// CONS-1640
// Change original base path reference to new data structure.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.9  2011/05/06 15:12:22  kchalupa
// Made logger settings configurable from the config file.  This includes the conscise log and verifier log.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.8  2009/10/30 14:03:20  kchalupa
// Added cancel functionality.
//
// Revision 1.1.2.7  2009/10/26 19:43:19  kchalupa
// Added CD Import commandline importer arg value.
//
// Revision 1.1.2.6  2009/10/20 14:00:42  kchalupa
// Added function to remove read only attribute on imported files for DVD Import.  Also restricted import on CD, study and series directories.
//
// Revision 1.1.2.5  2009/06/04 19:03:30  kchalupa
// Added code to properly add logging for status form; coding standards.
//
// Revision 1.1.2.4  2009/02/25 15:15:56  kchalupa
// Catch exception in LogMessage for Logging.
//
// Revision 1.1.2.3  2008/10/20 17:39:37  kchalupa
// Don't output debug info.
//
// Revision 1.1.2.2  2008/10/08 18:00:46  kchalupa
// Added login/context feature for luanching from Console and stand-alone
//
// Initial 1.0 6/30/2008  kchalupa
// Initial Version.
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities.Importer/Utils.cs,v 1.1.2.11 2011/05/19 16:02:42 kchalupa Exp $
// $Date: 2011/05/19 16:02:42 $23 July 2008
// $Id: Utils.cs,v 1.1.2.11 2011/05/19 16:02:42 kchalupa Exp $000000004

#endregion
