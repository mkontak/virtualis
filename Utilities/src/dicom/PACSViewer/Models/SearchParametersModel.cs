// Copyright © 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Kyle R Chalupa(mailto:kchalupa@viatronix.com)

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Viatronix.Dicom;

namespace Viatronix.Utilities.Dicom.PACSViewer.Models
{
  /// <summary>
  /// The parameters model.
  /// </summary>
  public class SearchParametersModel
  {

    #region fields

    /// <summary>
    /// The parameters.
    /// </summary>
    private CFindParameters m_parameters = CFindParameters.Create();

    /// <summary>
    /// The patient date of birth.
    /// </summary>
    private DateTime m_patientDobFrom = new DateTime(1900, 1, 1);

    /// <summary>
    /// The patient date of birth.
    /// </summary>
    private DateTime m_patientDobTo = new DateTime(2099, 12, 31);

    /// <summary>
    /// The patient time of birth.
    /// </summary>
    private DateTime m_patientTobFrom = new DateTime(1900, 1, 1);

    /// <summary>
    /// The patient time of birth.
    /// </summary>
    private DateTime m_patientTobTo = new DateTime(2099, 12, 31);

    /// <summary>
    /// The study date.
    /// </summary>
    private DateTime m_studyDateFrom = new DateTime(1900, 1, 1);

    /// <summary>
    /// The study date.
    /// </summary>
    private DateTime m_studyDateTo = new DateTime(2099, 12, 31);

    /// <summary>
    /// The study time.
    /// </summary>
    private DateTime m_studyTimeFrom = new DateTime(1900, 1, 1);

    /// <summary>
    /// The study time.
    /// </summary>
    private DateTime m_studyTimeTo = new DateTime(2099, 12, 31);

    /// <summary>
    /// The series date.
    /// </summary>
    private DateTime m_seriesDateFrom = new DateTime(1900, 1, 1);

    /// <summary>
    /// The series date.
    /// </summary>
    private DateTime m_seriesDateTo = new DateTime(2099, 12, 31);

    /// <summary>
    /// The series time.
    /// </summary>
    private DateTime m_seriesTimeFrom = new DateTime(1900, 1, 1);

    /// <summary>
    /// The series time.
    /// </summary>
    private DateTime m_seriesTimeTo = new DateTime(2099, 12, 31);

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the patient date of birth.
    /// </summary>
    public DateTime PatientDateOfBirthFrom
    {
      get { return m_patientDobFrom; }
      set
      {
        m_patientDobFrom = value;
        m_parameters.SetPatientDob(PatientDateOfBirthFrom, PatientDateOfBirthTo);
      }
    } // PatientDateOfBirthFrom


    /// <summary>
    /// Gets or sets the patient date of birth.
    /// </summary>
    public DateTime PatientDateOfBirthTo
    {
      get { return m_patientDobTo; }
      set
      {
        m_patientDobTo = value;
        m_parameters.SetPatientDob(PatientDateOfBirthFrom, PatientDateOfBirthTo);
      }
    } // PatientDateOfBirthTo


    /// <summary>
    /// Gets or sets the patient time of birth.
    /// </summary>
    public DateTime PatientTimeOfBirthFrom
    {
      get { return m_patientTobFrom; }
      set
      {
        m_patientTobFrom = value;
        m_parameters.SetPatientTob(PatientTimeOfBirthFrom, PatientTimeOfBirthTo);
      }
    } // PatientTimeOfBirthFrom


    /// <summary>
    /// Gets or sets the patient time of birth.
    /// </summary>
    public DateTime PatientTimeOfBirthTo
    {
      get { return m_patientTobTo; }
      set
      {
        m_patientTobTo = value;
        m_parameters.SetPatientTob(PatientTimeOfBirthFrom, PatientTimeOfBirthTo);
      }
    } // PatientTimeOfBirthTo


    /// <summary>
    /// Gets or sets the study date.
    /// </summary>
    public DateTime StudyDateFrom
    {
      get { return m_studyDateFrom; }
      set
      {
        m_studyDateFrom = value;
        m_parameters.SetStudyDate(StudyDateFrom, StudyDateTo);
      }
    } // StudyDateFrom


    /// <summary>
    /// Gets or sets the study date.
    /// </summary>
    public DateTime StudyDateTo
    {
      get { return m_studyDateTo; }
      set
      {
        m_studyDateTo = value;
        m_parameters.SetStudyDate(StudyDateFrom, StudyDateTo);
      }
    } // StudyDateTo


    /// <summary>
    /// Gets or sets the study time.
    /// </summary>
    public DateTime StudyTimeFrom
    {
      get { return m_studyTimeFrom; }
      set
      {
        m_studyTimeFrom = value;
        m_parameters.SetStudyTime(StudyTimeFrom, StudyTimeTo);
      }
    } // StudyTimeFrom


    /// <summary>
    /// Gets or sets the study time.
    /// </summary>
    public DateTime StudyTimeTo
    {
      get { return m_studyTimeTo; }
      set
      {
        m_studyTimeTo = value;
        m_parameters.SetStudyTime(StudyTimeFrom, StudyTimeTo);
      }
    } // StudyTimeTo


