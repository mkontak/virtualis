// $Id: XPFileStructureFilter.cs,v 1.3.2.2 2007/09/13 17:03:17 mkontak Exp $
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
using System.Collections.Specialized;
using System.IO;
using System.Windows.Forms;
using System.Xml;

using Viatronix.v3D.UI;
using Viatronix.v3D.Core;
using Viatronix.v3D.Utilities;
using Viatronix.v3D.Dicom;

namespace Viatronix.v3D.Console
{
	/// <summary>
	/// Filters a directory using a VC file layout.
	/// </summary>
	public class XPFileStructureFilter : FileStructureFilter
	{

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
    /// Determines if the specified path matches a XPFileStructureFilter.
    /// </summary>
    /// <param name="path">The file system path.</param>
    /// <returns>true if the path is a XPFileStructureFilter; otherwise false</returns>
    public override bool CanProcess(string path)
    {


      if( !Directory.Exists( path ))
        return false;

      bool containsVolumeFile = ( Directory.GetFiles( path, "*" + Utilities.IO.FileExtensions.Volume ).Length > 0 );
      if (!containsVolumeFile)
        return false;


      string[] list = { "xp", "angio", "ang", "cs" };
      StringCollection tags = new StringCollection();
      tags.AddRange(list);

      string subType = GetSubType(path);

      if (tags.Contains(subType))
        return true;


      // Check for a subdirectory
      DirectoryInfo directory = new DirectoryInfo(path);
      DirectoryInfo[] directories = directory.GetDirectories();

      if (directories.Length == 0)
        return true;
    

      foreach( DirectoryInfo subDir in directories )
      {
        if ( tags.Contains(subDir.Name.ToLower()) )
          return true;
      }

      return false;
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
        OnProgress(5, 100);

        elements = CreateElementList(path);

        // create patient object and temp directory
        patient = CreatePatient(elements);
        DirectoryInfo patientDir = Directory.CreateDirectory(Path.Combine(Utilities.Registry.SystemSettings.TempDirectory, patient.Location));
        patient.Serialize(patientDir.FullName);

        OnProgress(15, 100);

        // create study and temp directory
        Study study = CreateStudy(elements, patient);
        DirectoryInfo studyDir = Directory.CreateDirectory(Path.Combine(Utilities.Registry.SystemSettings.TempDirectory, study.Location));
        study.Serialize(studyDir.FullName);

        OnProgress(20, 100);

        if (args.Cancel)
          throw new FileStructureFilterException();

        // create Dicom series
        Series dicom = CreateSeries(elements, FileStructureFilter.DicomType, study, false);

        if (elements != null)
        {
          // add dicom files to dicom series
          foreach (string file in Directory.GetFiles(path, "*" + Utilities.IO.FileExtensions.Dicom))
          {
            string filename = Path.GetFileName(file);
            File.Copy(file, Path.Combine(dicom.FullPath, filename), true);
            dicom.Files.Add(filename);
          }
        }

        if (dicom.Files.Count == 0)
          dicom.State = "Deleted";

        // create Volume series
        Series volume = CreateSeries(elements, FileStructureFilter.VolumeType, study, true);

        AddLinks(dicom, volume);

        if (args.Cancel)
          throw new FileStructureFilterException();

        // add all files at the top level to the volume series (excluding dicom)
        bool previewImageFound = false;
        foreach (string file in Directory.GetFiles(path))
        {
          if (Path.GetExtension(file) == Utilities.IO.FileExtensions.Dicom)
            continue;

          string filename = string.Empty;

          string extension = Path.GetExtension(file);

          previewImageFound = previewImageFound || Utilities.IO.FileExtensions.Equals(extension, Utilities.IO.FileExtensions.Png);

          //////////////////////////////////////////////////////////////////////
          // create the volume preview image and volume file renaming them
          /////////////////////////////////////////////////////////////////////
          if (Utilities.IO.FileExtensions.Equals(extension, Utilities.IO.FileExtensions.Png) ||
              Utilities.IO.FileExtensions.Equals(extension, Utilities.IO.FileExtensions.Vol))
          {
            filename = volume.UID.ToString("_") + extension;
          }
          else
            filename = Path.GetFileName(file);

          File.Copy(file, Path.Combine(volume.FullPath, filename), true);
          volume.Files.Add(filename);
        }

        OnProgress(35, 100);

        // determine the plugin (subtype) that created the structure
        string subtype = DetermineSubType(path);
        if (subtype != string.Empty)
        {
          volume.PreprocessAs = subtype;

          // read the session directory
          string subDir = Directory.GetDirectories(path, subtype + "*")[0];
          Series session = CreateSession(elements, study, subDir, subtype, args);

          // creates additional series for different plugins
          CreatePluginSeries(elements, subDir, dicom, volume, session, study, args);

          if (session != null)
          {
            AddLinks(volume, session);
            AddLinks(dicom, session);

            // move the session files to the temporary directory
            GenerateSessionFiles(subDir, session, volume);

            session.Serialize(session.FullPath);
          }
        }

        volume.Serialize(volume.FullPath);
        dicom.Serialize(dicom.FullPath);

        OnProgress(100, 100);
      }
      catch (Exception ex)
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
        if (elements != null) elements.Dispose();
      }

