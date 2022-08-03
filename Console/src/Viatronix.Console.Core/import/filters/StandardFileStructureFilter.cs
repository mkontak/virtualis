// $Id: StandardFileStructureFilter.cs,v 1.1 2006/10/30 15:35:04 gdavidson Exp $
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

using Viatronix.v3D.UI;
using Viatronix.v3D.Core;

namespace Viatronix.v3D.Console
{
	/// <summary>
	/// Filters a directory using a Patient-Study-Series as a layout.
	/// </summary>
	public class StandardFileStructureFilter : FileStructureFilter
	{

    #region fields
    
    /// <summary>
    /// Default text added to the Patient's TreeNode to denote this filter.
    /// </summary>
    private const string DisplayText = "(Standard File Structure)";

    #endregion

    #region methods

    /// <summary>
    /// Adds any files found in the directory to the provided series.
    /// </summary>
    /// <param name="series"></param>
    /// <param name="directory"></param>
    /// <param name="args"></param>
    private void AddFiles( Series series, DirectoryInfo directory, CancelEventArgs args )
    {
      // cancel the operation
      if( args.Cancel )
        throw new FileStructureFilterException();

      // search each sub directory for files
      foreach( DirectoryInfo subDirectory in directory.GetDirectories() )
        AddFiles( series, subDirectory, args );

      foreach( FileInfo file in directory.GetFiles() )
      {
        // do not include Patient.xml, Study.xml or Series.xml
        if( FileSystemConstants.IsFileSystemFile( file ))
          continue;

        series.Files.Add( file.FullName.Replace( series.FullPath, string.Empty ).TrimStart( Path.DirectorySeparatorChar ));
      }
    } // AddFiles( series, directory, args )


    /// <summary>
    /// Links the parent series with its referenced and dependent series.
    /// </summary>
    /// <param name="table">A mapping of series objects and their xml describing their referenced and dependent series.</param>
    /// <param name="collection">All created series objects.</param>
    private void LinkReferences( Hashtable table, SeriesCollection collection )
    {
      foreach( DictionaryEntry entry in table )
      {
        Series series = (Series) entry.Key;
        XmlNode node = (XmlNode) entry.Value;

        // loop through each depenedent series xml and assign the child series to its parent.
        foreach( XmlNode dependentNode in node.SelectNodes( "DEPENDENT_SERIES/series" ))
        { 
          string uid = dependentNode.Attributes[ "UID" ].Value;
          Series dependentSeries = collection[ uid ];
          if( dependentSeries != null )
            series.DependentSeries.Add( dependentSeries );
        }

        // loop through each referenced series xml and assign the child series to its parent.
        foreach( XmlNode refNode in node.SelectNodes( "REFERENCED_SERIES/series" ))
        { 
          string uid = refNode.Attributes[ "UID" ].Value;
          Series refSeries = collection[ uid ];
          if( refSeries != null )
            series.ReferencedSeries.Add( refSeries );
        }
      }
    } // LinkReferences( table, collection )

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
      get { return false; }
    } // RequiresDeletion


    /// <summary>
    /// Determines if the specified path matches a StandardFileStructureFilter.
    /// </summary>
    /// <param name="path">The file system path.</param>
    /// <returns>true if the path is a StandardFileStructureFilter; otherwise false</returns>
    public override bool CanProcess( string path )
    {
      DirectoryInfo patientDir = new DirectoryInfo( path );
      if( !patientDir.Exists )
        return false;

      // check for a patient.xml file
      if( patientDir.GetFiles( FileSystemConstants.PatientFileName ).Length != 1 )
        return false;

      bool hasStudies = false;
      bool hasSeries = false;

      // check the patient directory for subdirectories
      foreach( DirectoryInfo studyDir in patientDir.GetDirectories())
      {
        hasStudies = true;

        // check for study.xml files
        if( studyDir.GetFiles( FileSystemConstants.StudyFileName ).Length != 1 )
          return false;

        // check the study directory for subdirectories
        foreach( DirectoryInfo seriesDir in studyDir.GetDirectories())
        {
          hasSeries = true;

          if( seriesDir.GetFiles( FileSystemConstants.SeriesFileName ).Length != 1 )
            return false;
        }
      }

      return ( hasStudies & hasSeries );
    } // CanProcess( patientDir )