    /// <summary>
    /// Gets or sets the series date.
    /// </summary>
    public DateTime SeriesDateFrom
    {
      get { return m_seriesDateFrom; }
      set
      {
        m_seriesDateFrom = value;
        m_parameters.SetStudyDate(SeriesDateFrom, SeriesDateTo);
      }
    } // SeriesDateFrom


    /// <summary>
    /// Gets or sets the series date.
    /// </summary>
    public DateTime SeriesDateTo
    {
      get { return m_seriesDateTo; }
      set
      {
        m_seriesDateTo = value;
        m_parameters.SetStudyDate(SeriesDateFrom, SeriesDateTo);
      }
    } // SeriesDateTo


    /// <summary>
    /// Gets and sets the series time.
    /// </summary>
    public DateTime SeriesTimeFrom
    {
      get { return m_seriesTimeFrom; }
      set
      {
        m_seriesTimeFrom = value;
        m_parameters.SetSeriesTime(SeriesTimeFrom, SeriesTimeTo);
      }
    } // SeriesTimeFrom


    /// <summary>
    /// Gets and sets the series time.
    /// </summary>
    public DateTime SeriesTimeTo
    {
      get { return m_seriesTimeTo; }
      set
      {
        m_seriesTimeTo = value;
        m_parameters.SetSeriesTime(SeriesTimeFrom, SeriesTimeTo);
      }
    } // SeriesTimeTo

    public string PatientName
    {
      get { return m_parameters.PatientNames;  }
      set { m_parameters.PatientNames = value;  }
    }

    /// <summary>
    /// Gets or sets the search level.
    /// </summary>
    public string Level
    {
      get { return m_parameters.Level; }
      set { m_parameters.Level = value; }
    } // Level
    

    /// <summary>
    /// Gets or sets the medical id.
    /// </summary>
    public string MedicalId
    {
      get { return m_parameters.MedicalId; }
      set { m_parameters.MedicalId = value; }
    } // MedicalId


    /// <summary>
    /// Gets or sets the physician.
    /// </summary>
    public string Physician
    {
      get { return m_parameters.Physician; }
      set { m_parameters.Physician = value; }
    } // Physician


    /// <summary>
    /// Gets or sets the study id.
    /// </summary>
    public string StudyId
    {
      get { return m_parameters.StudyId; }
      set { m_parameters.StudyId = value; }
    } // StudyId

    #endregion

    #region methods

 
    /// <summary>
    /// Gets the parameters.
    /// </summary>
    /// <returns>The parameters.</returns>
    public CFindParameters GetParameters()
    {
      return m_parameters;
    } // GetParameters()


    /// <summary>
    /// Adds the patient name.
    /// </summary>
    /// <param name="value">The patient name to add.</param>
    public void AddPatientName(string value)
    {
      m_parameters.AddPatientNames(value);
    } // AddPatientName( value )


    /// <summary>
    /// Adds the study uid.
    /// </summary>
    /// <param name="value">The study uid to add.</param>
    public void AddStudyUid(string value)
    {
      m_parameters.AddStudyUids(value);
    } // AddStudyUid( value )


    /// <summary>
    /// Adds the series uid.
    /// </summary>
    /// <param name="value">The series uid to add.</param>
    public void AddSeriesUid(string value)
    {
      m_parameters.AddSeriesUids(value);
    } // AddSeriesUid( value )


    /// <summary>
    /// Adds the instance uid to the list.
    /// </summary>
    /// <param name="value">The instance uid to add.</param>
    public void AddInstanceUid(string value)
    {
      m_parameters.AddInstanceUids(value);
    } // AddInstanceUid( value )


    /// <summary>
    /// Adds the modality to the list.
    /// </summary>
    /// <param name="value">The modality to add.</param>
    public void AddModality(string value)
    {
      m_parameters.AddModalities(value);
    } // AddModality( value )


    /// <summary>
    /// Clears out the patient names.
    /// </summary>
    public void ClearPatientNames()
    {
      m_parameters.ClearPatientNames();
    } // ClearPatientNames()


    /// <summary>
    /// Clears out the study uids.
    /// </summary>
    public void ClearStudyUids()
    {
      m_parameters.ClearStudyUids();
    } // ClearStudyUids()


    /// <summary>
    /// Clears out the series uids.
    /// </summary>
    public void ClearSeriesUids()
    {
      m_parameters.ClearSeriesUids();
    } // ClearSeriesUids()


    /// <summary>
    /// Clears out the instance uids.
    /// </summary>
    public void ClearInstanceUids()
    {
      m_parameters.ClearInstanceUids();
    } // ClearInstanceUids()


    /// <summary>
    /// Clears out the modalities.
    /// </summary>
    public void ClearModalities()
    {
      m_parameters.ClearModalities();
    } // ClearModalities()

    #endregion

  } // class SearchParametersModel
} // namespace Viatronix.Utilities.Dicom.PACSViewer.Models