      return patient;
    } // Process( path, args )

    #endregion

    #region methods

    /// <summary>
    /// Provides a method for plugins to create additional series.
    /// </summary>
    /// <param name="elements">The ElementList</param>
    /// <param name="path">Path to the plugin directory</param>
    /// <param name="dicom">The dicom series</param>
    /// <param name="volume">The volume series</param>
    /// <param name="session">The session series</param>
    /// <param name="study">The study</param>
    protected virtual void CreatePluginSeries( ElementList elements, string path, Series dicom, Series volume, Series session, Study study, CancelEventArgs args )
    {
    }


    /// <summary>
    /// Assigns the referenced and dependent links.
    /// </summary>
    /// <param name="parent">Patent series</param>
    /// <param name="child">Child series</param>
    protected void AddLinks( Series parent, Series child )
    {
      if( parent != null && child != null )
      {
        parent.DependentSeries.Add( child );
        child.ReferencedSeries.Add( parent );
      }
    } // AddLinks( parent, child )
    

    /// <summary>
    /// Determines the plugin (subtype).
    /// </summary>
    /// <param name="path">The path.</param>
    /// <returns>The subtype</returns>
    protected virtual string DetermineSubType( string path )
    {
      DirectoryInfo directory = new DirectoryInfo( path );
      foreach( DirectoryInfo subDir in directory.GetDirectories() )
      {
        string dirName = subDir.Name.ToUpper();
        if( dirName == "XP" || dirName == "CS" )
          return dirName;
        else if( dirName == "ANGIO" || dirName == "ANG" )
          return "ANG";
      }

      return string.Empty;
    } // DetermineSubType( path )



    /// <summary>
    /// Copies the files associated with the directory name.
    /// </summary>
    /// <param name="path">The path.</param>
    /// <param name="itemName">The session item being copied.</param>
    /// <param name="startProgress">The initial progress.</param>
    /// <param name="endProgress">The final progress.</param>
    /// <param name="series"></param>
    private void CopyFiles( string path, string itemName, int startProgress, int endProgress, Series series, CancelEventArgs args )
    {
      string destination = string.Empty;

      // check the main path for files containing the provided itemName
      string[] files = Directory.GetFiles( path, "*" + itemName + ".xml" );
      if( files.Length > 0 )
      {
        // create a destination subdirectory off the series full path
        destination = Directory.CreateDirectory( Path.Combine( series.FullPath, itemName )).FullName;

        string filename = Path.Combine( destination, itemName + ".xml" );
        File.Copy( files[0], filename, true );
        series.Files.Add( filename.Replace( path, string.Empty ).TrimStart( Path.DirectorySeparatorChar ));
      }
      else
        return;

      // check that sub directory exists
      string directory = Path.Combine( path, itemName );
      if( !Directory.Exists( directory ))
        return;

      // check that files exist in the subdirectory
      files = Directory.GetFiles( directory );
      if( files.Length == 0 )
        return;

      int index = 1;
      int totalProgress = startProgress;
      foreach( string file in files )
      {
        if( args.Cancel )
          throw new FileStructureFilterException();

        // copy the file to the destinatio directory and append it to the series Files collection
        File.Copy( file, Path.Combine( destination, Path.GetFileName( file )), true );
        series.Files.Add( file.Replace( directory, string.Empty ).TrimStart( Path.DirectorySeparatorChar ));

        totalProgress += (int)(( index * 1.0f ) / files.Length ) * ( endProgress - startProgress );
        OnProgress( totalProgress, 100 );
      }
    } // CopyFiles( path, directoryName, startProgress, endProgress, series )


    /// <summary>
    /// Moves the label volume to the dataset directory and creats Components, Annotations and Path xml files from the session.xml
    /// </summary>
    /// <param name="path">Path to the session directory.</param>
    /// <param name="session">The session series.</param>
    private void GenerateSessionFiles( string path, Series session, Series volume )
    {
      // create a subdirectory (dataset directory) under the session series fullpath
      DirectoryInfo datasetDir = Directory.CreateDirectory( Path.Combine( session.FullPath, volume.UID.ToString() ));

      // find the *label.vol, copy it to the dataset directory and append it to the session series Files collection
      foreach( string file in Directory.GetFiles( path, "*label" + Utilities.IO.FileExtensions.Volume ))
      {
        string filename = Path.Combine( datasetDir.FullName, "label.vol" );
        File.Copy( file, filename, true );
        session.Files.Add( filename.Replace( session.FullPath, string.Empty ).TrimStart( Path.DirectorySeparatorChar ));
        break;
      }

      // locate the old session's xml file
      string[] files = Directory.GetFiles( path, "*session.xml" );
      if( files.Length == 0 )
        return;

      XmlDocument doc = new XmlDocument();
      doc.Load( files[0] );

      // create seperate Components, Annotations and Path xml files from the information in the old *session.xml file
      WriteFile( doc.DocumentElement, Path.Combine( datasetDir.FullName, "Components.xml" ), "Components", session );
      WriteFile( doc.DocumentElement, Path.Combine( datasetDir.FullName, "Annotations.xml" ), "Annotations", session );
      WriteFile( doc.DocumentElement, Path.Combine( datasetDir.FullName, "Path.xml" ), "Vessel", session );
      
      // create a new session.xml and save it into the session series's fullpath
      string sessionFile = Path.Combine( session.FullPath, "session.xml" );
      XmlTextWriter writer = new XmlTextWriter( sessionFile, System.Text.Encoding.UTF8 );
      writer.WriteStartDocument();
      writer.WriteStartElement( "session" );
      writer.WriteAttributeString( "version", "2.1" );
      writer.WriteAttributeString( "pluginID", session.SubType.ToLower() );
      writer.WriteStartElement( "datasets" );
      writer.WriteStartElement( "seriesGroup" );
      
      foreach( Series series in session.ReferencedSeries )
      {
        writer.WriteStartElement( "series" );
        writer.WriteAttributeString( "uid", series.UID.ToString() );
        writer.WriteAttributeString( "path", string.Empty );
        writer.WriteAttributeString( "type", series.Type );
        writer.WriteAttributeString( "subtype", series.SubType );
        writer.WriteEndElement();
      }

      writer.WriteEndDocument();
      writer.Flush();
      writer.Close();

      session.Files.Add( sessionFile.Replace( session.FullPath, string.Empty ).TrimStart( Path.DirectorySeparatorChar ));
    } // GenerateSessionFiles( path, session )


    /// <summary>
    /// Takes the information located under the specified node and writesit to a seperate file.
    /// </summary>
    /// <param name="parent">The parent xml node.</param>
    /// <param name="destination">The output file</param>
    /// <param name="xpath">The xpath query.</param>
    /// <param name="session">The session series</param>
    protected void WriteFile( XmlNode parent, string destination, string xpath, Series session )
    {
      // find the internal node using the nodeName as an XPath query
      XmlNode node = parent.SelectSingleNode( xpath );
      if( node != null )
      {
        // write the node out to its own file
        XmlTextWriter writer = new XmlTextWriter( destination, System.Text.Encoding.UTF8 );
        writer.WriteStartDocument();

        XmlNodeReader reader = new XmlNodeReader( node );
        writer.WriteNode( reader, true );
        reader.Close();

        writer.WriteEndDocument();

        writer.Flush();
        writer.Close();

        session.Files.Add( destination.Replace( session.FullPath, string.Empty ).TrimStart( Path.DirectorySeparatorChar ));
      }
    } // WriteFile( parent, destination, nodeName, session )


    /// <summary>
    /// Creates a session series.
    /// </summary>
    /// <param name="elements">ElementList</param>
    /// <param name="study">Parent study</param>
    /// <param name="subDir">Sub directory containing session.xml</param>
    /// <param name="subtype">Subtype for session.</param>
    /// <param name="args">CancelEventArgs</param>
    /// <returns></returns>
    private Series CreateSession( ElementList elements, Study study, string subDir, string subtype, CancelEventArgs args )
    {
      // locate the old session's xml file
      string[] files = Directory.GetFiles( subDir, "*session.xml" );
      if( files.Length == 0 )
        return null;
      
      // read the session directory
      if( Utilities.IO.PathUtilities.GetFileCount( subDir, false ) > 0 )
      {
        // create Session series
        Series session = CreateSeries( elements, FileStructureFilter.SessionType, subtype, study, true );
        session.Modality = "SC";


        // move the movies, snapshots and report to temporary directories
        CopyFiles( subDir, "movies", 35, 70, session, args );
        CopyFiles( subDir, "snapshots", 70, 85, session, args );
        CopyFiles( subDir, "report", 85, 90, session, args );
      
        if( args.Cancel )
          throw new FileStructureFilterException();
      
        return session;
      }

      return null;
    } // CreateSession( elements, study, subDir, subtype, args )

    #endregion

  } // class XPFileStructureFilter
} // namespace Viatronix.v3D.Console


