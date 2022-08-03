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
using System.Windows.Controls;
using System.Globalization;

namespace Viatronix.UI.Converters
{

  public class ListViewItemIndexConverter : IValueConverter
  {
    public object Convert(object value, Type TargetType, object parameter, CultureInfo culture)
    {

      ListViewItem item = (ListViewItem)value;

      ListView listView = ItemsControl.ItemsControlFromItemContainer(item) as ListView;


      int index = listView.ItemContainerGenerator.IndexFromContainer(item) + (parameter != null ? System.Convert.ToInt32(parameter) : 0);

      return index.ToString();

    }

    public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
    {
      throw new NotImplementedException();
    }
  }
}
