// $Id: VASFilter20.cs,v 1.1.2.2 2009/06/04 18:47:57 kchalupa Exp $
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
using System.Collections.Generic;
using System.Text;

using Viatronix.Logging;
using Viatronix.Utilities.Importer.Data;

namespace Viatronix.Utilities.Importer.Filters
{
  /// <summary>
  /// Processes Vascular 2.0 studies and converts them into the new V3D 3.0 file structure and inserts it
  /// into the V3D Database.
  /// </summary>
  public class VASFilter20 : Filter20
  {

    #region fields

    /// <summary>
    /// Subdirectories that should be interepreted as a session.
    /// </summary>
    string[] m_subDirectoryTags = { "vas" };

    #endregion

    #region methods

    /// <summary>
    /// Indicates if this filter can process the directory in question.
    /// </summary>
    /// <param name="ioArgs">Arguments to take into account.</param>
    /// <returns>True if the filter can process it, false otherwise.</returns>
    public override bool CanParse(ImporterArgs ioArgs)
    {
      List<string> directoriesToCheck = new List<string>();

      directoriesToCheck.AddRange(Directory.GetDirectories(ioArgs.Source));

      if(Utils.ContainsDicomInfo(ioArgs.Source))
      {
        // Check to make sure there's a vas* directory in here.
        foreach(string directory in directoriesToCheck)
        {
          string[] directoryComponents = directory.Split('\\');
          string directoryName = directoryComponents[directoryComponents.Length-1];

          if( directoryName.ToLower().StartsWith("vas") )
          {
            return true;
          } // if(directory.ToLower().StartsWith("vas"))
        } // foreach(string directory in directoriesToCheck)
      } // if(Utils.ContainsDicomInfo(ioArgs.Source))

      return false;
    } // CanParse(IOArgs ioArgs)


    /// <summary>
    /// Attempts to filter the directory and returns an Importer that carries the results of the conversions.
    /// </summary>
    /// <param name="args">Arguments to take into account.</param>
    /// <returns>An importer that has the conversions, or null if a problem occurred.</returns>
    public override IImporter Parse(ImporterArgs args)
    {
      ImportManager20 manager = new ImportManager20();

      try
      {
        Global.CheckForCancel();
        Global.NotifyProgress("Found that study is a Vascular 2.0", 1);

        manager.Initialize(args);

        // The list of dicom series that have already been parsed as they have folders containing them.
        // Ignore .dcm files having the same data set.
        List<string> parsedDicomDataSets = ParseDicomFolders(args.Source, "VAS", manager);

        foreach (string fileName in Directory.GetFiles(args.Source))
        {
          Global.CheckForCancel();

          string fileNameLC = fileName.ToLower();

          string destination = "{0}";
          SeriesTypes seriesType = SeriesTypes.None;
          string macroReplace = string.Empty;
          string dataSet = Utils.GetDataSet(fileNameLC);

          if (IsDicom(fileNameLC))
          {
            // If the data set already has a dicom series associated with it, just continue.
            if (parsedDicomDataSets.Contains(dataSet))
            {
              continue;
            } // if(parsedDicomFolders.Contains(dataSet))

            // Mark it as a Dicom file.
            seriesType = SeriesTypes.Dicom;
            destination = "dicom\\{0}";
          } // if(IsDicom(file))
          else if (IsVolume(fileNameLC))
          {
            // Mark it as a Volume file.
            seriesType = SeriesTypes.Dicom;
            destination = "volumes\\{0}";
          } // else if(IsVolume(fileNameLC))
          else if (IsCad(fileNameLC))
          {
            // Mark it as a Cad file.
            seriesType = SeriesTypes.Cad;
          } // else if(IsCad(fileNameLC))
          else
          {
            // Everything else goes into the Volume series.
            seriesType = SeriesTypes.Dicom;
            destination = "volumes\\{0}";
          }

          Global.CheckForCancel();

          // Handle inserting bad datasets.
          try
          {
            manager.AddFile(dataSet, seriesType, "VAS", fileName, destination, macroReplace);
          } // try
          catch (ArgumentException)
          {
            Log.Warning("File: " + fileName + " will be ignored because it is " +
              "not associated with a proper data set.", "VASFilter20", "Parse");
          } // catch( ArgumentException e )
        } // foreach(string file in Directory.GetFiles(ioArgs.Source))

        Global.CheckForCancel();

        bool subDirectoryParsed = false;

        // Parse sub directories that have a tag.
        foreach (string subDirectory in Directory.GetDirectories(args.Source))
        {
          Global.CheckForCancel();

          subDirectoryParsed = false;
          foreach (string tag in m_subDirectoryTags)
          {
            if (subDirectory.ToLower().IndexOf(tag) > -1)
            {
              ParseSessionPathRecursive(subDirectory, manager, args, subDirectory);
              subDirectoryParsed = true;
              break;
            } // if(subDirectory.IndexOf(tag) > -1)
          } // foreach(string tag in m_subDirectoryTags)

          if (!subDirectoryParsed)
          {
            Log.Warning("Subdirectory: " + subDirectory +
              " did not qualify as a session directory and will be ignored.", "VASFilter20", "Parse");
          } // if(!subDirectoryParsed)
        } // foreach(string subDirectory in subDirectories)

        Global.CheckForCancel();
        
        return manager;
      }
      catch (Exception)
      {
        Log.Error(string.Format("Failed to parse directory contents.  [DIRECTORY={0}]", args.Source), "VASFilter20", "Parse");
      }

      return null;
    } // Parse( args )


