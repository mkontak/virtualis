// Copyright © 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Kyle R Chalupa(mailto:kchalupa@viatronix.com)

using System;
using System.Configuration;
using System.Collections.ObjectModel;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;

using Viatronix.Utilities.Dicom.PACSViewer.Models;

namespace Viatronix.Utilities.Dicom.PACSViewer.ViewModels
{
  /// <summary>
  /// The search parameters view model.
  /// </summary>
  public class SearchParametersViewModel : ViewModelBase
  {

    #region fields

    /// <summary>
    /// The search parameters back end.
    /// </summary>
    private SearchParametersModel m_model = null;

    /// <summary>
    /// The list of search types.
    /// </summary>
    private ObservableCollection<string> m_searchTypes = new ObservableCollection<string>();
    
    /// <summary>
    /// The selected search type.
    /// </summary>
    private string m_selectedType = string.Empty;

    /// <summary>
    /// The patient name.
    /// </summary>
    private string m_patientName = string.Empty;

    /// <summary>
    /// The study uid.
    /// </summary>
    private string m_studyUid = string.Empty;

    /// <summary>
    /// The series uid.
    /// </summary>
    private string m_seriesUid = string.Empty;

    /// <summary>
    /// The instance uid.
    /// </summary>
    private string m_instanceUid = string.Empty;

    /// <summary>
    /// The list of modalities.
    /// </summary>
    private ObservableCollection<ModalityModel> m_modalities = new ObservableCollection<ModalityModel>();

    /// <summary>
    /// The selected modality.
    /// </summary>
    private ModalityModel m_selectedModality = null;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public SearchParametersViewModel()
    {
      m_model = new SearchParametersModel();

      PatientNameCommand = new RelayCommand(AddPatientName, () => true);
      StudyUidCommand    = new RelayCommand(AddStudyUid,    () => true);
      SeriesUidCommand   = new RelayCommand(AddSeriesUid,   () => true);
      InstanceUidCommand = new RelayCommand(AddInstanceUid, () => true);
      ModalityCommand    = new RelayCommand(AddModality,    () => true);
      ClearCommand       = new RelayCommand(Clear,          () => true);

      SearchTypes.Add("Patient");
      SearchTypes.Add("Study");
      SearchTypes.Add("Series");

      SelectedType = SearchTypes[0];

      Modalities = (ObservableCollection<ModalityModel>)ConfigurationManager.GetSection("modalities");
      if (Modalities.Count > 0)
      {
        SelectedModality = Modalities[0];
      }
    } // SearchParametersViewModel()

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the search types.
    /// </summary>
    public ObservableCollection<string> SearchTypes
    {
      get { return m_searchTypes; }
      set 
      { 
        m_searchTypes = value;
        RaisePropertyChanged("SearchTypes");
      }
    } // SearchTypes


    /// <summary>
    /// Gets or sets the selected type.
    /// </summary>
    public string SelectedType
    {
      get { return m_selectedType; }
      set
      {
        m_selectedType = value;
        m_model.Level =  value;
        RaisePropertyChanged("SelectedType");
      }
    } // SelectedType


    /// <summary>
    /// Gets or sets the patient name.
    /// </summary>
    public string PatientName
    {
      get { return m_patientName; }
      set
      {
        m_patientName = value;
        m_model.PatientName = value;
        RaisePropertyChanged("PatientName");
      }
    } // PatientName


    /// <summary>
    /// Gets or sets the patient date of birth.
    /// </summary>
    public DateTime PatientDateOfBirthFrom
    {
      get { return m_model.PatientDateOfBirthFrom; }
      set
      { 
        m_model.PatientDateOfBirthFrom = value;
        RaisePropertyChanged("PatientDateOfBirthFrom");
      }
    } // PatientDateOfBirthFrom


    /// <summary>
    /// Gets or sets the patient date of birth.
    /// </summary>
    public DateTime PatientDateOfBirthTo
    {
      get { return m_model.PatientDateOfBirthTo; }
      set 
      { 
        m_model.PatientDateOfBirthTo = value;
        RaisePropertyChanged("PatientDateOfBirthTo");
      }
    } // PatientDateOfBirthTo


    /// <summary>
    /// Gets or sets the patient time of birth.
    /// </summary>
    public DateTime PatientTimeOfBirthFrom
    {
      get { return m_model.PatientTimeOfBirthFrom; }
      set 
      { 
        m_model.PatientTimeOfBirthFrom = value;
        RaisePropertyChanged("PatientTimeOfBirthFrom");
      }
    } // PatientTimeOfBirthFrom


    /// <summary>
    /// Gets or sets the patient time of birth.
    /// </summary>
    public DateTime PatientTimeOfBirthTo
    {
      get { return m_model.PatientDateOfBirthTo; }
      set
      {
        m_model.PatientDateOfBirthTo = value;
        RaisePropertyChanged("PatientTimeOfBirthTo");
      }
    } // PatientTimeOfBirthTo


