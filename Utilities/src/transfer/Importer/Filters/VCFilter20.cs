// $Id: VCFilter20.cs,v 1.1.2.5 2009/06/04 19:15:34 kchalupa Exp $
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
using System.Windows.Forms;

using Viatronix.Dicom;
using Viatronix.Logging;
using Viatronix.Enterprise.Entities;
using Viatronix.Utilities.Importer.Data;

namespace Viatronix.Utilities.Importer.Filters
{
  /// <summary>
  /// Reads VC studies and filters the files into groups.
  /// </summary>
  public class VCFilter20 : Filter20
  {

    #region fields

    /// <summary>
    /// The datsets, for which dicom were already read in.
    /// </summary>
    private List<string> m_parsedDicomDatasets = new List<string>();

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
      // Check for an INS file.
      if (Directory.GetFiles(ioArgs.Source, "*?.ins").Length > 0)
      {
        // Check for .vol file, if it exists return true.
        if (Directory.GetFiles(ioArgs.Source, "*?.vol").Length > 0)
        {
          return true;
        } // if (Directory.GetFiles(ioArgs.Source, "*?.vol").Length > 0)
        // Check for .ecv file, if it exists return true.
        else if (Directory.GetFiles(ioArgs.Source, "*?.ecv").Length > 0)
        {
          return true;
        } // else if (Directory.GetFiles(ioArgs.Source, "*?.ecv").Length > 0)
      } // if (Directory.GetFiles(ioArgs.Source, "*?.ins").Length > 0)

      return false;
    } // CanProcess(IOArgs args)


