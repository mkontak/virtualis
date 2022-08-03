// $Id: ImportManager20.cs,v 1.1.2.6 2009/10/30 14:09:09 kchalupa Exp $
//
// Copyright © 2008, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Kyle R Chalupa] [kchalupa@viatronix.com]

using System;
using System.IO;
using System.Collections.Generic;
using System.Text;

using Viatronix.Dicom;
using Viatronix.Logging;
using Viatronix.Enterprise.Entities;
using Viatronix.Utilities;
using Viatronix.Utilities.IO;
using Viatronix.Utilities.Importer.Data;

namespace Viatronix.Utilities.Importer
{
  /// <summary>
  /// A class for managing the higher level aspects of series creation and the addition of files into their
  /// respective series.  The logic for the decision-making is handled elsewhere, however.
  /// </summary>
  public class ImportManager20 : IImporter
  {

    #region fields

    /// <summary>
    /// The paitient to which the current data applies.
    /// </summary>
    private Patient m_patient = null;

    /// <summary>
    /// The current study that applies to the above paitient.
    /// </summary>
    private ParsedStudy m_parsedStudy = null;

    /// <summary>
    /// The list of series' that have already been encountered.  They should be added to if applicable.
    /// </summary>
    private Dictionary<string, SortedDictionary<SeriesTypes, ParsedSeries>> m_dataSetToSeries = null;

    /// <summary>
    /// The session parsed series.
    /// </summary>
    private ParsedSeries m_sessionSeries = null;
    
    /// <summary>
    /// A map from the dataset to the DICOM image count.
    /// </summary>
    private Dictionary<string, int> m_dataSetToImageCount = null;

    /// <summary>
    /// Maps the data set to the proper element list.  Extracted from either a .dcm, .vol, or .ecv file.
    /// The logic for populating this map is included in the Filter classes.
    /// </summary>
    private Dictionary<string, Image> m_dataSetToElementList = null;

    /// <summary>
    /// Holds a collection of master series that can be cloned as opposed to being forced to create new
    /// series from the element lists every time.
    /// </summary>
    private Dictionary<string, Series> m_dataSetToMasterSeries = null;

    /// <summary>
    /// Whether or not the object was initialized.
    /// </summary>
    private bool m_initialized = false;

    /// <summary>
    /// Counter used for the series directories.
    /// </summary>
    //private int m_counter = 0;

    #endregion

    #region construction

    /// <summary>
    /// Default constructor.  User should call the Initialize method afterward.
    /// </summary>
    public ImportManager20()
    {
      m_dataSetToSeries = new Dictionary<string, SortedDictionary<SeriesTypes, ParsedSeries>>();
      m_dataSetToElementList = new Dictionary<string, Image>();
      m_dataSetToMasterSeries = new Dictionary<string,Series>();
      m_dataSetToImageCount = new Dictionary<string,int>();
    } // Importer()

    #endregion

    #region properties

    /// <summary>
    /// Accessor for the patient object embedded in the object.
    /// </summary>
    public Patient Patient
    {
      get { return m_patient; }
    } // Patient Property

    /// <summary>
    /// Accessor for the parsed study that will be imported.
    /// </summary>
    public ParsedStudy StudyToImport
    {
      get { return m_parsedStudy; }
    } // StudyToImport Property

    /// <summary>
    /// Accessor for the dataset dictionary.
    /// </summary>
    public Dictionary<string, SortedDictionary<SeriesTypes, ParsedSeries>> DataSets
    {
      get { return m_dataSetToSeries; }
    } // DataSets Property

    #endregion

    #region methods

