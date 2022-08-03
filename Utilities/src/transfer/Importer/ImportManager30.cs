// $Id: ImportManager30.cs,v 1.1.2.6 2011/05/18 19:59:56 kchalupa Exp $
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
using System.Configuration;
using System.Collections.Generic;

using Viatronix.Logging;
using Viatronix.Encryption;
using Viatronix.Enterprise;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.Data;
using Viatronix.Utilities.IO;
using Viatronix.Utilities.Registry;
using Viatronix.Utilities.Importer.Data;

namespace Viatronix.Utilities.Importer
{
  /// <summary>
  /// Performs import operations on V3D 3.0 datasets.
  /// </summary>
  public class ImportManager30 : IImporter
  {

    #region fields

    /// <summary>
    /// The source directory the patient is contained in.
    /// </summary>
    private string m_sourceDirectory = null;

    /// <summary>
    /// The patient that the ParsedStudy collection belongs to.
    /// </summary>
    private Patient m_patient = null;

    /// <summary>
    /// The list of ParsedStudies that belong to the patient.
    /// </summary>
    private List<ParsedStudy> m_studies = null;

    /// <summary>
    /// The remaining number of files to copy.
    /// </summary>
    private int m_remainingFiles = 0;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="patient">The patient that was read in.</param>
    /// <param name="patientDirectory">Directory of the patient xml file.</param>
    public ImportManager30(Patient patient, string patientDirectory)
    {
      m_patient = patient;
      m_sourceDirectory = patientDirectory;
      m_studies = new List<ParsedStudy>();
    } // ImportManager30(Patient patient, string patientDirectory)

    #endregion

    #region properties

    /// <summary>
    /// The patient object that will be added to the system.
    /// </summary>
    public Patient Patient
    {
      get { return m_patient;  }
      set { m_patient = value; }
    } // Patient


    /// <summary>
    /// Gets or sets the list of parsed studies.
    /// </summary>
    public List<ParsedStudy> ParsedStudies
    {
      get { return m_studies;  }
      set { m_studies = value; }
    } // ParsedStudies

    #endregion

    #region methods

    /// <summary>
    /// Adds a study from the specified Xml file.
    /// </summary>
    /// <param name="xmlStudyFile">An Xml serialization of a study.</param>
    /// <returns>A Parsed study based on the inputs.</returns>
    public ParsedStudy AddStudy(string xmlStudyFile)
    {
      if( xmlStudyFile != null && File.Exists(xmlStudyFile) )
      {
        ParsedStudy pStudy = ParsedStudy.Create(xmlStudyFile, m_patient);
        m_studies.Add(pStudy);

        //pStudy.Study.Patient = m_patient;

        pStudy.Study.Name = m_patient.Name;
        pStudy.Study.MedicalId = m_patient.MedicalId;
        pStudy.Study.Dob = m_patient.Dob;
        pStudy.Study.Gender = m_patient.Gender;
        pStudy.Study.Weight = m_patient.Weight;

        m_patient.Studies.Add(pStudy.Study);
        return pStudy;
      } // if(seriesXmlFileName != null && File.Exists(seriesXmlFileName))
      else
      {
        Utils.LogMessage(LogTypes.Error, m_patient, "Import of Study failed;" +
          " malformed study xml file found.");
      }

      return null;
    } // AddStudy(string xmlStudyFile)


    /// <summary>
    /// Adds the parsed study from a parsed study object.
    /// </summary>
    /// <param name="parsedStudy">The study to add.</param>
    //public void AddStudy(ParsedStudy parsedStudy)
    //{
    //  m_patient.Studies.Add(parsedStudy.Study);
    //  //parsedStudy.Study.Patient = m_patient;
    //  m_studies.Add(parsedStudy);
    //} // AddStudy()


    /// <summary>
    /// Removes the specified study from the list.
    /// </summary>
    /// <param name="parsedStudy">Study to remove.</param>
    //public void RemoveStudy(ParsedStudy parsedStudy)
    //{
    //  if(m_studies.Contains(parsedStudy))
    //  {
    //    m_studies.Remove(parsedStudy);
    //  } // if(m_studyList.Contains(parsedStudy))
    //} // RemoveStudy(ParsedStudy parsedStudy)


