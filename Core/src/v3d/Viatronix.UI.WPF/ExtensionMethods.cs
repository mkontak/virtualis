using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using GalaSoft.MvvmLight;

namespace Viatronix.UI
{
  public static class ExtensionMethods
  {





    public static Visibility ToVisibleOrHidden( this bool value )
    {
      return ( value ) ? Visibility.Visible : Visibility.Hidden;
    }


    public static Visibility ToVisibleOrCollapsed( this bool value )
    {
      return ( value ) ? Visibility.Visible : Visibility.Collapsed;
    }

  }
}
