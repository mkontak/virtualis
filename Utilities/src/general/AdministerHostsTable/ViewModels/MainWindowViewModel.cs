// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: HostTable.cpp
//
// Description: 
//
// Owner: Kyle R Chalupa (mailto: kchalupa@viatronix.com)

using System;
using System.IO;
using System.Xml;
using System.Windows;
using System.Threading;
using System.Windows.Controls;
using System.Configuration;
using System.Collections.ObjectModel;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;

using Viatronix.Dicom;
using Viatronix.Utilities.General.AdministerHostsTable.ValidationRules;

namespace Viatronix.Utilities.General.AdministerHostsTable.ViewModels
{

  #region delegates

  /// <summary>
  /// Delegate to inform that the state has changed.
  /// </summary>
  /// <typeparam name="T">The target type.</typeparam>
  /// <param name="sender">The origin of the event.</param>
  /// <param name="oldValue">The original value.</param>
  /// <param name="newValue">The new value.</param>
  public delegate void StateChanged<T>(object sender, T oldValue, T newValue);

  #endregion

  /// <summary>
  /// The hosts manager.
  /// </summary>
  public class MainWindowViewModel : ViewModelBase
  {

    #region events

    /// <summary>
    /// Notifies that the editing changed handler.
    /// </summary>
    public event StateChanged<bool> EditingStateChanged;

    #endregion

    #region fields

    /// <summary>
    /// The list of hosts.
    /// </summary>
    private ObservableCollection<HostViewModel> m_hosts = new ObservableCollection<HostViewModel>();

    /// <summary>
    /// The modified flag.
    /// </summary>
    private bool m_modified = false;

    /// <summary>
    /// Determines if editing is enabled.
    /// </summary>
    private bool m_editing = false;

    /// <summary>
    /// Determines if editing is disabled.
    /// </summary>
    private bool m_notEditing = true;

    /// <summary>
    /// The selected host.
    /// </summary>
    private HostViewModel m_selectedHost = null;

    /// <summary>
    /// Is a host selected?
    /// </summary>
    private bool m_isHostSelected = false;

    /// <summary>
    /// The current instance in the collection.
    /// </summary>
    private HostViewModel m_currentInstance = null;

    /// <summary>
    /// The temporary host is used for the edit process.  If cancel is clicked it is discarded if ok then it replaces the previous copy.
    /// </summary>
    private HostViewModel m_temporaryHost = null;

    /// <summary>
    /// The selected attribute.
    /// </summary>
    private AttributeViewModel m_selectedAttribute = null;

    /// <summary>
    /// Is an attribute selected.
    /// </summary>
    private bool m_isAttributeSelected = false;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public MainWindowViewModel()
    {
      AddHostCommand = new RelayCommand(AddHost);
      ModifyHostCommand = new RelayCommand(ModifyHost);
      DeleteHostCommand = new RelayCommand(DeleteHost);
      SaveCommand = new RelayCommand(SaveHostTable);
      ReloadCommand = new RelayCommand(ReloadHostTable);
      AddAttributeCommand = new RelayCommand(AddAttribute);
      ModifyAttributeCommand = new RelayCommand(ModifyAttribute);
      DeleteAttributeCommand = new RelayCommand(RemoveAttribute);
      OKCommand = new RelayCommand(SaveHostChanges);
      CancelCommand = new RelayCommand(DiscardHostChanges);
    } // MainWindowViewModel()

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the modified flag.
    /// </summary>
    public bool Modified
    {
      get { return m_modified; }
      set
      {
        m_modified = value;
        RaisePropertyChanged("Modified");
      }
    } // Modified


    /// <summary>
    /// Determines if the host fields should be shown.
    /// </summary>
    public bool Editing
    {
      get { return m_editing; }
      set
      {
        bool original = m_editing;

        m_editing = value;
        RaisePropertyChanged("Editing");
        NotEditing = !m_editing;

        if (EditingStateChanged != null)
        {
          EditingStateChanged(this, original, value);
        }
      }
    } // Editing