    /// <summary>
    /// This method replaces the half filled references and dependents objects created from the series xml
    /// files with the full fledged objects.  This fixes inconsistencies in the database when a half filled
    /// object was inserted into the database.  As a result, critical fields were left unfilled.
    /// </summary>
    private void MakeReferencesFullWeight(ParsedStudy study)
    {
      // Build up a map of uid to full fledged series objects.
      Dictionary<string, Series> uidToSeries = new Dictionary<string, Series>();
      foreach (Series series in study.Study.Series)
      {
        Series dummy = null;
        string uid = series.Uid.Replace(".", "_");
        if (!uidToSeries.TryGetValue(uid, out dummy))
        {
          uidToSeries.Add(uid, series);
        } // if(!uidToSeries.TryGetValue(series.UID.ToString("_"), out series))
      } // foreach(Series series in study.Series)

      foreach (Series series in study.Study.Series)
      {
        // Run through the dependent series replacing the light weight objects with full weight ones.

        for (int i = series.Dependents.Count - 1; i >= 0; i--)
        {
          Series lightWeight = series.Dependents[i];
          string dependentUID = lightWeight.Uid.Replace(".", "_");

          Series fullWeight = null;
          if (uidToSeries.TryGetValue(dependentUID, out fullWeight))
          {
            series.Dependents.RemoveAt(i);
            series.Dependents.Insert(i, fullWeight);
          } // if(uidToSeries.TryGetValue(dependentUID, out fullWeight))
          else
          {
            Log.Warning("Dependent Series " + dependentUID + " could not be located, full weight" +
              "replacement could not be performed.", "ImportManager30", "MakeReferencesFullWeight");
          } // else
        } // for(int i=0;i<series.DependentSeries.Count;i++)

        for (int i = series.References.Count - 1; i >= 0; i--)
        {
          Series lightWeight = series.References[i];
          string referenceUID = lightWeight.Uid.Replace(".", "_");

          Series fullWeight = null;
          if (uidToSeries.TryGetValue(referenceUID, out fullWeight))
          {
            series.References.RemoveAt(i);
            series.References.Insert(i, fullWeight);
          } // if(uidToSeries.TryGetValue(dependentUID, out fullWeight))
          else
          {
            Log.Error(string.Format("Referenced Series {0} could not be located, study import cancelled.", referenceUID), "ImportManager30", "MakeReferencesFullWeight");
            Utils.LogMessage(LogTypes.Error, series, string.Format("Referenced Series {0} could not be located, study import cancelled.", referenceUID));
            throw new ApplicationException(string.Format("Referenced Series {0} could not be located, study import cancelled.", referenceUID));
          } // else
        } // for(int i=0;i<series.DependentSeries.Count;i++)
      } // foreach(Series series in study.Series)
    } // MakeReferencesFullWeight(Study study)


