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
using System.Windows.Input;
using System.Globalization;

namespace Viatronix.UI.Converters
{
  public class CursorConverter : IValueConverter
  {

    public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
    {


      Cursor cursor = (value != null && value is bool && (bool)value == true ? Cursors.Wait : Cursors.Arrow );

      return cursor;

    }

    public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
    {
      throw new NotImplementedException();
    }


  }
}
