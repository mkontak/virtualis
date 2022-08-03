using System;
using System.IO;
using System.Windows;
using System.Windows.Forms;
using System.Configuration;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;

using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;

using Viatronix.Utilities;
using Viatronix.Licensing;

namespace Viatronix.Utilities.Licensing.LicenseGenerator.ViewModels
{
  /// <summary>
  /// This is a view model for the main window.
  /// </summary>
  public class GeneratorViewModel : ViewModelBase
  {

    #region fields

    /// <summary>
    /// The license file formatter.
    /// </summary>
    private LicenseFileFormatter m_formatter = new LicenseFileFormatter();

    /// <summary>
    /// The available license types.
    /// </summary>
    private ObservableCollection<string> m_licenseTypes = new ObservableCollection<string>();

    /// <summary>
    /// The default filename to save to.
    /// </summary>
    private string m_defaultFilename = string.Empty;

    /// <summary>
    /// The license count.
    /// </summary>
    private string m_numberLicenses = string.Empty;

    /// <summary>
    /// The license expiration in days.
    /// </summary>
    private string m_licenseDuration = string.Empty;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public GeneratorViewModel()
    {
      SaveCommand = new RelayCommand(Save);

      try
      {
        SetupLicenseTypes();

        SystemTimeToLiveDate = Convert.ToDateTime(ConfigurationManager.AppSettings["SystemTimeToLiveDate"]);
        NumberLicenses  = ConfigurationManager.AppSettings["NumberOfLicenses"];
        LicenseDuration = ConfigurationManager.AppSettings["LicenseDuration"];
      }
      catch (Exception)
      {
      }
    } // GeneratorViewModel()

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the system id.
    /// </summary>
    public string SystemId
    {
      get { return m_formatter.SystemId; }
      set
      {
        m_formatter.SystemId = value;
        RaisePropertyChanged("SystemId");
      }
    } // SystemId


    /// <summary>
    /// Gets or sets the comments.
    /// </summary>
    public string Comments
    {
      get { return m_formatter.Comments; }
      set
      {
        m_formatter.Comments = value;
        RaisePropertyChanged("Comments");
      }
    } // Comments


    /// <summary>
    /// Gets or sets whether we should reset the database expiration date.
    /// </summary>
    public bool ShouldResetSystemTimeToLiveDate
    {
      get { return m_formatter.ShouldResetSystemTimeToLiveDate; }
      set
      {
        m_formatter.ShouldResetSystemTimeToLiveDate = value;
        RaisePropertyChanged("ShouldResetSystemTimeToLiveDate");
      }
    } // ShouldResetSystemTimeToLiveDate


    /// <summary>
    /// Gets or sets the system time to live date.
    /// </summary>
    public DateTime SystemTimeToLiveDate
    {
      get { return m_formatter.SystemTimeToLiveDate; }
      set
      {
        m_formatter.SystemTimeToLiveDate = value;
        RaisePropertyChanged("SystemTimeToLiveDate");
      }
    } // SystemTimeToLiveDate


    /// <summary>
    /// Gets or sets whether or not we should install licenses.
    /// </summary>
    public bool ShouldInstallLicenses
    {
      get { return m_formatter.ShouldInstallLicenses; }
      set
      {
        m_formatter.ShouldInstallLicenses = value;

        if (!value)
        {
          IssuedBy = "";
          NumberLicenses = "";
          LicenseDuration = "";
        }

        RaisePropertyChanged("ShouldInstallLicenses");
      }
    } // ShouldInstallLicenses


    /// <summary>
    /// Gets or sets the issued by.
    /// </summary>
    public string IssuedBy
    {
      get { return m_formatter.IssuedBy; }
      set
      {
        m_formatter.IssuedBy = value;
        RaisePropertyChanged("IssuedBy");
      }
    } // IssuedBy


    /// <summary>
    /// Gets or sets the number of licenses.
    /// </summary>
    public string NumberLicenses
    {
      get { return m_numberLicenses; }
      set
      {
        m_numberLicenses = value;
        RaisePropertyChanged("NumberLicenses");
      }
    } // NumberLicenses


    /// <summary>
    /// Gets or sets the license duration in days.
    /// </summary>
    public string LicenseDuration
    {
      get { return m_licenseDuration; }
      set
      {
        m_licenseDuration = value;
        RaisePropertyChanged("LicenseDuration");
      }
    } // LicenseDuration


    /// <summary>
    /// Gets or sets the license type.
    /// </summary>
    public string LicenseType
    {
      get { return m_formatter.LicenseType; }
      set
      {
        m_formatter.LicenseType = value;
        RaisePropertyChanged("LicenseType");
      }
    } // LicenseType


    /// <summary>
    /// Gets or sets the available license types.
    /// </summary>
    public ObservableCollection<string> LicenseTypes
    {
      get { return m_licenseTypes; }
      set
      {
        m_licenseTypes = value;
        RaisePropertyChanged("LicenseTypes");
      }
    } // LicenseTypes

    #endregion

    #region commands

    /// <summary>
    /// Gets or sets the save command.
    /// </summary>
    public RelayCommand SaveCommand
    {
      get;
      private set;
    } // SaveCommand