    /// <summary>
    /// Initializes the ImportManager, filling out the element lists it should contain and so forth.
    /// </summary>
    /// <param name="args">The path that intialize should explore.</param>
    /// <returns>Whether or not the folder has any dicom information.</returns>
    public void Initialize(ImporterArgs args)
    {
      try
      {
        Global.CheckForCancel();
        Global.NotifyProgress("Extracting DICOM Information", 5);
                
        Utils.GetDicomInfo(args.Source, m_dataSetToElementList, m_dataSetToImageCount);

        Global.CheckForCancel();
        Global.NotifyProgress("Creating Viatronix Study Information", 5);

        // Fill in patient and study and break out.
        foreach (KeyValuePair<string, Image> pair in m_dataSetToElementList)
        {
          Global.CheckForCancel();

          // Create the patient and study based on the element list.
          m_patient = Utils.CreatePatient(pair.Value);
          m_parsedStudy = ParsedStudy.Create(Utils.CreateStudy(pair.Value));

          m_parsedStudy.Study.Name = m_patient.Name;
          m_parsedStudy.Study.MedicalId = m_patient.MedicalId;
          m_parsedStudy.Study.Dob = m_patient.Dob;
          m_parsedStudy.Study.Gender = m_patient.Gender;
          m_parsedStudy.Study.Weight = m_patient.Weight;

          // Link the two together.
          m_patient.Studies.Add(m_parsedStudy.Study);

          // Mark the manager as initialized so we can add files to it and perform copy operations.
          m_initialized = true;

          break;
        } // foreach (KeyValuePair<string, ElementList> pair in m_dataSetToElementList)

        Global.CheckForCancel();

        if (m_dataSetToElementList.Count < 1)
        {
          Utils.LogMessage(LogTypes.Error, string.Format("Import Failed; No files contained held dicom information.  [DIRECTORY={0}]", args.Source));
          throw new ApplicationException("No files held dicom information");
        }
      }
      catch (Exception e)
      {
        throw e;
      }
    } // Initialize( args )


    /// <summary>
    /// Adds the specified file pointed to by the info structure.
    /// </summary>
    /// <param name="info">Object to provide source and destination info.</param>
    /// <param name="parser">The parser reading the file.</param>
    public void AddFile(Viatronix.Utilities.Importer.Data.FileParseInfo info, string parser)
    {
      AddFile(info.Dataset, info.SeriesType, parser, info.Source, info.Destination, info.MacroReplace);
    } // AddFile( info, parser )


    /// <summary>
    /// Puts the file into the appropriate bin using the provided parameters.
    /// </summary>
    /// <remarks>Searches are case sensative.</remarks>
    /// <param name="dataSet">"1s", "3p" and so on.</param>
    /// <param name="seriesType">"Dicom", "Volume"...</param>
    /// <param name="source">The absolute source where the file is located including the filename.</param>
    /// <param name="destination">The relative destination excluding the filename.</param>
    /// <param name="macroReplace">The text that should replace the placeholder character. Preferably a UID</param>
    public void AddFile( string dataSet, SeriesTypes seriesType, string parser, string source, string destination, string macroReplace )
    {
      if( !m_initialized )
        throw new ApplicationException("ImportManager was not yet initialized.");

      if(source == null || source == string.Empty)
      {
        throw new ArgumentException("Invalid source was specified.");
      } // if(source == null || source == string.Empty)

      // Will come back null if a file hasn't already been added, create if null.
      ParsedSeries addToSeries = GetParsedSeries(dataSet, seriesType);

      if(addToSeries == null)
      {
        addToSeries = CreateParsedSeries(dataSet, seriesType, parser);
      }
      
      if(addToSeries != null)
      {
        addToSeries.AddFile(source, destination, macroReplace);
      } // if(addToSeries != null)
      else
      {
        // No element list exists for the dataSet or user passed in a bogus seriesType.
        Log.Debug("Invalid Data Set or Series Type requested for call to Add Series.", "ImportManager20", "AddFile");
        throw new ArgumentException("Invalid DataSet or SeriesType requested.");
      } // else
    } // AddFile()