    /// <summary>
    /// Parses the given directory to create updated VC file and directory structures.
    /// </summary>
    /// <param name="args">The file system ioArgs.</param>
    /// <returns>A useful importmanager object if the path had dicom information, otherwise null.</returns>
    public override IImporter Parse(ImporterArgs args)
    {
      Global.NotifyProgress("Found that study is a Virtual Colonosocopy 2.0", 1);

      // Create and initialize the manager.
      ImportManager20 manager = new ImportManager20();

      // Verify that the manager has useful information.
      try
      {
        manager.Initialize(args);

        Global.CheckForCancel();

        //Get the list of files and loop through them.
        string[] files = Directory.GetFiles(args.Source, "*?.*");

        // The list of dicom series that have already been parsed as they have folders containing them.
        // Ignore .dcm files having the same data set.
        List<string> parsedDicomDataSets = ParseDicomFolders(args.Source, "VC", manager);

        // Loop through each one.
        foreach (string file in files)
        {
          Global.CheckForCancel();

          FileParseInfo info = HandleFile(manager, file);

          if (info != null)
          {
            // Handle inserting bad datasets.
            try
            {
              manager.AddFile(info, "VC");
            } // try
            catch (ArgumentException)
            {
              Log.Warning("File: " + file + " will be ignored because it is " +
                "not associated with a proper data set.", "VCFilter20", "Parse");
            }
          }
        } // foreach( string file in files )

        Global.CheckForCancel();

        List<string> sessionFiles = new List<string>();
        sessionFiles.AddRange(Directory.GetFiles(args.Source, "*?.vcsx"));
        sessionFiles.AddRange(Directory.GetFiles(args.Source, "*?.mkvl"));

        foreach(string file in sessionFiles)
        {
          FileParseInfo info = new FileParseInfo(file);
          info.SeriesType = SeriesTypes.Session;
          info.Destination = "{0}";

          if(info.Dataset != "Unknown")
          {
            info.MacroReplace = manager.GetSeriesUID(info.Dataset, SeriesTypes.Dicom, "_");
          }

          manager.AddFile(info, "VC");
        }

        string macroReplace2 = string.Empty;
        string[] vrxFiles = Directory.GetFiles(args.Source, "*?.vrx");

        if (vrxFiles.Length > 0)
        {
          /////////////////////////////////////////////////////////////////////////////////////
          // Build up a listing of datasets that should be imported.  This is necessary to ensure the
          // integrity of the Session's VRX file.  MKVL files with datasets will be imported.  All other
          // datasets will not be copied over.
          /////////////////////////////////////////////////////////////////////////////////////

          Global.CheckForCancel();

          List<string> acceptableDataSets = new List<string>();

          foreach (string filePath in Directory.GetFiles(args.Source, "*?.mkvl"))
          {
            string filePathLC = filePath.ToLower();
            string fileDataSet = Utils.GetDataSet(filePathLC);
            if (filePathLC.EndsWith(".mkvl") && fileDataSet != "Unknown")
            {
              acceptableDataSets.Add(fileDataSet);
            } // if(filePathLC.EndsWith(".mkvl") && fileDataSet != "Unknown")
          } // foreach(string filePath in Directory.GetFiles(ioArgs.Source, "*?.mkvl"))

          Global.CheckForCancel();

          if (acceptableDataSets.Count > 0)
          {
            List<string> removingDataSets = new List<string>();

            foreach (KeyValuePair<string, SortedDictionary<SeriesTypes, ParsedSeries>> dataSetToSeriesList in
              manager.DataSets)
            {
              bool savingGrace = false;
              foreach (string acceptableDataSet in acceptableDataSets)
              {
                if (dataSetToSeriesList.Key.ToLower() == acceptableDataSet.ToLower())
                {
                  savingGrace = true;
                  break;
                } // if(dataSet.ToLower() == acceptableDataSet.ToLower())
              } // foreach(string acceptableDataSet in acceptableDataSets)

              if (!savingGrace)
              {
                foreach (KeyValuePair<SeriesTypes, ParsedSeries> keyPair in dataSetToSeriesList.Value)
                {
                  Log.Warning(keyPair.Value.Series.Uid.Replace(".", "_") + "; Series will not be imported in the interest of maintaining VRX integrity.", "VCFilter20", "Parse");

                  manager.StudyToImport.RemoveSeries(keyPair.Value);
                } // foreach(KeyValuePair<string, ParsedSeries> parsedSeries in dataSetToSeriesList.Value)

                removingDataSets.Add(dataSetToSeriesList.Key);
              } // if(!savingGrace)
            } // foreach(string dataSet in manager.DataSets.Keys)

            foreach (string removeDataSet in removingDataSets)
            {
              manager.DataSets.Remove(removeDataSet);
            } // foreach(string removeDataSet in removingDataSets)

            /////////////////////////////////////////////////////////////////////////////////////
            // Look for the supine series within the acceptable list and then grab the UID from
            // that set's Volume series.
            /////////////////////////////////////////////////////////////////////////////////////

            string primaryDataset = string.Empty;
            foreach (string acceptedDataSet in acceptableDataSets)
            {
              string acceptedDataSetLC = acceptedDataSet.ToLower();
              if (acceptedDataSetLC.EndsWith("s"))
              {
                primaryDataset = acceptedDataSetLC;
              } // if(acceptedDataSetLC.EndsWith("s"))
            } // foreach(string acceptedDataSet in acceptableDataSets)

            if (primaryDataset == string.Empty)
            {
              primaryDataset = acceptableDataSets[0].ToLower();
            } // if(primaryDataset == string.Empty)

            // Name the VRX after the supine dataset's volume series.
            if (primaryDataset != string.Empty)
            {
              macroReplace2 = manager.GetSeriesUID(primaryDataset, SeriesTypes.Dicom, "_");
              macroReplace2 = macroReplace2.Substring(0, macroReplace2.Length - 2);
            } // if(primaryVolumeSeries != null)
            else
            {
              Log.Error("Unable to find Primary Series VRX must be with the Session UID" +
                " Please rename by hand after import.", "VCFilter20", "Parse");
            } // else

            try
            {
              manager.AddFile(string.Empty, SeriesTypes.Session, "VC", vrxFiles[0], "{0}", macroReplace2);
            } // try
            catch (ArgumentException)
            {
              Log.Warning("File: " + vrxFiles[0] + " will be ignored because it is " +
                "not associated with a proper data set.", "VCFilter20", "Parse");
            } // catch( ArgumentException )
          } // if(acceptableDataSets.Count > 0)
          else
          {
            ParsedSeries primaryVolumeSeries = manager.GetPrimaryVolumeSeries();
            if (primaryVolumeSeries != null)
            {
              macroReplace2 = primaryVolumeSeries.Series.Uid.Replace(".", "_");
              macroReplace2 = macroReplace2.Substring(0, macroReplace2.Length - 2);
            } // if(primaryVolumeSeries != null)
            else
            {
              Log.Error("Unable to find Primary Series VRX must be with the Session UID" +
                " Please rename by hand after import.", "VCFilter20", "Parse");
            } // else

            try
            {
              manager.AddFile(string.Empty, SeriesTypes.Session, "VC", vrxFiles[0], "{0}", macroReplace2);
            } // try
            catch (ArgumentException)
            {
              Log.Warning("File: " + vrxFiles[0] + " will be ignored because it is " +
                "not associated with a proper data set.", "VCFilter20", "Parse");
            } // catch( ArgumentException )
          } // else
        } // if(vrxFiles.Length > 0)

        Global.CheckForCancel();

        return manager;
      }
      catch (Exception e)
      {
        Log.Error(string.Format("Failed to parse directory contents.  [DIRECTORY={0}]", args.Source), "VASFilter20", "Parse");
      }
      
      return null;
    } // Parse()


