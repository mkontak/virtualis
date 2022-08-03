using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;
using GalaSoft.MvvmLight.Helpers;

using Viatronix.Enterprise.Entities;
using Viatronix.Utilities.Service.ServiceManager.Models;

namespace Viatronix.Utilities.Service.ServiceManager.ViewModels
{
  public class AgentJobsViewModel : ViewModelBase
  {

    private List<JobFrequency> m_frequencies = new List<JobFrequency>();

    private AgentJobsModel m_model;

    private string m_statusMessage = string.Empty;


    private System.Data.DataView m_dataView = new System.Data.DataView();

    private RelayCommand<System.Data.DataRowView> m_selectedRowChanged;

    private Viatronix.Enterprise.Entities.AgentJob m_selectedJob = new AgentJob();


    public AgentJobsViewModel(Viatronix.Enterprise.SDK.Providers.IServiceProvider provider)
    {
      m_model = new AgentJobsModel(provider);

      m_model.EntityChanged += new EntityChangedHandler(OnEntityChanged);
      m_model.EntityDeleted += new EntityChangedHandler(OnEntityDeleted);

      m_model.BatchCompleted += new BatchEventHandler(OnBatchCompleted);
      m_model.BatchFailed += new BatchEventHandler(OnBatchFailed);
      m_model.BatchUpdated += new BatchEventHandler(OnBatchUpdated);

      m_model.TableFilled += new EventHandler(OnTableFilled);

      m_model.Refresh();


      m_frequencies.Add(JobFrequency.None);
      m_frequencies.Add(JobFrequency.Daily);
      m_frequencies.Add(JobFrequency.Weekly);
      m_frequencies.Add(JobFrequency.Monthly);

    }


    public List<JobFrequency> Frequencies
    { get { return m_frequencies; } }

    public Viatronix.Enterprise.Entities.AgentJob SelectedJob
    {
      get { return m_selectedJob; }
      set
      {
        m_selectedJob = value;
        RaisePropertyChanged("SelectedJob");
      }
    }

    /// <summary>
    /// Jobs list
    /// </summary>
    public System.Data.DataView Jobs
    {
      get 
      {
        return m_model.DataViewReadOnly;
        
      }
      set { }
    }

    /// <summary>
    /// Gets/Sets the status message
    /// </summary>
    public string StatusMessage
    {
      get { return m_statusMessage; }
      set { RaisePropertyChanged<string>("StatusMessage", m_statusMessage, value, false); }
    }

    #region event handles

    /// <summary>
    /// Handles the entity changed event
    /// </summary>
    /// <param name="sender">Object sending event</param>
    /// <param name="args">DataRow change event arguements</param>
    private void OnEntityChanged(object sender, EntityChangeEventArgs args)
    {
      RaisePropertyChanged("CanUpdate");
    } // OnEntityUpdated(object sender, EntityChangeEventArgs args)

    /// <summary>
    /// Handles the entity changed event
    /// </summary>
    /// <param name="sender">Object sending event</param>
    /// <param name="args">DataRow change event arguements</param>
    private void OnEntityDeleted(object sender, EntityChangeEventArgs args)
    {
      RaisePropertyChanged("CanDelete");
    } // OnEntityUpdated(object sender, EntityChangeEventArgs args)

    /// <summary>
    /// Handles the database batch updated event
    /// </summary>
    /// <param name="sender">sending object</param>
    /// <param name="args">Arguements</param>
    private void OnBatchUpdated(object sender, BatchEventArgs args)
    {
      StatusMessage = string.Format("{0} of {1}", args.Action, args.Entity.Tag);
    } // OnBatchUpdated(object sender, BatchEventArgs args)
    /// <summary>
    /// Handles the database batch failed event
    /// </summary>
    /// <param name="sender">sending object</param>
    /// <param name="args">Arguements</param>
    private void OnBatchFailed(object sender, BatchEventArgs args)
    {
      // Message

      switch (args.Action)
      {
        case Actions.Change:
          RaisePropertyChanged("CanUpdate");
          break;
        case Actions.Delete:
          RaisePropertyChanged("CanDelete");
          break;


      };

    } // OnBatchFailed(object sender, BatchEventArgs args)

    /// <summary>
    /// Handles the database batch completed event
    /// </summary>
    /// <param name="sender">sending object</param>
    /// <param name="args">Arguements</param>
    private void OnBatchCompleted(object sender, BatchEventArgs args)
    {
      switch (args.Action)
      {
        case Actions.Change:
          RaisePropertyChanged("CanUpdate");
          break;
        case Actions.Delete:
          RaisePropertyChanged("CanDelete");
          break;


      };

    } // OnBatchUpdated(object sender, BatchEventArgs args)

    /// <summary>
    /// Handles the table filled event indicating that the DataView is ready
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnTableFilled(object sender, EventArgs e)
    {
      RaisePropertyChanged("Entities");
    }

    #endregion


    #region commands


    /// <summary>
    /// Selected the database
    /// </summary>
    public RelayCommand<System.Data.DataRowView> SelectionChangedCommand
    {
      get
      {
        if (m_selectedRowChanged == null)
          m_selectedRowChanged = new RelayCommand<System.Data.DataRowView>((row) =>
          {
            if ( row != null )
              SelectedJob = (AgentJob)((EntityDataRow)row.Row).Entity;
          }, (database) => true);

        return m_selectedRowChanged;
      }
    } // SelectionChanged


    #endregion

  } // class AgentJobsViewModel
} // namespace Viatronix.Utilities.Service.ServiceManager.ViewModels