    /// <summary>
    /// Puts the file into the appropriate bin using the provided parameters.
    /// </summary>
    /// <remarks>Searches are case sensative.</remarks>
    /// <param name="dataSet">"1s", "3p" and so on.</param>
    /// <param name="seriesType">"Dicom", "Volume"...</param>
    /// <param name="source">The absolute source where the file is located including the filename.</param>
    /// <param name="destination">The relative destination excluding the filename.</param>
    /// <param name="macroReplace">The text that should replace the placeholder character. Preferably a UID</param>
    public void AddVirtualFile(string dataSet, SeriesTypes seriesType, string parser, string contents, string destination, string macroReplace)
    {
      if( !m_initialized )
        throw new ApplicationException("ImportManager was not yet initialized.");

      if( contents == null || contents == string.Empty )
      {
        throw new ArgumentException("No contents was specified.");
      } // if(source == null || source == string.Empty)

      ParsedSeries addToSeries = GetParsedSeries(dataSet, seriesType);

      // If a file had not been added yet, create the series.
      if( addToSeries == null )
      {
        addToSeries = CreateParsedSeries(dataSet, seriesType, parser);
      } // if( addToSeries == null )

      // If the parsed series is null this time, throw an exception.
      if( addToSeries != null )
      {
        addToSeries.AddVirtualFile(contents, destination, macroReplace);
      } // if(addToSeries != null)
      else
      {
        Log.Debug("Invalid Data Set or Series Type requested for call to Add Series.", "Filter20", "AddVirtualFile");
        throw new ArgumentException("Invalid Data Set or Series Type requested for call to Add Series.");
      } // else
    } // AddFile()


    /// <summary>
    /// Looks in the series matrix for the desired dataSet/seriesType match and returns the UID if it exits.
    /// If the series doesn't exist, it creates it.
    /// </summary>
    /// <param name="dataSet">"1p", "3s" case sensative.</param>
    /// <param name="seriesType">"Dicom", "volume" case insensative.</param>
    /// <param name="delimeter">The placeholder character in the UID, _ , . and so on.</param>
    /// <returns>The desired UID if the Series exists or an empty string.</returns>
    public string GetSeriesUID(string dataSet, SeriesTypes seriesType, string delimeter)
    {
      string uidResult = string.Empty;
      ParsedSeries parsedSeries = GetParsedSeries(dataSet, seriesType);

      if( parsedSeries != null )
      {
        uidResult = parsedSeries.Series.Uid.Replace(".", delimeter);
      } // if(addToSeries != null)
      else
      {
        Log.Debug("Invalid Data Set or Series Type requested for call to Get Series UID.", "Filter20", "GetSeriesUID");
        throw new ArgumentException("Invalid Data Set or Series Type requested for call to Get Series UID.");
      } // else

      return uidResult;
    } // GetSeriesUID()