    /// <summary>
    /// Inserts the Patient, Study collection and Series collection into the Viatronix DB.
    /// </summary>
    /// <param name="ioArgs">Determines whether or not the session should be included.</param>
    /// <returns>True if the series was recorded successfully or false otherwise.</returns>
    /// <exception cref="System.ApplicationException">
    /// If an error occurred in recording to the database.
    /// </exception>
    private bool InsertIntoDatabase(ImporterArgs ioArgs)
    {
      bool result = false;

      foreach (ParsedStudy pStudy in ParsedStudies)
      {
        // Sort the ParsedSeries in the list so that the dependent series will be added first into the 
        // database.
        SeriesTypeComparer comparer = new SeriesTypeComparer();
        pStudy.ParsedSeriesList.Sort(comparer);

        foreach (ParsedSeries pSeries in pStudy.ParsedSeriesList)
        {
          try
          {
            // Make sure either the session can be added or if not, that this series that we are dealing with
            // is not the session.
            if (ioArgs.ImportSessions || (!ioArgs.ImportSessions && pSeries.Series.Type != SeriesTypes.Session ) )
            {
              // Perform the database record.
              if (ioArgs.PerformDatabaseRecord)
              {
                pSeries.PerformDatabaseInsert(ioArgs);
              } // if(ioArgs.PerformDatabaseRecord)

              if (ioArgs.PerformCopy || ioArgs.PerformDatabaseRecord)
              {
                // If either action was taken, log a message that the import was successful.

                Utils.LogMessage(LogTypes.Info, pSeries.Series, "Imported");

                Log.Information(m_patient.Name + "; " + pStudy.Study.Uid.Replace(".", "_") +
                  "; " + pSeries.Series.Uid + "; Imported", "ImportManager30", "InsertIntoDatabase");
              } // if( ioArgs.PerformCopy || ioArgs.PerformDatabaseRecord )
              else
              {
                // If neither action was taken, make a note of that.
                Log.Warning(m_patient.Name + "; " + pStudy.Study.Uid.Replace(".", "_") +
                  pSeries.Series.Uid.Replace(".", "_") + "; " + "Importing Failed; Nothing to be done.", "ImportManager30", "InsertIntoDatabase");
              } // else
            } // if( ioArgs.PerformCopy || ioArgs.PerformDatabaseRecord )
            else
            {
              // Warn that the session was skipped over.

              Utils.LogMessage(LogTypes.Warning, pSeries.Series,
                "Import Failed; Importing of Session was not performed at the discretion of the user.");
            } // else
          } // try
          catch (Exception e)
          {
            Utils.LogMessage(LogTypes.Error, pSeries.Series, "Import Failed; Error in Importing Series.");

            Log.Error(m_patient.Name + "; " + pStudy.Study.Uid.Replace(".", "_") +
              "; " + pSeries.Series.Uid.Replace(".", "_") +
              "; Import Failed; Error in Importing Series; Exception " + e.Message, "ImportManager30", "InsertIntoDatabase");

            throw new ApplicationException("Error in Inserting to Viatronix DB");
          } // catch(Exception e)
        } // foreach(ParsedSeries pSeries in m_parsedStudy.ParsedSeriesList)
      } // foreach(ParsedStudy pStudy in ParsedStudyList)

      return result;
    } // InsertIntoDatabase()


