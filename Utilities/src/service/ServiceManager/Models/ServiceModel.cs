using System;
using System.Collections.ObjectModel;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;
using System.Threading.Tasks;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;
using GalaSoft.MvvmLight.Helpers;
using System.Windows.Controls;
using Viatronix.Enterprise.SDK.Providers;
using Viatronix.Enterprise;
using Viatronix;


namespace Viatronix.Utilities.Service.ServiceManager.Models
{

  public class ServiceModel 
  {

    #region fields

    
    private DatabaseModel m_databaseModel = null;

    private AgentJobsModel m_agentModel = null;


    /// <summary>
    /// Providers
    /// </summary>
    private List<IProvider> m_providers = new List<IProvider>();

    //
    private Viatronix.Enterprise.SDK.Providers.IServiceProvider m_serviceProvider = null;


    //
    private Viatronix.Enterprise.SDK.Providers.ISecurityProvider m_securityProvider = null;

    /// <summary>
    /// Status message
    /// </summary>
    private string m_statusMessage = string.Empty;

    #endregion

    #region constructors

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="securityProvider">The service provider.  Assume that the login has already occurred.</param>
    /// <param name="serviceProvider">The service provider.</param>
    public ServiceModel(ISecurityProvider securityProvider, Viatronix.Enterprise.SDK.Providers.IServiceProvider serviceProvider)
    {
      m_securityProvider = securityProvider;
      m_serviceProvider = serviceProvider;

      m_databaseModel = new DatabaseModel(m_serviceProvider);

      m_databaseModel.BatchCompleted += new BatchEventHandler(OnDatabaseBatchCompleted);
      m_databaseModel.BatchFailed += new BatchEventHandler(OnDatabaseBatchFailed);
      m_databaseModel.BatchUpdated += new BatchEventHandler(OnDatabaseBatchUpdated);

      m_databaseModel.EntityDeleted += new EntityChangedHandler(OnEntityDeleted);
      m_databaseModel.EntityChanged += new EntityChangedHandler(OnEntityChanged);

      m_databaseModel.TableFilled += new EventHandler(OnTableFilled);

      m_agentModel = new AgentJobsModel(m_serviceProvider);
    } // ServiceModel()

    #endregion

    #region events

    /// <summary>
    /// Entity Deleted Event
    /// </summary>
    public event EntityChangedHandler DatabaseEntityDeleted;

     /// <summary>
    /// Entity Changed Event
    /// </summary>
    public event EntityChangedHandler DatabaseEntityChanged;


    /// <summary>
    /// Database batch process completed 
    /// </summary>
    public event BatchEventHandler DatabaseBatchCompleted;

    /// <summary>
    /// Database batch process failed event
    /// </summary>
    public event BatchEventHandler DatabaseBatchFailed;


    /// <summary>
    /// Database was batch process update
    /// </summary>
    public event BatchEventHandler DatabaseBatchUpdated;

    /// <summary>
    /// Database table filled event
    /// </summary>
    public event EventHandler DatabaseTableFilled;

    #endregion

    #region properties

    /// <summary>
    /// Gets the service provider
    /// </summary>
    public Viatronix.Enterprise.SDK.Providers.IServiceProvider ServiceProvider
    { get { return m_serviceProvider; } }

     #endregion

    #region event handlers

    private void OnTableFilled(object sender, EventArgs e)
    { if (DatabaseTableFilled != null) DatabaseTableFilled(sender, e); }

    private void OnDatabaseBatchCompleted(object sender, BatchEventArgs e)
    { if (DatabaseBatchCompleted != null) DatabaseBatchCompleted(sender, e); }

    private void OnDatabaseBatchFailed(object sender, BatchEventArgs e)
    { if (DatabaseBatchFailed != null) DatabaseBatchFailed(sender, e); }

    private void OnDatabaseBatchUpdated(object sender, BatchEventArgs e)
    { if (DatabaseBatchUpdated != null) DatabaseBatchUpdated(sender, e); }

    private void OnEntityChanged(object sender, EntityChangeEventArgs e)
    { if (DatabaseEntityChanged != null) DatabaseEntityChanged(sender, e); }

    private void OnEntityDeleted(object sender, EntityChangeEventArgs e)
    { if (DatabaseEntityDeleted != null) DatabaseEntityDeleted(sender, e); }

    #endregion

    #region mnethods

    /// <summary>
    /// Refreshes the database 
    /// </summary>
    public void Refresh()
    {
      m_databaseModel.Refresh();
      m_agentModel.Refresh();
    }


    public void BatchUpdate()
    {
      m_databaseModel.BatchUpdate();
    }

    public void BatchDelete()
    {
      m_databaseModel.BatchDelete();
    }

    #endregion

    #region methods

    #endregion

  } // class ServiceModel
} // namespace Viatronix.Utilities.Service.ServiceManager.Models