    /// <summary>
    /// Gets or sets the medical id.
    /// </summary>
    public string MedicalId
    {
      get { return m_model.MedicalId; }
      set
      {
        m_model.MedicalId = value;
        RaisePropertyChanged("MedicalId");
      }
    } // MedicalId


    /// <summary>
    /// Gets or sets the study uid.
    /// </summary>
    public string StudyUid
    {
      get { return m_studyUid; }
      set
      {
        m_studyUid = value;
        RaisePropertyChanged("StudyUid");
      }
    } // StudyUid


    /// <summary>
    /// Gets or sets the study date.
    /// </summary>
    public DateTime StudyDateFrom
    {
      get { return m_model.StudyDateFrom; }
      set
      {
        m_model.StudyDateFrom = value;
        RaisePropertyChanged("StudyDateFrom");
      }
    } // StudyDateFrom


    /// <summary>
    /// Gets or sets the study date.
    /// </summary>
    public DateTime StudyDateTo
    {
      get { return m_model.StudyDateTo; }
      set
      {
        m_model.StudyDateTo = value;
        RaisePropertyChanged("StudyDateTo");
      }
    } // StudyDateTo


    /// <summary>
    /// Gets or sets the study time.
    /// </summary>
    public DateTime StudyTimeFrom
    {
      get { return m_model.StudyTimeFrom; }
      set
      {
        m_model.StudyTimeFrom = value;
        RaisePropertyChanged("StudyTimeFrom");
      }
    } // StudyTimeFrom


    /// <summary>
    /// Gets or sets the study time.
    /// </summary>
    public DateTime StudyTimeTo
    {
      get { return m_model.StudyTimeTo; }
      set
      {
        m_model.StudyTimeTo = value;
        RaisePropertyChanged("StudyTimeTo");
      }
    } // StudyTimeTo


    /// <summary>
    /// Gets or sets the physician.
    /// </summary>
    public string Physician
    {
      get { return m_model.Physician; }
      set
      {
        m_model.Physician = value;
        RaisePropertyChanged("Physician");
      }
    } // Physician


    /// <summary>
    /// Gets or sets the study id.
    /// </summary>
    public string StudyId
    {
      get { return m_model.StudyId; }
      set
      {
        m_model.StudyId = value;
        RaisePropertyChanged("StudyId");
      }
    } // StudyId


    /// <summary>
    /// Gets or sets the series uid.
    /// </summary>
    public string SeriesUid
    {
      get { return m_seriesUid; }
      set
      {
        m_seriesUid = value;
        RaisePropertyChanged("SeriesUid");
      }
    } // SeriesUid


    /// <summary>
    /// Gets or sets the series date.
    /// </summary>
    public DateTime SeriesDateFrom
    {
      get { return m_model.SeriesDateFrom; }
      set
      {
        m_model.SeriesDateFrom = value;
        RaisePropertyChanged("SeriesDateFrom");
      }
    } // SeriesDateFrom


    /// <summary>
    /// Gets or sets the series date.
    /// </summary>
    public DateTime SeriesDateTo
    {
      get { return m_model.SeriesDateTo; }
      set
      {
        m_model.SeriesDateTo = value;
        RaisePropertyChanged("SeriesDateTo");
      }
    } // SeriesDateTo


    /// <summary>
    /// Gets or sets the series time.
    /// </summary>
    public DateTime SeriesTimeFrom
    {
      get { return m_model.SeriesTimeFrom; }
      set
      {
        m_model.SeriesTimeFrom = value;
        RaisePropertyChanged("SeriesTimeFrom");
      }
    } // SeriesTimeFrom


    /// <summary>
    /// Gets or sets the series time.
    /// </summary>
    public DateTime SeriesTimeTo
    {
      get { return m_model.SeriesTimeTo; }
      set
      {
        m_model.SeriesTimeTo = value;
        RaisePropertyChanged("SeriesTimeTo");
      }
    } // SeriesTimeTo


    /// <summary>
    /// Gets or sets the instance uid.
    /// </summary>
    public string InstanceUid
    {
      get { return m_instanceUid; }
      set
      {
        m_instanceUid = value;
        RaisePropertyChanged("InstanceUid");
      }
    } // InstanceUid


    /// <summary>
    /// Gets or sets the list of modalities.
    /// </summary>
    public ObservableCollection<ModalityModel> Modalities
    {
      get { return m_modalities; }
      set
      {
        m_modalities = value;
        RaisePropertyChanged("Modalities");
      }
    } // Modalities


