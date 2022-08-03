// $Id: CardiacFileStructureFilter.cs,v 1.2.2.1 2007/09/13 17:03:17 mkontak Exp $
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
	/// Filters a directory using a Cardiac file layout.
	/// </summary>
	public class CardiacFileStructureFilter : FileStructureFilter
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
    /// Determines if the specified path matches a VCFileStructureFilter.
    /// </summary>
    /// <param name="path">The file system path.</param>
    /// <returns>true if the path is a VCFileStructureFilter; otherwise false</returns>
    public override bool CanProcess( string path )
    {
      if( !Directory.Exists( path ))
        return false;

      string[] files = Directory.GetFiles( path, "*" + Utilities.IO.FileExtensions.Volume );
      return ( files.Length > 1 );
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

        patient = CreatePatient(elements);
        if (patient.Name.Empty)
          patient.Name = new Viatronix.v3D.Core.PersonName("Cardiac Patient");

        patientDirectory = Path.Combine(Utilities.Registry.SystemSettings.TempDirectory, patient.Location);
        DirectoryInfo patientDir = Directory.CreateDirectory(patientDirectory);
        patient.Serialize(patientDir.FullName);

        OnProgress(10, 100);

        Study study = CreateStudy(elements, patient);
        DirectoryInfo studyDir = Directory.CreateDirectory(Path.Combine(Utilities.Registry.SystemSettings.TempDirectory, study.Location));
        study.Serialize(studyDir.FullName);

        OnProgress(15, 100);

        int index = 1;
        string[] files = Directory.GetFiles(path, "*" + Utilities.IO.FileExtensions.Volume);

        // create a volume series for each volume file
        foreach (string file in files)
        {
          elements = CreateElementListFromFile(file);

          // create the volume series
          Series volume = CreateSeries(elements, FileStructureFilter.VolumeType, string.Empty, "Cardiac " + index.ToString(), study, true);
          volume.PreprocessAs = "CAR";

          // create a series directory
          DirectoryInfo seriesDir = Directory.CreateDirectory(Path.Combine(Utilities.Registry.SystemSettings.TempDirectory, volume.Location));
          volume.FullPath = seriesDir.FullName;
          volume.Serialize(seriesDir.FullName);

          File.Copy(file, Path.Combine(volume.FullPath, Path.GetFileName(file)), true);
          volume.Files.Add(file.Replace(path, string.Empty).TrimStart(Path.DirectorySeparatorChar));

          OnProgress(index, files.Length);
          index++;

          OnProgress((int)(85 * ((index * 1.0d) / files.Length)) + 10, 100);
        }

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

  } // class CardiacFileStructureFilter
} // namespace Viatronix.v3D.Console


#region revision history

// $Log: CardiacFileStructureFilter.cs,v $
// Revision 1.2.2.1  2007/09/13 17:03:17  mkontak
// Make sure the temp directory is deleted if an exception occurs
//
// Revision 1.2  2007/03/12 19:38:35  mkontak
// coding standards
//
// Revision 1.1  2006/10/30 15:35:03  gdavidson
// Moved from Viatronix.v3D.Console
//
// Revision 1.9  2006/08/05 18:57:02  gdavidson
// Changed process as
//
// Revision 1.8  2006/07/18 14:52:35  mkontak
// Fix rights and transferring into the system
//
// Revision 1.7  2005/12/30 17:17:57  mkontak
// New DB changes
//
// Revision 1.6  2005/12/02 15:29:53  gdavidson
// Commented code
//
// Revision 1.5  2005/11/22 17:49:17  gdavidson
// Modified the FilteStructureFilter's CreateSeries method
//
// Revision 1.4  2005/11/19 04:32:30  gdavidson
// Added serialization of series xml
//
// Revision 1.3  2005/11/18 19:16:04  gdavidson
// Modified the progress notification events.
//
// Revision 1.2  2005/11/16 22:45:45  gdavidson
// Commented code
//
// Revision 1.1  2005/11/16 14:04:27  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Console.Core/import/filters/CardiacFileStructureFilter.cs,v 1.2.2.1 2007/09/13 17:03:17 mkontak Exp $
// $Id: CardiacFileStructureFilter.cs,v 1.2.2.1 2007/09/13 17:03:17 mkontak Exp $

#endregion