    /// <summary>
    /// Gets if editing is disabled.
    /// </summary>
    public bool NotEditing
    {
      get { return m_notEditing; }
      private set
      {
        m_notEditing = value;
        RaisePropertyChanged("NotEditing");
      }
    } // NotEditing


    /// <summary>
    /// Gets or sets the list of hosts.
    /// </summary>
    public ObservableCollection<HostViewModel> Hosts
    {
      get { return m_hosts; }
      set
      {
        m_hosts = value;
        RaisePropertyChanged("Hosts");
      }
    } // Hosts


    /// <summary>
    /// Gets or sets the if a host was selected.
    /// </summary>
    public bool IsHostSelected
    {
      get { return m_isHostSelected; }
      set
      {
        m_isHostSelected = value;
        RaisePropertyChanged("IsHostSelected");
      }
    } // IsHostSelected


    /// <summary>
    /// Gets or sets the selected host.
    /// </summary>
    public HostViewModel SelectedHost
    {
      get { return m_selectedHost; }
      set
      {
        m_selectedHost = value;
        IsHostSelected = m_selectedHost != null;
        RaisePropertyChanged("SelectedHost");
      }
    } // SelectedHost


    /// <summary>
    /// Gets or sets the temporary host for the modify and add.
    /// </summary>
    public HostViewModel TemporaryHost
    {
      get { return m_temporaryHost; }
      set
      {
        m_temporaryHost = value;
        RaisePropertyChanged("TemporaryHost");
      }
    } // TemporaryHost


    /// <summary>
    /// Gets or sets if the attribute is selected.
    /// </summary>
    public bool IsAttributeSelected
    {
      get { return m_isAttributeSelected; }
      set
      {
        m_isAttributeSelected = value;
        RaisePropertyChanged("IsAttributeSelected");
      }
    } // IsAttributeSelected


    /// <summary>
    /// Gets or sets the selected attribute.
    /// </summary>
    public AttributeViewModel SelectedAttribute
    {
      get { return m_selectedAttribute; }
      set
      {
        m_selectedAttribute = value;
        IsAttributeSelected = value != null;
        RaisePropertyChanged("SelectedAttribute");
      }
    } // SelectedAttribute

    #endregion

    #region commands

    /// <summary>
    /// Gets or sets the add host command.
    /// </summary>
    public RelayCommand AddHostCommand
    {
      get;
      private set;
    } // AddHostCommand


    /// <summary>
    /// Gets or sets the modify host command.
    /// </summary>
    public RelayCommand ModifyHostCommand
    {
      get;
      private set;
    } // ModifyHostCommand


    /// <summary>
    /// Gets or sets the delete host command.
    /// </summary>
    public RelayCommand DeleteHostCommand
    {
      get;
      private set;
    } // DeleteHostCommand


    /// <summary>
    /// Gets or sets the save command.
    /// </summary>
    public RelayCommand SaveCommand
    {
      get;
      private set;
    } // SaveCommand


    /// <summary>
    /// Gets or sets the reload command.
    /// </summary>
    public RelayCommand ReloadCommand
    {
      get;
      private set;
    } // ReloadCommand


    /// <summary>
    /// Gets or sets the add attribute command.
    /// </summary>
    public RelayCommand AddAttributeCommand
    {
      get;
      private set;
    } // AddAttributeCommand


    /// <summary>
    /// Gets or sets the modify attribute command.
    /// </summary>
    public RelayCommand ModifyAttributeCommand
    {
      get;
      private set;
    } // ModifyAttributeCommand


    /// <summary>
    /// Gets or sets the delete attribute command.
    /// </summary>
    public RelayCommand DeleteAttributeCommand
    {
      get;
      private set;
    } // DeleteAttributeCommand


    /// <summary>
    /// Gets or sets the ok command.
    /// </summary>
    public RelayCommand OKCommand
    {
      get;
      private set;
    } // OKCommand


