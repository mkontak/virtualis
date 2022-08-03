// $Id: XPFilter20.cs,v 1.1.2.2 2009/06/04 18:49:42 kchalupa Exp $
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
using System.Xml;
using System.Collections.Generic;

using Viatronix.Logging;
using Viatronix.Enterprise.Entities;
using Viatronix.Utilities.Importer.Data;

namespace Viatronix.Utilities.Importer.Filters
{
  /// <summary>
  /// Parses XP 20 directories.
  /// </summary>
  public class XPFilter20 : Filter20
  {

    #region fields

    /// <summary>
    /// Subdirectories that should be interepreted as a session.
    /// </summary>
    string[] m_subDirectoryTags = { "xp", "angio", "ang", "cs" };

    #endregion

    #region methods

    /// <summary>
    /// Determines whether or not the filter can process the given directory.
    /// 
    /// Looking for one .ins file and either a .vol or .ecv
    /// </summary>
    /// <param name="args">Describes the ioArgs that should be investigated.</param>
    /// <returns>Whether or not the filter can process the directory.</returns>
    public override bool CanParse(ImporterArgs ioArgs)
    {
      if(!Utils.ContainsDicomInfo(ioArgs.Source))
      {
        return false;
      } // if(!Utils.ContainsDicomInfo(ioArgs.Source))

      string[] subDirectories = Directory.GetDirectories(ioArgs.Source);

      // Check to make sure one of the subDirectories has a tag in it's name.
      if(subDirectories.Length != 0)
      {
        foreach(string subDirectory in subDirectories)
        {
          string subDirectoryLC = subDirectory.ToLower();

          foreach(string tag in m_subDirectoryTags)
          {
             if(subDirectoryLC.IndexOf(tag) > -1)
             {
               return true;
             } // if(subDirectoryLC.IndexOf(tag) > -1)
          } // foreach(string necessaryComponent in m_subDirectoriesMustHave)
        } // foreach(string subDirectory in subDirectories)
      } // if(!result)
      else
      {
        // If no sub directories were found the base directory is okay to be parsed.
        return true;
      } // else

      return false;
    } // CanParse(IOArgs ioArgs)


    /// <summary>
    /// Parses the directory provided and tries to perform the conversions.
    /// </summary>
    /// <param name="args">Arguments to take into account.</param>
    /// <returns>An importer if successful, false otherwise.</returns>
    public override IImporter Parse(ImporterArgs args)
    {
      ImportManager20 manager = new ImportManager20();
      string[] subDirectories = Directory.GetDirectories(args.Source);

      try
      {
        Global.CheckForCancel();
        Global.NotifyProgress("Found that study is a Explorer 2.0", 1);

        manager.Initialize(args);

        // The list of dicom series that have already been parsed as they have folders containing them.
        // Ignore .dcm files having the same data set.
        List<string> parsedDicomDataSets = ParseDicomFolders(args.Source, "XP", manager);

        foreach (string fileName in Directory.GetFiles(args.Source))
        {
          Global.CheckForCancel();

          string fileNameLC = fileName.ToLower();

          // Parameters to the ImportManager.Add()
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

            // Otherwise, add as usual.
            seriesType = SeriesTypes.Dicom;
            destination = "dicom\\{0}";
          } // if(fileNameLC.EndsWith(".dcm"))
          else if (IsVolume(fileNameLC))
          {
            seriesType = SeriesTypes.Dicom;
            destination = "volumes\\{0}";
          } // else if(IsVolume(fileNameLC))
          else if (IsCad(fileNameLC))
          {
            seriesType = SeriesTypes.Cad;
          } // else if(IsCad(fileNameLC))
          else
          {
            // All other files are volume type.
            seriesType = SeriesTypes.Dicom;
          } // else

          Global.CheckForCancel();

          // Handle inserting bad datasets.
          try
          {
            manager.AddFile(dataSet, seriesType, "XP", fileName, destination, macroReplace);
          } // try
          catch (ArgumentException)
          {
            Log.Warning("File: " + fileName + " will be ignored because it is " +
              "not associated with a proper data set.", "XPFilter20", "Parse");
          }
        } // foreach(string fileName in ioArgs.Source)

        Global.CheckForCancel();

        bool subDirectoryParsed = false;

        // Parse sub directories that have a tag.
        foreach (string subDirectory in subDirectories)
        {
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
              " did not qualify as a session directory and will be ignored.", "XPFilter20", "Parse");
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
    } // Parse(ImporterArgs ioArgs)


    /// <summary>
    /// Checks to see if the file in question is a session file.
    /// 
    /// Session files are contained in a subDirectory that only contains session files.  So this function
    /// is not needed.
    /// </summary>
    /// <remarks>
    /// Performs a lower case sensative check.  Caller is responsible for
    /// forcing consistency by using string.ToLower().
    /// </remarks>
    /// <param name="path">The path to the file.</param>
    /// <returns>Whether or not the file is a session file.</returns>
    public override bool IsSession(string path)
    {
      return false;
    } // IsSession(string path)


