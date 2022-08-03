using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Windows.Data;
using GalaSoft.MvvmLight.Command;


namespace Viatronix.UI.ViewModels
{
    /// <summary>
    /// The ViewModel for the application's main window.
    /// </summary>
    abstract public class MainWindowViewModel : WorkspaceViewModel
    {
        #region Fields

        ReadOnlyCollection<CommandViewModel> _commands;
        readonly ObservableCollection<WorkspaceViewModel> _workspaces;
        protected readonly ICollectionView _collectionView;
        WorkspaceViewModel _activeWorkspace;

        #endregion // Fields

        #region Constructor

        public MainWindowViewModel()
        {
            _workspaces = new ObservableCollection<WorkspaceViewModel>();
            _workspaces.CollectionChanged += this.OnWorkspacesChanged;
            _collectionView = CollectionViewSource.GetDefaultView(this.Workspaces);
            List<CommandViewModel> cmds = this.CreateCommands();
            _commands = new ReadOnlyCollection<CommandViewModel>(cmds);
        }

        #endregion // Constructor

        #region Commands

        /// <summary>
        /// Returns a read-only list of commands 
        /// that the UI can display and execute.
        /// </summary>
        public ReadOnlyCollection<CommandViewModel> Commands
        {
            get
            {
                /*if (_commands == null)
                {
                    List<CommandViewModel> cmds = this.CreateCommands();
                    _commands = new ReadOnlyCollection<CommandViewModel>(cmds);
                }*/
                return _commands;
            }
        }

        protected abstract List<CommandViewModel> CreateCommands();

        #endregion // Commands

        #region Workspaces

        /// <summary>
        /// Returns the collection of available workspaces to display.
        /// A 'workspace' is a ViewModel that can request to be closed.
        /// </summary>
        public ObservableCollection<WorkspaceViewModel> Workspaces
        {
            get
            {
                return _workspaces;
            }
        }

        void OnWorkspacesChanged(object sender, NotifyCollectionChangedEventArgs e)
        {
            if (e.NewItems != null && e.NewItems.Count != 0)
                foreach (WorkspaceViewModel workspace in e.NewItems)
                    workspace.RequestClose += this.OnWorkspaceRequestClose;

            if (e.OldItems != null && e.OldItems.Count != 0)
                foreach (WorkspaceViewModel workspace in e.OldItems)
                    workspace.RequestClose -= this.OnWorkspaceRequestClose;
        }

        void OnWorkspaceRequestClose(object sender, EventArgs e)
        {
            WorkspaceViewModel workspace = sender as WorkspaceViewModel;
            workspace.Dispose();
            this.Workspaces.Remove(workspace);
        }

        public WorkspaceViewModel ActiveWorkSpace
        {
            get { return _activeWorkspace; }
            set
            {
                _activeWorkspace = value;
                Debug.Assert(this.Workspaces.Contains(_activeWorkspace));
                if (_collectionView != null)
                    _collectionView.MoveCurrentTo(_activeWorkspace);
                RaisePropertyChanged(() => ActiveWorkSpace);
            }
        }

        #endregion // Workspaces

        #region Private Helpers

        /*protected void SetActiveWorkspace(WorkspaceViewModel workspace)
        {
            ActiveWorkSpace = workspace;
        }*/

        #endregion // Private Helpers
    }
}