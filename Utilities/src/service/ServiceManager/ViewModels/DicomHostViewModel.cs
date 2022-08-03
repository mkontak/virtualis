using System;
using System.Windows;
using System.Collections.Generic;
using System.Collections.ObjectModel;

using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;

using Viatronix.Enterprise;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;

namespace Viatronix.Utilities.Service.ServiceManager.ViewModels
{
  /// <summary>
  /// Provides information about the host.
  /// </summary>
  public class DicomHostViewModel : ViewModelBase
  {

    #region fields

    /// <summary>
    /// The encapsulated host.
    /// </summary>
    private DicomHost m_dicomHost = null;

    /// <summary>
    /// The list of attributes.
    /// </summary>
    private ObservableCollection<DicomHostAttributeViewModel> m_attributes = new ObservableCollection<DicomHostAttributeViewModel>();

    /// <summary>
    /// The selected attribute.
    /// </summary>
    private DicomHostAttributeViewModel m_selectedAttribute = null;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public DicomHostViewModel()
    {
      m_dicomHost = new DicomHost();

      AddAttributeCommand = new RelayCommand(OnAddAttribute);
      ModifyAttributeCommand = new RelayCommand(OnModifyAttribute);
      DeleteAttributeCommand = new RelayCommand(OnDeleteAttribute);

      RaisePropertyChanged("Attributes");
      RaisePropertyChanged("HasAttributes");
    } // DicomHostViewModel()


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="dicomHost">The dicom host.</param>
    public DicomHostViewModel(DicomHost dicomHost)
    {
      m_dicomHost = dicomHost;
      foreach(KeyValuePair<string, string> attribute in dicomHost.Attributes)
      {
        Attributes.Add(new DicomHostAttributeViewModel(attribute.Key, attribute.Value));
      }

      AddAttributeCommand = new RelayCommand(OnAddAttribute);
      ModifyAttributeCommand = new RelayCommand(OnModifyAttribute);
      DeleteAttributeCommand = new RelayCommand(OnDeleteAttribute);

      RaisePropertyChanged("Attributes");
      RaisePropertyChanged("HasAttributes");
    } // DicomHostViewModel( dicomHost )

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the name.
    /// </summary>
    public string Name
    {
      get { return m_dicomHost.Name; }
      set
      {
        m_dicomHost.Name = value;
        RaisePropertyChanged("Name");
      }
    } // Name


    /// <summary>
    /// Gets or sets the server.
    /// </summary>
    public string Server
    {
      get { return m_dicomHost.Server; }
      set
      {
        m_dicomHost.Server = value;
        RaisePropertyChanged("Server");
      }
    } // Server


    /// <summary>
    /// Gets or sets the AE Title.
    /// </summary>
    public string AETitle
    {
      get { return m_dicomHost.AETitle; }
      set
      {
        m_dicomHost.AETitle = value;
        RaisePropertyChanged("AETitle");
      }
    } // AETitle


    /// <summary>
    /// Gets or sets the port.
    /// </summary>
    public uint Port
    {
      get { return m_dicomHost.Port; }
      set
      {
        m_dicomHost.Port = value;
        RaisePropertyChanged("Port");
      }
    } // Port


    /// <summary>
    /// Gets or sets if the query operation is enabled for this host.
    /// </summary>
    public bool IsQueryEnabled
    {
      get { return (m_dicomHost.Services & DicomServiceTypes.Query) == DicomServiceTypes.Query; }
      set
      {
        m_dicomHost.Services = m_dicomHost.Services | DicomServiceTypes.Query;
        RaisePropertyChanged("IsQueryEnabled");
      }
    } // IsQueryEnabled


    /// <summary>
    /// Gets or sets if the retrieve operation is enabled for this host.
    /// </summary>
    public bool IsRetrieveEnabled
    {
      get { return (m_dicomHost.Services & DicomServiceTypes.Retrieve) == DicomServiceTypes.Retrieve; }
      set
      {
        m_dicomHost.Services = m_dicomHost.Services | DicomServiceTypes.Retrieve;
        RaisePropertyChanged("IsRetrieveEnabled");
      }
    } // IsRetrieveEnabled


    /// <summary>
    /// Gets or sets if the store operation is enabled for this host.
    /// </summary>
    public bool IsStoreEnabled
    {
      get { return (m_dicomHost.Services & DicomServiceTypes.Store) == DicomServiceTypes.Store; }
      set
      {
        m_dicomHost.Services = m_dicomHost.Services | DicomServiceTypes.Store;
        RaisePropertyChanged("IsStoreEnabled");
      }
    } // IsStoreEnabled