    /// <summary>
    /// Determines if the provided file is a session.
    /// </summary>
    /// <param name="path">The file path to examine.</param>
    /// <returns>False</returns>
    public override bool IsSession(string path)
    {
      return false;
    } // IsSession(string path)


    /// <summary>
    /// Travels along a directory hitting all subdirectories and subdirectories of those in turn.
    /// </summary>
    /// <param name="path">The path that should be parsed.</param>
    /// <param name="manager">The ImportManager that we will be adding files to as we find them.</param>
    /// <param name="directoryToStripOff">The directory to strip off when constructing the destination.</param>
    private void ParseSessionPathRecursive(string path, ImportManager20 manager, ImporterArgs ioArgs, string directoryToStripOff)
    {
      Global.CheckForCancel();

      throw new ApplicationException("Need to address after file system done.");

      if( path != null && Directory.Exists(path) )
      {
        // Used for obtaining the dataset to generate annotation files.
        string volumeSeriesUID = null;

        Dictionary<string, string> specialCaseMap = new Dictionary<string, string>();

        specialCaseMap.Add("snapshots.xml", "snapshots\\snapshots.xml");
        specialCaseMap.Add("report.xml", "report\\report.xml");
        specialCaseMap.Add("movies.xml", "movies\\movies.xml");
        specialCaseMap.Add("session.xml", "Do Not Copy");
        specialCaseMap.Add("-label.vol", "{0}\\label.vol");
        specialCaseMap.Add(".lb.vol", "{0}\\isotropicLabel.vol");

        foreach(string filePath in Directory.GetFiles(path))
        {
          bool shouldCopy = true;
          string fileNameLC = Path.GetFileName(filePath).ToLower();

          string dataSet = Utils.GetDataSet(fileNameLC);

          string macroReplace = string.Empty;
          string destination = filePath.Remove(filePath.IndexOf(directoryToStripOff), directoryToStripOff.Length);
          SeriesTypes seriesType = SeriesTypes.Session;

          if(IsVolume(fileNameLC))
          {
            destination = "{0}";
            seriesType = SeriesTypes.Preprocessed;

            if(volumeSeriesUID == null)
            {
              volumeSeriesUID = manager.GetSeriesUID(Utils.GetDataSet(fileNameLC), SeriesTypes.Dicom, ".");
            } // if(volumeSeriesUID == null)

            foreach(KeyValuePair<string, string> keypair in specialCaseMap)
            {
              if(fileNameLC.EndsWith(keypair.Key) && keypair.Value != "Do Not Copy")
              {
                seriesType = SeriesTypes.Session;
                destination = string.Format( keypair.Value, volumeSeriesUID );
              } // if(fileNameLC.EndsWith(keypair.Key) && keypair.Value != "Do Not Copy")
              else if(fileNameLC.EndsWith(keypair.Key) && keypair.Value == "Do Not Copy")
              {
                // Don't copy the file.
                shouldCopy = false;
              } // else if(fileNameLC.EndsWith(keypair.Key) && keypair.Value == "Do Not Copy")
            } // foreach(KeyValuePair<string, string> keypair in sessionTags)
          } // if(IsVolume(fileNameLC))
          else if(fileNameLC.EndsWith(".xml"))
          {
            foreach( KeyValuePair<string, string> keypair in specialCaseMap )
            {
              if( fileNameLC.EndsWith(keypair.Key) && keypair.Value != "Do Not Copy" )
              {
                destination = keypair.Value;
              } // if(fileNameLC.EndsWith(keypair.Key) && keypair.Value != "Do Not Copy")
              else if( fileNameLC.EndsWith(keypair.Key) && keypair.Value == "Do Not Copy" )
              {
                // Don't copy the file.
                shouldCopy = false;
              } // else if(fileNameLC.EndsWith(keypair.Key) && keypair.Value == "Do Not Copy")
            } // foreach(KeyValuePair<string, string> keypair in sessionTags)
          } // else if(fileNameLC.EndsWith(".xml"))

          // Should the file be copied.
          if(shouldCopy)
          {
            // Handle inserting bad datasets.
            try
            {
              string[] sessionXmlFiles = Directory.GetFiles(path, "*session.xml");

              // Only session.xml can create the session series.
              if(seriesType != SeriesTypes.Session || manager.GetParsedSeries(string.Empty, SeriesTypes.Session) != null || sessionXmlFiles.Length > 0 )
              {
                manager.AddFile(dataSet, seriesType, "VAS", filePath, destination, macroReplace);
              } // if(seriesType != "Session" || manager.GetParsedSeries(string.Empty, "Session") != null)
            } // try
            catch( ArgumentException )
            {
              Log.Warning("File: " + filePath + " will be ignored because it is " +
                "not associated with a proper data set.", "VASFilter20", "ParseSessionPathRecursive");
            } // catch( ArgumentException e )
          } // if(shouldCopy)
        } // foreach(string filePath in Directory.GetFiles(path))

        if(volumeSeriesUID != null)
        {
          string[] sessionXmlFile = Directory.GetFiles(path, "*session.xml");
          if(sessionXmlFile.Length > 0)
          {
            manager.CreateReferenceLinks(ioArgs);
            GenerateAnnotationFiles(sessionXmlFile[0], string.Empty, volumeSeriesUID, 
              manager.GetParsedSeries(string.Empty, SeriesTypes.Session));
          }
        } // if(volumeSeriesUID != null)

        foreach( string dir in Directory.GetDirectories(path) )
        {
          ParseSessionPathRecursive(dir, manager, ioArgs, directoryToStripOff);
        } // foreach( string dir in Directory.GetDirectories(path) )
      } // if( path != null && path != "" && Directory.Exists(path) )
      else
      {
        // Log an error that a bad directory was passed into the path parse.
        Log.Warning("Directory: <" + path + "> doesn't exist and will not be examined as a session", "VASFilter20", "ParseSessionPathRecursive");

        throw new ArgumentException("Directory: <" + path + "> doesn't exist and will not be examined as a session");
      } // else
    } // ParseSessionPathRecursive(string path, ImportManager manager, string directoryToStripOff)

    #endregion

  } // VASFilter20
} // namespace Viatronix.Utilities.Importer.Filters

#region revision history

// $Log: VASFilter20.cs,v $
// Revision 1.1.2.2  2009/06/04 18:47:57  kchalupa
// Coding standards.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities.Importer/Filters/VASFilter20.cs,v 1.1.2.2 2009/06/04 18:47:57 kchalupa Exp $
// $Id: VASFilter20.cs,v 1.1.2.2 2009/06/04 18:47:57 kchalupa Exp $

#endregion