    /// <summary>
    /// Creates a Parsed Series from the provided data.
    /// </summary>
    /// <param name="dataSet"></param>
    /// <param name="seriesType"></param>
    /// <param name="parser"></param>
    /// <returns></returns>
    private ParsedSeries CreateParsedSeries(string dataSet, SeriesTypes seriesType, string parser)
    {
      ParsedSeries parsedSeriesResult = null;

      if(seriesType == SeriesTypes.Session)
      {
        // If the object doesn't exist, create it and initialize the proper fields.
        if( m_sessionSeries == null )
        {
          // Fill out the parsed series object.
          m_sessionSeries = new ParsedSeries(new Series());
          m_sessionSeries.Series.Type = seriesType;
          m_sessionSeries.Series.Description = seriesType + " Series";

          m_sessionSeries.Series.Uid = Viatronix.Dicom.Utility.GenerateUid();

          // Empty out the processing property.
          m_sessionSeries.Series.Process = string.Empty;

          // DateProcessed's minimal value caused problems when adding to the database.
          m_sessionSeries.Series.DateProcessed = DateTime.Now;

          // Fill in SubType.
          m_sessionSeries.Series.Application = parser;

          // Link the series to the study and vice versa.
          m_parsedStudy.AddSeries(m_sessionSeries);
          m_parsedStudy.Study.Series.Add(m_sessionSeries.Series);
          m_sessionSeries.ParsedStudy = m_parsedStudy;
          m_sessionSeries.Series.StudyUid = m_parsedStudy.Study.Uid;
        } // if( m_sessionSeries == null )

        // Set the result to the session series.
        parsedSeriesResult = m_sessionSeries;

      }
      else
      {
        // Look for the proper data set dictionary.
        SortedDictionary<SeriesTypes, ParsedSeries> parsedSeriesDictionary = null;
        if( !m_dataSetToSeries.TryGetValue(dataSet, out parsedSeriesDictionary) )
        {
          // If there wasn't an element list to correspond to the data set, log a warning and bail out.
          if( !m_dataSetToElementList.ContainsKey(dataSet) )
          {
            Log.Debug("Invalid Data Set or Series Type requested for call to Create Series.", "ImportManager20", "CreateParsedSeries");
            throw new ArgumentException("Invalid Data Set or Series Type requested for call to Create Series.");
          } // if(!m_dataSetToElementList.ContainsKey(dataSet))

          // If a series dictionary wasn't found, create one.
          parsedSeriesDictionary = new SortedDictionary<SeriesTypes, ParsedSeries>(new SeriesTypeComparer());
          m_dataSetToSeries.Add(dataSet, parsedSeriesDictionary);
        }

        // Look for the particular series that the file should be added to.
        ParsedSeries parSeries = null;
        if( !parsedSeriesDictionary.TryGetValue(seriesType, out parSeries) )
        {
          // Get the appropriate element list for the data set to create the series.
          Image image = null;
          if (m_dataSetToElementList.TryGetValue(dataSet, out image))
          {
            Series masterSeries = null;
            if( !m_dataSetToMasterSeries.TryGetValue(dataSet, out masterSeries) )
            {
              masterSeries = Utils.CreateSeries(image);
              masterSeries.StudyUid = m_parsedStudy.Study.Uid;
              // Clear out the processing property.
              masterSeries.Process = string.Empty;

              // Set the image count.
              int imageCount = 0;
              m_dataSetToImageCount.TryGetValue(dataSet, out imageCount);

              masterSeries.ImageCount = imageCount;

              // Add the series to the master series list.
              m_dataSetToMasterSeries.Add(dataSet, masterSeries);
            } // if(!m_dataSetToMasterSeries.TryGetValue(dataSet, out masterSeries))

            Series castFromMaster = ( Series )masterSeries.Clone();

            // Create the parsed series based on the element list.
            parSeries = new ParsedSeries(castFromMaster);

            // Carry out these actions for non dicom series'.
            if(seriesType != SeriesTypes.Dicom)
            {
              // Generate the UID and mark the RreprocessAs.
              parSeries.Series.Uid = Viatronix.Dicom.Utility.GenerateUid();

              // Fix for volume series that only have ECV files.  If a vol file is found,
              // the subType will be cleared to "".
              parSeries.Series.Application = parser;
            } // if( seriesTypeLC != "dicom" )
              
            // Link up the series to the study.
            m_parsedStudy.AddSeries(parSeries);
            m_parsedStudy.Study.Series.Add(parSeries.Series);

            // Set the series type and description.
            parSeries.Series.Description = seriesType + " Series";
            parSeries.Series.Type = seriesType;

            parSeries.Series.State = States.Completed;
            parSeries.ParsedStudy = m_parsedStudy;

            parSeries.Series.DateProcessed = DateTime.Now;

            // Add the new parsed series to the dictionary.
            parsedSeriesDictionary.Add(seriesType, parSeries);

          } // if (m_dataSetToElementList.TryGetValue(dataSet, out eleList))
          else
          {
            // Log a message that there was an error in creating the series.
            Utils.LogMessage(LogTypes.Error, m_parsedStudy.Study + "DataSet " + dataSet + 
              "; Import Failed; No valid element list does not exist for it");

            Log.Error(m_patient.Name + ";" + m_parsedStudy.Study.Uid.Replace(".", "_") + 
              "; DataSet " + dataSet + "; Import Failed; No valid element list does not exist for it", "ImportManager20", "CreateParsedSeries");

            throw new ArgumentException("ElementList does not exist for specified data set " + dataSet + " on series creation.");
          }
        }

        // Set the result to the located or created ParsedSeries.
        parsedSeriesResult = parSeries;
      }

      return parsedSeriesResult;
    } // CreateParsedSeries(string dataSet, string seriesType, string parser)