    /// <summary>
    /// Gets or sets te cancel command.
    /// </summary>
    public RelayCommand CancelCommand
    {
      get;
      private set;
    } // CancelCommand

    #endregion

    #region methods

    /// <summary>
    /// Initializes the host manager when the window is fully ready.
    /// </summary>
    public void Initialize()
    {
      Editing = false;
      Modified = false;
      IsHostSelected = false;
      Hosts.Clear();

      HostTable.Create();
      foreach (Host host in HostTable.CurrentHostTable.GetHosts(ServiceTypes.ALL))
      {
        Hosts.Add(HostViewModel.Create(host));
      }
    } // Initialize()


    /// <summary>
    /// Adds a host to the table.
    /// </summary>
    private void AddHost()
    {
      m_currentInstance = null;
      TemporaryHost = HostViewModel.Create();

      Editing = true;
    } // AddHost


    /// <summary>
    /// Modifies the selected host in the table.
    /// </summary>
    private void ModifyHost()
    {
      if (SelectedHost != null)
      {
        m_currentInstance = SelectedHost;
        TemporaryHost = (HostViewModel)m_currentInstance.Clone();

        Editing = true;
      }
    } // ModifyHost()


    /// <summary>
    /// Deletes the selected host from the table.
    /// </summary>
    private void DeleteHost()
    {
      if (SelectedHost != null && MessageBox.Show(Messages.GetMessage("VX007"), Messages.GetMessage("VX008"), MessageBoxButton.YesNo) == MessageBoxResult.Yes)
      {
        // Update the backing store.
        HostTable.CurrentHostTable.RemoveHost(SelectedHost.Name);

        // Update the GUI.
        Hosts.Remove(SelectedHost);
        Modified = true;
      }
    } // DeleteHost()


    /// <summary>
    /// Saves the host table.
    /// </summary>
    private void SaveHostTable()
    {
      Modified = false;
      HostTable.CurrentHostTable.Save();
    } // SaveHostTable()


    /// <summary>
    /// Reloads the host table.
    /// </summary>
    private void ReloadHostTable()
    {
      if (MessageBox.Show(Messages.GetMessage("VX011"), Messages.GetMessage("VX012"), MessageBoxButton.YesNo, MessageBoxImage.Question) == MessageBoxResult.Yes)
      {
        Initialize();
      }
    } // ReloadHostTable()


    /// <summary>
    /// Adds a attribute to the host.
    /// </summary>
    public void AddAttribute()
    {
      AttributeViewModel temporaryInstance = AttributeViewModel.Create();

      AttributeWindow attributeDialog = new AttributeWindow();
      attributeDialog.DataContext = temporaryInstance;
      attributeDialog.Tag = new KeyValuePair<string, ObservableCollection<AttributeViewModel>>(string.Empty, TemporaryHost.Attributes);

      if (attributeDialog.ShowDialog() == true)
      {
        TemporaryHost.AddAttribute(temporaryInstance);
      }
    } // AddAttribute()


    /// <summary>
    /// Modifies a host's attribute.
    /// </summary>
    public void ModifyAttribute()
    {
      if (SelectedAttribute != null)
      {
        AttributeViewModel temporaryAttribute = (AttributeViewModel)SelectedAttribute.Clone();

        AttributeWindow attributeDialog = new AttributeWindow();
        attributeDialog.DataContext = temporaryAttribute;
        attributeDialog.Tag = new KeyValuePair<string, ObservableCollection<AttributeViewModel>>(SelectedAttribute.Name, TemporaryHost.Attributes);

        if (attributeDialog.ShowDialog() == true)
        {
          TemporaryHost.ModifyAttribute(temporaryAttribute, SelectedAttribute.Name); 
        }
      }
    } // ModifyAttribute()


    /// <summary>
    /// Deletes a host's attribute.
    /// </summary>
    public void RemoveAttribute()
    {
      if (SelectedAttribute != null && MessageBox.Show(Messages.GetMessage("VX009"), 
        Messages.GetMessage("VX010"), MessageBoxButton.YesNo) == MessageBoxResult.Yes)
      {
        TemporaryHost.RemoveAttribute(SelectedAttribute);
      }
    } // DeleteAttribute()