    /// <summary>
    /// Gets or sets the attribute list.
    /// </summary>
    public ObservableCollection<DicomHostAttributeViewModel> Attributes
    {
      get { return m_attributes; }
      set
      {
        m_attributes = value;
        RaisePropertyChanged("Attributes");
        RaisePropertyChanged("HasAttributes");
      }
    } // Attributes


    /// <summary>
    /// Gets or sets the selected attribute.
    /// </summary>
    public DicomHostAttributeViewModel SelectedAttribute
    {
      get { return m_selectedAttribute; }
      set
      {
        m_selectedAttribute = value;
        RaisePropertyChanged("SelectedAttribute");
        RaisePropertyChanged("IsAttributeSelected");
      }
    } // SelectedAttribute


    /// <summary>
    /// Gets if an attribute was selected.
    /// </summary>
    public bool IsAttributeSelected
    {
      get { return m_selectedAttribute != null; }
    } // IsAttributeSelected


    /// <summary>
    /// Gets if the host has attributes.
    /// </summary>
    public string HasAttributes
    {
      get { return m_attributes.Count > 0 ? "Yes" : "No"; }
    } // HasAttributes


    /// <summary>
    /// Gets the dicom host.
    /// </summary>
    public DicomHost DicomHost
    {
      get { return m_dicomHost; }
    } // DicomHost

    #endregion

    #region commands

    /// <summary>
    /// Gets or sets the command to add an attribute.
    /// </summary>
    public RelayCommand AddAttributeCommand
    {
      get;
      private set;
    } // AddAttributeCommand


    /// <summary>
    /// Gets or sets the command to modify an attribute.
    /// </summary>
    public RelayCommand ModifyAttributeCommand
    {
      get;
      private set;
    } // ModifyAttributeCommand


    /// <summary>
    /// Gets or sets the command to delete an attribute.
    /// </summary>
    public RelayCommand DeleteAttributeCommand
    {
      get;
      private set;
    } // DeleteAttributeCommand

    #endregion

    #region methods

    /// <summary>
    /// Adds an attribute.
    /// </summary>
    /// <param name="attribute">The attribute.</param>
    public void AddAttribute(DicomHostAttributeViewModel attribute)
    {
      Attributes.Add(attribute);
      DicomHost.Attributes.Add(attribute.Name, attribute.Value);
    } // AddAttribute( attribute )


    /// <summary>
    /// Updates an attribute.
    /// </summary>
    /// <param name="attribute">The attribute.</param>
    public void UpdateAttribute(DicomHostAttributeViewModel attribute)
    {
      for(int i = 0; i < Attributes.Count; ++i)
      {
        if(Attributes[i].Name == attribute.Name)
        {
          Attributes[i].Value = attribute.Value;
        }
      }

      DicomHost.Attributes.Remove(attribute.Name);
      DicomHost.Attributes.Add(attribute.Name, attribute.Value);
    } // UpdateAttribute( attribute )


    /// <summary>
    /// Deletes an attribute.
    /// </summary>
    /// <param name="attribute">The attribute.</param>
    public void DeleteAttribute(DicomHostAttributeViewModel attribute)
    {
      for (int i = 0; i < Attributes.Count; ++i)
      {
        if (Attributes[i].Name == attribute.Name)
        {
          Attributes.RemoveAt(i);
        }
      }

      DicomHost.Attributes.Remove(attribute.Name);
    } // DeleteAttribute( attribute )


    /// <summary>
    /// Performs an insert of an attribute.
    /// </summary>
    private void OnAddAttribute()
    {
      DicomHostAttributeWindow dialog = new DicomHostAttributeWindow();

      dialog.EditingMode = EditingModes.Insert;
      dialog.DicomHost = this;
      dialog.Attribute = new DicomHostAttributeViewModel();

      dialog.ShowDialog();
    } // OnAddAttribute()


    /// <summary>
    /// Performs an insert of an attribute.
    /// </summary>
    private void OnModifyAttribute()
    {
      if (SelectedAttribute != null)
      {
        DicomHostAttributeWindow dialog = new DicomHostAttributeWindow();

        dialog.EditingMode = EditingModes.Update;
        dialog.DicomHost = this;
        dialog.Attribute = SelectedAttribute;

        dialog.ShowDialog();
      }
    } // OnModifyAttribute()


    /// <summary>
    /// Performs an insert of an attribute.
    /// </summary>
    private void OnDeleteAttribute()
    {
      if (SelectedAttribute != null)
      {
        if (MessageBox.Show("Are you sure you want to delete the selected Attribute.", "Delete Attribute?", MessageBoxButton.YesNo, MessageBoxImage.Question) == MessageBoxResult.Yes)
        {
          DeleteAttribute(SelectedAttribute);
          SelectedAttribute = null;
        }
      }
    } // OnDeleteAttribute()

    #endregion

  } // class DicomHostViewModel
} // namespace Viatronix.Utilities.Service.ServiceManager.ViewModels
