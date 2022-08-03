// $Id: FileSystemTransferProvider.cs,v 1.3.2.4 2011/05/18 01:56:59 mkontak Exp $ 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )

using System;
using System.IO;
using System.Windows.Forms;
using System.Collections.Generic;
using Viatronix.UI;
using Viatronix.Logging;
using Viatronix.v3D.Core;
using Viatronix.Utilities;
using Viatronix.Utilities.IO;
using Viatronix.Enterprise.Entities;
using Viatronix.Utilities.Registry;

namespace Viatronix.Console
{
	/// <summary>
	/// Summary description for FileSystemTransferProvider.
	/// </summary>
	public sealed class FileSystemTransferProvider :  ITransferProvider
	{
   
    #region fields

    /// <summary>
    /// Transfer Progress Event Handler object
    /// </summary>
    public event TransferProgressEventHandler Progress = null;

    /// <summary>
    /// Holds what the output directory is in the event of an abort.
    /// </summary>
    string m_transferToDirectory = string.Empty;

    #endregion

    #region methods

    /// <summary>
    /// OnProgress method
    /// </summary>
    /// <param name="args"></param>
    private void OnProgress(TransferProgressEventArgs args)
    {
      if( this.Progress != null )
        this.Progress(this, args);
    }//OnProgress( TransferProgressEventArgs args )

    #endregion

    #region ITransferProvider Members
    
    /// <summary>
    /// Transfers the data to the FileSystem.
    /// </summary>
    /// <param name="args">The arguments to take into account.</param>
    public void Transfer(TransferProviderArgs args)
    {
      //////////////////////////////////////////////////////////////////////////////////////
      // Perform statup functions.
      //////////////////////////////////////////////////////////////////////////////////////

      OnProgress(new TransferProgressEventArgs("Starting Up", 0, 100));

      string transferToDirectory = (string)args.TransferObject;

      Series seriesToTransfer = args.SeriesCollection[0];
      Study studyToTransfer = Global.DataProvider.GetStudy(seriesToTransfer);
 
      Global.DataProvider.GetSeriesPath(seriesToTransfer);

      string sourcePatientLocation = Path.Combine(SystemSettings.CurrentSystemSettings.CacheBasePath, Path.GetDirectoryName(Path.GetDirectoryName(seriesToTransfer.Directory)));
      string destinationPatientLocation = Path.Combine(transferToDirectory, Path.GetDirectoryName(Path.GetDirectoryName(seriesToTransfer.Directory)));

      // Set up to handle an abort.
      m_transferToDirectory = destinationPatientLocation;

      //List<Study> refStudyList = new List<Study>();

      //////////////////////////////////////////////////////////////////////////////////////
      // Check for referenced series outside the current patient scope.
      //////////////////////////////////////////////////////////////////////////////////////

      OnProgress(new TransferProgressEventArgs("Gathering Referenced Resources", 20, 100));

      //foreach(Series series in args.List<Series>)
      //{
      //  foreach(Series referenced in series.References)
      //  {
      //    Study refSeriesStudy = referenced.Study;
      //
      //    if( studyToTransfer != refSeriesStudy && !refStudyList.Contains(refSeriesStudy) )
      //    {
      //      refStudyList.Add(refSeriesStudy);
      //    } // if( refSeriesPatient != patientToTransfer && ... )
      //  } // foreach(Series referenced in series.References)
      //} // foreach(Series series in args.List<Series>)

      //for(int i=0;i<refStudyList.Count;i++)
      //{
      //  foreach( Series series in refStudyList[i].Series )
      //  {
      //    foreach( Series referenced in series.References )
      //    {
      //      Study refSeriesStudy = referenced.Study;
      //
      //      bool studySame = false;
      //
      //      if( studyToTransfer != refSeriesStudy && !refStudyList.Contains(refSeriesStudy) )
      //      {
      //        refStudyList.Add(refSeriesStudy);
      //      } // if( studyToTransfer != refSeriesStudy && !refStudyList.Contains(refSeriesStudy) )
      //    } // foreach(Series referenced in series.References)
      //  } // foreach(Series series in args.List<Series>)
      //} // for(int i=0;i<refStudyList.Count;i++)

      ///////////////////////////////////////////////////////////////////////////////////////
      // Check for the disk space needed and the free space on the device.
      ///////////////////////////////////////////////////////////////////////////////////////

      OnProgress(new TransferProgressEventArgs("Checking Disk Space", 30, 100));

      long directoryUsedSpace = PathUtilities.GetSize(sourcePatientLocation, true);

      //foreach(Study study in refStudyList)
      //{
      //  directoryUsedSpace += PathUtilities.GetSize(Path.Combine(patientLoc, study.Location), true);
      //} // foreach(Patient patient in refPatientsList)

      long free = SystemInfo.GetAvailableDiskSpace(transferToDirectory);

      if(free < directoryUsedSpace)
      {
        Logging.Log.Error("Transfer could not continue.  There was not enough free space to transfer" +
          "the desired data.  Needed: " + directoryUsedSpace + " Available: " + free, "FileSystemTransferProvider", "Transfer");

        throw new IOException("There is not enough free space to transfer the desired data.  Please clean" +
          " up the disk and try again.");
      } // if(free < directoryUsedSpace)

      ///////////////////////////////////////////////////////////////////////////////////////
      // Write out selected patient first, then the referenced studies.
      ///////////////////////////////////////////////////////////////////////////////////////

      OnProgress(new TransferProgressEventArgs("Transferring Requested Resources", 35, 100));

      try
      {
        bool directoryExists = true;
        string guidDirectory = string.Empty;
        for(int i=0;i<8192;i++)
        {
          guidDirectory = Path.Combine(transferToDirectory, Guid.NewGuid().ToString());

          if(!System.IO.Directory.Exists(guidDirectory))
          {
            args.TransferObject = guidDirectory;
            directoryExists = false;
            break;
          } // if(!System.IO.Directory.Exists(guidDirectory))
        } // for(int i=0;i<8192;i++)

        if(!directoryExists)
        {
          PathUtilities.CopyDirectory(sourcePatientLocation, guidDirectory, false, false, true);
        } // if(!directoryExists)
        else
        {
          Logging.Log.Error("Unique transfer session directory couldn't be created!", "FileSystemTransferProvider", "Transfer");

          throw new TransferException("Unique transfer session directory couldn't be created!");
        } // else
      } // try
      catch( System.IO.IOException e )
      {
        Logging.Log.Error("Error in writing out study to file system archival;  Exception: " +
          e.Message, "FileSystemTransferProvider", "Transfer");

        throw new TransferException("Error in writing out transfer.");
      } // catch(System.IO.IOException e)

      //foreach(Study study in refStudyList)
      //{
      //  // Need to add referenced studies to patient object to create acurate TOC.
      //  // Will this affect performance later on?
      //  studyToTransfer.Patient.Studies.Add(study);
      //
      //  try
      //  {
      //    // TODO Need to revise to take into account referenced study's actual location on disk.
      //    PathUtilities.CopyDirectory(study.Location, 
      //      , false, false, true);
      //  } // try
      //  catch(System.IO.IOException e)
      //  {
      //    Logging.Logger.Err("Error in writing out study to file system archival;  Exception: " +
      //      e.Message);
      //  } // catch(System.IO.IOException e)
      //} // foreach(Study study in refStudyList)

      ///////////////////////////////////////////////////////////////////////////////////////
      // Perform any cleanup operations required.
      ///////////////////////////////////////////////////////////////////////////////////////

      OnProgress(new TransferProgressEventArgs("Transfer Completed", 100, 100));
    } // Transfer( TransferProviderArgs args )


