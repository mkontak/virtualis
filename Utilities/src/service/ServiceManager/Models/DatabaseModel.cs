using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data;
using System.Threading.Tasks;
using Viatronix.Enterprise.Entities;
using Viatronix;
using Viatronix.Enterprise;

namespace Viatronix.Utilities.Service.ServiceManager.Models
{

  #region enums

  /// <summary>
  /// Actions performed on the entities
  /// </summary>
  public enum Actions 
  {
    None = 0,
    Delete,
    Insert,
    Change,
  };

  public enum BatchStates 
  {
    Completed = 0,
    Failed
  };

  #endregion

  #region class BatchEventArgs

  /// <summary>
  /// Entity change event arguements
  /// </summary>
  public class BatchEventArgs : EventArgs
  {
    private BatchStates m_state = BatchStates.Completed; 
    private bool m_continue = false;
    private IEntity m_entity = null;
    private Actions m_action = Actions.None;
    private string m_message = string.Empty;

    public BatchEventArgs(IEntity entity, Actions action, BatchStates state, string message)
    { m_entity = entity; m_action = action; m_state = state; m_message = message; }

    public Actions Action
    { get { return m_action; } }

    public IEntity Entity
    { get { return m_entity; } }

    public bool Continue
    {
      get { return m_continue; }
      set { m_continue = value; }
    }
    public string Message
    { get { return m_message; } }

    public BatchStates State
    { get { return m_state; } }
    
  } // EntityChangeEventArgs

  #endregion
  
  #region class EntityChangeEventArgs

  /// <summary>
  /// Entity change event arguements
  /// </summary>
  public class EntityChangeEventArgs : EventArgs
  {
    private IEntity m_entity = null;
    private Actions m_action = Actions.None;

    public EntityChangeEventArgs(IEntity entity, Actions action)
    { m_entity = entity; m_action = action; }

    public Actions Action
    { get { return m_action; } }

    public IEntity Entity
    { get { return m_entity; } }
  } // EntityChangeEventArgs

  #endregion
   
  #region delegates

  public delegate void EntityChangedHandler(object sender, EntityChangeEventArgs e);

  public delegate void BatchEventHandler(object sender, BatchEventArgs e);

  #endregion

  #region class DatabaseModel

  /// <summary>
  /// Agent Jobs Model
  /// </summary>
  public class DatabaseModel
  {

    #region struct

    /// <summary>
    /// Change Set Item 
    ///   Contains the entity and action performed
    /// </summary>
    public struct ChangeSetItem
    {
      /// <summary>
      /// Constructor
      /// </summary>
      /// <param name="entity">Entity</param>
      /// <param name="action">Action taken</param>
      public ChangeSetItem(IEntity entity, Actions action)
      { this.Entity = entity; Action = action; }

      /// <summary>
      /// Entity
      /// </summary>
      public Viatronix.Enterprise.Entities.IEntity Entity;

      /// <summary>
      /// Action
      /// </summary>
      public Actions Action;

    };  // struct ChangeSet

    #endregion

    #region fields

    /// <summary>
    /// Service provider
    /// </summary>
    private Viatronix.Enterprise.SDK.Providers.IServiceProvider m_serviceProvider = null;

    /// <summary>
    /// List of current jobs
    /// </summary>
    private List<Database> m_databases = new   List<Database>();

    // DataRow/Entity map based on the current table
    private List<Viatronix.Enterprise.Entities.IEntity> m_entities = new List<Viatronix.Enterprise.Entities.IEntity>();

    /// <summary>
    /// Current DataView
    /// </summary>
    private DataView m_dataView = new DataView();

    /// <summary>
    /// Database
    /// </summary>
    private Database m_database = new Database();

    /// <summary>
    /// Current Table
    /// </summary>
    private Table m_table = new Table();

    // ChangeSet
    private List<ChangeSetItem> m_changeSet = new List<ChangeSetItem>();

    private bool m_buildingDataView = false;