    /// <summary>
    /// Saves the changes made to the host.  Either adds the host or commits the changes made to the existing host.
    /// </summary>
    private void SaveHostChanges()
    {
      FreeformValidationRule freeformRule = new FreeformValidationRule();
      HostNameValidationRule hostnameRule = new HostNameValidationRule();
      PortValidationRule portRule = new PortValidationRule();

      ValidationResult result = freeformRule.Validate(TemporaryHost.Name, Thread.CurrentThread.CurrentCulture);
      if (!result.IsValid)
      {
        // Display message that name validation failed.
        MessageBox.Show(Messages.GetMessage("VX017") + result.ErrorContent.ToString(), Messages.GetMessage("VX018"));
        return;
      }

      result = hostnameRule.Validate(TemporaryHost.Server, Thread.CurrentThread.CurrentCulture);
      if (!result.IsValid)
      {
        // Display message that server validation failed.
        MessageBox.Show(Messages.GetMessage("VX019") + result.ErrorContent.ToString(), Messages.GetMessage("VX020"));
        return;
      }

      result = freeformRule.Validate(TemporaryHost.AETitle, Thread.CurrentThread.CurrentCulture);
      if (!result.IsValid)
      {
        // Display message that aetitle validation failed.
        MessageBox.Show(Messages.GetMessage("VX021") + result.ErrorContent.ToString(), Messages.GetMessage("VX022"));
        return;
      }

      result = portRule.Validate(TemporaryHost.Port, Thread.CurrentThread.CurrentCulture);
      if(!result.IsValid)
      {
        // Display message that port validation failed.
        MessageBox.Show(Messages.GetMessage("VX023") + result.ErrorContent.ToString(), Messages.GetMessage("VX024"));
        return;
      }


      if (m_currentInstance != null) // Update the current instance with the modified value.
      {
        foreach (HostViewModel viewModel in Hosts)
        {
          if (m_currentInstance.Name != viewModel.Name && TemporaryHost.Name.ToLower() == viewModel.Name.ToLower())
          {
            // Host already exists with that name.
            MessageBox.Show(Messages.GetMessage("VX013"), Messages.GetMessage("VX014"));
            return;
          }
        }

        // Update the backing store.
        HostTable.CurrentHostTable.RemoveHost(m_currentInstance.Name);
        HostTable.CurrentHostTable.AddHost(TemporaryHost.GetInternalObject());

        // Update the GUI portion.
        for (int i = Hosts.Count - 1; i > -1; --i)
        {
          if (Hosts[i].Name == m_currentInstance.Name && Hosts[i].Server == m_currentInstance.Server && Hosts[i].AETitle == m_currentInstance.AETitle && Hosts[i].Port == m_currentInstance.Port)
          {
            Hosts.RemoveAt(i);
            Hosts.Insert(i, TemporaryHost);
          }
        }

        m_currentInstance = null;
        TemporaryHost = null;
      }
      else // Add the instance.
      {
        foreach (HostViewModel viewModel in Hosts)
        {
          if (viewModel.Name.ToLower() == TemporaryHost.Name.ToLower())
          {
            // Host already exists with that name.
            MessageBox.Show(Messages.GetMessage("VX013"), Messages.GetMessage("VX014"));
            return;
          }
        }

        HostTable.CurrentHostTable.AddHost(TemporaryHost.GetInternalObject());
        Hosts.Add(TemporaryHost);
        TemporaryHost = null;
      }

      Modified = true;
      Editing = false;
    } // SaveHostChanges()


    /// <summary>
    /// Discards the changes made to the host.
    /// </summary>
    private void DiscardHostChanges()
    {
      Editing = false;

      TemporaryHost = null;
      m_currentInstance = null;
    } // DiscardHostChanges()

    #endregion

  } // class MainWindowViewModel
} // namespace Viatronix.Utilities.General.AdministerHostsTable.ViewModels