    /// <summary>
    /// Handles an abort event.
    /// </summary>
    public void Abort()
    {
      if(m_transferToDirectory != string.Empty)
      {
        try
        {
          Viatronix.Utilities.IO.PathUtilities.DeleteDirectory(m_transferToDirectory);
        } // try
        catch(IOException e)
        {
          Log.Error(string.Format("Error in cleaning up File System Transfer outputs when an abort was issued.  [ERROR=\'{0}\']", e.Message), "FileSystemTransferProvider", "Abort");
        } // catch(IOException e)
      } // if(m_transferToDirectory != string.Empty)

      throw new TransferAbortException(" User Aborted Transfer Operation ");
    } // Abort()

    #endregion

  }//class FileSystemTransferProvider
}//namespace Viatronix.Console


#region revision History

// $Log: FileSystemTransferProvider.cs,v $
// Revision 1.3.2.4  2011/05/18 01:56:59  mkontak
// Issue CON-1640: Windows 7 Compliance by removing the SystemSettings
// registry to a global config file. Move the default log directory to the windows
// common application data directory (XP - C:\Document and Settings\All Users\Application Data , Win7 - C:\ProgramData ).
// Merged the Viatronix.v3D.Logger project into the Viatronix.v3D.Utilities project and
// signed the Utilities assembly.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.3.2.3  2009/02/26 20:40:36  kchalupa
// Issue #6256: Needed to set current progress to 100/100 to have the ProgressDialog close itself.
//
// Revision 1.3.2.2  2008/10/15 20:03:23  kchalupa
// FileSystemTransferProvider TOC Removal and now copies into GUID subdirectory under selected directory.
//
// Revision 1.3.2.1  2008/10/03 19:35:59  kchalupa
// File System Transfer Functionality
//
// Revision 1.3  2007/02/28 19:58:15  romy
// added comments before the code review
//
// Revision 1.2  2006/02/16 13:00:08  mkontak
// Transfer from PACS to V3D
//
// Revision 1.1  2005/12/13 20:45:59  romy
// re arranged
//
// Revision 1.5  2005/12/02 02:41:43  romy
// coding standards
//
// Revision 1.4  2005/09/30 16:06:21  romy
// added args param
//
// Revision 1.3  2005/09/22 16:16:37  romy
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/Transfer/FileSystemTransferProvider.cs,v 1.3.2.4 2011/05/18 01:56:59 mkontak Exp $
// $Id: FileSystemTransferProvider.cs,v 1.3.2.4 2011/05/18 01:56:59 mkontak Exp $

#endregion revision History