    /// <summary>
    /// Returns the parsed series that is part of the dataSet parameter and also has same type as seriesType.
    /// </summary>
    /// <param name="dataSet">The DataSet of the requested ParsedSeries.</param>
    /// <param name="seriesType">The SeriesType of the requested ParsedSeries.</param>
    /// <param name="parser">Needed in the event that the ParsedSeries didn't already exist.</param>
    /// <returns>The ParsedSeries if it exists or null.</returns>
    /// <exception cref="System.ArgumentException">
    /// If the dataset/seriesType combination does not exist.
    /// </exception>
    public ParsedSeries GetParsedSeries(string dataSet, SeriesTypes seriesType)
    {
      ParsedSeries parsedSeriesResult = null;
      if(seriesType == SeriesTypes.Session)
      {
        // Set the result to the session series.
        parsedSeriesResult = m_sessionSeries;
      } // if( seriesTypeLC == "session" )
      else
      {
        // Look for the proper data set dictionary.
        SortedDictionary<SeriesTypes, ParsedSeries> parsedSeriesDictionary = null;
        if( !m_dataSetToSeries.TryGetValue(dataSet, out parsedSeriesDictionary) )
        {
          // If there wasn't an element list to correspond to the data set, log a warning and bail out.
          if( !m_dataSetToElementList.ContainsKey(dataSet) )
          {
            Log.Error("No ElementList exists for requested Data Set: " + dataSet, "ImportManager20", "GetParsedSeries");

            throw new ArgumentException("No ElementList exists for requested Data Set: " + dataSet);
          } // if(!m_dataSetToElementList.ContainsKey(dataSet))

          // If a series dictionary wasn't found, create one.
          parsedSeriesDictionary = new SortedDictionary<SeriesTypes, ParsedSeries>(new SeriesTypeComparer());
          m_dataSetToSeries.Add(dataSet, parsedSeriesDictionary);
        } // if (!m_seriesList.TryGetValue(sliceWidthAndOrientation, out seriesList))

        // Look for the particular series that the file should be added to.
        ParsedSeries parSeries = null;
        parsedSeriesDictionary.TryGetValue(seriesType, out parSeries);

        // Set the result to the located or created ParsedSeries.
        parsedSeriesResult = parSeries;

      } // else

      return parsedSeriesResult;
    } // GetParsedSeries(string dataSet, string seriesType)


    /// <summary>
    /// Returns the primary volume series.  Created to fix a problem with importing 2.0 VCs.
    /// </summary>
    /// <returns>The primary volume series.</returns>
    public ParsedSeries GetPrimaryVolumeSeries()
    {
      // Used for keeping the lowest sliceSpacing.
      int minimumSliceSpacing = int.MaxValue;
      SortedDictionary<SeriesTypes, ParsedSeries> primaryDataSet = null;

      foreach(KeyValuePair<string, SortedDictionary<SeriesTypes, ParsedSeries>> dataSetToSeriesList in m_dataSetToSeries)
      {
        string dataSetLC = dataSetToSeriesList.Key.ToLower();
        int sliceSpacing = System.Convert.ToInt32(dataSetLC[0].ToString());
        if( dataSetLC.EndsWith("s") && ( sliceSpacing < minimumSliceSpacing ) )
        {
          primaryDataSet = dataSetToSeriesList.Value;
          minimumSliceSpacing = sliceSpacing;
        } // if(dataSetLC.EndsWith("s") && ( sliceSpacing < minimumSliceSpacing ) )
      } // foreach(KeyValuePair<string, Dictionary<string, ParsedSeries>> dataSetToSeriesList in m_dataSetToSeries)

      if(primaryDataSet == null)
      {
        foreach( KeyValuePair<string, SortedDictionary<SeriesTypes, ParsedSeries>> dataSetToSeriesList in m_dataSetToSeries )
        {
          string dataSetLC = dataSetToSeriesList.Key.ToLower();
          int sliceSpacing = System.Convert.ToInt32(dataSetLC[0].ToString());
          if( dataSetLC.EndsWith("p") && ( sliceSpacing < minimumSliceSpacing ) )
          {
            primaryDataSet = dataSetToSeriesList.Value;
            minimumSliceSpacing = sliceSpacing;
          } // if(dataSetLC.EndsWith("s") && ( sliceSpacing < minimumSliceSpacing ) )
        } // foreach(KeyValuePair<string, Dictionary<string, ParsedSeries>> dataSetToSeriesList in m_dataSetToSeries)
      } // if(primaryDataSet = null)

      if(primaryDataSet != null)
      {
        foreach(KeyValuePair<SeriesTypes, ParsedSeries> seriesTypeToParsedSeries in primaryDataSet)
        {
          if(seriesTypeToParsedSeries.Key == SeriesTypes.Dicom)
          {
            return seriesTypeToParsedSeries.Value;
          } // if(seriesTypeToParsedSeries.Key.ToLower() == "volume")
        } // foreach(KeyValuePair<string, ParsedSeries> seriesTypeToParsedSeries in primaryDataSet)

        Log.Error("Volume Series within Primary Series could not be found", "ImportManager20", "GetPrimaryVolumeSeries");
      } // if(primaryDataSet != null)
      else
      {
        Log.Error("Primary Data Set could not be found!", "ImportManager20", "GetPrimaryVolumeSeries");
      } // else

      return null;
    } // GetPrimaryVolumeSeries()