    /// <summary>
    /// Verifies whether or not the file being examined is to be added to the session series.
    /// </summary>
    /// <param name="filePath">The file that will be examined</param>
    /// <returns>True if the file is a session file, otherwise false.</returns>
    public override bool IsSession(string filePath)
    {
      return filePath.EndsWith(".amk") || filePath.EndsWith(".mrk") || filePath.EndsWith(".mkvl") || 
        filePath.EndsWith(".vcsx") || filePath.EndsWith(".vcsn") || filePath.EndsWith(".vrx");
    } // IsSession(string path)


    /// <summary>
    /// Determines the file's series type.
    /// </summary>
    /// <param name="file">The file to examine.</param>
    /// <returns>The type of series the file belongs to.</returns>
    private SeriesTypes DetermineSeriesType(string file)
    {
      string fileNameLC = file.ToLower();

      if (IsMovie(fileNameLC) || IsSnapshot(fileNameLC) || IsSession(fileNameLC))
      {
        return SeriesTypes.Session;
      } // if( IsMovie(fileName) || IsSnapshot(fileName) || IsSession(fileName))
      else if (IsDicom(fileNameLC))
      {
        return SeriesTypes.Dicom;
      } // else if( IsDicom( fileName ))
      else if (IsVolume(fileNameLC))
      {
        return SeriesTypes.Dicom;
      } // else if( IsVolume(fileName) )
      else if (IsCad(fileNameLC))
      {
        return SeriesTypes.Cad;
      } // else if( IsCad(fileName) )
      else
      {
        return SeriesTypes.Preprocessed;
      } // else
    } // DetermineSeriesType( file )

        
    /// <summary>
    /// Handles the specified file.
    /// </summary>
    /// <param name="manager">The import manager to build up.</param>
    /// <param name="file">The file to handle.</param>
    private FileParseInfo HandleFile(ImportManager20 manager, string file)
    {
      SeriesTypes seriesType = DetermineSeriesType(file);

      if (seriesType == SeriesTypes.Dicom)
      {
        return HandleDicom(manager, file);
      }
      else if (seriesType == SeriesTypes.Preprocessed)
      {
        return HandlePreprocessed(manager, file);
      }
      else if (seriesType == SeriesTypes.Cad)
      {
        return HandleCAD(manager, file);
      }
      else
      {
        return HandleSession(manager, file);
      }
    } // HandleFile( file )


    /// <summary>
    /// Handles a dicom file.
    /// </summary>
    /// <param name="manager">The manager to add the file to.</param>
    /// <param name="file">The file to read.</param>
    private FileParseInfo HandleDicom(ImportManager20 manager, string file)
    {
      if(!m_parsedDicomDatasets.Contains(Utils.GetDataSet(file)))
      {
        FileParseInfo info = new FileParseInfo(file, string.Format("Dicom\\{0}", ImageFactory.Create(file, 
          ParameterType.FILE).InstanceNumber.ToString()));
        info.SeriesType = SeriesTypes.Dicom;
      }

      return null;
    } // HandleDicom( manager, file )


