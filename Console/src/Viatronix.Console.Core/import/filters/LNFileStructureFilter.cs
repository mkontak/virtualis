// $Id: LNFileStructureFilter.cs,v 1.3.2.2 2007/09/13 17:03:17 mkontak Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )
// Co-Author: Romy ( mailto:romy@viatronix.com )

using System;
using System.ComponentModel;
using System.Collections;
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
	/// Filters a directory using a LN file layout.
	/// </summary>
	public class LNFileStructureFilter : FileStructureFilter
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
    public override bool CanProcess( string path )
    {
      if( !Directory.Exists( path ))
        return false;

      string[] files = Directory.GetFiles( path, "*" + Utilities.IO.FileExtensions.Volume );
      if( files.Length == 0 )
        return false;

      string subType = GetSubType(path);

      if (subType == "ln")
        return true;

      DirectoryInfo directory = new DirectoryInfo( path );
      DirectoryInfo[] directories = directory.GetDirectories();
      if( directories.Length == 0 )
        return true;

      foreach( DirectoryInfo subDir in directories )
      {
        string name = subDir.Name.ToLower();
        if( name == "lungcad" )
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
        patientDirectory = Path.Combine(Utilities.Registry.SystemSettings.TempDirectory, patient.Location);
        DirectoryInfo patientDirInfo = Directory.CreateDirectory(patientDirectory);
        patient.Serialize(patientDirInfo.FullName);

        OnProgress(15, 100);

        // create study and temp directory
        Study study = CreateStudy(elements, patient);
        DirectoryInfo studyDirInfo = Directory.CreateDirectory(Path.Combine(Utilities.Registry.SystemSettings.TempDirectory, study.Location));
        study.Serialize(studyDirInfo.FullName);

        OnProgress(20, 100);

        if (args.Cancel)
          throw new FileStructureFilterException();

        // create Dicom series
        Series dicom = CreateSeries(elements, FileStructureFilter.DicomType, study, false);
        if (elements != null)
        {
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

        foreach (string file in Directory.GetFiles(path))
        {
          if (Path.GetExtension(file) == Utilities.IO.FileExtensions.Dicom)
            continue;

          string filename = Path.GetFileName(file);
          File.Copy(file, Path.Combine(volume.FullPath, filename), true);
          volume.Files.Add(filename);
        }

        OnProgress(35, 100);

        // determine the plugin (subtype) that created the structure
        string subtype = DetermineSubType(path);
        if (subtype != string.Empty)
        {
          // Create the Plugin Series.
          string subDir = System.IO.Path.Combine(path, "LungCAD");  //Directory.GetDirectories( path, subtype + "*" )[0];
          if (Utilities.IO.PathUtilities.GetFileCount(subDir, false) > 0)
          {
            // creates additional series for different plugins
            CreatePluginSeries(elements, subDir, dicom, volume, null, study, args);

            if (args.Cancel)
              throw new FileStructureFilterException();
          }
        }

        volume.Serialize(volume.FullPath);

        if (dicom != null)
          dicom.Serialize(dicom.FullPath);

        OnProgress(100, 100);
      }
      catch (System.Exception ex)
      {
        Logging.Logger.Err("Failed to filter study : " + ex.Message);

        if ( patientDirectory.Length > 0 && Directory.Exists(patientDirectory) )
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
      // create a preprocessed series
      Series preproc = CreateSeries( elements, FileStructureFilter.CADType, "LN", study, true );
      AddLinks( dicom, preproc );
      AddLinks( volume, preproc );
      //AddLinks( preproc, session );

      preproc.Serialize( preproc.FullPath );

      // locate the preprocessed files
      foreach( string file in Directory.GetFiles( path ))
      {
        if( args.Cancel )
          throw new FileStructureFilterException();

        string filename = Path.GetFileName( file );
        File.Copy( file, Path.Combine( preproc.FullPath, filename ), true );
        preproc.Files.Add( filename );
      }
    } // CreatePluginSeries( elements, path, dicom, volume, session, study )



    /// <summary>
    /// Assigns the referenced and dependent links.
    /// </summary>
    /// <param name="parent">Patent series</param>
    /// <param name="child">Child series</param>
    protected void AddLinks( Series parent, Series child )
    {
      if( parent != null && child != null )
      {
        parent.DependentSeries.Add((Series)child.Clone(false, false));
        child.ReferencedSeries.Add((Series)parent.Clone(false, false));
      }
    } // AddLinks( parent, child )
    

    /// <summary>
    /// Determines the plugin (subtype).
    /// </summary>
    /// <param name="path">The path.</param>
    /// <returns>The subtype</returns>
    protected virtual string DetermineSubType( string path )
    {
          return "LN";
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
    private void GenerateSessionFiles( string path, Series session )
    {
      // locate the volume sereis
      Series volume = session.ReferencedSeries.FindFirst( "Volume" );

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
      writer.WriteAttributeString( "pluginID", session.SubType );
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

    #endregion

  } // class XPFileStructureFilter
} // namespace Viatronix.v3D.Console


#region revision history

// $Log: LNFileStructureFilter.cs,v $
// Revision 1.3.2.2  2007/09/13 17:03:17  mkontak
// Make sure the temp directory is deleted if an exception occurs
//
// Revision 1.3.2.1  2007/07/05 14:27:26  mkontak
// Fixed import issue by cloning new object for the references and dependents
// instead of reuing the surrent object. Using the current objects caused a cyclic
// dependency during a series clone.
//
// Revision 1.3  2007/03/13 12:18:09  mkontak
// coding standards
//
// Revision 1.2  2007/02/28 17:02:04  mkontak
// Check the path for a subtype
//
// Revision 1.1  2006/10/30 15:35:04  gdavidson
// Moved from Viatronix.v3D.Console
//
// Revision 1.4  2006/07/18 14:52:35  mkontak
// Fix rights and transferring into the system
//
// Revision 1.3  2005/11/22 17:49:17  gdavidson
// Modified the FilteStructureFilter's CreateSeries method
//
// Revision 1.2  2005/11/20 04:41:36  romy
// changed preprocessed series to CAD
//
// Revision 1.1  2005/11/18 20:55:46  romy
// initial version
//

// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Console.Core/import/filters/LNFileStructureFilter.cs,v 1.3.2.2 2007/09/13 17:03:17 mkontak Exp $
// $Id: LNFileStructureFilter.cs,v 1.3.2.2 2007/09/13 17:03:17 mkontak Exp $

#endregion
