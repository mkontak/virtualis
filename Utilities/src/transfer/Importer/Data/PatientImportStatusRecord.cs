// $Id: PatientImportStatusRecord.cs,v 1.1.2.2 2009/10/30 14:02:31 kchalupa Exp $
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

using Viatronix.Enterprise.Entities;

namespace Viatronix.Utilities.Importer.Data
{
  /// <summary>
  /// The state of importing the directory.
  /// </summary>
  public enum Status
  {
    Successful,  // Importing was successful.
    Failed,      // Importing failed.
    Pending,     // Importing is currently running.
    Unknown,     // Importing unset.
  } // Status


  /// <summary>
  /// Delegate to create an event that the status was set on a directory.
  /// </summary>
  /// <param name="directory">The directory that was imported.</param>
  /// <param name="status">The resulting status.</param>
  public delegate void StatusDelegate(string directory, Status status);

  /// <summary>
  /// Records the status of a patient import.
  /// </summary>
  public class DirectoryImportStatusRecord
  {

    #region events

    /// <summary>
    /// Inform that the status was set.
    /// </summary>
    public static StatusDelegate StatusSet;

    #endregion

    #region fields

    /// <summary>
    /// Base import directory.
    /// </summary>
    private string m_importDirectory = string.Empty;

    /// <summary>
    /// The patient that is attempted to be imported.
    /// </summary>
    private Patient m_patient = null;

    /// <summary>
    /// The number of studies that have failed to be imported.
    /// </summary>
    private int m_studiesFailed = 0;

    /// <summary>
    /// The number of series that have failed to be imported.
    /// </summary>
    private int m_seriesFailed = 0;

    /// <summary>
    /// Status of importing.
    /// </summary>
    private Status m_status = Status.Unknown;

    #endregion

    #region construction

    /// <summary>
    /// Records a failure to import a patient.
    /// </summary>
    /// <param name="importDirectory">The directory that is currently being imported.</param>
    public DirectoryImportStatusRecord(string importDirectory)
    {
      m_importDirectory = importDirectory;
      m_studiesFailed = 0;
      m_seriesFailed = 0;
    } // PatientImportStatusRecord(string importDirectory)
    
    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the directory that we're importing from.
    /// </summary>
    public string ImportDirectory
    {
      get { return m_importDirectory;  }
      set { m_importDirectory = value; }
    } // ImportDirectory


    /// <summary>
    /// Gets or sets the patient that was read from the ImportDirectory.
    /// </summary>
    public Patient Patient
    {
      get { return m_patient;  }
      set { m_patient = value; }
    } // Patient


    /// <summary>
    /// Gets the number of studies that failed to be imported.
    /// </summary>
    public int StudiesFailed
    {
      get { return m_studiesFailed; }
    } // StudiesFailed


    /// <summary>
    /// Gets the number of series that failed to be imported.
    /// </summary>
    public int SeriesFailed
    {
      get { return m_seriesFailed; }
    } // StudiesFailed


    /// <summary>
    /// Gets the status of importing.
    /// </summary>
    public Status Status
    {
      get { return m_status; }
    } // Status

    #endregion

    #region methods

    /// <summary>
    /// Report the patient import passed.
    /// </summary>
    public void ReportPatientImportSucceeded()
    {
      if (m_status != Status.Successful)
      {
        m_status = Status.Successful;

        if (StatusSet != null)
        {
          StatusSet(m_importDirectory, m_status);
        }

        Utils.LogMessage(LogTypes.Error, "Directory Import Successful! Source Path: " + m_importDirectory);
      }
    } // ReportPatientImportSucceeded()


    /// <summary>
    /// Report the patient import as a failure.
    /// </summary>
    public void ReportPatientImportFailed()
    {
      if (m_status != Status.Failed)
      {
        m_status = Status.Failed;

        if (StatusSet != null)
        {
          StatusSet(m_importDirectory, m_status);
        }

        Utils.LogMessage(LogTypes.Error, "Directory Import Failed! Source Path: " + m_importDirectory);
      }
    } // ReportPatientImportFailed()


    /// <summary>
    /// Reports that the patient import was started.
    /// </summary>
    public void ReportPatientImportStarted()
    {
      if (m_status != Status.Pending)
      {
        m_status = Status.Pending;

        if (StatusSet != null)
        {
          StatusSet(m_importDirectory, m_status);
        }

        Utils.LogMessage(LogTypes.Info, String.Format("Directory Import Started.  [DIRECTORY={0}]", m_importDirectory));
      }
    } // ReportPatientImportStarted()


    /// <summary>
    /// Returns a string representation of the record, specifically the status.
    /// </summary>
    /// <returns></returns>
    public override string ToString()
    {
      return System.IO.Path.GetFileName(m_importDirectory) + " " + m_status.ToString();
    } // ToString()

    #endregion

  } // DirectoryImportStatusRecord
} // namespace Viatronix.Utilities.Importer.Data

#region revision history

// $Log: PatientImportStatusRecord.cs,v $
// Revision 1.1.2.2  2009/10/30 14:02:31  kchalupa
// Update the status enum and surrounding functions
//
// Revision 1.1.2.1  2009/06/04 18:59:15  kchalupa
// Initial Revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities.Importer/PatientImportStatusRecord.cs,v 1.1.2.2 2009/10/30 14:02:31 kchalupa Exp $
// $Id: PatientImportStatusRecord.cs,v 1.1.2.2 2009/10/30 14:02:31 kchalupa Exp $

#endregion