#region revision history

// $Log: XPFileStructureFilter.cs,v $
// Revision 1.3.2.2  2007/09/13 17:03:17  mkontak
// Make sure the temp directory is deleted if an exception occurs
//
// Revision 1.3.2.1  2007/06/19 05:10:22  mkontak
// Issue 5687: problem with volume file names after impoerting
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
// Revision 1.18  2006/07/18 14:52:27  mkontak
// Fix rights and transferring into the system
//
// Revision 1.17  2006/07/13 14:30:25  mkontak
// Change to set the series location upon creation
//
// Revision 1.16  2005/12/30 17:17:57  mkontak
// New DB changes
//
// Revision 1.15  2005/11/24 00:07:09  gdavidson
// Serialized out the dicom xml
//
// Revision 1.14  2005/11/23 01:03:15  gdavidson
// Creates a preview image for the volume series.
//
// Revision 1.13  2005/11/22 17:49:17  gdavidson
// Modified the FilteStructureFilter's CreateSeries method
//
// Revision 1.12  2005/11/22 03:22:45  gdavidson
// Added a check for the old session file before converting it to the new format.
//
// Revision 1.11  2005/11/21 20:09:42  gdavidson
// Output session subtype in lowercase
//
// Revision 1.10  2005/11/21 14:31:06  gdavidson
// Fixed the references stored in the session.xml
//
// Revision 1.9  2005/11/19 04:34:40  gdavidson
// Added a CreateSession method
//
// Revision 1.8  2005/11/18 21:09:24  gdavidson
// Set the series ProcessAs if a session is found
//
// Revision 1.7  2005/11/18 19:20:44  gdavidson
// Modified the WriteFile method
//
// Revision 1.6  2005/11/18 00:59:58  gdavidson
// Fixed subtype for angio.
//
// Revision 1.5  2005/11/17 14:54:02  gdavidson
// Fixed the processing to seriealize the volume and dicom series.xml even if a session does not exist.
//
// Revision 1.4  2005/11/16 22:45:45  gdavidson
// Commented code
//
// Revision 1.3  2005/11/16 14:01:39  gdavidson
// Modified the CreateSeries method to create a new series.
//
// Revision 1.2  2005/11/10 19:46:28  gdavidson
// Modified the creation of the core objects to use information contained within either dicom or vol files.
//
// Revision 1.1  2005/11/08 21:37:08  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Console.Core/import/filters/XPFileStructureFilter.cs,v 1.3.2.2 2007/09/13 17:03:17 mkontak Exp $
// $Id: XPFileStructureFilter.cs,v 1.3.2.2 2007/09/13 17:03:17 mkontak Exp $

#endregion