    /// <summary>
    /// Initiates a copy command cascade catastrophe to the individual parsed series
    /// objects contained herein.  Both the regular m_seriesList and m_sessionSeries are copied.
    /// </summary>
    /// <param name="ioArgs">Determines whether file copy database records should take place
    /// and if the session should be included.</param>
    public void Copy(ImporterArgs args)
    {
      try
      {
        // The patient, study, series directories (location and full path on the series) are set up in the Filter30 as we're reading the source directory.  

        // Check to see if the patient already exists in the databse and file system.
        bool patientExistsDB = false;
        bool patientExistsFS = false;

        if(args.PerformCopy && Directory.Exists(Path.Combine(args.Destination, m_patient.Directory)))
        {
          patientExistsFS = true;
        }

        if(args.PerformDatabaseRecord)
        {
          List<Study> studies = args.Provider.GetStudies(XmlBuilder.Create("study", string.Format("<query mid=\"{0}\" />", m_patient.MedicalId)));
          foreach (Study study in studies)
          {
            if (m_patient.MedicalId == study.MedicalId)
            {
              patientExistsDB = true;
              break;
            }
          }
        }

        if(patientExistsDB && patientExistsFS)
        {
          // Remove or fail.

          Log.Warning("Patient is already included in the database and in the File System.  [PATIENT=" + m_patient.ToString() + "]", "ImportManager30", "Copy");

          // For Remove.
          //Directory.Delete(Path.Combine(ioArgs.Destination, m_patient.Directory));
          //Provider.Database.DeletePatient(conString, m_patient);

          // For failure.
          // throw new ApplicationException("Patient exists in viatronix database and in cache base path.");
        }
        else if(patientExistsDB && !patientExistsFS)
        {
          // Remove db entry and import or fail.

          Log.Warning("Patient is already included in the database, but not in the File System.  [PATIENT=" + m_patient.ToString() + "]", "ImportManager30", "Copy");

          // For remove db entry.
          // Provider.Database.DeletePatient(conString, m_patient);

          // For failure.
          // throw new ApplicationException("Patient exists in viatronix database");
        }
        else if(!patientExistsDB && patientExistsFS)
        {
          Log.Warning("Patient is already included in the File System, but not in the Database.  [PATIENT=" + m_patient.ToString() + "]", "ImportManager30", "Copy");

          // Delete FS copy and proceed.
          // Directory.Delete(Path.Combine(ioArgs.Destination, m_patient.Directory));
        }

        Global.CheckForCancel();

        foreach (ParsedStudy parStudy in m_studies)
        {
          /// Make the series full weight.
          MakeReferencesFullWeight(parStudy);
        }

        Global.CheckForCancel();

        if(args.PerformCopy && Directory.Exists(Path.Combine(args.Destination, m_patient.Directory)))
        {
          throw new IOException("Path already exists. Path: " + Path.Combine(args.Destination, m_patient.Directory));
        }

        try
        {
          // Perform the copy if it should be done.
          if (args.PerformCopy)
          {
            int totalFiles = PathUtilities.GetFileCount(args.Source, true);
            m_remainingFiles = totalFiles;

            CopyDirectory(args.Source, args.Destination, false, false, true, totalFiles);
            Utils.ClearReadOnlyAttributeRecursively(Path.Combine(args.Destination, Path.GetFileName(args.Source)));

            // --------------------------------------------------
            // After copy, move volume series into the dicom series.
            // ==================================================

            List<string> volumeFilesAdded = new List<string>();
            List<ParsedSeries> volumeSeries = new List<ParsedSeries>();

            foreach (ParsedStudy parStudy in m_studies)
            {
              foreach (ParsedSeries parSeries in parStudy.ParsedSeriesList)
              {

                // ==========================================================================================================
                // TODO: Changed to reflect the new changes imn the structure. No more volumes they are now embedded in the 
                //       DICOM.
                //
                //  if (parSeries.Series.Type.ToLower() == "volume" && parSeries.Series.References.Count == 1 &&
                //  parSeries.Series.References[0].Type.ToLower() == "dicom")
                // ==========================================================================================================
                if (parSeries.Series.Type == SeriesTypes.Dicom && parSeries.Series.References.Count == 1 )
                {
                  ParsedSeries dicomSeries = parStudy.Find(parSeries.Series.References[0].Uid);

                  if (dicomSeries != null)
                  {
                    string sourceVolumeDirectory = Path.Combine(args.Destination, parStudy.Study.Directory, parSeries.Series.Directory);
                    string destVolumeDirectory = Path.Combine(args.Destination, parStudy.Study.Directory, dicomSeries.Series.Directory, "volumes");
                    if (!Directory.Exists(destVolumeDirectory))
                    {
                      Directory.CreateDirectory(destVolumeDirectory);
                    }

                    // Move .vol and .txt into the dicom directory.

                    int count = 1;
                    string[] volumes = Directory.GetFiles(sourceVolumeDirectory, "*.vol");
                    foreach (string volumeFile in volumes)
                    {
                      string destFilePath = Path.Combine(destVolumeDirectory, Path.GetFileName(volumeFile));
                      File.Move(volumeFile, destFilePath);

                      string volumeFileWithoutExt = Path.GetFileNameWithoutExtension(volumeFile);
                      if (!volumeFilesAdded.Contains(volumeFileWithoutExt))
                      {
                        volumeFilesAdded.Add(volumeFileWithoutExt);
                        dicomSeries.Series.CreateVolume("1", string.Format("volume{0:3D}",count++), null, destFilePath, parSeries.Series.ImageCount);
                      }
                    }

                    volumes = Directory.GetFiles(sourceVolumeDirectory, "*.ecv");
                    foreach (string volumeFile in volumes)
                    {
                      string destFilePath = Path.Combine(destVolumeDirectory, Path.GetFileName(volumeFile));
                      File.Move(volumeFile, destFilePath);

                      string volumeFileWithoutExt = Path.GetFileNameWithoutExtension(volumeFile);
                      if (!volumeFilesAdded.Contains(volumeFileWithoutExt))
                      {
                        volumeFilesAdded.Add(volumeFileWithoutExt);
                        dicomSeries.Series.CreateVolume("1", string.Format("volume{0:3D}", count++), null, destFilePath, parSeries.Series.ImageCount);
                      }
                    }

                    volumes = Directory.GetFiles(sourceVolumeDirectory, "*_hq");
                    foreach (string volumeFile in volumes)
                    {
                      string destFilePath = Path.Combine(destVolumeDirectory, Path.GetFileName(volumeFile));
                      File.Move(volumeFile, destFilePath);

                      string volumeFileWithoutExt = Path.GetFileNameWithoutExtension(volumeFile);
                      if (!volumeFilesAdded.Contains(volumeFileWithoutExt))
                      {
                        volumeFilesAdded.Add(volumeFileWithoutExt);
                        dicomSeries.Series.CreateVolume("1", string.Format("volume{0:3D}", count++), null, destFilePath, parSeries.Series.ImageCount);
                      }
                    }

                    string[] convLogs = Directory.GetFiles(sourceVolumeDirectory, "*.txt");
                    foreach (string convLog in convLogs)
                    {
                      File.Move(convLog, Path.Combine(destVolumeDirectory, Path.GetFileName(convLog)));
                    }

                    // Delete the volume directory.
                    Directory.Delete(sourceVolumeDirectory, true);

                    // Add the series to be removed from reference lists.
                    volumeSeries.Add(parSeries);
                  }
                  else
                  {
                    Log.Error("Could not find dicom series referenced by volume series.", "ImportManager30", "Copy");
                    throw new NotFoundException("Could not find dicom series referenced by volume series.");
                  }
                }
              }
            }

            foreach (ParsedStudy parStudy in m_studies)
            {
              // Remove references from all series.
              foreach (ParsedSeries parSeries in parStudy.ParsedSeriesList)
              {
                foreach(ParsedSeries seriesToRemove in volumeSeries)
                {
                  RemoveReference(parSeries.Series, seriesToRemove.Series);
                }
              }

              // Remove instance of volume series from any studies.
              foreach (ParsedSeries seriesToRemove in volumeSeries)
              {
                parStudy.ParsedSeriesList.Remove(seriesToRemove);
              }
            }

            // Perform the database insert of objects if it should be done.
            if (args.PerformDatabaseRecord)
            {
              InsertIntoDatabase(args);
            }
          }
        }
        catch (Exception e)
        {
          Log.Error("Failed to copy the directory, cleaning up.  [ERROR=" + e.Message + "]", "ImportManager30", "Copy");

          try
          {
            if( Directory.Exists(Path.Combine(args.Destination, m_patient.Directory)))
            {
              PathUtilities.DeleteDirectory(Path.Combine(args.Destination, m_patient.Directory));
            }
          }
          catch(Exception ex)
          {
            Log.Error("Failed to clean up [ERROR=" + ex.Message + "]", "ImportManager30", "Copy");
          }

          throw;
        }

        Global.LogPatientImportSucceeded(args.Source);
        Utils.LogMessage(LogTypes.Info, m_patient, "Imported");
        Log.Information(m_patient.Name + "; Imported", "ImportManager30", "Copy");
      }
      catch (System.IO.IOException e)
      {
        Utils.LogMessage(LogTypes.Error, m_patient, "Importing Failed; Error in copying directory contents.");
        Log.Error(m_patient.Name + "; Importing Failed; Error in " + "copying directory contents. [ERROR=" + e.Message + "]", "ImportManager30", "Copy");
        Global.LogPatientImportFailed(args.Source);
      }
      catch (Exception e)
      {
        Utils.LogMessage(LogTypes.Error, m_patient, "Importing Failed; Error in importing patient.");
        Log.Error(m_patient.Name + "; Importing Failed; Error in importing patient [Error=" + e.Message + "]", "ImportManager30", "Copy");
        Global.LogPatientImportFailed(args.Source);
      }
    } // Copy(ImporterArgs ioArgs)


