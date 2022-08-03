// $Id: VCFileStructureFilter.cs,v 1.4.2.8 2007/09/13 17:03:17 mkontak Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.ComponentModel;
using System.Collections;
using System.IO;
using System.Windows.Forms;
using System.Xml;
using System.Drawing;

using Viatronix.v3D.UI;
using Viatronix.v3D.Core;
using Viatronix.v3D.Utilities;
using Viatronix.v3D.Utilities.IO;
using Viatronix.v3D.Dicom;

namespace Viatronix.v3D.Console
{
	/// <summary>
	/// Filters a directory using a VC file layout.
	/// </summary>
	public class VCFileStructureFilter : FileStructureFilter
	{

    #region fields

    /// <summary>
    /// Default text added to the Patient's TreeNode to denote this filter.
    /// </summary>
    private const string DisplayText = "(VC File Structure)";

    /// <summary>
    /// Default text added to denote a prone session.
    /// </summary>
    private const string SupineText = "Supine";

    /// <summary>
    /// Default text added to denote a prone session.
    /// </summary>
    private const string ProneText = "Prone";

    /// <summary>
    /// Default text added to denote a prone session.
    /// </summary>
    private const string VolumeText = "Volume";

    /// <summary>
    /// Default text added to denote a prone session.
    /// </summary>
    private const string DicomText = "Dicom";

    /// <summary>
    /// Default text added to denote a prone session.
    /// </summary>
    private const string PreprocessedText = "Preprocessed";

    /// <summary>
    /// Default text added to denote a prone session.
    /// </summary>
    private const string SessionText = "Session";

    /// <summary>
    /// Default text added to denote a prone session.
    /// </summary>
    private const string CADText = "CAD";

    /// <summary>
    /// Filter for isolating supine files.
    /// </summary>
    private const string SupineFilter = "_?s";

    /// <summary>
    /// Filter for isolating prone files.
    /// </summary>
    private const string ProneFilter = "_?p";

    private readonly string[] SessionExtensions = new string[] { ".wmv", ".avi"  };

    #endregion

    #region override methods

    /// <summary>
    /// Determines if the structure returned by the filter is relative to the full paths in the series objects.
    /// </summary>
    public override bool IsRelative
    {
      get { return true; }
    } // IsRelative


    /// <summary>
    /// Determines if any temporary files need to be deleted.
    /// </summary>
    public override bool RequiresDeletion
    {
      get { return true; }
    } // RequiresDeletion


    /// <summary>
    /// Determines if the specified path matches a VCFileStructureFilter.
    /// </summary>
    /// <param name="path">The file system path.</param>
    /// <returns>true if the path is a VCFileStructureFilter; otherwise false</returns>
    public override bool CanProcess( string path )
    {
      if( !Directory.Exists( path ))
        return false;

      bool canProcess = false;

      // check for supine or prone
      string[] files = Directory.GetFiles( path, "*_??.ins" );
      if( files.Length > 0 )
      {
        string file = Path.GetFileNameWithoutExtension( files[0] ).ToLower();
        if( file[ file.Length - 1 ] == 's' || file[ file.Length - 1 ] == 'p' )
          canProcess |= true;
      }
      else
      {
        string subType = GetSubType(path);
        if( subType == "vc" )
          canProcess |= true;
      }

      if( canProcess )
      {
        files = Directory.GetFiles( path, "*.se?" );

        // exclude version earlier than 1.2.2 (*.set, *.sec, *.ses)
        if( files.Length > 0 )
        {
          string file = files[0].ToLower();
          switch( file[ file.Length - 1 ])
          {
            case 't': goto case 's';
            case 'c': goto case 's';
            case 's':
              canProcess = false;
              string message = "The 3D model was built with a version older than 1.2.2 and cannot be imported.";
              Viatronix.v3D.Logging.Logger.Dbg( message );
              Global.Frame.Invoke( new SetterDelegate< string >( Viatronix.v3D.UI.MessageBox.ShowWarning ), message );
              throw new FileStructureFilterException( message );
          }
        }
      }

      return canProcess;
    } // CanProcess( path )


