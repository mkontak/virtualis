using System;
using System.IO;
using System.Collections.ObjectModel;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using GalaSoft.MvvmLight;

using Viatronix.Utilities.IO;
using Viatronix.Enterprise.Entities;

namespace Viatronix.Utilities.Transfer.ViewModels
{
  /// <summary>
  /// A study view model.
  /// </summary>
  public class StudyViewModel : ViewModelBase
  {

    #region fields

    /// <summary>
    /// The study this view model is based on.
    /// </summary>
    private Study m_study = null;

    /// <summary>
    /// The list of series.
    /// </summary>
    private ObservableCollection<SeriesViewModel> m_series = new ObservableCollection<SeriesViewModel>();

    /// <summary>
    /// The staging area directory.
    /// </summary>
    private string m_stagingArea = string.Empty;

    #endregion

    #region construction
    
    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="study">The study to create from.</param>
    /// <param name="includeSeries">Should the internal series list be populated by the series in the study.</param>
    public StudyViewModel(Study study, bool includeSeries = true)
    {
      m_study = study;

      if(includeSeries)
      {
        foreach(Series series in study.Series)
        {
          SeriesViewModel seriesViewModel = new SeriesViewModel(series);
          m_series.Add(seriesViewModel);
        }
      }
    } // StudyViewModel( study, includeSeries )

    #endregion

    #region properties

    /// <summary>
    /// Gets the study.
    /// </summary>
    public Study Study
    {
      get { return m_study; }
    } // Study


    /// <summary>
    /// Gets or sets the patient's name.
    /// </summary>
    public string Name
    {
      get { return string.Format("{0}, {1}", m_study.Name.LastName, m_study.Name.FirstName); }
      set
      {
        m_study.Name = PersonName.Create(value);
        RaisePropertyChanged("Name");
      }
    } // Name


    /// <summary>
    /// Gets or sets the physician.
    /// </summary>
    public string Physician
    {
      get { return m_study.Physician; }
      set
      {
        m_study.Physician = value;
        RaisePropertyChanged("Physician");
      }
    } // Physician


    /// <summary>
    /// Gets or sets the medical id.
    /// </summary>
    public string MedicalId
    {
      get { return m_study.MedicalId; }
      set
      {
        m_study.MedicalId = value;
        RaisePropertyChanged("MedicalId");
      }
    } // MedicalId


    /// <summary>
    /// Gets or sets the patient date of birth.
    /// </summary>
    public DateTime Dob
    {
      get { return m_study.Dob; }
      set
      {
        m_study.Dob = value;
        RaisePropertyChanged("Dob");
      }
    } // Dob


    /// <summary>
    /// Gets or sets the gender.
    /// </summary>
    public Genders Gender
    {
      get { return m_study.Gender; }
      set
      {
        m_study.Gender = value;
        RaisePropertyChanged("Gender");
      }
    } // Gender


    /// <summary>
    /// Gets or sets the collection of series.
    /// </summary>
    public ObservableCollection<SeriesViewModel> Series
    {
      get { return m_series; }
      set
      {
        m_series = value;
        RaisePropertyChanged("Series");
      }
    } // Series


    /// <summary>
    /// Gets the FULL PATH to the directory.
    /// </summary>
    public string Url
    {
      get { return m_study.Url; }
    } // Url


    /// <summary>
    /// Gets or sets the staging area directory.
    /// </summary>
    public string StagingArea
    {
      get { return m_stagingArea; }
      set { m_stagingArea = value; }
    } // StagingArea


    /// <summary>
    /// Gets the FULL PATH to the directory.
    /// </summary>
    public string StagingUrl
    {
      get { return Path.Combine(StagingArea, m_study.Uid.Replace(".", "_")); }
    } // StagingUrl


    /// <summary>
    /// Gets the short path.
    /// </summary>
    public string StagingUrlShortPath
    {
      get { return PathUtilities.GetWindowsShortPathName(StagingUrl); }
    } // StagingUrlShortPath

    #endregion

    #region methods

    /// <summary>
    /// Serializes the study to the specified directory.  Filename is "study.xml"
    /// </summary>
    /// <param name="directory">The directory to serialize to.</param>
    public void Serialize(string directory)
    {
      m_study.Serialize(Path.Combine(directory, "study.xml"));
    } // Serialize( directory )

    #endregion

  } // class StudyViewModel
} // namespace Viatronix.Utilities.Transfer.ViewModels