    /// <summary>
    /// Copies the source diretory to the destination directory.
    /// </summary>
    /// <param name="source">Source directory</param>
    /// <param name="destination">Destination directory</param>
    /// <param name="rename">
    /// Rename flag indicates whether the destination is an exact path or a parent to the sources path. If true then it means we are looking to 
    /// rename the directory if false we are looking to copy the source into the destination as a sub-directory.
    /// </param>
    /// <param name="overWrite">Flag indicating you want to overwrite the destination if it exists.</param>
    /// <param name="recursive">Copies all files and sub-directories recursivly.</param>
    /// <param name="totalFiles">The total number of files.</param>
    /// <param name="remaining">The number of remaining files.</param>
    private void CopyDirectory(String source, String destination, bool rename, bool overWrite, bool recursive, int totalFiles)
    {
      if ( Directory.Exists(source) )
      {
        String destPath = ( rename ? destination : String.Concat(destination, Path.DirectorySeparatorChar.ToString(), Path.GetFileName(source)) );
 
        /////////////////////////////////////////////////////
        // Make sure the destination does not already exist
        /////////////////////////////////////////////////////
        if ( Directory.Exists(destPath) )
        {
          /////////////////////////////////////////////
          // If overwrite is set delete the destination
          /////////////////////////////////////////////
          if ( overWrite )
          {
            Viatronix.Utilities.IO.PathUtilities.DeleteDirectoryContents(destPath, true);
          } // END ... If we are to overwrite
          else
          {
            Log.Error(string.Concat("Path already exists [", destPath, "] and overwrite was not specified"), "PathUtilities", "CopyDirectory");
            throw new System.IO.IOException("Path already exists");
          } // END ... Else not overwrite
        } // END ... If the paths exists
        else
        {
          Log.Debug(string.Concat("Creating destination directory [", destPath, "]"), "PathUtilities", "CopyDirectory");
          Directory.CreateDirectory(destPath);
        }

        Log.Debug(string.Concat("Creating destination directory [", destPath, "]"), "PathUtilities", "CopyDirectory");
        Directory.CreateDirectory(destPath);

        // Get all files in the source
        string[] files = Directory.GetFiles(source);


        int filesCopied = totalFiles - m_remainingFiles;

        //////////////////
        // Copy each  file
        //////////////////
        for ( int i =0; i < files.Length; ++i )
        {
          Global.CheckForCancel();

          int completed = 5 + (int)((((double)filesCopied++ / (double)totalFiles)) * 90.0);
          Global.NotifyProgress(string.Format("Copying File: {0}", Path.GetFileNameWithoutExtension(files[i])), completed);

          String destFile = Path.Combine(destPath, Path.GetFileName(files[i]));
          File.Copy(files[i], destFile, true);
        } // END ... for each file

        m_remainingFiles -= files.Length;

        //////////////////////////////////////////
        // Only copy sub-directories is recursive
        //////////////////////////////////////////
        if ( recursive )
        {
          String[] dirs = Directory.GetDirectories(source);

          ///////////////////////////////////////
          // For each sub-directory call ourself
          ///////////////////////////////////////
          for ( int i = 0; i < dirs.Length; i++ )
          {
            // Call ourself
            CopyDirectory(dirs[i], destPath, false, overWrite, recursive, totalFiles);
          } // END ... for each directory

        } // END ... If recursive
      }
    } // CopyDirectory(String * sSource, String * sDestination, bool bRename, bool bOverWrite, bool bRecursive)


