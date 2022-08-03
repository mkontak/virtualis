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
using GalaSoft.MvvmLight.Threading;

using Viatronix.Dicom;

using Viatronix.Utilities.Dicom.PACSViewer.Models;

namespace Viatronix.Utilities.Dicom.PACSViewer.ViewModels
{
  /// <summary>
  /// The main view model.
  /// </summary>
  public class PacsQueryViewModel : ViewModelBase
  {

    #region fields

    /// <summary>
    /// Determines whether the query is not active.
    /// </summary>
    private bool m_notActive = true;

    /// <summary>
    /// The model.
    /// </summary>
    private PacsQueryModel m_model = new PacsQueryModel();

    /// <summary>
    /// The view model for the parameters.
    /// </summary>
    private SearchParametersViewModel m_parametersViewModel = new SearchParametersViewModel();

    /// <summary>
    /// The selected index.
    /// </summary>
    private Host m_selectedConnection = null;

    /// <summary>
    /// The list of dicom server.
    /// </summary>
    private ObservableCollection<Host> m_dicomServer = new ObservableCollection<Host>();

    /// <summary>
    /// The list of records retrieved from the pacs.
    /// </summary>
    private ObservableCollection<PacsRecordViewModel> m_records = new ObservableCollection<PacsRecordViewModel>();

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public PacsQueryViewModel()
    {
      DispatcherHelper.Initialize();

      m_model.Started   += new ClientEventHandler(OnQueryStartedHandler);
      m_model.Progress  += new ClientEventHandler(OnQueryProgressHandler);
      m_model.Completed += new ClientEventHandler(OnQueryCompletedHandler);
      m_model.Failed    += new ClientEventHandler(OnQueryFailedHandler);

      List<Host> hosts = new List<Host>();
      try
      {
        // Get all hosts that can be quere id
        hosts = HostTable.CurrentHostTable.GetHosts(ServiceTypes.QUERY);
      }
      catch ( Exception e )
      {
        Logging.Log.Error("Failed to get host table : " + e.Message, "PacsQueryViewModel", "PacsQueryViewModel");
      }

      //Connections = (ObservableCollection<ServerConnectionModel>)ConfigurationManager.GetSection("serverConnections");
      if (hosts.Count > 0)
      {
        foreach (Host host in hosts)
        {
          Servers.Add(host);
        }
        SelectedConnection = Servers[0];
      }

      StandardQueryCommand = new RelayCommand(PerformStandardQuery, () => true);
      ServiceQueryCommand = new RelayCommand(PerformServiceQuery, () => true);
    } // PacsQueryViewModel()

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets whether we're not active.
    /// </summary>
    public bool NotActive
    {
      get { return m_notActive; }
      set
      {
        m_notActive = value;
        RaisePropertyChanged("NotActive");
      }
    } // NotActive


    /// <summary>
    /// Gets or sets the search parameters.
    /// </summary>
    public SearchParametersViewModel Parameters
    {
      get { return m_parametersViewModel; }
      set 
      { 
        m_parametersViewModel = value;
        RaisePropertyChanged("Parameters");
      }
    } // Parameters


    /// <summary>
    /// Gets or sets the selected item.
    /// </summary>
    public Host SelectedConnection
    {
      get { return m_selectedConnection; }
      set
      {
        m_selectedConnection = value;
        RaisePropertyChanged("SelectedConnection");
      }
    } // SelectedConnection


    /// <summary>
    /// Gets or sets the list of connections.
    /// </summary>
    public ObservableCollection<Host> Servers
    {
      get { return m_dicomServer; }
      set
      {
        m_dicomServer = value;
        RaisePropertyChanged("Servers");
      }
    } // Connections


    /// <summary>
    /// The retireved records from the pacs.
    /// </summary>
    public ObservableCollection<PacsRecordViewModel> Records
    {
      get { return m_records; }
      set
      {
        m_records = value;
        RaisePropertyChanged("Records");
      }
    } // Records

    #endregion

    #region commands

    /// <summary>
    /// The command to query the pacs server.
    /// </summary>
    public RelayCommand StandardQueryCommand
    {
      get;
      private set;
    } // StandardQueryCommand


    /// <summary>
    /// The command to query the pacs server.
    /// </summary>
    public RelayCommand ServiceQueryCommand
    {
      get;
      private set;
    } // ServiceQueryCommand

    #endregion

    #region methods

    /// <summary>
    /// Performs the service query.
    /// </summary>
    public void PerformStandardQuery()
    {
      //if (Parameters.PatientName != string.Empty)
      //{
      //  Parameters.GetModel().AddPatientName(Parameters.PatientName);
      //}

      //Parameters.GetModel().AddModality(Parameters.SelectedModality.Id);
      m_model.PerformQuery(m_selectedConnection, m_parametersViewModel.GetModel());
    } // PerformStandardQuery()



    /// <summary>
    /// Performs the service query.
    /// </summary>
    public void PerformServiceQuery()
    {
      m_model.PerformQuery(m_selectedConnection, m_parametersViewModel.GetModel());
    } // PerformServiceQuery()

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the query model's started event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnQueryStartedHandler(object sender, ClientEventArgs args)
    {
      DispatcherHelper.CheckBeginInvokeOnUI(() => NotActive = false);
    } // OnQueryStartedHandler( sender, args )


    /// <summary>
    /// Handles the query model's progress event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnQueryProgressHandler(object sender, ClientEventArgs args)
    {
      PacsRecordViewModel record = new PacsRecordViewModel();
      record.LoadXml(args.Image.ToXml());

      DispatcherHelper.CheckBeginInvokeOnUI(() => Records.Add(record));
    } // OnQueryProgressHandler( sender, args )


    /// <summary>
    /// Handles the model's query completed event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnQueryCompletedHandler(object sender, ClientEventArgs args)
    {
      DispatcherHelper.CheckBeginInvokeOnUI(() => NotActive = true);
    } // OnQueryCompletedHandler( sender, args )


    /// <summary>
    /// Handles the model's query failed event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnQueryFailedHandler(object sender, ClientEventArgs args)
    {
      System.Windows.MessageBox.Show("An error occured performing the query.", "Error");
      Logging.Log.Error(string.Format("An error occured performing the query.  Exception: {0}", args.Message), "PacsQueryViewModel", "OnQueryFailedHandler");

      DispatcherHelper.CheckBeginInvokeOnUI(() => NotActive = true);
    } // OnQueryFailedHandler( sender, args )

    #endregion

  } // class PacsQueryViewModel
} // namespace Viatronix.Utilities.Dicom.PACSViewer.ViewModels
