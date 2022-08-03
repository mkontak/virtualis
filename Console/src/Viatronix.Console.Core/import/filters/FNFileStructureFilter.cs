// $Id: FNFileStructureFilter.cs,v 1.2 2007/02/28 17:02:04 mkontak Exp $
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
	/// Filters a directory using a FN file layout.
	/// </summary>
	public class FNFileStructureFilter : XPFileStructureFilter
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
      if( files.Length < 2 )
        return false;

      string subType = GetSubType(path);

      if (subType == "fn")
        return true;

 
      DirectoryInfo directory = new DirectoryInfo( path );
      foreach( DirectoryInfo subDir in directory.GetDirectories() )
      {
        string name = subDir.Name.ToLower();
        if( name == "fn" )
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
      return "FN";
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
      FileInfo[] files = Directory.GetParent( path ).GetFiles( "*_PET.vol" );
      if( files.Length == 0 )
      {
        Viatronix.v3D.Logging.Logger.Err( "Unable to locate PET data. [PATH=" + path + "]" );
        throw new Exception( "Unable to locate PET data." );
      }

      if( dicom.Description == FileStructureFilter.DicomType )
        dicom.Description = dicom.Modality.Trim() + " Dicom";

      if( volume.Description == FileStructureFilter.VolumeType )
        volume.Description = volume.Modality.Trim() + " Volume";

      ElementList petElements = CreateElementListFromFile( files[0].FullName );
      
      // create a preprocessed series
      Series petDicom = CreateSeries( petElements, FileStructureFilter.DicomType, string.Empty, study, true );
      petDicom.Modality = "PT";
      petDicom.Description = "PET Dicom";

      petDicom.State = "Deleted";
      AddLinks( petDicom, session );

      petDicom.Serialize( petDicom.FullPath );

      // create a preprocessed series
      Series petVolume = CreateSeries( petElements, FileStructureFilter.VolumeType, string.Empty, study, true );
      petVolume.Modality = "PT";
      petVolume.PreprocessAs = "FN";
      petVolume.Description = "PET Volume";

      AddLinks( petDicom, petVolume );
      AddLinks( petVolume, session );
      
      petVolume.Serialize( petVolume.FullPath );

      // copy pet vol to series
      string petFile = Path.GetFileName( files[0].Name ).Replace( "_PET", string.Empty );
      File.Copy( files[0].FullName, Path.Combine( petVolume.FullPath, petFile), true );
      petVolume.Files.Add( petFile );

      // remove pet vol from ct volume series
      volume.Files.Remove( Path.GetFileName( files[0].Name ));
      File.Delete( Path.Combine( volume.FullPath, files[0].Name ));
    } // CreatePluginSeries( elements, path, dicom, volume, session, study, args )

    #endregion

  } // class FNFileStructureFilter
} // namespace Viatronix.v3D.Console


#region revision history

// $Log: FNFileStructureFilter.cs,v $
// Revision 1.2  2007/02/28 17:02:04  mkontak
// Check the path for a subtype
//
// Revision 1.1  2006/10/30 15:35:04  gdavidson
// Moved from Viatronix.v3D.Console
//
// Revision 1.6  2005/12/30 17:17:57  mkontak
// New DB changes
//
// Revision 1.5  2005/11/24 00:06:41  gdavidson
// Serialized out the pet dicom
//
// Revision 1.4  2005/11/22 17:49:17  gdavidson
// Modified the FilteStructureFilter's CreateSeries method
//
// Revision 1.3  2005/11/19 04:34:24  gdavidson
// Reworked the filter to include session information
//
// Revision 1.2  2005/11/18 21:30:34  gdavidson
// Modified the filter to ignore session.
//
// Revision 1.1  2005/11/18 21:08:36  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Console.Core/import/filters/FNFileStructureFilter.cs,v 1.2 2007/02/28 17:02:04 mkontak Exp $
// $Id: FNFileStructureFilter.cs,v 1.2 2007/02/28 17:02:04 mkontak Exp $

#endregion