    /// <summary>
    /// Creates a TreeNode structure that represents the data contained in the specifeid path.
    /// </summary>
    /// <param name="path">The file system path.</param>
    /// <returns>A TreeNode structure that represents the data contained in the specifeid path.</returns>
    public override Patient Process( string path, CancelEventArgs args )
    {
      Hashtable cache = new Hashtable();
      SeriesCollection seriesCollection = new SeriesCollection();

      DirectoryInfo patientDir = new DirectoryInfo( path );

      // find the Patient.Xml file
      FileInfo[] patientFiles = patientDir.GetFiles( FileSystemConstants.PatientFileName );

      if( args.Cancel )
        throw new FileStructureFilterException();

      OnProgress( 5, 100 );

      XmlDocument doc = new XmlDocument();
      using( FileStream stream = patientFiles[0].OpenRead() )
        doc.Load( stream );

      // create a Patient object from the xml
      Patient patient = Patient.FromXml( doc.DocumentElement );

      OnProgress( 25, 100 );

      // parse the study directories
      foreach( DirectoryInfo studyDir in patientDir.GetDirectories())
      {
        if( args.Cancel )
          throw new FileStructureFilterException();

        // find the Study.Xml file
        FileInfo[] studyFiles = studyDir.GetFiles( FileSystemConstants.StudyFileName );
        using( FileStream stream = studyFiles[0].OpenRead() )
          doc.Load( stream );

        // create a Study object from the xml
        Study study = Study.FromXml( doc.DocumentElement );
        patient.Studies.Add( study );
        study.Patient = patient;

        OnProgress( 50, 100 );

        // parse the study directories
        foreach( DirectoryInfo seriesDir in studyDir.GetDirectories())
        {
          if( args.Cancel )
            throw new FileStructureFilterException();

          // find the Series.Xml file
          FileInfo[] seriesFiles = seriesDir.GetFiles( FileSystemConstants.SeriesFileName );
          using( FileStream stream = seriesFiles[0].OpenRead() )
            doc.Load( stream );

          // create a Series object from the xml
          Series series = Series.FromXml( doc.DocumentElement );
          series.FullPath = seriesDir.FullName;
          study.Series.Add( series );
          series.Study = study;

          // hold onto the series xml, to later build the referenced and dependent series
          cache.Add( series, doc.DocumentElement );
          seriesCollection.Add( series );

          AddFiles( series, seriesDir, args );
        }
      }

      OnProgress( 80, 100 );

      LinkReferences( cache, seriesCollection );

      OnProgress( 100, 100 );

      return patient;
    } // Process( patientDir )


    #endregion

  } // class StandardFileStructureFilter
} // namespace Viatronix.v3D.Console

#region revision history

// $Log: StandardFileStructureFilter.cs,v $
// Revision 1.1  2006/10/30 15:35:04  gdavidson
// Moved from Viatronix.v3D.Console
//
// Revision 1.8  2005/11/18 19:21:23  gdavidson
// Modified the progress notification events.
//
// Revision 1.7  2005/11/16 22:46:17  gdavidson
// Commented code
//
// Revision 1.6  2005/11/08 21:37:38  gdavidson
// Extended the abstract FileStructureFilter class
//
// Revision 1.5  2005/11/07 16:43:31  gdavidson
// Modified the IFileStructureFilter interface.
//
// Revision 1.4  2005/09/23 18:25:38  romy
// added comment
//
// Revision 1.3  2005/04/19 20:01:14  gdavidson
// Modified the IFileStructureFilter interface.
//
// Revision 1.2  2005/03/29 01:07:11  gdavidson
// Added an AddFile method.
//
// Revision 1.1  2001/01/02 12:47:16  gdavidson
// Initial Revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Console.Core/import/filters/StandardFileStructureFilter.cs,v 1.1 2006/10/30 15:35:04 gdavidson Exp $
// $Id: StandardFileStructureFilter.cs,v 1.1 2006/10/30 15:35:04 gdavidson Exp $

#endregion