    /// <summary>
    /// Creates a Tree structure that represents the data contained in the specifeid path.
    /// </summary>
    /// <param name="path">The file system path.</param>
    /// <returns>A Tree structure that represents the data contained in the specifeid path.</returns>
    public override Patient Process( string path, CancelEventArgs args )
    {
      string patientDirectory = string.Empty;
      ElementList elements = null;
      Patient patient = null;

      try
      {
        OnProgress( 5, 100 );

        // create and element list from the provided path
        elements = CreateElementList( path );
        patient = CreatePatient( elements );
        Study study = CreateStudy( elements, patient );

        // create a session series and assign all the files
        Series session = CreateSeries(elements, SessionText, "VC", SessionText + " Series", study, true);
        //session.FullPath = "";
        session.Modality = "SC";

        OnProgress( 10, 100 );

        // create all series associated with the Supine orientation
        SeriesCollection collection = CreateSeriesCollection( path, SupineFilter, SupineText, study, session, 10, args );
        OnProgress( 50, 100 );

        // create all series associated with the Prone orientation
        collection = CreateSeriesCollection( path, ProneFilter, ProneText, study, session, 50, args );
        OnProgress( 90, 100 );



        Bitmap bitmap = new Bitmap(System.Reflection.Assembly.GetExecutingAssembly().GetManifestResourceStream("Viatronix.v3D.Console.Core.res.movie.png"));
        ArrayList files = FindSessionFiles(path, args);

        foreach (string file in files)
        {

          string baseName = Path.GetFileNameWithoutExtension(file);
          string filename = Viatronix.v3D.Utilities.IO.PathUtilities.CombinePaths("Movies",Path.GetFileName(file));

          string directory = Viatronix.v3D.Utilities.IO.PathUtilities.CombinePaths(session.FullPath, "Movies");

          if (!Directory.Exists(directory))
          {
            Directory.CreateDirectory(directory);
          }

          File.Copy(file, Viatronix.v3D.Utilities.IO.PathUtilities.CombinePaths(session.FullPath, filename));
          bitmap.Save(Viatronix.v3D.Utilities.IO.PathUtilities.CombinePaths(session.FullPath, "Movies", baseName + ".jpg"), System.Drawing.Imaging.ImageFormat.Jpeg);


          // assign any session files
          session.Files.Add(filename);
        }


        // if the session contains files then addit to the study
        if (session.Files.Count > 0)
        {
          //collection.Add(session);
          //study.Series.Add(session);
          foreach (Series series in study.Series)
          {
            if (series.Type.ToLower() == "session")
              continue;

            series.DependentSeries.Add(session);
            session.ReferencedSeries.Add(series);

          }

          session.Serialize(session.FullPath);
        }
        else
        {
          study.Series.Remove(session);
          Directory.Delete(session.FullPath);
        }

        study.Serialize(Path.Combine(Utilities.Registry.SystemSettings.TempDirectory, study.Location));

        OnProgress( 100, 100 );
      }
      catch ( Exception ex )
      {
        Logging.Logger.Err("Failed to filter study : " + ex.Message);

        if (patientDirectory.Length > 0 && Directory.Exists(patientDirectory))
        {
          Logging.Logger.Dbg("Deleting temporay patient directory [" + patientDirectory + "] due to exception");
          Directory.Delete(patientDirectory, true);
        }

        throw;
      }
      finally
      {
        /// Dispose of the elemnt list toi relase the unmanaged resources
        if ( elements != null ) elements.Dispose();
      }

      return patient;
    } // Process( path )

    #endregion

    #region methods