    #endregion

    

    #region constructors

    public DatabaseModel(Viatronix.Enterprise.SDK.Providers.IServiceProvider provider)
    {
      m_serviceProvider = provider;
      m_dataView.Table = new DataTable("Empty");

      Refresh();

    }

    public DatabaseModel() { }

    #endregion

    #region properties

    /// <summary>
    /// Gets the list of database
    /// </summary>
    public List<Database> Databases
    { get { return m_databases; } }


    /// <summary>
    /// Gets/Sets the current table
    /// </summary>
    public Database Database
    {
      get { return m_database; }
      set
      {
        m_database = value;
        m_table = new Viatronix.Enterprise.Entities.Table();
        BuildDataView();
      }
    }

    /// <summary>
    /// Gets/Sets the current table
    /// </summary>
    public Table Table
    {
      get { return m_table; }
      set
      {
        m_table = value;
        BuildDataView();
      }
    }

    /// <summary>
    /// Gets the current data view based on the current table
    /// </summary>
    public DataView DataView
    { get { return m_dataView; } }


    /// <summary>
    /// Gets the set of entities that have been modified
    /// </summary>
    public bool CanDelete
    { 
      get 
      {
        bool canDelete = false;
        foreach (ChangeSetItem item in m_changeSet)
        {
          if (item.Action == Actions.Delete)
          {
            canDelete = true;
            break;
          }
        }
        return canDelete; 
      } 
    }

    /// <summary>
    /// Gets the set of entities that have been modified
    /// </summary>
    public bool CanUpdate
    {
      get
      {
        bool canUpdate = false;
        foreach (ChangeSetItem item in m_changeSet)
        {
          if (item.Action == Actions.Change)
          {
            canUpdate = true;
            break;
          }
        }
        return canUpdate;
      }
    }