    /// <summary>
    /// Gets or sets the selected modality.
    /// </summary>
    public ModalityModel SelectedModality
    {
      get { return m_selectedModality; }
      set
      {
        m_selectedModality = value;
        m_model.ClearModalities();
        m_model.AddModality(value.Id.ToUpper());
        RaisePropertyChanged("SelectedModality");
      }
    } // SelectedModality

    #endregion

    #region commands

    /// <summary>
    /// Gets or sets the command to add the patient name.
    /// </summary>
    public RelayCommand PatientNameCommand
    {
      get;
      private set;
    } // PatientNameCommand


    /// <summary>
    /// Gets or sets the command to add the study uid.
    /// </summary>
    public RelayCommand StudyUidCommand
    {
      get;
      private set;
    } // StudyUidCommand


    /// <summary>
    /// Gets or sets the command to add the series uid.
    /// </summary>
    public RelayCommand SeriesUidCommand
    {
      get;
      private set;
    } // SeriesUidCommand


    /// <summary>
    /// Gets or sets the command to add the instance uid.
    /// </summary>
    public RelayCommand InstanceUidCommand
    {
      get;
      private set;
    } // InstanceUidCommand


    /// <summary>
    /// Gets or sets the command to add the modality.
    /// </summary>
    public RelayCommand ModalityCommand
    {
      get;
      private set;
    } // ModalityCommand


    /// <summary>
    /// Gets or sets the command to clear the fields.
    /// </summary>
    public RelayCommand ClearCommand
    {
      get;
      private set;
    } // ClearCommand

    #endregion

    #region methods

    /// <summary>
    /// Gets the model.
    /// </summary>
    public SearchParametersModel GetModel()
    {
      return m_model;
    } // GetModel()


    /// <summary>
    /// Adds the patient name to the model.
    /// </summary>
    private void AddPatientName()
    {
      if (PatientName != string.Empty)
      {
        m_model.AddPatientName(PatientName);
        PatientName = string.Empty;
      }
      else
      {
        System.Windows.MessageBox.Show("The patient name cannot be empty.  Please enter a value and try again.", "Invalid Value", System.Windows.MessageBoxButton.OK, System.Windows.MessageBoxImage.Error);
      }
    } // AddPatientName()


    /// <summary>
    /// Adds the study uid to the model.
    /// </summary>
    private void AddStudyUid()
    {
      m_model.AddStudyUid(StudyUid);
      StudyUid = string.Empty;
    } // AddStudyUid()


    /// <summary>
    /// Adds the series uid to the model.
    /// </summary>
    private void AddSeriesUid()
    {
      m_model.AddSeriesUid(SeriesUid);
      SeriesUid = string.Empty;
    } // AddSeriesUid()


    /// <summary>
    /// Adds the instance uid to the mdoel.
    /// </summary>
    private void AddInstanceUid()
    {
      m_model.AddInstanceUid(InstanceUid);
      InstanceUid = string.Empty;
    } // AddInstanceUid()


    /// <summary>
    /// Adds the modality to the model.
    /// </summary>
    private void AddModality()
    {
      m_model.AddModality(SelectedModality.Id);

      if (Modalities.Count > 0)
      {
        SelectedModality = Modalities[0];
      }
    } // AddModality()


    /// <summary>
    /// Clears the fields.
    /// </summary>
    private void Clear()
    {
      PatientName            = string.Empty;
      PatientDateOfBirthFrom = new DateTime(1900, 1, 1);
      PatientDateOfBirthTo = new DateTime(2099, 12, 31);
      PatientTimeOfBirthFrom = new DateTime(1900, 1, 1);
      PatientTimeOfBirthTo   = new DateTime(2099, 12, 31);
      MedicalId              = string.Empty;
      StudyUid               = string.Empty;
      StudyDateFrom          = new DateTime(1900, 1, 1);
      StudyDateTo            = new DateTime(2099, 12, 31);
      StudyTimeFrom          = new DateTime(1900, 1, 1);
      StudyTimeTo            = new DateTime(2099, 12, 31);
      StudyId                = string.Empty;
      Physician              = string.Empty;
      SeriesUid              = string.Empty;
      SeriesDateFrom         = new DateTime(1900, 1, 1);
      SeriesDateTo           = new DateTime(2099, 12, 31);
      SeriesTimeFrom         = new DateTime(1900, 1, 1);
      SeriesTimeTo           = new DateTime(2099, 12, 31);
      InstanceUid            = string.Empty;

      if (Modalities.Count > 0)
      {
        SelectedModality = Modalities[0];
      }

      m_model.ClearPatientNames();
      m_model.ClearStudyUids();
      m_model.ClearSeriesUids();
      m_model.ClearInstanceUids();
      m_model.ClearModalities();
    } // Clear()

    #endregion

  } // class SearchParametersViewModel
} // namespace Viatronix.Utilities.Dicom.PACSViewer.ViewModels