    /// <summary>
    /// Creates a series collection matching the orientation provided.
    /// </summary>
    /// <param name="path">The path containing the files.</param>
    /// <param name="filter">Orentation filter</param>
    /// <param name="orientation">Orientation</param>
    /// <param name="study">The parent study</param>
    /// <param name="args">CancelEventArgs for early termination.</param>
    /// <returns>A SeriesCollection for the orientation.</returns>
    private SeriesCollection CreateSeriesCollection( string path, string filter, string orientation, Study study, Series session, int progress, CancelEventArgs args )
    {
      SeriesCollection collection = new SeriesCollection();
      ElementList elements = null;

      try
      {
        string[] files = Directory.GetFiles(path, "*" + filter + "*");

        //Take the VRX file here and add it with the Supine Series.
        //Backward compatiblity hack
        if ( filter == SupineFilter )
        {
          string[] reportFile = Directory.GetFiles(path, "*.vrx");
          reportFile.CopyTo(files, 0);
        }//if ( filter == SupineFilter )


        /////////////////////////////////////////////////
        // Make sure there are files in the directory
        /////////////////////////////////////////////////
        if (files.Length == 0)
        {
          return collection;
        }

        /// Create the element list
        elements = CreateElementList( path, "*" + filter + "*" );


        // create a dicom series and assign all the files
        Series dicom = CreateSeries( elements, DicomText, string.Empty, orientation + " " + DicomText + " Series", study, false );
        dicom.PatientOrientation = orientation;
      
        collection.Add( dicom );

        // create a volume series
        Series volume = CreateSeries( elements, VolumeText, string.Empty, orientation + " " + VolumeText + " Series", study, true );
        volume.PatientOrientation = orientation;

        // create a preprocessed series
        Series preprocessed = CreateSeries(elements, PreprocessedText, "VC", orientation + " " + PreprocessedText + " Series",  study, true);
        //preprocessed.FullPath = "";
        preprocessed.PatientOrientation = orientation;

        // create a cad series
        Series cad = CreateSeries(elements, CADText, "VC", orientation + " " + CADText + " Series", study, true);
        //cad.FullPath = "";
        cad.PatientOrientation = orientation;


        int index = 0;

        AddDicomFiles(path, filter, dicom, args);

        bool addDicomFiles = dicom.Files.Count == 0;

        /////////////////////////////////////////////////////////////////////////
        // Loop through all the files sorting them into their appropriate series 
        /////////////////////////////////////////////////////////////////////////
        foreach( string file in files )
        {

          // Check the cancel flag
          if (args.Cancel)
          {
            throw new FileStructureFilterException();
          } // END ... If we are being canceled

          // Filename
          string filename = Path.GetFileName( file );

          // Extension
          string extension = Path.GetExtension(file);

          string destPath = string.Empty;
        
          // extension matches Volume series (not high quality ecv's)
          if ( FileExtensions.Equals(extension, FileExtensions.Vol) || extension.ToLower() == ".log.txt" || 
               FileExtensions.Equals(extension, FileExtensions.Png) || 
               ( FileExtensions.Equals(extension, FileExtensions.Ecv) && !Path.GetFileNameWithoutExtension(file).EndsWith("_hq") ) )
          {
            filename = volume.UID.ToString("_") + extension;

            volume.Files.Add( filename );
            destPath = volume.FullPath;
          }

          // Dicom Series
          else if ( addDicomFiles && FileExtensions.Equals(extension, FileExtensions.Dicom))
          {
            dicom.Files.Add(filename);
            destPath = Path.Combine(dicom.FullPath,"dicom");
          }

          // extension matches CAD series
          else if (extension == ".cad.xml" || extension == ".cad.log.txt")
          {
            cad.Files.Add(filename);
            destPath = cad.FullPath;
          }
          else if ( extension == ".vrx" )
          {
            //added a fix to keep the vrx filename same as supine vol file name. BACKWARD COMPATIBLITY Issue
            string uid = volume.UID.ToString("_");
            if ( uid.Length > 2 )
              uid = uid.Substring(0, uid.Length - 2);

            filename = uid + extension;
            session.Files.Add(filename);
            destPath = session.FullPath;
          }
            // extension matches Session series
          else if (extension == ".amk" || extension == ".mrk" || extension == ".mkvl" || extension == ".vcsx" || extension == ".vcsn" ||
                   FileExtensions.Equals(extension, FileExtensions.VCReport) ||
                   FileExtensions.Equals(extension, FileExtensions.Avi) || FileExtensions.Equals(extension, FileExtensions.Wmv))
          {
            filename = volume.UID.ToString("_") + extension;
            session.Files.Add(filename);
            destPath = session.FullPath;
          }

            // all other files are considered preprocessed
          else
          {
            preprocessed.Files.Add(filename);
            destPath = preprocessed.FullPath;
          }

          if ( destPath.Length > 0 )
            File.Copy( file, Path.Combine(destPath, filename));

          destPath = string.Empty;

          OnProgress( (int)( 30 * (( ++index * 1.0d ) / files.Length )) + progress, 100 );

        } // foreach( string file in Directory.GetFiles( path, "*" + filter + "*" ))


        ///////////////////////////////////////////////////////////////
        // Mark the dicom series deleted if we have no dicom files
        ///////////////////////////////////////////////////////////////
        if ( dicom.Files.Count == 0 )
        {
          dicom.State = "Deleted";
        } // END ... If there are no dicom files

        dicom.Serialize(dicom.FullPath);

        ///////////////////////////////////////////////////////////////
        // If the volume series contains no files then delete it
        ///////////////////////////////////////////////////////////////
        if (volume.Files.Count > 0)
        {
          //volume.SubType = (containsVolumeFile ? "VC" : string.Empty);
          collection.Add(volume);
          dicom.DependentSeries.Add((Series)volume.Clone(false, false));
          volume.ReferencedSeries.Add((Series)dicom.Clone(false, false));

          ///////////////////////////////////////////////////////////////
          // If the preprocessed series contains no files then delete it
          ///////////////////////////////////////////////////////////////
          if (preprocessed.Files.Count > 0)
          {
            collection.Add(preprocessed);

            dicom.DependentSeries.Add((Series)preprocessed.Clone(false, false));
            volume.DependentSeries.Add((Series)preprocessed.Clone(false, false));

            preprocessed.ReferencedSeries.Add((Series)dicom.Clone(false, false));
            preprocessed.ReferencedSeries.Add((Series)volume.Clone(false, false));



            if (cad.Files.Count > 0)
            {
              collection.Add(cad);

              dicom.DependentSeries.Add((Series)cad.Clone(false, false));
              volume.DependentSeries.Add((Series)cad.Clone(false, false));
              preprocessed.DependentSeries.Add((Series)cad.Clone(false, false));

              cad.ReferencedSeries.Add((Series)dicom.Clone(false, false));
              cad.ReferencedSeries.Add((Series)volume.Clone(false, false));
              cad.ReferencedSeries.Add((Series)preprocessed.Clone(false, false));

              cad.Serialize(cad.FullPath);
            }
            else
            {
              study.Series.Remove(cad);
              Directory.Delete(cad.FullPath);

            }

            preprocessed.Serialize(preprocessed.FullPath);

          } // END ... If we have preprocessed series files
          else
          {
            study.Series.Remove(preprocessed);
            Directory.Delete(preprocessed.FullPath);

          }

          volume.Serialize(volume.FullPath);

        } // END ... If we have volume series files
        else
        {
          study.Series.Remove(volume);
          Directory.Delete(volume.FullPath);
        }


      }
      catch ( Exception ex )
      {
        Viatronix.v3D.Logging.Logger.Err("Failed o create series collection from directory : " + ex.Message);
        throw;
      }
      finally
      {
        if ( elements != null ) elements.Dispose();
      }

      return collection;

    } // CreateSeriesCollection( path, filter, orientation, study, session, args )



