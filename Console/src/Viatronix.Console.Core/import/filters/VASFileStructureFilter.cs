// $Id: VASFileStructureFilter.cs,v 1.2 2007/02/28 17:02:04 mkontak Exp $
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
using Viatronix.v3D.Utilities;
using Viatronix.v3D.Dicom;

namespace Viatronix.v3D.Console
{
	/// <summary>
	/// Filters a directory using a VC file layout.
	/// </summary>
	public class VASFileStructureFilter : XPFileStructureFilter
	{

    #region override methods
    
    /// <summary>
    /// Determines if the specified path matches a VCFileStructureFilter.
    /// </summary>
    /// <param name="path">The file system path.</param>
    /// <returns>true if the path is a VCFileStructureFilter; otherwise false</returns>
    public override bool CanProcess( string path )
    {
      if( !Directory.Exists( path ))
        return false;

      string[] files = Directory.GetFiles( path, "*" + Utilities.IO.FileExtensions.Volume );
      if( files.Length == 0 )
        return false;


      string subType = GetSubType(path);

      if (subType == "vas")
        return true;

      DirectoryInfo directory = new DirectoryInfo( path );
      foreach( DirectoryInfo subDir in directory.GetDirectories() )
      {
        string name = subDir.Name.ToLower();
        if( name == "vas" )
          return true;
      }

      return false;
    } // CanProcess( path )


    /// <summary>
    /// Determines the plugin (subtype).
    /// </summary>
    /// <param name="path">The path.</param>
    /// <returns>The subtype</returns>
    protected override string DetermineSubType( string path )
    {
      return "VAS";
    } // DetermineSubType( path )


    /// <summary>
    /// Provides a method for plugins to create additional series.
    /// </summary>
    /// <param name="elements">The ElementList</param>
    /// <param name="path">Path to the plugin directory</param>
    /// <param name="dicom">The dicom series</param>
    /// <param name="volume">The volume series</param>
    /// <param name="session">The session series</param>
    /// <param name="study">The study</param>
    protected override void CreatePluginSeries( ElementList elements, string path, Series dicom, Series volume, Series session, Study study, CancelEventArgs args )
    {
      // create a preprocessed series
      Series preproc = CreateSeries( elements, FileStructureFilter.PreprocessedType, "VAS", study, true );
      AddLinks( dicom, preproc );
      AddLinks( volume, preproc );

      string datasetDir = string.Empty;
      if( session != null )
      {
        AddLinks( preproc, session );

        datasetDir = Path.Combine( session.FullPath, volume.UID.ToString() );
        Directory.CreateDirectory( datasetDir );

        // locate the old session's xml file
        string[] files = Directory.GetFiles( path, "*session.xml" );
        if( files.Length > 0 )
        {
          XmlDocument doc = new XmlDocument();
          doc.Load( files[0] );

          // create seperate Components, Annotations and Path xml files from the information in the old *session.xml file
          XmlNode node = doc.DocumentElement.SelectSingleNode( "//VascularPlugin" );
          node.Attributes[ "version" ].Value = "4.0";
          WriteFile( doc.DocumentElement, Path.Combine( datasetDir, "isotropicComponents.xml" ), "//VascularPlugin", session );
        }
      }

      preproc.Serialize( preproc.FullPath );

      // locate the preprocessed files
      foreach( string file in Directory.GetFiles( path ))
      {
        if( args.Cancel )
          throw new FileStructureFilterException();

        // exclude the label.vol and any xml
        if( file.EndsWith( "-label.vol" ) || Path.GetExtension( file ) == Utilities.IO.FileExtensions.Xml )
          continue;

        // create isotropicLabel vol
        if( session != null && file.EndsWith( ".lb.vol" ))
        {
          string filename = "isotropicLabel.vol";
          File.Copy( file, Path.Combine( datasetDir, filename ), true );
          session.Files.Add( volume.UID.ToString() + Path.DirectorySeparatorChar + filename );
        }
        else if( session != null && file.EndsWith( ".va.png" ))
        {
          string filename = "vesselArray.png";
          File.Copy( file, Path.Combine( datasetDir, filename ), true );
          session.Files.Add( volume.UID.ToString() + Path.DirectorySeparatorChar + filename );
        }
        else
        {
          string originalName = Path.GetFileName( file );
          string filename = preproc.UID.ToString( "_" ) + originalName.Substring( originalName.Length - 7, 7 );
          File.Copy( file, Path.Combine( preproc.FullPath, filename ), true );
          preproc.Files.Add( filename );
        }
      }
    } // CreatePluginSeries( elements, path, dicom, volume, session, study )


    #endregion

  } // class VASFileStructureFilter
} // namespace Viatronix.v3D.Console


#region revision history

// $Log: VASFileStructureFilter.cs,v $
// Revision 1.2  2007/02/28 17:02:04  mkontak
// Check the path for a subtype
//
// Revision 1.1  2006/10/30 15:35:04  gdavidson
// Moved from Viatronix.v3D.Console
//
// Revision 1.9  2005/11/24 03:56:35  gdavidson
// Added vesselArray and isotropicArray to the session series
//
// Revision 1.8  2005/11/24 02:39:49  gdavidson
// Added code to create a vesselArray.png
//
// Revision 1.7  2005/11/22 17:49:17  gdavidson
// Modified the FilteStructureFilter's CreateSeries method
//
// Revision 1.6  2005/11/22 03:23:20  gdavidson
// Added a check for a session series before adding data to the session file.
//
// Revision 1.5  2005/11/21 20:11:07  gdavidson
// Modified the session's version number
//
// Revision 1.4  2005/11/18 21:47:21  gdavidson
// Added code to create the dataset subdirectory under the session.
//
// Revision 1.3  2005/11/18 21:09:42  gdavidson
// Modified the preprocessed file names.
//
// Revision 1.2  2005/11/16 22:45:45  gdavidson
// Commented code
//
// Revision 1.1  2005/11/08 21:37:08  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Console.Core/import/filters/VASFileStructureFilter.cs,v 1.2 2007/02/28 17:02:04 mkontak Exp $
// $Id: VASFileStructureFilter.cs,v 1.2 2007/02/28 17:02:04 mkontak Exp $

#endregion
