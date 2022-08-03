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

namespace DicomViewer.Converters
{

  public class IsViewingImageToMultiImageVisibilityConverter : IValueConverter
  {




    public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
    {
      return ( value != null ? ( ((bool)value) == true ? Visibility.Hidden : Visibility.Visible ) : Visibility.Visible);
    }


    public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
    {
      throw new NotImplementedException();
    }

  }

}