    /// <summary>
    /// Adds any dicom files matching the orientation.
    /// </summary>
    /// <param name="path">The path containing the files</param>
    /// <param name="filter">The oreintation filter</param>
    /// <param name="dicom">The dicom series.</param>
    /// <param name="args">CancelEventArgs for early termination</param>
    private void AddDicomFiles( string path, string filter, Series dicom, CancelEventArgs args )
    {

      string dicomPath = PathUtilities.CombinePaths(dicom.FullPath, "dicom");

      if (!Directory.Exists(dicomPath))
        Directory.CreateDirectory(dicomPath);

      string[] directories = Directory.GetDirectories( path, "*" + filter + "_DCM" );
      if( directories.Length == 0 )
        return;
      
      dicom.Files.Clear();
      foreach( string file in Directory.GetFiles( directories[0], "*" + Utilities.IO.FileExtensions.Dicom ))
      {
        if( args.Cancel )
          throw new FileStructureFilterException();

        dicom.Files.Add( file );    

        File.Copy(file, PathUtilities.CombinePaths(dicom.FullPath,"dicom",Path.GetFileName(file)));
      }
    } // AddDicomFiles( path, filter, dicom, args )


    /// <summary>
    /// Returns a collection of files assocaited with a session.
    /// </summary>
    /// <param name="path">The path containing the files.</param>
    /// <param name="args">CancelEventArgs for early termination</param>
    /// <returns>a collection of files</returns>
    private ArrayList FindSessionFiles( string path, CancelEventArgs args )
    {
      ArrayList files = new ArrayList();

      foreach( string ext in SessionExtensions )
      {
        if( args.Cancel )
          throw new FileStructureFilterException();

        files.AddRange( Directory.GetFiles( path, "*" + ext ));
      }

      return files;
    } //  FindSessionFiles( path, args )