    /// <summary>
    /// Creates the reference links between the various series contained in our study.
    /// </summary>
    /// <param name="ioArgs">Arguments to take into account.</param>
    public void CreateReferenceLinks(ImporterArgs ioArgs)
    {
      if(m_initialized && m_dataSetToSeries != null)
      {
        IComparer<SeriesTypes> comparer = new SeriesTypeComparer();

        foreach( KeyValuePair<string, SortedDictionary<SeriesTypes, ParsedSeries>> keyPair in m_dataSetToSeries )
        {
          foreach(KeyValuePair<SeriesTypes, ParsedSeries> keyPair2 in keyPair.Value)
          {
            System.Console.WriteLine("Series Name: " + keyPair2.Key + " UID: " + keyPair2.Value.Series.Uid );

            // Check to see that the session series exists and add the current series as a reference and
            // make the opposite link.  Do these things only if the copy session flag is set.
            if( m_sessionSeries != null && ioArgs.ImportSessions )
            {
              if(!m_sessionSeries.Series.References.Contains(keyPair2.Value.Series))
              {
                m_sessionSeries.Series.References.Add(keyPair2.Value.Series);
              } // if(!m_sessionSeries.Series.ReferencedSeries.Contains(keyPair2.Value.Series))

              if(!keyPair2.Value.Series.Dependents.Contains(m_sessionSeries.Series))
              {
                keyPair2.Value.Series.Dependents.Add(m_sessionSeries.Series);
              } // if(keyPair2.Value.Series.DependentSeries.Contains(m_sessionSeries))
            } // if(m_sessionSeries != null)

            foreach(KeyValuePair<SeriesTypes, ParsedSeries> keyPair3 in keyPair.Value)
            {
              // If the looping key is less than the current key, then add the current series as a dependent
              // then add the looping series as a reference.  If these weren't already added.
              if(comparer.Compare(keyPair3.Key, keyPair2.Key) < 0)
              {
                if (!keyPair3.Value.Series.Dependents.Contains(keyPair2.Value.Series))
                {
                  keyPair3.Value.Series.Dependents.Add(keyPair2.Value.Series);
                } // if(!keyPair3.Value.Series.DependentSeries.Contains(keyPair2.Value.Series))

                if(!keyPair2.Value.Series.References.Contains(keyPair3.Value.Series))
                {
                  keyPair2.Value.Series.References.Add(keyPair3.Value.Series);
                } // if(!keyPair2.Value.Series.ReferencedSeries.Contains(keyPair3.Value.Series))
              } // if( SeriesTypeComparer.Compare(keyPair3.Key, keyPair2.Key ) < 0)
              else
              {
                break;
              } // else
            } // foreach(KeyValuePair<string, ParsedSeries> keyPair3 in keyPair.Value)
          } // foreach(KeyValuePair<string, ParsedSeries> keyPair2 in keyPair.Value)
        } // foreach(KeyValuePair<string, SortedDictionary<string, ParsedSeries>> keyPair in m_seriesList)
      } // foreach( KeyValuePair<string, SortedDictionary<string, ParsedSeries>> keyPair in m_seriesList )
    } // CreateReferenceLinks(ImporterArgs ioArgs)


