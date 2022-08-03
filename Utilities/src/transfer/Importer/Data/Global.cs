// $Id: Global.cs,v 1.1.2.2 2009/10/30 13:58:17 kchalupa Exp $
//
// Copyright © 2009, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Kyle R Chalupa] [kchalupa@viatronix.com]

using System;
using System.Collections.Generic;
using System.Text;

//using Viatronix.v3D.Core;
using Viatronix.Utilities.Importer.Data;

namespace Viatronix.Utilities.Importer.Data
{

  #region delegates

  /// <summary>
  /// Delegate for informing that a progress event has occurred.
  /// </summary>
  /// <param name="message">Describes the current action.</param>
  /// <param name="progress">The current progress, an integer between 0 and 100; inclusive.</param>
  public delegate void ProgressEventHandler(string message, int progress);

  #endregion

  /// <summary>
  /// Global class.
  /// </summary>
  public static class Global
  {

    #region events

    /// <summary>
    /// Notifies that progress has occurred.
    /// </summary>
    public static event ProgressEventHandler Progress;

    #endregion

    #region fields

    /// <summary>
    /// Dictionary of the directory to the importing record.
    /// </summary>
    private static Dictionary<string, DirectoryImportStatusRecord> m_directoryToImportRecord = new Dictionary<string, DirectoryImportStatusRecord>();

    /// <summary>
    /// The main dialog.
    /// </summary>
    private static ImportDialog m_importDialog = null;
    
    /// <summary>
    /// The cancelled flag.
    /// </summary>
    private static bool m_cancelled = false;

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets a map from the directory to import to the record stating the status of the import.
    /// </summary>
    public static Dictionary<string, DirectoryImportStatusRecord> DirectoryToRecord
    {
      get { return m_directoryToImportRecord;  } 
      set { m_directoryToImportRecord = value; }
    } // DirectoryToRecord


    /// <summary>
    /// Gets or sets the main dialog.
    /// </summary>
    public static ImportDialog MainDialog
    {
      get { return m_importDialog;  }
      set 
      {
        m_importDialog = value;
        m_importDialog.Cancelled += new EventHandler(OnImportCancelledHandler);
      }
    } // MainDialog


    /// <summary>
    /// Gets or sets the cancelled flag.
    /// </summary>
    public static bool Cancelled
    {
      get { return m_cancelled; }
      set { m_cancelled = value; }
    } // Cancelled

    #endregion

    #region methods

    /// <summary>
    /// Initialize.
    /// </summary>
    public static void Initialize()
    {
      Cancelled = false;
      DirectoryToRecord.Clear();
    } // Initialize()


    /// <summary>
    /// Logs that the specified directory was noticed and was examined.
    /// </summary>
    /// <param name="directory">The directory that is being imported.</param>
    public static void LogDirectoryNoticed(string directory)
    {
      DirectoryImportStatusRecord record = null;
      if(!m_directoryToImportRecord.TryGetValue(directory, out record))
      {
        record = new DirectoryImportStatusRecord(directory);
        record.ReportPatientImportStarted();

        m_directoryToImportRecord.Add(directory, record);
      }
    } // LogDirectoryNoticed( directory )


    /// <summary>
    /// Log that the patient was imported successfully.
    /// </summary>
    /// <param name="directory">The directory that was imported.</param>
    public static void LogPatientImportSucceeded(string directory)
    {
      DirectoryImportStatusRecord record = null;
      if (m_directoryToImportRecord.TryGetValue(directory, out record))
      {
        record.ReportPatientImportSucceeded();
      }
      else
      {
        record = new DirectoryImportStatusRecord(directory);
        record.ReportPatientImportSucceeded();

        m_directoryToImportRecord.Add(directory, record);
      }

      Global.NotifyProgress("Import Completed Successfully!", 100);
    } // LogPatientImportSucceeded( directory )


    /// <summary>
    /// Logs a failed attempt to import a study into the system.
    /// </summary>
    /// <param name="directory">The directory that contains the study inteded for import.</param>
    public static void LogPatientImportFailed(string directory)
    {
      DirectoryImportStatusRecord record = null;
      if (m_directoryToImportRecord.TryGetValue(directory, out record))
      {
        record.ReportPatientImportFailed();
      }
      else
      {
        record = new DirectoryImportStatusRecord(directory);
        record.ReportPatientImportFailed();

        m_directoryToImportRecord.Add(directory, record);
      }

      Global.NotifyProgress("Import Failed!", 100);
    } // LogPatientImportFailed( directory )


    /// <summary>
    /// Notifies that progress has occurred.
    /// </summary>
    /// <param name="message">The message that describes the current action.</param>
    /// <param name="progress">The current progress, an integer between 0 and 100; inclusive.</param>
    public static void NotifyProgress(string message, int progress)
    {
      if (Progress != null)
      {
        Progress(message, progress);
      }
    } // NotifyProgress( message, progress )


    /// <summary>
    /// Checks for the cancel.
    /// </summary>
    public static void CheckForCancel()
    {
      if (Global.Cancelled)
      {
        throw new ApplicationException("Import was cancelled.");
      }
    } // CheckForCancel()




    #endregion

    #region event handlers

    /// <summary>
    /// Handles the import cancel.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private static void OnImportCancelledHandler(object sender, EventArgs args)
    {
      Global.Cancelled = true;
    } // OnImportCancelledHandler( sender, args )

    #endregion

  } // class Global
} // namespace Viatronix.Utilities.Importer.Data

#region revision history

// $Log: Global.cs,v $
// Revision 1.1.2.2  2009/10/30 13:58:17  kchalupa
// Remove partial study/series status.
//
// Revision 1.1.2.1  2009/06/04 19:00:06  kchalupa
// Initial Revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities.Importer/Global.cs,v 1.1.2.2 2009/10/30 13:58:17 kchalupa Exp $
// $Id: Global.cs,v 1.1.2.2 2009/10/30 13:58:17 kchalupa Exp $

#endregion
