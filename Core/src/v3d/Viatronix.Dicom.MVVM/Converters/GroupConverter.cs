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
using Viatronix.Dicom;

namespace Viatronix.Dicom.MVVM.Converters
{
  public class GroupConverter : IValueConverter
  {

 

    #region IValueConverter Members

    public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
    {
      //if (targetType != typeof(string))
      //  throw new InvalidOperationException("The target must be string or derived types");

      if (value != null)
      {

        if (value is string)
        {
          string source = (string)value;

          char[] del = { ',' };

          string[] fields = source.Split(del);

          return fields[0];
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