    /// <summary>
    /// Initiates a copy command cascade catastrophe to the individual parsed series
    /// objects contained herein.  Both the regular m_seriesList and m_sessionSeries are copied.
    /// </summary>
    /// <param name="ioArgs">Defines the destination, whether the copy, database should be performed
    /// and whether the session should be included.</param>
    public void Copy(ImporterArgs args)
    {
      // Create reference links.
      CreateReferenceLinks(args);

      if(m_parsedStudy != null &&  m_dataSetToSeries != null)
      {
        // Sort the ParsedSeries in the list so that the dependent series will be added first into the 
        // database.
        SeriesTypeComparer comparer = new SeriesTypeComparer();
        m_parsedStudy.ParsedSeriesList.Sort(comparer);

        //m_patient.Directory = string.Join("_", m_patient.Name.Replace(' ', '_'), m_patient.Name.Replace(' ', '_'), m_patient.MedicalId);
        m_parsedStudy.Study.Directory = m_parsedStudy.Study.Uid.Replace(".", "_");

        string studyDirectory = Path.Combine(args.Destination, m_parsedStudy.Study.Directory);

        try
        {
          if (args.PerformCopy)
          {
            if (Directory.Exists(studyDirectory))
            {
              Log.Error(m_patient.Name + "; Import Failed; Patient directory already exists.", "ImportManager20", "Copy");

              Global.LogPatientImportFailed(args.Source);

              return;
            }
            else
            {
              // Create the Patient Directory and Serialize out the patient.
              if (!Directory.Exists(studyDirectory))
              {
                Directory.CreateDirectory(studyDirectory);
              } // if( !Directory.Exists(patientDirectory) )
            }
          }
          else
          {
            // 2.0 style directories require copy bit.
            Global.LogPatientImportFailed(args.Source);
            Utils.LogMessage(LogTypes.Error, "Perform copy was not stipluated, 2.0 style directories require that option.");
            Log.Error("Perform copy was not stipluated, 2.0 style directories require that option.", "ImportManager20", "Copy");
            throw new Exception("Perform copy was not stipluated, 2.0 style directories require that option.");
          }

          // Get the total file count.
          int totalFileCount = 0;
          foreach (ParsedSeries pSeries in m_parsedStudy.ParsedSeriesList)
          {
            totalFileCount += pSeries.Files.Count;
          }

          int remaining = totalFileCount;
          foreach (ParsedSeries pSeries in m_parsedStudy.ParsedSeriesList)
          {
            pSeries.Series.Directory = Series.GenerateDirecrtoryName(pSeries.Series);
            //pSeries.Series.Location = Path.Combine(m_patient.Directory, m_parsedStudy.Study.Directory, pSeries.Series.Directory);
            //pSeries.Series.FullPath = Path.Combine(args.Destination, pSeries.Series.Location);

            // Make sure either the session can be added or if not, that this series that we are dealing with
            // is not the session.
            if (args.ImportSessions || (!args.ImportSessions && pSeries.Series.Type != SeriesTypes.Session))
            {
              // Perform the copy.
              if (args.PerformCopy)
              {
                try
                {
                  pSeries.Copy(args, totalFileCount, remaining);

                  remaining -= pSeries.Files.Count;
                }
                catch (Exception e)
                {
                  Utils.LogMessage(LogTypes.Error, pSeries.Series, "Failed to perform copy [ERROR=" + e.Message + "]");
                  throw;
                }
              }
            }
            else
            {
              Log.Warning(m_patient.Name + "; " + m_parsedStudy.Study.Uid.Replace(".", "_") + pSeries.Series.Uid.Replace(".", "_") + "; " +
                "Import Failed; Importing of Session was not performed at the discretion of the user.", "ImportManager20", "Copy");
            }
          }

          foreach (ParsedSeries parSeries in m_parsedStudy.ParsedSeriesList)
          {
            List<string> volumeFilesAdded = new List<string>();

            if (parSeries.Series.Type == SeriesTypes.Dicom)
            {
              // Iterate through the volume files and add them to the dicom series' volumes list.

              int count = 1;
              string[] volumeFiles = Directory.GetFiles(Path.Combine(studyDirectory, parSeries.Series.Directory, "volumes"), "*.vol");
              foreach (string volumeFile in volumeFiles)
              {
                string volumeFileWithoutExt = Path.GetFileNameWithoutExtension(volumeFile);
                if (!volumeFilesAdded.Contains(volumeFileWithoutExt))
                {
                  volumeFilesAdded.Add(volumeFileWithoutExt);
                  parSeries.Series.CreateVolume("1", string.Format("volume{0:3D}", count++), null, volumeFile, -1);

                }
              }

              volumeFiles = Directory.GetFiles(Path.Combine(studyDirectory, parSeries.Series.Directory, "volumes"), "*.ecv");
              foreach (string volumeFile in volumeFiles)
              {
                string volumeFileWithoutExt = Path.GetFileNameWithoutExtension(volumeFile);
                if (!volumeFilesAdded.Contains(volumeFileWithoutExt))
                {
                  volumeFilesAdded.Add(volumeFileWithoutExt);
                  parSeries.Series.CreateVolume("1", string.Format("volume{0:3D}", count++), null, volumeFile, -1);
                }
              }

              volumeFiles = Directory.GetFiles(Path.Combine(studyDirectory, parSeries.Series.Directory, "volumes"), "*_hq");
              foreach (string volumeFile in volumeFiles)
              {
                string volumeFileWithoutExt = Path.GetFileNameWithoutExtension(volumeFile);
                if (!volumeFilesAdded.Contains(volumeFileWithoutExt))
                {
                  volumeFilesAdded.Add(volumeFileWithoutExt);
                  parSeries.Series.CreateVolume("1", string.Format("volume{0:3D}", count++), null, volumeFile, -1);
                }
              }
            }
          }

          foreach (ParsedSeries pSeries in m_parsedStudy.ParsedSeriesList)
          {
            // Perform the database insert/update.
            if (args.PerformDatabaseRecord)
            {
              pSeries.PerformDatabaseInsert(args);
            }

            if (args.PerformCopy || args.PerformDatabaseRecord)
            {
              // If either action was taken, log a message that the import was successful.

              Utils.LogMessage(LogTypes.Info, pSeries.Series, "Imported");

              Log.Information(m_patient.Name + "; " + m_parsedStudy.Study.Uid.Replace(".", "_") +
                "; " + pSeries.Series.Uid.ToString() + "; Imported", "ImportManager20", "Copy");
            }
            else
            {
              Log.Warning(m_patient.Name + "; " + m_parsedStudy.Study.Uid.Replace(".", "_") +
                pSeries.Series.Uid.Replace(".", "_") + "; " + "Importing Failed; Nothing to be done.", "ImportManager20", "Copy");
            }
          }

          Global.LogPatientImportSucceeded(args.Source);
        }
        catch (Exception)
        {
          Global.LogPatientImportFailed(args.Source);
          Log.Error("Import failed, cleaning up.  Path " + Path.Combine(args.Destination, m_patient.Directory), "ImportManager20", "Copy");

          try
          {
            if(Directory.Exists(studyDirectory))
            {
              PathUtilities.DeleteDirectory(studyDirectory);
            }
          }
          catch (Exception e)
          {
            Log.Error("Failed to clean up after import failure; " + e.Message, "ImportManager20", "Copy");
          }
        }
      }
    } // Copy( args )