    /// <summary>
    /// Gets the set of entities that have been modified
    /// </summary>
    public bool CanInsert
    {
      get
      {
        bool canInsert = false;
        foreach (ChangeSetItem item in m_changeSet)
        {
          if (item.Action == Actions.Insert)
          {
            canInsert = true;
            break;
          }
        }
        return canInsert;
      }
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
    /// Entity Inserted Event
    /// </summary>
    public event EntityChangedHandler EntityInserted;

 
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

    #region event handlers



    /// <summary>
    /// Handles the row is deleting event
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnRowDeleting(object sender, DataRowChangeEventArgs e)
    {
      Viatronix.Enterprise.Entities.IEntity entity = ((EntityDataRow)e.Row).Entity;


      m_changeSet.Add(new ChangeSetItem(entity, Actions.Delete));

      // Fires the event
      if (EntityDeleted != null)
        EntityDeleted(this, new EntityChangeEventArgs(entity, Actions.Delete));

    } // OnRowDeleting(object sender, DataRowChangeEventArgs e)

    /// <summary>
    /// Handles the row changed event
    /// </summary>
    /// <param name="sender">Sending object</param>
    /// <param name="e">Event arguements</param>
    private void OnRowChanged(object sender, DataRowChangeEventArgs e)
    {
      Viatronix.Enterprise.Entities.IEntity entity = ((EntityDataRow)e.Row).Entity;

      foreach (var propInfo in m_table.EntityType.GetProperties())
      {

        // ==================================================
        // Ignore the Tag property from the entity objects
        // ==================================================
        if (propInfo.Name != "Tag")
        {

          // Get the cell value
          object value = e.Row[propInfo.Name];

          // =================================================================================
          // If the modified value is not null and the value is different then what is in the 
          // entity object then change the entity objects corresponding property and add to the 
          // modified list.
          // =========================================================================================
          if (value != null && !value.Equals(propInfo.GetValue(entity, null)))
          {
            propInfo.SetValue(entity, value);
            m_changeSet.Add(new ChangeSetItem(entity, Actions.Change));
          }

        } // END .. If the property is not the Tag property

      } // END ... For each property

 
      // Fires the event
      if (EntityChanged != null)
        EntityChanged(this, new EntityChangeEventArgs(entity, Actions.Change));

    } // OnRowChangedEvent()

    /// <summary>
    /// Handles the row changed event
    /// </summary>
    /// <param name="sender">Sending object</param>
    /// <param name="e">Event arguements</param>
    private void OnRowInserted(object sender, System.Data.DataTableNewRowEventArgs e)
    {
      if (m_buildingDataView)
        return;

      Viatronix.Enterprise.Entities.IEntity entity = (Viatronix.Enterprise.Entities.IEntity )Activator.CreateInstance(m_table.EntityType);


      foreach (var propInfo in m_table.EntityType.GetProperties())
      {
        // Get the cell value
        object value = e.Row[propInfo.Name];

        // =================================================================================
        // If the modified value is not null and the value is different then what is in the 
        // entity object then change the entity objects corresponding property and add to the 
        // modified list.
        // =========================================================================================
        if (value != null && !value.Equals(propInfo.GetValue(entity, null)))
        {
          propInfo.SetValue(entity, value);
        }
      }

      m_changeSet.Add(new ChangeSetItem(entity, Actions.Insert));


      // Fires the event
      if (EntityChanged != null)
        EntityChanged(this, new EntityChangeEventArgs(entity, Actions.Insert));

    } // OnRowChangedEvent()

    #endregion


    #region methods

    /// <summary>
    /// Refreshes the jobs from the sevice provider
    /// </summary>
    public void Refresh()
    {
      if (m_serviceProvider != null)
      {
        if ( m_databases.Count == 0 )
          m_databases = m_serviceProvider.GetDatabases();

        BuildDataView();
      }

    } // Rrefresh()

   
    /// <summary>
    /// Builds the DataView object agains the current table
    /// </summary>
    private void BuildDataView()
    {

      try
      {
        ///Clear the delete amd change sets
        m_changeSet.Clear();

        m_entities = GetEntities(m_table);

        m_dataView = m_entities.ToDataView(false);

        if (m_dataView.Table != null)
        {
          // Setup event handlers
          m_dataView.Table.RowDeleting += new DataRowChangeEventHandler(OnRowDeleting);
          m_dataView.Table.TableNewRow += new DataTableNewRowEventHandler(OnRowInserted);
          m_dataView.Table.RowChanged += new DataRowChangeEventHandler(OnRowChanged);
        }

      }
      finally
      {
        m_buildingDataView = false;

        if (TableFilled != null)
          TableFilled(this, EventArgs.Empty);


      }
    }   // BuildDataView()

    /// <summary>
    /// Inserts the specified job into the database
    /// </summary>
    /// <param name="job"></param>
    public List<IEntity> GetEntities(Table table)
    {
      List<IEntity> entities = new List<IEntity>();

      
      try
      {
        if ( table != null )
          entities = m_serviceProvider.GetTable(table);

        return entities;
      }
      catch (Exception ex)
      {
        Viatronix.Logging.Log.Error(string.Format("Failed to retrieve table entities {0} : {1}", table.Name, ex.Message), "DatabaseModel", "GetEntities");
        throw;
      }
    } // GetEntities(Database job)

    /// <summary>
    /// Inserts the specified job into the database
    /// </summary>
    /// <param name="job"></param>
    public void Delete(IEntity entity)
    {
      try
      {
      }
      catch (Exception ex)
      {
        Viatronix.Logging.Log.Error(string.Format("Failed to delete entity {0} : {1}", entity.Tag, ex.Message), "DatabaseModel", "Delete");
        throw;
      }
    } // Delete(Database job)

    /// <summary>
    /// Inserts the specified job into the database
    /// </summary>
    /// <param name="job"></param>
    public void Insert(IEntity entity)
    {
      try
      {
      }
      catch (Exception ex)
      {
        Viatronix.Logging.Log.Error(string.Format("Failed to insert entity {0} : {1}", entity.Tag, ex.Message), "DatabaseModel", "Delete");
        throw;
      }
    } // Delete(Database job)

 
    /// <summary>
    /// Performss the batch update
    /// </summary>
    public void BatchUpdate()
    {
      List<ChangeSetItem> items = new List<ChangeSetItem>();

      try
      {
        foreach (ChangeSetItem item in m_changeSet)
        {
          if (item.Action == Actions.Change)
          {

            //m_serviceProvider.Update(item.Entity);

            items.Add(item);

            if (BatchUpdated != null)
              BatchUpdated(this, new BatchEventArgs(item.Entity, Actions.Change, BatchStates.Completed, string.Empty));
          }
        }


        if (BatchCompleted != null)
          BatchCompleted(this, new BatchEventArgs(null, Actions.Change, BatchStates.Completed, string.Empty));

      }
      catch (Exception ex)
      {
        Viatronix.Logging.Log.Error("Failed to update entity : " + ex.Message, "DatabaseModel", "BatchUpdate");

        if (BatchFailed != null)
          BatchFailed(this, new BatchEventArgs(null, Actions.Change, BatchStates.Failed, ex.Message));

      }
      finally
      {
        foreach (ChangeSetItem item in items)
        {
          m_changeSet.Remove(item);
        }

        Refresh();

      }
    }

    /// <summary>
    /// Performs the batch delete
    /// </summary>
    public void BatchDelete()
    {
      List<ChangeSetItem> items = new List<ChangeSetItem>();

      try
      {
        foreach (ChangeSetItem item in m_changeSet)
        {
          if (item.Action == Actions.Delete)
          {

            //m_serviceProvider.Delete(item.Entity);

            items.Add(item);

            if (BatchUpdated != null)
              BatchUpdated(this, new BatchEventArgs(item.Entity, Actions.Delete, BatchStates.Completed, string.Empty));
          }
        }


        if (BatchCompleted != null)
          BatchCompleted(this, new BatchEventArgs(null, Actions.Delete, BatchStates.Completed, string.Empty));

      }
      catch (Exception ex)
      {
        Viatronix.Logging.Log.Error("Failed to delete entity : " + ex.Message, "DatabaseModel", "BatchDelete");


        if (BatchFailed != null)
          BatchFailed(this, new BatchEventArgs(null, Actions.Delete, BatchStates.Failed, ex.Message));

      }
      finally
      {
        foreach (ChangeSetItem item in items)
        {
          m_changeSet.Remove(item);
        }

        Refresh();

      }
    }

    /// <summary>
    /// Perfoms the batch insert
    /// </summary>
    public void BatchInsert()
    {
      List<ChangeSetItem> items = new List<ChangeSetItem>();

      try
      {
        foreach (ChangeSetItem item in m_changeSet)
        {
          if (item.Action == Actions.Insert)
          {

            //m_serviceProvider.Insert(item.Entity);

            items.Add(item);

            if (BatchUpdated != null)
              BatchUpdated(this, new BatchEventArgs(item.Entity, Actions.Insert, BatchStates.Completed, string.Empty));
          }
        }


        if (BatchCompleted != null)
          BatchCompleted(this, new BatchEventArgs(null, Actions.Insert, BatchStates.Completed, string.Empty));

      }
      catch (Exception ex)
      {
        Viatronix.Logging.Log.Error("Failed to insert entity : " + ex.Message, "DatabaseModel", "BatchInsert");


        if (BatchFailed != null)
          BatchFailed(this, new BatchEventArgs(null, Actions.Insert, BatchStates.Failed, ex.Message));

      }
      finally
      {
        foreach (ChangeSetItem item in items)
        {
          m_changeSet.Remove(item);
        }
        Refresh();

      }

    }

    #endregion


  }; // class DatabaseModel

  #endregion

} // namespace Viatronix.Utilities.Service.ServiceManager.Models
