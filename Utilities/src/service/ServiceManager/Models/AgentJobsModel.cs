using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;
using System.Threading.Tasks;
using Viatronix.Enterprise.Entities;
using Viatronix;

namespace Viatronix.Utilities.Service.ServiceManager.Models
{
  /// <summary>
  /// Agent Jobs Model
  /// </summary>
  public class AgentJobsModel
  {

    #region fields

    //private DatabaseModel m_databaseModel;

    private System.Data.DataView m_dataView = new System.Data.DataView();

    private System.Data.DataView m_dataViewReadOnly = new DataView();

    /// <summary>
    /// Service provider
    /// </summary>
    private Viatronix.Enterprise.SDK.Providers.IServiceProvider m_serviceProvider = null;

    /// <summary>
    /// List of current jobs
    /// </summary>
    private List<AgentJob> m_jobs = new List<AgentJob>();

    #endregion

    #region constructors

    public AgentJobsModel(Viatronix.Enterprise.SDK.Providers.IServiceProvider provider)
    {

      m_serviceProvider = provider;


      Refresh();

    }

    public AgentJobsModel() { }

    #endregion

    #region properties

    /// <summary>
    /// Gets the list of jobs
    /// </summary>
    public System.Data.DataView DataView
    {
      get { return m_dataView; }
    }
    /// <summary>
    /// Gets the list of jobs
    /// </summary>
    public System.Data.DataView DataViewReadOnly
    {
      get { return m_dataViewReadOnly; }
    }



    #endregion

    #region events

    /// <summary>
    /// Entity Deleted Event
    /// </summary>
    public event EntityChangedHandler EntityChanged;

    /// <summary>
    /// Entity Deleted Event
    /// </summary>
    public event EntityChangedHandler EntityDeleted;



    /// <summary>
    /// Event for a batch update, delete or insert has failed
    /// </summary>
    public event BatchEventHandler BatchFailed;

    /// <summary>
    /// Event for a batch update, delete or insert has completed
    /// </summary>
    public event BatchEventHandler BatchCompleted;

    /// <summary>
    /// Event for a batch update, delete or insert has updated
    /// </summary>
    public event BatchEventHandler BatchUpdated;

    /// <summary>
    /// Table was filled
    /// </summary>
    public event EventHandler TableFilled;

    #endregion

    #region methods

     /// <summary>
    /// Refreshes the jobs from the sevice provider
    /// </summary>
    public void Refresh()
    {
      if ( m_serviceProvider != null )
        m_jobs = m_serviceProvider.GetAgentJobs();

      m_dataView = m_jobs.ToList<IEntity>().ToDataView(false);
      m_dataViewReadOnly = m_jobs.ToList<IEntity>().ToDataView(true);

      if (m_jobs.Count > 0)
      {
        // Setup event handlers
        m_dataView.Table.RowDeleting += new DataRowChangeEventHandler(OnRowDeleting);
        m_dataView.Table.RowChanged += new DataRowChangeEventHandler(OnRowChanged);
      }

      if (TableFilled != null)
        TableFilled(this, EventArgs.Empty);



    } // Rrefresh()

    /// <summary>
    /// Inserts the specified job into the database
    /// </summary>
    /// <param name="job"></param>
    public void Insert(AgentJob job)
    {
      try
      {
        // Validate
        m_serviceProvider.Insert(job);
        Refresh();
      }
      catch (Exception ex)
      {
        Viatronix.Logging.Log.Error(string.Format("Failed to insert agent job {0} : {1}", job.Name, ex.Message), "AgentJobModel", "Insert");
        throw;
      }
    } // Insert(AgentJob job)

    /// <summary>
    /// Inserts the specified job into the database
    /// </summary>
    /// <param name="job"></param>
    public void Delete(AgentJob job)
    {
      try
      {
        // Validate
        m_serviceProvider.Delete(job);
        Refresh();
      }
      catch (Exception ex)
      {
        Viatronix.Logging.Log.Error(string.Format("Failed to delete agent job {0} : {1}", job.Name, ex.Message), "AgentJobModel", "Delete");
        throw;
      }
    } // Delete(AgentJob job)



    #endregion

    #region event handles

    /// <summary>
    /// Handles the row is deleting event
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnRowDeleting(object sender, DataRowChangeEventArgs e)
    {
      // Fires the event
      if (EntityDeleted != null)
        EntityDeleted(this, new EntityChangeEventArgs(((EntityDataRow)(e.Row)).Entity, Actions.Delete));

    } // OnRowDeleting(object sender, DataRowChangeEventArgs e)

    /// <summary>
    /// Handles the row changed event
    /// </summary>
    /// <param name="sender">Sending object</param>
    /// <param name="e">Event arguements</param>
    private void OnRowChanged(object sender, DataRowChangeEventArgs e)
    {
      // Fires the event
      if (EntityChanged != null)
        EntityChanged(this, new EntityChangeEventArgs(((EntityDataRow)(e.Row)).Entity, Actions.Change));

    } // OnRowChangedEvent()

    /// <summary>
    /// Handles the database batch updated event
    /// </summary>
    /// <param name="sender">sending object</param>
    /// <param name="args">Arguements</param>
    private void OnBatchUpdated(object sender, BatchEventArgs args)
    {
      if (BatchUpdated != null)
        BatchUpdated(sender, args);
    } // OnBatchUpdated(object sender, BatchEventArgs args)
    /// <summary>
    /// Handles the database batch failed event
    /// </summary>
    /// <param name="sender">sending object</param>
    /// <param name="args">Arguements</param>
    private void OnBatchFailed(object sender, BatchEventArgs args)
    {
      if (BatchFailed != null)
        BatchFailed(sender, args);
    } // OnBatchFailed(object sender, BatchEventArgs args)

    /// <summary>
    /// Handles the database batch completed event
    /// </summary>
    /// <param name="sender">sending object</param>
    /// <param name="args">Arguements</param>
    private void OnBatchCompleted(object sender, BatchEventArgs args)
    {
      if (BatchCompleted != null)
        BatchCompleted(sender, args);
    } // OnBatchUpdated(object sender, BatchEventArgs args)

    /// <summary>
    /// Handles the table filled event indicating that the DataView is ready
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnTableFilled(object sender, EventArgs e)
    {
      if (TableFilled != null)
        TableFilled(sender, e);
    }

    #endregion

  } // class AgentJobsModel
} // namespace Viatronix.Utilities.Service.ServiceManager.Models