    /// <summary>
    /// Removes the reference from the series tree.
    /// </summary>
    /// <param name="target">The series whose references will be looked through for referenceToRemove.</param>
    /// <param name="referenceToRemove">The series to remove.</param>
    public static void RemoveReference(Series target, Series referenceToRemove)
    {
      for (int i = target.References.Count - 1; i > -1; --i)
      {
        Series reference = target.References[i];
        if (reference != referenceToRemove)
        {
          RemoveReference(reference, referenceToRemove);
        }
        else
        {
          target.References.RemoveAt(i);
        }
      }
    } // RemoveReference( target, referenceToRemove )

    #endregion

  } // ImportManager30
} // namespace Viatronix.Utilities.Importer

#region revision history

// $Log: ImportManager30.cs,v $
// Revision 1.1.2.6  2011/05/18 19:59:56  kchalupa
// CONS-1640
// Updated the references to the SystemSettings to the new data structures.
//
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.1.2.5  2009/10/30 15:54:47  kchalupa
// Attempt to clean up if copy fails.
//
// Revision 1.1.2.4  2009/10/20 14:05:39  kchalupa
// Clear the read only bits in the imported directory.
//
// Revision 1.1.2.3  2009/07/29 16:09:59  kchalupa
// Overwrite old Series.xml of all imported 3.0 series with the new spec.
//
// Revision 1.1.2.2  2009/06/04 19:31:00  kchalupa
// Coding standards, slimmed down mini log, new status logging.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities.Importer/ImportManager30.cs,v 1.1.2.6 2011/05/18 19:59:56 kchalupa Exp $
// $Id: ImportManager30.cs,v 1.1.2.6 2011/05/18 19:59:56 kchalupa Exp $

#endregion
