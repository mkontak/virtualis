// $Id: Filter30.cs,v 1.1.2.2 2009/06/04 18:51:54 kchalupa Exp $
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
using System.Text;

//using Viatronix.v3D.Core;
using Viatronix.Dicom;
using Viatronix.Logging;
using Viatronix.Utilities.Importer.Data;

namespace Viatronix.Utilities.Importer.Filters
{
  /// <summary>
  /// Filters directories containing 3.0 data.
  /// </summary>
  public class Filter30 : IFilter
  {

    #region methods

    /// <summary>
    /// Determines whether or not the folder can be processed.
    /// </summary>
    /// <param name="args">Defines where the ioArgs is located.</param>
    /// <returns>Whether or not the particular filter can process the folder's contents.</returns>
    public bool CanParse(ImporterArgs ioArgs)
    {
      bool patientFound = false;

      if(Directory.Exists(ioArgs.Source))
      {
        string[] xmlPatientCandidateFiles = Directory.GetFiles(ioArgs.Source, "patient.xml");

        // If a file exists with the desired name, mark that we can proceed.
        if(xmlPatientCandidateFiles.Length > 0)
        {
          patientFound = true;
        } // if(xmlPatientCandidateFiles.Length > 0)
      } // if(Directory.Exists(ioArgs.Source))

      // If a patient was not found, return.
      if(!patientFound)
      {
        return false;
      } // if(!patientFound)

      bool studyFound = false;
      string studyDirectory = null;
      string[] studyDirectories = Directory.GetDirectories(ioArgs.Source);

      foreach(string directory in studyDirectories)
      {
        string[] xmlStudyCandidateFiles = Directory.GetFiles(directory, "study.xml");

        if(xmlStudyCandidateFiles.Length > 0)
        {
          studyFound = true;
          studyDirectory = directory;
          break;
        } // if(xmlStudyCandidateFiles.Length > 0)
      } // foreach(string directory in studyDirectories)

      // If a study was not found, return.
      if( !studyFound )
      {
        return false;
      } // if(!patientFound)

      bool foundOneSeriesXml = false;
      string[] seriesDirectories = Directory.GetDirectories(studyDirectory);

      // Look in each series directory.
      foreach(string directory in seriesDirectories)
      {
        string[] xmlSeriesCandidateFiles = Directory.GetFiles(directory, "series.xml");

        if(xmlSeriesCandidateFiles.Length > 0)
        {
          foundOneSeriesXml = true;
        } // if(xmlSeriesCandidateFiles.Length > 0)
      } // foreach(string directory in seriesDirectories)

      return foundOneSeriesXml;
    } // CanParse()


