using System;
using System.Windows.Input;
using GalaSoft.MvvmLight.Command;

namespace Viatronix.UI.ViewModels
{
    /// <summary>
    /// This ViewModelBase subclass requests to be removed 
    /// from the UI when its CloseCommand executes.
    /// This class is abstract.
    /// </summary>
    public abstract class WorkspaceViewModel : ViewModelBase
    {
      #region Fields

      /// <summary>
      /// Close command
      /// </summary>
      RelayCommand m_closeCommand;

      #endregion // Fields

      #region Constructor

        protected WorkspaceViewModel()
        {
        }

        #endregion // Constructor

      #region CloseCommand

      /// <summary>
      /// Returns the command that, when invoked, attempts
      /// to remove this workspace from the user interface.
      /// </summary>
      public ICommand CloseCommand
      {
          get
          {
            if (m_closeCommand == null)
              m_closeCommand = new RelayCommand(() => this.OnRequestClose());
            return m_closeCommand;
          }
      }

      #endregion // CloseCommand

      #region RequestClose [event]

      /// <summary>
      /// Raised when this workspace should be removed from the UI.
      /// </summary>
      public event EventHandler RequestClose;

      void OnRequestClose()
      {
          EventHandler handler = this.RequestClose;
          if (handler != null)
              handler(this, EventArgs.Empty);
      }

      #endregion // RequestClose [event]
   }
}