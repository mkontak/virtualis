using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Data;
using System.Windows.Media;
using System.IO;
using System.Windows;
using System.Windows.Media.Imaging;
using System.Windows.Interop;
using System.Runtime.InteropServices;


namespace Viatronix.UI.Converters
{
  public class BoolToVisibilityConverter : IValueConverter
  {



    #region IValueConverter Members

    public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
    {
      //if (targetType != typeof(string))
      //  throw new InvalidOperationException("The target must be string or derived types");

      if (value != null)
      {

        if (value is bool)
        {
          bool input = (bool)value;

          if (input == true)
            return Visibility.Visible;
          else
            return Visibility.Hidden;
          

        }

      }
      return null;
    }

    public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
    {
      throw new NotImplementedException();
    }

    #endregion
  }
}