    /// <summary>
    /// Travels along a directory hitting all subdirectories and subdirectories of those in turn.
    /// </summary>
    /// <param name="path">The path that should be parsed.</param>
    /// <param name="manager">The ImportManager that we will be adding files to as we find them.</param>
    /// <param name="ioArgs">Determines whether or not the session should be included in the reference links
    /// generation.</param>
    /// <param name="directoryToStripOff">The directory to strip off when constructing the destination.</param>
    private void ParseSessionPathRecursive(string path, ImportManager20 manager, ImporterArgs ioArgs, string directoryToStripOff)
    {
      Global.CheckForCancel();

      if (path != null && Directory.Exists(path))
      {
        // Used for obtaining the dataset to generate annotation files.
        string volumeSeriesUID = null;

        Dictionary<string, string> specialCaseMap = new Dictionary<string, string>();

        specialCaseMap.Add("snapshots.xml", "snapshots\\snapshots.xml");
        specialCaseMap.Add("report.xml", "report\\report.xml");
        specialCaseMap.Add("movies.xml", "movies\\movies.xml");
        specialCaseMap.Add("session.xml", "Do Not Copy");
        specialCaseMap.Add("-label.vol", "{0}\\label.vol");

        foreach (string filePath in Directory.GetFiles(path))
        {
          bool shouldCopy = true;
          string fileNameLC = Path.GetFileName(filePath).ToLower();

          string dataSet = Utils.GetDataSet(fileNameLC);
          string macroReplace = string.Empty;
          string destination = filePath.Remove(filePath.IndexOf(directoryToStripOff), directoryToStripOff.Length);
          SeriesTypes seriesType = SeriesTypes.Session;

          if (IsVolume(fileNameLC) && volumeSeriesUID == null)
          {
            volumeSeriesUID = manager.GetSeriesUID(Utils.GetDataSet(fileNameLC), SeriesTypes.Dicom, ".");
          } // if(IsVolume(fileNameLC) && volumeSeriesUID == null)

          foreach (KeyValuePair<string, string> keypair in specialCaseMap)
          {
            if (fileNameLC.EndsWith(keypair.Key) && keypair.Value != "Do Not Copy")
            {
              destination = string.Format(keypair.Value, volumeSeriesUID);
            } // if(fileNameLC.EndsWith(keypair.Key) && keypair.Value != "Do Not Copy")
            else if (fileNameLC.EndsWith(keypair.Key) && keypair.Value == "Do Not Copy")
            {
              // Don't copy the file.
              shouldCopy = false;
            } // else if(fileNameLC.EndsWith(keypair.Key) && keypair.Value == "Do Not Copy")
          } // foreach(KeyValuePair<string, string> keypair in sessionTags)

          // Should the file be copied.
          if (shouldCopy)
          {
            // Handle inserting bad datasets.
            try
            {
              string[] sessionXmlFiles = Directory.GetFiles(path, "*session.xml");

              // Only session.xml can create the session series.
              if (seriesType != SeriesTypes.Session || manager.GetParsedSeries(string.Empty, SeriesTypes.Session) != null
                || sessionXmlFiles.Length > 0)
              {
                manager.AddFile(dataSet, seriesType, "XP", filePath, destination, macroReplace);
              } // if(seriesType != "Session" || manager.GetParsedSeries(string.Empty, "Session") != null)
            } // try
            catch (ArgumentException)
            {
              Log.Warning("File: " + filePath + " will be ignored because it is not associated" +
                " with a proper data set.", "XPFilter20", "ParseSessionPathRecursive");
            } // catch( ArgumentException e )
          } // if(shouldCopy)
        } // foreach(string filePath in Directory.GetFiles(path))

        if (volumeSeriesUID != null)
        {
          string[] sessionXmlFile = Directory.GetFiles(path, "*session.xml");
          if (sessionXmlFile.Length > 0)
          {
            manager.CreateReferenceLinks(ioArgs);
            GenerateAnnotationFiles(sessionXmlFile[0], string.Empty, volumeSeriesUID,
              manager.GetParsedSeries(string.Empty, SeriesTypes.Session));
          }
        } // if(volumeSeriesUID != null)

        foreach (string dir in Directory.GetDirectories(path))
        {
          ParseSessionPathRecursive(dir, manager, ioArgs, directoryToStripOff);
        } // foreach( string dir in Directory.GetDirectories(path) )
      } // if( path != null && path != "" && Directory.Exists(path) )
      else
      {
        // Log an error that a bad directory was passed into the path parse.
        Log.Warning("Directory: <" + path + "> doesn't exist and will not be examined as a session", "XPFilter20", "ParseSessionPathRecursive");

        throw new ArgumentException("Directory: <" + path + "> doesn't exist and will not be examined as " +
          "a session");
      } // else
    } // ParseSessionPathRecursive(string path, ImportManager manager, string directoryToStripOff)

    #endregion

  } // XPFilter20
} // namespace Viatronix.Utilities.Importer.Filters

#region revision history

// $Log: XPFilter20.cs,v $
// Revision 1.1.2.2  2009/06/04 18:49:42  kchalupa
// Coding standards.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities.Importer/Filters/XPFilter20.cs,v 1.1.2.2 2009/06/04 18:49:42 kchalupa Exp $
// $Id: XPFilter20.cs,v 1.1.2.2 2009/06/04 18:49:42 kchalupa Exp $

#endregion
