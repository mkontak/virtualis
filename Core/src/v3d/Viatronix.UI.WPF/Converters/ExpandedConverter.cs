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
using System.Globalization;


namespace Viatronix.UI.Converters
{
  public class ExpandedConverter : IMultiValueConverter
  {


    public object Convert(object[] values, Type targetType, object parameter, CultureInfo culture)
    {

      string expanded = "";

      if (values.Length > 0)
      {
        bool input = (values[0] != null ? (bool)values[0] : false);
        bool hasChildren = ( values[1] != null ? (bool)values[1] : false);


        expanded = ( hasChildren ? (  input ? "-" : "+") : "" );
      }


      return expanded;

    }

    public object [] ConvertBack(object value, Type[] targetType, object parameter, CultureInfo culture)
    {
      throw new NotImplementedException();
    }

  }
}