    #endregion

  } // ImportManager20
} // namespace Viatronix.Utilities.Importer

#region revision history

// $Log: ImportManager20.cs,v $
// Revision 1.1.2.6  2009/10/30 14:09:09  kchalupa
// Check for copy bit and that the dest directory does not exists, if either fail.  Also clean up if copy failure occurs.
//
// Revision 1.1.2.5  2009/06/04 19:27:01  kchalupa
// Coding standards, slimmed down mini log.
//
// Revision 1.1.2.4  2008/10/20 16:06:21  kchalupa
// Fix for VC Sessions to remove extra datasets that the user may have sent that threw off the VRX file's integrity.
//
// Revision 1.1.2.3  2008/10/17 15:44:09  kchalupa
// Fix for VC Session files not being read in properly in VC.
//
// Revision 1.1.2.2  2008/10/08 18:00:46  kchalupa
// Added login/context feature for luanching from Console and stand-alone
//
// Revision 1.1 09/10/2008 14:09:57  kchalupa
// Added code to mark the subtype if the volume series
// doesn't have any .vol files within it.
//
// Initial 1.0 8/01/2008  kchalupa
// Initial Version.
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities.Importer/ImportManager20.cs,v 1.1.2.6 2009/10/30 14:09:09 kchalupa Exp $
// $Date: 2009/10/30 14:09:09 $23 July 2008
// $Id: ImportManager20.cs,v 1.1.2.6 2009/10/30 14:09:09 kchalupa Exp $000000004

#endregion
