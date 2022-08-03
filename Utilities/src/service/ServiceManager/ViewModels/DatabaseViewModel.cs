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
using Viatronix.Utilities.Service.ServiceManager.Models;

namespace Viatronix.Utilities.Service.ServiceManager.ViewModels
{
  public class DatabaseViewModel : ViewModelBase
  {

    #region fields


    private DatabaseModel m_model = null;



    private RelayCommand m_refreshCommand;
    private RelayCommand m_updateCommand;
    private RelayCommand m_deleteCommand;

    private RelayCommand<Viatronix.Enterprise.Entities.Database> m_selectedDatabaseChanged;

    private RelayCommand<Viatronix.Enterprise.Entities.Table> m_selectedTableChanged;


    /// <summary>
    /// Status message
    /// </summary>
    private string m_statusMessage = string.Empty;

    #endregion

    #region constructors

    /// <summary>
    /// Constructor
    /// </summary>
    public DatabaseViewModel(Viatronix.Enterprise.SDK.Providers.IServiceProvider provider)
    {
      m_model = new DatabaseModel(provider);

      m_model.EntityChanged += new EntityChangedHandler(OnEntityChanged);
      m_model.EntityDeleted += new EntityChangedHandler(OnEntityDeleted);

      m_model.BatchCompleted += new BatchEventHandler(OnBatchCompleted);
      m_model.BatchFailed += new BatchEventHandler(OnBatchFailed);
      m_model.BatchUpdated += new BatchEventHandler(OnBatchUpdated);

      m_model.TableFilled += new EventHandler(OnTableFilled);

      m_model.Refresh();


    }



    #endregion

    #region properties

    /// <summary>
    /// Gets the list of databases retrieved from the system
    /// </summary>
    public List<Viatronix.Enterprise.Entities.Database> Databases
    {
      get { return m_model.Databases; }
      set { RaisePropertyChanged("Databases"); }
    }

    /// <summary>
    /// Gets/Sets the entities as a DataView
    /// </summary>
    public System.Data.DataView Entities
    {
      get { return m_model.DataView; }
      set
      {
      }
    } // Entities


    public bool AllowDelete
    {
      get { return m_model.Table.CanServiceDelete; }
    }

    public bool AllowInsert
    {
      get { return m_model.Table.CanServiceInsert; }
    }

    public bool AllowUpdate
    {
      get { return m_model.Table.CanServiceUpdate; }
    }

    public bool CanDelete
    {
      get { return (AllowDelete && m_model.CanDelete); }
    }

    public bool CanUpdate
    {
      get { return (AllowUpdate && m_model.CanUpdate); }
    }

    public bool CanRefresh
    {
      get { return (m_model.Table != null); }
    }

    /// <summary>
    /// Gets/Sets the status message
    /// </summary>
    public string StatusMessage
    {
      get { return m_statusMessage; }
      set { RaisePropertyChanged<string>("StatusMessage", m_statusMessage, value, false); }
    }

    #endregion

    #region commands

    /// <summary>
    /// Refreshes  the data
    /// </summary>
    public RelayCommand RefreshCommand
    {
      get
      {
        if (m_refreshCommand == null)
          m_refreshCommand = new RelayCommand(() =>
          {
            m_model.Refresh();
          }, () => true);

        return m_refreshCommand;
      }
    } // RefreshCommand


    /// <summary>
    /// Deletes  the data
    /// </summary>
    public RelayCommand DeleteCommand
    {
      get
      {
        if (m_deleteCommand == null)
          m_deleteCommand = new RelayCommand(() =>
          {
            Delete();
          }, () => true);

        return m_deleteCommand;
      }
    } // DeleteCommand

    /// <summary>
    /// Updates  the data
    /// </summary>
    public RelayCommand UpdateCommand
    {
      get
      {
        if (m_updateCommand == null)
          m_updateCommand = new RelayCommand(() =>
          {
            Update();
          }, () => true);

        return m_updateCommand;
      }
    } // UpdateCommand


    /// <summary>
    /// Selected the database
    /// </summary>
    public RelayCommand<Viatronix.Enterprise.Entities.Database> SelectedDatabaseChangedCommand
    {
      get
      {
        if (m_selectedDatabaseChanged == null)
          m_selectedDatabaseChanged = new RelayCommand<Viatronix.Enterprise.Entities.Database>((database) =>
          {
            m_model.Database = database;
          }, (database) => true);

        return m_selectedDatabaseChanged;
      }
    } // SelectedDatabaseChangedCommand

    /// <summary>
    /// Selected the database
    /// </summary>
    public RelayCommand<Viatronix.Enterprise.Entities.Table> SelectedTableChangedCommand
    {
      get
      {
        if (m_selectedTableChanged == null)
          m_selectedTableChanged = new RelayCommand<Viatronix.Enterprise.Entities.Table>((table) =>
          {
            m_model.Table = table;
            RaisePropertyChanged("CanRefresh");
            RaisePropertyChanged("AllowDelete");
            RaisePropertyChanged("AllowUpdate");
            RaisePropertyChanged("AllowInsert");
          }, (database) => true);

        return m_selectedTableChanged;
      }
    } // SelectedDatabaseChangedCommand


    #endregion

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

    #region methods




    private void Update()
    {
      m_model.BatchUpdate();
    }

    private void Delete()
    {
      m_model.BatchDelete();
    }
    #endregion

  } // class DatabaseViewModel
} // namespace Viatronix.Utilities.Service.ServiceManager.ViewModels