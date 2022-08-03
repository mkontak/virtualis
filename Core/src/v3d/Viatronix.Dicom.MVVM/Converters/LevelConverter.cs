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
using System.Globalization;

namespace Viatronix.Dicom.MVVM.Converters
{

  public class LevelConverter : DependencyObject, IMultiValueConverter
  {

    public object Convert(object[] values, Type targetType, object parameter, CultureInfo culture)
    {
      int level     = 0;
      int depth     = 1;
      double indent = 1;

      if (values.Length == 3)
      {
        level = (values[0] is int ? (int)values[0] : 0);
        depth = (values[1] is int ? (int)values[1] : 1);
        indent = (values[2] is double ? (double)values[2] : 1);
      }
      else
      {
        depth  = (values[0] is int ? (int)values[0] : 1);
        indent = (values[1] is double ? (double)values[1] : 1);
      }

      return indent * (depth - level);

    }



    public object[] ConvertBack(object value, Type[] targetTypes, object parameter, CultureInfo culture)
    {
      throw new NotImplementedException();
    }

  }

}