    #endregion

    #region methods

    /// <summary>
    /// Sets up the license types.
    /// </summary>
    private void SetupLicenseTypes()
    {
      string types = ConfigurationManager.AppSettings["LicenseTypes"];
      string defaultType = ConfigurationManager.AppSettings["DefaultLicenseType"];

      bool found = false;
      foreach (string type in types.Split(new char[] { '|' }, StringSplitOptions.RemoveEmptyEntries))
      {
        m_licenseTypes.Add(type);
        if (defaultType.ToLower() == type.ToLower())
        {
          found = true;
        }
      }

      if (found)
      {
        m_formatter.LicenseType = defaultType;
      }
      else
      {
        m_formatter.LicenseType = m_licenseTypes.Count > 0 ? m_licenseTypes[0] : string.Empty;
      }
    } // SetupLicenseTypes()


    /// <summary>
    /// Saves the license file.
    /// </summary>
    private void Save()
    {
      if (!m_formatter.ShouldResetSystemTimeToLiveDate && !m_formatter.ShouldInstallLicenses)
      {
        System.Windows.MessageBox.Show(Messages.GetMessage("VX023"), Messages.GetMessage("VX024"), MessageBoxButton.OK, MessageBoxImage.Warning);
        return;
      }

      if (m_formatter.SystemId == string.Empty)
      {
        System.Windows.MessageBox.Show(Messages.GetMessage("VX003"), Messages.GetMessage("VX004"), MessageBoxButton.OK, MessageBoxImage.Warning);
        return;
      }

      if (m_formatter.ShouldResetSystemTimeToLiveDate)
      {
        if (m_formatter.SystemTimeToLiveDate.Date < DateTime.Now.Date)
        {
          System.Windows.MessageBox.Show(Messages.GetMessage("VX001"), Messages.GetMessage("VX002"), MessageBoxButton.OK, MessageBoxImage.Warning);
          return;
        }
      }

      if (m_formatter.ShouldInstallLicenses)
      {
        if (m_formatter.IssuedBy == string.Empty)
        {
          System.Windows.MessageBox.Show(Messages.GetMessage("VX005"), Messages.GetMessage("VX006"), MessageBoxButton.OK, MessageBoxImage.Warning);
          return;
        }

        if (NumberLicenses == string.Empty)
        {
          System.Windows.MessageBox.Show(Messages.GetMessage("VX007"), Messages.GetMessage("VX008"), MessageBoxButton.OK, MessageBoxImage.Warning);
          return;
        }

        try
        {
          m_formatter.NumberLicenses = Convert.ToInt32(NumberLicenses);

          if (m_formatter.NumberLicenses < 1)
          {
            System.Windows.MessageBox.Show(Messages.GetMessage("VX009"), Messages.GetMessage("VX010"), MessageBoxButton.OK, MessageBoxImage.Warning);
            return;
          }
        }
        catch (Exception)
        {
          System.Windows.MessageBox.Show(Messages.GetMessage("VX011"), Messages.GetMessage("VX012"), MessageBoxButton.OK, MessageBoxImage.Warning);
          return;
        }

        if (LicenseDuration == string.Empty)
        {
          System.Windows.MessageBox.Show(Messages.GetMessage("VX013"), Messages.GetMessage("VX014"), MessageBoxButton.OK, MessageBoxImage.Warning);
          return;
        }

        try
        {
          m_formatter.LicenseDuration = Convert.ToInt32(LicenseDuration);
          if (m_formatter.LicenseDuration < 1)
          {
            System.Windows.MessageBox.Show(Messages.GetMessage("VX015"), Messages.GetMessage("VX016"), MessageBoxButton.OK, MessageBoxImage.Warning);
            return;
          }
        }
        catch (Exception)
        {
          System.Windows.MessageBox.Show(Messages.GetMessage("VX017"), Messages.GetMessage("VX018"), MessageBoxButton.OK, MessageBoxImage.Warning);
          return;
        }
      }

      string filename = ConfigurationManager.AppSettings["DefaultLicenseFile"];

      SaveFileDialog dialog = new SaveFileDialog();
      dialog.InitialDirectory = Path.GetDirectoryName(filename);
      dialog.FileName = filename;
      dialog.Filter = "All Files (*.*)|*.*";

      if (dialog.ShowDialog() == DialogResult.OK)
      {
        try
        {
          using (TextWriter writer = new StreamWriter(dialog.FileName))
          {
            writer.WriteLine(Viatronix.Encryption.Crypto.Encrypt(m_formatter.ToFormattedString()));
          }

          System.Windows.MessageBox.Show(Messages.GetMessage("VX019"), Messages.GetMessage("VX020"), MessageBoxButton.OK, MessageBoxImage.Information);
        }
        catch (Exception e)
        {
          System.Windows.MessageBox.Show(string.Format(Messages.GetMessage("VX021"), e.Message), Messages.GetMessage("VX022"), MessageBoxButton.OK, MessageBoxImage.Error);
        }
      }
    } // Save()

    #endregion

  } // class GeneratorViewModel
} // namespace Viatronix.Utilities.Licensing.LicenseGenerator.ViewModels
