using System;
using System.IO;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using GalaSoft.MvvmLight;

using Viatronix.Utilities.IO;
using Viatronix.Enterprise.Entities;

namespace Viatronix.Utilities.Transfer.ViewModels
{
  /// <summary>
  /// The series view model base.
  /// </summary>
  public class SeriesViewModel : ViewModelBase
  {

    #region fields

    /// <summary>
    /// The series this object is based on.
    /// </summary>
    private Series m_series = null;

    /// <summary>
    /// The list of transfer items.
    /// </summary>
    private ObservableCollection<TransferItemViewModel> m_transferItems = new ObservableCollection<TransferItemViewModel>();

    /// <summary>
    /// Set during the staging area creation.
    /// </summary>
    private string m_stagingArea = string.Empty;

    /// <summary>
    /// The series directory.
    /// </summary>
    private string m_seriesDirectory = string.Empty;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="series">The series this object is based on.</param>
    public SeriesViewModel(Series series)
    {
      m_series = series;
      m_seriesDirectory = string.Format("{0}_{1}", m_series.Type.ToString().ToUpper(), DateTime.Now.ToString("yyyyMMddhhmmssffff")); ;
    } // SeriesViewModel( series )

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the series uid.
    /// </summary>
    public string Uid
    {
      get { return m_series.Uid; }
      private set
      {
        m_series.Uid = value;
        RaisePropertyChanged("Uid");
      }
    } // Uid


    /// <summary>
    /// Gets or sets the series type.
    /// </summary>
    public SeriesTypes Type
    {
      get { return m_series.Type; }
      private set
      {
        m_series.Type = value;
        RaisePropertyChanged("Type");
      }
    } // Type


    /// <summary>
    /// Gets or sets the series description.
    /// </summary>
    public string Description
    {
      get { return m_series.Description; }
      set
      {
        m_series.Description = value;
        RaisePropertyChanged("Description");
      }
    } // Description


    /// <summary>
    /// Gets or sets the patient orientation.
    /// </summary>
    public string PatientOrientation
    {
      get { return m_series.PatientOrientation; }
      private set
      {
        m_series.PatientOrientation = value;
        RaisePropertyChanged("PatientOrientation");
      }
    } // PatientOrientation


    /// <summary>
    /// Gets or sets the application of the series.
    /// </summary>
    public string Application
    {
      get { return m_series.Application; }
      private set
      {
        m_series.Application = value;
        RaisePropertyChanged("Application");
      }
    } // Application


    /// <summary>
    /// Gets the series.
    /// </summary>
    public Series Series
    {
      get { return m_series; }
    } // Series


    /// <summary>
    /// Gets or sets the list of transfer items that will this series can transfer.
    /// </summary>
    public ObservableCollection<TransferItemViewModel> TransferItems
    {
      get { return m_transferItems; }
      set
      {
        m_transferItems = value;
        RaisePropertyChanged("TransferItems");
      }
    } // TransferItems


    /// <summary>
    /// Gets if this series is selected for transfer.
    /// </summary>
    public bool IsEligibleForTransfer
    {
      get
      {
        foreach(TransferItemViewModel item in TransferItems)
        {
          if(item.IsSelected)
          {
            return true;
          }
        }

        return false;
      }
    } // IsEligibleForTransfer


    /// <summary>
    /// Gets if this series is required for the transfer.
    /// </summary>
    public bool IsRequiredForTransfer
    {
      get
      {
        foreach(TransferItemViewModel item in TransferItems)
        {
          if(item.IsRequired)
          {
            return true;
          }
        }

        return false;
      }
    } // IsRequiredForTransfer


    /// <summary>
    /// Gets the url of the directory.
    /// </summary>
    public string Url
    {
      get { return m_series.Url; }
    } // Url

    
    /// <summary>
    /// Gets or sets the staging area directory being used for the transfer prep.
    /// </summary>
    public string StagingArea
    {
      get { return m_stagingArea; }
      set { m_stagingArea = value; }
    } // StagingArea


    /// <summary>
    /// Gets or sets the series directory.
    /// </summary>
    public string SeriesDirectory
    {
      get { return m_seriesDirectory; }
      set { m_seriesDirectory = value; }
    } // SeriesDirectory


    /// <summary>
    /// Gets the staging url.
    /// </summary>
    public string StagingUrl
    {
      get { return Path.Combine(StagingArea, m_series.StudyUid.Replace('.', '_'), m_seriesDirectory.Replace('.', '_')); }
    } // StagingUrl


    /// <summary>
    /// Gets the short path of the staging url.
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
      m_series.Serialize(Path.Combine(directory, "series.xml"));
    } // Serialize( directory )


    /// <summary>
    /// Retrieves matching files from the staging area.
    /// </summary>
    /// <param name="itemName">The item name.</param>
    /// <returns>The list of files prepended with the staging area directory.</returns>
    public string[] RetrieveFilesFromStaging(string itemName)
    {
      List<string> files = new List<string>();

      TransferItemViewModel item = TransferItems.Find(itemName);
      if (item != null)
      {
        if (item.IsSelected)
        {
          foreach (string file in item.RetrieveFiles(StagingUrl))
          {
            files.Add(file);
          }
        }
        else if (item.IsRequired)
        {
          string[] itemFiles = item.RetrieveFiles(StagingUrl);
          if (itemFiles.Length > 0)
          {
            files.Add(itemFiles[0]);
          }
        }
      }

      return files.ToArray();
    } // RetrieveFilesFromStaging( itemName )

    #endregion

  } // class SeriesViewModel
} // namespace Viatronix.Utilities.Transfer.ViewModels
