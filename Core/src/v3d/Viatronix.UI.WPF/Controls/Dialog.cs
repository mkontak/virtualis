using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using Viatronix.UI.ViewModels;
using GalaSoft.MvvmLight.Messaging;

namespace Viatronix.UI
{
  public class Dialog : Window
  {

    protected override void OnClosed( EventArgs args )
    {
      base.OnClosed( args );

      ViewModelBase viewModel = this.DataContext as ViewModelBase;
      if( viewModel != null )
        viewModel.LocalMessenger.Unregister( this );

      Messenger.Default.Unregister( this );
    }


    public bool? ShowDialog( Window owner )
    {
      this.Owner = owner;
      this.WindowStartupLocation = WindowStartupLocation.CenterOwner;
      this.ResizeMode = ResizeMode.NoResize;
      this.ShowInTaskbar = false;
      return ShowDialog();
    }

  }
}