    #endregion

  } // class VCFileStructureFilter
} // namespace Viatronix.v3D.Console

#region revision history

// $Log: VCFileStructureFilter.cs,v $
// Revision 1.4.2.8  2007/09/13 17:03:17  mkontak
// Make sure the temp directory is deleted if an exception occurs
//
// Revision 1.4.2.7  2007/07/05 22:43:37  mkontak
// Fix the filtering for movies
//
// Revision 1.4.2.6  2007/07/05 14:27:26  mkontak
// Fixed import issue by cloning new object for the references and dependents
// instead of reuing the surrent object. Using the current objects caused a cyclic
// dependency during a series clone.
//
// Revision 1.4.2.5  2007/06/26 21:36:43  gdavidson
// Issue #5702: Do not import data previous to VC 1.2.2
//
// Revision 1.4.2.4  2007/06/25 20:06:24  romy
// due to some backward compatiblity issues, the way we were handling VRX files has been changed. According to the new version, VRX files are also renamed with the Supine series files.
//
// Revision 1.4.2.3  2007/06/25 14:24:27  romy
// Fixed issue #5698
//
// Revision 1.4.2.2  2007/06/22 21:38:40  mkontak
// Fix probelm with importing into the dicom subdirectory
//
// Revision 1.4.2.1  2007/06/19 05:10:28  mkontak
// Issue 5687: problem with volume file names after impoerting
//
// Revision 1.4  2007/03/05 16:17:30  gdavidson
// Issue #5338: Perform all launch operations on the main thread
//
// Revision 1.3  2007/02/28 17:02:04  mkontak
// Check the path for a subtype
//
// Revision 1.2  2007/01/19 20:56:57  mkontak
// Changes for transfer provdier and PACS archival
//
// Revision 1.1  2006/10/30 15:35:04  gdavidson
// Moved from Viatronix.v3D.Console
//
// Revision 1.16  2006/10/10 21:23:52  gdavidson
// Issue #5059: Check the directories for correct prefixes and also generate a new UID for any session series.
//
// Revision 1.15  2006/10/09 21:10:41  gdavidson
// Removed the subtype from the volume series.
//
// Revision 1.14  2006/07/18 14:52:27  mkontak
// Fix rights and transferring into the system
//
// Revision 1.13  2006/02/07 20:48:32  mkontak
// Dispose element lists
//
// Revision 1.12  2005/11/25 15:50:05  gdavidson
// Removed the modification of a session's date processed
//
// Revision 1.11  2005/11/22 17:49:17  gdavidson
// Modified the FilteStructureFilter's CreateSeries method
//
// Revision 1.10  2005/11/18 19:21:09  gdavidson
// Modified the progress notification events.
//
// Revision 1.9  2005/11/17 23:14:15  gdavidson
// Added a file pattern when searching for dicom
//
// Revision 1.8  2005/11/17 16:20:49  gdavidson
// Added progress events.
//
// Revision 1.7  2005/11/16 22:45:45  gdavidson
// Commented code
//
// Revision 1.6  2005/11/16 13:58:13  gdavidson
// Modified the filter to use volume information if dicom does not exist.
//
// Revision 1.5  2005/11/10 19:46:28  gdavidson
// Modified the creation of the core objects to use information contained within either dicom or vol files.
//
// Revision 1.4  2005/11/08 21:37:38  gdavidson
// Extended the abstract FileStructureFilter class
//
// Revision 1.3  2005/11/07 16:43:31  gdavidson
// Modified the IFileStructureFilter interface.
//
// Revision 1.2  2005/04/19 20:01:27  gdavidson
// Modified the IFileStructureFilter interface.
//
// Revision 1.1  2001/01/02 12:47:16  gdavidson
// Initial Revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Console.Core/import/filters/VCFileStructureFilter.cs,v 1.4.2.8 2007/09/13 17:03:17 mkontak Exp $
// $Id: VCFileStructureFilter.cs,v 1.4.2.8 2007/09/13 17:03:17 mkontak Exp $

#endregion
