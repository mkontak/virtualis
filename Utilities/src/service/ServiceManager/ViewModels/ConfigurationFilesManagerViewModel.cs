using System;
using System.IO;
using System.Windows;
using System.Configuration;
using System.Collections.ObjectModel;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;

using Viatronix.Enterprise;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;

namespace Viatronix.Utilities.Service.ServiceManager.ViewModels
{
  /// <summary>
  /// View model to modify configuration files.
  /// </summary>
  public class ConfigurationFilesManagerViewModel : ViewModelBase
  {

    #region enums

    /// <summary>
    /// The modes of the tab.
    /// </summary>
    public enum EditingModes
    {
      Selection,
      Editing
    } // enum EditingModes

    #endregion

    #region fields

    /// <summary>
    /// The list of configuration files.
    /// </summary>
    private ObservableCollection<ConfigurationFileViewModel> m_configurationFiles = new ObservableCollection<ConfigurationFileViewModel>();

    /// <summary>
    /// The selected configuration file.
    /// </summary>
    private ConfigurationFileViewModel m_selectedConfigurationFile = null;

    /// <summary>
    /// The contents of the file.
    /// </summary>
    private string m_fileContents = string.Empty;

    /// <summary>
    /// A backup of the file contents to perform revert, etc.
    /// </summary>
    private string m_fileContentsBackup = string.Empty;

    /// <summary>
    /// Has a document been loaded?  Save.
    /// </summary>
    private EditingModes m_editingMode = EditingModes.Selection;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public ConfigurationFilesManagerViewModel()
    {
      List<ConfigurationFileViewModel> configurationFiles = (List<ConfigurationFileViewModel>)ConfigurationManager.GetSection("filesSection");
      List<SystemSetting> systemSettings = Global.GetSystemSettings(XmlBuilder.Create("systemsetting", new Dictionary<string, string>() {{ "subsystem", "system" }, {"expand", "1"}}));

      foreach(ConfigurationFileViewModel listFile in configurationFiles)
      {
        foreach(SystemSetting setting in systemSettings)
        {
          listFile.Filename = listFile.Filename.Replace(string.Format("$({0})", setting.Name), setting.Value);
        }

        ConfigurationFiles.Add(listFile);
      }
    } // ConfigurationFilesManagerViewModel()

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the list of available configuration files.
    /// </summary>
    public ObservableCollection<ConfigurationFileViewModel> ConfigurationFiles
    {
      get { return m_configurationFiles; }
      set
      {
        m_configurationFiles = value;
        RaisePropertyChanged("ConfigurationFiles");
      }
    } // ConfigurationFiles
    

    /// <summary>
    /// Gets or sets the selected configuration file.
    /// </summary>
    public ConfigurationFileViewModel SelectedConfigurationFile
    {
      get { return m_selectedConfigurationFile; }
      set
      {
        m_selectedConfigurationFile = value;
        RaisePropertyChanged("SelectedConfigurationFile");
      }
    } // SelectedConfigurationFile


    /// <summary>
    /// Gets or sets the file contents.
    /// </summary>
    public string FileContents
    {
      get { return m_fileContents; }
      set
      {
        m_fileContents = value;
        RaisePropertyChanged("FileContents");
      }
    } // FileContents
    

    /// <summary>
    /// Gets or sets the Editing Mode.
    /// </summary>
    public EditingModes EditingMode
    {
      get { return m_editingMode; }
      set
      {
        m_editingMode = value;
        RaisePropertyChanged("EditingMode");
      }
    } // EditingMode

    #endregion

  } // class ConfigurationFilesViewModel
} // namespace Viatronix.Utilities.Service.ServiceManager.ViewModels