    /// <summary>
    /// Processes the folder to determine files fit into what series.
    /// </summary>
    /// <param name="args">Defines where the ioArgs is located.</param>
    /// <param name="importer">The object to hold what files fit into what series.</param>
    /// <returns>A Paitient object that was defined by the set of files found.</returns>
    public IImporter Parse(ImporterArgs args)
    {
      Global.NotifyProgress("Found that study is a Viatronix 3.0.", 1);

      // Currently in patient directory.
      string[] patientDirectoryFiles = Directory.GetFiles(args.Source, "patient.xml");

      ImportManager30 importer = null;
      Patient patient = null;

      if (patientDirectoryFiles.Length > 0)
      {
        XmlDocument document = new XmlDocument();
        document.Load(patientDirectoryFiles[0]);

        patient = Patient.Create(document.DocumentElement);
        patient.Directory = Path.GetFileName(args.Source);

        importer = new ImportManager30(patient, args.Source);
      }

      if( patient == null )
      {
        // Log an error and return null.
        Utils.LogMessage(LogTypes.Error, "Import Failed; Directory <" + args.Source +
          "> contains no valid patient xml file");

        throw new ApplicationException("Importing of directory; <" + args.Source + ">; Failed; Due to lack of valid patient.xml file");
      } // if(parsedSeries == null)

      // Descending into the study directories.
      foreach(string studyDirectory in Directory.GetDirectories(args.Source))
      {
        string[] studyDirectoryFiles = Directory.GetFiles(studyDirectory, "study.xml");
        
        ParsedStudy parsedStudy = null;

        if(studyDirectoryFiles.Length > 0)
        {
          parsedStudy = importer.AddStudy(studyDirectoryFiles[0]);
          parsedStudy.Study.Directory = Path.Combine(Path.GetFileName(patient.Directory), Path.GetFileName(studyDirectory));
        } // if(studyFile == "study.xml")

        if( parsedStudy == null )
        {
          // Log an error and continue parsing series directories.
          Utils.LogMessage(LogTypes.Error, "Import of Study Failed; Directory <" + args.Source +
            "> contains no valid study xml file");

          continue;
        } // if(parsedSeries == null)

        // Descending into the series directories.
        foreach(string seriesDirectory in Directory.GetDirectories(studyDirectory))
        {
          ParsedSeries parsedSeries = null;
          List<string> seriesFiles = new List<string>();
          string[] seriesDirectoryFiles = Directory.GetFiles(seriesDirectory, "series.xml");

          if(seriesDirectoryFiles.Length > 0)
          {
            parsedSeries = parsedStudy.AddSeries(seriesDirectoryFiles[0]);
            parsedSeries.Series.Directory = Path.GetFileName(seriesDirectory);
          } // if(studyFile == "study.xml")

          if(parsedSeries == null)
          {
            // Log an error and continue parsing series directories.
            Utils.LogMessage(LogTypes.Error, "Import of Series Failed; Directory <" + args.Source +
              "> contains no valid series xml file");

            continue;
          } // if(parsedSeries == null)

          string[] seriesSubDirectories = Directory.GetDirectories(seriesDirectory);

          foreach(string seriesSubDirectory in seriesSubDirectories)
          {
            // Parse the subdirectories recursively and add all the files therein into the series.
            ParsePathRecursive(seriesSubDirectory, parsedSeries, seriesDirectory + "\\");
          } // foreach(string seriesSubDirectory in seriesSubDirectories)

          foreach(string file in seriesFiles)
          {
            parsedSeries.AddFile(file, Path.GetFileName(file), string.Empty);
          } // foreach(string file in seriesFiles)
        } // foreach(string seriesDirectory in Directory.GetDirectories(studyDirectory))
      } // foreach(string studyDirectory in Directory.GetDirectories(ioArgs.Source))

      return importer;
    } // Parse()


    /// <summary>
    /// Travels along a directory hitting all subdirectories and subdirectories of those in turn.
    /// </summary>
    /// <param name="path">The path that should be parsed.</param>
    /// <param name="parsedSeries">The parsedSeries that we will be adding files to as we find them.</param>
    /// <param name="directoryToStripOff">The directory to strip off when constructing the destination.</param>
    private void ParsePathRecursive(string path, ParsedSeries parsedSeries, string directoryToStripOff)
    {
      if( path != null && Directory.Exists(path) )
      {
        string[] files = Directory.GetFiles(path);

        foreach(string seriesFile in files)
        {
          string destination = seriesFile.Remove(seriesFile.IndexOf(directoryToStripOff), directoryToStripOff.Length);
          parsedSeries.AddFile(seriesFile, destination, string.Empty);
        } // foreach(string seriesFile in files)

        foreach( string dir in Directory.GetDirectories(path) )
        {
          ParsePathRecursive(dir, parsedSeries, directoryToStripOff);
        } // foreach( string dir in Directory.GetDirectories(path) )
      } // if( path != null && path != "" && Directory.Exists(path) )
      else
      {
        // Log an error that a bad directory was passed into the path parse.
        Log.Warning("Directory: <" + path + "> doesn't exist and will not be examined as a session", "Filter30", "ParsePathRecursive");

        throw new ArgumentException("Directory: <" + path + "> doesn't exist and will not be examined as a session");
      } // else
    } // ParsePathRecursive( string path )

    #endregion

  } // Filter30
} // namespace Viatronix.Utilities.Importer.Filters

#region revision history

// $Log: Filter30.cs,v $
// Revision 1.1.2.2  2009/06/04 18:51:54  kchalupa
// Coding standards.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities.Importer/Filters/Filter30.cs,v 1.1.2.2 2009/06/04 18:51:54 kchalupa Exp $
// $Id: Filter30.cs,v 1.1.2.2 2009/06/04 18:51:54 kchalupa Exp $

#endregion