    /// <summary>
    /// Handles a volume file.
    /// </summary>
    /// <param name="manager">The manager to add the file to.</param>
    /// <param name="file">The file to read.</param>
    private FileParseInfo HandleVolume(ImportManager20 manager, string file)
    {
      FileParseInfo info = new FileParseInfo(file, "volumes\\{0}");
      info.SeriesType = SeriesTypes.Dicom;

      return info;
    } // HandleVolume( manager, file )


    /// <summary>
    /// Handles a preprocessed file.
    /// </summary>
    /// <param name="manager">The manager to add the file to.</param>
    /// <param name="file">The file to read.</param>
    private FileParseInfo HandlePreprocessed(ImportManager20 manager, string file)
    {
      FileParseInfo info = new FileParseInfo(file, "{0}");
      info.SeriesType = SeriesTypes.Preprocessed;

      return info;
    } // HandlePreprocessed( manager, file )


    /// <summary>
    /// Handles a cad file.
    /// </summary>
    /// <param name="manager">The manager to add the file to.</param>
    /// <param name="file">The file to read.</param>
    private FileParseInfo HandleCAD(ImportManager20 manager, string file)
    {
      FileParseInfo info = new FileParseInfo(file, "{0}");
      info.SeriesType = SeriesTypes.Preprocessed;

      return info;
    } // HandleCAD( manager, file )


    /// <summary>
    /// Handles a session file.
    /// </summary>
    /// <param name="manager">The manager to add the file to.</param>
    /// <param name="file">The file to read.</param>
    private FileParseInfo HandleSession(ImportManager20 manager, string file)
    {
      string ext = Path.GetExtension(file).ToLower();

      // The mkvl and vcsx require the volume series to be in place to get the uid.  Do this at the end.
      if (ext != ".vrx" && ext != ".mkvl" && ext != ".vcsx" )
      {
        FileParseInfo info = new FileParseInfo(file);
        info.Destination = "{0}";
        info.SeriesType = SeriesTypes.Session;

        if(IsMovie(file))
        {
          info.Destination = string.Format("Movies\\{0}", Path.GetFileName(file));
          info.Dataset = string.Empty;
        }
        else if(IsSnapshot(file))
        {
          if(info.Dataset != "Unknown")
          {
            manager.AddFile(info.Dataset, SeriesTypes.Dicom, "VC", info.Source, info.Destination, info.MacroReplace);
            info.SeriesType = SeriesTypes.Preprocessed;
          }
          else
          {
            // Look for matching movie.  Will serve as coresponding preview image.
            string[] matchingFiles = Directory.GetFiles(Path.GetDirectoryName(file), Path.GetFileNameWithoutExtension(file) + ".*");
            if(matchingFiles.Length > 0)
            {
              foreach(string compareFile in matchingFiles)
              {
                if(IsMovie(compareFile))
                {
                  info.Destination = string.Format("Movies\\{0}", Path.GetFileName(file));
                }
              }
            }
          }
        }
        else if(IsSession(file))
        {
          info.Dataset = string.Empty;
        }

        return info;
      }

      return null;
    } // HandleSession( manager, file )
    
    #endregion

  } // VCFilter20
} // namespace Viatronix.Utilities.Importer.Filters

#region revision history

// $Log: VCFilter20.cs,v $
// Revision 1.1.2.5  2009/06/04 19:15:34  kchalupa
// Coding Standards.
//
// Revision 1.1.2.4  2008/10/20 16:06:21  kchalupa
// Fix for VC Sessions to remove extra datasets that the user may have sent that threw off the VRX file's integrity.
//
// Revision 1.1.2.3  2008/10/17 16:31:26  kchalupa
// Issue 6183 VC Sessions Reports are not correct.  Snapshots are missing.
//
// Revision 1.1.2.2  2008/10/17 15:44:09  kchalupa
// Fix for VC Session files not being read in properly in VC.
//
// Revision 1.1.2.1  2008/09/24 14:31:19  mkontak
// Initial Check in
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities.Importer/Filters/VCFilter20.cs,v 1.1.2.5 2009/06/04 19:15:34 kchalupa Exp $
// $Date: 2009/06/04 19:15:34 $24 July 2008
// $Id: VCFilter20.cs,v 1.1.2.5 2009/06/04 19:15:34 kchalupa Exp $000000004

#endregion
